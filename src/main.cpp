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

CRGB strip[STRIP_PIXEL_COUNT],  // Data for 1 strip worth of imagery
     tmpPixels[FLOOR_PIXEL_COUNT];
byte backImgIdx;                // Index of 'back' image (always 0 or 1)
int  transVars[FX_VARS_NUM];    // Alpha transition instance variables


unsigned long frameCount = 0;


Layer layer[2] = {
  Layer(strip, tmpPixels, transVars),
  Layer(strip, tmpPixels, transVars)
};



void frame();

// ---------------------------------------------------------------------------

void setup() {
  FastLED.addLeds<LED_TYPE,PIN_STRIP_DATA,PIN_STRIP_CLK,COLOR_ORDER>(strip, STRIP_PIXEL_COUNT).setCorrection(TypicalLEDStrip);
  //FastLED.setBrightness(36);
  FastLED.setMaxRefreshRate(FPS);

  pinMode(PIN_POT_SIDE, INPUT);

  // Initialize random number generator from a floating analog input.
  randomSeed(analogRead(0));
  backImgIdx        = 0;
  tCounter = -1;
  shouldAutoTransition = true;
  effectDurationBase = 5 * FPS;
}

int pot;

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

  EVERY_N_MILLISECONDS(400) {
    pot = analogRead(PIN_POT_SIDE);
    shouldAutoTransition = pot < EFFECT_DURATION_POTI_STOP ? false : true;
    effectDurationBase = FPS + pow(__potiBase, pot) * EFFECT_DURATION_MAX_SECONDS * FPS;

    // quickly come back from long durations
    if (tCounter < - EFFECT_DURATION_STRETCH * effectDurationBase)
      tCounter = - effectDurationBase;
  }

}

// Timer1 interrupt handler.  Called at equal intervals; 60 Hz by default.
void frame() {

  int frntImgIdx = 1 - backImgIdx;


  //////////////////////////////////////////////////////////////
  // Primary effect (background)
  //////////////////////////////////////////////////////////////
  layer[backImgIdx].render();


#ifdef DO_TRANSITION
  //////////////////////////////////////////////////////////////
  // Secondary effect (foreground) during transition in progress
  //////////////////////////////////////////////////////////////
  if (shouldAutoTransition) {
    if (tCounter > 0) {
      layer[frntImgIdx].renderComposite();
    }
  }
#endif


  //////////////////////////////////////////////////////////////
  // apply gamma
  //////////////////////////////////////////////////////////////
  for (byte pixel=0; pixel < STRIP_PIXEL_COUNT; pixel++) {
    strip[pixel] = gamma(strip[pixel]);
  }



  //////////////////////////////////////////////////////////////
  // Count up to next transition (or end of current one):
  //////////////////////////////////////////////////////////////
  tCounter++;
  if (shouldAutoTransition && (tCounter == 0)) { // Transition start
    layer[frntImgIdx].transitionStart();
    transitionTime = random(FPS/2, 3 * FPS);
  }

  if (tCounter >= transitionTime) {
    if (shouldAutoTransition) {
      backImgIdx             = 1 - backImgIdx;     // Invert back index
      tCounter = - random(effectDurationBase, EFFECT_DURATION_STRETCH * effectDurationBase);
    } else {
      tCounter = 1; // do not start a new transition
    }
  }
}
