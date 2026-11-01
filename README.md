# ESP32 Match Game

Two players each secretly pick **option 1 or 2** on their own pair of buttons. Once both have locked in, a WS2812B LED strip glows:

- **Green**: both picked the same option
- **Red**: they picked different options

After a 5 s reveal the lights go off, and the next round starts once every button is released. A half-finished round times out after 30 s.

Built with an ESP32 DevKit, four push buttons and a WS2812B strip. No soldering: spade terminals, jumper wires and a breadboard.

## Rules (the logic)

| Rule | Behaviour |
|---|---|
| First press locks | A player's second press in the same round is ignored |
| Both buttons at once | Counts as option 1 |
| Button held at power-up, or after a timeout | Ignored until it has been released once |
| Reveal | 5 s |
| Timeout | 30 s after the first lock, if the other player never picks |
| Next round | Starts after the reveal and after all buttons are released |

The game logic (`firmware/src/MatchGame.*`) has no Arduino calls, so it runs in native unit tests on a PC: 12 tests cover all four combinations, the rules above and a full second round.

## Wiring

All inputs are 3.3 V GPIO with the internal pull-up, so each button just connects its pin to GND. No resistors are needed on the buttons.

| Part | ESP32 pin |
|---|---|
| Player A, option 1 | GPIO 13 |
| Player A, option 2 | GPIO 14 |
| Player B, option 1 | GPIO 27 |
| Player B, option 2 | GPIO 26 |
| Strip DIN | GPIO 4, through a 330 Ω resistor |
| Strip 5V | ESP32 5V pin |
| Strip GND, other button contacts | GND |

Notes from the build:

- WS2812B strips only read data at the **input** end. The arrows printed on the strip point away from the input.
- Only 16 LEDs are lit (the first 8 and the last 8 of 60), in pure green or red, at brightness 80/255. That is about 100 mA, safe to run from USB. Never show white (three channels at once).
- Use the **Adafruit NeoPixel** library. FastLED gave stuck colours on this board.

## Build and test

Requires [PlatformIO](https://platformio.org/).

```
cd firmware
pio test -e native      # unit tests on your PC
pio run -e esp32        # build for the board
pio run -e esp32 -t upload
```

`strip-test/` and `button-test/` are small bring-up programs: prove the strip shows green, red and off, and that each button prints exactly one press and release, before running the full game.
