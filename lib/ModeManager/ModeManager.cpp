#include "ModeManager.h"

ModeManager::ModeManager(void) {
  mode         = ModeWandererRed;
  selectedMode = mode;
}

void ModeManager::selectNext(void) {
  selectedMode = (selectedMode + 1) % MODE_NUM;
}

void ModeManager::selectPrevious(void) {
  selectedMode = (selectedMode - 1 + MODE_NUM) % MODE_NUM;
}

void ModeManager::apply(void) {
  mode = selectedMode;
}

int ModeManager::getMode(void) { return(mode + 1); }
int ModeManager::getSelectedMode(void) { return(selectedMode + 1); }

