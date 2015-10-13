#include "ModeManager.h"

ModeManager::ModeManager(void) {
  effect         = 0;
  selectedEffect = effect;
}

void ModeManager::selectNext(void) {
  selectedEffect = (selectedEffect + 1) % EFFECT_NUM;
}

void ModeManager::selectPrevious(void) {
  selectedEffect = (selectedEffect - 1 + EFFECT_NUM) % EFFECT_NUM;
}

void ModeManager::readInputs(void) {
  triggered = digitalRead(PIN_KLINKE) == 0 ? false : true;
}

void ModeManager::apply(void) {
  effect = selectedEffect;
}
