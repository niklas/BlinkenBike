#include "Settings.h"
#include "FastLED.h"
int  tCounter,                  // Countdown to next transition
     effectDurationBase,        // Currently selected base for Duration (in frames) of current transition
     transitionTime;            // Duration (in frames) of current transition
bool shouldAutoTransition;

float __potiBase = pow(1.0/(EFFECT_DURATION_MAX_SECONDS * FPS), 1.0 / 1023);
