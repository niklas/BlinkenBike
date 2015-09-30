#include <Arduino.h>

class LEDStrip {

 public:

  LEDStrip(uint16_t n, uint8_t dpin, uint8_t cpin); // Configurable pins
  void
    begin(void),
    setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b),
    setPixelColor(uint16_t n, uint32_t c),
    show(void),
    updatePins(uint8_t dpin, uint8_t cpin), // Change pins, configurable
    updateLength(uint16_t n);               // Change strip length
  uint16_t
    numPixels(void);
  uint32_t
    Color(byte, byte, byte),
    getPixelColor(uint16_t n);

 private:

  uint16_t
    numLEDs,    // Number of RGB LEDs in strip
    numBytes;   // Size of 'pixels' buffer below
  uint8_t
    *pixels,    // Holds LED color values (3 bytes each) + latch bytes
    clkpin    , datapin;     // Clock & data pin numbers
#ifdef __AVR__
  uint8_t
    clkpinmask, datapinmask; // Clock & data PORT bitmasks
  volatile uint8_t
    *clkport  , *dataport;   // Clock & data PORT registers
#endif
  void
    startBitbang(void);
  boolean
    begun;       // If 'true', begin() method was previously invoked
};

