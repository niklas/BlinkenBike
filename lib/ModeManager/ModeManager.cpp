#include "ModeManager.h"

ModeManager::ModeManager(void) {
  effect         = 0;
  selectedEffect = effect;
  pot            = 0;

  potiBase = pow(1.0/(EFFECT_DURATION_MAX_SECONDS * FPS), 1.0 / 1023);
  effectDurationBase = 5 * FPS;

  pinMode(PIN_POT_SIDE, INPUT);
  pinMode(PIN_TOGGLE_1, INPUT);
  pinMode(PIN_TOGGLE_2, INPUT);
  pinMode(PIN_KLINKE , INPUT);
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

  toggle1 = digitalRead(PIN_TOGGLE_1);
  toggle2 = digitalRead(PIN_TOGGLE_2);
}

bool ModeManager::isEmergency(void) {
  return(toggle2 == 1 ? true : false);
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

ModeManager mode = ModeManager();
