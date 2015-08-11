#include <ctime>
#include <time.h>
#include <iostream>
#include "utility.h"
#include "QLGameObject.h"
#include "Scopa.h"


typedef std::vector<Card> groupCardStateType;
typedef std::pair<groupCardStateType,groupCardStateType> stateType;

const int HAND_TABLE_SEPARATOR = -1;
const int STATES_SEPARATOR = -2;

const std::vector<Card> STATE_ATOMS = {
        Card(Card::VOID,1, nullptr,nullptr),
        Card(Card::VOID,2, nullptr,nullptr),
        Card(Card::VOID,3, nullptr,nullptr),
        Card(Card::VOID,4, nullptr,nullptr),
        Card(Card::VOID,5, nullptr,nullptr),
        Card(Card::VOID,6, nullptr,nullptr),
        Card(Card::VOID,7, nullptr,nullptr),
        Card(Card::VOID,8, nullptr,nullptr),
        Card(Card::VOID,9, nullptr,nullptr),
        Card(Card::VOID,10,nullptr,nullptr),
        
        Card(Card::DENARI,1, nullptr,nullptr),
        Card(Card::DENARI,2, nullptr,nullptr),
        Card(Card::DENARI,3, nullptr,nullptr),
        Card(Card::DENARI,4, nullptr,nullptr),
        Card(Card::DENARI,5, nullptr,nullptr),
        Card(Card::DENARI,6, nullptr,nullptr),
        Card(Card::DENARI,7, nullptr,nullptr),
        Card(Card::DENARI,8, nullptr,nullptr),
        Card(Card::DENARI,9, nullptr,nullptr),
        Card(Card::DENARI,10,nullptr,nullptr)
    };




bool isValid(groupCardStateType hand, groupCardStateType table) {
    std::map<Card,int> mp;
    for(groupCardStateType::iterator it = hand.begin(); it != hand.end(); it++) {
        if(mp.find(*it) == mp.end()) { mp[*it] = 1; }
        else { mp[*it]++; }
    }
    for(groupCardStateType::iterator it = table.begin(); it != table.end(); it++) {
        if(mp.find(*it) == mp.end()) { mp[*it] = 1; }
        else { mp[*it]++; }
    }
    for(std::map<Card,int>::iterator it = mp.begin(); it != mp.end(); it++) {
        if( (it->first.suit == Card::DENARI && it->second > 1) || (it->first.suit == Card::VOID && it->second > 3) ) { return false; }
    }
    return true;
}

void auxiliarFun(groupCardStateType table, std::vector<groupCardStateType> hands, std::vector<stateType>& states) {
    for(std::vector<groupCardStateType>::iterator it = hands.begin(); it != hands.end(); it++) {
        if(isValid(*it,table)) {
            stateType nstate = stateType(*it,table);
            states.push_back(nstate);
        }
    }
}



std::vector<stateType> generateStates() {
    std::vector<groupCardStateType> hands;
    for(std::vector<Card>::const_iterator it1 = STATE_ATOMS.begin(); it1 != STATE_ATOMS.end(); it1++) {
        groupCardStateType curHand = { *it1 };
        hands.push_back(curHand);
        for(std::vector<Card>::const_iterator it2 = it1; it2 != STATE_ATOMS.end(); it2++) {
            curHand = { *it1, *it2 };
            hands.push_back(curHand);
            for(std::vector<Card>::const_iterator it3 = it2; it3 != STATE_ATOMS.end(); it3++) {
                curHand = { *it1, *it2, *it3 };
                hands.push_back(curHand);
            }
        }
    }
    
    std::vector<stateType> states;
    int counter = 0;
    groupCardStateType curTable = {};
    auxiliarFun(curTable,hands,states);
    for(std::vector<Card>::const_iterator it1 = STATE_ATOMS.begin(); it1 != STATE_ATOMS.end(); it1++) {
        counter++;
        std::cout << "card " << counter << std::endl;
        curTable = {*it1};
        auxiliarFun(curTable,hands,states);
        for(std::vector<Card>::const_iterator it2 = it1; it2 != STATE_ATOMS.end(); it2++) {
            curTable = { *it1, *it2 };
            auxiliarFun(curTable,hands,states);
            for(std::vector<Card>::const_iterator it3 = it2; it3 != STATE_ATOMS.end(); it3++) {
                curTable = { *it1, *it2, *it3 };
                auxiliarFun(curTable,hands,states);
                for(std::vector<Card>::const_iterator it4 = it3; it4 != STATE_ATOMS.end(); it4++) {
                    curTable = { *it1, *it2, *it3, *it4 };
                    auxiliarFun(curTable,hands,states);
                }
            }
        }
    }
        
    std::sort(states.begin(),states.end());
    return states;
}

void saveStates(std::vector<stateType> states, const char* filename) {
    std::ofstream statesFile;
    statesFile.open(filename, std::ios::out | std::ios::binary);
    
    std::cout << "Start writing data on file..." << std::endl;
    int counter = 0;
    int total = states.size();
    for(std::vector<stateType>::iterator it = states.begin(); it != states.end(); it++) {
        counter++;
        if(counter%10000 == 0) { std::cout << "state " << counter << " of " << total << std::endl; }
        for(std::vector<Card>::iterator it1 = it->first.begin(); it1 != it->first.end(); it1++) {
            statesFile.write((char *)&(it1->suit),sizeof(int));
            statesFile.write((char *)&(it1->value),sizeof(int));
        }
        statesFile.write((char *)&(HAND_TABLE_SEPARATOR),sizeof(int));
        for(std::vector<Card>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
            statesFile.write((char *)&(it2->suit),sizeof(int));
            statesFile.write((char *)&(it2->value),sizeof(int));
        }
        statesFile.write((char *)&(STATES_SEPARATOR),sizeof(int));
    }
    statesFile.close();
}

std::vector<stateType> loadStates(const char* filename)
{
    clock_t time_a = clock();
    
    std::vector<stateType> states;
    
    std::ifstream statesFile;
    std::vector<Card> tmpHand, tmpTable;
    int tmpSuit, tmpValue;
    statesFile.open(filename, std::ios::out | std::ios::binary);
    int counter = 0;
    while(statesFile.good()) {
        statesFile.read((char *)&tmpSuit,sizeof(int));
        if(!statesFile.good()) { break; }
        while(tmpSuit != HAND_TABLE_SEPARATOR) {
            statesFile.read((char *)&tmpValue,sizeof(int));
            Card ncard = Card((Card::Suit)tmpSuit,tmpValue,nullptr,nullptr);
            tmpHand.push_back(ncard);
            statesFile.read((char *)&tmpSuit,sizeof(int));
        }
        statesFile.read((char *)&tmpSuit,sizeof(int));
        if(!statesFile.good()) { break; }
        while(tmpSuit != STATES_SEPARATOR) {
            statesFile.read((char *)&tmpValue,sizeof(int));
            Card ncard = Card((Card::Suit)tmpSuit,tmpValue,nullptr,nullptr);
            tmpTable.push_back(ncard);
            statesFile.read((char *)&tmpSuit,sizeof(int));
        }
        states.push_back(stateType(tmpHand,tmpTable));
        tmpHand.clear();
        tmpTable.clear();
        counter++;
        if(counter%10000 == 0) {
            std::cout << counter << " states..." << std::endl;
        }
    }
    statesFile.close();
    
    clock_t time_b = clock();
    
    if (time_a == ((clock_t)-1) || time_b == ((clock_t)-1)) {
        perror("Unable to calculate elapsed time");
    }
    else {
        unsigned int total_time_a = (unsigned int)(time_b - time_a);
        std::cout << "Total time: " << total_time_a << std::endl;
        std::cout << "Number of states: " << states.size() << std::endl;
    }
    
    return states;
}