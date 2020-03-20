#ifndef __CARD_LOGIC_H__
#define __CARD_LOGIC_H__

#include "CardDefine.h"
#include "Define.h"


//
// Structs
//
typedef struct {        // カード
    eCardType type;
    int number;
    int baseValue;
} Card;

typedef struct {        // デッキデータ
    Card cards[MAX_CARD_NUM * MAX_CARD_TYPE];  // デッキのカード
    int cardCount;                             // デッキの枚数
} DeckData;



//
// Functions
//
BOOL initDeck(DeckData* const deckDataP, const int maxCardNumber, const int maxCardType);
BOOL drawDeck(DeckData* const deckDataP, Card* const cardP);
BOOL shuffleCard(Card* const cardsP, const int maxCardCount);
BOOL getCard(Card* const cardsP, const int maxCardCount, Card* const outputCardP);
BOOL calcTotalValue(Card* const cardsP, const int maxCardCount, int* const outputValueP);

#endif  // __CARD_LOGIC_H__
