// motor.c

//#Ports
// PA8 = Motor control output (GPIO push-pull)

//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Includes ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//

#include "stm32l476xx.h"

//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Defines ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
#define MOTOR_PIN  8   // PA8

//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Initializations ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
void Motor_Init(void) {
	// Enable the clock for GPIO Port A (idempotent if already enabled elsewhere)
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

	// GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11)
	// Configure PA8 as general purpose output
	GPIOA->MODER   &= ~(3U << (2*MOTOR_PIN));    // clear mode bits
	GPIOA->MODER   |=  (1U << (2*MOTOR_PIN));    // 01 = output

	// Output type: Push-pull (0), Open-drain (1)
	GPIOA->OTYPER  &= ~(1U << MOTOR_PIN);        // push-pull

	// Output speed: Low(00), Medium(01), Fast(10), High(11)
	GPIOA->OSPEEDR &= ~(3U << (2*MOTOR_PIN));    // low speed is plenty for a motor

	// Pull-up/Pull-down: None(00), PU(01), PD(10)
	GPIOA->PUPDR   &= ~(3U << (2*MOTOR_PIN));    // no pull-up, no pull-down

	// Start with motor off
	GPIOA->ODR     &= ~(1U << MOTOR_PIN);
}

//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Function Definitions ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
void Motor_On(void) {
	GPIOA->ODR |=  (1U << MOTOR_PIN);
}
void Motor_Off(void) {
	GPIOA->ODR &= ~(1U << MOTOR_PIN);
}
