#include "Layer.h"

Layer::Layer(CRGB * theTarget, CRGB * theScratch, int * theTMeta) {
  orientation = ORIENTATION_LINEAR;
  target = theTarget;
  scratch = theScratch;
  tmeta = theTMeta;

  setRandomEffect();
}

// Randomly pick next image effect and trans effect indices:
void Layer::transitionStart() {
  setTransition( random8(TRANSITION_NUM) );
}

void Layer::setRandomEffect() {
  setEffect( random8(AUTO_EFFECT_NUM) );
}

void Layer::setEffect(byte id) {
  effect       = id;
  initEffect();
}

void Layer::initEffect() {
  orientation     = getEffectOrientation(effect);
  if (orientation == ORIENTATION_NIL)
    orientation = static_cast<Orientation>(random8(NUM_ORIENTATIONS));
  effectInit(effect)(meta, pixelCount());
}

void Layer::setTransition(byte id) {
  transititionIdx = id;
  transitionInit(transititionIdx)(tmeta, pixelCount());
}

int Layer::pixelCount() {
  switch(orientation) {
    case ORIENTATION_LINEAR: return(STRIP_PIXEL_COUNT);
    case ORIENTATION_FLOOR:  return(FLOOR_PIXEL_COUNT);
    default: return(0);
  }
}

void Layer::render() {
  switch(orientation) {
    case ORIENTATION_LINEAR: renderLinear(); break;
    case ORIENTATION_FLOOR: renderFloor(); break;
    default: break;
  }

  effectStep(effect)(meta, pixelCount());
}

void Layer::renderComposite() {
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
  CRGB nxt;
  effectPixel(effect)(meta, &nxt, 0, pixelCount());
  *preview = blend(*preview, nxt, 127);
  effectStep(effect)(meta, pixelCount());
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
