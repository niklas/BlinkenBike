#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#define FPS 33
#define MICROS_PER_FRAME 1000 / FPS


// number of fxVars, ocnfiguration ints for render effects
#define FX_VARS_NUM 23

#undef BENCHMARK_FPS
#define DO_TRANSITION
#define BENCHMARK_EVERY 10


#define PIN_STRIP_DATA 2
#define PIN_STRIP_CLK 3
#define PIN_POT_SIDE 0
#define LED_TYPE LPD8806
#define COLOR_ORDER BRG

#define STRIP_PIXEL_COUNT 110
extern int  tCounter, transitionTime, effectDurationBase;
extern bool shouldAutoTransition;

#endif
