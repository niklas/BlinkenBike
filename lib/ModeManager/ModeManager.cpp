#include "ModeManager.h"

ModeManager::ModeManager(void) {
  effect         = 0;
  selectedEffect = effect;
  pot            = 0;

  potiBase = pow(1.0/(EFFECT_DURATION_MAX_SECONDS * FPS), 1.0 / 1023);
  effectDurationBase = 5 * FPS;
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

  effectDurationBase = FPS + pow(potiBase, pot) * EFFECT_DURATION_MAX_SECONDS * FPS;
}

bool ModeManager::shouldAutoTransition(void) {
  return(pot < EFFECT_DURATION_POTI_STOP ? false : true);
}

void ModeManager::apply(void) {
  effect = selectedEffect;
}

int ModeManager::randomEffectDuration() {
   return random16(effectDurationBase, EFFECT_DURATION_STRETCH * effectDurationBase);
}
