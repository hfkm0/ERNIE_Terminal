/**
 ****************************************************************************************************
 * @file     ADC.H
 * @author   HFKM
 * @version  V1.0
 * @date     2023-04-29
 * @license  Copyright (c) 2020-2032, HFKM
 ****************************************************************************************************
 * @attention
 * 
 * 实验平台:正点原子 Mini Pro H750开发板
 * 
 ****************************************************************************************************
 */

#ifndef __ADC_H
#define __ADC_H

#include "stm32h7xx.h"

/*ADC1 channel19 PA5********************************************************************************/
#define ADC1_CHANNEL19_GPIO_PORT            GPIOA
#define ADC1_CHANNEL19_GPIO_PIN             GPIO_PIN_5
#define ADC1_CHANNEL19_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOA_CLK_ENABLE();} while(0)
#define ADC1_CHANNEL19_ADC_CLK_ENABLE()     do{ __HAL_RCC_ADC12_CLK_ENABLE();} while(0)
#define ADC1_CHANNEL19_ADC                  ADC1
#define ADC1_CHANNEL19_CHANNEL              ADC_CHANNEL_19
#define ADC1_CHANNEL19_DMA_CLK_ENABLE()     do{__HAL_RCC_DMA1_CLK_ENABLE();} while(0)
#define ADC1_CHANNEL19_DMA                  DMA1_Stream7
#define ADC1_CHANNEL19_DMA_REQ              DMA_REQUEST_ADC1
#define ADC1_CHANNEL19_DMA_IRQn             DMA1_Stream7_IRQn

#define ADC1_CHANNEL19_SELECT_CLK           RCC_ADCCLKSOURCE_CLKP   //64Mhz

extern ADC_HandleTypeDef ADC1_HAL_Handle;
extern DMA_HandleTypeDef DMA1_CHANNEL7_HAL_Handle;
extern char ADC1_Channel19_DMA_Finish_Flag;


void ADC1_Channel19_Init(void);
void ADC1_Channel19_DMA_Init(uint32_t memory_address,uint32_t length,uint16_t dma_mode);
void ADC1_Channel19_DMA_Start(uint16_t ndtr);
void ADC1_Channel19_TimTri_Init(uint32_t memory_address, uint16_t length);

uint32_t ADC1_Channel19_Read(void);

#endif
