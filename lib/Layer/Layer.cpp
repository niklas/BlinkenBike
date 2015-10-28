#include "Layer.h"

Layer::Layer(CRGB * theTarget, CRGB * theScratch, int * theTMeta) {
  orientation = ORIENTATION_LINEAR;
  target = theTarget;
  scratch = theScratch;
  effect = 0;
  tmeta = theTMeta;
}

// Randomly pick next image effect and trans effect indices:
void Layer::transitionStart() {
  setEffect( random8(AUTO_EFFECT_NUM) );
  setTransition( random8(TRANSITION_NUM) );
}

void Layer::setEffect(byte id) {
  effect       = id;
  orientation     = getEffectOrientation(effect);
  if (orientation == ORIENTATION_NIL)
    orientation = static_cast<Orientation>(random8(NUM_ORIENTATIONS));
  meta[0]         = 0; // Effect not yet initialized
}

void Layer::setTransition(byte id) {
  transititionIdx = id;
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
    effectInit(effect)(meta, pixelCount());
  }

  switch(orientation) {
    case ORIENTATION_LINEAR: renderLinear(); break;
    case ORIENTATION_FLOOR: renderFloor(); break;
    default: break;
  }

  effectStep(effect)(meta, pixelCount());
}

void Layer::renderComposite() {
  if (meta[0] == 0) {
    effectInit(effect)(meta, pixelCount());
  }
  if (tmeta[0] == 0) {
    transitionInit(transititionIdx)(tmeta, pixelCount());
  }

  switch(orientation) {
    case ORIENTATION_LINEAR: renderCompositeLinear(); break;
    case ORIENTATION_FLOOR: renderCompositeFloor(); break;
    default: break;
  }

  effectStep(effect)(meta, pixelCount());
}

void Layer::renderLinear() {
  for(int pix = 0; pix < STRIP_PIXEL_COUNT; pix++) {
    effectPixel(effect)(meta, &target[pix], pix, STRIP_PIXEL_COUNT);
  }
}

void Layer::renderPreview(CRGB * preview) {
  effectPixel(effect)(meta, preview, 0, FLOOR_PIXEL_COUNT);
}

void Layer::renderFloor() {
  for(int pix = 0; pix < FLOOR_PIXEL_COUNT; pix++) {
    effectPixel(effect)(meta, &scratch[pix], pix, FLOOR_PIXEL_COUNT);
  }

  mapFloorToLinear();
}

void Layer::renderCompositeLinear() {
  int trans;

  for(int pix = 0; pix < STRIP_PIXEL_COUNT; pix++) {
    effectPixel(effect)(meta, &scratch[0], pix, STRIP_PIXEL_COUNT);

    // calculate trans btwn 1-256 so we can do a shift devide
    transitionPixel(transititionIdx)(tmeta, &trans, pix, STRIP_PIXEL_COUNT);

    target[pix] = blend(target[pix], scratch[0], trans);
  }
}

void Layer::renderCompositeFloor() {
  for(int pix = 0; pix < FLOOR_PIXEL_COUNT; pix++) {
    effectPixel(effect)(meta, &scratch[pix], pix, FLOOR_PIXEL_COUNT);
  }

  composeFloorToLinear();
}

void Layer::mapFloorToLinear() {
  int f,l;
  CRGB pixel;
  for(l = 0; l < STRIP_PIXEL_COUNT; l++) {
    f = floorMap(l);

    pixel = scratch[f];
    if (f < FLOOR_PIXEL_COUNT-1) {
      pixel = blend(pixel, scratch[f+1], ORIENTATION_INTERPOLATION);
    }
    target[l] = pixel;
  }
}

void Layer::composeFloorToLinear() {
  int f,l;
  int trans;
  CRGB pixel;
  for(l = 0; l < STRIP_PIXEL_COUNT; l++) {
    f = floorMap(l);

    // calculate trans btwn 1-256 so we can do a shift devide
    transitionPixel(transititionIdx)(tmeta, &trans, l, STRIP_PIXEL_COUNT);

    pixel = blend(target[l], scratch[f], trans);
    if (f < FLOOR_PIXEL_COUNT-1) {
      pixel = blend(pixel, scratch[f+1], ORIENTATION_INTERPOLATION);
    }
    target[l] = pixel;
  }
}
