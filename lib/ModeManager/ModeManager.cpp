#include "ModeManager.h"

ModeManager::ModeManager(void) {
  effect         = 0;
  selectedEffect = effect;
  pot            = 0;
}

void ModeManager::selectNext(void) {
  selectedEffect = (selectedEffect + 1) % EFFECT_NUM;
}

void ModeManager::selectPrevious(void) {
  selectedEffect = (selectedEffect - 1 + EFFECT_NUM) % EFFECT_NUM;
}

void ModeManager::readInputs(void) {
  triggered = digitalRead(PIN_KLINKE) == 0 ? false : true;
  pot = analogRead(PIN_POT_SIDE);
}

bool ModeManager::shouldAutoTransition(void) {
  return(pot < EFFECT_DURATION_POTI_STOP ? false : true);
}

void ModeManager::apply(void) {
  effect = selectedEffect;
}
