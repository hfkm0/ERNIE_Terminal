/**
 ****************************************************************************************************
 * @file     KEY.C
 * @author   HFKM
 * @version  V1.0
 * @date     2023-02-12
 * @brief    按键-HAL库版本 实验
 * @license  Copyright (c) 2020-2032, HFKM
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 Mini Pro H750开发板
 * 
 ****************************************************************************************************
 */

#include "./BSP/KEY/key.h"

/**
 * @brief    Initialize all keys
 * @param    none
 * @retval   none
 */
void KEY_All_Init(void)
{
    KEY_UP_Init();
    KEY0_Init();
    KEY1_Init();
}

/**
 * @brief    Initialize the KEY_UP
 * @param    none
 * @retval   none
 */
void KEY_UP_Init(void) {
    GPIO_InitTypeDef KEY_UP_init_struct;
    KEY_UP_CLK_ENABLE();
    KEY_UP_init_struct.Mode = GPIO_MODE_INPUT;
    KEY_UP_init_struct.Pin = KEY_UP_PIN;
    KEY_UP_init_struct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(KEY_UP_PORT, &KEY_UP_init_struct);
}

/**
 * @brief    Initialize the KEY0
 * @param    none
 * @retval   none
 */
void KEY0_Init(void) {
    GPIO_InitTypeDef KEY0_init_struct;
    KEY0_CLK_ENABLE();
    KEY0_init_struct.Mode = GPIO_MODE_INPUT;
    KEY0_init_struct.Pin = KEY0_PIN;
    KEY0_init_struct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEY0_PORT, &KEY0_init_struct);
}

/**
 * @brief    Initialize the KEY1
 * @param    none
 * @retval   none
 */
void KEY1_Init(void) {
    GPIO_InitTypeDef KEY1_init_struct;
    KEY1_CLK_ENABLE();
    KEY1_init_struct.Mode = GPIO_MODE_INPUT;
    KEY1_init_struct.Pin = KEY1_PIN;
    KEY1_init_struct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEY1_PORT, &KEY1_init_struct);
}

/**
 * @brief    Scan keys
 * @param    Mode: Detection mode
 * @retval   KEY_VALUE
 * @note return value is the key which be pressed
 *       Priority:  KEY_UP > KEY0 > KEY1
 */
KEY_VALUE_ENUM KEY_Scan(KEY_SCAN_MODE Mode) {
    if (KEY_UP == 1) {
        return KEY_VALUE_KEY_UP;
    } else if (KEY0 == 0) {
        return KEY_VALUE_KEY0;
    } else if (KEY1 == 0) {
        return KEY_VALUE_KEY1;
    } else{
		return KEY_VALUE_NONE;
	}
}

/**
 * @brief      practical and simple scan keys 
 * @param       none
 * @retval      none
 */
void Key_Scan_Practical(void) {
    KEY_VALUE_ENUM Key_Pressed = KEY_VALUE_NONE;
    while (1) {
        Key_Pressed = KEY_Scan(KEY_SCAN_MODE_SINGLE);
        if (Key_Pressed) {
            //delay_ms(1);//Add delay to eleminate jitter
            Key_Pressed = KEY_Scan(KEY_SCAN_MODE_SINGLE);
            if (Key_Pressed) {
                Key_Trigger_Callback(Key_Pressed);
				while (KEY_Scan(KEY_SCAN_MODE_SINGLE) != KEY_VALUE_NONE)
            ;
            }
        }
    }
}

/**
 * @brief       According to trigger key, trigger callback function
 * @param       key_Pressed : trigger key
 * @retval      none
 */
void Key_Trigger_Callback(KEY_VALUE_ENUM Key_Pressed) {
    switch (Key_Pressed) {
        case KEY_VALUE_KEY_UP: {
            break;
        }
        case KEY_VALUE_KEY0: {
            break;
        }
        case KEY_VALUE_KEY1: {
            break;
        }
    }
}


