#include "Arduino.h"
#include "buttons.h"

enum {
  BUTTON_NONE = 0,
  BUTTON_1 = 1,
  BUTTON_2 = 2,
  BUTTON_3 = 3
};

void dispatchButtons() {
  int val;
  byte button = BUTTON_NONE;

  val = analogRead(PIN_BUTTONS);


  if (val > 960) {
    button = BUTTON_1;
  } else if (val > 910) {
    button = BUTTON_2;
  } else if (val > 510) {
    button = BUTTON_3;
  }

  if (button != BUTTON_NONE) {
    Serial.print(button);
    Serial.print("  ");
    Serial.print(val);
    Serial.println();
  }
}


