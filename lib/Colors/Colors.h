#ifndef __COLORS_H__
#define __COLORS_H__

#include <Arduino.h>
#include <avr/pgmspace.h>

byte gamma(byte x);
long hsv2rgb(long h, byte s, byte v);

#endif
