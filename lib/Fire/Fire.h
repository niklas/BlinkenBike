#ifndef __FIRE_H__
#define __FIRE_H__

#include "Settings.h"
#include <FastLED.h>

void Fire__init(byte *, byte);
void Fire__eachStep(byte *, byte, byte, byte, byte);
int Wipe__y(int, int, byte, int);

#endif

