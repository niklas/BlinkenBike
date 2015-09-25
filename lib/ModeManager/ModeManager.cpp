#include "Arduino.h"
#include "ModeManager.h"

ModeManager::ModeManager(void) {
  mode         = ModeWandererRed;
  selectedMode = mode;
}

void ModeManager::selectNext(void) {
  selectedMode = (selectedMode + 1) % MODE_NUM;
  Serial.print("next mode: ");
  Serial.println(selectedMode);
}

void ModeManager::selectPrevious(void) {
  selectedMode = (selectedMode - 1 + MODE_NUM) % MODE_NUM;
  Serial.print("prev mode: ");
  Serial.println(selectedMode);
}

void ModeManager::apply(void) {
  mode = selectedMode;
  Serial.println("applied");
}

int ModeManager::getMode(void) { return(mode + 1); }
int ModeManager::getSelectedMode(void) { return(selectedMode + 1); }

