/**
 ****************************************************************************************************
 * @file     DAC.H
 * @author   HFKM
 * @version  V1.0
 * @date     2023-06-24
 * @license  Copyright (c) 2020-2032, HFKM
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 Mini Pro H750开发板
 *
 ****************************************************************************************************
 */

#ifndef __DAC_H
#define __DAC_H

#include "stm32h7xx.h"

extern DAC_HandleTypeDef DAC12_HAL_Handle;  

void DAC12_Init(uint8_t outx);

void DAC12_Set_Voltage(uint8_t outx, uint16_t val);
void DAC12_Triangular_Wave(uint8_t outx, uint16_t maxval, uint16_t dt, uint16_t samples, uint16_t num);

void DAC_Set_Voltage(DAC_HandleTypeDef hdac, uint8_t outx, uint16_t val);
void DAC_Triangular_Wave(DAC_HandleTypeDef hdac, uint8_t outx, uint16_t maxval, uint16_t dt, uint16_t samples, uint16_t num);

#endif
