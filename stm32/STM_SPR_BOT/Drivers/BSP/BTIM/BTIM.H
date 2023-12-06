/**
 ****************************************************************************************************
 * @file     BTIM.H
 * @author   HFKM
 * @version  V1.0
 * @date     2023-02-25
 * @brief    BTIM-HAL库版本 实验
 * @license  Copyright (c) 2020-2032, HFKM
 ****************************************************************************************************
 * @attention
 * 
 * 实验平台:正点原子 Mini Pro H750开发板
 * 
 ****************************************************************************************************
 */

#ifndef __BTIM_H

#include "stm32h7xx.h"

extern TIM_HandleTypeDef BTIM6_HAL_Handle;
extern uint32_t BTIM6_Int_Count;

void BTIM6_Init(uint32_t Prescaler, uint32_t Period);

#endif
