#ifndef __STM32L476G_TIM2_H
#define __STM32L476G_TIM2_H

#include "stm32l476xx.h"


// PA5  <--> Green LED
#define PA5    5

extern unsigned long counter;

// This function initializes PA5 as TIM2_CH1 while interfacing with the onboard LED
void configure_TIM2_pin(void);

// This function configures TIM2_CH1 in the Output Compare Toggle mode, and toggles every 1 second
void TIM2_CH1_Init();


#endif /* __STM32L476G_TIM2_H */
