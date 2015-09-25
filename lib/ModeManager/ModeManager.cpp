#include "LPD8806.h"
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

int ModeManager::getMode(void) { return(mode); }
int ModeManager::getSelectedMode(void) { return(selectedMode); }

// TODO strip.Color is actually RBG?!
uint32_t ModeManager::getColor(int m, LPD8806 leds, int bright) {
  switch(m) {
    case ModeWandererRed:   return( leds.Color( bright , 0          , 0          ));
    case ModeWandererBlue:  return( leds.Color( 0          , bright , 0          ));
    case ModeWandererGreen: return( leds.Color( 0          , 0          , bright ));
    case ModeWandererWhite: return( leds.Color( bright , bright , bright ));
    default:                return( leds.Color( 0          , 0          , 0          ));
  }
}
