#include "Gamma.h"
// Gamma correction compensates for our eyes' nonlinear perception of
// intensity.  It's the LAST step before a pixel value is stored, and
// allows intermediate rendering/processing to occur in linear space.
// The table contains 256 elements (8 bit input), though the outputs are
// only 7 bits (0 to 127).  This is normal and intentional by design: it
// allows all the rendering code to operate in the more familiar unsigned
// 8-bit colorspace (used in a lot of existing graphics code), and better
// preserves accuracy where repeated color blending operations occur.
// Only the final end product is converted to 7 bits, the native format
// for the LPD8806 LED driver.  Gamma correction and 7-bit decimation
// thus occur in a single operation.
//
// It seems to represent gamma of value 2.5
// We doubled the values because FastLED halfs the values for us
PROGMEM const unsigned char gammaTable[]  = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  4,  4,  4,  4,
    4,  4,  4,  4,  4,  6,  6,  6,  6,  6,  6,  6,  6,  8,  8,  8,
    8,  8,  8,  8, 10, 10, 10, 10, 10, 12, 12, 12, 12, 12, 14, 14,
   14, 14, 14, 16, 16, 16, 16, 18, 18, 18, 18, 20, 20, 20, 20, 22,
   22, 22, 24, 24, 24, 26, 26, 26, 26, 28, 28, 28, 30, 30, 32, 32,
   32, 34, 34, 34, 36, 36, 36, 38, 38, 40, 40, 42, 42, 42, 44, 44,
   46, 46, 48, 48, 48, 50, 50, 52, 52, 54, 54, 56, 56, 58, 58, 60,
   60, 62, 64, 64, 66, 66, 68, 68, 70, 70, 72, 74, 74, 76, 76, 78,
   80, 80, 82, 82, 84, 86, 86, 88, 90, 90, 92, 94, 94, 96, 98,100,
  100,102,104,104,106,108,110,110,112,114,116,116,118,120,122,124,
  124,126,128,130,132,134,134,136,138,140,142,144,146,148,148,150,
  152,154,156,158,160,162,164,166,168,170,172,174,176,178,180,182,
  184,186,188,190,192,194,196,198,200,202,204,208,210,212,214,216,
  218,220,222,226,228,230,232,234,236,240,242,244,246,250,252,254
};

// This function (which actually gets 'inlined' anywhere it's called)
// exists so that gammaTable can reside out of the way down here in the
// utility code...didn't want that huge table distracting or intimidating
// folks before even getting into the real substance of the program, and
// the compiler permits forward references to functions but not data.
// TODO when inlining this function, it is not found in main.cpp
byte gamma(byte x) {
  return pgm_read_byte(&gammaTable[x]);
}

CRGB gamma(CRGB c) {
  return CRGB(
    gamma(c.r),
    gamma(c.g),
    gamma(c.b)
  );
}

