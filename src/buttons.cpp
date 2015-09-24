#include "Arduino.h"
#include "buttons.h"

enum {
  BUTTON_NONE = 0,
  BUTTON_1 = 1,
  BUTTON_2 = 2,
  BUTTON_3 = 3
};

byte pressedButton = BUTTON_NONE;
byte pressedButtonTimes = 0;

void sig_buttonPressed(byte button) {
  Serial.println(button);
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


