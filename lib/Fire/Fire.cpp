#include "Fire.h"

void Fire__init(byte * f, byte l) {
  byte h;
  // make everything cold
  for(h = 0; h < l; h++) {
    f[h] = 0;
  }
}

void Fire__eachStep(byte * f, byte l) {
  byte h;

  // Cool down every cell a little
  for(h = 0; h < l; h++) {
    f[h] = qsub8( f[h], random8(0, ((FIRE_COOLING * 10) / l) + 2));
  }

  //  Heat from each cell drifts 'up' and diffuses a little
  for(h = l - 1; h >= 2; h--) {
    f[h] = (f[h] + f[h - 1] + f[h - 2] ) / 3;
  }

  //  Randomly ignite new 'sparks' of heat near the bottom
  if( random8() < FIRE_SPARKING ) {
    byte y = random8(FIRE_BASE);
    f[y] = qadd8( f[y], random8(160,255) );
  }
}
