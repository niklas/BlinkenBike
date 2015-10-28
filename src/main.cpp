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

byte seatOnFire;
byte seatFire[SEAT_FIRE_HEIGHT];


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

  Fire__init(seatFire, SEAT_FIRE_HEIGHT);
  seatOnFire = 0;


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
    if (tCounter >= 0) tCounter -= 2; // animate back
    else tCounter = -2; // already activated, keep
  }
}

void fadeWithLight(byte pos, byte fade, CRGB color) {
  strip[pos] %= fade;
  strip[pos] += color;
}

void fartEffect() {
  byte pixel, p, fade;
  CRGB color;

  Fire__eachStep(seatFire, SEAT_FIRE_HEIGHT,
                 255 - scale8(seatOnFire, SEAT_FIRE_WARMING),
                 scale8(seatOnFire, SEAT_FIRE_SPARKING),
                 SEAT_FIRE_BASE);

  fade  = 255 - scale8(seatOnFire, SEAT_FIRE_TRANS);
  for (pixel = 0; pixel < SEAT_FIRE_HEIGHT; pixel++) {
    color = HeatColor(seatFire[pixel]).fadeLightBy(255-seatOnFire);
    fadeWithLight(FirstOnSeatTube+pixel, fade, color);

    if (pixel % 2 == 0) {
      p = pixel >> 1;
      fadeWithLight(LastOnSeatLeft-p, fade, color);
      fadeWithLight(LastOnSeatRight-p, fade, color);
    }
  }
}

// Timer1 interrupt handler.  Called at equal intervals; 60 Hz by default.
void frame() {
  byte pixel;

  frntImgIdx = 1 - backImgIdx;


  if (tCounter < 0) { // no animation going
    if (mode.toggle1 == 1) {
      forceEffect(Effect_stvzo67);
    } else if (mode.toggle2 == 1) {
      forceEffect(Effect_usa_police);
    }
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
      // select the next effect so we can preview it
      layer[backImgIdx].setRandomEffect();

      backImgIdx  = 1 - backImgIdx;     // Invert back index
      effectDuration = mode.randomEffectDuration();
      tCounter    = - effectDuration;
    }
  }

  //////////////////////////////////////////////////////////////
  // Additional Effects
  //////////////////////////////////////////////////////////////

  if (mode.triggered) {
    seatOnFire = qadd8(seatOnFire, 23);
  } else {
    if (seatOnFire > 0) seatOnFire = qsub8(seatOnFire, 16);
  }
  if (seatOnFire > 0) fartEffect();


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

  if (mode.shouldAutoTransition() && (tCounter < 0)) {
    layer[frntImgIdx].renderPreview(&LED_PREVIEW_EFFECT);
  } else {
    LED_PREVIEW_EFFECT = CRGB::Black;
  }
}
