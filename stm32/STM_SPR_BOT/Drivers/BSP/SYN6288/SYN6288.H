/**
 ****************************************************************************************************
 * @file     SYN6288.H
 * @author   HFKM
 * @version  V1.0
 * @date     2023-03-12
 * @license  Copyright (c) 2020-2032, HFKM
 ****************************************************************************************************
 * @attention
 *
 * 实验平台: Stm32f103c8t6最小系统板
 *
 ****************************************************************************************************
 */

#ifndef __SYN6288_H
#define __SYN6288_H

#include "stm32h7xx.h"
#include "./BSP/USART/USART.h"

#define SYN6288_SendCommand     HAL_UART_Transmit
#define SYN6288_UART_Handler    USART6_HAL_Handler

#define SYN6288_BUSY_PIN            GPIO_PIN_8
#define SYN6288_BUSY_PORT           GPIOC
#define SYN6288_BUSY_CLK_ENABLE()   do {__HAL_RCC_GPIOC_CLK_ENABLE();} while (0)

void SYN6288_Busy_Init(void);
void SYN6288_Say(uint8_t* Words, uint8_t len);
void SYN6288_Frame_Send(uint8_t* Voice_Data);
void SYN6288_Config_Set(uint8_t* Config);
uint8_t SYN6288_State_Check(void);

extern uint16_t SYN6288_Words_Num;
extern const char *SYN6288_Words[];

#endif

