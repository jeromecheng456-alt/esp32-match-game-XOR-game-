// Match Game firmware. Pins: A1=13 A2=14 B1=27 B2=26 (to GND when pressed),
// strip DIN = GPIO4 via 330 ohm. Pure green = SAME, pure red = DIFFERENT, off otherwise.
// Never shows white (3 channels = 3x current).
#ifndef PIO_UNIT_TESTING
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "MatchGame.h"

constexpr int PIN_A1 = 13, PIN_A2 = 14, PIN_B1 = 27, PIN_B2 = 26;
constexpr int PIN_LED_DATA = 4;
constexpr int NUM_LEDS = 60;     // drive all 60 so none show stray colours
constexpr int NUM_LIT = 8;       // first 8 and last 8 light up (16 x ~6 mA = ~100 mA)
constexpr int BRIGHTNESS = 80;   // of 255, current cap
constexpr uint32_t REVEAL_MS = 5000;
constexpr uint32_t TIMEOUT_MS = 30000;

Adafruit_NeoPixel strip(NUM_LEDS, PIN_LED_DATA, NEO_GRB + NEO_KHZ800);
MatchGame game(REVEAL_MS, TIMEOUT_MS);

MatchResult lastResult = MatchResult::NONE;
uint8_t lastA = 0, lastB = 0;

void showResult(MatchResult r) {
  uint32_t colour = 0;
  if (r == MatchResult::SAME) colour = strip.Color(0, 255, 0);
  if (r == MatchResult::DIFFERENT) colour = strip.Color(255, 0, 0);
  strip.clear();
  for (int i = 0; i < NUM_LIT; i++) {
    strip.setPixelColor(i, colour);                   // first NUM_LIT
    strip.setPixelColor(NUM_LEDS - 1 - i, colour);    // last NUM_LIT
  }
  strip.show();
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_A1, INPUT_PULLUP);
  pinMode(PIN_A2, INPUT_PULLUP);
  pinMode(PIN_B1, INPUT_PULLUP);
  pinMode(PIN_B2, INPUT_PULLUP);
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  showResult(MatchResult::NONE);
  Serial.println("match-game ready");
}

void loop() {
  Inputs in{!digitalRead(PIN_A1), !digitalRead(PIN_A2),
            !digitalRead(PIN_B1), !digitalRead(PIN_B2)};
  game.update(millis(), in);

  // Diagnostic: while waiting for all buttons to be released, say which are down.
  static uint32_t lastDiagMs = 0;
  if (game.state() == MatchState::WAIT_RELEASE && millis() - lastDiagMs > 1000) {
    lastDiagMs = millis();
    Serial.printf("waiting for release: A1=%d A2=%d B1=%d B2=%d (1 = pressed)\n",
                  in.a1, in.a2, in.b1, in.b2);
  }

  if (game.pickA() != lastA) {
    lastA = game.pickA();
    if (lastA) { Serial.print("A locked "); Serial.println(lastA); }
  }
  if (game.pickB() != lastB) {
    lastB = game.pickB();
    if (lastB) { Serial.print("B locked "); Serial.println(lastB); }
  }

  MatchResult r = game.result();
  if (r != lastResult) {
    lastResult = r;
    showResult(r);
    if (r == MatchResult::SAME) Serial.println("RESULT SAME (green)");
    else if (r == MatchResult::DIFFERENT) Serial.println("RESULT DIFFERENT (red)");
    else Serial.println("round over");
  }
  delay(5);
}
#endif
