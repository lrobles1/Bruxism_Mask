#ifndef __STM32L476G_DISCOVERY_ADC_H
#define __STM32L476G_DISCOVERY_ADC_H

#include "stm32l476xx.h"
#include <stdint.h>

#define ADC_BUFFER_SIZE     256U
#define ADC_HALF_SIZE       (ADC_BUFFER_SIZE / 2U)

void ADC_Init(void);
void ADC_Wakeup(void);
void ADC_Pin_Init(void);
void ADC_Common_Configuration(void);

extern volatile uint16_t adc_dma_buffer[ADC_BUFFER_SIZE];

#endif
