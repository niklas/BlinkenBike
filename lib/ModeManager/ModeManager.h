#ifndef __MODE_MANAGER__H_
#define __MODE_MANAGER__H_

#include "Effects.h"

// Here we keep the state of the "app"

class ModeManager {
  public:
    ModeManager();

    void
      readInputs(void),
      selectNext(void),
      selectPrevious(void),
      apply(void);

    bool
      shouldAutoTransition(void),
      triggered;

    int
      effect,
      pot,
      effectDurationBase,        // Currently selected base for Duration (in frames) of current transition
      randomEffectDuration(void),
      selectedEffect;

  private:
    float
      potiBase;
};

#endif
