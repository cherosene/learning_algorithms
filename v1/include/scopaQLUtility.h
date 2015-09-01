#ifndef SCOPAQLUTILITY_H
#define SCOPAQLUTILITY_H

#include <QLGameObject.h>
#include "Scopa.h"

const int MATCH_COUNTER_STEP = 10000;
const int BACKUP_SAVE_STEP = 1000000;

extern const char* PLAYER_QLTABLE_FILENAME;
extern const char* ENEMY_QLTABLE_FILENAME;

extern const char* PLAYER_QLTABLE_BACKUP_FILENAME;
extern const char* ENEMY_QLTABLE_BACKUP_FILENAME;

const float LEARNING_RATE   = 0.1;
const float DISCOUNT_FACTOR = 0.2;

const float REWARD_WIN  = 0.2;
const float REWARD_LOSE = 0.2;


typedef std::vector<Card> cardVec;
typedef std::pair<unsigned int,unsigned long int> stateAlias;

enum Action { PLAY1 , PLAY2, PLAY3 };


int cardToInt(Card card);
Card intToCard(int n);

cardVec intToCardVec(int n);
int cardVecToInt(cardVec cv);

cardVec bestSubtable(QLGameObject<Action,stateAlias>* go, cardVec& hand, cardVec& table);

std::vector<Action> validActFun(stateAlias state);


void saveQLTable(QLGameObject<Action,stateAlias>& go, const char* filename);
void loadQLTable(QLGameObject<Action,stateAlias>& go, const char* filename);


#endif // SCOPAQLUTILITY_H