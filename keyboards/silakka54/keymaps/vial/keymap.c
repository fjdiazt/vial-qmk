// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "qmk_settings.h"
#include "transactions.h"

#define CAPS_LOCK_LED_LAYER 7
#define KEYPRESS_LED_DURATION 60
#define SILAKKA54_LED_COUNT 2
#define SILAKKA54_LEFT_LED_INDEX 0
#define SILAKKA54_RIGHT_LED_INDEX 1
#define SILAKKA54_LAYER_LED_VALUE ((uint8_t)(255 * SILAKKA54_LAYER_LED_INTENSITY))
#define SILAKKA54_KEYPRESS_LED_VALUE ((uint8_t)(255 * SILAKKA54_KEYPRESS_LED_INTENSITY))
#define SILAKKA54_CAPS_LED_VALUE ((uint8_t)(255 * SILAKKA54_CAPS_LED_INTENSITY))

enum custom_keycodes {
    LYRLED = QK_KB_0,
    TYPLED
};

const rgblight_segment_t PROGMEM layer_1_led[] = RGBLIGHT_LAYER_SEGMENTS({0, SILAKKA54_LED_COUNT, 85, 255, SILAKKA54_LAYER_LED_VALUE});
const rgblight_segment_t PROGMEM layer_2_led[] = RGBLIGHT_LAYER_SEGMENTS({0, SILAKKA54_LED_COUNT, 191, 255, SILAKKA54_LAYER_LED_VALUE});
const rgblight_segment_t PROGMEM layer_3_led[] = RGBLIGHT_LAYER_SEGMENTS({0, SILAKKA54_LED_COUNT, 128, 255, SILAKKA54_LAYER_LED_VALUE});
const rgblight_segment_t PROGMEM layer_4_led[] = RGBLIGHT_LAYER_SEGMENTS({0, SILAKKA54_LED_COUNT, 43, 255, SILAKKA54_LAYER_LED_VALUE});
const rgblight_segment_t PROGMEM layer_5_led[] = RGBLIGHT_LAYER_SEGMENTS({0, SILAKKA54_LED_COUNT, 170, 255, SILAKKA54_LAYER_LED_VALUE});
const rgblight_segment_t PROGMEM layer_6_led[] = RGBLIGHT_LAYER_SEGMENTS({0, SILAKKA54_LED_COUNT, 21, 255, SILAKKA54_LAYER_LED_VALUE});
const rgblight_segment_t PROGMEM layer_7_led[] = RGBLIGHT_LAYER_SEGMENTS({0, SILAKKA54_LED_COUNT, 213, 255, SILAKKA54_LAYER_LED_VALUE});
const rgblight_segment_t PROGMEM caps_lock_led[] = RGBLIGHT_LAYER_SEGMENTS({0, SILAKKA54_LED_COUNT, 0, 255, SILAKKA54_CAPS_LED_VALUE});

const rgblight_segment_t *const PROGMEM rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    layer_1_led,
    layer_2_led,
    layer_3_led,
    layer_4_led,
    layer_5_led,
    layer_6_led,
    layer_7_led,
    caps_lock_led
);

#if SILAKKA54_KEYPRESS_LED_FEEDBACK
static uint8_t keypress_feedback_seed;
static bool keypress_feedback_active;
static uint16_t keypress_feedback_timer;
#endif
static bool layer_indicator_enabled = true;
static bool keypress_feedback_enabled = true;

bool led_update_user(led_t led_state);
layer_state_t layer_state_set_user(layer_state_t state);

static bool is_left_keypress(keyrecord_t *record) {
    return record->event.key.row < MATRIX_ROWS / 2;
}

static uint8_t local_led_index(void) {
    return is_keyboard_left() ? SILAKKA54_LEFT_LED_INDEX : SILAKKA54_RIGHT_LED_INDEX;
}

static void refresh_indicator_leds(void) {
    rgblight_sethsv_noeeprom(HSV_OFF);
    layer_state_set_user(layer_state);
    led_update_user(host_keyboard_led_state());
}

#if SILAKKA54_KEYPRESS_LED_FEEDBACK
static void show_keypress_feedback(uint8_t hue) {
    rgblight_sethsv_at(hue, 255, SILAKKA54_KEYPRESS_LED_VALUE, local_led_index());
    keypress_feedback_timer = timer_read();
    keypress_feedback_active = true;
}

static void keypress_led_sync(uint8_t initiator2target_buffer_size, const void *initiator2target_buffer, uint8_t target2initiator_buffer_size, void *target2initiator_buffer) {
    show_keypress_feedback(*(const uint8_t *)initiator2target_buffer);
}
#endif

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
#if SILAKKA54_KEYPRESS_LED_FEEDBACK
    transaction_register_rpc(RPC_ID_USER_KEYPRESS_LED, keypress_led_sync);
#endif
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
                refresh_indicator_leds();
            }
            return false;
        case TYPLED:
#if SILAKKA54_KEYPRESS_LED_FEEDBACK
            if (record->event.pressed) {
                keypress_feedback_enabled = !keypress_feedback_enabled;
                keypress_feedback_active = false;
                refresh_indicator_leds();
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
        if (is_left_keypress(record) == is_keyboard_left()) {
            show_keypress_feedback(keypress_feedback_seed);
        } else {
            transaction_rpc_send(RPC_ID_USER_KEYPRESS_LED, sizeof(keypress_feedback_seed), &keypress_feedback_seed);
        }
    }
#endif

    return true;
}

#if SILAKKA54_KEYPRESS_LED_FEEDBACK
void housekeeping_task_user(void) {
    if (keypress_feedback_active && timer_elapsed(keypress_feedback_timer) > KEYPRESS_LED_DURATION) {
        keypress_feedback_active = false;
        refresh_indicator_leds();
    }
}
#endif

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
