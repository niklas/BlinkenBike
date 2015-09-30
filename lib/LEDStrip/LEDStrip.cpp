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
  pixels = NULL;
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
  uint8_t  latchBytes;
  uint16_t dataBytes, totalBytes;

  numLEDs = numBytes = 0;
  if(pixels) free(pixels); // Free existing data (if any)

  dataBytes  = n * 3;
  latchBytes = (n + 31) / 32;
  totalBytes = dataBytes + latchBytes;
  if((pixels = (uint8_t *)malloc(totalBytes))) { // Alloc new data
    numLEDs  = n;
    numBytes = totalBytes;
    memset( pixels           , 0x80, dataBytes);  // Init to RGB 'off' state
    memset(&pixels[dataBytes], 0   , latchBytes); // Clear latch bytes
  }
  // 'begun' state does not change -- pins retain prior modes
}

uint16_t LEDStrip::numPixels(void) {
  return numLEDs;
}

void LEDStrip::show(void) {
  uint8_t  *ptr = pixels;
  uint16_t i    = numBytes;

  // This doesn't need to distinguish among individual pixel color
  // bytes vs. latch data, etc.  Everything is laid out in one big
  // flat buffer and issued the same regardless of purpose.
  uint8_t p, bit;

  while(i--) {
    p = *ptr++;
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
}

// Convert separate R,G,B into combined 32-bit GRB color:
uint32_t LEDStrip::Color(byte r, byte g, byte b) {
  return ((uint32_t)(g | 0x80) << 16) |
         ((uint32_t)(r | 0x80) <<  8) |
                     b | 0x80 ;
}

// Set pixel color from separate 7-bit R, G, B components:
void LEDStrip::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
  if(n < numLEDs) { // Arrays are 0-indexed, thus NOT '<='
    uint8_t *p = &pixels[n * 3];
    *p++ = g | 0x80; // Strip color order is GRB,
    *p++ = r | 0x80; // not the more common RGB,
    *p++ = b | 0x80; // so the order here is intentional; don't "fix"
  }
}

// Set pixel color from 'packed' 32-bit GRB (not RGB) value:
void LEDStrip::setPixelColor(uint16_t n, uint32_t c) {
  if(n < numLEDs) { // Arrays are 0-indexed, thus NOT '<='
    uint8_t *p = &pixels[n * 3];
    *p++ = (c >> 16) | 0x80;
    *p++ = (c >>  8) | 0x80;
    *p++ =  c        | 0x80;
  }
}

// Query color from previously-set pixel (returns packed 32-bit GRB value)
uint32_t LEDStrip::getPixelColor(uint16_t n) {
  if(n < numLEDs) {
    uint16_t ofs = n * 3;
    return ((uint32_t)(pixels[ofs    ] & 0x7f) << 16) |
           ((uint32_t)(pixels[ofs + 1] & 0x7f) <<  8) |
            (uint32_t)(pixels[ofs + 2] & 0x7f);
  }

  return 0; // Pixel # is out of bounds
}

