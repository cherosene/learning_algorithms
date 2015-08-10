#include <ctime>
#include <time.h>
#include <iostream>
#include "utility.h"
#include "QLGameObject.h"
#include "Scopa.h"


typedef std::vector<Card> groupCardStateType;
typedef std::pair<groupCardStateType,groupCardStateType> stateType;

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


int main (int argc, char const* argv[])
{
    // define states
    std::vector<Card> stateAtoms = {
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
    
    clock_t time_a = clock();
    
    std::vector<groupCardStateType> hands;
    for(std::vector<Card>::iterator it1 = stateAtoms.begin(); it1 != stateAtoms.end(); it1++) {
        groupCardStateType curHand = { *it1 };
        hands.push_back(curHand);
        for(std::vector<Card>::iterator it2 = it1; it2 != stateAtoms.end(); it2++) {
            curHand = { *it1, *it2 };
            hands.push_back(curHand);
            for(std::vector<Card>::iterator it3 = it2; it3 != stateAtoms.end(); it3++) {
                curHand = { *it1, *it2, *it3 };
                hands.push_back(curHand);
            }
        }
    }
    
    std::cout << "Number of hands: " << hands.size() << std::endl;
    
    clock_t time_b = clock();
    
    std::vector<stateType> states;
    int counter = 0;
    for(std::vector<Card>::iterator it1 = stateAtoms.begin(); it1 != stateAtoms.end(); it1++) {
        counter++;
        std::cout << "card " << counter << std::endl;
        groupCardStateType curTable = {*it1};
        auxiliarFun(curTable,hands,states);
        for(std::vector<Card>::iterator it2 = it1; it2 != stateAtoms.end(); it2++) {
            curTable = { *it1, *it2 };
            auxiliarFun(curTable,hands,states);
            for(std::vector<Card>::iterator it3 = it2; it3 != stateAtoms.end(); it3++) {
                curTable = { *it1, *it2, *it3 };
                auxiliarFun(curTable,hands,states);
                for(std::vector<Card>::iterator it4 = it3; it4 != stateAtoms.end(); it4++) {
                    curTable = { *it1, *it2, *it3, *it4 };
                    auxiliarFun(curTable,hands,states);
                }
            }
        }
    }
    clock_t time_c = clock();
    
    std::sort(states.begin(),states.end());
    
    clock_t time_d = clock();
    
    
    int htSeparator = -1, sSeparator = -2;
    std::ofstream statesFile;
    statesFile.open("states.bin", std::ios::out | std::ios::binary);
    
    std::cout << "Start writing data on file..." << std::endl;
    counter = 0;
    int total = states.size();
    for(std::vector<stateType>::iterator it = states.begin(); it != states.end(); it++) {
        counter++;
        if(counter%10000 == 0) { std::cout << "state " << counter << " of " << total << std::endl; }
        for(std::vector<Card>::iterator it1 = it->first.begin(); it1 != it->first.end(); it1++) {
            statesFile.write((char *)&(it1->suit),sizeof(int));
            statesFile.write((char *)&(it1->value),sizeof(int));
        }
        statesFile.write((char *)&(htSeparator),sizeof(int));
        for(std::vector<Card>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
            statesFile.write((char *)&(it2->suit),sizeof(int));
            statesFile.write((char *)&(it2->value),sizeof(int));
        }
        statesFile.write((char *)&(sSeparator),sizeof(int));
    }
    statesFile.close();
    
    
    
    if (time_a == ((clock_t)-1) || time_b == ((clock_t)-1) || time_c == ((clock_t)-1) || time_d == ((clock_t)-1)) {
        perror("Unable to calculate elapsed time");
    }
    else {
        unsigned int total_time_a = (unsigned int)(time_b - time_a);
        unsigned int total_time_b = (unsigned int)(time_c - time_b);
        unsigned int total_time_c = total_time_a + total_time_b;
        unsigned int total_time_d = (unsigned int)(time_d - time_c);
        std::cout << "Time first operation: " << total_time_a << std::endl;
        std::cout << "Time second operation: " << total_time_b << std::endl;
        std::cout << "Total time: " << total_time_c << std::endl;
        std::cout << "Time sorting: " << total_time_d << std::endl;
    }
    
    return 0;
}