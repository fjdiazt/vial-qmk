// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "qmk_settings.h"

#if SILAKKA54_KEYPRESS_LED_FEEDBACK
#    define CAPS_LOCK_LED_LAYER 11
#    define KEYPRESS_LED_FIRST_LAYER 7
#else
#    define CAPS_LOCK_LED_LAYER 7
#endif

enum custom_keycodes {
    LYRLED = QK_KB_0,
    TYPLED
};

const rgblight_segment_t PROGMEM layer_1_led[] = RGBLIGHT_LAYER_SEGMENTS({0, 1, 85, 255, 80});
const rgblight_segment_t PROGMEM layer_2_led[] = RGBLIGHT_LAYER_SEGMENTS({0, 1, 191, 255, 80});
const rgblight_segment_t PROGMEM layer_3_led[] = RGBLIGHT_LAYER_SEGMENTS({0, 1, 128, 255, 80});
const rgblight_segment_t PROGMEM layer_4_led[] = RGBLIGHT_LAYER_SEGMENTS({0, 1, 43, 255, 80});
const rgblight_segment_t PROGMEM layer_5_led[] = RGBLIGHT_LAYER_SEGMENTS({0, 1, 170, 255, 80});
const rgblight_segment_t PROGMEM layer_6_led[] = RGBLIGHT_LAYER_SEGMENTS({0, 1, 21, 255, 80});
const rgblight_segment_t PROGMEM layer_7_led[] = RGBLIGHT_LAYER_SEGMENTS({0, 1, 213, 255, 80});
#if SILAKKA54_KEYPRESS_LED_FEEDBACK
const rgblight_segment_t PROGMEM keypress_1_led[] = RGBLIGHT_LAYER_SEGMENTS({0, 1, 234, 128, 70});
const rgblight_segment_t PROGMEM keypress_2_led[] = RGBLIGHT_LAYER_SEGMENTS({0, 1, 64, 255, 70});
const rgblight_segment_t PROGMEM keypress_3_led[] = RGBLIGHT_LAYER_SEGMENTS({0, 1, 132, 102, 70});
const rgblight_segment_t PROGMEM keypress_4_led[] = RGBLIGHT_LAYER_SEGMENTS({0, 1, 11, 176, 70});
#endif
const rgblight_segment_t PROGMEM caps_lock_led[] = RGBLIGHT_LAYER_SEGMENTS({0, 1, 0, 255, 100});

const rgblight_segment_t *const PROGMEM rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    layer_1_led,
    layer_2_led,
    layer_3_led,
    layer_4_led,
    layer_5_led,
    layer_6_led,
    layer_7_led,
#if SILAKKA54_KEYPRESS_LED_FEEDBACK
    keypress_1_led,
    keypress_2_led,
    keypress_3_led,
    keypress_4_led,
#endif
    caps_lock_led
);

#if SILAKKA54_KEYPRESS_LED_FEEDBACK
static uint8_t keypress_feedback_seed;
#endif
static bool layer_indicator_enabled = true;
static bool keypress_feedback_enabled = true;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT(
        KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                               KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                               KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSPC,
        KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                               KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                               KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
                                            KC_LGUI, MO(1), KC_SPC,           KC_ENT,  KC_RCTL,  KC_RALT
    ),
    [1] = LAYOUT(
        KC_GRV,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                              KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
        KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                            KC_PGUP, KC_PGDN, KC_HOME, KC_END,  KC_DEL,  KC_F12,
        KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                            KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_LBRC, KC_RBRC,
        KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                            KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                                            KC_TRNS, KC_TRNS, KC_TRNS,           KC_TRNS,  KC_TRNS,  KC_TRNS
    )
};

static bool is_home_row_flow_tap_disabled_key(uint16_t keycode) {
    switch (get_tap_keycode(keycode)) {
        case KC_D:
        case KC_F:
        case KC_J:
        case KC_K:
            return true;
    }

    return false;
}

static bool is_home_row_ctrl_tap_key(uint16_t keycode) {
    switch (get_tap_keycode(keycode)) {
        case KC_D:
        case KC_K:
            return true;
    }

    return false;
}

static bool is_thumb_key_position(keyrecord_t *record) {
    switch (record->event.key.row) {
        case 4:
            return record->event.key.col >= 3 && record->event.key.col <= 5;
        case 9:
            return record->event.key.col >= 3 && record->event.key.col <= 5;
    }

    return false;
}

uint16_t get_flow_tap_term(uint16_t keycode, keyrecord_t *record, uint16_t prev_keycode) {
    if (is_home_row_flow_tap_disabled_key(keycode)) {
        return 0;
    }

    if (is_flow_tap_key(keycode) && is_flow_tap_key(prev_keycode)) {
#ifdef QMK_SETTINGS
        return QS.flow_tap_term;
#else
        return FLOW_TAP_TERM;
#endif
    }

    return 0;
}

bool get_chordal_hold(uint16_t tap_hold_keycode, keyrecord_t *tap_hold_record, uint16_t other_keycode, keyrecord_t *other_record) {
#ifdef QMK_SETTINGS
    if (!QS_tapping_chordal_hold) {
        return true;
    }
#endif

    if (is_thumb_key_position(tap_hold_record)) {
        return true;
    }

    if (is_home_row_ctrl_tap_key(tap_hold_keycode)) {
        return true;
    }

    return get_chordal_hold_default(tap_hold_record, other_record);
}

bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    if (is_home_row_flow_tap_disabled_key(keycode)) {
        return true;
    }

#ifdef QMK_SETTINGS
    return QS_tapping_permissive_hold;
#else
    return false;
#endif
}

void keyboard_post_init_user(void) {
    rgblight_layers = rgb_layers;
    rgblight_enable_noeeprom();
    rgblight_sethsv_noeeprom(HSV_OFF);
}

bool led_update_user(led_t led_state) {
    rgblight_set_layer_state(CAPS_LOCK_LED_LAYER, led_state.caps_lock);
    return true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LYRLED:
            if (record->event.pressed) {
                layer_indicator_enabled = !layer_indicator_enabled;
                layer_state_set_user(layer_state);
            }
            return false;
        case TYPLED:
#if SILAKKA54_KEYPRESS_LED_FEEDBACK
            if (record->event.pressed) {
                keypress_feedback_enabled = !keypress_feedback_enabled;
                rgblight_unblink_all_but_layer(CAPS_LOCK_LED_LAYER);
            }
#endif
            return false;
    }

#if SILAKKA54_KEYPRESS_LED_FEEDBACK
    if (record->event.pressed) {
        if (!keypress_feedback_enabled) {
            return true;
        }

        keypress_feedback_seed = (keypress_feedback_seed * 17) + (uint8_t)timer_read() + (uint8_t)keycode;
        rgblight_unblink_all_but_layer(CAPS_LOCK_LED_LAYER);
        rgblight_blink_layer(KEYPRESS_LED_FIRST_LAYER + (keypress_feedback_seed & 0x03), 60);
    }
#endif

    return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    for (uint8_t i = 0; i < 7; i++) {
        rgblight_set_layer_state(i, false);
    }

    if (!layer_indicator_enabled) {
        return state;
    }

    switch (get_highest_layer(state)) {
        case 1:
            rgblight_set_layer_state(0, true);
            break;
        case 2:
            rgblight_set_layer_state(1, true);
            break;
        case 3:
            rgblight_set_layer_state(2, true);
            break;
        case 4:
            rgblight_set_layer_state(3, true);
            break;
        case 5:
            rgblight_set_layer_state(4, true);
            break;
        case 6:
            rgblight_set_layer_state(5, true);
            break;
        case 7:
            rgblight_set_layer_state(6, true);
            break;
    }

    return state;
}
