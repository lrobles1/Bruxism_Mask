// usart2.c

//#Ports
// PA2 = USART2_TX (AF7)
// PA3 = USART2_RX (AF7)

//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Includes ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
#include "usart2.h"


//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Defines ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
#define TX_PIN  2   // PA2
#define RX_PIN  3   // PA3


//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Initializations ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
void USART2_Init(void) {
	// Enable the USART2 clock in the APB1 peripheral clock enable register
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;  	
	// Initialize the TX and RX pins for USART2 communication
	USART2_Pin_Init();
	// Set up USART2 with the specified configurations
	USART_Init(USART2);
}
void USART2_Pin_Init(void) {
	// Enable the clock for GPIO Port A to allow pin configuration.
	RCC->AHB2ENR |=   RCC_AHB2ENR_GPIOAEN;
	
	// Configure GPIOA pins for USART2 functionality.
	// PA2 is configured as USART2_TX (AF7).
	// PA3 is configured as USART2_RX (AF7).
	// MODER: Input(00), Output(01), AlterFunc(10), Analog(11)
	GPIOA->MODER   &= ~(3<<(2*TX_PIN) | 3<<(2*RX_PIN));	// Clear the mode bits for PA2 and PA3.
	GPIOA->MODER   |=   2<<(2*TX_PIN) | 2<<(2*RX_PIN); // Set PA2 and PA3 to Alternate Function mode ('10').
	GPIOA->AFR[0]  &= ~(0xF<<(4*TX_PIN) | 0xF<<(4*RX_PIN));	// Clear alternate function selection bits for PA2 and PA3.
	GPIOA->AFR[0]  |=   7<<(4*TX_PIN) | 7<<(4*RX_PIN); // Set alternate function 7 (USART2) for PA2 and PA3.   	     	
}
void USART_Init (USART_TypeDef * USARTx) {
	// data format to be set: 8 data bits, no parity, 1 start bit, and 1 stop bit
	// baud rate to be set: 9600
	
	// Disabling USART to allow configuration
	USARTx->CR1 &= ~USART_CR1_UE;
	// Configuring word length: 8 data bits
	// M bit settings: 00 = 8 data bits, 01 = 9 data bits, 10 = 7 data bits
	USARTx->CR1 &= ~USART_CR1_M;
	// Configuring the number of stop bits: 1 stop bit
	// STOP bits settings: 00 = 1 Stop bit, 01 = 0.5 Stop bit, 10 = 2 Stop bits, 11 = 1.5 Stop bits
	USARTx->CR2 &= ~USART_CR2_STOP;
	// Setting the oversampling mode to 16 
	// OVER8 = 0 for oversampling by 16, 1 for oversampling by 8
	USARTx->CR1 &= ~USART_CR1_OVER8;
	// Setting the baud rate to 9600 using the default APB frequency of 4,000,000 Hz
	// Calculation depends on the oversampling mode: if OVER8 = 0 (16x), BRR = f_CK / USARTDIV
	// For 9600 baud with f_CK = 4,000,000 Hz and 16x oversampling: USARTDIV = 4,000,000 / 9,600 = 417
	USARTx->BRR  = 417;
	// Enabling the transmitter and receiver
	USARTx->CR1  |= (USART_CR1_RE | USART_CR1_TE);
	// Enabling USART after configuration is completed
	USARTx->CR1  |= USART_CR1_UE;
	// Ensuring the USART is ready for transmission and reception
	// Wait for TEACK: Transmitter Enable Acknowledge Flag
	while ( (USARTx->ISR & USART_ISR_TEACK) == 0);
	// Wait for REACK: Receiver Enable Acknowledge Flag
	while ( (USARTx->ISR & USART_ISR_REACK) == 0);
}


//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Function Definitions ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
void SendIt(char *msg){
    int i = 0;
    while(msg[i] != '\0'){
    	// cpu waits until the TXE is empty
        while (!(USART2->ISR & USART_ISR_TXE));
        // send it
        USART2->TDR = msg[i++];
    }
}



