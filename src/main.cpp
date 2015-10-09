// THIS PROGRAM *WILL* *NOT* *WORK* ON REALLY LONG LED STRIPS.  IT USES
// AN INORDINATE AMOUNT OF RAM IN ORDER TO ACHIEVE ITS BUTTERY-SMOOTH
// ANIMATION.  See the 'strandtest' sketch for a simpler and less RAM-
// intensive example that can process more LEDs (100+).

// ALSO: NOT COMPATIBLE WITH TRINKET OR GEMMA for way too many reasons.

// Example to control LPD8806-based RGB LED Modules in a strip; originally
// intended for the Adafruit Digital Programmable LED Belt Kit.
// REQUIRES TIMER1 LIBRARY: http://www.arduino.cc/playground/Code/Timer1
// ALSO REQUIRES LPD8806 LIBRARY, which should be included with this code.

// I'm generally not fond of canned animation patterns.  Wanting something
// more nuanced than the usual 8-bit beep-beep-boop-boop pixelly animation,
// this program smoothly cycles through a set of procedural animated effects
// and transitions -- it's like a Video Toaster for your waist!  Some of the
// coding techniques may be a bit obtuse (e.g. function arrays), so novice
// programmers may have an easier time starting out with the 'strandtest'
// program also included with the LPD8806 library.

#include "FastLED.h"

FASTLED_USING_NAMESPACE
#include <avr/pgmspace.h>
#include "Gamma.h"
#include "Settings.h"
#include "Effects.h"
#include "Transitions.h"
#include "Layout.h"
#include "Layer.h"
#ifdef BENCHMARK_FPS
#include "benchmark.h"
#endif

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
#ifdef BENCHMARK_FPS
  Serial.begin(9600);
#endif

  Serial.begin(9600);
  pinMode(PIN_POT_SIDE, INPUT);

  // Initialize random number generator from a floating analog input.
  randomSeed(analogRead(0));
  backImgIdx        = 0;
  tCounter = -1;
  shouldAutoTransition = true;
  effectDurationBase = 5 * FPS;
}

int pot;
unsigned long startedAt = millis();
unsigned int wait;

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
    shouldAutoTransition = pot < 5 ? false : true;
    effectDurationBase = map(pot , 5, 1023, 5 * FPS, 2);
    Serial.println(effectDurationBase);
    Serial.println(shouldAutoTransition);
  }

  // try keep a constant framerate
  wait = MICROS_PER_FRAME - ( millis() - startedAt );
  if ( (wait > 0) && (wait < 100)) FastLED.delay(wait);
  startedAt = millis();

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
    Serial.println(F("transition start"));
    layer[frntImgIdx].transitionStart();
    transitionTime = random(FPS/2, 3 * FPS);
  }

  if (tCounter >= transitionTime) {
    if (shouldAutoTransition) {
      Serial.println("transition done");
      backImgIdx             = 1 - backImgIdx;     // Invert back index
      tCounter = - random(effectDurationBase, 6 * effectDurationBase);
    } else {
      tCounter = 1; // do not start a new transition
    }
  }
}
