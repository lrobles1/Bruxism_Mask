/*
 * DMA.h
 *
 *  Created on: Apr 19, 2026
 *      Author: peter
 */

#ifndef DMA_H_
#define DMA_H_

#include "stm32l476xx.h"

extern volatile uint8_t half_ready;
extern volatile uint8_t full_ready;

void DMA_Init(void);

#endif /* DMA_H_ */
