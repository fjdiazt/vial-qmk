# fjdiazt Silakka54 Vial fork notes

This fork currently carries custom Silakka54/Vial firmware changes for an RP2040-Zero Silakka54 build with the USB cable plugged into the right half (`MASTER_RIGHT`).

## What changed

- Added a one-pixel WS2812/RGBLIGHT LED indicator on `GP16` for the Silakka54 Vial keymap.
- Configured split RGBLIGHT as one LED per half.
- Added per-layer LED colors for layers 1-7.
- Added caps-lock LED indication.
- Added optional typing/key-press LED feedback that shows a pseudo-random hue on each keypress.
- Added two Vial custom keycodes so the LED behaviors can be toggled from the keyboard after flashing.
- Customized tap-hold behavior for home-row mods.
- Restored 8 dynamic Vial layers for this keymap.
- Added `Miryoku Custom.vil` under `keyboards/silakka54/layouts/` as the working exported layout/profile.

LED support was adapted from EloyEMC's Silakka54 RP2040-Zero LED indicator work: <https://github.com/EloyEMC/Multi-color-LED-indicator-for-Silakka54-with-RP2040-Zero>.

## LED hardware and build flags

Configured in `keyboards/silakka54/keymaps/vial/config.h`:

```c
#define RGBLIGHT_LED_COUNT 2
#define RGBLED_SPLIT { 1, 1 }
#define WS2812_DI_PIN GP16
#define RGBLIGHT_LAYERS
```

`RGBLIGHT_LED_COUNT` is `2` in the actual firmware: one LED on the left half and one LED on the right half. `RGBLED_SPLIT { 1, 1 }` enables split RGBLIGHT synchronization.

Typing feedback is build-time gated:

```c
#define SILAKKA54_KEYPRESS_LED_FEEDBACK 1
```

Set that to `0` before building to compile out the per-keypress feedback feature.

LED brightness is split into 1.0-based intensity multipliers:

```c
#define SILAKKA54_LAYER_LED_INTENSITY 0.31372549
#define SILAKKA54_KEYPRESS_LED_INTENSITY 0.25
#define SILAKKA54_CAPS_LED_INTENSITY 0.39215686
```

Layer and caps defaults match the previous values from this fork: layer `80 / 255`, caps `100 / 255`. Keypress feedback defaults to the old layer value dimmed by 20%: `80 * 0.8 = 64`, or about `0.25` on a `0.0` to `1.0` scale.

## LED colors

Configured in `keyboards/silakka54/keymaps/vial/keymap.c` using `RGBLIGHT_LAYER_SEGMENTS({start, count, hue, sat, val})`.

Layer colors:

| Layer | Color | HSV segment |
| --- | --- | --- |
| 1 | green | `{0, 2, 85, 255, SILAKKA54_LAYER_LED_VALUE}` |
| 2 | purple | `{0, 2, 191, 255, SILAKKA54_LAYER_LED_VALUE}` |
| 3 | cyan | `{0, 2, 128, 255, SILAKKA54_LAYER_LED_VALUE}` |
| 4 | yellow | `{0, 2, 43, 255, SILAKKA54_LAYER_LED_VALUE}` |
| 5 | blue | `{0, 2, 170, 255, SILAKKA54_LAYER_LED_VALUE}` |
| 6 | orange | `{0, 2, 21, 255, SILAKKA54_LAYER_LED_VALUE}` |
| 7 | magenta | `{0, 2, 213, 255, SILAKKA54_LAYER_LED_VALUE}` |
| Caps Lock | red | `{0, 2, 0, 255, SILAKKA54_CAPS_LED_VALUE}` |

Typing feedback shows a pseudo-random HSV color for 60 ms on the LED for the half where the key was pressed. Left-half keypresses use LED index `0`; right-half keypresses use LED index `1`. It uses full saturation and `SILAKKA54_KEYPRESS_LED_VALUE`. The hue is mixed from a small timer/keycode-based seed, so it can reuse any color, including colors also used for layers. It is meant as fun visual feedback, not accurate typing analytics.

## Vial custom keycodes

Declared in `keyboards/silakka54/keymaps/vial/keymap.c` and exposed in `keyboards/silakka54/keymaps/vial/vial.json`:

| Keycode | Vial name | Behavior |
| --- | --- | --- |
| `LYRLED` | Toggle Layer LED | Turns layer indicator LEDs on/off at runtime. |
| `TYPLED` | Toggle Typing LED | Turns per-keypress typing feedback on/off at runtime. |

Both toggles currently default to on after boot. They are runtime toggles only and are not persisted to EEPROM.

## Tap-hold behavior

Configured in `keyboards/silakka54/keymaps/vial/keymap.c`, with one weak-hook support change in `quantum/qmk_settings.c`.

- `get_flow_tap_term()` disables Flow Tap for the Ctrl/Shift home-row mod tap keys: `D`, `F`, `J`, `K`.
- `A`, `S`, `L`, and `;` use the global Vial/QMK Settings Flow Tap value.
- `get_permissive_hold()` forces permissive hold on for `D`, `F`, `J`, `K`, even when the global Vial setting is off.
- `get_chordal_hold()` respects the global Vial Chordal Hold setting, but if enabled it bypasses Chordal Hold for thumb-key positions and the Ctrl home-row tap keys `D` and `K`.
- `quantum/qmk_settings.c` marks the QMK Settings `get_permissive_hold()`, `get_chordal_hold()`, and `get_flow_tap_term()` hooks as weak so this keymap can override them while still using Vial QMK Settings.

## Current layout assumptions

The exported profile lives at:

```text
keyboards/silakka54/layouts/Miryoku Custom.vil
```

The working thumb order after the layer swap is:

```text
Left:  LT4(KC_LGUI), KC_BSPACE, LT1(KC_TAB)
Right: LT5(KC_DELETE), LT3(KC_ENTER), LT2(KC_SPACE)
```

The base home-row mods in that profile are:

```text
Left:  LGUI_T(KC_A), LALT_T(KC_S), LCTL_T(KC_D), LSFT_T(KC_F)
Right: RGUI_T(KC_SCOLON), RALT_T(KC_L), RCTL_T(KC_K), RSFT_T(KC_J)
```

## Build

Build the right-master UF2 from QMK MSYS or PowerShell with QMK MSYS bash:

```sh
cd /c/src/qmk-firmware
make silakka54:vial EXTRAFLAGS=-DMASTER_RIGHT
cp .build/silakka54_vial.uf2 .build/silakka54_vial_RIGHT.uf2
cp .build/silakka54_vial.uf2 silakka54_vial_RIGHT.uf2
```

Flash `silakka54_vial_RIGHT.uf2`. For this build, the right half is the master because the USB cable is plugged into the right side.

# Quantum Mechanical Keyboard Firmware

[![Current Version](https://img.shields.io/github/tag/qmk/qmk_firmware.svg)](https://github.com/qmk/qmk_firmware/tags)
[![Discord](https://img.shields.io/discord/440868230475677696.svg)](https://discord.gg/qmk)
[![Docs Status](https://img.shields.io/badge/docs-ready-orange.svg)](https://docs.qmk.fm)
[![GitHub contributors](https://img.shields.io/github/contributors/qmk/qmk_firmware.svg)](https://github.com/qmk/qmk_firmware/pulse/monthly)
[![GitHub forks](https://img.shields.io/github/forks/qmk/qmk_firmware.svg?style=social&label=Fork)](https://github.com/qmk/qmk_firmware/)

This is a keyboard firmware based on the [tmk\_keyboard firmware](https://github.com/tmk/tmk_keyboard) with some useful features for Atmel AVR and ARM controllers, and more specifically, the [OLKB product line](https://olkb.com), the [ErgoDox EZ](https://ergodox-ez.com) keyboard, and the Clueboard product line.

## Documentation

* [See the official documentation on docs.qmk.fm](https://docs.qmk.fm)

The docs are powered by [VitePress](https://vitepress.dev/). They are also viewable offline; see [Previewing the Documentation](https://docs.qmk.fm/#/contributing?id=previewing-the-documentation) for more details.

You can request changes by making a fork and opening a [pull request](https://github.com/qmk/qmk_firmware/pulls).

## Supported Keyboards

* [Planck](/keyboards/planck/)
* [Preonic](/keyboards/preonic/)
* [ErgoDox EZ](/keyboards/ergodox_ez/)
* [Clueboard](/keyboards/clueboard/)
* [Cluepad](/keyboards/clueboard/17/)
* [Atreus](/keyboards/atreus/)

The project also includes community support for [lots of other keyboards](/keyboards/).

## Maintainers

QMK is developed and maintained by Jack Humbert of OLKB with contributions from the community, and of course, [Hasu](https://github.com/tmk). The OLKB product firmwares are maintained by [Jack Humbert](https://github.com/jackhumbert), the Ergodox EZ by [ZSA Technology Labs](https://github.com/zsa), the Clueboard by [Zach White](https://github.com/skullydazed), and the Atreus by [Phil Hagelberg](https://github.com/technomancy).

## Official Website

[qmk.fm](https://qmk.fm) is the official website of QMK, where you can find links to this page, the documentation, and the keyboards supported by QMK.
