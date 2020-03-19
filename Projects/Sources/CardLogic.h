#ifndef __CARD_LOGIC_H__
#define __CARD_LOGIC_H__

#include "CardDefine.h"
#include "Define.h"


//
// Structs
//
typedef struct {
    eCardType type;
    int number;
    int baseValue;
} Card;


//
// Functions
//
BOOL initDeck(Card* const cardsP, const int maxCardNumber, const int maxCardType);
BOOL shuffleDeck(Card* const cardsP, const int maxCardCount);
BOOL getCard(Card* const cardsP, const int maxCardCount, Card* const outputCardP);
BOOL calcTotalValue(Card* const cardsP, const int maxCardCount, int* const outputValueP);

#endif  // __CARD_LOGIC_H__
