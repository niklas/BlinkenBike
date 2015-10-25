#ifndef __FIRE_H__
#define __FIRE_H__

// FIRE_COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100
  #define FIRE_COOLING 50

// FIRE_SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
  #define FIRE_SPARKING 100

// FIRE_BASE: How many pixels release new heat sources.
  #define FIRE_BASE 6


#include <FastLED.h>

void Fire__init(byte *, byte);
void Fire__eachStep(byte *, byte);

#endif

