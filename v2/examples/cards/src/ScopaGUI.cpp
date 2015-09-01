#include "ScopaGUI.h"
#include <iostream>
#include <SDL_image.h>


// definitions
constexpr const ScopaGUI::GroupRole ScopaGUI::PLAYER_POSITION       [];
constexpr const char*               ScopaGUI::CARD_IMAGE_NAMES      [];
constexpr const bool                ScopaGUI::GROUP_ROLE_IS_PILE    [];
constexpr const bool                ScopaGUI::GROUP_ROLE_IS_VISIBLE [];
constexpr const unsigned int        ScopaGUI::GROUP_ROLE_X          [];
constexpr const unsigned int        ScopaGUI::GROUP_ROLE_Y          [];
constexpr const int                 ScopaGUI::GROUP_ROLE_SHIFT_X    [];
constexpr const int                 ScopaGUI::GROUP_ROLE_SHIFT_Y    [];


// exception messagges
const char *ScopaGUI::exception::Reason[12] = {
    "Failed to init SDL engine.",
    "Failed to create SDL window.",
    "Failed to create SDL renderer.",
    "Failed to create SDL texture surface.",
    "Failed to load SDL image.",
    "Void suit not allowed in this context.",
    "Not a valid GroupRole.",
    "No focus specified.",
    "Focus not set on hand.",
    "Card not in focus.",
    "The focus is on the wrong hand.",
    "The card was not found."
};

/*  Cursor methods  */
// public methods

ScopaGUI::Cursor::Cursor(const Card& ncard, GroupRole gr, ScopaGUI& nout) : card(ncard), groupRole(gr), out(nout) {    
    if(!out.groupByRole(gr).mem(ncard)) { throw exception(exception::CARD_NOT_FOUND); }
}

void ScopaGUI::Cursor::setFocus(const Card& ncard, GroupRole gr) {
    if(!out.groupByRole(gr).mem(ncard)) { throw exception(exception::CARD_NOT_FOUND); }
    card = ncard;
    groupRole = gr;
}

unsigned int ScopaGUI::Cursor::index() {
    CardGroup& cgRef = out.groupByRole(groupRole);
    return std::distance(cgRef.begin(), cgRef.find(card)); // FIXME: this take linear time; set has not random access iterator, so <iterator> - <iterator> is not viable
}

void ScopaGUI::Cursor::render() {
    unsigned int cardIndex = index();
    unsigned int x = GROUP_ROLE_X[groupRole] + cardIndex * GROUP_ROLE_SHIFT_X[groupRole];
    unsigned int y = GROUP_ROLE_Y[groupRole] + cardIndex * GROUP_ROLE_SHIFT_Y[groupRole];
    out.renderTexture(out.images[CURSOR_IMAGE_CODE], x, y, CURSOR_WIDTH, CURSOR_HEIGHT);
}



/*  ScopaGUI methods  */
// public methods

ScopaGUI::ScopaGUI(const char* cardImagesDirectory) {
    focus = nullptr;
    startGUI();
    // load images
    for(unsigned int i = 0; i <= Scopa::CARD_NUMBER; ++i) {
        images[i] = loadTexture( (std::string(cardImagesDirectory) + "/" + CARD_IMAGE_NAMES[i]).c_str() );
    }
    images[CURSOR_IMAGE_CODE]       = loadTexture( (std::string(cardImagesDirectory) + "/" + CURSOR_IMAGE_NAME).c_str() );
    images[BACKGROUND_IMAGE_CODE]   = loadTexture( (std::string(cardImagesDirectory) + "/" + BACKGROUND_IMAGE_NAME).c_str() );
}

ScopaGUI::~ScopaGUI() {
    stopGUI();
    delete focus;
    for(size_t i = 0; i < sizeof(images)/sizeof(SDL_Texture*); ++i) {
        SDL_DestroyTexture(images[i]);
    }
}

void ScopaGUI::setAutomaticShowOnChange(bool autoShow) {
    showOnChange = autoShow;
}

void ScopaGUI::showAll() {
    SDL_RenderClear(renderer);
    // render background
    renderBackground();
    // render CardGroups
    renderCardGroup(    DECK,               groupByRole(DECK)               );
    renderCardGroup(    TABLE,              groupByRole(TABLE)              );
    renderCardGroup(    HAND_UP,            groupByRole(HAND_UP)            );
    renderCardGroup(    HAND_DOWN,          groupByRole(HAND_DOWN)          );
    renderCardGroup(    CAPTURED_PILE_UP,   groupByRole(CAPTURED_PILE_UP)   );
    renderCardGroup(    CAPTURED_PILE_DOWN, groupByRole(CAPTURED_PILE_DOWN) );
    // render Cursors
    if(focus) { focus->render(); }
    for(std::vector<Cursor>::iterator it = cursorsToRender.begin(); it != cursorsToRender.end(); it++) {
        it->render();
    }
    // present render
    SDL_RenderPresent(renderer);
}

void ScopaGUI::setFocus(Card ncard, GroupRole gr) {
    if(focus) { focus->setFocus(ncard,gr); }
    else { focus = new Cursor(ncard,gr,*this); }
    if(showOnChange) { showAll(); }
}

void ScopaGUI::setFocus(Card ncard, Who player) {
    setFocus(ncard,PLAYER_POSITION[player]);
}

void ScopaGUI::setFocus(Who player) {
    setFocus(hand[player].first(),player);
}

void ScopaGUI::looseFocus(bool showHere) {
    delete focus;
    focus = nullptr;
    if(showOnChange && showHere) { showAll(); }
}

void ScopaGUI::moveFocus(int distance) {
    if(!focus) { throw exception(exception::NO_FOCUS); }
    int currentIndex = focus->index();
    int jumpLength = currentIndex + distance;
    int indexMaxValue = groupByRole(focus->groupRole).size();
    int wantedIndex;
    if(jumpLength >= 0) { wantedIndex = jumpLength % indexMaxValue; }
    else { wantedIndex = jumpLength + ( - jumpLength/indexMaxValue + 1 ) * indexMaxValue; }
    
    CardGroup::iterator currentIt = groupByRole(focus->groupRole).find(focus->card);
    std::advance(currentIt,wantedIndex-currentIndex);
    focus->setFocus(*currentIt,focus->groupRole);
    if(showOnChange) { showAll(); }
}

void ScopaGUI::moveFocusLeft() {
    if(!focus) { throw exception(exception::NO_FOCUS); }
    switch(focus->groupRole) {
        case HAND_UP:
            moveFocus(1);
        break;
        case HAND_DOWN:
            moveFocus(-1);
        break;
        default:
            throw exception(exception::FOCUS_NOT_ON_HAND);
        break;
    }
    if(showOnChange) { showAll(); }
}

void ScopaGUI::moveFocusRight() {
    if(!focus) { throw exception(exception::NO_FOCUS); }
    switch(focus->groupRole) {
        case HAND_UP:
            moveFocus(-1);
        break;
        case HAND_DOWN:
            moveFocus(1);
        break;
        default:
            throw exception(exception::FOCUS_NOT_ON_HAND);
        break;
    }
    if(showOnChange) { showAll(); }
}

void ScopaGUI::addCursor(Card ncard, GroupRole gr, bool showHere) {
    Cursor ncursor = Cursor(ncard,gr,*this);
    cursorsToRender.push_back(ncursor);
    if(showOnChange && showHere) { showAll(); }
}

void ScopaGUI::addCursors(CardGroup cg, GroupRole gr, bool showHere) {
    for(CardGroup::iterator it = cg.begin(); it != cg.end(); it++) {
        addCursor(*it,gr,false);
    }
    if(showOnChange && showHere) { showAll(); }
}

void ScopaGUI::clearCursors(bool showHere) {
    cursorsToRender.clear();
    if(showOnChange && showHere) { showAll(); }
}

unsigned int ScopaGUI::playOnFocus(const CardGroup& cg) {
    return playOnFocus(cg.cards);
}

unsigned int ScopaGUI::playOnFocus(capture capt) {
    if(!focus) { throw exception(exception::NO_FOCUS); }
    if( focus->groupRole != HAND_UP && focus->groupRole != HAND_DOWN) { throw exception(exception::FOCUS_NOT_ON_HAND); }
    if(!groupByRole(focus->groupRole).mem(focus->card)) { throw exception(exception::CARD_NOT_FOUND); }
    
    unsigned int result = playCard(currentTurn,focus->card,capt);
    looseFocus(false);
    if(showOnChange) { showAll(); }
    return result;
}

ScopaGUI::Key ScopaGUI::readKey() {
    SDL_Event inEvent;
    while(true) {
        SDL_WaitEvent( &inEvent );
        if(inEvent.type == SDL_KEYUP) {
            switch(inEvent.key.keysym.sym) {
                case SDLK_UP:
                    return UP;
                break;
                case SDLK_DOWN:
                    return DOWN;
                break;
                case SDLK_LEFT:
                    return LEFT;
                break;
                case SDLK_RIGHT:
                    return RIGHT;
                break;
                case SDLK_RETURN:
                    return RETURN;
                break;
                case SDLK_ESCAPE:
                    return ESCAPE;
                break;
                default:
                break;
            }
        }
        // FIXME: is this the right place where to put SDL_QUIT control?
        else if(inEvent.type == SDL_QUIT) {
            stopGUI();
            exit(0);
        }
    }
}

CardGroup ScopaGUI::userChooseCapture(std::vector<CardGroup> captures) {
    std::vector<CardGroup>::iterator currentCaptureIt = captures.begin();
    addCursors(*currentCaptureIt,TABLE);
    while(true) {
        Key inputKey = readKey();
        switch(inputKey) {
            case LEFT:
                if(currentCaptureIt == captures.begin()) { currentCaptureIt = captures.end() - 1; }
                else { currentCaptureIt--; }
                clearCursors(false);
                addCursors(*currentCaptureIt,TABLE);
            break;
            case RIGHT:
                if(currentCaptureIt == captures.end()-1) { currentCaptureIt = captures.begin(); }
                else { currentCaptureIt++; }
                clearCursors(false);
                addCursors(*currentCaptureIt,TABLE);
            break;
            case RETURN:
                clearCursors();
                return *currentCaptureIt;
            break;
            default:
            break;
        }
    }
}

unsigned int ScopaGUI::userPlay() {
    while(true) {
        Key inputKey = readKey();
        std::vector<CardGroup> captures;
        switch(inputKey) {
            case LEFT:
                moveFocusLeft();
            break;
            case RIGHT:
                moveFocusRight();
            break;
            case RETURN:
                captures = possibleCaptures(focus->card.value);
                if(captures.size() == 0) { return playOnFocus(CardGroup()); }
                else if(captures.size() == 1) { return playOnFocus(captures.front()); }
                else { return playOnFocus(userChooseCapture(captures)); }
            break;
            default:
            break;
        }
    }
}


// protected and private methods

void ScopaGUI::startGUI() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){ throw exception(exception::SDL_INIT); }
    window = SDL_CreateWindow(WINDOW_NAME, WINDOW_X, WINDOW_Y, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr){
        SDL_Quit();
        throw exception(exception::SDL_CREATE_WINDOW);
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr){ 
        if (window) {SDL_DestroyWindow(window);}
    	SDL_Quit();
        throw exception(exception::SDL_CREATE_RENDERER);
    }
}

void ScopaGUI::stopGUI() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void ScopaGUI::renderCardGroup(GroupRole cgRole, CardGroup& cg) {
    if(cg.empty()) { return; }
    if(GROUP_ROLE_IS_PILE[cgRole]) {
        renderCard(GROUP_ROLE_IS_VISIBLE[cgRole],cg.first(),GROUP_ROLE_X[cgRole],GROUP_ROLE_Y[cgRole]);
    }
    else {
        unsigned int currentX = GROUP_ROLE_X[cgRole];
        unsigned int currentY = GROUP_ROLE_Y[cgRole];
        for(CardGroup::iterator it = cg.begin(); it != cg.end(); it++) {
            renderCard(GROUP_ROLE_IS_VISIBLE[cgRole],*it,currentX,currentY);
            currentX += GROUP_ROLE_SHIFT_X[cgRole];
            currentY += GROUP_ROLE_SHIFT_Y[cgRole];
        }
    }
}

CardGroup& ScopaGUI::groupByRole(GroupRole gr) {
    switch(gr) {
        case DECK:
            return deck;
        break;
        case TABLE:
            return table;
        break;
        case HAND_UP:
            return PLAYER_POSITION[PLAYER_ZERO] == HAND_UP? hand[PLAYER_ZERO] : hand[PLAYER_ONE];
        break;
        case HAND_DOWN:
            return PLAYER_POSITION[PLAYER_ZERO] == HAND_DOWN? hand[PLAYER_ZERO] : hand[PLAYER_ONE];
        break;
        case CAPTURED_PILE_UP:
            return PLAYER_POSITION[PLAYER_ZERO] == HAND_UP? capturedPile[PLAYER_ZERO] : capturedPile[PLAYER_ONE];
        break;
        case CAPTURED_PILE_DOWN:
            return PLAYER_POSITION[PLAYER_ZERO] == HAND_DOWN? capturedPile[PLAYER_ZERO] : capturedPile[PLAYER_ONE];
        break;
        default:
            throw exception(exception::GROUP_ROLE_NOT_VALID);
        break;
    }
}

unsigned int ScopaGUI::imageCode(Card& card) {
    unsigned int result = card.value;
    switch(card.suit) {
        case Card::VOID:
            if(card.suit == Card::VOID) { throw exception(exception::VOID_SUIT_NOT_ALLOWED); }
        break;
        case Card::BASTONI:
        break;
        case Card::COPPE:
            result += 10;
        break;
        case Card::SPADE:
            result += 20;
        break;
        case Card::DENARI:
            result += 30;
        break;
        default:
        break;
    }
    return result;
}

SDL_Texture* ScopaGUI::loadTexture(const char* filename) {
	SDL_Texture *texture = nullptr;
	SDL_Surface *loadedImage = IMG_Load(filename);
	if (loadedImage != nullptr){
		texture = SDL_CreateTextureFromSurface(renderer, loadedImage);
		SDL_FreeSurface(loadedImage);
		if (texture == nullptr){
			SDL_Quit();
            std::cout << (renderer == NULL) << std::endl;
            fprintf(stderr, "CreateTextureFromSurface failed: %s\n", SDL_GetError());
            throw exception(exception::SDL_CREATE_TEXTURE_SURFACE);
		}
	}
	else {
		SDL_Quit();
        std::cout << filename << std::endl;
        throw exception(exception::SDL_LOAD_IMAGE);
    }
    return texture;
}

void ScopaGUI::renderTexture(SDL_Texture *tex, unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
	//Setup the destination rectangle
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
    dst.w = width;
    dst.h = height;
	SDL_RenderCopy(renderer, tex, NULL, &dst);
}

void ScopaGUI::renderCard(bool isVisible, Card card, unsigned int x, unsigned int y) {
    SDL_Texture* tex = isVisible? images[imageCode(card)] : images[BACK_IMAGE_CODE];
    renderTexture(tex,x,y,CARD_WIDTH,CARD_HEIGHT);
}

void ScopaGUI::renderBackground() {
    renderTexture(images[BACKGROUND_IMAGE_CODE],0,0,WINDOW_WIDTH,WINDOW_HEIGHT);
}