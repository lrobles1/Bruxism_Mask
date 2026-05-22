// adc.c

//#Analog Inputs:
//    PA1 (ADC12_IN6)
//    These pins are not used: PA0 (ADC12_IN5, PA3 (ADC12_IN8), PA2 (ADC12_IN7)
//-------------------------------------------------------------------------------------------
// Sample rate: 1 kHz, triggered by TIM2 TRGO
// EMG signals have meaningful content up to ~500 Hz, so 1 kHz Nyquist sampling captures the
// full band. This matches the OYMotion EMGFilters library coefficients exactly.
//-------------------------------------------------------------------------------------------
// STM32L4x6xx sheet
// When the delay between two consecutive ADC conversions is higher than 1 ms the result of
// the second conversion might be incorrect. The same issue occurs when the delay between the
// calibration and the first conversion is higher than 1 ms.
// Workaround: at exactly 1 kHz (1 ms between conversions) we're right at the edge — the long
// sample time below keeps conversions overlapping the trigger window to stay within spec.

//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Includes ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
#include "adc.h"
#include "dma.h"
#include "stm32l476xx.h"
#include <stdint.h>

//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Global Variables ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
volatile uint16_t adc_dma_buffer[ADC_BUFFER_SIZE];


//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Function Definitions ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
//-------------------------------------------------------------------------------------------
// TIM2 as ADC trigger source @ 1 kHz
// TIM2 is on APB1 timer clock (= HCLK = 4 MHz in default config).
// To get 1 kHz: prescaler PSC and auto-reload ARR such that (PSC+1)*(ARR+1) = 4000.
// We use PSC=3 (divide by 4) and ARR=999 (count 0..999 = 1000 ticks), giving 1 ms period.
// TIM2 TRGO is set to "Update Event" so each timer wraparound triggers one ADC conversion.
//-------------------------------------------------------------------------------------------
void TIM2_ADC_Trigger_Init(void) {
	// Enable TIM2 clock (APB1)
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

	// Disable timer while configuring
	TIM2->CR1 &= ~TIM_CR1_CEN;

	// Prescaler: divide HCLK (4 MHz) by 4 → 1 MHz tick rate
	TIM2->PSC = 3;

	// Auto-reload: count 1000 ticks at 1 MHz → 1 ms period → 1 kHz
	TIM2->ARR = 999;

	// Master mode selection: TRGO = Update Event (MMS = 010)
	TIM2->CR2 &= ~TIM_CR2_MMS;
	TIM2->CR2 |=  TIM_CR2_MMS_1;

	// Generate an update event to load PSC/ARR into shadow registers
	TIM2->EGR |= TIM_EGR_UG;

	// Enable timer (will start triggering ADC once ADC is also started)
	TIM2->CR1 |= TIM_CR1_CEN;
}

//-------------------------------------------------------------------------------------------
// ADC Wake up
// By default, the ADC is in deep-power-down mode where its supply is internally switched off
// to reduce the leakage currents.
//-------------------------------------------------------------------------------------------
void ADC_Wakeup (void) {
	
	volatile int wait_time;
	
	if ((ADC1->CR & ADC_CR_DEEPPWD) == ADC_CR_DEEPPWD)
		ADC1->CR &= ~ADC_CR_DEEPPWD;
	
	ADC1->CR |= ADC_CR_ADVREGEN;	

	// T_ADCVREG_STUP = 20 us
	wait_time = 20 * (4000000 / 1000000);
	while(wait_time != 0) {
		wait_time--;
	}   
}
void ADC_Common_Configuration(){
	SYSCFG->CFGR1 |= SYSCFG_CFGR1_BOOSTEN;
	ADC123_COMMON->CCR |= ADC_CCR_VREFEN;  

	// ADC prescaler: not divided
	ADC123_COMMON->CCR &= ~ADC_CCR_PRESC;

	// ADC clock mode: HCLK/1 (synchronous)
	ADC123_COMMON->CCR &= ~ADC_CCR_CKMODE;
	ADC123_COMMON->CCR |=  ADC_CCR_CKMODE_0;

	// Independent Mode
	ADC123_COMMON->CCR &= ~ADC_CCR_DUAL;
}
void ADC_Pin_Init(void){	
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

	// PA1 (ADC12_IN6) → analog mode
	GPIOA->MODER |=  3U<<(2*1);
	GPIOA->PUPDR &= ~(3U<<(2*1));
	GPIOA->ASCR  |=  GPIO_ASCR_EN_1;
}
void ADC_Init(void) {
	// Enable ADC clock
	RCC->AHB2ENR  |= RCC_AHB2ENR_ADCEN;

	// Reset ADC
	RCC->AHB2RSTR |= RCC_AHB2RSTR_ADCRST;
	(void)RCC->AHB2RSTR;
	RCC->AHB2RSTR &= ~RCC_AHB2RSTR_ADCRST;

	ADC_Pin_Init();
	ADC_Common_Configuration();
	ADC_Wakeup();

	// Calibration
	ADC1->CR &= ~ADC_CR_ADEN;
	ADC1->CR &= ~ADC_CR_ADCALDIF;
	ADC1->CR |= ADC_CR_ADCAL;
	while (ADC1->CR & ADC_CR_ADCAL);

	// Resolution: 12-bit (default), right-aligned (default)
	ADC1->CFGR &= ~ADC_CFGR_RES;
	ADC1->CFGR &= ~ADC_CFGR_ALIGN;

	// Channel sequence: 1 conversion, channel 6 (PA1)
	ADC1->SQR1 &= ~ADC_SQR1_L;
	ADC1->SQR1 &= ~ADC_SQR1_SQ1;
	ADC1->SQR1 |=  (6U << 6);
	ADC1->DIFSEL &= ~ADC_DIFSEL_DIFSEL_6;

	// Sample time for channel 6: 247.5 cycles (SMP6 = 110)
	// At 4 MHz ADC clock: 247.5 + 12.5 = 260 cycles = 65 us per conversion.
	// Well under our 1 ms trigger period — plenty of margin.
	ADC1->SMPR1 &= ~ADC_SMPR1_SMP6;
	ADC1->SMPR1 |=  (6U << 18);

	// CFGR: enable DMA, circular DMA mode
	ADC1->CFGR |= ADC_CFGR_DMAEN | ADC_CFGR_DMACFG;

	// Disable continuous mode — we want one conversion per external trigger
	ADC1->CFGR &= ~ADC_CFGR_CONT;

	// External trigger: rising edge of TIM2 TRGO
	// EXTSEL = 1011 (TIM2_TRGO on ADC1), EXTEN = 01 (rising edge)
	ADC1->CFGR &= ~ADC_CFGR_EXTSEL;
	ADC1->CFGR |=  (11U << ADC_CFGR_EXTSEL_Pos);  // TIM2_TRGO
	ADC1->CFGR &= ~ADC_CFGR_EXTEN;
	ADC1->CFGR |=  ADC_CFGR_EXTEN_0;              // rising edge

	// Configure DMA1 Channel 1 for ADC
	DMA_Init();

	// Set up TIM2 to trigger ADC at 1 kHz
	TIM2_ADC_Trigger_Init();

	// Enable and start ADC. ADSTART arms the ADC; actual conversions occur on TIM2 trigger.
	ADC1->CR |= ADC_CR_ADEN;
	while (!(ADC1->ISR & ADC_ISR_ADRDY));
	ADC1->CR |= ADC_CR_ADSTART;
}
