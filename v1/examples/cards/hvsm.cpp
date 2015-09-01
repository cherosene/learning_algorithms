#include <vector>
#include <map>
#include <sstream>
#include <iomanip>
#include "utility.h"
#include "Card.h"
#include "CardGroup.h"
#include "Scopa.h"
#include "scopaQLUtility.h"




int main (int argc, char const* argv[])
{
    srand(clock());
    
    // log function
    std::function<void(const char*)> log = [](const char* msg){
        std::cout << msg << std::endl;
    };
    
    // initialize graphic
    std::pair<SDL_Window*,SDL_Renderer*> winRen = startSDL("Cards", Scopa::SCREEN_X, Scopa::SCREEN_Y, Scopa::SCREEN_WIDTH, Scopa::SCREEN_HEIGHT);
    SDL_Window* win = winRen.first;
    SDL_Renderer* ren = winRen.second;
    
    
    // create the game and initialize some useful variables
    log("Creating the game...");
    Scopa game = Scopa(ren);
    Scopa::Who curTurn = Scopa::PLAYER;
    bool multipleChoice = false;
    Card curCard = Card(Card::VOID,0,nullptr,nullptr);
    
    SDL_Event inEvent;
    SDL_KeyboardEvent kEv;
    enum Mode { PLAY, CAPTURE };
    Mode currentMode = PLAY;
    std::vector<Scopa::captureType> tableCaptures = game.generateTableCaptures();
    std::vector<Scopa::captureType> currentCaptures;
    std::vector<Scopa::captureType>::iterator captureIt;
    
    
    // FIXME: find a simple way to include playSmart in scopaQLUtility.h
    // define function to play cards
    log("Defining moves...");
    std::function<void(int)> playSmart = [&](int n) {
        CardGroup *hand = nullptr, *capturedPile = nullptr;
        int *scopaPoints = nullptr, *points = nullptr;
        game.sideOfTheTable(curTurn,hand,capturedPile,points,scopaPoints);
        curCard = *(hand->cards.begin() + n);
        std::vector<Scopa::captureType> curCaptures = game.generateTableCaptures();
        if(game.hasMultipleChoice(curCard,curCaptures)) {
            multipleChoice = true;
        }
        else {
            multipleChoice = false;
            game.playCard(curTurn,curCard);
        }
    };
    
    std::map<Action,std::function<void()>> atm;
    atm[PLAY1] = [curTurn,&game,&multipleChoice,&curCard,playSmart](){ playSmart(0); };
    atm[PLAY2] = [curTurn,&game,&multipleChoice,&curCard,playSmart](){ playSmart(1); };
    atm[PLAY3] = [curTurn,&game,&multipleChoice,&curCard,playSmart](){ playSmart(2); };
    
    
    // define players
    log("Defining enemy...");
    QLGameObject<Action,stateAlias> enemy = QLGameObject<Action,stateAlias>(atm,validActFun);
    enemy.setQlParameters(LEARNING_RATE,DISCOUNT_FACTOR);
    loadQLTable(enemy,ENEMY_QLTABLE_FILENAME);
    
    while(1) {
    
        // starting the match
        game.startMatch();
        game.setFocus(curTurn);
    
        game.renderAll();
    
        while(!game.matchHasEnded()) {
            // DEBUG
            log("begin of a new turn...");
            bool isKeyEvent = false;
            log( std::to_string(game.pointsPlayer).c_str() );
            log( std::to_string(game.pointsEnemy).c_str() );
        
            switch(curTurn){
                case Scopa::PLAYER:
                    // DEBUG
                    log("Current turn: PLAYER");
                    // human play
                    isKeyEvent = false;
                    while(!isKeyEvent) {
                        SDL_WaitEvent( &inEvent );
                        std::cout << inEvent.type << std::endl;
                        isKeyEvent = inEvent.type == SDL_KEYUP;
                    }
                    kEv = inEvent.key;
                    // DEBUG
                    log("Key red");
                
                    switch(currentMode) {
                        case PLAY:
                            switch(kEv.keysym.sym) {
                                case SDLK_UP:
                                    game.moveFocus(1);
                                break;
                                case SDLK_DOWN:
                                    game.moveFocus(-1);
                                break;
                                case SDLK_RETURN:
                                    if(game.hasMultipleChoice(game.focus->second,tableCaptures)) {
                                        //DEBUG
                                        log("multiple choice");
                                    
                                        currentMode = CAPTURE;
                                        currentCaptures = game.filterByValue(tableCaptures,game.focus->second.value);
                                        captureIt = currentCaptures.begin();
                                        game.addCursor(game.table,captureIt->second);
                                    }
                                    else {
                                        //DEBUG
                                        log("single choice");
                                    
                                        game.playOnFocus(curTurn);
                                        tableCaptures = game.generateTableCaptures();
                                        curTurn = Scopa::ENEMY;
                                    }
                                break;
                                case SDLK_ESCAPE:
                                    stopSDL(win,ren);
                                    return 0;
                                break;
                                default:
                                break;
                            }
                        break;
                        case CAPTURE:
                            switch(kEv.keysym.sym) {
                                case SDLK_UP:
                                    if(captureIt == currentCaptures.begin()) { captureIt = currentCaptures.end()-1; }
                                    else { captureIt--; }
                                    game.resetCursors();
                                    game.addCursor(game.table,captureIt->second);
                                break;
                                case SDLK_DOWN:
                                    if(captureIt == currentCaptures.end()-1) { captureIt = currentCaptures.begin(); }
                                    else { captureIt++; }
                                    game.resetCursors();
                                    game.addCursor(game.table,captureIt->second);
                                break;
                                case SDLK_RETURN:
                                    game.playOnFocus(curTurn,captureIt->second);
                                    tableCaptures = game.generateTableCaptures();
                                    curTurn = Scopa::ENEMY;
                                    game.resetCursors();
                                    currentMode = PLAY;
                                break;
                                case SDLK_ESCAPE:
                                    stopSDL(win,ren);
                                    return 0;
                                break;
                                default:
                                break;
                            }
                        break;
                    }
                break;
                case Scopa::ENEMY:
                    // DEBUG
                    log("Current turn: ENEMY");
                    // choose the best subtable as the new state
                    cardVec best;
                    if(game.table.cards.size()<=4) { best = game.table.cards; }
                    else { best = bestSubtable(&enemy,game.handEnemy.cards,game.table.cards); }
                    stateAlias sa = stateAlias(cardVecToInt(game.handEnemy.cards),cardVecToInt(best));
                    enemy.qlUpdate(sa);
            
                    // do the best action; if more capture are possible, choose the one which gives the best future results
                    enemy.doAction();
            
                    if(multipleChoice == true) {
                        std::vector<Scopa::captureType> captures = game.generateTableCaptures();
                        int bestValue = 0;
                        std::vector<Card> bestCapture;
                        for(std::vector<Scopa::captureType>::iterator captureIt = captures.begin(); captureIt != captures.end(); captureIt++) {
                            if(curCard.value == captureIt->first) {
                                int tmpValue = game.evalPlay(captureIt->second);
                                if(tmpValue > bestValue) {
                                    bestValue = tmpValue;
                                    bestCapture = captureIt->second;
                                }
                            }
                        }
                        game.playCard(curTurn,curCard,bestCapture);
                    }
                
                    if(game.handPlayer.cards.size() != 0) { game.setFocus(Scopa::PLAYER); }
                    curTurn = Scopa::PLAYER;
                break;
            }
        
            if(game.matchHasEnded()) {
                log("Game has ended!");
                game.evaluateScore();
            }
            else if(game.handPlayer.cards.size() == 0 && game.handEnemy.cards.size() == 0) {
                game.dealHand(Scopa::PLAYER);
                game.dealHand(Scopa::ENEMY);
                game.setFocus(Scopa::PLAYER);
            }
        
            game.renderAll();
            SDL_Delay(100);
        
        }
    
        log( std::to_string(game.pointsPlayer).c_str() );
        log( std::to_string(game.pointsEnemy).c_str() );
        
        game.pointsPlayer = 0;
        game.pointsEnemy  = 0;
    
    }
    
    return 0;
}