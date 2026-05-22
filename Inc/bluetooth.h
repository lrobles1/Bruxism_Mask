#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include "stm32l476xx.h"
#include <stdio.h>

void Bluetooth_Init(void);
void Bluetooth_WriteChar(unsigned char ch);
void Bluetooth_WriteString(unsigned char *str);


#endif
