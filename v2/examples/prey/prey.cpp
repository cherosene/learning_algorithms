#include <iostream>
#include <fstream>
#include <SDL.h>
#include <vector>
#include "sdlUtility.h"
#include "Entity.h"
#include "LinearNeuralNetwork.h"



enum Keys { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT };
enum CallbackCode { RENDER_CB, MOVEMENT_CB };


const char *FILENAME                                        = "prey.nn";
const std::vector<unsigned int> NEURAL_NETWORK_DESCRIPTOR   = {4,20,10,5};
const float ALPHA                                           = 0.5;
const float REWARD_CAPTURE                                  = 1;
const float REWARD_DISTANCE                                 = 0.01;

const uint32_t TIME_GAP_RENDER      = 20;
const uint32_t TIME_GAP_MOVEMENT    = 20;

// const SDL_Color&    BACKGROUND_COLOR    = BLACK;
const char*         WINDOW_NAME     = "Hunter and Prey";

const unsigned int  BOUND_MIN_X     = 100;
const unsigned int  BOUND_MAX_X     = 540;
const unsigned int  BOUND_MIN_Y     = 100;
const unsigned int  BOUND_MAX_Y     = 380;
// const SDL_Color&    BOUND_COLOR     = RED;

const unsigned int  HUNTER_WIDTH    = 30;
const unsigned int  HUNTER_HEIGHT   = 30;
const float         HUNTER_SPEED    = 5;
// const SDL_Color&    HUNTER_COLOR    = GREEN;
const unsigned int  HUNTER_START_X  = 120;
const unsigned int  HUNTER_START_Y  = 120;

const unsigned int  PREY_WIDTH      = 10;
const unsigned int  PREY_HEIGHT     = 10;
const float         PREY_SPEED      = 15;
// const SDL_Color&    PREY_COLOR      = BLUE;
const unsigned int  PREY_START_X    = 530;
const unsigned int  PREY_START_Y    = 370;





// rendering functions
void renderEntity(Entity& e, SDL_Renderer *destination) {
    renderRect( e.x - (e.width / 2), e.y - (e.height / 2), e.width, e.height, e.color, destination );
}


// user input functions
void checkKeyPressed(SDL_Keycode key, bool* keyCheckArray, bool valueToSet) {
    switch(key) {
        case SDLK_UP:
        case SDLK_w:
            keyCheckArray[KEY_UP] = valueToSet;
        break;
        case SDLK_DOWN:
        case SDLK_s:
            keyCheckArray[KEY_DOWN] = valueToSet;
        break;
        case SDLK_LEFT:
        case SDLK_a:
            keyCheckArray[KEY_LEFT] = valueToSet;
        break;
        case SDLK_RIGHT:
        case SDLK_d:
            keyCheckArray[KEY_RIGHT] = valueToSet;
        break;
        default:
        break;
    }
}

void userMovement(Entity& e, bool* keyCheckArray) {
    if( keyCheckArray[KEY_UP] )         { e.move(Entity::UP); }
    else if( keyCheckArray[KEY_DOWN] )  { e.move(Entity::DOWN); }
    else if( keyCheckArray[KEY_LEFT] )  { e.move(Entity::LEFT); }
    else if( keyCheckArray[KEY_RIGHT] ) { e.move(Entity::RIGHT); }
    else                                { e.move(Entity::IDLE); }
}

// automatic movement function
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
    return (Entity::Action)std::distance( output.begin(), std::max_element(output.begin(), output.end()) );
}



// callbacks
uint32_t render_callback(uint32_t interval, void *param) {
    SDL_Event event;
    SDL_UserEvent userevent;
    userevent.type = SDL_USEREVENT;
    userevent.code = RENDER_CB;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    
    return interval;
}

uint32_t movement_callback(uint32_t interval, void *param) {
    SDL_Event event;
    SDL_UserEvent userevent;
    userevent.type = SDL_USEREVENT;
    userevent.code = MOVEMENT_CB;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    
    return interval;
}



int main( int argc, char* args[] )
{
    SDL_Color BACKGROUND_COLOR  = SDL_Color();  BACKGROUND_COLOR.r  = 0;    BACKGROUND_COLOR.g  = 0;    BACKGROUND_COLOR.b  = 0;     BACKGROUND_COLOR.a = 255;
    SDL_Color HUNTER_COLOR      = SDL_Color();  HUNTER_COLOR.r      = 255;  HUNTER_COLOR.g      = 0;    HUNTER_COLOR.b      = 0;     HUNTER_COLOR.a     = 255;
    SDL_Color BOUND_COLOR       = SDL_Color();  BOUND_COLOR.r       = 0;    BOUND_COLOR.g       = 255;  BOUND_COLOR.b       = 0;    BOUND_COLOR.a       = 255;
    SDL_Color PREY_COLOR        = SDL_Color();  PREY_COLOR.r        = 0;    PREY_COLOR.g        = 0;    PREY_COLOR.b        = 255;  PREY_COLOR.a        = 255;
    
    
    // Init GUI
    SDL_Window *screen = NULL;
    if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 ) {
        std::cout << "Error on SDL_Init:" << std::endl << SDL_GetError() << std::endl;
        return 1;
    }
    
    screen = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if( screen == NULL ) {
        std::cout << "Error on SDL_CreateWindow:" << std::endl << SDL_GetError() << std::endl;
        return 1;
    }
    
    SDL_Renderer* bgRenderer = NULL;
    bgRenderer =  SDL_CreateRenderer( screen, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if( bgRenderer == NULL ) {
        std::cout << "Error on SDL_CreateRenderer:" << std::endl << SDL_GetError() << std::endl;
        return 1;
    }

    
    // Define neural network
    LinearNeuralNetwork nn = LinearNeuralNetwork( 0, { 1 }, NULL, NULL );
    if(std::ifstream(FILENAME)) { nn = LinearNeuralNetwork( FILENAME, Neuron::SIGMOID, Neuron::SIGMOID_DER ); }
    else                        { nn = LinearNeuralNetwork( ALPHA, NEURAL_NETWORK_DESCRIPTOR, Neuron::SIGMOID, Neuron::SIGMOID_DER ); }
    

    // Inizialization
    SDL_Rect bounds = SDL_Rect();
    bounds.x = BOUND_MIN_X;
    bounds.y = BOUND_MIN_Y;
    bounds.w = BOUND_MAX_X - BOUND_MIN_X;
    bounds.h = BOUND_MAX_Y - BOUND_MIN_Y;
    
    Entity hunter   = Entity( HUNTER_WIDTH, HUNTER_HEIGHT,  HUNTER_SPEED,   HUNTER_START_X, HUNTER_START_Y, bounds, HUNTER_COLOR );
    Entity prey     = Entity( PREY_WIDTH,   PREY_HEIGHT,    PREY_SPEED,     PREY_START_X,   PREY_START_Y,   bounds, PREY_COLOR );
    
    // Render
    clr(bgRenderer,BACKGROUND_COLOR);
    renderRect(bounds, BOUND_COLOR, bgRenderer, false);
    renderEntity(hunter, bgRenderer);
    renderEntity(prey, bgRenderer);
    SDL_RenderPresent(bgRenderer);



    // TIMERS
    SDL_TimerID renderTimerId = SDL_AddTimer(TIME_GAP_RENDER, render_callback, NULL);
    if( renderTimerId == 0 ) {
        std::cout << "Error on SDL_AddTimer:" << std::endl << SDL_GetError() << std::endl;
        return 1;
    }
    
    SDL_TimerID movementTimerId = SDL_AddTimer(TIME_GAP_MOVEMENT, movement_callback, NULL);
    if( movementTimerId == 0 ) {
        std::cout << "Error on SDL_AddTimer:" << std::endl << SDL_GetError() << std::endl;
        return 1;
    }


    // main cycle
    //// structure to read user events
    SDL_Event event;
    //// to check when the key is pressed or released
    bool isKeyPressed[4] = { false, false, false, false };
    
    bool quit = false;
    while( quit == false ) {
        
        bool matchIsOver = false;
        while(!matchIsOver) {
        
            // event handling
            while( SDL_PollEvent( &event ) ) {
                switch( event.type ) {
                    case SDL_QUIT:
                        quit = true;
                    break;
                    case SDL_KEYDOWN:
                        // Keyboard input handling - keydown
                        checkKeyPressed( event.key.keysym.sym, isKeyPressed, true );
                    break;
                    case SDL_KEYUP:
                        // Keyboard input handling - keyup
                        checkKeyPressed( event.key.keysym.sym, isKeyPressed, false );
                    break;
                    case SDL_USEREVENT:
                    
                        switch(event.user.code) {
                            case RENDER_CB:
                                // Render
                                clr(bgRenderer,BACKGROUND_COLOR);
                                renderRect(bounds, BOUND_COLOR, bgRenderer, false);
                                renderEntity(hunter, bgRenderer);
                                renderEntity(prey, bgRenderer);
                                SDL_RenderPresent(bgRenderer);
                            break;
                            case MOVEMENT_CB:
                                int initialDistance = dist(hunter,prey);
                                // Entities movement
                                hunter.move( actHunter(hunter,prey) );
                                //userMovement(prey, isKeyPressed);
                            
                                // DEBUG
                                std::vector<float> outputNn = outPrey( hunter, prey, nn );
                                Entity::Action preyAction   = actPrey( outputNn );
                                prey.move( preyAction );
                            
                                // check contact
                                if( checkContact( hunter, prey ) ) {
                                    std::vector<float> err (5, 0);
                                    err[preyAction] = - REWARD_CAPTURE * outputNn[preyAction];
                                    nn.learn( err );
                                    matchIsOver = true;
                                }
                                else {
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
                            break;
                            // default:
    //                             std::cout << "!!! UserEvent code not defined!!!" << std::endl;
    //                             return 1;
    //break;
                        }
                
                    
                    break;
                    default:
                    break;
                }
                
                
            }
        }
        
        hunter.reset();
        prey.reset();
        
    }

    SDL_Quit();
    return 0;
}
