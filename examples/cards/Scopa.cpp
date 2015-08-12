#include "Scopa.h"
#include <string>
#include "utility.h"


Card::Suit charToSuit(char c) {
    switch(c){
        case 'c':
            return Card::BASTONI;
            break;
        case 'd':
            return Card::DENARI;
            break;
        case 'h':
            return Card::COPPE;
            break;
        case 's':
            return Card::SPADE;
            break;
        default:
            // FIXME: throw exception
        break;
    }
}



Scopa::Scopa(SDL_Renderer* nren) {
    ren = nren;
    
    handPlayer            = CardGroup( true,  false,  ren,    HANDPLAYER_X,           HANDPLAYER_Y,   HANDPLAYER_SHIFT_X, HANDPLAYER_SHIFT_Y  );
    handEnemy             = CardGroup( true, false,  ren,    HANDENEMY_X,            HANDENEMY_Y,    HANDENEMY_SHIFT_X,  HANDENEMY_SHIFT_Y   );
    capturedPilePlayer    = CardGroup( true,  true,   ren,    CAPTUREDPILEPLAYER_X,   CAPTUREDPILEPLAYER_Y                                    );
    capturedPileEnemy     = CardGroup( true,  true,   ren,    CAPTUREDPILEENEMY_X,    CAPTUREDPILEENEMY_Y                                     );
    table                 = CardGroup( true,  false,  ren,    TABLE_X,                TABLE_Y,        TABLE_SHIFT_X,      TABLE_SHIFT_Y       );
    deck                  = CardGroup( false, true,   ren,    DECK_X,                 DECK_Y                                                  );
    
    if(ren != nullptr) {
        loadDeck();
        loadCursor();
    }
    else {
        loadFakeDeck();
    }
    
    pointsPlayer = 0;
    pointsEnemy = 0;
    scopaPointsPlayer = 0;
    scopaPointsEnemy = 0;
    
    focus = nullptr;
}


// FIXME: fix this mess
void Scopa::loadFakeDeck() {
    // load deck
    std::vector<char> suitChar = {'d','c','h','s'};
    for(std::vector<char>::iterator cit = suitChar.begin(); cit != suitChar.end(); cit++)
    {
        for(size_t i = 1; i <= 10 ; ++i)
        {
            Card ncard = Card( charToSuit(*cit), i, nullptr, nullptr );
            deck.pushTop(ncard);
        }
    }
}

void Scopa::loadDeck() {
    
    // laod textures
    std::string dirName = std::string("Nu-mamDeck/");

    cardTex["c1"]    = loadTexture(dirName + std::string("001c.gif"), ren);
    cardTex["c2"]    = loadTexture(dirName + std::string("002c.gif"), ren);
    cardTex["c3"]    = loadTexture(dirName + std::string("003c.gif"), ren);
    cardTex["c4"]    = loadTexture(dirName + std::string("004c.gif"), ren);
    cardTex["c5"]    = loadTexture(dirName + std::string("005c.gif"), ren);
    cardTex["c6"]    = loadTexture(dirName + std::string("006c.gif"), ren);
    cardTex["c7"]    = loadTexture(dirName + std::string("007c.gif"), ren);
    cardTex["c8"]    = loadTexture(dirName + std::string("011c.gif"), ren);
    cardTex["c9"]    = loadTexture(dirName + std::string("012c.gif"), ren);
    cardTex["c10"]   = loadTexture(dirName + std::string("013c.gif"), ren);
    
    cardTex["d1"]    = loadTexture(dirName + std::string("001d.gif"), ren);
    cardTex["d2"]    = loadTexture(dirName + std::string("002d.gif"), ren);
    cardTex["d3"]    = loadTexture(dirName + std::string("003d.gif"), ren);
    cardTex["d4"]    = loadTexture(dirName + std::string("004d.gif"), ren);
    cardTex["d5"]    = loadTexture(dirName + std::string("005d.gif"), ren);
    cardTex["d6"]    = loadTexture(dirName + std::string("006d.gif"), ren);
    cardTex["d7"]    = loadTexture(dirName + std::string("007d.gif"), ren);
    cardTex["d8"]    = loadTexture(dirName + std::string("011d.gif"), ren);
    cardTex["d9"]    = loadTexture(dirName + std::string("012d.gif"), ren);
    cardTex["d10"]   = loadTexture(dirName + std::string("013d.gif"), ren);
    
    cardTex["h1"]    = loadTexture(dirName + std::string("001h.gif"), ren);
    cardTex["h2"]    = loadTexture(dirName + std::string("002h.gif"), ren);
    cardTex["h3"]    = loadTexture(dirName + std::string("003h.gif"), ren);
    cardTex["h4"]    = loadTexture(dirName + std::string("004h.gif"), ren);
    cardTex["h5"]    = loadTexture(dirName + std::string("005h.gif"), ren);
    cardTex["h6"]    = loadTexture(dirName + std::string("006h.gif"), ren);
    cardTex["h7"]    = loadTexture(dirName + std::string("007h.gif"), ren);
    cardTex["h8"]    = loadTexture(dirName + std::string("011h.gif"), ren);
    cardTex["h9"]    = loadTexture(dirName + std::string("012h.gif"), ren);
    cardTex["h10"]   = loadTexture(dirName + std::string("013h.gif"), ren);
    
    cardTex["s1"]    = loadTexture(dirName + std::string("001s.gif"), ren);
    cardTex["s2"]    = loadTexture(dirName + std::string("002s.gif"), ren);
    cardTex["s3"]    = loadTexture(dirName + std::string("003s.gif"), ren);
    cardTex["s4"]    = loadTexture(dirName + std::string("004s.gif"), ren);
    cardTex["s5"]    = loadTexture(dirName + std::string("005s.gif"), ren);
    cardTex["s6"]    = loadTexture(dirName + std::string("006s.gif"), ren);
    cardTex["s7"]    = loadTexture(dirName + std::string("007s.gif"), ren);
    cardTex["s8"]    = loadTexture(dirName + std::string("011s.gif"), ren);
    cardTex["s9"]    = loadTexture(dirName + std::string("012s.gif"), ren);
    cardTex["s10"]   = loadTexture(dirName + std::string("013s.gif"), ren);
    
    cardTex["back"]  = loadTexture(dirName + std::string("Back_1.bmp"), ren);
    
    cardTex["dummy"] = loadTexture(dirName + std::string("Back_2.bmp"), ren);
    
    
    // load deck
    std::vector<char> suitChar = {'d','c','h','s'};
    for(std::vector<char>::iterator cit = suitChar.begin(); cit != suitChar.end(); cit++)
    {
        for(size_t i = 1; i <= 10 ; ++i)
        {
            Card ncard = Card( charToSuit(*cit), i, cardTex[*cit+std::to_string(i)], cardTex["back"] );
            deck.pushTop(ncard);
        }
    }
}

void Scopa::loadCursor() { cursorTex = loadTexture(std::string("images/hand_pointer2.png"), ren); }


std::vector<Scopa::captureType> Scopa::filterByValue(std::vector<captureType> cVec, int value) {
    std::vector<captureType> result;
    for(std::vector<captureType>::iterator it = cVec.begin(); it != cVec.end(); it++) {
        if(it->first == value) { result.push_back(*it); }
    }
    return result;
}


void Scopa::addCursor(cursorType cur) {
    cursors.push_back(cur);
}

void Scopa::addCursor(CardGroup cg, std::vector<Card> cardVec) {
    for(std::vector<Card>::iterator it = cardVec.begin(); it != cardVec.end(); it++ ) {
        addCursor(cursorType(cg,*it));
    }
}

void Scopa::resetCursors() { cursors.clear(); }

void Scopa::setFocus(Who who) {
    std::cout << "void Scopa::setFocus(Who who)" << std::endl;
    who == PLAYER? setFocus(handPlayer) : setFocus(handEnemy);
}

void Scopa::setFocus(CardGroup cg) {
    std::cout << "void Scopa::setFocus(CardGroup cg)" << std::endl;
    
    if(cg.cards.size() == 0) {
        // FIXME: throw exception, focus on empty hand
    }
    else { setFocus(cg,*cg.cards.begin()); }
}

void Scopa::setFocus(CardGroup cg, Card card) {
    focus = new cursorType(cg,card);
}

void Scopa::loseFocus() { focus = nullptr; }

void Scopa::moveFocus(int n) {
    if(focus == nullptr) { return; }
    
    int distance;
    CardGroup& cg = focus->first;
    Card card = focus->second;
    CardGroup::cardIterator cit = std::find(cg.cards.begin(),cg.cards.end(),card);
    
    if(n > 0) { distance = std::min( n, (int)(cg.cards.end() - cit - 1)); }
    else if(n < 0) { distance = std::max( n, (int)(cg.cards.begin() - cit) ); }
    else { return; }
    
    focus = new cursorType(cg,*(cit+distance));
}




std::pair<int,int> Scopa::cursorCoordinates(cursorType cur) {
    return cur.first.cardCoordinates(cur.second);
}

void Scopa::renderAll() {
    SDL_RenderClear(ren);
    
    handPlayer.render();
    handEnemy.render();
    capturedPilePlayer.render();
    capturedPileEnemy.render();
    table.render();
    deck.render();
    
    if(focus) {
        std::pair<int,int> xy = cursorCoordinates(*focus);
        renderTexture(cursorTex, ren, xy.first, xy.second, CURSOR_WIDTH, CURSOR_HEIGHT);
    }
    for(std::vector<cursorType>::iterator it = cursors.begin(); it != cursors.end(); it++) {
        std::pair<int,int> xy = cursorCoordinates(*it);
        renderTexture(cursorTex, ren, xy.first, xy.second, CURSOR_WIDTH, CURSOR_HEIGHT);
    }
    
    SDL_RenderPresent(ren);
}


void Scopa::dealHand(Who who) {
    switch(who) {
        case PLAYER:
            deal(deck,handPlayer,3);
            break;
        case ENEMY:
            deal(deck,handEnemy,3);
            break;
    }
}

void Scopa::dealTable() {
    deal(deck,table,4);
}

void Scopa::startMatch() {
    moveAll(handPlayer,deck);
    moveAll(handEnemy,deck);
    moveAll(capturedPilePlayer,deck);
    moveAll(capturedPileEnemy,deck);
    moveAll(table,deck);
    
    deck.shuffle();
    dealTable();
    dealHand(PLAYER);
    dealHand(ENEMY);
    
    scopaPointsPlayer = 0;
    scopaPointsEnemy = 0;
}

void Scopa::evaluateScore() {
    int pointsPlayer = scopaPointsPlayer, pointsEnemy = scopaPointsEnemy;
    int cardNumPlayer = capturedPilePlayer.cards.size(), cardNumEnemy = capturedPileEnemy.cards.size();
    
    if( cardNumPlayer + cardNumEnemy != 40 ) {
        // FIXME: throw exception
    }
    
    // number of cards
    if(cardNumPlayer > cardNumEnemy) { pointsPlayer++; }
    else if(cardNumPlayer < cardNumEnemy) { pointsEnemy++; }
    
    // number of DENARI
    int denariPlayer = 0, denariEnemy = 0;
    for(CardGroup::cardIterator it = capturedPilePlayer.cards.begin(); it != capturedPilePlayer.cards.end(); it++) {
        if(it->suit == Card::DENARI) { denariPlayer++; }
    }
    for(CardGroup::cardIterator it = capturedPileEnemy.cards.begin(); it != capturedPileEnemy.cards.end(); it++) {
        if(it->suit == Card::DENARI) { denariEnemy++; }
    }
    if(denariPlayer > denariEnemy) { pointsPlayer++; }
    else if(denariPlayer < denariEnemy) { pointsEnemy++; }
    
    // primiera
    // FIXME: not implemented yet
}


void Scopa::sideOfTheTable(Who who, CardGroup*& hand, CardGroup*& capturedPile, int*& points, int*& scopaPoints) {
    switch(who) {
        case PLAYER:
            hand = &handPlayer;
            capturedPile = &capturedPilePlayer;
            points = &pointsPlayer;
            scopaPoints = &scopaPointsPlayer;
            break;
        case ENEMY:
            hand = &handEnemy;
            capturedPile = &capturedPileEnemy;
            points = &pointsEnemy;
            scopaPoints = &scopaPointsEnemy;
            break;
    }
}

bool Scopa::hasCard(Who who, Card card) {
    CardGroup *hand = nullptr, *capturedPile = nullptr;
    int *scopaPoints = nullptr, *points = nullptr;
    sideOfTheTable(who,hand,capturedPile,points,scopaPoints);
    if(hand->mem(card)) { return true; }
    else { return false; }
}

bool Scopa::matchHasEnded() {
    if(deck.cards.size() == 0 && table.cards.size() == 0 && handPlayer.cards.size() == 0 && handEnemy.cards.size() == 0) { return true; }
    else { return false; }
}


std::vector<Scopa::captureType> Scopa::generateTableCaptures() {
    std::vector<captureType> result, tmpVec;
    int sum;
    std::vector<Card> nvec;
        
    for(CardGroup::cardIterator it1 = table.cards.begin(); it1 != table.cards.end(); it1++) {
        tmpVec.clear();
        for(std::vector<captureType>::iterator it2 = result.begin(); it2 != result.end(); it2++) {
            sum = it1->value + it2->first;
            if(sum <= 10) {
                nvec = it2->second;
                nvec.push_back(*it1);
                tmpVec.push_back( captureType(sum,nvec) );
                nvec.clear();
            }
        }
        result.insert( result.end(), tmpVec.begin(), tmpVec.end() );
        std::vector<Card> dumbVec = { *it1 };
        result.push_back( captureType( it1->value, dumbVec ) );
    }
    return result;
}

int Scopa::howManyChoice(Card card, std::vector<captureType> captures) {
    int counter = 0, counterSingle = 0;
    
    for(std::vector<captureType>::iterator it = captures.begin(); it != captures.end(); it++) {
        if( it->first == card.value ) {
            counter++;
            if( it->second.size() == 1 ) { counterSingle++; }
        }
    }
    
    if(counterSingle > 0) { return counterSingle; }
    else { return counter; }
}

bool Scopa::hasMultipleChoice(Card card, std::vector<captureType> captures) { return howManyChoice(card,captures) > 1; }

int Scopa::evalPlay(std::vector<Card> capture) {
    int result = 0;
    if(std::find(capture.begin(),capture.end(),Card(Card::DENARI,7 ,nullptr,nullptr)) == capture.end()) { result++; }
    if(std::find(capture.begin(),capture.end(),Card(Card::DENARI,10,nullptr,nullptr)) == capture.end()) { result++; }
    if(capture.size() == table.cards.size()) { result++; }
    return result;
}


bool Scopa::playCard(Who who, Card card) {
    
    if(!hasCard(who,card)) {
        // FIXME: throw exception
    }
    
    std::vector<Scopa::captureType> tc = generateTableCaptures();
    std::vector<Card> capturedCards;
    bool isSingleCard = false;
    for(std::vector<Scopa::captureType>::iterator it = tc.begin(); it != tc.end(); it++) {
        if(it->first == card.value) {
            if(capturedCards.empty() || (isSingleCard == false && it->second.size() == 1)) { capturedCards = it->second; }
            else {
                // FIXME: throw exception, multiple choice
            }
        }
    }
    
    CardGroup *hand = nullptr, *capturedPile = nullptr;
    int *scopaPoints, *points;
    sideOfTheTable(who,hand,capturedPile,points,scopaPoints);
    if(capturedCards.size() == 0) {
        move(*hand,table,card);
        return false;
    }
    else {
        move(*hand,*capturedPile,card);
        for(std::vector<Card>::iterator it = capturedCards.begin(); it != capturedCards.end(); it++) { move(table,*capturedPile,*it); }
        if(table.cards.size() == 0) { (*points)++; (*scopaPoints)++; }
        return true;
    }
}

bool Scopa::playCard(Who who, Card card, std::vector<Card> capturedOnTable) {
    std::cout << "bool Scopa::playCard(Who who, Card card, std::vector<Card> capturedOnTable)" << std::endl;
    
    if(!hasCard(who,card)) { return false; }
    
    std::vector<Scopa::captureType> tc = generateTableCaptures();
    bool isOnTable = false;
    for(std::vector<Scopa::captureType>::iterator it = tc.begin(); it != tc.end() && isOnTable != true; it++)
        if(cvcmp(capturedOnTable, it->second)) { isOnTable = true; }
    if(!isOnTable) { return false; }
    
    CardGroup *hand = nullptr, *capturedPile = nullptr;
    int *scopaPoints, *points;
    sideOfTheTable(who,hand,capturedPile,points,scopaPoints);
    move(*hand,*capturedPile,card);
    for(std::vector<Card>::iterator it = capturedOnTable.begin(); it != capturedOnTable.end(); it++) { move(table,*capturedPile,*it); }
    if(table.cards.size() == 0) { (*points)++; (*scopaPoints)++; }
    return true;
}

void Scopa::playOnFocus(Who who) {
    std::cout << "void Scopa::playOnFocus(Who who)" << std::endl;
    CardGroup *hand = nullptr, *capturedPile = nullptr;
    int *scopaPoints, *points;
    sideOfTheTable(who,hand,capturedPile,points,scopaPoints);
    
    if(!hand->mem(focus->second)) {
        // FIXME: throw exception
    }
    
    playCard(who,focus->second);
    if(hand->cards.size() == 0) { loseFocus(); }
    else { setFocus(*hand); }
}

void Scopa::playOnFocus(Who who, std::vector<Card> capturedOnTable) {
    CardGroup *hand = nullptr, *capturedPile = nullptr;
    int *scopaPoints, *points;
    sideOfTheTable(who,hand,capturedPile,points,scopaPoints);
    
    if(!hand->mem(focus->second)) {
        // FIXME: throw exception
    }
    
    playCard(who,focus->second,capturedOnTable);
    if(hand->cards.size() == 0) { loseFocus(); }
    else { setFocus(*hand); }
}


void Scopa::shuffle() { deck.shuffle(); }