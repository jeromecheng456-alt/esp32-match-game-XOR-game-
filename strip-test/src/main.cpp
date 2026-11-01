// Match Game phase 1: strip test.
// Cycles pure green -> pure red -> off on the first 8 LEDs, 2 s each.
// DIN -> 330 ohm -> GPIO4. Never shows white (3 channels = 3x current).
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

constexpr int PIN_LED_DATA = 4;
constexpr int NUM_LEDS = 60;     // drive all 60 so none show stray colours
constexpr int NUM_LIT = 8;       // only these light up
constexpr int BRIGHTNESS = 80;   // of 255, current cap

Adafruit_NeoPixel strip(NUM_LEDS, PIN_LED_DATA, NEO_GRB + NEO_KHZ800);

void show(uint8_t r, uint8_t g, const char* name) {
  strip.clear();
  for (int i = 0; i < NUM_LIT; i++) strip.setPixelColor(i, strip.Color(r, g, 0));
  strip.show();
  Serial.println(name);
}

void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.clear();
  strip.show();
  Serial.println("strip-test ready");
}

void loop() {
  show(0, 255, "GREEN");
  delay(2000);
  show(255, 0, "RED");
  delay(2000);
  show(0, 0, "OFF");
  delay(2000);
}
