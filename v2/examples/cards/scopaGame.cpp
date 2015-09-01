#include <fstream>
#include "ScopaGUI.h"
#include "utility.h"
#include "QLAgent.h"

typedef std::pair<unsigned int, unsigned int>   stateAlias;
typedef unsigned int                            actionAlias;

const size_t MATCH_COUNTER_STEP         = 1000;
const float CASUAL_ACTION_PROBABILITY   = 0.1;
const float LEARNING_RATE               = 0.1;
const float DISCOUNT_FACTOR             = 0.05;
const float GENERATION_VALUE_RANGE      = 1.;
const float WIN_REWARD                  = 0.1;
const float LOOSE_REWARD                = -2.;
const char* QLT_FILENAME                = "scopa.qlt";


bool inputSaysPlayerIsHuman(char const* msg) {
    if(!strcmp(msg,"h")) { return true; }
    else if(!strcmp(msg,"m")) { return false; }
    print( (std::string(msg) + " is not a valid string to determine if player is human.").c_str() );
    exit(-1);
}

// Valid actions are the position of the cards player can play
std::vector<actionAlias> validActFun(stateAlias state) {
    unsigned int handEncoded = state.first;
    std::vector<Card> handVector = CardGroup::decode(handEncoded);
    std::vector<actionAlias> result;
    unsigned int counter = 0;
    Card lastCard = Card(Card::VOID,0);
    for(std::vector<Card>::iterator it = handVector.begin(); it != handVector.end(); it++) {
        if(lastCard != *it) {
            result.push_back(counter);
            lastCard = *it;
        }
        counter++;
    }
    return result;
}

void saveQLTable(QLAgent<actionAlias,stateAlias>::qlt& qlTable, const char* filename) {
    std::ofstream outputFile;
    outputFile.open(filename);
    int counter = 0;
    for(QLAgent<actionAlias,stateAlias>::qlt::iterator it = qlTable.begin(); it != qlTable.end(); it++) {
        counter++;
        outputFile.write((char *)&(it->first.first),sizeof(actionAlias));
        outputFile.write((char *)&(it->first.second.first),sizeof(unsigned int));
        outputFile.write((char *)&(it->first.second.second),sizeof(unsigned int));
        outputFile.write((char *)&(it->second),sizeof(float));
    }
    outputFile.close();
    print( ("Saved " + std::to_string(counter) + " states.").c_str() );
}

void loadQLTable(QLAgent<actionAlias,stateAlias>::qlt& qlTable, const char* filename) {
    std::ifstream inputFile;
    inputFile.open(filename);
    actionAlias action;
    unsigned int stateFirst;
    unsigned int stateSecond;
    float value;
    int counter = 0;
    while(inputFile.good() || !inputFile.eof()) {
        counter++;
        inputFile.read((char *)&action,sizeof(actionAlias));
        if(!inputFile.good()) { counter--; break; }
        inputFile.read((char *)&stateFirst,sizeof(unsigned int));
        inputFile.read((char *)&stateSecond,sizeof(unsigned int));
        inputFile.read((char *)&value,sizeof(float));
        stateAlias nstate = stateAlias(stateFirst,stateSecond);
        
        qlTable[std::pair<actionAlias,stateAlias>(action,nstate)] = value;
    }
    inputFile.close();
    print( ( "Loaded " + std::to_string(counter) + " states." ).c_str() );
}




int main (int argc, char const* argv[]) {
    try {
        ScopaGUI game = ScopaGUI("examples/cards/images");
        bool playerIsHuman[2];

        srand(clock()); 
        if (argc < 4) {
            print("Too few argument!");
            exit(-1);
        }
        playerIsHuman[Scopa::PLAYER_ZERO] = inputSaysPlayerIsHuman(argv[1]);
        playerIsHuman[Scopa::PLAYER_ONE]  = inputSaysPlayerIsHuman(argv[2]);
        // FIXME: use a standard background if boot players are machine
        if(!playerIsHuman[Scopa::PLAYER_ZERO] && !playerIsHuman[Scopa::PLAYER_ONE]) { game.setAutomaticShowOnChange(false); }
        // FIXME: if not a number, give error
        unsigned int matchTotalNumber = atoi(argv[3]);
    
        // QLAgents initialization and function definition
        QLAgent<actionAlias,stateAlias>::qlt qlTable;
        QLAgent<actionAlias,stateAlias> player[2] = {
            QLAgent<actionAlias,stateAlias>(validActFun,qlTable),
            QLAgent<actionAlias,stateAlias>(validActFun,qlTable)
        };
        player[Scopa::PLAYER_ZERO].setQLParameters(CASUAL_ACTION_PROBABILITY, LEARNING_RATE, DISCOUNT_FACTOR, GENERATION_VALUE_RANGE);
        player[Scopa::PLAYER_ONE ].setQLParameters(CASUAL_ACTION_PROBABILITY, LEARNING_RATE, DISCOUNT_FACTOR, GENERATION_VALUE_RANGE);
        float reward[2] = { 0, 0 };
        // strategy to play the card and choose the capture
        std::function<Card()> strategyCard = [&](){
            static bool isFirstTime[2] = { true, true };
            unsigned int handEncoded = CardGroup::encode(game.hand[game.currentTurn]), tableEncoded = CardGroup::encode(game.table);
            stateAlias state = stateAlias(handEncoded,tableEncoded);
            if(isFirstTime[game.currentTurn]) {
                isFirstTime[game.currentTurn] = false;
                player[game.currentTurn].setCurrentState(state);
            }
            else {
                // FIXME: the expression (game.currentTurn+1)%2 is not very pretty
                float currentReward = WIN_REWARD * reward[game.currentTurn] + LOOSE_REWARD * reward[(game.currentTurn+1)%2];
                player[game.currentTurn].update(state,currentReward);
            }
            actionAlias action = player[game.currentTurn].chooseAction();
            
            CardGroup::iterator it = game.hand[game.currentTurn].begin();
            std::advance(it,action);
            return *it;
        };
        std::function<CardGroup(std::vector<CardGroup>)> strategyCapture = [&](std::vector<CardGroup> captures){
            std::function<float(CardGroup)> tmpFun = [&](CardGroup cg){ return game.standardCaptureEvaluationFunction(cg); };
            return game.chooseBestCapture(captures,tmpFun);
        };
    
        if( !playerIsHuman[Scopa::PLAYER_ZERO] || !playerIsHuman[Scopa::PLAYER_ONE] ) {
            print("Load qlTable...");
            loadQLTable(qlTable, QLT_FILENAME);
            print("Loading done!");
        }
    
        // main loop
        game.startMatch(true);
        
        for(size_t matchCounter = 0; matchCounter < matchTotalNumber; ++matchCounter) {
            if(matchCounter % MATCH_COUNTER_STEP == 0) { print( ("Match number " + std::to_string(matchCounter)).c_str() ); }
            
            // match starts here
            while(!game.matchHasEnded()) {
                if(game.roundHasEnded()) { game.startRound(); }
                game.setFocus(game.currentTurn);
                if(playerIsHuman[game.currentTurn]) { game.userPlay(); }
                else { reward[game.currentTurn] = game.strategyPlay(strategyCard,strategyCapture); }
            }
            
            float pointsBeforeScoreEvaluation[2] = { (float)game.pointsMatch[Scopa::PLAYER_ZERO], (float)game.pointsMatch[Scopa::PLAYER_ZERO] };
            game.evaluateScore();
            reward[Scopa::PLAYER_ZERO] += game.pointsMatch[Scopa::PLAYER_ZERO] - pointsBeforeScoreEvaluation[Scopa::PLAYER_ZERO];
            reward[Scopa::PLAYER_ONE]  += game.pointsMatch[Scopa::PLAYER_ONE]  - pointsBeforeScoreEvaluation[Scopa::PLAYER_ONE];
            
            if(playerIsHuman[Scopa::PLAYER_ZERO] || playerIsHuman[Scopa::PLAYER_ONE]) {
                print("Match ended!");
                print( ("Points player zero: " + std::to_string(game.pointsMatch[Scopa::PLAYER_ZERO]) ).c_str() );
                print( ("Points player one: "  + std::to_string(game.pointsMatch[Scopa::PLAYER_ONE])  ).c_str() );
            }
            game.startMatch();
        }
        
        print("The simulation has ended!");
        print("Save qlTables...");
        if( !playerIsHuman[Scopa::PLAYER_ZERO] || !playerIsHuman[Scopa::PLAYER_ONE] ) {
            print("Save player zero qlTable...");
            saveQLTable(qlTable, QLT_FILENAME);
            print("Save done!");
        }
        
        
    }
    catch(std::exception& e) {
        print(e.what());
    }
    
    
    return 0;
}