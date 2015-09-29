#include "Arduino.h"
#include "LPD8806.h"
#include "SPI.h"
#include <MsTimer2.h>

#define FPS 60


#include <ModeManager.h>
#include "buttons.h"

#define ON_BOARD_LED 13
#define MAX_BRIGHTNESS 127

#define PIN_PREVIEW_DATA 6
#define PIN_PREVIEW_CLOCK 7

// Number of RGB LEDs in strand
int nLEDs = 110;

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
ModeManager mode = ModeManager();

// Callbacks must be declared early
void each_tick();
void buttonPressed(int button) {
  switch(button) {
    case BUTTON_1: mode.selectNext()     ; break ;
    case BUTTON_2: mode.apply()          ; break ;
    case BUTTON_3: mode.selectPrevious() ; break ;
  }
}

void setup()
{
  pinMode(ON_BOARD_LED, OUTPUT);     // set pin as output
  pinMode(potPin, INPUT);
  Serial.begin(9600);
  Serial.println("Welcome to BlinkenBike © niklas@lanpartei.de");


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
  uint32_t color, previewColor, black;
  strip.show();
  preview.show();
  tick++;
  potToBrightness(potPin);
  dispatchButtons();

  // clear old position
  black = strip.Color(0,0,0);
  strip.setPixelColor(pos, black);
  strip.setPixelColor((pos + nLEDs/2) % nLEDs, black);
  strip.setPixelColor((pos + nLEDs/3) % nLEDs, black);
  strip.setPixelColor((pos + 3 * nLEDs/4) % nLEDs, black);
  strip.setPixelColor((pos + 5 * nLEDs/6) % nLEDs, black);

  // one point chasing down
  pos = tick % nLEDs;

  color = mode.getColor(mode.getMode(), strip, brightness);

  strip.setPixelColor(pos, color);
  strip.setPixelColor((pos + 1) % nLEDs, color);
  strip.setPixelColor((pos + 2) % nLEDs, color);
  strip.setPixelColor((pos + 3) % nLEDs, color);
  strip.setPixelColor((pos + 4) % nLEDs, color);
  strip.setPixelColor((pos + nLEDs/2) % nLEDs, color);
  strip.setPixelColor((pos + nLEDs/2 + 1) % nLEDs, color);
  strip.setPixelColor((pos + nLEDs/2 + 2) % nLEDs, color);
  strip.setPixelColor((pos + nLEDs/3) % nLEDs, color);
  strip.setPixelColor((pos + nLEDs/3 + 1) % nLEDs, color);
  strip.setPixelColor((pos + 3 * nLEDs/4) % nLEDs, color);
  strip.setPixelColor((pos + 3 * nLEDs/4 + 1) % nLEDs, color);
  strip.setPixelColor((pos + 3 * nLEDs/4 + 2) % nLEDs, color);
  strip.setPixelColor((pos + 3 * nLEDs/4 + 3) % nLEDs, color);
  strip.setPixelColor((pos + 5 * nLEDs/6) % nLEDs, color);
  strip.setPixelColor((pos + 5 * nLEDs/6 + 1) % nLEDs, color);

  previewColor = mode.getColor(mode.getSelectedMode(), preview, brightness);

  preview.setPixelColor(0, black);
  preview.setPixelColor(1, previewColor);
}

void loop() { } // see each_tick()
