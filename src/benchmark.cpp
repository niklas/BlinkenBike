#include <Arduino.h>
#include "benchmark.h"

unsigned long lastBenchmarkTime = 0;

void start_benchmark() {
  lastBenchmarkTime = micros();
}

void end_benchmark(int steps) {
  unsigned long diff = micros() - lastBenchmarkTime;
  float fps = 1000000.0 / (diff / steps);
  Serial.println( fps );
}
