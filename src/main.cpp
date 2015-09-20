#ifdef ENERGIA
  #include "Energia.h"
#else
  #include "Arduino.h"
#endif

#include "LPD8806.h"
#include "SPI.h"

#define ON_BOARD_LED 13


// Number of RGB LEDs in strand
int nLEDs = 96;

// Chose 2 pins for output; can be any valid output pins:
int dataPin  = 2;
int clockPin = 3;

// First parameter is the number of LEDs in the strand.  The LED strips
// are 32 LEDs per meter but you can extend or cut the strip.  Next two
// parameters are SPI data and clock pins:
LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);

void setup()
{
  pinMode(ON_BOARD_LED, OUTPUT);     // set pin as output
  strip.begin();
  strip.show();
}

// Chase one dot down the full strip.  Good for testing purposes.
void colorChase(uint32_t c, uint8_t wait) {
  int i;

  digitalWrite(ON_BOARD_LED, HIGH);

  // Start by turning all pixels off:
  for(i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, 0);

  // Then display one pixel at a time:
  for(i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c); // Set new pixel 'on'
    strip.show();              // Refresh LED states
    //strip.setPixelColor(i, 0); // Erase pixel, but don't refresh!
    delay(wait);
  }

  strip.show(); // Refresh to turn off last pixel
  digitalWrite(ON_BOARD_LED, LOW);   // set the LED off
  delay(wait);
}

void loop() {
  colorChase(strip.Color(127,  0,  0), 100); // Red
  colorChase(strip.Color(  0,127,  0), 100); // Green
  colorChase(strip.Color(  0,  0,127), 100); // Blue
  colorChase(strip.Color(127,127,127), 100); // White
}
