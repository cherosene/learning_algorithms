#include <fstream>
#include <iostream>
#include <algorithm>
#include "Entity.h"
#include "LinearNeuralNetwork.h"


const unsigned int              SIMULATION_NUMBER           = 10000;
const std::vector<unsigned int> NEURAL_NETWORK_DESCRIPTOR   = {4,10,5};
const float                     ALPHA                       = 0.5;
const char*                     FILENAME                    = "prey.nn";

const unsigned int  MAX_MOVE_NUMBER     = 10000;
const float         REWARD_DISTANCE     = 0.01;
const float         REWARD_CAPTURE      = 1;

const unsigned int  BOUND_MIN_X     = 100;
const unsigned int  BOUND_MAX_X     = 540;
const unsigned int  BOUND_MIN_Y     = 100;
const unsigned int  BOUND_MAX_Y     = 380;

const unsigned int  HUNTER_WIDTH    = 30;
const unsigned int  HUNTER_HEIGHT   = 30;
const float         HUNTER_SPEED    = 5;
const unsigned int  HUNTER_START_X  = 120;
const unsigned int  HUNTER_START_Y  = 120;

const unsigned int  PREY_WIDTH      = 10;
const unsigned int  PREY_HEIGHT     = 10;
const float         PREY_SPEED      = 15;
const unsigned int  PREY_START_X    = 530;
const unsigned int  PREY_START_Y    = 370;



Entity::Action actHunter(const Entity& hunter, const Entity& prey) {
    int deltaX = hunter.x - prey.x, deltaY = hunter.y - prey.y;
    if( abs(deltaX) > abs(deltaY)  ) {
        if(deltaX > 0)  { return Entity::LEFT; }
        else            { return Entity::RIGHT; }
    }
    else {
        if(deltaY > 0)  { return Entity::UP; }
        else            { return Entity::DOWN; }
    }
    return Entity::IDLE;
}

std::vector<float> outPrey(const Entity& hunter, const Entity& prey, LinearNeuralNetwork& nn) {
    std::vector<float> input    = { (float)hunter.x, (float)hunter.y, (float)prey.x, (float)prey.y };
    return nn.out( input );
}

Entity::Action actPrey(std::vector<float> output) {
    // std::cout << std::distance( output.begin(), std::max_element(output.begin(), output.end()) ) << std::endl;
    // std::cout << (Entity::Action)std::distance( output.begin(), std::max_element(output.begin(), output.end()) ) << std::endl;
    // getchar();
    return (Entity::Action)std::distance( output.begin(), std::max_element(output.begin(), output.end()) );
}


int main (int argc, char const* argv[])
{   
    LinearNeuralNetwork nn = LinearNeuralNetwork( 0, { 1 }, NULL, NULL );
    
    // load nn data
    if(std::ifstream(FILENAME)) { nn = LinearNeuralNetwork( FILENAME, Neuron::SIGMOID, Neuron::SIGMOID_DER ); }
    else                        { nn = LinearNeuralNetwork( ALPHA, NEURAL_NETWORK_DESCRIPTOR, Neuron::SIGMOID, Neuron::SIGMOID_DER ); }

    
    
    SDL_Rect bounds = SDL_Rect();
    bounds.x = BOUND_MIN_X;
    bounds.y = BOUND_MIN_Y;
    bounds.w = BOUND_MAX_X - BOUND_MIN_X;
    bounds.h = BOUND_MAX_Y - BOUND_MIN_Y;
    
    SDL_Color whatever = SDL_Color();
    
    Entity hunter   = Entity( HUNTER_WIDTH, HUNTER_HEIGHT,  HUNTER_SPEED,   HUNTER_START_X, HUNTER_START_Y, bounds, whatever );
    Entity prey     = Entity( PREY_WIDTH,   PREY_HEIGHT,    PREY_SPEED,     PREY_START_X,   PREY_START_Y,   bounds, whatever );
    
    
    // simulation
    for(size_t simCounter = 0; simCounter < SIMULATION_NUMBER; ++simCounter) {
        
        unsigned int moves;
        
        for(size_t moveCounter = 0; moveCounter < MAX_MOVE_NUMBER; ++moveCounter) {
            
            // gather data
            int initialDistance = dist(hunter, prey);
            
            // move
            Entity::Action hunterAction = actHunter( hunter, prey );
            std::vector<float> outputNn = outPrey( hunter, prey, nn );
            Entity::Action preyAction   = actPrey( outputNn );
            hunter.move( hunterAction );
            prey.move( preyAction );
            
            // DEBUG
            // std::cout << " H(" << hunter.x << "," << hunter.y << ")  P(" << prey.x << "," << prey.y << ")" << std::endl;
            moves = moveCounter;
            
            // check if touching and learn
            if( checkContact( hunter, prey ) ) {
                // std::cout << "distances" << initialDistance << std::endl;
                std::vector<float> err (5, 0);
                err[preyAction] = - REWARD_CAPTURE * outputNn[preyAction];
                nn.learn( err );
                break;
            }
            else{
                int distance = dist(hunter, prey);
                // std::cout << "distances" << distance << " " << initialDistance << std::endl;
                std::vector<float> err (5, 0);
                if( distance > initialDistance ) {
                    err[preyAction] = ( 1 - outputNn[preyAction] ) * REWARD_DISTANCE;
                }
                else if( distance < initialDistance ) {
                    err[preyAction] = - outputNn[preyAction] * REWARD_DISTANCE;
                }
                nn.learn( err );
            }
            // getchar();
            
        }
        
        hunter.reset();
        prey.reset();
        
        nn.debug();
        std::cout << simCounter << std::endl;
        std::cout << "-> " << moves << std::endl;
        // getchar();
        
    }
    
    nn.save(FILENAME, "prey");
    
    return 0;
}