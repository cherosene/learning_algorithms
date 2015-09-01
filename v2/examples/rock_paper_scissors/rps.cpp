#include <iostream>
#include <vector>
#include <string>
#include "QLAgent.h"


const float LOSE_REWARD = -0.3;
const float WIN_REWARD  =  0.3;
const float DRAW_REWARD =  0.0;

enum Action {ROCK, PAPER, SCISSOR, ACTION_NUM};


std::vector<std::function<Action(int)>> STRATEGIES = {
    [](int n){
        if(n%3 == 0) { return ROCK; }
        else if(n%3 == 1) { return PAPER; }
        return SCISSOR;
    },
    [](int n){
        if(n%2 == 0) { return ROCK; }
        return PAPER;
    }
};


int main (int argc, char const* argv[])
{
    std::function<void(const char*)> log = [](const char* msg){ std::cout << msg << std::endl; };
    
    if( argc < 3 ) {
        log("Too few arguments!!!");
        exit(1);
    }
    const int MATCH_TOTAL_NUMBER = atoi(argv[1]);
    
    // choose enemy strategy
    std::function<Action(int)> strategy = STRATEGIES[atoi(argv[2])];
    
    try {
        // define qlplayer
        QLAgent<Action,Action> player = QLAgent<Action,Action>( [](Action act){ std::vector<Action> vact = {ROCK,PAPER,SCISSOR}; return vact; } );
        player.setQLParameters(0.1,0.2,1.);
        player.setCurrentState(ROCK);
    
        for(int counter = 0; counter != MATCH_TOTAL_NUMBER; counter++) {
            // choose actions
            Action playerChoice = player.chooseAction();
            Action enemyChoice  = strategy(counter);
            // evaluate the match
            float reward;
            int matchResult = ((int)(playerChoice) - (int)(enemyChoice)) %3;
            switch(matchResult) {
                case 1:
                    reward = LOSE_REWARD;
                    std::cout << "L";
                break;
                case 0:
                    reward = DRAW_REWARD;
                    std::cout << "D";
                break;
                case -1:
                    reward = WIN_REWARD;
                    std::cout << "W";
                break;
                default:
                    reward = 0;
                break;
            }
        
            player.update(enemyChoice,reward);
        }
    }
    catch (std::exception e) {
        log(e.what());
    }
    
    
    
    log("");
    log("Game has ended!");
    return 0;
}