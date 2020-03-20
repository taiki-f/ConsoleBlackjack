#include "CardLogic.h"
#include "Define.h"
#include <stdlib.h>


// カードをシャッフル
BOOL shuffleCard(Card* const cardsP, const int maxCardCount)
{
    int i, cardNum;
    Card card;

    if (!cardsP)
    {
        return FALSE;
    }

    for (i = 0; i < maxCardCount; ++i)
    {
        cardNum = rand() % maxCardCount;
        card = cardsP[i];
        cardsP[i] = cardsP[cardNum];
        cardsP[cardNum] = card;
    }

    return TRUE;
}

// カードを1枚引く
BOOL getCard(Card* const cardsP, const int maxCardCount, Card* const outputCardP)
{
    Card* cardP;

    if (!cardsP || !outputCardP || maxCardCount <= 0)
    {
        return FALSE;
    }

    cardP = &cardsP[maxCardCount - 1];
    *outputCardP = *cardP;

    // 引いたカードを初期化
    cardP->type = eCardTypeNil;
    cardP->number = 0;
    cardP->baseValue = 0;

    return TRUE;
}

// エースの分の値を再計算
static BOOL calcAceValue(int* const valueP, const int aceCount, int* const outputValueP)
{
    int addValue;

    if (!valueP || aceCount == 0 || !outputValueP)
    {
        return FALSE;
    }

    // エースを11と捉えるカードの枚数を取得
    addValue = aceCount * 10;
    while (0 < addValue)
    {
        if ((*valueP) + addValue <= BLACKJACK_VALUE)
        {
            break;
        }
        addValue -= 10;
    }
    *valueP += addValue;

    return TRUE;
}

// カードのトータル値を計算
BOOL calcTotalValue(Card* const cardsP, const int maxCardCount, int* const outputValueP)
{
    int i, aceCount = 0, totalValue = 0;

    if (!cardsP || !outputValueP || maxCardCount <= 0)
    {
        return FALSE;
    }

    for (i = 0; i < maxCardCount; ++i)
    {
        totalValue += cardsP[i].baseValue;
        if (cardsP[i].number == 1)
        {
            aceCount++;
        }
    }

    // エース分再計算
    calcAceValue(&totalValue, aceCount, &totalValue);
    *outputValueP = totalValue;

    return TRUE;
}
