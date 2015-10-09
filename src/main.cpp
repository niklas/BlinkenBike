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

#include <avr/pgmspace.h>
#include "TimerOne.h"
#include "Trigometry.h"
#include "Colors.h"
#include "Settings.h"
#include "Effects.h"
#include "Transitions.h"
#include "Layout.h"
#include "Layer.h"
#include "FastLED.h"
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



void callback();

// ---------------------------------------------------------------------------

void setup() {
  FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(strip, STRIP_PIXEL_COUNT).setCorrection(TypicalLEDStrip);
#ifdef BENCHMARK_FPS
  Serial.begin(9600);
#endif

  // Initialize random number generator from a floating analog input.
  randomSeed(analogRead(0));
  backImgIdx        = 0;
  tCounter = -1;

#ifdef FPS_BY_TIMER
  // Timer1 is used so the strip will update at a known fixed frame rate.
  // Each effect rendering function varies in processing complexity, so
  // the timer allows smooth transitions between effects (otherwise the
  // effects and transitions would jump around in speed...not attractive).
  Timer1.initialize();
  Timer1.attachInterrupt(callback, 1000000 / FPS); // XX frames/second
#endif
}

#ifndef FPS_BY_TIMER
unsigned long startedAt = millis();
unsigned int wait;

void loop() {
  // try keep a constant framerate
  wait = MICROS_PER_FRAME - ( millis() - startedAt );
  if ( (wait > 0) && (wait < 100)) delay(wait);
  startedAt = millis();

  frameCount++;

#ifdef BENCHMARK_FPS
  if (frameCount % BENCHMARK_EVERY == 0) {
    end_benchmark(BENCHMARK_EVERY);
    start_benchmark();
  }
#endif

  callback();

}
#else
void loop() { } // using Timer in setup()
#endif

// Timer1 interrupt handler.  Called at equal intervals; 60 Hz by default.
void callback() {
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
    end_benchmark(frameCount);
    frameCount = 1;
    start_benchmark();
  }
#endif

  int pix;
  int frntImgIdx = 1 - backImgIdx;
  byte * imgPtr;


  //////////////////////////////////////////////////////////////
  // Primary effect (background)
  //////////////////////////////////////////////////////////////
  layer[backImgIdx].render();


#ifdef DO_TRANSITION
  //////////////////////////////////////////////////////////////
  // Secondary effect (foreground) during transition in progress
  //////////////////////////////////////////////////////////////
  if (tCounter > 0) {
    layer[frntImgIdx].renderComposite();
  }
#endif


  //////////////////////////////////////////////////////////////
  // Count up to next transition (or end of current one):
  //////////////////////////////////////////////////////////////
  tCounter++;
  if(tCounter == 0) { // Transition start

    layer[frntImgIdx].transitionStart();
    transitionTime         = random(FPS/2, 3 * FPS);
  } else if (tCounter >= transitionTime) { // End transition
    backImgIdx             = 1 - backImgIdx;     // Invert back index
    tCounter               = - random(2 * FPS, 6 * FPS); // Hold image 2 to 6 seconds
  }
}
