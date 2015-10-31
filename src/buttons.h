#define PIN_BUTTONS 4
#define BUTTONS_PRESS_DURATION 7

enum {
  BUTTON_NONE = 0,
  BUTTON_1 = 1,
  BUTTON_2 = 2,
  BUTTON_3 = 3
};


void dispatchButtons();
void onPressedButton( void (*f)(int b) );
