#include "ScopaQLUtility.h"

const char* PLAYER_QLTABLE_FILENAME = "qlTablePlayer.qlt";
const char* ENEMY_QLTABLE_FILENAME  = "qlTableEnemy.qlt";

const char* PLAYER_QLTABLE_BACKUP_FILENAME = "qlTablePlayer_backup.qlt";
const char* ENEMY_QLTABLE_BACKUP_FILENAME  = "qlTableEnemy_backup.qlt";


int cardToInt(Card card) {
    int result = card.value;
    if(card.suit == Card::DENARI) { result += 10; }
    return result;
}
Card intToCard(int n) {
    Card::Suit suit = (n-1)/10==0? Card::VOID : Card::DENARI;
    int val = n%10==0? 10 : n%10;
    return Card( suit, val, nullptr, nullptr );
}

cardVec intToCardVec(int n) {
    cardVec result;
    while(n != 0) {
        int r = n%21;
        result.push_back( intToCard(r) );
        n /= 21;
    }
    // DEBUG
    //std::sort(result.begin(),result.end());
    return result;
}
int cardVecToInt(cardVec cv) {
    for(cardVec::iterator it = cv.begin(); it != cv.end(); it++) {
        if(it->suit != Card::DENARI) { it->suit = Card::VOID; }
    }
    std::sort(cv.begin(),cv.end());
    
    int result = 0;
    for(cardVec::iterator it = cv.end()-1; it != cv.begin()-1; it--) {
        result += cardToInt(*it);
        result *= 21;
    }
    result /= 21;
    return result;
}


cardVec bestSubtable(QLGameObject<Action,stateAlias>* go, cardVec& hand, cardVec& table) {
    std::vector<Card> best;
    best.insert(best.end(),table.begin(),table.begin()+4);
    int handInt = cardVecToInt(hand);
    stateAlias nstate = stateAlias(handInt,cardVecToInt(best));
    if(go->qlTable.find(std::pair<Action,stateAlias>(PLAY1,nstate)) == go->qlTable.end()) { go->addState(nstate); }
    float bestValue = go->stateValue(nstate);
    
    for(std::vector<Card>::iterator it1 = table.begin(); it1 != table.end(); it1++) {
        for(std::vector<Card>::iterator it2 = it1 +1; it2 != table.end(); it2++) {
            for(std::vector<Card>::iterator it3 = it2 +1; it3 != table.end(); it3++) {
                for(std::vector<Card>::iterator it4 = it3 +1; it4 != table.end(); it4++) {
                    cardVec subtable = { *it1, *it2, *it3, *it4 };
                    nstate = stateAlias(handInt,cardVecToInt(subtable));
                    if(go->qlTable.find(std::pair<Action,stateAlias>(PLAY1,nstate)) == go->qlTable.end()) { go->addState(nstate); }
                    float curValue = go->stateValue(nstate);
                    if(curValue > bestValue) { best = subtable; bestValue = curValue; }
                }
            }
        }
    }
    return best;
}

std::vector<Action> validActFun(stateAlias state) {
    std::vector<Action> result;
    std::vector<Card> tmpVec = intToCardVec(state.first);
    result.push_back(PLAY1);
    if(tmpVec.size() >= 2) { result.push_back(PLAY2); }
    if(tmpVec.size() >= 3) { result.push_back(PLAY3); }
    return result;
};


void saveQLTable(QLGameObject<Action,stateAlias>& go, const char* filename) {
    std::ofstream outputFile;
    outputFile.open(filename);
    int counter = 0;
    for(QLGameObject<Action,stateAlias>::qlTableType::iterator it = go.qlTable.begin(); it != go.qlTable.end(); it++) {
        counter++;
        
        outputFile.write((char *)&(it->first.first),sizeof(Action));
        outputFile.write((char *)&(it->first.second.first),sizeof(unsigned int));
        outputFile.write((char *)&(it->first.second.second),sizeof(unsigned long int));
        outputFile.write((char *)&(it->second),sizeof(float));
    }
    outputFile.close();
    std::cout << "Saved " << counter << " states." << std::endl;
}

void loadQLTable(QLGameObject<Action,stateAlias>& go, const char* filename) {
    std::ifstream inputFile;
    inputFile.open(filename);
    Action act;
    unsigned int stateFirst;
    unsigned long int stateSecond;
    float value;
    int counter = 0;
    while(inputFile.good() || !inputFile.eof()) {
        counter++;
        inputFile.read((char *)&act,sizeof(Action));
        if(!inputFile.good()) { break; }
        inputFile.read((char *)&stateFirst,sizeof(unsigned int));
        inputFile.read((char *)&stateSecond,sizeof(unsigned long int));
        inputFile.read((char *)&value,sizeof(float));
        stateAlias nstate = stateAlias(stateFirst,stateSecond);
        
        
        go.qlTable[std::pair<Action,stateAlias>(act,nstate)] = value;
    }
    inputFile.close();
    std::cout << "Loaded " << counter << " states." << std::endl;
}