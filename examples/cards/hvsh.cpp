#include <vector>
#include <map>
#include <sstream>
#include <iomanip>
#include "utility.h"
#include "Card.h"
#include "CardGroup.h"
#include "Scopa.h"




int main (int argc, char const* argv[])
{
    std::pair<SDL_Window*,SDL_Renderer*> winRen = startSDL("Cards", Scopa::SCREEN_X, Scopa::SCREEN_Y, Scopa::SCREEN_WIDTH, Scopa::SCREEN_HEIGHT);
    SDL_Window* win = winRen.first;
    SDL_Renderer* ren = winRen.second;
    
    Scopa game = Scopa(ren);
    
    //// START TEST
    
    game.startMatch();
    game.setFocus(game.handPlayer);
    game.renderAll();
    
    SDL_Event inEvent;
    SDL_KeyboardEvent kEv;
    
    enum Mode { PLAY, CAPTURE };
    Mode currentMode = PLAY;
    Scopa::Who who = Scopa::PLAYER;
    game.setFocus(who);
    
    std::vector<Scopa::captureType> tableCaptures = game.generateTableCaptures();
    std::vector<Scopa::captureType> currentCaptures;
    std::vector<Scopa::captureType>::iterator captureIt;
    
    int mainCycleNum = 0;
    while( 1 ){
        SDL_WaitEvent( &inEvent );
        kEv = inEvent.key;
        
        std::cout << "Cycle number: " << mainCycleNum << std::endl;
        std::cout << "  mode: " << currentMode << "; who: " << who << std::endl;
        mainCycleNum++;
        
        switch(currentMode) {
            case PLAY:
                if(kEv.type == SDL_KEYUP) {
                    switch(kEv.keysym.sym) {
                        case SDLK_UP:
                            game.moveFocus(1);
                        break;
                        case SDLK_DOWN:
                            game.moveFocus(-1);
                        break;
                        case SDLK_RETURN:
                            if(game.hasMultipleChoice(game.focus->second,tableCaptures)) {
                                currentMode = CAPTURE;
                                currentCaptures = game.filterByValue(tableCaptures,game.focus->second.value);
                                captureIt = currentCaptures.begin();
                                game.addCursor(game.table,captureIt->second);
                            }
                            else {
                                game.playOnFocus(who);
                                tableCaptures = game.generateTableCaptures();
                                who = who == Scopa::PLAYER? Scopa::ENEMY : Scopa::PLAYER;
                                game.setFocus(who);
                            }
                        break;
                        case SDLK_ESCAPE:
                            stopSDL(win,ren);
                            return 0;
                        break;
                        default:
                        break;
                    }
        
                }
            break;
            case CAPTURE:
                if(kEv.type == SDL_KEYUP) {
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
                            game.playOnFocus(who,captureIt->second);
                            tableCaptures = game.generateTableCaptures();
                            who = who == Scopa::PLAYER? Scopa::ENEMY : Scopa::PLAYER;
                            game.resetCursors();
                            game.setFocus(who);
                            currentMode = PLAY;
                        break;
                        case SDLK_ESCAPE:
                            stopSDL(win,ren);
                            return 0;
                        break;
                        default:
                        break;
                    }
                }
            break;
        }
        
        
        if(game.handPlayer.cards.size() == 0 && game.handEnemy.cards.size() == 0) {
            if(game.deck.cards.size() == 0) { break; }
            game.dealHand(Scopa::PLAYER);
            game.dealHand(Scopa::ENEMY);
            game.setFocus(who);
        }
        
        game.renderAll();
        SDL_Delay(100);
    }
    
    std::cout << "Partita finita!" << std::endl;
    std::pair<int,int> score = game.evaluateScore();
    std::cout << "Player: " << score.first << std::endl;
    std::cout << "Enemy: " << score.second << std::endl;
    
    //// END TEST
    
    
    stopSDL(win, ren);
    
    return 0;
}