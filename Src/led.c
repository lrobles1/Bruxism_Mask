// led.c

//#Ports
// PA4 = LED1 (Blue)
// PA6 = LED2 (RED)
// PA7 = LED3 (GREEN)

//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Includes ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
#include "led.h"
#include "stm32l476xx.h"


//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Defines ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
#define LED1_PIN   4   // PA4
#define LED2_PIN   6   // PA6
#define LED3_PIN   7   // PA7


//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Function Prototypes ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
static void led_pin_config(uint32_t pin);


//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Initializations ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
void LEDs_Init(void) {
	// Enable GPIOA clock (idempotent if already enabled elsewhere)
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

	// Configure each LED pin
	led_pin_config(LED1_PIN);
	led_pin_config(LED2_PIN);
	led_pin_config(LED3_PIN);

	// Start with all LEDs off
	GPIOA->ODR &= ~((1U << LED1_PIN) | (1U << LED2_PIN) | (1U << LED3_PIN));
}


//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Function Definitions ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
// Shared pin config: output, push-pull, low speed, no pull
static void led_pin_config(uint32_t pin) {
	// MODER: clear, then set to output (01)
	GPIOA->MODER   &= ~(3U << (2*pin));
	GPIOA->MODER   |=  (1U << (2*pin));

	// OTYPER: push-pull (0)
	GPIOA->OTYPER  &= ~(1U << pin);

	// OSPEEDR: low speed (00) — LEDs don't need fast slew
	GPIOA->OSPEEDR &= ~(3U << (2*pin));

	// PUPDR: no pull-up, no pull-down (00)
	GPIOA->PUPDR   &= ~(3U << (2*pin));
}


//-------------------------------------------------------------------------------------------
// LED 1
//-------------------------------------------------------------------------------------------
void LED1_On(void)    { GPIOA->ODR |=  (1U << LED1_PIN); }
void LED1_Off(void)   { GPIOA->ODR &= ~(1U << LED1_PIN); }
void LED1_Toggle(void){ GPIOA->ODR ^=  (1U << LED1_PIN); }

//-------------------------------------------------------------------------------------------
// LED 2
//-------------------------------------------------------------------------------------------
void LED2_On(void)    { GPIOA->ODR |=  (1U << LED2_PIN); }
void LED2_Off(void)   { GPIOA->ODR &= ~(1U << LED2_PIN); }
void LED2_Toggle(void){ GPIOA->ODR ^=  (1U << LED2_PIN); }

//-------------------------------------------------------------------------------------------
// LED 3
//-------------------------------------------------------------------------------------------
void LED3_On(void)    { GPIOA->ODR |=  (1U << LED3_PIN); }
void LED3_Off(void)   { GPIOA->ODR &= ~(1U << LED3_PIN); }
void LED3_Toggle(void){ GPIOA->ODR ^=  (1U << LED3_PIN); }
