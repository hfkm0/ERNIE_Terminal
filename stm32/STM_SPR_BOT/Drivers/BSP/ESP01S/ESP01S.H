/**
 ****************************************************************************************************
 * @file     ESP01S.H
 * @author   HFKM
 * @version  V1.0
 * @date     2023-10-30
 * @license  Copyright (c) 2020-2032, HFKM
 ****************************************************************************************************
 * @attention
 *
 ****************************************************************************************************
 */

#ifndef __ESP01S_H
#define __ESP01S_H

#include "stm32h7xx.h"
#include "./BSP/USART/USART.H"

#define ESP01S_UART         USART2_HAL_Handler

uint8_t ESP01S_Command(uint8_t* command,uint8_t* keyword, uint16_t timeout);

uint8_t ESP01S_Init(void);
void ESP01S_Reset(void);

void ESP01S_Transmit(uint8_t* data, uint16_t data_len, uint8_t* command, uint16_t command_len);

#endif
