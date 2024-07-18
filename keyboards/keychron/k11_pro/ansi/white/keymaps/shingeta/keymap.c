/* Copyright 2023 @ Keychron (https://www.keychron.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H

// 薙刀式
#include "naginata.h"

// content of naginata.c

/* Copyright 2018-2019 eswai <@eswai>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#if !defined(__AVR__)
  #include <string.h>
  //#define memcpy_P(des, src, len) memcpy(des, src, len)
#endif

#define NGBUFFER 5 // バッファのサイズ

static uint8_t ng_chrcount = 0; // 文字キー入力のカウンタ (シフトキーを除く)
static bool is_naginata = false; // 薙刀式がオンかオフか
static uint8_t naginata_layer = 0; // レイヤー番号
static uint8_t n_modifier = 0; // 押しているmodifierキーの数
static uint64_t keycomb = (uint64_t)0; // 新下駄 // 同時押しの状態を示す。64bitの各ビットがキーに対応する。

static bool is_shingeta = true;

// 43ーを64bitの各ビットに割り当てる // 新下駄
#define SG_Q    ((uint64_t)1<<0)
#define SG_W    ((uint64_t)1<<1)
#define SG_E    ((uint64_t)1<<2)
#define SG_R    ((uint64_t)1<<3)
#define SG_T    ((uint64_t)1<<4)

#define SG_Y    ((uint64_t)1<<5)
#define SG_U    ((uint64_t)1<<6)
#define SG_I    ((uint64_t)1<<7)
#define SG_O    ((uint64_t)1<<8)
#define SG_P    ((uint64_t)1<<9)

#define SG_A    ((uint64_t)1<<10)
#define SG_S    ((uint64_t)1<<11)
#define SG_D    ((uint64_t)1<<12)
#define SG_F    ((uint64_t)1<<13)
#define SG_G    ((uint64_t)1<<14)

#define SG_H    ((uint64_t)1<<15)
#define SG_J    ((uint64_t)1<<16)
#define SG_K    ((uint64_t)1<<17)
#define SG_L    ((uint64_t)1<<18)
#define SG_SCLN ((uint64_t)1<<19)

#define SG_Z    ((uint64_t)1<<20)
#define SG_X    ((uint64_t)1<<21)
#define SG_C    ((uint64_t)1<<22)
#define SG_V    ((uint64_t)1<<23)
#define SG_B    ((uint64_t)1<<24)

#define SG_N    ((uint64_t)1<<25)
#define SG_M    ((uint64_t)1<<26)
#define SG_COMM ((uint64_t)1<<27)
#define SG_DOT  ((uint64_t)1<<28)
#define SG_SLSH ((uint64_t)1<<29)

#define SG_1    ((uint64_t)1<<30) // 新下駄
#define SG_2    ((uint64_t)1<<31) // 新下駄
#define SG_3    ((uint64_t)1<<32) // 新下駄
#define SG_4    ((uint64_t)1<<33) // 新下駄
#define SG_5    ((uint64_t)1<<34) // 新下駄
#define SG_6    ((uint64_t)1<<35) // 新下駄
#define SG_7    ((uint64_t)1<<36) // 新下駄
#define SG_8    ((uint64_t)1<<37) // 新下駄
#define SG_9    ((uint64_t)1<<38) // 新下駄
#define SG_0    ((uint64_t)1<<39) // 新下駄
#define SG_MINS ((uint64_t)1<<40) // 新下駄

#define SG_X1   ((uint64_t)1<<41) // 新下駄

#define SG_SHFT ((uint64_t)1<<42)

// 文字入力バッファ
static uint16_t ninputs[NGBUFFER];

// キーコードとキービットの対応
// メモリ削減のため配列はNG_Qを0にしている
const uint64_t ng_key[] = { // 新下駄
  [NG_Q    - NG_Q] = SG_Q,
  [NG_W    - NG_Q] = SG_W,
  [NG_E    - NG_Q] = SG_E,
  [NG_R    - NG_Q] = SG_R,
  [NG_T    - NG_Q] = SG_T,

  [NG_Y    - NG_Q] = SG_Y,
  [NG_U    - NG_Q] = SG_U,
  [NG_I    - NG_Q] = SG_I,
  [NG_O    - NG_Q] = SG_O,
  [NG_P    - NG_Q] = SG_P,

  [NG_A    - NG_Q] = SG_A,
  [NG_S    - NG_Q] = SG_S,
  [NG_D    - NG_Q] = SG_D,
  [NG_F    - NG_Q] = SG_F,
  [NG_G    - NG_Q] = SG_G,

  [NG_H    - NG_Q] = SG_H,
  [NG_J    - NG_Q] = SG_J,
  [NG_K    - NG_Q] = SG_K,
  [NG_L    - NG_Q] = SG_L,
  [NG_SCLN - NG_Q] = SG_SCLN,

  [NG_Z    - NG_Q] = SG_Z,
  [NG_X    - NG_Q] = SG_X,
  [NG_C    - NG_Q] = SG_C,
  [NG_V    - NG_Q] = SG_V,
  [NG_B    - NG_Q] = SG_B,

  [NG_N    - NG_Q] = SG_N,
  [NG_M    - NG_Q] = SG_M,
  [NG_COMM - NG_Q] = SG_COMM,
  [NG_DOT  - NG_Q] = SG_DOT,
  [NG_SLSH - NG_Q] = SG_SLSH,

  [NG_1    - NG_Q] = SG_1, // 新下駄
  [NG_2    - NG_Q] = SG_2, // 新下駄
  [NG_3    - NG_Q] = SG_3, // 新下駄
  [NG_4    - NG_Q] = SG_4, // 新下駄
  [NG_5    - NG_Q] = SG_5, // 新下駄
  [NG_6    - NG_Q] = SG_6, // 新下駄
  [NG_7    - NG_Q] = SG_7, // 新下駄
  [NG_8    - NG_Q] = SG_8, // 新下駄
  [NG_9    - NG_Q] = SG_9, // 新下駄
  [NG_0    - NG_Q] = SG_0, // 新下駄
  [NG_MINS - NG_Q] = SG_MINS, // 新下駄

  [NG_X1   - NG_Q] = SG_X1, // 新下駄

  [NG_SHFT - NG_Q] = SG_SHFT, // 新下駄
};

// 薙刀式カナ変換テーブル // 新下駄
// 順序つき
// #ifdef NAGINATA_JDOUJI
// typedef struct {
//   uint32_t key[3];
//   char kana[5];
// } naginata_keymap_ordered;
// #endif

// 順序なし
typedef struct {
  uint64_t key; // 新下駄
  char kana[5];
} naginata_keymap;

// 順序なしロング // 新下駄
// typedef struct {
//   uint32_t key;
//   char kana[15];
// } naginata_keymap_long;

// 順序なしUNICODE // 新下駄
// typedef struct {
//   uint32_t key;
//   char kana[10];
// } naginata_keymap_unicode;

// #ifdef NAGINATA_JDOUJI // 新下駄
// const PROGMEM naginata_keymap_ordered ngmapo[] = {
//   {.key = {NG_K, NG_E, 0}   , .kana = "ite"},
//   {.key = {NG_L, NG_D, 0}   , .kana = "uto"},
// };
// #endif

const PROGMEM naginata_keymap ngmap[] = { // 新下駄
  // 単独
  {.key = SG_1               , .kana = "1"},
  {.key = SG_2               , .kana = "2"},
  {.key = SG_3               , .kana = "3"},
  {.key = SG_4               , .kana = "4"},
  {.key = SG_5               , .kana = "5"},
  {.key = SG_6               , .kana = "6"},
  {.key = SG_7               , .kana = "7"},
  {.key = SG_8               , .kana = "8"},
  {.key = SG_9               , .kana = "9"},
  {.key = SG_0               , .kana = "0"},
  {.key = SG_MINS            , .kana = "-"},
  {.key = SG_SHFT            , .kana = " "},

  {.key = SG_Q               , .kana = "-"},
  {.key = SG_W               , .kana = "ni"},
  {.key = SG_E               , .kana = "ha"},
  {.key = SG_R               , .kana = ","},
  {.key = SG_T               , .kana = "ti"},
  {.key = SG_Y               , .kana = "gu"},
  {.key = SG_U               , .kana = "ba"},
  {.key = SG_I               , .kana = "ko"},
  {.key = SG_O               , .kana = "ga"},
  {.key = SG_P               , .kana = "hi"},
  {.key = SG_X1              , .kana = "ge"},

  {.key = SG_A               , .kana = "no"},
  {.key = SG_S               , .kana = "to"},
  {.key = SG_D               , .kana = "ka"},
  {.key = SG_F               , .kana = "nn"},
  {.key = SG_G               , .kana = "ltu"},
  {.key = SG_H               , .kana = "ku"},
  {.key = SG_J               , .kana = "u"},
  {.key = SG_K               , .kana = "i"},
  {.key = SG_L               , .kana = "si"},
  {.key = SG_SCLN            , .kana = "na"},

  {.key = SG_Z               , .kana = "su"},
  {.key = SG_X               , .kana = "ma"},
  {.key = SG_C               , .kana = "ki"},
  {.key = SG_V               , .kana = "ru"},
  {.key = SG_B               , .kana = "tu"},
  {.key = SG_N               , .kana = "te"},
  {.key = SG_M               , .kana = "ta"},
  {.key = SG_COMM            , .kana = "de"},
  {.key = SG_DOT             , .kana = "."},
  {.key = SG_SLSH            , .kana = "bu"},

  // 中指シフト
  {.key = SG_K|SG_Q        , .kana = "fa"},
  {.key = SG_K|SG_W        , .kana = "go"},
  {.key = SG_K|SG_E        , .kana = "hu"},
  {.key = SG_K|SG_R        , .kana = "fi"},
  {.key = SG_K|SG_T        , .kana = "fe"},
  {.key = SG_D|SG_Y        , .kana = "wi"},
  {.key = SG_D|SG_U        , .kana = "pa"},
  {.key = SG_D|SG_I        , .kana = "yo"},
  {.key = SG_D|SG_O        , .kana = "mi"},
  {.key = SG_D|SG_P        , .kana = "we"},
  {.key = SG_D|SG_X1       , .kana = "ulo"},

  {.key = SG_K|SG_A        , .kana = "ho"},
  {.key = SG_K|SG_S        , .kana = "ji"},
  {.key = SG_K|SG_D        , .kana = "re"},
  {.key = SG_K|SG_F        , .kana = "mo"},
  {.key = SG_K|SG_G        , .kana = "yu"},
  {.key = SG_D|SG_H        , .kana = "he"},
  {.key = SG_D|SG_J        , .kana = "a"},
  {.key = SG_D|SG_K        , .kana = ""},
  {.key = SG_D|SG_L        , .kana = "o"},
  {.key = SG_D|SG_SCLN     , .kana = "e"},

  {.key = SG_K|SG_Z        , .kana = "du"},
  {.key = SG_K|SG_X        , .kana = "zo"},
  {.key = SG_K|SG_C        , .kana = "bo"},
  {.key = SG_K|SG_V        , .kana = "mu"},
  {.key = SG_K|SG_B        , .kana = "fo"},
  {.key = SG_D|SG_N        , .kana = "se"},
  {.key = SG_D|SG_M        , .kana = "ne"},
  {.key = SG_D|SG_COMM     , .kana = "be"},
  {.key = SG_D|SG_DOT      , .kana = "pu"},
  {.key = SG_D|SG_SLSH     , .kana = "vu"},

  {.key = SG_K|SG_1        , .kana = "la"},
  {.key = SG_K|SG_2        , .kana = "li"},
  {.key = SG_K|SG_3        , .kana = "lu"},
  {.key = SG_K|SG_4        , .kana = "le"},
  {.key = SG_K|SG_5        , .kana = "lo"},

  // 薬指シフト
  {.key = SG_L|SG_Q        , .kana = "di"},
  {.key = SG_L|SG_W        , .kana = "me"},
  {.key = SG_L|SG_E        , .kana = "ke"},
  {.key = SG_L|SG_R        , .kana = "teli"},
  {.key = SG_L|SG_T        , .kana = "deli"},
  {.key = SG_S|SG_Y        , .kana = "sye"},
  {.key = SG_S|SG_U        , .kana = "pe"},
  {.key = SG_S|SG_I        , .kana = "do"},
  {.key = SG_S|SG_O        , .kana = "ya"},
  {.key = SG_S|SG_P        , .kana = "je"},

  {.key = SG_L|SG_A        , .kana = "wo"},
  {.key = SG_L|SG_S        , .kana = "sa"},
  {.key = SG_L|SG_D        , .kana = "o"},
  {.key = SG_L|SG_F        , .kana = "ri"},
  {.key = SG_L|SG_G        , .kana = "zu"},
  {.key = SG_S|SG_H        , .kana = "bi"},
  {.key = SG_S|SG_J        , .kana = "ra"},
  {.key = SG_S|SG_K        , .kana = ""},
  {.key = SG_S|SG_L        , .kana = ""},
  {.key = SG_S|SG_SCLN     , .kana = "so"},

  {.key = SG_L|SG_Z        , .kana = "ze"},
  {.key = SG_L|SG_X        , .kana = "za"},
  {.key = SG_L|SG_C        , .kana = "gi"},
  {.key = SG_L|SG_V        , .kana = "ro"},
  {.key = SG_L|SG_B        , .kana = "nu"},
  {.key = SG_S|SG_N        , .kana = "wa"},
  {.key = SG_S|SG_M        , .kana = "da"},
  {.key = SG_S|SG_COMM     , .kana = "pi"},
  {.key = SG_S|SG_DOT      , .kana = "po"},
  {.key = SG_S|SG_SLSH     , .kana = "tile"},

  {.key = SG_L|SG_1        , .kana = "xya"},
  {.key = SG_L|SG_2        , .kana = "mya"},
  {.key = SG_L|SG_3        , .kana = "myu"},
  {.key = SG_L|SG_4        , .kana = "myo"},
  {.key = SG_L|SG_5        , .kana = "xwa"},

  {.key = SG_I|SG_E     , .kana = "sho"},
  {.key = SG_I|SG_W     , .kana = "shu"},
  {.key = SG_I|SG_R     , .kana = "kyu"},
  {.key = SG_I|SG_F     , .kana = "kyo"},
  {.key = SG_I|SG_V     , .kana = "kya"},
  {.key = SG_I|SG_C     , .kana = "sha"},
  {.key = SG_I|SG_Q     , .kana = "hyu"},
  {.key = SG_I|SG_A     , .kana = "hyo"},
  {.key = SG_I|SG_Z     , .kana = "hya"},
  {.key = SG_I|SG_T     , .kana = "chu"},
  {.key = SG_I|SG_G     , .kana = "cho"},
  {.key = SG_I|SG_B     , .kana = "cha"},

  {.key = SG_I|SG_1     , .kana = "xyu"},
  {.key = SG_I|SG_2     , .kana = "bya"},
  {.key = SG_I|SG_3     , .kana = "byu"},
  {.key = SG_I|SG_4     , .kana = "byo"},

  {.key = SG_O|SG_E     , .kana = "jo"},
  {.key = SG_O|SG_W     , .kana = "ju"},
  {.key = SG_O|SG_R     , .kana = "gyu"},
  {.key = SG_O|SG_F     , .kana = "gyo"},
  {.key = SG_O|SG_V     , .kana = "gya"},
  {.key = SG_O|SG_C     , .kana = "ja"},
  {.key = SG_O|SG_Q     , .kana = "ryu"},
  {.key = SG_O|SG_A     , .kana = "ryo"},
  {.key = SG_O|SG_Z     , .kana = "rya"},
  {.key = SG_O|SG_T     , .kana = "nyu"},
  {.key = SG_O|SG_G     , .kana = "nyo"},
  {.key = SG_O|SG_B     , .kana = "nya"},

  {.key = SG_O|SG_1     , .kana = "xyo"},
  {.key = SG_O|SG_2     , .kana = "pya"},
  {.key = SG_O|SG_3     , .kana = "pyu"},
  {.key = SG_O|SG_4     , .kana = "pyo"},

  // others
  // [RF]・[RG]・　[HU]／
  // [FG]「」　　　[HJ]（）
  // [FV]！[FB]！　[NJ]？

  {.key = SG_R|SG_F     , .kana = "/"},
  {.key = SG_R|SG_G     , .kana = "/"},
  {.key = SG_F|SG_G     , .kana = "()"SS_TAP(X_LEFT) }, // （）←
  {.key = SG_F|SG_V     , .kana = "!"},
  {.key = SG_F|SG_B     , .kana = "!"},
  {.key = SG_N|SG_J     , .kana = "?"},
  {.key = SG_H|SG_J     , .kana = "[]"SS_TAP(X_LEFT) }, // 「」←
  {.key = SG_H|SG_U     , .kana = "/"},
};

const PROGMEM naginata_keymap ngmap2[] = { // 薙刀式
  // 単独
  {.key = SG_1               , .kana = "1"},
  {.key = SG_2               , .kana = "2"},
  {.key = SG_3               , .kana = "3"},
  {.key = SG_4               , .kana = "4"},
  {.key = SG_5               , .kana = "5"},
  {.key = SG_6               , .kana = "6"},
  {.key = SG_7               , .kana = "7"},
  {.key = SG_8               , .kana = "8"},
  {.key = SG_9               , .kana = "9"},
  {.key = SG_0               , .kana = "0"},
  {.key = SG_MINS            , .kana = "-"},
  {.key = SG_Q               , .kana = "vu"},

  // 清音
  {.key = SG_J                      , .kana = "a"       }, // あ
  {.key = SG_K                      , .kana = "i"       }, // い
  {.key = SG_L                      , .kana = "u"       }, // う
  {.key = SG_SHFT|SG_O               , .kana = "e"       }, // え
  {.key = SG_SHFT|SG_N               , .kana = "o"       }, // お
  {.key = SG_F                      , .kana = "ka"      }, // か
  {.key = SG_W                      , .kana = "ki"      }, // き
  {.key = SG_H                      , .kana = "ku"      }, // く
  {.key = SG_S                      , .kana = "ke"      }, // け
  {.key = SG_V                      , .kana = "ko"      }, // こ
  {.key = SG_SHFT|SG_U               , .kana = "sa"      }, // さ
  {.key = SG_R                      , .kana = "si"      }, // し
  {.key = SG_O                      , .kana = "su"      }, // す
  {.key = SG_SHFT|SG_A               , .kana = "se"      }, // せ
  {.key = SG_B                      , .kana = "so"      }, // そ
  {.key = SG_N                      , .kana = "ta"      }, // た
  {.key = SG_SHFT|SG_G               , .kana = "ti"      }, // ち
  {.key = SG_SHFT|SG_SCLN            , .kana = "fu"      }, // ふ
  {.key = SG_E                      , .kana = "te"      }, // て
  {.key = SG_D                      , .kana = "to"      }, // と
  {.key = SG_M                      , .kana = "na"      }, // な
  {.key = SG_SHFT|SG_D               , .kana = "ni"      }, // に
  {.key = SG_SHFT|SG_R               , .kana = "ne"      }, // ね
  {.key = SG_SHFT|SG_COMM            , .kana = "mu"      }, // む
  {.key = SG_SHFT|SG_J               , .kana = "no"      }, // の
  {.key = SG_C                      , .kana = "ha"      }, // は
  {.key = SG_X                      , .kana = "hi"      }, // ひ
  {.key = SG_SHFT|SG_X               , .kana = "hi"      }, // ひ
  {.key = SG_SHFT|SG_DOT             , .kana = "wa"      }, // わ
  {.key = SG_P                      , .kana = "he"      }, // へ
  {.key = SG_Z                      , .kana = "ho"      }, // ほ
  {.key = SG_SHFT|SG_Z               , .kana = "ho"      }, // ほ
  {.key = SG_SHFT|SG_F               , .kana = "ma"      }, // ま
  {.key = SG_SHFT|SG_B               , .kana = "mi"      }, // み
  {.key = SG_SHFT|SG_W               , .kana = "nu"      }, // ぬ
  {.key = SG_SHFT|SG_S               , .kana = "me"      }, // め
  {.key = SG_SHFT|SG_K               , .kana = "mo"      }, // も
  {.key = SG_SHFT|SG_H               , .kana = "ya"      }, // や
  {.key = SG_SHFT|SG_P               , .kana = "yu"      }, // ゆ
  {.key = SG_SHFT|SG_I               , .kana = "yo"      }, // よ
  {.key = SG_DOT                    , .kana = "ra"      }, // ら
  {.key = SG_SHFT|SG_E               , .kana = "ri"      }, // り
  {.key = SG_I                      , .kana = "ru"      }, // る
  {.key = SG_SLSH                   , .kana = "re"      }, // れ
  {.key = SG_SHFT|SG_SLSH            , .kana = "re"      }, // れ
  {.key = SG_A                      , .kana = "ro"      }, // ろ
  {.key = SG_SHFT|SG_L               , .kana = "tsu"      }, // つ
  {.key = SG_SHFT|SG_C               , .kana = "wo"      }, // を
  {.key = SG_COMM                   , .kana = "nn"      }, // ん
  {.key = SG_SCLN                   , .kana = "-"       }, // ー

  // 濁音
  {.key = SG_J|SG_F                  , .kana = "ga"      }, // が
  {.key = SG_J|SG_W                  , .kana = "gi"      }, // ぎ
  {.key = SG_F|SG_H                  , .kana = "gu"      }, // ぐ
  {.key = SG_J|SG_S                  , .kana = "ge"      }, // げ
  {.key = SG_J|SG_V                  , .kana = "go"      }, // ご
  {.key = SG_F|SG_U                  , .kana = "za"      }, // ざ
  {.key = SG_J|SG_R                  , .kana = "zi"      }, // じ
  {.key = SG_F|SG_O                  , .kana = "zu"      }, // ず
  {.key = SG_J|SG_A                  , .kana = "ze"      }, // ぜ
  {.key = SG_J|SG_B                  , .kana = "zo"      }, // ぞ
  {.key = SG_F|SG_N                  , .kana = "da"      }, // だ
  {.key = SG_J|SG_G                  , .kana = "di"      }, // ぢ
  {.key = SG_F|SG_SCLN               , .kana = "bu"      }, // ぶ
  {.key = SG_J|SG_E                  , .kana = "de"      }, // で
  {.key = SG_J|SG_D                  , .kana = "do"      }, // ど
  {.key = SG_J|SG_C                  , .kana = "ba"      }, // ば
  {.key = SG_J|SG_X                  , .kana = "bi"      }, // び
  {.key = SG_F|SG_P                  , .kana = "be"      }, // べ
  {.key = SG_J|SG_Z                  , .kana = "bo"      }, // ぼ
  {.key = SG_F|SG_L                  , .kana = "du"      }, // づ

  // 半濁音
  {.key = SG_M|SG_C                  , .kana = "pa"      }, // ぱ
  {.key = SG_M|SG_X                  , .kana = "pi"      }, // ぴ
  {.key = SG_V|SG_SCLN                , .kana = "pu"      }, // ぷ
  {.key = SG_V|SG_P                  , .kana = "pe"      }, // ぺ
  {.key = SG_M|SG_Z                  , .kana = "po"      }, // ぽ

  // 小書き
  {.key = SG_Q|SG_H                  , .kana = "xya"     }, // ゃ
  {.key = SG_Q|SG_P                  , .kana = "xyu"     }, // ゅ
  {.key = SG_Q|SG_I                  , .kana = "xyo"     }, // ょ
  {.key = SG_Q|SG_J                  , .kana = "xa"      }, // ぁ
  {.key = SG_Q|SG_K                  , .kana = "xi"      }, // ぃ
  {.key = SG_Q|SG_L                  , .kana = "xu"      }, // ぅ
  {.key = SG_Q|SG_O                  , .kana = "xe"      }, // ぇ
  {.key = SG_Q|SG_N                  , .kana = "xo"      }, // ぉ
  {.key = SG_Q|SG_SHFT|SG_L           , .kana = "xwa"     }, // ゎ
  {.key = SG_G                      , .kana = "xtu"     }, // っ

  // 清音拗音 濁音拗音 半濁拗音
  {.key = SG_R|SG_H                  , .kana = "sya"     }, // しゃ
  {.key = SG_R|SG_P                  , .kana = "syu"     }, // しゅ
  {.key = SG_R|SG_I                  , .kana = "syo"     }, // しょ
  {.key = SG_J|SG_R|SG_H              , .kana = "zya"     }, // じゃ
  {.key = SG_J|SG_R|SG_P              , .kana = "zyu"     }, // じゅ
  {.key = SG_J|SG_R|SG_I              , .kana = "zyo"     }, // じょ
  {.key = SG_W|SG_H                  , .kana = "kya"     }, // きゃ
  {.key = SG_W|SG_P                  , .kana = "kyu"     }, // きゅ
  {.key = SG_W|SG_I                  , .kana = "kyo"     }, // きょ
  {.key = SG_J|SG_W|SG_H              , .kana = "gya"     }, // ぎゃ
  {.key = SG_J|SG_W|SG_P              , .kana = "gyu"     }, // ぎゅ
  {.key = SG_J|SG_W|SG_I              , .kana = "gyo"     }, // ぎょ
  {.key = SG_G|SG_H                  , .kana = "tya"     }, // ちゃ
  {.key = SG_G|SG_P                  , .kana = "tyu"     }, // ちゅ
  {.key = SG_G|SG_I                  , .kana = "tyo"     }, // ちょ
  {.key = SG_J|SG_G|SG_H              , .kana = "dya"     }, // ぢゃ
  {.key = SG_J|SG_G|SG_P              , .kana = "dyu"     }, // ぢゅ
  {.key = SG_J|SG_G|SG_I              , .kana = "dyo"     }, // ぢょ
  {.key = SG_D|SG_H                  , .kana = "nya"     }, // にゃ
  {.key = SG_D|SG_P                  , .kana = "nyu"     }, // にゅ
  {.key = SG_D|SG_I                  , .kana = "nyo"     }, // にょ
  {.key = SG_X|SG_H                  , .kana = "hya"     }, // ひゃ
  {.key = SG_X|SG_P                  , .kana = "hyu"     }, // ひゅ
  {.key = SG_X|SG_I                  , .kana = "hyo"     }, // ひょ
  {.key = SG_J|SG_X|SG_H              , .kana = "bya"     }, // びゃ
  {.key = SG_J|SG_X|SG_P              , .kana = "byu"     }, // びゅ
  {.key = SG_J|SG_X|SG_I              , .kana = "byo"     }, // びょ
  {.key = SG_M|SG_X|SG_H              , .kana = "pya"     }, // ぴゃ
  {.key = SG_M|SG_X|SG_P              , .kana = "pyu"     }, // ぴゅ
  {.key = SG_M|SG_X|SG_I              , .kana = "pyo"     }, // ぴょ
  {.key = SG_B|SG_H                  , .kana = "mya"     }, // みゃ
  {.key = SG_B|SG_P                  , .kana = "myu"     }, // みゅ
  {.key = SG_B|SG_I                  , .kana = "myo"     }, // みょ
  {.key = SG_E|SG_H                  , .kana = "rya"     }, // りゃ
  {.key = SG_E|SG_P                  , .kana = "ryu"     }, // りゅ
  {.key = SG_E|SG_I                  , .kana = "ryo"     }, // りょ

  // 清音外来音 濁音外来音
  {.key = SG_M|SG_E|SG_K              , .kana = "thi"     }, // てぃ
  {.key = SG_M|SG_E|SG_P              , .kana = "thu"     }, // てゅ
  {.key = SG_J|SG_E|SG_K              , .kana = "dhi"     }, // でぃ
  {.key = SG_J|SG_E|SG_P              , .kana = "dhu"     }, // でゅ
  {.key = SG_M|SG_D|SG_L              , .kana = "toxu"    }, // とぅ
  {.key = SG_J|SG_D|SG_L              , .kana = "doxu"    }, // どぅ
  {.key = SG_M|SG_R|SG_O              , .kana = "sye"     }, // しぇ
  {.key = SG_M|SG_G|SG_O              , .kana = "tye"     }, // ちぇ
  {.key = SG_J|SG_R|SG_O              , .kana = "zye"     }, // じぇ
  {.key = SG_J|SG_G|SG_O              , .kana = "dye"     }, // ぢぇ
  {.key = SG_V|SG_SCLN|SG_J            , .kana = "fa"      }, // ふぁ
  {.key = SG_V|SG_SCLN|SG_K            , .kana = "fi"      }, // ふぃ
  {.key = SG_V|SG_SCLN|SG_O            , .kana = "fe"      }, // ふぇ
  {.key = SG_V|SG_SCLN|SG_N            , .kana = "fo"      }, // ふぉ
  {.key = SG_V|SG_SCLN|SG_P            , .kana = "fyu"     }, // ふゅ
  {.key = SG_V|SG_K|SG_O              , .kana = "ixe"     }, // いぇ
  {.key = SG_V|SG_L|SG_K              , .kana = "wi"      }, // うぃ
  {.key = SG_V|SG_L|SG_O              , .kana = "we"      }, // うぇ
  {.key = SG_V|SG_L|SG_N              , .kana = "uxo"     }, // うぉ
  {.key = SG_V|SG_H|SG_J              , .kana = "kuxa"    }, // くぁ
  {.key = SG_V|SG_H|SG_K              , .kana = "kuxi"    }, // くぃ
  {.key = SG_V|SG_H|SG_O              , .kana = "kuxe"    }, // くぇ
  {.key = SG_V|SG_H|SG_N              , .kana = "kuxo"    }, // くぉ
  {.key = SG_V|SG_H|SG_L              , .kana = "kuxwa"   }, // くゎ
  {.key = SG_F|SG_H|SG_J              , .kana = "guxa"    }, // ぐぁ
  {.key = SG_F|SG_H|SG_K              , .kana = "guxi"    }, // ぐぃ
  {.key = SG_F|SG_H|SG_O              , .kana = "guxe"    }, // ぐぇ
  {.key = SG_F|SG_H|SG_N              , .kana = "guxo"    }, // ぐぉ
  {.key = SG_F|SG_H|SG_L              , .kana = "guxwa"   }, // ぐゎ
  {.key = SG_V|SG_L|SG_J           , .kana = "tsa"     }, // つぁ
  {.key = SG_V|SG_L|SG_K           , .kana = "tsi"     }, // つぃ
  {.key = SG_V|SG_L|SG_O           , .kana = "tse"     }, // つぇ
  {.key = SG_V|SG_L|SG_N           , .kana = "tso"     }, // つぉ

  // 追加
  {.key = SG_SHFT            , .kana = " "},
  {.key = SG_Q               , .kana = ""},
  {.key = SG_V|SG_SHFT        , .kana = ","},
  {.key = SG_M|SG_SHFT        , .kana = "."SS_TAP(X_ENTER)},
  {.key = SG_U               , .kana = SS_TAP(X_BSPC)},

  // enter
  {.key = SG_V|SG_M           , .kana = SS_TAP(X_ENTER)},
  // enter+シフト(連続シフト)
  {.key = SG_SHFT|SG_V|SG_M    , .kana = SS_TAP(X_ENTER)},
  // left
  {.key = SG_T               , .kana = SS_TAP(X_LEFT)},
  // right
  {.key = SG_Y               , .kana = SS_TAP(X_RIGHT)},

  {.key = SG_J|SG_K|SG_T       , .kana = "/"},  // ・
  {.key = SG_J|SG_K|SG_F       , .kana = "[]"}, // 「」
  {.key = SG_J|SG_K|SG_D       , .kana = "?"},  // ？
  {.key = SG_J|SG_K|SG_C       , .kana = "!"},  // ！
  {.key = SG_J|SG_K|SG_S       , .kana = "()"},  //（）
  {.key = SG_J|SG_K|SG_X       , .kana = "{}"},  //｛｝
  {.key = SG_J|SG_K|SG_A       , .kana = "///"},  // ・

  // 非標準の変換
  {.key = SG_V|SG_DOT|SG_COMM  , .kana = "fe"      }, // ふぇ
  {.key = SG_X|SG_C|SG_M       , .kana = "pyu"     }, // ピュ
};

 // 新下駄
// const PROGMEM naginata_keymap_long ngmapl[] = {
//   {.key = SG_SHFT|SG_T        , .kana = SS_DOWN(X_LSHIFT)SS_TAP(X_DOWN)SS_UP(X_LSHIFT)},
//   {.key = SG_SHFT|SG_Y        , .kana = SS_DOWN(X_LSHIFT)SS_TAP(X_UP)SS_UP(X_LSHIFT)},

//   // 編集モード1
//   {.key = SG_D|SG_F|SG_P       , .kana = SS_TAP(X_ESCAPE)SS_TAP(X_ESCAPE)SS_TAP(X_ESCAPE)},

//   {.key = SG_D|SG_F|SG_K       , .kana = SS_DOWN(X_LSHIFT)SS_TAP(X_LEFT)SS_UP(X_LSHIFT)},
//   {.key = SG_D|SG_F|SG_L       , .kana = SS_TAP(X_LEFT)SS_TAP(X_LEFT)SS_TAP(X_LEFT)SS_TAP(X_LEFT)SS_TAP(X_LEFT)},

//   {.key = SG_D|SG_F|SG_COMM    , .kana = SS_DOWN(X_LSHIFT)SS_TAP(X_RIGHT)SS_UP(X_LSHIFT)},
//   {.key = SG_D|SG_F|SG_DOT     , .kana = SS_TAP(X_RIGHT)SS_TAP(X_RIGHT)SS_TAP(X_RIGHT)SS_TAP(X_RIGHT)SS_TAP(X_RIGHT)},

// #ifdef NAGINATA_EDIT_WIN
//   {.key = SG_J|SG_K|SG_Q       , .kana = SS_DOWN(X_LCTRL)SS_TAP(X_END)SS_UP(X_LCTRL)},
//   {.key = SG_J|SG_K|SG_W       , .kana = SS_DOWN(X_LCTRL)SS_TAP(X_HOME)SS_UP(X_LCTRL)},
//   {.key = SG_D|SG_F|SG_U       , .kana = SS_DOWN(X_LSHIFT)SS_TAP(X_END)SS_UP(X_LSHIFT)SS_TAP(X_BSPACE)},
//   {.key = SG_D|SG_F|SG_H       , .kana = SS_TAP(X_ENTER)SS_TAP(X_END)},
//   {.key = SG_D|SG_F|SG_Y       , .kana = SS_TAP(X_HOME)},
//   {.key = SG_D|SG_F|SG_N       , .kana = SS_TAP(X_END)},
// #endif
// #ifdef NAGINATA_EDIT_MAC
//   {.key = SG_J|SG_K|SG_Q       , .kana = SS_DOWN(X_LGUI)SS_TAP(X_DOWN)SS_UP(X_LGUI)},
//   {.key = SG_J|SG_K|SG_W       , .kana = SS_DOWN(X_LGUI)SS_TAP(X_UP)SS_UP(X_LGUI)},
//   {.key = SG_D|SG_F|SG_U       , .kana = SS_DOWN(X_LSHIFT)SS_DOWN(X_LGUI)SS_TAP(X_RIGHT)SS_UP(X_LGUI)SS_UP(X_LSHIFT)SS_LGUI("x")},
//   {.key = SG_D|SG_F|SG_H       , .kana = SS_TAP(X_ENTER)SS_DOWN(X_LGUI)SS_TAP(X_RIGHT)SS_UP(X_LGUI)},
//   {.key = SG_D|SG_F|SG_Y       , .kana = SS_DOWN(X_LGUI)SS_TAP(X_LEFT)SS_UP(X_LGUI)},
//   {.key = SG_D|SG_F|SG_N       , .kana = SS_DOWN(X_LGUI)SS_TAP(X_RIGHT)SS_UP(X_LGUI)},
// #endif

//   // 編集モード2
// #ifdef NAGINATA_EDIT_WIN
//   {.key = SG_M|SG_COMM|SG_T    , .kana = SS_TAP(X_HOME)" "SS_TAP(X_END)},
//   {.key = SG_M|SG_COMM|SG_G    , .kana = SS_TAP(X_HOME)"   "SS_TAP(X_END)},
//   {.key = SG_C|SG_V|SG_U       , .kana = SS_DOWN(X_LSHIFT)SS_TAP(X_HOME)SS_UP(X_LSHIFT)SS_LCTRL("x")},
//   {.key = SG_C|SG_V|SG_I       , .kana = SS_DOWN(X_LCTRL)SS_TAP(X_BSPACE)SS_UP(X_LCTRL)},
// #endif
// #ifdef NAGINATA_EDIT_MAC
//   {.key = SG_M|SG_COMM|SG_T    , .kana = SS_DOWN(X_LGUI)SS_TAP(X_LEFT)SS_UP(X_LGUI)" "SS_DOWN(X_LGUI)SS_TAP(X_RIGHT)SS_UP(X_LGUI)},
//   {.key = SG_M|SG_COMM|SG_G    , .kana = SS_DOWN(X_LGUI)SS_TAP(X_LEFT)SS_UP(X_LGUI)"   "SS_DOWN(X_LGUI)SS_TAP(X_RIGHT)SS_UP(X_LGUI)},
//   {.key = SG_C|SG_V|SG_U       , .kana = SS_DOWN(X_LSHIFT)SS_DOWN(X_LGUI)SS_TAP(X_LEFT)SS_UP(X_LGUI)SS_UP(X_LSHIFT)SS_LGUI("x")},
//   {.key = SG_C|SG_V|SG_I       , .kana = ""},
// #endif

// };

 // 新下駄
// const PROGMEM naginata_keymap_unicode ngmapu[] = {
  // 編集モード2 // 新下駄
// #ifdef NAGINATA_EDIT_WIN
//   {.key = SG_SHFT|SG_W    , .kana = "309C"},
//   {.key = SG_L           , .kana = "309B"},
// #endif
// };

// 薙刀式のレイヤー、シフトキーを設定
void set_naginata(uint8_t layer) {
  naginata_layer = layer;
}

// 薙刀式をオンオフ
void naginata_on(void) {
  is_naginata = true;
  keycomb = (uint64_t)0; // 新下駄
  naginata_clear();
  layer_on(naginata_layer);

  tap_code(KC_LNG1); // Mac
  //tap_code(KC_DEL); // Mac
  //tap_code(KC_INT4); // Win
}

void naginata_off(void) {
  is_naginata = false;
  keycomb = (uint64_t)0; // 新下駄
  naginata_clear();
  layer_off(naginata_layer);

  tap_code(KC_LNG2); // Mac
  //tap_code(KC_HOME); // Mac
  //tap_code(KC_INT5); // Win
}

// 薙刀式の状態
bool naginata_state(void) {
  return is_naginata;
}


// キー入力を文字に変換して出力する
void naginata_type(void) {
 // 新下駄
// #ifdef NAGINATA_JDOUJI
//   naginata_keymap_ordered bngmapo; // PROGMEM buffer
// #endif
  naginata_keymap bngmap; // PROGMEM buffer
   // 新下駄
  // naginata_keymap_long bngmapl; // PROGMEM buffer
  // naginata_keymap_unicode bngmapu; // PROGMEM buffer

  uint32_t skey = 0; // 連続押しの場合のバッファ

  switch (keycomb) {
    // send_stringできないキー、長すぎるマクロはここで定義
 // 新下駄
    // case SG_F|SG_G:
    //   naginata_off();
    //   break;
    // case SG_J|SG_K|SG_T:
// #ifdef NAGINATA_EDIT_WIN
//       tap_code(KC_HOME);
//       for (int i = 0; i < 10; i++) tap_code(KC_RGHT);
// #endif
// #ifdef NAGINATA_EDIT_MAC
//       register_code(KC_LGUI);
//       tap_code(KC_LEFT);
//       unregister_code(KC_LGUI);
//       for (int i = 0; i < 10; i++) tap_code(KC_RGHT);
// #endif
//       break;
//     case SG_J|SG_K|SG_G:
// #ifdef NAGINATA_EDIT_WIN
//       tap_code(KC_HOME);
//       for (int i = 0; i < 20; i++) tap_code(KC_RGHT);
// #endif
// #ifdef NAGINATA_EDIT_MAC
//       register_code(KC_LGUI);
//       tap_code(KC_LEFT);
//       unregister_code(KC_LGUI);
//       for (int i = 0; i < 20; i++) tap_code(KC_RGHT);
// #endif
//       break;
//     case SG_J|SG_K|SG_B:
// #ifdef NAGINATA_EDIT_WIN
//       tap_code(KC_HOME);
//       for (int i = 0; i < 30; i++) tap_code(KC_RGHT);
// #endif
// #ifdef NAGINATA_EDIT_MAC
//       register_code(KC_LGUI);
//       tap_code(KC_LEFT);
//       unregister_code(KC_LGUI);
//       for (int i = 0; i < 30; i++) tap_code(KC_RGHT);
// #endif
//       break;
// #ifdef NAGINATA_EDIT_WIN
//     case SG_C|SG_V|SG_P:
//       send_unicode_hex_string("FF5C");
//       tap_code(KC_ENT);
//       tap_code(KC_END);
//       send_unicode_hex_string("300A 300B");
//       tap_code(KC_ENT);
//       tap_code(KC_LEFT);
//       break;
//     case SG_C|SG_V|SG_Y:
//       send_unicode_hex_string("300D");
//       tap_code(KC_ENT);
//       tap_code(KC_ENT);
//       tap_code(KC_SPC);
//       break;
//     case SG_C|SG_V|SG_H:
//       send_unicode_hex_string("300D");
//       tap_code(KC_ENT);
//       tap_code(KC_ENT);
//       send_unicode_hex_string("300C");
//       tap_code(KC_ENT);
//       break;
//     case SG_C|SG_V|SG_N:
//       send_unicode_hex_string("300D");
//       tap_code(KC_ENT);
//       tap_code(KC_ENT);
//       break;
// #endif
    default:
      // キーから仮名に変換して出力する。
      // 同時押しの場合 ngmapに定義されている
      // 順序つき
      // #ifdef NAGINATA_JDOUJI // 新下駄
      // for (int i = 0; i < sizeof ngmapo / sizeof bngmapo; i++) {
      //   memcpy_P(&bngmapo, &ngmapo[i], sizeof(bngmapo));
      //   if (ninputs[0] == bngmapo.key[0] && ninputs[1] == bngmapo.key[1] && ninputs[2] == bngmapo.key[2]) {
      //     send_string(bngmapo.kana);
      //     naginata_clear();
      //     return;
      //   }
      // }
      // #endif
      // 順序なし
      if(is_shingeta){
        for (int i = 0; i < sizeof ngmap / sizeof bngmap; i++) {
            memcpy_P(&bngmap, &ngmap[i], sizeof(bngmap));
            if (keycomb == bngmap.key) {
            send_string(bngmap.kana);
            naginata_clear();
            return;
            }
        }
      }else{
        for (int i = 0; i < sizeof ngmap2 / sizeof bngmap; i++) {
            memcpy_P(&bngmap, &ngmap2[i], sizeof(bngmap));
            if (keycomb == bngmap.key) {
            send_string(bngmap.kana);
            naginata_clear();
            return;
            }
        }
      }
      // // 順序なしロング // 新下駄
      // for (int i = 0; i < sizeof ngmapl / sizeof bngmapl; i++) {
      //   memcpy_P(&bngmapl, &ngmapl[i], sizeof(bngmapl));
      //   if (keycomb == bngmapl.key) {
      //     send_string(bngmapl.kana);
      //     naginata_clear();
      //     return;
      //   }
      // }
      // 順序なしUNICODE // 新下駄
      // for (int i = 0; i < sizeof ngmapu / sizeof bngmapu; i++) {
      //   memcpy_P(&bngmapu, &ngmapu[i], sizeof(bngmapu));
      //   if (keycomb == bngmapu.key) {
      //     send_unicode_hex_string(bngmapu.kana);
      //     // tap_code(KC_ENT);
      //     naginata_clear();
      //     return;
      //   }
      // }
      // 連続押しの場合 ngmapに定義されていない
      if(is_shingeta){
        for (int j = 0; j < ng_chrcount; j++) {
            skey = ng_key[ninputs[j] - NG_Q];
            // if ((keycomb & SG_SHFT) > 0) skey |= SG_SHFT; // シフトキー状態を反映 // 新下駄
            for (int i = 0; i < sizeof ngmap / sizeof bngmap; i++) {
                memcpy_P(&bngmap, &ngmap[i], sizeof(bngmap));
                if (skey == bngmap.key) {
                    send_string(bngmap.kana);
                    break;
                }
            }
        }
      }else{
        for (int j = 0; j < ng_chrcount; j++) {
            skey = ng_key[ninputs[j] - NG_Q];
            // if ((keycomb & SG_SHFT) > 0) skey |= SG_SHFT; // シフトキー状態を反映 // 新下駄
            for (int i = 0; i < sizeof ngmap2 / sizeof bngmap; i++) {
                memcpy_P(&bngmap, &ngmap2[i], sizeof(bngmap));
                if (skey == bngmap.key) {
                    send_string(bngmap.kana);
                    break;
                }
            }
        }
      }
  }

  naginata_clear(); // バッファを空にする
}

// バッファをクリアする
void naginata_clear(void) {
  for (int i = 0; i < NGBUFFER; i++) {
    ninputs[i] = 0;
  }
  ng_chrcount = 0;
}

// 入力モードか編集モードかを確認する
void naginata_mode(uint16_t keycode, keyrecord_t *record) {
  if (!is_naginata) return;

  // modifierが押されたらレイヤーをオフ
  switch (keycode) {
    case KC_LCTL:
    case KC_LSFT:
    case KC_LALT:
    case KC_LGUI:
    case KC_RCTL:
    case KC_RSFT:
    case KC_RALT:
    case KC_RGUI:
      if (record->event.pressed) {
        n_modifier++;
        layer_off(naginata_layer);
      } else {
        n_modifier--;
        if (n_modifier == 0) {
          layer_on(naginata_layer);
        }
      }
      break;
  }

}

// 薙刀式の入力処理
bool process_naginata(uint16_t keycode, keyrecord_t *record) {
  // if (!is_naginata || n_modifier > 0) return true;

  if (record->event.pressed) {
    switch (keycode) {
      case NG_Q ... NG_SHFT: // 新下駄
        ninputs[ng_chrcount] = keycode; // キー入力をバッファに貯める
        ng_chrcount++;
        keycomb |= ng_key[keycode - NG_Q]; // キーの重ね合わせ
        if(is_shingeta){
            // 2文字押したら処理を開始 // 新下駄
            if (ng_chrcount > 1) { // 新下駄
                naginata_type();
            }
        }
        return false;
        break;
    }
  } else { // key release
    switch (keycode) {
      case NG_Q ... NG_SHFT: // 新下駄
        // 3文字入力していなくても、どれかキーを離したら処理を開始する
        if (ng_chrcount > 0) {
          naginata_type();
        }
        keycomb &= ~ng_key[keycode - NG_Q]; // キーの重ね合わせ
        return false;
        break;
    }
  }
  return true;
}


// END OF naginata.c

NGKEYS naginata_keys;

// clang-format off
enum layers{
  _QWERTY,
  WIN_BASE,
  MAC_FN1,
  WIN_FN1,
  FN2,
// 薙刀式
  _NAGINATA, // 薙刀式入力レイヤー
// 薙刀式
  _LOWER,
  _RAISE,
  _ADJUST,
};


// // 薙刀式 // 新下駄
// enum combo_events {
//   NAGINATA_ON_CMB,
//   NAGINATA_OFF_CMB,
// };

// #if defined(DQWERTY)
// const uint16_t PROGMEM ngon_combo[] = {KC_H, KC_J, COMBO_END};
// const uint16_t PROGMEM ngoff_combo[] = {KC_F, KC_G, COMBO_END};
// #endif
// #if defined(DEUCALYN)
// const uint16_t PROGMEM ngon_combo[] = {KC_G, KC_T, COMBO_END};
// const uint16_t PROGMEM ngoff_combo[] = {KC_I, KC_U, COMBO_END};
// #endif
// #if defined(DWORKMAN)
// const uint16_t PROGMEM ngon_combo[] = {KC_Y, KC_N, COMBO_END};
// const uint16_t PROGMEM ngoff_combo[] = {KC_T, KC_G, COMBO_END};
// #endif

// combo_t key_combos[COMBO_COUNT] = {
//   [NAGINATA_ON_CMB] = COMBO_ACTION(ngon_combo),
//   [NAGINATA_OFF_CMB] = COMBO_ACTION(ngoff_combo),
// };

// // IME ONのcombo
// void process_combo_event(uint8_t combo_index, bool pressed) {
//   switch(combo_index) {
//     case NAGINATA_ON_CMB:
//       if (pressed) {
//         naginata_on();
//       }
//       break;
//     case NAGINATA_OFF_CMB:
//       if (pressed) {
//         naginata_off();
//       }
//       break;
//   }
// }
// // 薙刀式

enum custom_keycodes {
  QWERTY = NG_SAFE_RANGE,
  EISU,
  LOWER,
  RAISE,
  ADJUST,
  BACKLIT,
  KANA2,
  UNDGL,
  RGBRST,
  SWSGT // 新下駄 or 薙刀式のトグル
};

#define CTLTB CTL_T(KC_TAB)
#define GUITB GUI_T(KC_TAB)
#define ABLS    LALT(KC_BSLS)
#define CMDENT  CMD_T(KC_ENT)
#define SFTSPC  LSFT_T(KC_SPC)
#define CTLSPC  CTL_T(KC_SPC)
#define ALTSPC  ALT_T(KC_SPC)
#define ALTENT  ALT_T(KC_ENT)
#define CTLBS   CTL_T(KC_BSPC)
#define CTLENT  CTL_T(KC_ENT)

#define MO_MACB MO(_QWERTY)
#define MO_WINB MO(WIN_BASE)

#define TO_MACB TO(_QWERTY)
#define TO_WINB TO(WIN_BASE)


// mix above for K3 Pro

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
//     [MAC_BASE] = LAYOUT(
//         KC_ESC,   KC_BRID,  KC_BRIU,  KC_MCTL,  KC_LPAD,  BL_DOWN,  BL_UP,    KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  KC_SNAP,  KC_DEL,   BL_STEP,
//         KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_INT3,  KC_BSPC,  KC_PGUP,
//         KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,                      KC_PGDN,
//         KC_LCTL,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_NUHS,  KC_ENT,             KC_HOME,
//         KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_INT1,  KC_RSFT,            KC_END,
//         KC_LCTL,  KC_LOPTN, KC_LCMMD, KC_LNG2,                      KC_SPC,                       KC_LNG1,  KC_RCMMD,MO(MAC_FN),KC_LEFT,  KC_UP,    KC_DOWN,  KC_RGHT),     

//    [_QWERTY] = LAYOUT_69_ansi(
//        KC_ESC,   KC_BRID,  KC_BRIU,  KC_MCTL,  KC_LPAD,  BL_DOWN,  BL_UP,    KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  KC_SNAP,  KC_DEL,   BL_STEP,
//        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_INT3,  KC_BSPC,  KC_PGUP,
//        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,                      KC_PGDN,
//        KC_LCTL,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_NUHS,  KC_ENT,             KC_HOME,
//        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_INT1,  KC_RSFT,            KC_END,
//        KC_LCTL,  KC_LOPTN, KC_LCMMD, KC_LNG2,                      KC_SPC,                       KC_LNG1,  KC_RCMMD,MO(MAC_FN),KC_LEFT,  KC_UP,    KC_DOWN,  KC_RGHT),

    [_QWERTY] = LAYOUT_69_ansi(
        KC_ESC,  KC_1,     KC_2,     KC_3,    KC_4,    KC_5,    KC_6,     KC_7,    KC_8,    KC_9,    KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,          KC_INS,
        KC_TAB,  KC_Q,     KC_W,     KC_E,    KC_R,    KC_T,    KC_Y,     KC_U,    KC_I,    KC_O,    KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,          KC_DEL,
        KC_CAPS, KC_A,     KC_S,     KC_D,    KC_F,    KC_G,              KC_H,    KC_J,    KC_K,    KC_L,     KC_SCLN,  KC_QUOT,  KC_ENT,           KC_HOME,
        KC_LSFT,           KC_Z,     KC_X,    KC_C,    KC_V,    KC_B,     KC_B,    KC_N,    KC_M,    KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT, KC_UP,
        KC_LCTL, KC_LOPTN, KC_LCMMD,          KC_SPC,           MO(MAC_FN1), MO(FN2),       KC_SPC,            KC_RCMMD,           KC_LEFT, KC_DOWN, KC_RGHT),
    

    // WORKAROUND: just define as empty for test
    [_LOWER] = LAYOUT_69_ansi(
        _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______),

    [_RAISE] = LAYOUT_69_ansi(
        _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______),

    [_ADJUST] = LAYOUT_69_ansi(
        _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______),



// _NAGINATA
//   +-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+
//   |  __   | NG_Q  | NG_W  | NG_E  | NG_R  | NG_T  |       |       | NG_Y  | NG_U  | NG_I  | NG_O  | NG_P  |  __   |
//   +-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+
//   |  __   | NG_A  | NG_S  | NG_D  | NG_F  | NG_G  |       |       | NG_H  | NG_J  | NG_K  | NG_L  |NG_SCLN|  __   |
//   +-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+
//   |  __   | NG_Z  | NG_X  | NG_C  | NG_V  | NG_B  |       |       | NG_N  | NG_M  |NG_COMM|NG_DOT |NG_SLSH|  __   |
//   +-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+
//   |       |       |       |       | LOWER |NG_SHFT|  __   |  __   |NG_SHFT| RAISE |       |       |       |       |
//   +-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+
//

//     [MAC_BASE] = LAYOUT(
//         KC_ESC,   KC_BRID,  KC_BRIU,  KC_MCTL,  KC_LPAD,  BL_DOWN,  BL_UP,    KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  KC_SNAP,  KC_DEL,   BL_STEP,
//         KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_INT3,  KC_BSPC,  KC_PGUP,
//         KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,                      KC_PGDN,
//         KC_LCTL,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_NUHS,  KC_ENT,             KC_HOME,
//         KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_INT1,  KC_RSFT,            KC_END,
//         KC_LCTL,  KC_LOPTN, KC_LCMMD, KC_LNG2,                      KC_SPC,                       KC_LNG1,  KC_RCMMD,MO(MAC_FN),KC_LEFT,  KC_UP,    KC_DOWN,  KC_RGHT),  
//    [MAC_BASE] = LAYOUT_69_ansi(
//        KC_ESC,  KC_1,     KC_2,     KC_3,    KC_4,    KC_5,    KC_6,     KC_7,    KC_8,    KC_9,    KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,          KC_INS,
//        KC_TAB,  KC_Q,     KC_W,     KC_E,    KC_R,    KC_T,    KC_Y,     KC_U,    KC_I,    KC_O,    KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,          KC_DEL,
//        KC_CAPS, KC_A,     KC_S,     KC_D,    KC_F,    KC_G,              KC_H,    KC_J,    KC_K,    KC_L,     KC_SCLN,  KC_QUOT,  KC_ENT,           KC_HOME,
//        KC_LSFT,           KC_Z,     KC_X,    KC_C,    KC_V,    KC_B,     KC_B,    KC_N,    KC_M,    KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT, KC_UP,
//        KC_LCTL, KC_LOPTN, KC_LCMMD,          KC_SPC,           MO(MAC_FN1), MO(FN2),       KC_SPC,            KC_RCMMD,           KC_LEFT, KC_DOWN, KC_RGHT),

    [_NAGINATA] = LAYOUT_69_ansi(
        _______, NG_1, NG_2, NG_3, NG_4, NG_5, NG_6, NG_7, NG_8,    NG_9,   NG_0,    NG_MINS, _______, _______, _______,
        _______, NG_Q, NG_W, NG_E, NG_R, NG_T, NG_Y, NG_U, NG_I,    NG_O,   NG_P,    NG_X1,   _______, _______, _______,
        _______, NG_A, NG_S, NG_D, NG_F, NG_G, NG_H, NG_J, NG_K,    NG_L,   NG_SCLN, KC_BSPC, _______, _______,
        _______, NG_Z, NG_X, NG_C, NG_V, NG_B, NG_B, NG_N, NG_M, NG_COMM, NG_DOT, NG_SLSH, _______, _______,
        KC_LCTL, KC_LOPTN, KC_LCMMD,          KC_SPC,           MO(MAC_FN1), MO(FN2),       KC_SPC,            KC_RCMMD,           KC_LEFT, KC_DOWN, KC_RGHT),

    [MAC_FN1] = LAYOUT_69_ansi(
        KC_GRV,  KC_BRID,  KC_BRIU,  KC_MCTL, KC_LPAD, BL_DOWN, BL_UP,    KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE,  KC_VOLD,  KC_VOLU,  _______,          _______,
        _______, BT_HST1,  BT_HST2,  BT_HST3, _______, _______, _______,  _______, _______, _______, _______,  KC_INS,   KC_PGUP,  _______,          _______,
        BL_TOGG, BL_STEP,  BL_UP,    _______, _______, _______,           _______, _______, _______, KC_SNAP,  KC_PGDN,  KC_END,   _______,          _______,
        _______,           _______,  BL_DOWN, _______, _______, _______,  _______, NK_TOGG, _______, _______,  _______,  _______,  _______, _______,
        _______, _______,  _______,           _______,          _______,  _______,          _______,           _______,            _______, _______, _______),

    [WIN_BASE] = LAYOUT_69_ansi(
        KC_ESC,  KC_1,     KC_2,     KC_3,    KC_4,    KC_5,    KC_6,     KC_7,    KC_8,    KC_9,    KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,          KC_INS,
        KC_TAB,  KC_Q,     KC_W,     KC_E,    KC_R,    KC_T,    KC_Y,     KC_U,    KC_I,    KC_O,    KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,          KC_DEL,
        KC_CAPS, KC_A,     KC_S,     KC_D,    KC_F,    KC_G,              KC_H,    KC_J,    KC_K,    KC_L,     KC_SCLN,  KC_QUOT,  KC_ENT,           KC_HOME,
        KC_LSFT,           KC_Z,     KC_X,    KC_C,    KC_V,    KC_B,     KC_B,    KC_N,    KC_M,    KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT, KC_UP,
        KC_LCTL, KC_LWIN,  KC_LALT,           KC_SPC,           MO(WIN_FN1), MO(FN2),       KC_SPC,            KC_RALT,            KC_LEFT, KC_DOWN, KC_RGHT),

    [WIN_FN1] = LAYOUT_69_ansi(
        KC_GRV,  KC_BRID,  KC_BRIU,  KC_TASK, KC_FILE, BL_DOWN, BL_UP,    KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE,  KC_VOLD,  KC_VOLU,  _______,          _______,
        _______, BT_HST1,  BT_HST2,  BT_HST3, _______, _______, _______,  _______, _______, KC_APP,  KC_SCRL,  KC_INS,   KC_PGUP,  _______,          _______,
        BL_TOGG, BL_STEP,  BL_UP,    _______, _______, _______,           _______, _______, _______, KC_PSCR,  KC_PGDN,  KC_END,   _______,          _______,
        _______,           _______,  BL_DOWN, _______, _______, _______,  _______, NK_TOGG, _______, _______,  _______,  _______,  _______, _______,
        _______, _______,  _______,           _______,          _______,  _______,          _______,           _______,            _______, _______, _______),

    [FN2] = LAYOUT_69_ansi(
        KC_TILD, KC_F1,    KC_F2,    KC_F3,   KC_F4,   KC_F5,   KC_F6,    KC_F7,   KC_F8,   KC_F9,   KC_F10,   KC_F11,   KC_F12,   _______,          _______,
        _______, _______,  _______,  _______, _______, _______, _______,  _______, _______, _______, _______,  _______,  _______,  _______,          _______,
        _______, _______,  _______,  _______, _______, _______,           _______, _______, _______, _______,  _______,  _______,  _______,          _______,
        _______,           _______,  _______, _______, _______, BAT_LVL,  BAT_LVL, _______, _______, _______,  _______,  _______,  _______, _______,
        _______, _______,  _______,           _______,          _______,  _______,          _______,           _______,            _______, _______, _______)

};

int RGB_current_mode;

void persistent_default_layer_set(uint16_t default_layer) {
  eeconfig_update_default_layer(default_layer);
  default_layer_set(default_layer);
}

// Setting ADJUST layer RGB back to default
void update_tri_layer_RGB(uint8_t layer1, uint8_t layer2, uint8_t layer3) {
  if (IS_LAYER_ON(layer1) && IS_LAYER_ON(layer2)) {
    layer_on(layer3);
  } else {
    layer_off(layer3);
  }
}

void matrix_init_user(void) {
  // 薙刀式 // 新下駄
  set_naginata(_NAGINATA);
  // #ifdef NAGINATA_EDIT_MAC
  // set_unicode_input_mode(UC_OSX);
  // #endif
  // #ifdef NAGINATA_EDIT_WIN
  // set_unicode_input_mode(UC_WINC);
  // #endif
  // 薙刀式
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (record->event.pressed) {
    // set_timelog();
  }

  switch (keycode) {
    case SWSGT: // toggle is_shingeta
        if (record->event.pressed) {
            is_shingeta = !is_shingeta;
        }
        return false;
        break;
    case QWERTY:
      if (record->event.pressed) {
        persistent_default_layer_set(1UL<<_QWERTY);
      }
      return false;
      break;
    case KC_HOME: // EISU:
      if (record->event.pressed) {
        // 薙刀式
        naginata_off();
      }
      return false;
      break;
    case KC_DEL: // KANA
      if (record->event.pressed) {
        // 薙刀式
        naginata_on();
      }
      return false;
      break;
    case ADJUST:
      if (record->event.pressed) {
        layer_on(_ADJUST);
      } else {
        layer_off(_ADJUST);
      }
      return false;
      break;
    case LOWER:
      if (record->event.pressed) {
        layer_on(_LOWER);
      } else {
        layer_off(_LOWER);
      }
      update_tri_layer(_LOWER, _RAISE, _ADJUST);
      return false;
      break;
    case RAISE:
      if (record->event.pressed) {
        layer_on(_RAISE);
      } else {
        layer_off(_RAISE);
      }
      update_tri_layer(_LOWER, _RAISE, _ADJUST);
      return false;
      break;
  }

  // 薙刀式
  bool a = true;
  if (naginata_state()) {
    naginata_mode(keycode, record);
    a = process_naginata(keycode, record);
  }
  if (a == false) return false;
  // 薙刀式

  return true;
}
