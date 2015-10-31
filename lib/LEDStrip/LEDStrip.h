#include <Arduino.h>

#ifndef __LEDSTRIP_H__
#define __LEDSTRIP_H__

class LEDStrip {

 public:

  LEDStrip(uint16_t n, uint8_t dpin, uint8_t cpin); // Configurable pins
  void
    begin(void),
    show(byte * pix),
    updatePins(uint8_t dpin, uint8_t cpin), // Change pins, configurable
    updateLength(uint16_t n);               // Change strip length
  uint16_t
    numPixels(void);
  uint32_t
    Color(byte, byte, byte);

 private:

  uint16_t
    numLEDs;    // Number of RGB LEDs in strip
  uint8_t
    clkpin    , datapin;     // Clock & data pin numbers
  void
    sendByte(uint8_t p);
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

#endif
