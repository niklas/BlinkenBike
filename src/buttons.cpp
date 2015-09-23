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


  if (val > 1010) {
    button = BUTTON_1;
  } else if (val > 930) {
    button = BUTTON_2;
  } else if (val > 510) {
    button = BUTTON_3;
  }

  Serial.print(val);
  Serial.print("  ");
  Serial.println(button);
}


