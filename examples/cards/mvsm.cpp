#include "utility.h"
#include "Scopa.h"
#include "scopaStates.h"


const char* PLAYER_QLTABLE_FILENAME = "qlTablePlayer.qlt";
const char* ENEMY_QLTABLE_FILENAME  = "qlTableEnemy.qlt";

const float REWARD_WIN  = 0.2;
const float REWARD_LOSE = 0.2;


typedef std::vector<Card> cardVec;
typedef std::pair<int,int> stateAlias;

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

void saveQLTable(QLGameObject<Action,stateAlias> go, const char* filename) {
    std::ofstream outputFile;
    outputFile.open(filename);
    for(QLGameObject<Action,stateAlias>::qlTableType::iterator it = go.qlTable.begin(); it != go.qlTable.end(); it++) {
        outputFile << (int)it->first.first << std::endl;
        outputFile << it->first.second.first << std::endl;
        outputFile << it->first.second.second << std::endl;
        outputFile << it->second << std::endl;
    }
    outputFile.close();
}


// std::vector<std::vector<Card>> generateSubtables(Scopa game) {
//     std::vector<std::vector<Card>> result;
//     for(std::vector<Card>::iterator it1 = game.table.cards.begin(); it1 != game.table.cards.end(); it1++) {
//         for(std::vector<Card>::iterator it2 = it1+1; it2 != game.table.cards.end(); it2++) {
//             for(std::vector<Card>::iterator it3 = it2+1; it3 != game.table.cards.end(); it3++) {
//                 for(std::vector<Card>::iterator it4 = it3+1; it4 != game.table.cards.end(); it4++) {
//                     std::vector<Card> tmpSubtable = { *it1, *it2, *it3, *it4 };
//                     result.push_back(tmpSubtable);
//                 }
//             }
//         }
//     }
//     return result;
// }



int main (int argc, char const* argv[])
{
    if(argc<2) {
        // FIXME: throw exception
    }
    
    // define states and actions
    // std::cout << "Loading states..." << std::endl;
    // std::vector<stateType> states = loadStates("states.bin");
    
    // create the game and initialize some useful variables
    std::cout << "Creating the game..." << std::endl;
    Scopa game = Scopa(nullptr);
    Scopa::Who curTurn = Scopa::PLAYER;
    bool multipleChoice = false;
    Card curCard = Card(Card::VOID,0,nullptr,nullptr);
    int playerLastScore = 0, enemyLastScore = 0;
    
    // define function to play cards
    std::cout << "Defining moves..." << std::endl;
    std::function<void(int)> playSmart = [curTurn,&game,&multipleChoice,&curCard](int n) {
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
    
    std::cout << "Defining players..." << std::endl;
    std::cout << "  Defining first player..." << std::endl;
    QLGameObject<Action,stateAlias> player = QLGameObject<Action,stateAlias>(atm,validActFun);
    player.setQlParameters(0.1,0.2);
    // FIXME
    //std::ifstream inPlayer(PLAYER_QLTABLE_FILENAME);
    //if (inPlayer) { loadQLTable(player,inPlayer); }
    //inPlayer.close();
    
    std::cout << "  Defining second player..." << std::endl;
    QLGameObject<Action,stateAlias> enemy  = QLGameObject<Action,stateAlias>(atm,validActFun);
    enemy.setQlParameters(0.1,0.2);
    //std::ifstream inEnemy(ENEMY_QLTABLE_FILENAME);
    //if (inEnemy) { enemy.loadQLTable(inEnemy); }
    //inEnemy.close();

    
    // main loop
    std::cout << "Starting the simulation..." << std::endl;
    int matchTotalNum = atoi(argv[1]);
    for(int matchCounter = 0; matchCounter < matchTotalNum; ++matchCounter) {
        if(matchCounter%10==0) {
            std::cout << "Match number " << matchCounter << "..." << std::endl;
            std::cout << "  Current score: P" << game.pointsPlayer << " E" << game.pointsEnemy << std::endl;
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
            
            //// ALERT!!! Here the QL algorithm can't choose what to do (it's a fault of this implementation; it mantains a low number of states but conflicts are bound to be solved by the main program)
            // if there are more than 4 cards, set the current state to the best subtable there is
            // if(game.table.cards.size() > 4)
//             {
//                 std::vector<std::vector<Card>> subtables = generateSubtables(game);
//                 std::vector<Card> bestTable = *(subtables.begin());
//                 float bestValue = curPlayer->stateValue(stateAlias(cardVecToInt(hand->cards),cardVecToInt(bestTable)));
//                 for(std::vector<std::vector<Card>>::iterator subtableIt = subtables.begin(); subtableIt != subtables.end(); subtableIt++) {
//                     stateAlias tmpState = stateAlias(cardVecToInt(hand->cards),cardVecToInt(*subtableIt));
//                     float tmpValue = curPlayer->stateValue(tmpState);
//                     if(tmpValue > bestValue) {
//                         bestValue = tmpValue;
//                         bestTable = *subtableIt;
//                     }
//                 }
//                 curPlayer->overrideLastState(stateAlias(cardVecToInt(hand->cards),cardVecToInt(bestTable)));
//             }
            
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
            if(game.matchHasEnded()) { game.evaluateScore(); }
            float reward = (otherScore - otherLastScore) * REWARD_WIN - (curScore - curLastScore) * REWARD_LOSE;
            otherLastScore = otherScore;
            otherPlayer->qlUpdate(stateAlias(cardVecToInt(hand->cards),cardVecToInt(game.table.cards)),reward);
            
            if(game.handPlayer.cards.size() == 0 && game.handEnemy.cards.size() == 0) {
                std::cout << "  New hand!" << std::endl;
                game.dealHand(Scopa::PLAYER);
                game.dealHand(Scopa::ENEMY);
            }
            
            
            curTurn = curTurn == Scopa::PLAYER? Scopa::ENEMY : Scopa::PLAYER;
        }
        
    }
    
    std::cout << "Simulation ended!" << std::endl;
    std::cout << "Saving qlTables..." << std::endl;
    
    // save qlTables
    saveQLTable(player,PLAYER_QLTABLE_FILENAME);
    saveQLTable(enemy,ENEMY_QLTABLE_FILENAME);
    
    std::cout << "All done!" << std::endl;
    
    return 0;
}