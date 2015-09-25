#include "modes.h"

void mode_select_next() {
  selectedMode = (selectedMode + 1) % MODE_NUM;
}
void mode_select_previous() {
  selectedMode = (selectedMode - 1) % MODE_NUM;
}
void mode_apply() {
  mode = selectedMode;
}
