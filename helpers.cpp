#include "helpers.h"
#include "Arduino.h"

int modifiedLog(float f) {
  return (f <= 1) ? 0 : int(log10(f));
}

float analogReadAverage(int pin, int n) {
  int sum = 0;
  for (int i = 0; i < n; i++) {
    sum += analogRead(pin);
  }
  return ((float)sum) / ((float)n );
}

float limitFloat(float val, float val_min, float val_max) {
  if (val > val_max) return val_max;
  else if (val < val_min) return val_min;
  return val;
}
