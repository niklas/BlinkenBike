#include "Arduino.h"
#include "LPD8806.h"
#include "SPI.h"
#include <MsTimer2.h>

#define FPS 60


#include "modes.h"
#include "buttons.h"

#define ON_BOARD_LED 13
#define MAX_BRIGHTNESS 127

#define PIN_PREVIEW_DATA 6
#define PIN_PREVIEW_CLOCK 7

// Number of RGB LEDs in strand
int nLEDs = 48;

// Chose 2 pins for output; can be any valid output pins:
int dataPin  = 2;
int clockPin = 3;
int potPin = 0;

int brightness = 8;

// First parameter is the number of LEDs in the strand.  The LED strips
// are 32 LEDs per meter but you can extend or cut the strip.  Next two
// parameters are SPI data and clock pins:
LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);
LPD8806 preview = LPD8806(2, PIN_PREVIEW_DATA, PIN_PREVIEW_CLOCK);

// Callbacks must be declared early
void each_tick();
void buttonPressed(int button) {
  Serial.println(button);
  switch(button) {
    case BUTTON_1: mode_select_next()       ; break;
    case BUTTON_2: mode_apply()             ; break;
    case BUTTON_3: mode_select_previous()   ; break;
  }
}

void setup()
{
  pinMode(ON_BOARD_LED, OUTPUT);     // set pin as output
  pinMode(potPin, INPUT);
  Serial.begin(9600);
  Serial.println("will print poti");


  mode = ModeWandererRed;
  selectedMode = mode;
  onPressedButton(buttonPressed);

  preview.begin();


  strip.begin();
  strip.show();

  MsTimer2::set(1000 / FPS, each_tick);
  MsTimer2::start();
}

void potToBrightness(int pin) {
  int val = analogRead(pin);

  val = constrain(val, 1, 1023);
  brightness = map(val, 0, 1023, 0, MAX_BRIGHTNESS);
}

uint32_t tick = 0;
int pos;

void each_tick() {
  uint32_t color, previewColor;
  strip.show();
  preview.show();
  tick++;
  potToBrightness(potPin);
  dispatchButtons();

  // clear old position
  strip.setPixelColor(pos, strip.Color(0,0,0));

  // one point chasing down
  pos = tick % nLEDs;

  // TODO strip.Color is actually RBG?!
  switch(mode) {
    case ModeWandererRed:   color = strip.Color( brightness, 0, 0)                   ; break ;
    case ModeWandererBlue:  color = strip.Color( 0, brightness, 0)                   ; break ;
    case ModeWandererGreen: color = strip.Color( 0, 0, brightness)                   ; break ;
    case ModeWandererWhite: color = strip.Color( brightness, brightness, brightness) ; break ;
    default:                color = strip.Color(0,0,0)                               ; break ;
  }
  strip.setPixelColor(pos, color);

  if (tick % FPS == 0) {
    Serial.print("selected mode: ");
    Serial.println(selectedMode);
  }
  switch(selectedMode) {
    case ModeWandererRed:   previewColor = preview.Color( brightness, 0, 0)                   ; break ;
    case ModeWandererBlue:  previewColor = preview.Color( 0, brightness, 0)                   ; break ;
    case ModeWandererGreen: previewColor = preview.Color( 0, 0, brightness)                   ; break ;
    case ModeWandererWhite: previewColor = preview.Color( brightness, brightness, brightness) ; break ;
    default:                previewColor = strip.Color(0,0,0)                                 ; break ;
  }

  preview.setPixelColor(0, preview.Color(0,0,0));
  preview.setPixelColor(1, previewColor);
}

void loop() { } // see each_tick()
