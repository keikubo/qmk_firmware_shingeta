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

// clang-format off
enum layers{
  MAC_BASE,
  MAC_FN,
  WIN_BASE,
  WIN_FN,
  MAC_ASTARTE,
  WIN_ASTARTE,
  MAC_ASTARTE_FN,
  WIN_ASTARTE_FN,
};

#define MO_MACB MO(MAC_BASE)
#define MO_WINB MO(WIN_BASE)
#define MO_MAST MO(MAC_ASTARTE)
#define MO_WAST MO(WIN_ASTARTE)
#define MO_MAFN MO(MAC_ASTARTE_FN)
#define MO_WIFN MO(WIN_ASTARTE_FN)

#define TO_MACB TO(MAC_BASE)
#define TO_WINB TO(WIN_BASE)
#define TO_MAST TO(MAC_ASTARTE)
#define TO_WAST TO(WIN_ASTARTE)
#define TO_MAFN TO(MAC_ASTARTE_FN)
#define TO_WIFN TO(WIN_ASTARTE_FN)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [MAC_BASE] = LAYOUT(
        KC_ESC,   KC_BRID,  KC_BRIU,  KC_MCTL,  KC_LPAD,  BL_DOWN,  BL_UP,    KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  KC_SNAP,  KC_DEL,   BL_STEP,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_INT3,  KC_BSPC,  KC_PGUP,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,                      KC_PGDN,
        KC_LCTL,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_NUHS,  KC_ENT,             KC_HOME,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_INT1,  KC_RSFT,            KC_END,
        KC_LCTL,  KC_LOPTN, KC_LCMMD, KC_LNG2,                      KC_SPC,                       KC_LNG1,  KC_RCMMD,MO(MAC_FN),KC_LEFT,  KC_UP,    KC_DOWN,  KC_RGHT),

    [MAC_FN] = LAYOUT(
        _______,  KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   TO_MAST,  _______,  BL_TOGG,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        BL_TOGG,  BL_STEP,  BL_UP,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                      _______,
        _______,  _______,  BL_DOWN,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,            _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,  _______,  _______,  _______,                      _______,                      _______,  _______,  _______,  _______,  _______,  _______,  _______),

    [WIN_BASE] = LAYOUT(
        KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_SNAP,  KC_DEL,   BL_STEP,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_INT3,  KC_BSPC,  KC_PGUP,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,                      KC_PGDN,
        KC_LCTL,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_NUHS,  KC_ENT,             KC_HOME,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_INT1,  KC_RSFT,            KC_END,
        KC_LCTL,  KC_LGUI,  KC_LALT,  KC_INT5,                      KC_SPC,                       KC_INT4,  KC_RALT, MO(WIN_FN),KC_LEFT,  KC_UP,    KC_DOWN,  KC_RGHT),

    [WIN_FN] = LAYOUT(
        _______,  KC_BRID,  KC_BRIU,  KC_TASK,  KC_FILE,  BL_DOWN,  BL_UP,    KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  TO_WAST,  _______,  BL_TOGG,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        BL_TOGG,  BL_STEP,  BL_UP,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                      _______,
        _______,  _______,  BL_DOWN,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,            _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,  _______,  _______,  _______,                      _______,                      _______,  _______,  _______,  _______,  _______,  _______,  _______),

    [MAC_ASTARTE] = LAYOUT(
        KC_ESC,   KC_BRID,  KC_BRIU,  KC_MCTL,  KC_LPAD,  BL_DOWN,  BL_UP,    KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  TO_MACB,  KC_DEL,   BL_STEP,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_INT3,  KC_BSPC,  KC_PGUP,
        KC_TAB,   KC_Q,     KC_X,     KC_U,     KC_Y,     KC_SLSH,  KC_Y,     KC_J,     KC_D,     KC_H,     KC_G,     KC_W,     KC_Z,                         KC_PGDN,
        KC_LCTL,  KC_I,     KC_O,     KC_E,     KC_A,     KC_DOT,   KC_K,     KC_T,     KC_N,     KC_S,     KC_R,     KC_P,     KC_NUHS,  KC_ENT,             KC_HOME,
        KC_LSFT,            KC_QUOT,  KC_MINS,  KC_COMM,  KC_C,     KC_SCLN,  KC_M,     KC_L,     KC_L,     KC_B,     KC_V,     KC_INT1,  KC_RSFT,            KC_END,
        KC_LCTL,  KC_LOPTN, KC_LCMMD, KC_LNG2,                      KC_SPC,                       KC_LNG1,  KC_RCMMD, MO_MAFN,  KC_LEFT,  KC_UP,    KC_DOWN,  KC_RGHT),

    [MAC_ASTARTE_FN] = LAYOUT(
        _______,  KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   TO_MACB,  _______,  BL_TOGG,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        BL_TOGG,  BL_STEP,  BL_UP,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                      _______,
        _______,  _______,  BL_DOWN,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,            _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,  _______,  _______,  _______,                      _______,                      _______,  _______,  _______,  _______,  _______,  _______,  _______),

    [WIN_ASTARTE] = LAYOUT(
        KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   TO_WINB,  KC_DEL,   BL_STEP,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_INT3,  KC_BSPC,  KC_PGUP,
        KC_TAB,   KC_Q,     KC_X,     KC_U,     KC_Y,     KC_SLSH,  KC_Y,     KC_J,     KC_D,     KC_H,     KC_G,     KC_W,     KC_Z,                         KC_PGDN,
        KC_LCTL,  KC_I,     KC_O,     KC_E,     KC_A,     KC_DOT,   KC_K,     KC_T,     KC_N,     KC_S,     KC_R,     KC_P,     KC_NUHS,  KC_ENT,             KC_HOME,
        KC_LSFT,            KC_QUOT,  KC_MINS,  KC_COMM,  KC_C,     KC_SCLN,  KC_M,     KC_L,     KC_L,     KC_B,     KC_V,     KC_INT1,  KC_RSFT,            KC_END,
        KC_LCTL,  KC_LGUI,  KC_LALT,  KC_INT5,                      KC_SPC,                       KC_INT4,  KC_RALT,  MO_WIFN,  KC_LEFT,  KC_UP,    KC_DOWN,  KC_RGHT),

    [WIN_ASTARTE_FN] = LAYOUT(
        _______,  KC_BRID,  KC_BRIU,  KC_TASK,  KC_FILE,  BL_DOWN,  BL_UP,    KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  TO_WINB,  _______,  BL_TOGG,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        BL_TOGG,  BL_STEP,  BL_UP,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                      _______,
        _______,  _______,  BL_DOWN,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,            _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,  _______,  _______,  _______,                      _______,                      _______,  _______,  _______,  _______,  _______,  _______,  _______),
};