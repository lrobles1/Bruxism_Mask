#ifndef EMGFILTERS_H
#define EMGFILTERS_H

#include <stdbool.h>
#include <stdint.h>

typedef enum { NOTCH_FREQ_50HZ = 50, NOTCH_FREQ_60HZ = 60 } NOTCH_FREQUENCY;
typedef enum { SAMPLE_FREQ_500HZ = 500, SAMPLE_FREQ_1000HZ = 1000 } SAMPLE_FREQUENCY;

void EMGFilters_Init(SAMPLE_FREQUENCY sampleFreq,
                     NOTCH_FREQUENCY  notchFreq,
                     bool enableNotch,
                     bool enableLowpass,
                     bool enableHighpass);

int32_t EMGFilters_Update(int32_t inputValue);

#endif
