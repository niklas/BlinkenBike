#include "Arduino.h"
#include "LPD8806.h"
#include "SPI.h"
#include "buttons.h"

#define ON_BOARD_LED 13
#define MAX_BRIGHTNESS 127

#define PIN_PREVIEW_DATA 6
#define PIN_PREVIEW_CLOCK 7

// Number of RGB LEDs in strand
int nLEDs = 96;

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

void setup()
{
  pinMode(ON_BOARD_LED, OUTPUT);     // set pin as output
  pinMode(potPin, INPUT);
  Serial.begin(9600);
  Serial.println("will print poti");

  preview.begin();


  strip.begin();
  strip.show();
}

void potToBrightness(int pin) {
  int val = analogRead(pin);

  val = constrain(val, 1, 1023);
  brightness = map(val, 0, 1023, 0, MAX_BRIGHTNESS);
}

uint32_t tick = 0;
uint32_t color;
int pos;

void loop() {
  strip.show();
  tick++;
  potToBrightness(potPin);
  dispatchButtons();

  // clear old position
  strip.setPixelColor(pos, strip.Color(0,0,0));

  // one point chasing down
  pos = tick % nLEDs;
  color = strip.Color( 0, brightness, 0);
  strip.setPixelColor(pos, color);

  preview.setPixelColor(0, preview.Color(23,0,0));
  preview.setPixelColor(1, preview.Color(0,0,23));
  preview.show();
  delay(23);
}
