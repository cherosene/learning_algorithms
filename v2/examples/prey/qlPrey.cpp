#include <iostream>
#include <fstream>
#include <SDL.h>
#include <vector>
#include "sdlUtility.h"
#include "Entity.h"
#include "QLAgent.h"



enum Keys { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT };
enum CallbackCode { RENDER_CB, MOVEMENT_CB };

typedef Entity::Action QLAction;
typedef std::vector<int> QLState;


const char *FILENAME                = "prey.qlt";
const float CAP                     = 0.001;
const float LR                      = 0.1;
const float DF                      = 0.7;
const float GVR                     = 1;
const float SURVIVE_REWARD          = 0.1;
const float CATCH_REWARD            = -100;

const int FAR                       = 200;
const int NEAR                      = 100;

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
const float         PREY_SPEED      = 10;
// const SDL_Color&    PREY_COLOR      = BLUE;
const unsigned int  PREY_START_X    = 530;
const unsigned int  PREY_START_Y    = 370;



// saving and loading function
void saveQLTable(QLAgent<QLAction,QLState>::qlt& qlTable, const char* filename) {
    std::ofstream outputFile;
    outputFile.open(filename);
    int counter = 0;
    for(QLAgent<QLAction,QLState>::qlt::iterator it = qlTable.begin(); it != qlTable.end(); it++) {
        counter++;
        outputFile.write((char *)&(it->first.first),sizeof(QLAction));
        // FIXME: magic numbers
        for(size_t i = 0; i < 4; ++i) {
            outputFile.write((char *)&(it->first.second[i]),sizeof(int));
        }
        outputFile.write((char *)&(it->second),sizeof(float));
    }
    outputFile.close();
    std::cout << ("Saved " + std::to_string(counter) + " states.") << std::endl;
}

void loadQLTable(QLAgent<QLAction,QLState>::qlt& qlTable, const char* filename) {
    std::ifstream inputFile;
    inputFile.open(filename);
    QLAction action;
    int stateEntry;
    QLState state;
    float value;
    int counter = 0;
    while(inputFile.good() || !inputFile.eof()) {
        counter++;
        inputFile.read((char *)&action,sizeof(QLAction));
        if(!inputFile.good()) { counter--; break; }
        for(size_t i = 0; i < 4; ++i) {
            inputFile.read((char *)&stateEntry,sizeof(int));
            state.push_back(stateEntry);
        }
        inputFile.read((char *)&value,sizeof(float));
        
        qlTable[std::pair<QLAction,QLState>(action,state)] = value;
    }
    inputFile.close();
    std::cout << ( "Loaded " + std::to_string(counter) + " states." ) << std::endl;
}



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


int evaluateDist(int dist) {
    if      ( dist < - FAR  )  { return -2; }
    else if ( dist < - NEAR )  { return -1; }
    else if ( dist >   FAR  )  { return 2; }
    else if ( dist >   NEAR )  { return 1; }
    else                                    { return 0; }
}

int evaluateDistWall(int pos,bool isXPos) {
    int wMin, wMax;
    if(isXPos)  { wMin = BOUND_MIN_X; wMax = BOUND_MAX_X; }
    else        { wMin = BOUND_MIN_Y; wMax = BOUND_MAX_Y; }
    if      ( pos - wMin < NEAR )   { return -1; }
    else if ( wMax - pos < NEAR )   { return 1; }
    else                            { return 0; }
}

QLState dataToQLState(Entity& hunter, Entity& prey) {
    int distOtherX  = evaluateDist(hunter.x - prey.x);
    int distOtherY  = evaluateDist(hunter.y - prey.y);
    int distWallX   = evaluateDistWall(prey.x,true);
    int distWallY   = evaluateDistWall(prey.y,false);
    QLState result  = { distOtherX, distOtherY, distWallX, distWallY };
    return result;
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

    
    // Define QLAgent
    QLAgent<QLAction,QLState>::qlt table;
    if(std::ifstream(FILENAME)) { loadQLTable(table,FILENAME); }
    const std::vector<QLAction> actionVec { Entity::UP, Entity::DOWN, Entity::LEFT, Entity::RIGHT, Entity::IDLE };
    QLAgent<QLAction,QLState> preyAgent = QLAgent<QLAction,QLState>([&actionVec](QLState u){return actionVec;} , table);
    preyAgent.setQLParameters(CAP, LR, DF, GVR);

    // Inizialization
    SDL_Rect bounds = SDL_Rect();
    bounds.x = BOUND_MIN_X;
    bounds.y = BOUND_MIN_Y;
    bounds.w = BOUND_MAX_X - BOUND_MIN_X;
    bounds.h = BOUND_MAX_Y - BOUND_MIN_Y;
    
    Entity hunter   = Entity( HUNTER_WIDTH, HUNTER_HEIGHT,  HUNTER_SPEED,   HUNTER_START_X, HUNTER_START_Y, bounds, HUNTER_COLOR );
    Entity prey     = Entity( PREY_WIDTH,   PREY_HEIGHT,    PREY_SPEED,     PREY_START_X,   PREY_START_Y,   bounds, PREY_COLOR );
    
    preyAgent.setCurrentState(dataToQLState(hunter,prey));
    
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
    int matchCounter = 0;
    while( quit == false ) {
        
        bool matchIsOver = false;
        matchCounter++;
        // int moveCounter = 0;
        // int succesCounter = 0;
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
                                // Entities movement
                                hunter.move( actHunter(hunter,prey) );
                                //userMovement(prey, isKeyPressed);

                                prey.move( preyAgent.chooseAction() );

                                // check contact
                                if( checkContact( hunter, prey ) ) {
                                    hunter.reset();
                                    prey.reset();
                                    preyAgent.update(dataToQLState(hunter,prey), CATCH_REWARD);
                                    matchIsOver = true;
                                }
                                else {
                                    preyAgent.update(dataToQLState(hunter,prey), SURVIVE_REWARD);
                                }

                            break;
                        }


                    break;
                    default:
                    break;
                }
            }

            
            // 
// moveCounter++;
//
//             hunter.move( actHunter(hunter,prey) );
//             //userMovement(prey, isKeyPressed);
//
//             prey.move( preyAgent.chooseAction() );
//
//             // check contact
//             if( checkContact( hunter, prey ) ) {
//                 hunter.reset();
//                 prey.reset();
//                 preyAgent.update(dataToQLState(hunter,prey), CATCH_REWARD);
//                 matchIsOver = true;
//             }
//                else {
//                    preyAgent.update(dataToQLState(hunter,prey), SURVIVE_REWARD);
//                }
//
//             if(matchCounter%10000 == 0) { std::cout << matchCounter << std::endl; }
//             if(matchCounter%100000 == 0) { quit = true; }
//             if(moveCounter == 1000) { succesCounter++; matchIsOver = true; }
//             if(succesCounter == 100) { std::cout << "SUCCESS!!!" << std::endl; }
                
        }
        
    }
    
    std::cout << "states in table: " << table.size() << std::endl;
    saveQLTable(table,FILENAME);
    SDL_Quit();
    return 0;
}
