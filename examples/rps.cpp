#include <iostream>
#include <vector>
#include <string>
#include "QLGameObject.h"


enum Action {ROCK, PAPER, SCISSOR, ACTION_NUM};

char evaluateMatch(Action pa, Action ea)
{
    switch(pa)
    {
    case ROCK:
        switch(ea)
        {
        case ROCK:
            return 'D';
        break;
        case PAPER:
            return 'E';
        break;
        case SCISSOR:
            return 'P';
        break;
        default:
        // FIXME: throw an exception
        break;
        }
    break;
    case PAPER:
        switch(ea)
        {
        case ROCK:
            return 'P';
        break;
        case PAPER:
            return 'D';
        break;
        case SCISSOR:
            return 'E';
        break;
        default:
        // FIXME: throw an exception
        break;
        }
    break;
    case SCISSOR:
        switch(ea)
        {
        case ROCK:
            return 'E';
        break;
        case PAPER:
            return 'P';
        break;
        case SCISSOR:
            return 'D';
        break;
        default:
        // FIXME: throw an exception
        break;
        }
    break;
    default:
    // FIXME: throw an exception
    break;
    }
    return 'H';
}


int main (int argc, char const* argv[])
{
    int i, matchNum = 1000;
    std::vector<int> result;
    
    Action enemyAction, playerAction = ROCK;
    std::function<Action(int)> currentStrategy;
    
    
    // player setup
    std::map<Action,std::function<void()>> atm;
    for(int i = 0; i < ACTION_NUM; ++i)
        atm[static_cast<Action>(i)] = [&,i](){playerAction = static_cast<Action>(i);};
    std::vector<Action> states = {ROCK, PAPER, SCISSOR};
    QLGameObject<Action,Action> player = QLGameObject<Action,Action>(atm,states);
    player.rinitQlTable(10.);
    player.setQlParameters(0.1,0.2);
    
    // choose strategy
    std::map<std::string,std::function<Action(int)>> strategies;
    strategies["rock"] = [](int i){
        return ROCK;
    };
    strategies["cycle"] = [](int i){
        if(i%3 == 0) {return ROCK;}
        else if(i%3 == 1) {return PAPER;}
        else {return SCISSOR;}
    };
    strategies["rock-scissor"] = [](int i){
        if(i%2 == 0) {return ROCK;}
        else {return SCISSOR;}
    };
    currentStrategy = strategies["rock"];
    
    
    // read input
    if(argc > 1) { matchNum = atoi(argv[1]); }
    if(argc > 2) { currentStrategy = strategies[argv[2]]; }
    
    
    // body
    for(i = 0; i < matchNum; i++)
    {
        enemyAction = currentStrategy(i);
        player.doAction();
        char c = evaluateMatch(playerAction,enemyAction);
        std::cout << c;
        switch(c)
        {
            case 'P':
                player.qlUpdate(enemyAction,1.);
                break;
            case 'E':
                player.qlUpdate(enemyAction,-5.);
                break;
            case 'D':
                player.qlUpdate(enemyAction,0.);
                break;
        }
        
        if(argc > 3 && !strcmp(argv[3],"control"))
        {
            std::cout << std::endl;
            for(std::map<std::pair<Action,Action>,float>::iterator it = player.qlTable.begin(); it != player.qlTable.end(); it++)
                std::cout << static_cast<int>(it->first.first) << ' ' << static_cast<int>(it->first.second) << ": " << it->second << std::endl;
            getchar();
        }
    }
    
    std::cout << std::endl;
    
    return 0;
}