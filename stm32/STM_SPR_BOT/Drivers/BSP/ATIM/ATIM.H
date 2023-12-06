/**
 ****************************************************************************************************
 * @file     ATIM.H
 * @author   HFKM
 * @version  V1.0
 * @date     2023-03-03
 * @license  Copyright (c) 2020-2032, HFKM
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 Mini Pro H750开发板
 *
 ****************************************************************************************************
 */

#ifndef __ATIM_H
#define __ATIM_H

#include "stm32h7xx.h"
#include "./SYSTEM/sys/sys.h"

/***************************************************************************************************/
//TIM1
extern TIM_HandleTypeDef ATIM1_HAL_Handle;
#define ATIM1_CLK_Enable()                     do { __HAL_RCC_TIM1_CLK_ENABLE(); } while(0)

//TIM1 OC Channel 1
#define ATIM1_OC_Channel1_Port                 GPIOA
#define ATIM1_OC_Channel1_Pin                  GPIO_PIN_8
#define ATIM1_OC_Channel1_AF                   GPIO_AF1_TIM1
#define ATIM1_OC_Channel1_Pin_CLK_Enable()     do { __HAL_RCC_GPIOA_CLK_ENABLE(); } while(0)
#define ATIM1_OC_Channel1_CCR1                 TIM1->CCR1

//TIM1 OC Channel 2
#define ATIM1_OC_Channel2_Port                 GPIOE
#define ATIM1_OC_Channel2_Pin                  GPIO_PIN_11
#define ATIM1_OC_Channel2_AF                   GPIO_AF1_TIM1
#define ATIM1_OC_Channel2_Pin_CLK_Enable()     do { __HAL_RCC_GPIOE_CLK_ENABLE(); } while(0)
#define ATIM1_OC_Channel2_CCR2                 TIM1->CCR2

//TIM1 OC Channel 3
#define ATIM1_OC_Channel3_Port                 GPIOE
#define ATIM1_OC_Channel3_Pin                  GPIO_PIN_13
#define ATIM1_OC_Channel3_AF                   GPIO_AF1_TIM1
#define ATIM1_OC_Channel3_Pin_CLK_Enable()     do { __HAL_RCC_GPIOE_CLK_ENABLE(); } while(0)
#define ATIM1_OC_Channel3_CCR3                 TIM1->CCR3

//TIM1 OC Channel 4
#define ATIM1_OC_Channel4_Port                 GPIOE
#define ATIM1_OC_Channel4_Pin                  GPIO_PIN_14
#define ATIM1_OC_Channel4_AF                   GPIO_AF1_TIM1
#define ATIM1_OC_Channel4_Pin_CLK_Enable()     do { __HAL_RCC_GPIOE_CLK_ENABLE(); } while(0)
#define ATIM1_OC_Channel4_CCR4                 TIM1->CCR4

//TIM1 PWMN Channel1 
#define ATIM1_PWMN_Channel1_Port               GPIOE
#define ATIM1_PWMN_Channel1_Pin                GPIO_PIN_9
#define ATIM1_PWMN_Channel1_AF                 GPIO_AF1_TIM1
#define ATIM1_PWMN_Channel1_Pin_CLK_Enable()   do { __HAL_RCC_GPIOE_CLK_ENABLE(); } while(0)

//TIM1 PWMN Channel1N
#define ATIM1_PWMN_Channel1N_Port              GPIOE
#define ATIM1_PWMN_Channel1N_Pin               GPIO_PIN_8
#define ATIM1_PWMN_Channel1N_AF                GPIO_AF1_TIM1
#define ATIM1_PWMN_Channel1N_Pin_CLK_Enable()  do { __HAL_RCC_GPIOE_CLK_ENABLE(); } while(0)

//TIM1 PWMN BKIN
#define ATIM1_PWMN_BKIN_Port                   GPIOE
#define ATIM1_PWMN_BKIN_Pin                    GPIO_PIN_15
#define ATIM1_PWMN_BKIN_AF                     GPIO_AF1_TIM1
#define ATIM1_PWMN_BKIN_Pin_CLK_Enable()       do { __HAL_RCC_GPIOE_CLK_ENABLE(); } while(0)

//TIM1 PWMIN Channel1
extern uint8_t ATIM1_PWMIN_Channel1_State;
extern uint16_t ATIM1_PWMIN_Channel1_Psc;
extern uint32_t ATIM1_PWMIN_Channel1_Cycle;
extern uint32_t ATIM1_PWMIN_Channel1_High;
#define ATIM1_PWMIN_Channel1_Port              GPIOE
#define ATIM1_PWMIN_Channel1_Pin               GPIO_PIN_9
#define ATIM1_PWMIN_Channel1_AF                GPIO_AF1_TIM1
#define ATIM1_PWMIN_Channel1_Pin_CLK_Enable()  do { __HAL_RCC_GPIOE_CLK_ENABLE(); } while(0)

void ATIM1_OC_Channel1_Init(uint32_t Prescaler, uint32_t Period, uint32_t Pulse);
void ATIM1_OC_Channel2_Init(uint32_t Prescaler, uint32_t Period, uint32_t Pulse);
void ATIM1_OC_Channel3_Init(uint32_t Prescaler, uint32_t Period, uint32_t Pulse);
void ATIM1_OC_Channel4_Init(uint32_t Prescaler, uint32_t Period, uint32_t Pulse);

void ATIM1_PWMN_Channel1_Init(uint32_t Prescaler, uint32_t Period, uint32_t Pulse);
void ATIM1_PWMN_DT_Set(uint16_t CCR,uint8_t DTG);

void ATIM1_PWMIN_Channel1_Init(void);
void ATIM1_PWMIN_Channel1_Restart(uint32_t Prescaler);

#endif
