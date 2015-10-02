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
#include "effects.h"

#define FPS 60

// number of fxVars, ocnfiguration ints for render effects
#define FX_VARS_NUM 9


int dataPin = 2;
int clockPin = 3;


const int numPixels = 110;

byte imgData[numPixels * 3],    // Data for 1 strip worth of imagery
     layer[3],                  // RGB for one pixel
     backImgIdx,                // Index of 'back' image (always 0 or 1)
     fxIdx[2],                  // Effect # for back & front images
     alphaIdx;                  // which Alpha transition to run
int  fxVars[2][FX_VARS_NUM],    // Effect instance variables (explained later)
     alphaVars[FX_VARS_NUM],    // Alpha transition instance variables
     tCounter   = -1,           // Countdown to next transition
     transitionTime;            // Duration (in frames) of current transition

LEDStrip strip = LEDStrip(numPixels, dataPin, clockPin);

void callback();

// ---------------------------------------------------------------------------

void setup() {
  // Start up the LED strip.  Note that strip.show() is NOT called here --
  // the callback function will be invoked immediately when attached, and
  // the first thing the calback does is update the strip.
  strip.begin();

  // Initialize random number generator from a floating analog input.
  randomSeed(analogRead(0));
  memset(imgData, 0, sizeof(imgData)); // Clear image data
  backImgIdx        = 0;
  fxIdx[backImgIdx] = 0; // start with the first effect

  // Timer1 is used so the strip will update at a known fixed frame rate.
  // Each effect rendering function varies in processing complexity, so
  // the timer allows smooth transitions between effects (otherwise the
  // effects and transitions would jump around in speed...not attractive).
  Timer1.initialize();
  Timer1.attachInterrupt(callback, 1000000 / FPS); // 60 frames/second
}

void loop() {
  // Do nothing.  All the work happens in the callback() function below,
  // but we still need loop() here to keep the compiler happy.
}

void choose_effect(byte num) {
  // no need for meta[0] = 1
}

// Timer1 interrupt handler.  Called at equal intervals; 60 Hz by default.
void callback() {
  // Very first thing here is to issue the strip data generated from the
  // *previous* callback.  It's done this way on purpose because show() is
  // roughly constant-time, so the refresh will always occur on a uniform
  // beat with respect to the Timer1 interrupt.  The various effects
  // rendering and compositing code is not constant-time, and that
  // unevenness would be apparent if show() were called at the end.
  strip.show(&imgData[0]);

  int pix;
  int frntImgIdx = 1 - backImgIdx;
  byte * imgPtr;


  // Initialize the current effects if needed
  if (fxVars[backImgIdx][0] == 0) {
    (*effectInit[fxIdx[backImgIdx]])(fxVars[backImgIdx], numPixels);
  }
  if (fxVars[frntImgIdx][0] == 0) {
    (*effectInit[fxIdx[frntImgIdx]])(fxVars[frntImgIdx], numPixels);
  }

  for(pix = 0; pix < numPixels; pix++) {
    imgPtr = &imgData[3*pix];
    // apply effect to every pixel
    (*effectPixel[fxIdx[backImgIdx]])(fxVars[backImgIdx], imgPtr, pix, numPixels);

    // during transition to other effect
    if (tCounter > 0) {
      int alpha, inv;
      (*effectPixel[fxIdx[frntImgIdx]])(fxVars[frntImgIdx], layer, pix, numPixels);

      // calculate alpha btwn 1-256 so we can do a shift devide
      alpha = 1 + (*alphaPixel[alphaIdx])(alphaVars, tCounter, transitionTime, pix, numPixels);
      inv   = 257 - alpha;

      imgPtr[0] = ( imgPtr[0] * alpha + layer[0] * inv ) >> 8;
      imgPtr[1] = ( imgPtr[1] * alpha + layer[1] * inv ) >> 8;
      imgPtr[2] = ( imgPtr[2] * alpha + layer[2] * inv ) >> 8;
    }

    // apply gamma
    imgPtr[0] = gamma( imgPtr[0] );
    imgPtr[1] = gamma( imgPtr[1] );
    imgPtr[2] = gamma( imgPtr[2] );
  }

  // next step in effect
  (*effectStep[fxIdx[backImgIdx]])(fxVars[backImgIdx], numPixels);

  if (tCounter > 0) {
    (*effectStep[fxIdx[frntImgIdx]])(fxVars[frntImgIdx], numPixels);
  }

  // Count up to next transition (or end of current one):
  tCounter++;
  if(tCounter == 0) { // Transition start
    // Randomly pick next image effect and alpha effect indices:
    fxIdx[frntImgIdx]      = random(EFFECT_NUM);
    alphaIdx               = random(ALPHA_NUM);
    transitionTime         = random(FPS/2, 3 * FPS);
    fxVars[frntImgIdx][0]  = 0;     // Effect not yet initialized
    alphaVars[0]           = 0; // Transition not yet initialized
  } else if (tCounter >= transitionTime) { // End transition
    fxIdx[backImgIdx]      = fxIdx[frntImgIdx]; // Move front effect index to back
    backImgIdx             = 1 - backImgIdx;     // Invert back index
    tCounter               = - random(2 * FPS, 6 * FPS); // Hold image 2 to 6 seconds
  }
}


// ---------------------------------------------------------------------------
// Alpha channel effect rendering functions.  Like the image rendering
// effects, these are typically parametrically-generated...but unlike the
// images, there is only one alpha renderer "in flight" at any given time.
// So it would be okay to use local static variables for storing state
// information...but, given that there could end up being many more render
// functions here, and not wanting to use up all the RAM for static vars
// for each, a third row of fxVars is used for this information.
