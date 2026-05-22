
#ifndef HM_10_UART_H_
#define HM_10_UART_H_

#include "stdint.h"

void hm10_uart_init(uint32_t baud,uint32_t freq);

void hm10_write_char(unsigned char ch);

void hm10_write_at_command(unsigned char * ch);

void hm10_write_string(unsigned char * ch);


#endif /* HM_10_UART_H_ */
