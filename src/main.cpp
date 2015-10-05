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
#include "SPI.h"
#include "LEDStrip.h"
#include "TimerOne.h"
#include "trigometry.h"
#include "colors.h"
#include "settings.h"
#include "effects.h"
#include "transitions.h"
#ifdef BENCHMARK_FPS
#include "benchmark.h"
#endif

byte imgData[numPixels * 3],    // Data for 1 strip worth of imagery
     layer[3],                  // RGB for one pixel
     backImgIdx,                // Index of 'back' image (always 0 or 1)
     fxIdx[2],                  // Effect # for back & front images
     transIdx;                  // which Alpha transition to run
int  fxVars[2][FX_VARS_NUM],    // Effect instance variables (explained later)
     transVars[FX_VARS_NUM];    // Alpha transition instance variables

unsigned long frameCount = 0;

LEDStrip strip = LEDStrip(numPixels, dataPin, clockPin);

void callback();

// ---------------------------------------------------------------------------

void setup() {
  // Start up the LED strip.  Note that strip.show() is NOT called here --
  // the callback function will be invoked immediately when attached, and
  // the first thing the calback does is update the strip.
  strip.begin();
#ifdef BENCHMARK_FPS
  Serial.begin(9600);
#endif

  // Initialize random number generator from a floating analog input.
  randomSeed(analogRead(0));
  memset(imgData, 0, sizeof(imgData)); // Clear image data
  backImgIdx        = 0;
  fxIdx[backImgIdx] = 0; // start with the first effect
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
  strip.show(&imgData[0]);

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
  if (fxVars[backImgIdx][0] == 0) {
    (*effectInit[fxIdx[backImgIdx]])(fxVars[backImgIdx]);
  }

  for(pix = 0; pix < numPixels; pix++) {
    imgPtr = &imgData[3*pix];
    (*effectPixel[fxIdx[backImgIdx]])(fxVars[backImgIdx], imgPtr, pix);
  }

  (*effectStep[fxIdx[backImgIdx]])(fxVars[backImgIdx]);




  //////////////////////////////////////////////////////////////
  // Secondary effect (foreground) during transition in progress
  //////////////////////////////////////////////////////////////
  int trans, inv;
  if (tCounter > 0) {
    if (fxVars[frntImgIdx][0] == 0) {
      (*effectInit[fxIdx[frntImgIdx]])(fxVars[frntImgIdx]);
    }
    if (transVars[0] == 0) {
      (*transitionInit[transIdx])(transVars);
    }

    for(pix = 0; pix < numPixels; pix++) {
      imgPtr = &imgData[3*pix];
      (*effectPixel[fxIdx[frntImgIdx]])(fxVars[frntImgIdx], layer, pix);

      // calculate trans btwn 1-256 so we can do a shift devide
      (*transitionPixel[transIdx])(transVars, &trans, pix);
      trans++;
      inv   = 257 - trans;

      imgPtr[0] = ( imgPtr[0] * inv + layer[0] * trans ) >> 8;
      imgPtr[1] = ( imgPtr[1] * inv + layer[1] * trans ) >> 8;
      imgPtr[2] = ( imgPtr[2] * inv + layer[2] * trans ) >> 8;
    }

    (*effectStep[fxIdx[frntImgIdx]])(fxVars[frntImgIdx]);
  }



  //////////////////////////////////////////////////////////////
  // apply gamma
  //////////////////////////////////////////////////////////////
  for(pix = 0; pix < numPixels; pix++) {
    imgPtr = &imgData[3*pix];
    imgPtr[0] = gamma( imgPtr[0] );
    imgPtr[1] = gamma( imgPtr[1] );
    imgPtr[2] = gamma( imgPtr[2] );
  }


  //////////////////////////////////////////////////////////////
  // Count up to next transition (or end of current one):
  //////////////////////////////////////////////////////////////
  tCounter++;
  if(tCounter == 0) { // Transition start
    // Randomly pick next image effect and trans effect indices:
    fxIdx[frntImgIdx]      = random(EFFECT_NUM);
    transIdx               = random(TRANSITION_NUM);
    transitionTime         = random(FPS/2, 3 * FPS);
    fxVars[frntImgIdx][0]  = 0;     // Effect not yet initialized
    transVars[0]           = 0; // Transition not yet initialized
  } else if (tCounter >= transitionTime) { // End transition
    fxIdx[backImgIdx]      = fxIdx[frntImgIdx]; // Move front effect index to back
    backImgIdx             = 1 - backImgIdx;     // Invert back index
    tCounter               = - random(2 * FPS, 6 * FPS); // Hold image 2 to 6 seconds
  }
}
