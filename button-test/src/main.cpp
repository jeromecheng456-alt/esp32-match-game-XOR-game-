// Match Game phase 2: button test.
// Each pin uses the internal pull-up, so pressed = LOW (touched to GND).
// Prints one line per press (on the HIGH -> LOW edge) with simple 30 ms debounce.
#include <Arduino.h>

struct Btn { const char* name; int pin; bool last; uint32_t t; uint32_t presses; uint32_t releases; };
Btn btns[] = {
  {"A1 (GPIO13)", 13, true, 0, 0, 0},
  {"A2 (GPIO14)", 14, true, 0, 0, 0},
  {"B1 (GPIO27)", 27, true, 0, 0, 0},
  {"B2 (GPIO26)", 26, true, 0, 0, 0},
};

void setup() {
  Serial.begin(115200);
  for (auto& b : btns) pinMode(b.pin, INPUT_PULLUP);
  Serial.println("button-test ready");
}

void loop() {
  uint32_t now = millis();
  for (auto& b : btns) {
    bool level = digitalRead(b.pin);
    if (level != b.last && now - b.t > 30) {
      b.t = now;
      b.last = level;
      if (!level) { b.presses++;  Serial.printf("PRESS   %s  presses=%u releases=%u\n", b.name, (unsigned)b.presses, (unsigned)b.releases); }
      else        { b.releases++; Serial.printf("release %s  presses=%u releases=%u\n", b.name, (unsigned)b.presses, (unsigned)b.releases); }
    }
  }
}
