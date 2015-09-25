#include "Arduino.h"
#include "modes.h"

void mode_select_next() {
  selectedMode = (selectedMode + 1) % MODE_NUM;
  Serial.print("next mode: ");
  Serial.println(selectedMode);
}

void mode_select_previous() {
  selectedMode = (selectedMode - 1 + MODE_NUM) % MODE_NUM;
  Serial.print("prev mode: ");
  Serial.println(selectedMode);
}

void mode_apply() {
  mode = selectedMode;
  Serial.println("applied");
}
