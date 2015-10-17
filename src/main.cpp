// Define HardwareSerial_h to suppress loading of the Arduino standard Serial class (saves 173 bytes)
#define HardwareSerial_h

#include "FastLED.h"

FASTLED_USING_NAMESPACE
#include <avr/pgmspace.h>
#include "Gamma.h"
#include "Settings.h"
#include "Effects.h"
#include "Transitions.h"
#include "Layout.h"
#include "Layer.h"
#include "ModeManager.h"

CRGB strip[STRIP_PIXEL_COUNT],  // Data for 1 strip worth of imagery
     preview[PREVIEW_PIXEL_COUNT],
     tmpPixels[FLOOR_PIXEL_COUNT];
byte backImgIdx, frntImgIdx;    // Index of 'back'/'front' image (always 0 or 1)
int  transVars[FX_VARS_NUM];    // Alpha transition instance variables


Layer layer[2] = {
  Layer(strip, tmpPixels, transVars),
  Layer(strip, tmpPixels, transVars)
};



void frame();

// ---------------------------------------------------------------------------

void setup() {
  FastLED.addLeds<LED_TYPE,PIN_STRIP_DATA,PIN_STRIP_CLK,COLOR_ORDER>(strip, STRIP_PIXEL_COUNT).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,PIN_PREVIEW_DATA,PIN_PREVIEW_CLK,COLOR_ORDER>(preview, PREVIEW_PIXEL_COUNT).setCorrection(TypicalLEDStrip);
  //FastLED.setBrightness(36);
  FastLED.setMaxRefreshRate(FPS);

  // Initialize random number generator from a floating analog input.
  random16_set_seed(analogRead(0));
  backImgIdx        = 0;
  tCounter = -1;
  effectDuration = 23; // whatever
  frameCount = 0;


  preview[0] = CRGB::Red;
  preview[1] = CRGB::Green;
  preview[2] = CRGB::Blue;
  preview[3] = CRGB::Purple;
}

void loop() {
  // Very first thing here is to issue the strip data generated from the
  // *previous* callback.  It's done this way on purpose because show() is
  // roughly constant-time, so the refresh will always occur on a uniform
  // beat with respect to the Timer1 interrupt.  The various effects
  // rendering and compositing code is not constant-time, and that
  // unevenness would be apparent if show() were called at the end.
  FastLED.show();
  frameCount++;

#ifdef BENCHMARK_FPS
  if (frameCount % BENCHMARK_EVERY == 0) {
    end_benchmark(BENCHMARK_EVERY);
    start_benchmark();
  }
#endif

  frame();

  EVERY_N_MILLISECONDS(100) {
    mode.readInputs();

    if (mode.shouldAutoTransition()) {
      // quickly come back from long durations
      if (tCounter < - EFFECT_DURATION_STRETCH * mode.effectDurationBase)
        tCounter = - mode.effectDurationBase;
    } else {
      // start the new transition the moment we leave lock mode
      tCounter = tCounter < 0 ? -1 : 1;
    }
  }


}

void forceEffect(byte effect) {
  if (layer[backImgIdx].effect != effect) {     // transition to emergency not finished yet
    if (layer[frntImgIdx].effect != effect) {   // transition to emergency not started yet.
      layer[frntImgIdx].setEffect(effect);
      transitionTime = 2 * FPS;
      tCounter = 0;
    }
  } else {
    tCounter = -10; // already activated, keep
  }
}

// Timer1 interrupt handler.  Called at equal intervals; 60 Hz by default.
void frame() {

  frntImgIdx = 1 - backImgIdx;


  if (mode.toggle1 == 1) {
    forceEffect(Effect_stvzo67);
  } else if (mode.toggle2 == 1) {
    forceEffect(Effect_usa_police);
  }

  //////////////////////////////////////////////////////////////
  // Primary effect (background)
  //////////////////////////////////////////////////////////////
  layer[backImgIdx].render();


  //////////////////////////////////////////////////////////////
  // Secondary effect (foreground) during transition in progress
  //////////////////////////////////////////////////////////////
  if (mode.shouldAutoTransition()) {
    if (tCounter > 0) {
      layer[frntImgIdx].renderComposite();
    }

    //////////////////////////////////////////////////////////////
    // Count up to next transition (or end of current one):
    //////////////////////////////////////////////////////////////
    tCounter++;

    if (tCounter == 0) { // Transition start
      layer[frntImgIdx].transitionStart();
      transitionTime = random16(FPS/2, 3 * FPS);
    }

    if (tCounter >= transitionTime) {
      backImgIdx  = 1 - backImgIdx;     // Invert back index
      effectDuration = mode.randomEffectDuration();
      tCounter    = - effectDuration;
    }
  }

  //////////////////////////////////////////////////////////////
  // Additional Effects
  //////////////////////////////////////////////////////////////

  if (mode.triggered) {
    strip[23] = CRGB::Purple;
  }


  //////////////////////////////////////////////////////////////
  // Status / Preview
  //////////////////////////////////////////////////////////////
  LED_TICK = CRGB::Black;

  if (mode.shouldAutoTransition()) {
    LED_STATUS = CRGB::Green;
    if (tCounter < 0) {
      if ( (-tCounter < effectDuration >>2) && (effectDuration % -tCounter < 5)) {
        // in the last quarter while effect is shown, blink LED faster
        LED_TICK = CRGB::Purple;
      }
    }
  } else {
    LED_STATUS = CRGB::Red;
  }

  //////////////////////////////////////////////////////////////
  // apply gamma
  //////////////////////////////////////////////////////////////
  for (byte pixel=0; pixel < STRIP_PIXEL_COUNT; pixel++) {
    strip[pixel] = gamma(strip[pixel]);
  }
  for (byte pixel=0; pixel < PREVIEW_PIXEL_COUNT; pixel++) {
    preview[pixel] = gamma(preview[pixel]);
  }
}
