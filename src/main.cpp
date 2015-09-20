#ifdef ENERGIA
  #include "Energia.h"
#else
  #include "Arduino.h"
#endif

#include "LPD8806.h"
#include "SPI.h"

#define ON_BOARD_LED 13
#define MAX_BRIGHTNESS 127


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

void setup()
{
  pinMode(ON_BOARD_LED, OUTPUT);     // set pin as output
  Serial.begin(9600);
  Serial.println("will print poti");
  strip.begin();
  strip.show();
}

void potToBrightness(int pin) {
  int val = analogRead(pin);
  Serial.println(val);

  if (val < MAX_BRIGHTNESS) {
    brightness = val;
  } else {
    brightness = MAX_BRIGHTNESS;
  }
}

uint32_t tick = 0;
uint32_t color;
int pos;

void loop() {
  strip.show();
  tick++;
  potToBrightness(potPin);

  // clear old position
  strip.setPixelColor(pos, strip.Color(0,0,0));

  // one point chasing down
  pos = tick % nLEDs;
  color = strip.Color( brightness, 0, 0);
  strip.setPixelColor(pos, color);
  delay(23);
}
