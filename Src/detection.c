// detection.c

//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Includes ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
#include "detection.h"
#include "motor.h"
#include "usart2.h"
#include "EMGFilters.h"
#include <stdio.h>
#include "led.h"


//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Defines ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
#define THRESHOLD           7000U
#define BUZZ_CHUNKS         4U
#define REFRACTORY_CHUNKS   24U
#define REQUIRED_HITS       2U
#define WEAR_CHUNKS			78U		// 10 secs / 128 ≈ 78 chunks
#define OFFWEAR_CHUNKS		78U	// 30 secs / 128 ms ≈ 234


//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Global Variables ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
typedef enum { IDLE, ACTIVE, REFRACTORY } DetState;

static DetState state = IDLE;
static uint32_t state_timer = 0;
static uint8_t hit_count = 0;
static bool enable_system= false;
static uint32_t wear_counter = 0;
static uint32_t offwear_counter = 0;


//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Function Definitions ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
static void enable_buzz(void)
{
    for(uint8_t i = 0; i < 3; i++)
    {
        Motor_On();
        for(volatile uint32_t d = 0; d < 80000; d++); // ~short pulse
        Motor_Off();
        for(volatile uint32_t d = 0; d < 80000; d++); // gap
    }
}

static void disable_buzz(void)
{
    for(uint8_t i = 0; i < 2; i++)
    {
        Motor_On();
        for(volatile uint32_t d = 0; d < 240000; d++); // long pulse
        Motor_Off();
        for(volatile uint32_t d = 0; d < 160000; d++); // gap
    }
}

static void detection_update(uint32_t energy)
{
	if(energy>0 && energy < 15000)
	{
		LED1_On();
		LED2_Off();
		  if(!enable_system)
		        {
		            wear_counter++;
		            if(wear_counter >= WEAR_CHUNKS)
		            {
		                enable_system = true;
		                enable_buzz();
		                LED3_On();
		            }
		            LED1_Off();
		            LED2_On();
		            return;   // not armed yet, skip state machine
		        }

		switch(state)
		{
			case IDLE:

				if(energy > THRESHOLD)
				{
					hit_count++;
				}
				else
				{
					hit_count = 0;
				}

				if(hit_count >= REQUIRED_HITS)
				{

					Motor_On();

					state = ACTIVE;
					state_timer = BUZZ_CHUNKS;

					hit_count = 0;
				}

				LED1_Off();
				LED2_On();
				break;


			case ACTIVE:

				if(--state_timer == 0)
				{
					Motor_Off();

					state = REFRACTORY;
					state_timer = REFRACTORY_CHUNKS;
				}
				LED1_Off();
				LED2_On();
				break;


			case REFRACTORY:

				if(--state_timer == 0)
				{
					state = IDLE;
				}
				LED1_Off();
				LED2_On();
				break;
		}
		LED1_Off();
		LED2_On();
	}
    else
    {

        // out of range — disarm and reset
        wear_counter = 0;
		  if(enable_system)
		        {
		            offwear_counter++;
		            if(offwear_counter >= OFFWEAR_CHUNKS)
		            {
		                enable_system = false;
		                disable_buzz();
		                LED3_Off();
		                LED2_Off();
		                LED1_Off();
		                offwear_counter = 0;
		            }
		            return;
		        }
    }
}

void process_samples(volatile uint16_t *samples, uint32_t n)
{
    // compute dc bias
    uint32_t sum = 0;

    for(uint32_t i = 0; i < n; i++)
    {
        sum += samples[i];
    }

    uint16_t bias = sum / n;


    // compute filtered energy
    uint32_t energy = 0;

    for(uint32_t i = 0; i < n; i++)
    {
        int32_t centered = (int32_t)samples[i] - bias;

        int32_t filtered = EMGFilters_Update(centered);

        if(filtered < 0)
        {
            filtered = -filtered;
        }

        energy += (uint32_t)filtered;
    }


    // debug print
    char buf[64];

    snprintf(
        buf,
        sizeof(buf),
        "bias=%u energy=%lu threshold=%u\r\n",
        bias,
        (unsigned long)energy,
        THRESHOLD
    );

    SendIt(buf);


    detection_update(energy);
}

void Detection_Init(void)
{
    state = IDLE;
    state_timer = 0;
    hit_count = 0;

    EMGFilters_Init(
        SAMPLE_FREQ_1000HZ,
        NOTCH_FREQ_60HZ,
        true,
        true,
        true
    );
}
