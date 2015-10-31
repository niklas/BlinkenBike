#include "Arduino.h"
#include "buttons.h"

byte pressedButton = BUTTON_NONE;
byte pressedButtonTimes = 0;

void (*pressedButtonCallback)(int b);

void onPressedButton( void (*f)(int b) ) {
  pressedButtonCallback = f;
}

void sig_buttonPressed(byte button) {
  pressedButtonCallback(button);
}

void dispatchButtons() {
  int val;
  byte button = BUTTON_NONE;

  val = analogRead(PIN_BUTTONS);


  if (val > 960) {
    button = BUTTON_1;
  } else if (val > 910) {
    button = BUTTON_2;
  } else if (val > 510) {
    button = BUTTON_3;
  }

  // Debounce Buttons
  //
  // Must keep a button pressed for a while (BUTTONS_PRESS_DURATION)  until it is registered.
  // Tapping shorty works, too for half the BUTTONS_PRESS_DURATION.

  if (button != BUTTON_NONE) {                                  // something pressed
    if (pressedButton == button) {                              // button kept pressed
      pressedButtonTimes++;

      if (pressedButtonTimes > BUTTONS_PRESS_DURATION) {
        pressedButtonTimes = 0;
        sig_buttonPressed(button);
      }
    } else {                                                    // different button
      pressedButtonTimes = 0;
    }
  } else {                                                      // no button pressed

    if (
        (pressedButton != BUTTON_NONE) &&
        (pressedButtonTimes > BUTTONS_PRESS_DURATION >> 1)) {   // just released
      sig_buttonPressed(pressedButton);
    }

    pressedButtonTimes = 0;
  }

  pressedButton = button;

}


