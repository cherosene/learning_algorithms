#include "ScopaGUI.h"
#include "utility.h"

int main () {
    srand(clock());
    try {
        ScopaGUI game = ScopaGUI("examples/cards/images");
        game.startMatch(true);
        
        while(!game.matchHasEnded()) {
            if(game.roundHasEnded()) { game.startRound(); }
            game.setFocus(game.currentTurn);
            game.userPlay();
        }
        print("Game has ended!");
        game.evaluateScore();
        print("Player zero:");
        print(game.pointsMatch[Scopa::PLAYER_ZERO]);
        print("Player one:");
        print(game.pointsMatch[Scopa::PLAYER_ONE]);
    }
    catch(std::exception& e) {
        print(e.what());
    }
    
    return 0;
}