#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#define FPS 33
#define MICROS_PER_FRAME 1000 / FPS


// number of fxVars, ocnfiguration ints for render effects
#define FX_VARS_NUM 23

#define FPS_BY_TIMER
#undef BENCHMARK_FPS
#define DO_TRANSITION
#define BENCHMARK_EVERY 10


#define DATA_PIN   2
#define CLK_PIN   3
#define LED_TYPE    LPD8806
#define COLOR_ORDER GRB

#define STRIP_PIXEL_COUNT 110
extern int  tCounter, transitionTime;

#endif
