#include <time.h>
#include "utility.h"
#include "Scopa.h"
#include "scopaStates.h"


typedef std::vector<Card> cardVec;
typedef std::pair<unsigned int,unsigned long int> stateAlias;

enum Action { PLAY1 , PLAY2, PLAY3 };


bool isOrdered(cardVec cv) {
    cardVec tmpVec = cv;
    std::sort(tmpVec.begin(),tmpVec.end());
    return cv == tmpVec;
}

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

int cardVecToInt(cardVec cv) {
    // DEBUG
    if(cv.size() > 4) { std::cout << "More than 4 cards!!!" << std::endl; }
    
    int result = 0;
    std::sort(cv.begin(),cv.end());
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
        n /= 21;
    }
    std::sort(result.begin(),result.end());
    return result;
}


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


int main (int argc, char const* argv[])
{
    // TEST 1
    if(argc < 2) {
        std::cout << "Too few arguments!" << std::endl;
        exit(-1);
    }

    std::string filename = std::string(argv[1]);

    std::map<Action,std::function<void()>> atm;
    QLGameObject<Action,stateAlias> reader = QLGameObject<Action,stateAlias>(atm,nullptr);
    loadQLTable(reader,filename.c_str());

    int counter = 0;
    for(QLGameObject<Action,stateAlias>::qlTableType::iterator it = reader.qlTable.begin(); it != reader.qlTable.end(); it++) {
        std::cout << it->first.first << " hand (" << it->first.second.first << "): ";
        auto tmpVec = intToCardVec(it->first.second.first);
        for(auto it = tmpVec.begin(); it != tmpVec.end(); it++) {
            std::cout << "(" << it->suit << "," << it->value << ") ";
        }
        std::cout << " table (" << it->first.second.second << "): ";
        tmpVec = intToCardVec(it->first.second.second);
        for(auto it = tmpVec.begin(); it != tmpVec.end(); it++) {
            std::cout << "(" << it->suit << "," << it->value << ") ";
        }
        std::cout << std::endl;
        counter++;
        if(counter%10 == 0) {
            getchar();
        }
    }
    
    // TEST 2
    // cardVec cvTest = { Card(Card::VOID,1,nullptr,nullptr), Card(Card::VOID,2,nullptr,nullptr) };
//     std::cout << isOrdered(cvTest) << std::endl;
//
//     int n;
//     while(1) {
//         std::cin >> n;
//         std::vector<Card> cv = intToCardVec(n);
//         for(auto it = cv.begin(); it != cv.end(); it++) {
//             std::cout << "(" << it->suit << "," << it->value << ") ";
//         }
//         std::cout << std::endl;
//         std::cout << cardVecToInt(cv) << std::endl;
//         getchar();
//     }
    
    // TEST 3
    // if(argc < 2) {
//         std::cout << "Too few arguments!" << std::endl;
//         exit(-1);
//     }
//
//     std::string filename = std::string(argv[1]);
//
//
//
//     std::map<Action,std::function<void()>> atm = {
//         { PLAY1, nullptr },
//         { PLAY2, nullptr },
//         { PLAY3, nullptr }
//     };
//     QLGameObject<Action,stateAlias> reader = QLGameObject<Action,stateAlias>(atm);
//     reader.addState(stateAlias(1,0));
//     reader.addState(stateAlias(1,4));
//     reader.addState(stateAlias(1,4));
//     reader.addState(stateAlias(1,0));
//     reader.addState(stateAlias(3,2));
//     reader.addState(stateAlias(1,0));
//     reader.addState(stateAlias(5,3));
//     saveQLTable(reader,filename.c_str());
//
//     QLGameObject<Action,stateAlias> tester = QLGameObject<Action,stateAlias>(atm);
//     loadQLTable(tester,filename.c_str());
//     for(auto it = tester.qlTable.begin(); it != tester.qlTable.end(); it++) {
//         std::cout << it->first.first << " " << it->first.second.first << " " << it->first.second.second << " " << it->second << std::endl;
//     }
    
    // TEST 4
    // cardVec cv = {
//         Card(Card::VOID,1,nullptr,nullptr),
//         Card(Card::VOID,2,nullptr,nullptr),
//         Card(Card::VOID,3,nullptr,nullptr),
//         Card(Card::VOID,4,nullptr,nullptr),
//         Card(Card::VOID,5,nullptr,nullptr),
//         Card(Card::VOID,6,nullptr,nullptr),
//         Card(Card::VOID,7,nullptr,nullptr),
//         Card(Card::VOID,8,nullptr,nullptr),
//         Card(Card::VOID,9,nullptr,nullptr),
//         Card(Card::VOID,10,nullptr,nullptr),
//         Card(Card::DENARI,10,nullptr,nullptr),
//         Card(Card::DENARI,1,nullptr,nullptr),
//         Card(Card::DENARI,2,nullptr,nullptr),
//         Card(Card::DENARI,3,nullptr,nullptr),
//         Card(Card::DENARI,4,nullptr,nullptr),
//         Card(Card::DENARI,5,nullptr,nullptr),
//         Card(Card::DENARI,6,nullptr,nullptr),
//         Card(Card::DENARI,7,nullptr,nullptr),
//         Card(Card::DENARI,8,nullptr,nullptr),
//         Card(Card::DENARI,9,nullptr,nullptr)
//     };
//     std::cout << isOrdered(cv) << std::endl;
//     std::sort(cv.begin(),cv.end());
//     for(auto it = cv.begin(); it != cv.end(); it++) {
//         std::cout << it->suit << " " << it->value << std::endl;
//     }
    
    
    return 0;
}