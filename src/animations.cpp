#include <Arduino.h>
#include "animations.h"
#include "colors.h"
#include "trigometry.h"

// ---------------------------------------------------------------------------
// Image effect rendering functions.  Each effect is generated parametrically
// (that is, from a set of numbers, usually randomly seeded).  Because both
// back and front images may be rendering the same effect at the same time
// (but with different parameters), a distinct block of parameter memory is
// required for each image.  The 'fxVars' array is a two-dimensional array
// of integers, where the major axis is either 0 or 1 to represent the two
// images, while the minor axis holds 50 elements -- this is working scratch
// space for the effect code to preserve its "state."  The meaning of each
// element is generally unique to each rendering effect, but the first element
// is most often used as a flag indicating whether the effect parameters have
// been initialized yet.  When the back/front image indexes swap at the end of
// each transition, the corresponding set of fxVars, being keyed to the same
// indexes, are automatically carried with them.

// Simplest rendering effect: fill entire image with solid color
void renderEffect00(ANIMATION_SIGNATURE) {
  // Only needs to be rendered once, when effect is initialized:
  if(meta[0] == 0) {
    meta[1] = random(256); // R
    meta[2] = random(256); // G
    meta[3] = random(256); // B
    meta[0] = 1; // Effect initialized
  }
  for(int i=0; i<numPixels; i++) {
    *img++ = meta[1]; *img++ = meta[2]; *img++ = meta[3];
  }
}

// Rainbow effect (1 or more full loops of color wheel at 100% saturation).
// Not a big fan of this pattern (it's way overused with LED stuff), but it's
// practically part of the Geneva Convention by now.
void renderEffect01(ANIMATION_SIGNATURE) {
  if(meta[0] == 0) { // Initialize effect?
    // Number of repetitions (complete loops around color wheel); any
    // more than 4 per meter just looks too chaotic and un-rainbow-like.
    // Store as hue 'distance' around complete belt:
    meta[1] = (1 + random(4 * ((numPixels + 31) / 32))) * 1536;
    // Frame-to-frame hue increment (speed) -- may be positive or negative,
    // but magnitude shouldn't be so small as to be boring.  It's generally
    // still less than a full pixel per frame, making motion very smooth.
    meta[2] = 4 + random(meta[1]) / numPixels;
    // Reverse speed and hue shift direction half the time.
    if(random(2) == 0) meta[1] = -meta[1];
    if(random(2) == 0) meta[2] = -meta[2];
    meta[3] = 0; // Current position
    meta[0] = 1; // Effect initialized
  }

  long color, i;
  for(i=0; i<numPixels; i++) {
    color = hsv2rgb(meta[3] + meta[1] * i / numPixels,
      255, 255);
    *img++ = color >> 16; *img++ = color >> 8; *img++ = color;
  }
  meta[3] += meta[2];
}

// Sine wave chase effect
void renderEffect02(ANIMATION_SIGNATURE) {
  if(meta[0] == 0) { // Initialize effect?
    meta[1] = random(1536); // Random hue
    // Number of repetitions (complete loops around color wheel);
    // any more than 4 per meter just looks too chaotic.
    // Store as distance around complete belt in half-degree units:
    meta[2] = (1 + random(4 * ((numPixels + 31) / 32))) * 720;
    // Frame-to-frame increment (speed) -- may be positive or negative,
    // but magnitude shouldn't be so small as to be boring.  It's generally
    // still less than a full pixel per frame, making motion very smooth.
    meta[3] = 4 + random(meta[1]) / numPixels;
    // Reverse direction half the time.
    if(random(2) == 0) meta[3] = -meta[3];
    meta[4] = 0; // Current position
    meta[0] = 1; // Effect initialized
  }

  int  foo;
  long color;
  for(long i=0; i<numPixels; i++) {
    foo = fixSin(meta[4] + meta[2] * i / numPixels);
    // Peaks of sine wave are white, troughs are black, mid-range
    // values are pure hue (100% saturated).
    color = (foo >= 0) ?
       hsv2rgb(meta[1], 254 - (foo * 2), 255) :
       hsv2rgb(meta[1], 255, 254 + foo * 2);
    *img++ = color >> 16; *img++ = color >> 8; *img++ = color;
  }
  meta[4] += meta[3];
}

// Data for American-flag-like colors (20 pixels representing
// blue field, stars and stripes).  This gets "stretched" as needed
// to the full LED strip length in the flag effect code, below.
// Can change this data to the colors of your own national flag,
// favorite sports team colors, etc.  OK to change number of elements.
#define C_RED   160,   0,   0
#define C_WHITE 255, 255, 255
#define C_BLUE    0,   0, 100
PROGMEM const unsigned char flagTable[]  = {
  C_BLUE , C_WHITE, C_BLUE , C_WHITE, C_BLUE , C_WHITE, C_BLUE,
  C_RED  , C_WHITE, C_RED  , C_WHITE, C_RED  , C_WHITE, C_RED ,
  C_WHITE, C_RED  , C_WHITE, C_RED  , C_WHITE, C_RED };

// Wavy flag effect
void renderEffect_flag(ANIMATION_SIGNATURE) {
  long i, sum, s, x;
  int  idx1, idx2, a, b;
  if(meta[0] == 0) { // Initialize effect?
    meta[1] = 720 + random(720); // Wavyness
    meta[2] = 4 + random(10);    // Wave speed
    meta[3] = 200 + random(200); // Wave 'puckeryness'
    meta[4] = 0;                 // Current  position
    meta[0] = 1;                 // Effect initialized
  }
  for(sum=0, i=0; i<numPixels-1; i++) {
    sum += meta[3] + fixCos(meta[4] + meta[1] *
      i / numPixels);
  }

  for(s=0, i=0; i<numPixels; i++) {
    x = 256L * ((sizeof(flagTable) / 3) - 1) * s / sum;
    idx1 =  (x >> 8)      * 3;
    idx2 = ((x >> 8) + 1) * 3;
    b    = (x & 255) + 1;
    a    = 257 - b;
    *img++ = ((pgm_read_byte(&flagTable[idx1    ]) * a) +
              (pgm_read_byte(&flagTable[idx2    ]) * b)) >> 8;
    *img++ = ((pgm_read_byte(&flagTable[idx1 + 1]) * a) +
              (pgm_read_byte(&flagTable[idx2 + 1]) * b)) >> 8;
    *img++ = ((pgm_read_byte(&flagTable[idx1 + 2]) * a) +
              (pgm_read_byte(&flagTable[idx2 + 2]) * b)) >> 8;
    s += meta[3] + fixCos(meta[4] + meta[1] *
      i / numPixels);
  }

  meta[4] += meta[2];
  if(meta[4] >= 720) meta[4] -= 720;
}

// TO DO: Add more effects here...Larson scanner, etc.
