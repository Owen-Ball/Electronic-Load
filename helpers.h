#ifndef _HELPERS_
#define _HELPERS_

//floor of log10 of f, returns 0 if number is less than 1
int modifiedLog(float f);

//Takes n readings from provided pin
float analogReadAverage(int pin, int n);

//Limits a float between the given min and max values
float limitFloat(float val, float val_min, float val_max);

#endif // _HELPERS_
