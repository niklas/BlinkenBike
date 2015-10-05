/*
Arduino library to control LPD8806-based RGB LED Strips
Copyright (C) Adafruit Industries
Copyright (C) Niklas Hofer (optimized to not allocate memory)
MIT license
*/

#include "LEDStrip.h"

#include <SPI.h>
#define spi_out(n) (void)SPI.transfer(n)


/*****************************************************************************/

// Constructor for use with arbitrary clock/data pins:
LEDStrip::LEDStrip(uint16_t n, uint8_t dpin, uint8_t cpin) {
  begun  = false;
  updateLength(n);
  updatePins(dpin, cpin);
}


// Activate hard/soft SPI as appropriate:
void LEDStrip::begin(void) {
  startBitbang();
  begun = true;
}

// Change pin assignments post-constructor, using arbitrary pins:
void LEDStrip::updatePins(uint8_t dpin, uint8_t cpin) {
  if(begun == true) { // If begin() was previously invoked...
    pinMode(datapin, INPUT); // Restore prior data and clock pins to inputs
    pinMode(clkpin , INPUT);
  }
  datapin     = dpin;
  clkpin      = cpin;
#ifdef __AVR__
  clkport     = portOutputRegister(digitalPinToPort(cpin));
  clkpinmask  = digitalPinToBitMask(cpin);
  dataport    = portOutputRegister(digitalPinToPort(dpin));
  datapinmask = digitalPinToBitMask(dpin);
#endif

  // If previously begun, enable 'soft' SPI outputs now
  if(begun == true) startBitbang();
}

// Enable software SPI pins and issue initial latch:
void LEDStrip::startBitbang() {
  pinMode(datapin, OUTPUT);
  pinMode(clkpin , OUTPUT);
#ifdef __AVR__
  *dataport &= ~datapinmask; // Data is held low throughout (latch = 0)
  for(uint16_t i=((numLEDs+31)/32)*8; i>0; i--) {
    *clkport |=  clkpinmask;
    *clkport &= ~clkpinmask;
  }
#else
  digitalWrite(datapin, LOW);
  for(uint16_t i=((numLEDs+31)/32)*8; i>0; i--) {
    digitalWrite(clkpin, HIGH);
    digitalWrite(clkpin, LOW);
  }
#endif
}

// Change strip length (see notes with empty constructor, above):
void LEDStrip::updateLength(uint16_t n) {
  numLEDs  = n;
}

uint16_t LEDStrip::numPixels(void) {
  return numLEDs;
}

void LEDStrip::show(byte * ptr) {
  uint16_t i = numLEDs * 3;
  byte rg;

  // first, send all the data
  // we must swap Red and Green because our LEDstrips are different
  while(i--) {
    switch (i % 3) {
      case 1:
        rg = *ptr++;
        break;
      case 0:
        sendByte(*ptr++ | 0x80);
        sendByte(rg | 0x80);
        break;
      default:
        sendByte(*ptr++ | 0x80);
    }
  }

  // then, some latch 0's
  i = (numLEDs + 31) / 32;
  while(i--) {
    sendByte(0);
  }
}

void LEDStrip::sendByte(uint8_t p) {
  uint8_t bit;

  for(bit=0x80; bit; bit >>= 1) {
#ifdef __AVR__
    if(p & bit) *dataport |=  datapinmask;
    else        *dataport &= ~datapinmask;
    *clkport |=  clkpinmask;
    *clkport &= ~clkpinmask;
#else
    if(p & bit) digitalWrite(datapin, HIGH);
    else        digitalWrite(datapin, LOW);
    digitalWrite(clkpin, HIGH);
    digitalWrite(clkpin, LOW);
#endif
  }
}

// Convert separate R,G,B into combined 32-bit GRB color:
uint32_t LEDStrip::Color(byte r, byte g, byte b) {
  return ((uint32_t)(g | 0x80) << 16) |
         ((uint32_t)(r | 0x80) <<  8) |
                     b | 0x80 ;
}
