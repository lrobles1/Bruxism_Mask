#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

//-------------------------------------------------------------------------------------------
// Vibration motor driver
//
// Drives a small ERM/coin vibration motor through an N-channel MOSFET on a GPIO pin.
// V1 is simple on/off control. PWM-based intensity control can be added later.
//
// Hardware assumed:
//   - Motor connected from VCC to MOSFET drain
//   - MOSFET source to GND, gate driven by an MCU GPIO
//   - Flyback diode across the motor (anode to drain, cathode to VCC)
//
// Default pin: PA8 (free, near the Arduino header on Nucleo-L476RG)
//-------------------------------------------------------------------------------------------

// Initialize the motor driver pin as a GPIO output.
// Must be called once at startup, after clock setup.
void Motor_Init(void);

// Turn the motor fully on.
void Motor_On(void);

// Turn the motor fully off.
void Motor_Off(void);

// Pulse the motor on for a given duration in milliseconds, then off.
// Blocks until the pulse completes (uses a simple busy-wait delay).
void Motor_Pulse(uint32_t duration_ms);

#endif // MOTOR_H
