/* Copyright 2019 Thomas Baart <thomas@splitkb.com>
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

uint16_t copy_paste_timer;

enum layers { _QWERTY = 0, _COLEMAK_DH, _NAV, _SYM, _FUNCTION, _ADJUST };

enum custom_keycodes { KC_CCCV = SAFE_RANGE, KC_LEAD };

// Aliases for readability
#define QWERTY DF(_QWERTY)
#define COLEMAK DF(_COLEMAK_DH)

#define SYM MO(_SYM)
#define NAV MO(_NAV)
#define FKEYS MO(_FUNCTION)
#define ADJUST MO(_ADJUST)

#define LINUX
#ifdef LINUX
#    define _LCTL_T LGUI_T
#    define _RCTL_T RGUI_T
#    define _LGUI_T LCTL_T
#    define _RGUI_T RCTL_T
#else
#    define _LCTL_T LCTL_T
#    define _RCTL_T RCTL_T
#    define _LGUI_T LGUI_T
#    define _RGUI_T RGUI_T
#endif

#define CTL_ESC _LCTL_T(KC_ESC)
#define CMD_TAB _LGUI_T(KC_TAB)
#define CTL_QUOT _RCTL_T(KC_QUOTE)
#define CTL_MINS _RCTL_T(KC_MINUS)
#define GUI_ENT LGUI_T(KC_ENT)

#define SFT_ESC LSFT_T(KC_ESC)
#define CMD_ENT _LGUI_T(KC_ENT)
#define SFT_SPC RSFT_T(KC_SPC)
#define SFT_TAB LSFT_T(KC_TAB)

#define TAB_PREV SCMD(KC_LBRC)
#define TAB_NEXT SCMD(KC_RBRC)

// Combo definitions
enum combo_events {
    BSLSZ_UNDO,
    ZX_REDO,
    MCOMM_DEL_WORD,
    COMMDOT_FDEL_WORD,
    LBRCQUOT_RBRC,
};

const uint16_t PROGMEM undo_combo[] = {KC_BSLS, KC_Z, COMBO_END};
const uint16_t PROGMEM redo_combo[] = {KC_Z, KC_X, COMBO_END};
const uint16_t PROGMEM dwb_combo[]  = {KC_M, KC_COMM, COMBO_END};
const uint16_t PROGMEM dwf_combo[]  = {KC_COMM, KC_DOT, COMBO_END};
const uint16_t PROGMEM rbrc_combo[] = {KC_LBRC, KC_QUOTE, COMBO_END};

combo_t key_combos[COMBO_COUNT] = {[BSLSZ_UNDO] = COMBO_ACTION(undo_combo), [ZX_REDO] = COMBO_ACTION(redo_combo), [MCOMM_DEL_WORD] = COMBO_ACTION(dwb_combo), [COMMDOT_FDEL_WORD] = COMBO_ACTION(dwf_combo), [LBRCQUOT_RBRC] = COMBO_ACTION(rbrc_combo)};

void process_combo_event(uint16_t combo_index, bool pressed) {
    switch (combo_index) {
        case BSLSZ_UNDO:
            if (pressed) {
                tap_code16(LGUI(KC_Z));
            }
            break;
        case ZX_REDO:
            if (pressed) {
                tap_code16(SGUI(KC_Z));
            }
            break;

        case MCOMM_DEL_WORD:
            if (pressed) {
                tap_code16(LALT(KC_BSPC));
            }
            break;
        case COMMDOT_FDEL_WORD:
            if (pressed) {
                tap_code16(LALT(KC_DEL));
            }
            break;
        case LBRCQUOT_RBRC:
            if (pressed) {
                tap_code16(KC_RBRC);
            }
            break;
    }
}

// Home row mods
#define _A_ _LCTL_T(KC_A)
#define _S_ LALT_T(KC_S)
#define _D_ _LGUI_T(KC_D)
#define _F_ LSFT_T(KC_F)

#define _J_ RSFT_T(KC_J)
#define _K_ _RGUI_T(KC_K)
#define _L_ LALT_T(KC_L)
#define _SC_ _RCTL_T(KC_SCLN)

// Colemak homerow keys
#define _cA_ _LCTL_T(KC_A)
#define _cR_ LALT_T(KC_R)
#define _cS_ _LGUI_T(KC_S)
#define _cT_ LSFT_T(KC_T)

#define _cN_ RSFT_T(KC_N)
#define _cE_ _RGUI_T(KC_E)
#define _cI_ LALT_T(KC_I)
#define _cO_ _RCTL_T(KC_O)

// Note: LAlt/Enter (ALT_ENT) is not the same thing as the keyboard shortcut Alt+Enter.
// The notation `mod/tap` denotes a key that activates the modifier `mod` when held down, and
// produces the key `tap` when tapped (i.e. pressed and released).

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*
 * Base Layer: QWERTY
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  Tab   |   Q  |   W  |   E  |   R  |   T  |                              |   Y  |   U  |   I  |   O  |   P  |  [ Å   |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |Ctrl/Esc|   A  |   S  |   D  |   F  |   G  |                              |   H  |   J  |   K  |   L  | ;  : |  ' "   |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |  \ |   |   Z  |   X  |   C  |   V  |   B  | Tab  | CCCV |  |F-keys|Backsp|   N  |   M  | ,  < | . >  | /  ? |  - _   |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |Adjust| Alt  | Nav  | Shift| LGUI |  | RGUI/| Shift| Sym  | Ctrl | AltGr|
 *                        |      |      |      | Tab  | Esc  |  | Enter| Space|      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
[_QWERTY] = LAYOUT(
     KC_TAB  , KC_Q ,  KC_W   ,  KC_E  ,   KC_R ,   KC_T ,                                        KC_Y,   KC_U ,  KC_I ,   KC_O ,  KC_P , KC_LBRC,
     CTL_ESC , KC_A ,   _S_   ,   _D_  ,    _F_ ,   KC_G ,                                        KC_H,    _J_ ,   _K_ ,    _L_ ,KC_SCLN, KC_QUOT,
     KC_BSLS , KC_Z ,  KC_X   ,  KC_C  ,   KC_V ,   KC_B ,KC_CAPS, KC_CCCV,     FKEYS,   KC_BSPC, KC_N,   KC_M ,KC_COMM, KC_DOT ,KC_SLSH, KC_MINS,
                                 ADJUST , GUI_ENT,  NAV,  SFT_SPC, KC_LEAD,     CMD_ENT, SFT_SPC, SYM,  KC_RGUI, KC_RALT
    ),
/*
 * Base Layer: Colemak DH
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  Tab   |   Q  |   W  |   F  |   P  |   B  |                              |   J  |   L  |   U  |   Y  | ;  : |  [ Å   |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |Ctrl/Esc|   A  |   R  |   S  |   T  |   G  |                              |   M  |   N  |   E  |   I  |   O  |  ' "   |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | LShift |   Z  |   X  |   C  |   D  |   V  | [ {  |CapsLk|  |F-keys|  ] } |   K  |   H  | ,  < | . >  | /  ? |  - _   |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |Adjust| Alt  | Nav  | Space| LGUI |  | RGUI | Space| Sym  | Ctrl | AltGr|
 *                        |      |      |      | Shift| Tab  |  | Enter| Shift|      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_COLEMAK_DH] = LAYOUT(
     KC_TAB  , KC_Q ,  KC_W   ,  KC_F  ,   KC_P ,   KC_B ,                                        KC_J,   KC_L ,  KC_U ,   KC_Y ,KC_SCLN, KC_LBRC,
     CTL_ESC , KC_A,  _cR_   ,  _cS_  ,   _cT_ ,   KC_G ,                                         KC_M,   _cN_ ,  _cE_ ,   _cI_ ,  KC_O , CTL_QUOT,
     KC_BSLS , KC_Z ,  KC_X   ,  KC_C  ,   KC_D ,   KC_V , KC_CAPS, KC_CCCV,    FKEYS  , KC_BSPC, KC_K,   KC_H ,KC_COMM, KC_DOT ,KC_SLSH, KC_MINS,
                                 ADJUST, GUI_ENT,   NAV,  SFT_SPC , KC_LEAD,    CMD_ENT, SFT_SPC ,SYM, KC_RCTL, KC_RALT
    ),


/*
 * Nav Layer: Media, navigation
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |      |      |      |      |      |                              | Home | PgDn | PgUp | End  | VolUp| Delete |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        | Ctrl |  Alt | GUI  | Shift|      |                              |   ←  |  ↓   |  ↑   |   →  | VolDn| Insert |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      |      |      |      |      |      |ScLck |  |      |      | Pause|M Prev|M Play|M Next|VolMut| PrtSc  |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_NAV] = LAYOUT(
      _______, _______, _______, _______, _______, _______,                                     KC_HOME, KC_PGUP, KC_PGDN, KC_END,  KC_VOLU, KC_DELETE,
      _______, KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, _______,                                     KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_VOLD, KC_INSERT,
      _______, _______, _______, _______, _______, _______, _______, KC_SLCK, _______, KC_DEL,  KC_PAUSE,KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_PSCR,
                                 _______, _______, _______, _______, _______, _______, KC_BSPC, _______, TAB_PREV, TAB_NEXT
    ),

/*
 * Sym Layer: Numbers and symbols
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |    `   |  1   |  2   |  3   |  4   |  5   |                              |   6  |  7   |  8   |  9   |  0   |   =    |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |    ~   |  !   |  @   |  #   |  $   |  %   |                              |   ^  |  &   |  *   |  (   |  )   |   +    |
 * |--------+------+------+------+------+------+-------------.  ,------+-------------+------+------+------+------+--------|
 * |    |   |   \  |  :   |  ;   |  -   |  {   |  [   |      |  |      |  ]   |   }  |  _   |  ,   |  .   |  /   |   ?    |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_SYM] = LAYOUT(
      KC_GRV ,   KC_1 ,   KC_2 ,   KC_3 ,   KC_4 ,   KC_5 ,                                       KC_6 ,   KC_7 ,   KC_8 ,   KC_9 ,   KC_0 , KC_EQL ,
     KC_TILD , KC_EXLM,  KC_AT , KC_HASH,  KC_DLR, KC_PERC,                                     KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_PLUS,
     KC_PIPE , KC_BSLS, KC_COLN, KC_SCLN, KC_MINS, S(KC_LBRC), KC_LBRC, _______, _______, KC_RBRC, S(KC_RBRC), KC_UNDS, KC_COMM,  KC_DOT, KC_SLSH, KC_QUES,
                                 _______, _______, KC_BSPC, KC_SPC,  _______, _______, _______, _______, _______, _______
    ),

/*
 * Function Layer: Function keys
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |  F9  | F10  | F11  | F12  |      |                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |  F5  |  F6  |  F7  |  F8  |      |                              |      | Shift| Ctrl |  Alt |  GUI |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |  F1  |  F2  |  F3  |  F4  |      |      |      |  |      |      |      |      |      |      |      |        |
 * `--------+------+------+--------------------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_FUNCTION] = LAYOUT(
      _______,  KC_F9 ,  KC_F10,  KC_F11,  KC_F12, _______,                                     _______, _______, _______, _______, _______, _______,
      _______,  KC_F5 ,  KC_F6 ,  KC_F7 ,  KC_F8 , _______,                                     _______, KC_RSFT, KC_RGUI, KC_LALT, KC_RCTL, _______,
      _______,  KC_F1 ,  KC_F2 ,  KC_F3 ,  KC_F4 , _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),

/*
 * Adjust Layer: Default layer settings, RGB
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |      |      |QWERTY|      |      |                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |      |      |Colmak|      |      |                              | TOG  | SAI  | HUI  | VAI  | MOD  |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      |      |      |      |      |      |      |  |      |      |      | SAD  | HUD  | VAD  | RMOD |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [_ADJUST] = LAYOUT(
      _______, _______, _______, QWERTY , _______, _______,                                    _______, _______, _______, _______,  _______, _______,
      _______, _______, _______, COLEMAK, _______, _______,                                    RGB_TOG, RGB_SAI, RGB_HUI, RGB_VAI,  RGB_MOD, _______,
      _______, _______, _______, _______, _______, _______,_______, _______, _______, _______, _______, RGB_SAD, RGB_HUD, RGB_VAD, RGB_RMOD, _______,
                                 _______, _______, _______,_______, _______, _______, _______, _______, _______, _______
    ),

// /*
//  * Layer template
//  *
//  * ,-------------------------------------------.                              ,-------------------------------------------.
//  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
//  * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        `----------------------------------'  `----------------------------------'
//  */
//     [_LAYERINDEX] = LAYOUT(
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//                                  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
//     ),
};
// clang-format on

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_CCCV:  // One key copy/paste
            if (record->event.pressed) {
                copy_paste_timer = timer_read();
            } else {
                if (timer_elapsed(copy_paste_timer) > TAPPING_TERM) {  // Hold, copy
                    tap_code16(LCTL(KC_C));
                } else {  // Tap, paste
                    tap_code16(LCTL(KC_V));
                }
            }
            break;
        case KC_LEAD:  // localleader
            if (record->event.pressed) {
                SEND_STRING(SS_TAP(X_SPC) SS_DELAY(10) SS_TAP(X_M));
            }
            break;
    }
    return true;
}

#ifdef OLED_DRIVER_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) { return OLED_ROTATION_180; }

static void render_kyria_logo(void) {
    static const char PROGMEM kyria_logo[] = {0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   128, 128, 192, 224, 240, 112, 120, 56,  60,  28, 30, 14, 14, 14, 7,  7,   7,   7,   7,  7, 7,   7,   7, 7,  7,   7,   7,   7,  7,  7,  14, 14, 14,  30,  28,  60,  56,  120, 112, 240, 224, 192, 128, 128, 0,   0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,   0,   0,   0,   192, 224, 240, 124, 62,  31,  15,  7,   3,   1,   128, 192, 224, 240, 120, 56,  60, 28, 30, 14, 14, 7,   7,   135, 231, 127, 31,  255, 255, 31,  127, 231, 135, 7,  7,  14,
                                              14, 30, 28,  60,  56,  120, 240, 224, 192, 128, 1,   3,   7,   15,  31,  62,  124, 240, 224, 192, 0,   0,   0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0, 0,   0,   0, 0,  0,   0,   0,   0,  0,  0,  0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   240, 252, 255, 31,  7,   1,  0,   0,   192, 240, 252, 254, 255, 247, 243, 177, 176, 48,  48,  48,  48,  48,  48,  48,  120, 254, 135, 1,   0,   0,   255, 255, 0,   0,   1,   135, 254, 120, 48,  48,  48, 48, 48,  48,  48,  176, 177, 243, 247, 255, 254, 252, 240, 192, 0,   0,   1,   7,   31,  255, 252, 240, 0,  0,  0,  0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,
                                              0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0, 0,   0,   0, 0,  0,   0,   0,   0,  0,  0,  0,  0,  255, 255, 255, 0,   0,   0,   0,   0,   254, 255, 255, 1,   1,   7,  30, 120, 225, 129, 131, 131, 134, 134, 140, 140, 152, 152, 177, 183, 254, 248, 224, 255, 255, 224, 248, 254, 183, 177, 152, 152, 140, 140, 134, 134, 131, 131, 129, 225, 120, 30, 7,   1,   1,   255, 255, 254, 0,   0,   0,   0,   0,   255, 255, 255, 0,   0,   0,   0,   255, 255, 0,   0,   192, 192, 48,  48,  0,   0,   240, 240, 0,   0,   0,   0,   0,  0,  240, 240, 0,   0,   240, 240, 192, 192, 48,  48,  48,  48,  192, 192, 0,   0,   48,  48,  243, 243, 0,  0,  0,  0,  0,  0,   48,  48,  48,  48,  48,  48,  192, 192, 0,   0,   0,   0,  0,  0,
                                              0,  0,  255, 255, 255, 0,   0,   0,   0,   0,   127, 255, 255, 128, 128, 224, 120, 30,  135, 129, 193, 193, 97, 97, 49, 49, 25, 25, 141, 237, 127, 31, 7, 255, 255, 7, 31, 127, 237, 141, 25, 25, 49, 49, 97, 97,  193, 193, 129, 135, 30,  120, 224, 128, 128, 255, 255, 127, 0,  0,  0,   0,   0,   255, 255, 255, 0,   0,   0,   0,   63,  63,  3,   3,   12,  12,  48,  48,  0,   0,   0,   0,   51,  51,  51,  51,  51,  51,  15,  15,  0,   0,   63,  63,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   48,  48,  63,  63,  48,  48,  0,   0,   12,  12,  51,  51,  51,  51,  51,  51,  63,  63,  0,   0,   0,   0,   0,   0,   0,   0,  0,  15,  63,  255, 248, 224, 128, 0,   0,   3,   15,  63,  127, 255, 239, 207, 141, 13,  12,  12,  12,  12, 12, 12, 12, 30, 127, 225, 128, 0,   0,   255, 255, 0,   0,   128, 225, 127, 30, 12, 12,
                                              12, 12, 12,  12,  12,  13,  141, 207, 239, 255, 127, 63,  15,  3,   0,   0,   128, 224, 248, 255, 63,  15,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0, 0,   0,   0, 0,  0,   0,   0,   0,  0,  0,  0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   3,   7,   15,  62, 124, 248, 240, 224, 192, 128, 1,   3,   7,   15,  30,  28,  60,  56,  120, 112, 112, 224, 224, 225, 231, 254, 248, 255, 255, 248, 254, 231, 225, 224, 224, 112, 112, 120, 56, 60, 28,  30,  15,  7,   3,   1,   128, 192, 224, 240, 248, 124, 62,  15,  7,   3,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,
                                              0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,   0,   0,   0,  0, 0,   0,   0, 0,  0,   0,   0,   0,  0,  0,  0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   3,   7,   15, 14, 30,  28,  60,  56,  120, 112, 112, 112, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 112, 112, 112, 120, 56,  60,  28,  30,  14,  15,  7,  3,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0};
    oled_write_raw_P(kyria_logo, sizeof(kyria_logo));
}

static void render_qmk_logo(void) {
    static const char PROGMEM qmk_logo[] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0};

    oled_write_P(qmk_logo, false);
}

static void render_status(void) {
    render_qmk_logo();
    oled_write_P(PSTR("Kyria\n\n"), false);

    // Host Keyboard Layer Status
    oled_write_P(PSTR("Layer: "), false);
    switch (get_highest_layer(layer_state | default_layer_state)) {
        case _QWERTY:
            oled_write_P(PSTR("QWERTY\n"), false);
            break;
        case _COLEMAK_DH:
            oled_write_P(PSTR("Colemak-DH\n"), false);
            break;
        case _NAV:
            oled_write_P(PSTR("Nav\n"), false);
            break;
        case _SYM:
            oled_write_P(PSTR("Sym\n"), false);
            break;
        case _FUNCTION:
            oled_write_P(PSTR("Function\n"), false);
            break;
        case _ADJUST:
            oled_write_P(PSTR("Adjust\n"), false);
            break;
        default:
            oled_write_P(PSTR("Undefined\n"), false);
    }

    // Write host Keyboard LED Status to OLEDs
    led_t led_state = host_keyboard_led_state();
    oled_write_P(led_state.num_lock ? PSTR("NUMLCK ") : PSTR("       "), false);
    oled_write_P(led_state.caps_lock ? PSTR("CAPLCK ") : PSTR("       "), false);
    oled_write_P(led_state.scroll_lock ? PSTR("SCRLCK ") : PSTR("       "), false);
}

void oled_task_user(void) {
    if (is_keyboard_master()) {
        render_status();  // Renders the current keyboard state (layer, lock, caps, scroll, etc)
    } else {
        render_kyria_logo();
    }
}
#endif

#ifdef ENCODER_ENABLE
void encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) {
        // Volume control
        if (clockwise) {
            tap_code(KC_VOLU);
        } else {
            tap_code(KC_VOLD);
        }
    } else if (index == 1) {
        // Page up/Page down
        if (clockwise) {
            tap_code(KC_PGDN);
        } else {
            tap_code(KC_PGUP);
        }
    }
}
#endif
