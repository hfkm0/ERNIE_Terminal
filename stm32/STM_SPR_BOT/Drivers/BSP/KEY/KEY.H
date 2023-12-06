/**
 ****************************************************************************************************
 * @file     KEY.H
 * @author   HFKM
 * @version  V1.0
 * @date     2022-02-12
 * @license  Copyright (c) 2020-2032, HFKM
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 Mini Pro H750开发板
 * 
 ****************************************************************************************************
 */

#ifndef __KEY_H
#define __KEY_H

#include "stm32h7xx.h"

/***************************************************************************************************/
/**
 * @brief   Set enumeration for each key
 */
typedef enum {
    KEY_VALUE_WAIT      = 0,
	KEY_VALUE_NONE      = 1,
    KEY_VALUE_KEY_UP    = 2,
    KEY_VALUE_KEY0      = 3,
    KEY_VALUE_KEY1      = 4
} KEY_VALUE_ENUM;

typedef enum{
    KEY_SCAN_MODE_SINGLE = 0,           //The return key terminates when pressed
	KEY_SCAN_MODE_CONTINUOUS   = 1		//Returns key value continuously
} KEY_SCAN_MODE;

/***************************************************************************************************/
// KEY_UP
#define KEY_UP_PORT                 GPIOA
#define KEY_UP_PIN                  GPIO_PIN_0
#define KEY_UP_CLK_ENABLE()         do { __HAL_RCC_GPIOA_CLK_ENABLE(); } while (0)
#define KEY_UP                      HAL_GPIO_ReadPin(KEY_UP_PORT, KEY_UP_PIN)

/***************************************************************************************************/
// KEY0
#define KEY0_PORT                   GPIOA
#define KEY0_PIN                    GPIO_PIN_1
#define KEY0_CLK_ENABLE()           do { __HAL_RCC_GPIOA_CLK_ENABLE(); } while (0)
#define KEY0                        HAL_GPIO_ReadPin(KEY0_PORT, KEY0_PIN)

/***************************************************************************************************/
// KEY1
#define KEY1_PORT                   GPIOA
#define KEY1_PIN                    GPIO_PIN_15
#define KEY1_CLK_ENABLE()           do { __HAL_RCC_GPIOA_CLK_ENABLE(); } while (0)
#define KEY1                        HAL_GPIO_ReadPin(KEY1_PORT, KEY1_PIN)

/***************************************************************************************************/
void KEY_All_Init(void);
void KEY_UP_Init(void);
void KEY0_Init(void);
void KEY1_Init(void);
KEY_VALUE_ENUM KEY_Scan(KEY_SCAN_MODE Mode);
void Key_Scan_Practical(void);
void Key_Trigger_Callback(KEY_VALUE_ENUM Key_Pressed);

/***************************************************************************************************/

#endif
