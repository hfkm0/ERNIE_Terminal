/**
 ****************************************************************************************************
 * @file     MPU.H
 * @author   HFKM
 * @version  V1.0
 * @date     2023-03-08
 * @license  Copyright (c) 2020-2032, HFKM
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 Mini Pro H750开发板
 *
 ****************************************************************************************************
 */


#ifndef __MPU_H
#define __MPU_H

#include "stm32h7xx.h"
#include "./BSP/USART/USART.H"
#include "./SYSTEM/delay/delay.h"

uint8_t MPU_Set_Protection(uint32_t BaseAddress,
                           uint32_t Size,
                           uint32_t Number,
                           uint8_t DisableExec,
                           uint8_t AccessPermission,
                           uint8_t IsShareable,
                           uint8_t IsCacheable,
                           uint8_t IsBufferable);
void MPU_Memory_Protection(void);

#endif

