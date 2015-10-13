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
      selectedEffect;

  private:
};

#endif
