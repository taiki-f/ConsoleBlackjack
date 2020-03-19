#ifndef __CARD_DEFINE_H__
#define __CARD_DEFINE_H__

//
// Defines
//
typedef enum {
    eCardTypeNil,
    eCardTypeSpade,         // スペード
    eCardTypeClover,        // クローバー
    eCardTypeHeart,         // ハート
    eCardTypeDiamond,       // ダイヤ
} eCardType;
#define MAX_CARD_TYPE           (4)     // カードタイプの最大数
#define MAX_CARD_NUM            (13)    // カードの最大の数字
#define PLAYER_MAX_CARD_CNT     (9)     // プレイヤーの所持できる最大カード枚数
#define PLAYER_INIT_CARD_CNT    (2)     // プレイヤーの初期カード枚数
#define DELAER_MIN_CARD_VALUE   (17)    // ディーラーの最低カード値
#define BLACKJACK_VALUE         (21)    // BlackJackの値

#endif  // __CARD_DEFINE_H__
