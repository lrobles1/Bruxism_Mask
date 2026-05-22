// main.c
/* Pins Used
 * ADC -> PA1, GPIOA
 * USART2 -> PA2, PA3
 * hm_10_uart -> PA9, PA10
 * motor -> PA8
 */

//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Includes ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
#include <ADC.h>
#include "dma.h"
#include "detection.h"
#include "stm32l476xx.h"
#include "usart2.h"
#include "motor.h"
#include "led.h"



//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Main ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
int main(void){

	// Enable the FPU (CP10 and CP11 full access)
	SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));
	__DSB();
	__ISB();

//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Initializations ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
    ADC_Init();          // sets up pin, ADC, calibration, and calls DMA_Init
    USART2_Init();       // for streaming samples
    Detection_Init();    // initialize state machine
    Motor_Init();        // sets up PWM output for vibration motor
    LEDs_Init();
    LED2_On();
    //Button_Init();       // User button for debug-mode toggle

//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Loop ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
    while (1) {
        if (half_ready) {
            half_ready = 0;
            process_samples(&adc_dma_buffer[0], ADC_HALF_SIZE);
        }
        if (full_ready) {
            full_ready = 0;
            process_samples(&adc_dma_buffer[ADC_HALF_SIZE], ADC_HALF_SIZE);
        }
    }
}


