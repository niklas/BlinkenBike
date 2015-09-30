#include <Arduino.h>
#include "animations.h"
#include "colors.h"
#include "trigometry.h"

// List of image effect and alpha channel rendering functions; the code for
// each of these appears later in this file.  Just a few to start with...
// simply append new ones to the appropriate list here:
void (*renderEffect[])() = {
  renderEffect00,
  renderEffect01,
  renderEffect02,
  renderEffect03 };

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
void renderEffect00() {
  // Only needs to be rendered once, when effect is initialized:
  if(fxVars[0] == 0) {
    fxVars[1] = random(256); // R
    fxVars[2] = random(256); // G
    fxVars[3] = random(256); // B
    fxVars[0] = 1; // Effect initialized
  }
  byte *ptr = &imgData[0];
  for(int i=0; i<numPixels; i++) {
    *ptr++ = fxVars[1]; *ptr++ = fxVars[2]; *ptr++ = fxVars[3];
  }
}

// Rainbow effect (1 or more full loops of color wheel at 100% saturation).
// Not a big fan of this pattern (it's way overused with LED stuff), but it's
// practically part of the Geneva Convention by now.
void renderEffect01() {
  if(fxVars[0] == 0) { // Initialize effect?
    // Number of repetitions (complete loops around color wheel); any
    // more than 4 per meter just looks too chaotic and un-rainbow-like.
    // Store as hue 'distance' around complete belt:
    fxVars[1] = (1 + random(4 * ((numPixels + 31) / 32))) * 1536;
    // Frame-to-frame hue increment (speed) -- may be positive or negative,
    // but magnitude shouldn't be so small as to be boring.  It's generally
    // still less than a full pixel per frame, making motion very smooth.
    fxVars[2] = 4 + random(fxVars[1]) / numPixels;
    // Reverse speed and hue shift direction half the time.
    if(random(2) == 0) fxVars[1] = -fxVars[1];
    if(random(2) == 0) fxVars[2] = -fxVars[2];
    fxVars[3] = 0; // Current position
    fxVars[0] = 1; // Effect initialized
  }

  byte *ptr = &imgData[0];
  long color, i;
  for(i=0; i<numPixels; i++) {
    color = hsv2rgb(fxVars[3] + fxVars[1] * i / numPixels,
      255, 255);
    *ptr++ = color >> 16; *ptr++ = color >> 8; *ptr++ = color;
  }
  fxVars[3] += fxVars[2];
}

// Sine wave chase effect
void renderEffect02() {
  if(fxVars[0] == 0) { // Initialize effect?
    fxVars[1] = random(1536); // Random hue
    // Number of repetitions (complete loops around color wheel);
    // any more than 4 per meter just looks too chaotic.
    // Store as distance around complete belt in half-degree units:
    fxVars[2] = (1 + random(4 * ((numPixels + 31) / 32))) * 720;
    // Frame-to-frame increment (speed) -- may be positive or negative,
    // but magnitude shouldn't be so small as to be boring.  It's generally
    // still less than a full pixel per frame, making motion very smooth.
    fxVars[3] = 4 + random(fxVars[1]) / numPixels;
    // Reverse direction half the time.
    if(random(2) == 0) fxVars[3] = -fxVars[3];
    fxVars[4] = 0; // Current position
    fxVars[0] = 1; // Effect initialized
  }

  byte *ptr = &imgData[0];
  int  foo;
  long color;
  for(long i=0; i<numPixels; i++) {
    foo = fixSin(fxVars[4] + fxVars[2] * i / numPixels);
    // Peaks of sine wave are white, troughs are black, mid-range
    // values are pure hue (100% saturated).
    color = (foo >= 0) ?
       hsv2rgb(fxVars[1], 254 - (foo * 2), 255) :
       hsv2rgb(fxVars[1], 255, 254 + foo * 2);
    *ptr++ = color >> 16; *ptr++ = color >> 8; *ptr++ = color;
  }
  fxVars[4] += fxVars[3];
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
void renderEffect03() {
  long i, sum, s, x;
  int  idx1, idx2, a, b;
  if(fxVars[0] == 0) { // Initialize effect?
    fxVars[1] = 720 + random(720); // Wavyness
    fxVars[2] = 4 + random(10);    // Wave speed
    fxVars[3] = 200 + random(200); // Wave 'puckeryness'
    fxVars[4] = 0;                 // Current  position
    fxVars[0] = 1;                 // Effect initialized
  }
  for(sum=0, i=0; i<numPixels-1; i++) {
    sum += fxVars[3] + fixCos(fxVars[4] + fxVars[1] *
      i / numPixels);
  }

  byte *ptr = &imgData[0];
  for(s=0, i=0; i<numPixels; i++) {
    x = 256L * ((sizeof(flagTable) / 3) - 1) * s / sum;
    idx1 =  (x >> 8)      * 3;
    idx2 = ((x >> 8) + 1) * 3;
    b    = (x & 255) + 1;
    a    = 257 - b;
    *ptr++ = ((pgm_read_byte(&flagTable[idx1    ]) * a) +
              (pgm_read_byte(&flagTable[idx2    ]) * b)) >> 8;
    *ptr++ = ((pgm_read_byte(&flagTable[idx1 + 1]) * a) +
              (pgm_read_byte(&flagTable[idx2 + 1]) * b)) >> 8;
    *ptr++ = ((pgm_read_byte(&flagTable[idx1 + 2]) * a) +
              (pgm_read_byte(&flagTable[idx2 + 2]) * b)) >> 8;
    s += fxVars[3] + fixCos(fxVars[4] + fxVars[1] *
      i / numPixels);
  }

  fxVars[4] += fxVars[2];
  if(fxVars[4] >= 720) fxVars[4] -= 720;
}

// TO DO: Add more effects here...Larson scanner, etc.
