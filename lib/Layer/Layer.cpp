#include "Layer.h"

Layer::Layer(byte * theTarget, byte * theScratch, int * theTMeta) {
  orientation = ORIENTATION_LINEAR;
  target = theTarget;
  scratch = theScratch;
  effectIdx = 0;
  tmeta = theTMeta;
}

// Randomly pick next image effect and trans effect indices:
void Layer::transitionStart() {
  effectIdx       = random(EFFECT_NUM);
  transititionIdx = random(TRANSITION_NUM);
  orientation     = static_cast<Orientation>(random(NUM_ORIENTATIONS));
  orientation = ORIENTATION_FLOOR;
  meta[0]         = 0; // Effect not yet initialized
  tmeta[0]        = 0; // Transition not yet initialized
}

int Layer::pixelCount() {
  switch(orientation) {
    case ORIENTATION_LINEAR: return(STRIP_PIXEL_COUNT);
    case ORIENTATION_FLOOR:  return(FLOOR_PIXEL_COUNT);
    default: return(0);
  }
}

void Layer::render() {
  if (meta[0] == 0) {
    effectInit[effectIdx](meta, pixelCount());
  }

  switch(orientation) {
    case ORIENTATION_LINEAR: renderLinear(); break;
    case ORIENTATION_FLOOR: renderFloor(); break;
  }

  effectStep[effectIdx](meta, pixelCount());
}

void Layer::renderComposite() {
  if (meta[0] == 0) {
    effectInit[effectIdx](meta, pixelCount());
  }
  if (tmeta[0] == 0) {
    transitionInit[transititionIdx](tmeta, pixelCount());
  }

  switch(orientation) {
    case ORIENTATION_LINEAR: renderCompositeLinear(); break;
    case ORIENTATION_FLOOR: renderCompositeFloor(); break;
  }

  effectStep[effectIdx](meta, pixelCount());
}

void Layer::renderLinear() {
  for(int pix = 0; pix < STRIP_PIXEL_COUNT; pix++) {
    effectPixel[effectIdx](meta, &target[3*pix], pix, STRIP_PIXEL_COUNT);
  }
}

void Layer::renderFloor() {
  for(int pix = 0; pix < FLOOR_PIXEL_COUNT; pix++) {
    effectPixel[effectIdx](meta, &scratch[3*pix], pix, FLOOR_PIXEL_COUNT);
  }

  mapFloorToLinear();
}

void Layer::renderCompositeLinear() {
  byte * pixel;
  int trans, inv;

  for(int pix = 0; pix < STRIP_PIXEL_COUNT; pix++) {
    effectPixel[effectIdx](meta, &scratch[0], pix, STRIP_PIXEL_COUNT);

    // calculate trans btwn 1-256 so we can do a shift devide
    transitionPixel[transititionIdx](tmeta, &trans, pix, STRIP_PIXEL_COUNT);
    trans++;
    inv   = 257 - trans;

    pixel = &target[3*pix];
    pixel[0] = ( pixel[0] * inv + scratch[0] * trans ) >> 8;
    pixel[1] = ( pixel[1] * inv + scratch[1] * trans ) >> 8;
    pixel[2] = ( pixel[2] * inv + scratch[2] * trans ) >> 8;
  }
}

void Layer::renderCompositeFloor() {
  for(int pix = 0; pix < FLOOR_PIXEL_COUNT; pix++) {
    effectPixel[effectIdx](meta, &scratch[3*pix], pix, FLOOR_PIXEL_COUNT);
  }

  composeFloorToLinear();
}

void Layer::mapFloorToLinear() {
  int f,l;
  for(int l_led = 0; l_led < STRIP_PIXEL_COUNT; l_led++) {
    f = 3 * floorMap(l_led);
    l = 3 * l_led;

    target[l + 2] = scratch[f + 2];
    target[l + 1] = scratch[f + 1];
    target[l + 0] = scratch[f + 0];
  }
}

void Layer::composeFloorToLinear() {
  int f,l;
  int trans, inv;
  for(int l_led = 0; l_led < STRIP_PIXEL_COUNT; l_led++) {
    f = 3 * floorMap(l_led);
    l = 3 * l_led;

    // calculate trans btwn 1-256 so we can do a shift devide
    transitionPixel[transititionIdx](tmeta, &trans, l_led, STRIP_PIXEL_COUNT);
    trans++;
    inv   = 257 - trans;

    target[l + 2] = ( target[l + 2] * inv + scratch[f + 2] * trans) >> 8;
    target[l + 1] = ( target[l + 1] * inv + scratch[f + 1] * trans) >> 8;
    target[l + 0] = ( target[l + 0] * inv + scratch[f + 0] * trans) >> 8;
  }
}
