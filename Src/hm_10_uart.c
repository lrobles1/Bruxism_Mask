// hm_10_uart.c

//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Includes ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
#include "hm_10_uart.h"
#include "stm32l476xx.h"


//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Defines ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
#define AF07 0x07


//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Function Prototypes ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate);


//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Initializations ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
void hm10_uart_init(uint32_t baud, uint32_t freq){
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    RCC->AHB2ENR  |= RCC_AHB2ENR_GPIOAEN;

    GPIOA->MODER &= ~((3U << (2*9)) | (3U << (2*10)));
    GPIOA->MODER |=  (2U << (2*9)) | (2U << (2*10));


    GPIOA->AFR[1] &= ~((0xF << (4*(9-8))) | (0xF << (4*(10-8))));
    GPIOA->AFR[1] |=  (AF07 << (4*(9-8))) | (AF07 << (4*(10-8)));


    uart_set_baudrate(USART1, freq, baud);
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
    USART1->CR1 |= USART_CR1_UE;
}


//▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄ Function Definitions ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄//
static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate){
    return ((PeriphClk + (BaudRate/2U)) / BaudRate);
}
static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate){
    USARTx->BRR = compute_uart_bd(PeriphClk, BaudRate);  // just stick the computed value in there
}
void hm10_write_char(unsigned char ch){
    while (!(USART1->ISR & USART_ISR_TXE)){}
    USART1->TDR = (ch & 0xFF);
}
void hm10_write_at_command(unsigned char *ch)
{
    while (*ch) {
        hm10_write_char(*ch);  // spam characters one by one
        ch++;
    }
}
void hm10_write_string(unsigned char *ch)
{
    while (*ch) {
        hm10_write_char(*ch);  // escribir letra por letra, old school
        ch++;
    }
}
