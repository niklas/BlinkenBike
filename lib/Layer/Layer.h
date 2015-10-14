#ifndef __LAYER_H__
#define __LAYER_H__

#include <Arduino.h>
#include "Settings.h"
#include "Effects.h"
#include "Transitions.h"
#include "Layout.h"
#include "FastLED.h"

#define ORIENTATION_INTERPOLATION 80

class Layer {
  public:
    Layer(CRGB *, CRGB *, int * t);
    void render();
    void renderComposite();
    void transitionStart();
    void setEffect(byte);
    void setTransition(byte);
    int pixelCount();
  private:
    Orientation orientation;
    byte transititionIdx;           // which Alpha transition to run
    byte effectIdx;
    int meta[FX_VARS_NUM];

    int * tmeta;
    CRGB * target;
    CRGB * scratch;

    void renderLinear();
    void renderFloor();
    void renderCompositeLinear();
    void renderCompositeFloor();

    void mapFloorToLinear();
    void composeFloorToLinear();
};

#endif
