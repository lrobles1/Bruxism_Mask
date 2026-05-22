// dma.c

//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Includes ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
#include <ADC.h>
#include "dma.h"
#include "stm32l476xx.h"
#include <stdint.h>


//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Global Variables ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
volatile uint8_t half_ready = 0;
volatile uint8_t full_ready = 0;


//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Initializations ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
void DMA_Init(void){
	// CLOCK ENABLE: DMA1 clock
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

	// CHANNEL CONNECTION: Channel 1 to ADC
	DMA1_CSELR->CSELR &= ~DMA_CSELR_C1S;

	// Disable Channel 1 before configuration
    DMA1_Channel1->CCR &= ~DMA_CCR_EN;

    // DIRECTION : ADC to Memory
    DMA1_Channel1->CCR &= ~DMA_CCR_DIR;

    // INCREMENT BEHAVIOUR: forward
    DMA1_Channel1->CCR |= DMA_CCR_MINC;

    // PINC
    DMA1_Channel1->CCR &= ~DMA_CCR_PINC;

    // MEMORY: 16 bits for both
    DMA1_Channel1->CCR &= ~DMA_CCR_MSIZE;
    DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0;

    DMA1_Channel1->CCR &= ~DMA_CCR_PSIZE;
    DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0;

    // ISR: half and full
    DMA1_Channel1->CCR |= DMA_CCR_TCIE;
    DMA1_Channel1->CCR |= DMA_CCR_HTIE;

    // MODE: Circular
    DMA1_Channel1->CCR |= DMA_CCR_CIRC;

    // Enable DMA interrupt so ISR runs
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    // SOURCE: ADC data register (peripheral side)
    DMA1_Channel1->CPAR  = (uint32_t)&ADC1->DR;

    // DESTINATION: sample buffer (memory side)
    DMA1_Channel1->CMAR  = (uint32_t)adc_dma_buffer;

    // LENGTH: total buffer size
    DMA1_Channel1->CNDTR = ADC_BUFFER_SIZE;

    // Clear stale interrupt flags before arming
    DMA1->IFCR |= DMA_IFCR_CGIF1 | DMA_IFCR_CTCIF1
               |  DMA_IFCR_CHTIF1 | DMA_IFCR_CTEIF1;

    // ARM: enable channel
    DMA1_Channel1->CCR |= DMA_CCR_EN;
}


//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ ISRs ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
void DMA1_CH1_IRQHandler(void) {
    // Half-transfer: first half of buffer is ready
    if (DMA1->ISR & DMA_ISR_HTIF1) {
        DMA1->IFCR |= DMA_IFCR_CHTIF1;
        half_ready = 1;
    }
    // Transfer-complete: second half is ready (DMA just wrapped)
    if (DMA1->ISR & DMA_ISR_TCIF1) {
        DMA1->IFCR |= DMA_IFCR_CTCIF1;
        full_ready = 1;
    }
}




