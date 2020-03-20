﻿#include "CardLogic.h"
#include "Define.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


//
// Macros
//
#if defined(_DEBUG)
#   define LOG(...) printf(__VA_ARGS__)
#   define CHK(f)   assert(f)
#else
#   define LOG(...) {}
#   define CHK(f)   f
#endif
#define PRINT(...)      printf(__VA_ARGS__)
#define PRINT_CLEAR()   system("cls")


//
// Defines
//
typedef enum {          // ゲームフェイズ
    eGamePhaseEnd,
    eGamePhasePlayer,           // プレイヤーフェイズ
    eGamePhaseDealer,           // ディーラーフェイズ
    eGamePhaseResult,           // リザルトフェイズ
} eGamePhase;
#define MAX_BASE_VALUE  (10)    // 最大ベース値


//
// Structs
//
typedef struct {        // デッキデータ
    Card cards[MAX_CARD_NUM * MAX_CARD_TYPE];  // デッキのカード
    int cardCount;                             // デッキの枚数
} DeckData;

typedef struct {        // プレイデータ
    Card cards[PLAYER_MAX_CARD_CNT];        // 所持カード
    int cardCount;                          // 所持枚数
    int totalValue;                         // トータル値
} PlayData;

typedef eGamePhase(*PhaseExecute)();
typedef struct {        // フェイズ情報
    eGamePhase phase;                       // ゲームフェイズ
    PhaseExecute execute;                   // 実行関数
} PhaseInfo;


//
// Prototypes
//
static eGamePhase phasePlayer();
static eGamePhase phaseDealer();
static eGamePhase phaseResult();


//
// Works
//
static const PhaseInfo PHASE_INFO[] = {     // フェイズ情報
    {eGamePhasePlayer, phasePlayer},
    {eGamePhaseDealer, phaseDealer},
    {eGamePhaseResult, phaseResult},
    {eGamePhaseEnd, NULL},  // 終端判定
};
static eGamePhase m_gamePhase;              // ゲームフェイズ

static DeckData m_deck;                     // デッキデータ
static PlayData m_playerData;               // プレイヤーデータ
static PlayData m_dealerData;               // ディーラーデータ


// デッキの初期化
BOOL initDeck(DeckData* const deckDataP, const int maxCardNumber, const int maxCardType)
{
    int i;
    Card* cardP;
    const int maxCardCount = maxCardNumber * maxCardType;

    if (!deckDataP)
    {
        return FALSE;
    }

    for (i = 0; i < maxCardCount; ++i)
    {
        cardP = &deckDataP->cards[i];
        cardP->type = (i / maxCardNumber) + 1;
        cardP->number = (i % maxCardNumber) + 1;
        cardP->baseValue = (MAX_BASE_VALUE < cardP->number) ? MAX_BASE_VALUE : cardP->number;
    }

    return TRUE;
}

// デッキからカードを引く
BOOL drawDeck(DeckData* const deckDataP, Card* const cardP)
{
    return getCard(deckDataP->cards, deckDataP->cardCount--, cardP);
}

// 初期化
BOOL initialize()
{
    int i;
    const Card initCard = {
        .type = eCardTypeNil,
        .number = 0,
        .baseValue = 0,
    };

    LOG("--- initialize\n");

    // 乱数のシート値を設定
    srand((unsigned int)time(NULL));

    // デッキの初期化
    m_deck.cardCount = MAX_CARD_NUM * MAX_CARD_TYPE;
    CHK(initDeck(&m_deck, MAX_CARD_NUM, MAX_CARD_TYPE));
    CHK(shuffleCard(m_deck.cards, m_deck.cardCount));

    // プレイヤーデータの初期化
    for (i = 0; i < PLAYER_MAX_CARD_CNT; ++i)
    {
        m_playerData.cards[i] = initCard;
        m_dealerData.cards[i] = initCard;
    }

    // 初期枚数カードを引く
    for (i = 0; i < PLAYER_INIT_CARD_CNT; ++i)
    {
        CHK(drawDeck(&m_deck, &m_playerData.cards[i]));
        CHK(drawDeck(&m_deck, &m_dealerData.cards[i]));
    }
    m_playerData.cardCount = PLAYER_INIT_CARD_CNT;
    m_dealerData.cardCount = PLAYER_INIT_CARD_CNT;

    // 現在のカード値を計算しておく
    CHK(calcTotalValue(m_playerData.cards, m_playerData.cardCount, &m_playerData.totalValue));
    CHK(calcTotalValue(m_dealerData.cards, m_dealerData.cardCount, &m_dealerData.totalValue));

    // プレイヤーフェイズから開始
    m_gamePhase = eGamePhasePlayer;

    return TRUE;
}

// プレイデータを表示
void printPlayData()
{
    int i;
    const PlayData* playDataP;

    {
        PRINT("■ディーラー ");
        playDataP = &m_dealerData;
        PRINT("[total:%2d]", playDataP->totalValue);
        for (i = 0; i < playDataP->cardCount; ++i)
        {
            PRINT("[%d:%2d]", i, playDataP->cards[i].number);
        }
        PRINT("\n");
    }
    {
        PRINT("■プレイヤー ");
        playDataP = &m_playerData;
        PRINT("[total:%2d]", playDataP->totalValue);
        for (i = 0; i < playDataP->cardCount; ++i)
        {
            PRINT("[%d:%2d]", i, playDataP->cards[i].number);
        }
        PRINT("\n");
    }
    PRINT("\n");
}

// プレイヤーフェイズ
static eGamePhase phasePlayer()
{
    char drawSelectBuf[8];
    char drawSelect;
    eGamePhase phase = eGamePhasePlayer;

    LOG("--- phase player\n");
    printPlayData();

    if ((BLACKJACK_VALUE == m_playerData.totalValue) ||
        (BLACKJACK_VALUE == m_dealerData.totalValue))
    {
        // 既に終了しているのでリザルトフェイズへ
        phase = eGamePhaseResult;
    }
    else
    {
        PRINT("> カードを引きますか？ (y/n) : ");
        scanf_s("%s", drawSelectBuf, (unsigned int)sizeof(drawSelectBuf));

        drawSelect = drawSelectBuf[0];
        if (drawSelect == 'y' || drawSelect == 'Y')
        {
            // カードを引く
            CHK(drawDeck(&m_deck, &m_playerData.cards[m_playerData.cardCount]));
            m_playerData.cardCount++;
            CHK(calcTotalValue(m_playerData.cards, m_playerData.cardCount, &m_playerData.totalValue));

            if (BLACKJACK_VALUE <= m_playerData.totalValue)
            {
                // 21以上ならディーラーフェイズへ
                phase = eGamePhaseDealer;
            }
        }
        else if (drawSelect == 'n' || drawSelect == 'N')
        {
            // ディーラーフェイズへ
            phase = eGamePhaseDealer;
        }
        else
        {
            PRINT("入力が不正です。再度入力し直してください。\n");
        }
    }

    return phase;
}

// ディーラーフェイズ
static eGamePhase phaseDealer()
{
    eGamePhase phase = eGamePhaseDealer;

    LOG("--- phase dealer\n");
    printPlayData();

    if ((BLACKJACK_VALUE <= m_playerData.totalValue)                    // プレイヤーが21以上
        || (m_playerData.totalValue < m_dealerData.totalValue))         // ディーラーの方が数字が大きい
    {
        // 既に終了しているのでリザルトフェイズへ
        phase = eGamePhaseResult;
    }
    else if ((m_dealerData.totalValue < DELAER_MIN_CARD_VALUE)          // 最低カード値に到達していない
            || (m_dealerData.totalValue <= m_playerData.totalValue))    // ディーラーの方が数字が小さい or 引き分け
    {
        // カードを引く
        CHK(drawDeck(&m_deck, &m_dealerData.cards[m_dealerData.cardCount]));
        m_dealerData.cardCount++;
        CHK(calcTotalValue(m_dealerData.cards, m_dealerData.cardCount, &m_dealerData.totalValue));

        if (BLACKJACK_VALUE <= m_dealerData.totalValue)
        {
            // 21以上ならリザルトフェイズへ
            phase = eGamePhaseResult;
        }
    }

    return phase;
}

// リザルトフェイズ
static eGamePhase phaseResult()
{
    LOG("--- phase result\n");
    printPlayData();

    if (m_playerData.totalValue == m_dealerData.totalValue)
    {
        PRINT("△引き分けです\n");
    }
    else if (BLACKJACK_VALUE < m_dealerData.totalValue)
    {
        PRINT("〇プレイヤーの勝利です\n");
    }
    else if (BLACKJACK_VALUE < m_playerData.totalValue)
    {
        PRINT("×プレイヤーの敗北です\n");
    }
    else if (m_dealerData.totalValue < m_playerData.totalValue)
    {
        PRINT("〇プレイヤーの勝利です\n");
    }
    else if (m_playerData.totalValue < m_dealerData.totalValue)
    {
        PRINT("×プレイヤーの敗北です\n");
    }
    else
    {
        assert(FALSE && "想定していない結果判定です");
    }
    PRINT("\n\n");

    return eGamePhaseEnd;
}

// フェイズ実行関数を取得
static PhaseExecute getPhaseExecute(eGamePhase phase)
{
    const PhaseInfo* infoP = &PHASE_INFO[0];
    while (infoP->phase != eGamePhaseEnd)
    {
        if (infoP->phase == phase)
        {
            break;
        }
        infoP++;
    }

    return infoP->execute;
}

// フェイズの更新
BOOL updatePhase()
{
    BOOL isGameEnd = FALSE;
    const PhaseExecute execute = getPhaseExecute(m_gamePhase);
    if (execute)
    {
        m_gamePhase = execute();
    }
    else
    {
        isGameEnd = TRUE;
    }

    return isGameEnd;
}

// 更新
BOOL update()
{
    LOG("--- update\n");
    return updatePhase();;
}

// メイン関数
int main()
{
    char in[8] = "\0";
    BOOL isGameEnd = FALSE;

    while (in[0] != 'n' && in[0] != 'N')
    {
        PRINT_CLEAR();
        if (!initialize())
        {
            return -1;
        }

        while (!isGameEnd)
        {
            isGameEnd = update();
        }

        // ゲームをループできるようにする
        PRINT("> 再度やり直しますか？ (y/n) : ");
        scanf_s("%s", in, (unsigned int)sizeof(in));
        isGameEnd = FALSE;
    }

    return 0;
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
