#include "utility.h"
#include "Scopa.h"
#include "scopaStates.h"



const int MATCH_COUNTER_STEP = 10000;
const int BACKUP_SAVE_STEP = 10000000;

const char* PLAYER_QLTABLE_FILENAME = "qlTablePlayer.qlt";
const char* ENEMY_QLTABLE_FILENAME  = "qlTableEnemy.qlt";

const char* PLAYER_QLTABLE_BACKUP_FILENAME = "qlTablePlayer_backup.qlt";
const char* ENEMY_QLTABLE_BACKUP_FILENAME  = "qlTableEnemy_backup.qlt";

const float REWARD_WIN  = 0.2;
const float REWARD_LOSE = 0.2;


typedef std::vector<Card> cardVec;
typedef std::pair<unsigned int,unsigned long int> stateAlias;

enum Action { PLAY1 , PLAY2, PLAY3 };


int cardToInt(Card card) { return (int)card.suit * 10 + card.value; }
Card intToCard(int n) { return Card( (Card::Suit)((n-1)/10), (n+1)%10, nullptr, nullptr ); }

int cardVecToInt(cardVec cv) {
    int result = 0;
    for(cardVec::iterator it = cv.begin(); it != cv.end(); it++) {
        result += cardToInt(*it);
        result *= 21;
    }
    result /= 21;
    return result;
}
cardVec intToCardVec(int n) {
    cardVec result;
    while(n != 0) {
        int r = n%21;
        result.push_back( intToCard(r) );
        n -= r;
        n /= 21;
    }
    std::sort(result.begin(),result.end());
    return result;
}

void saveQLTable(QLGameObject<Action,stateAlias>& go, const char* filename) {
    std::ofstream outputFile;
    outputFile.open(filename);
    for(QLGameObject<Action,stateAlias>::qlTableType::iterator it = go.qlTable.begin(); it != go.qlTable.end(); it++) {
        outputFile.write((char *)&(it->first.first),sizeof(Action));
        outputFile.write((char *)&(it->first.second.first),sizeof(unsigned int));
        outputFile.write((char *)&(it->first.second.second),sizeof(unsigned long int));
        outputFile.write((char *)&(it->second),sizeof(float));
    }
    outputFile.close();
}

void loadQLTable(QLGameObject<Action,stateAlias>& go, std::ifstream& inputFile) {
    Action act;
    int stateFirst, stateSecond;
    float value;
    while(inputFile.good()) {
        inputFile.read((char *)&act,sizeof(Action));
        if(!inputFile.good()) { break; }
        inputFile.read((char *)&stateFirst,sizeof(unsigned int));
        inputFile.read((char *)&stateSecond,sizeof(unsigned long int));
        inputFile.read((char *)&value,sizeof(float));
        stateAlias nstate = stateAlias(stateFirst,stateSecond);
        go.qlTable[std::pair<Action,stateAlias>(act,nstate)] = value;
    }
}




int main (int argc, char const* argv[])
{
    // log function
    std::function<void(const char*)> log = [](const char* msg){
        std::cout << msg << std::endl;
    };
    
    if(argc<2) {
        // FIXME: throw exception
        log("No input argument.");
        exit(-1);
    }
    
    // create the game and initialize some useful variables
    log("Creating the game...");
    Scopa game = Scopa(nullptr);
    Scopa::Who curTurn = Scopa::PLAYER;
    bool multipleChoice = false;
    Card curCard = Card(Card::VOID,0,nullptr,nullptr);
    int playerLastScore = 0, enemyLastScore = 0;
    
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
    std::function<std::vector<Action>(stateAlias)> validActFun = [](stateAlias state){
        std::vector<Action> result;
        std::vector<Card> tmpVec = intToCardVec(state.first);
        result.push_back(PLAY1);
        if(tmpVec.size() >= 2) { result.push_back(PLAY2); }
        if(tmpVec.size() >= 3) { result.push_back(PLAY3); }
        return result;
    };
    
    log("Defining players...");
    log("  Defining first player...");
    QLGameObject<Action,stateAlias> player = QLGameObject<Action,stateAlias>(atm,validActFun);
    player.setQlParameters(0.1,0.2);
    std::ifstream inPlayer(PLAYER_QLTABLE_FILENAME);
    if (inPlayer) {
        log("   Loading player qlTable...");
        loadQLTable(player,inPlayer);
    }
    inPlayer.close();
    
    
    log("  Defining second player...");
    QLGameObject<Action,stateAlias> enemy  = QLGameObject<Action,stateAlias>(atm,validActFun);
    enemy.setQlParameters(0.1,0.2);
    std::ifstream inEnemy(ENEMY_QLTABLE_FILENAME);
    if (inEnemy) {
        log("   Loading enemy qlTable...");
        loadQLTable(enemy,inEnemy);
    }
    inEnemy.close();


    
    // main loop
    log("Starting the simulation...");
    int matchTotalNum = atoi(argv[1]);
    for(int matchCounter = 0; matchCounter < matchTotalNum; ++matchCounter) {
        if(matchCounter % MATCH_COUNTER_STEP == 0) { log( ("Match number " + std::to_string(matchCounter) + "...").c_str() ); }
        if(matchCounter % BACKUP_SAVE_STEP == 0) {
            log("Saving backup...");
            saveQLTable(player,PLAYER_QLTABLE_BACKUP_FILENAME);
            saveQLTable(enemy,ENEMY_QLTABLE_BACKUP_FILENAME);
            log("Backup done!");
        }
        
        game.startMatch();
        
        while(!game.matchHasEnded()) {
            
            QLGameObject<Action,stateAlias> *curPlayer, *otherPlayer;
            int *curLastScore, *otherLastScore, *curScore, *otherScore;
            switch(curTurn) {
                case Scopa::PLAYER:
                    curPlayer = &player;
                    otherPlayer = &enemy;
                    curScore = &game.pointsPlayer;
                    curLastScore = &playerLastScore;
                    otherScore = &game.pointsEnemy;
                    otherLastScore = &enemyLastScore;
                    break;
                case Scopa::ENEMY:
                    curPlayer = &enemy;
                    otherPlayer = &player;
                    curScore = &game.pointsEnemy;
                    curLastScore = &enemyLastScore;
                    otherScore = &game.pointsPlayer;
                    otherLastScore = &playerLastScore;
                    break;
            }
            CardGroup *hand, *capturePile;
            // FIXME: this pointer has no meaning
            int* trash;
            game.sideOfTheTable(curTurn,hand,capturePile,trash,trash);
            
            // do the best action; if more capture are possible, choose the one which gives the best future results
            curPlayer->doAction();
            
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
            
            // calculate reward and update
            if(game.matchHasEnded()) {
                game.evaluateScore();
            }
            else if(game.handPlayer.cards.size() == 0 && game.handEnemy.cards.size() == 0) {
                game.dealHand(Scopa::PLAYER);
                game.dealHand(Scopa::ENEMY);
            }
            float reward = (otherScore - otherLastScore) * REWARD_WIN - (curScore - curLastScore) * REWARD_LOSE;
            otherLastScore = otherScore;
            otherPlayer->qlUpdate(stateAlias(cardVecToInt(hand->cards),cardVecToInt(game.table.cards)),reward);
            
            curTurn = curTurn == Scopa::PLAYER? Scopa::ENEMY : Scopa::PLAYER;
        }
        
    }
    
    log("Simulation ended!");
    log("Saving qlTables...");
    
    // save qlTables
    saveQLTable(player,PLAYER_QLTABLE_FILENAME);
    saveQLTable(enemy,ENEMY_QLTABLE_FILENAME);
    
    log("All done!");
    
    return 0;
}