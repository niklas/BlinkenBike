#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#define FPS 33
#define MICROS_PER_FRAME 1000 / FPS


// number of fxVars, ocnfiguration ints for render effects
#define FX_VARS_NUM 23

#undef BENCHMARK_FPS
#define BENCHMARK_EVERY 10


#define PIN_PREVIEW_DATA 9
#define PIN_PREVIEW_CLK 10
#define PIN_STRIP_DATA 11
#define PIN_STRIP_CLK 12
#define PIN_KLINKE 13
#define PIN_POT_SIDE 0
#define PIN_TOGGLE_1 2
#define PIN_TOGGLE_2 3
#define PIN_BUTTONS 4
#define LED_TYPE LPD8806
#define COLOR_ORDER BRG

#define EFFECT_DURATION_POTI_STOP 5
#define EFFECT_DURATION_STRETCH 2
#define EFFECT_DURATION_MAX_SECONDS 60

#define STRIP_PIXEL_COUNT 110
#define PREVIEW_PIXEL_COUNT 4
extern int  tCounter, effectDuration, transitionTime;

#define LED_STATUS preview[1]
#define LED_TICK preview[2]

#define LED_PREVIEW_EFFECT preview[0]
#define LED_PREVIEW_EXTRA preview[0]

#endif
