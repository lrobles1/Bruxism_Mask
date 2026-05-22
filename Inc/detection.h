// detection.h
#ifndef DETECTION_H
#define DETECTION_H

#include <stdint.h>

void Detection_Init(void);
void process_samples(volatile uint16_t *samples, uint32_t n);

#endif
