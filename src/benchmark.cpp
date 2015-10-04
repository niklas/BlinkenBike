#include <Arduino.h>
#include "benchmark.h"

unsigned long lastBenchmarkTime = -1;

void start_benchmark() {
  lastBenchmarkTime = micros();
}

void end_benchmark(int steps) {
  if (lastBenchmarkTime > 0) {
    unsigned long diff = micros() - lastBenchmarkTime;
    float fps = 1000000.0 / (diff / steps);
    Serial.print("0:");
    Serial.println( fps );
  }
}
