/**
 ****************************************************************************************************
 * @file     SYN6288.C
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

#include "SYN6288.H"
#include "./BSP/USART/USART.H"
#include "string.h"

/**************SYN6288 控制指令(存储于ROM)*********************/
const uint8_t SYN6288_StopCom[] = {0xFD, 0X00, 0X02, 0X02, 0XFD};      //停止合成
const uint8_t SYN6288_SuspendCom[] = {0XFD, 0X00, 0X02, 0X03, 0XFC};   //暂停合成
const uint8_t SYN6288_RecoverCom[] = {0XFD, 0X00, 0X02, 0X04, 0XFB};   //恢复合成
const uint8_t SYN6288_ChackCom[] = {0XFD, 0X00, 0X02, 0X21, 0XDE};     //状态查询
const uint8_t SYN6288_PowerDownCom[] = {0XFD, 0X00, 0X02, 0X88, 0X77}; //进入POWER DOWN 状态命令

/**************SYN6288 语音文本控制配置(存储于ROM)*********************/
const uint8_t SYN6288_BackMusic = 0;
const uint8_t SYN6288_Forground_Volume[] = "[v10]";//前景音量，含提示音效 0~16
const uint8_t SYN6288_Background_Volume[] = "[m1]";//背景音量，0~16
const uint8_t SYN6288_Speed_Volume[] = "[t5]";//语音速度，0~5

/**************SYN6288 语音文本(存储于ROM)*********************/
//在这个参数设定总计的文本数量
uint16_t SYN6288_Words_Num = 1;
const char *SYN6288_Words[] = {"语音识别失败"};

/**
 * @brief   初始化SYN6288 BUSY引脚
 * @param   none
 * @retval  none
 * @note    none
 */
void SYN6288_Busy_Init(void)
{
    GPIO_InitTypeDef gpio_init_str;
    SYN6288_BUSY_CLK_ENABLE();
    gpio_init_str.Pin = SYN6288_BUSY_PIN;
    gpio_init_str.Mode = GPIO_MODE_INPUT;
    gpio_init_str.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(SYN6288_BUSY_PORT, &gpio_init_str);
}

/**
 * @brief   使SYN6288播放指定文本
 * @param   Words: 需要播放的文本
 * @param   len:   文本长度
 * @retval  无
 * @note    使用设定好的语音配置
 */
void SYN6288_Say(uint8_t* Words, uint8_t len){
    uint8_t Frame_Info[180]="";
    strcat((char*)Frame_Info, (char*)SYN6288_Forground_Volume);
    strcat((char*)Frame_Info, (char*)SYN6288_Background_Volume);
    strcat((char*)Frame_Info, (char*)SYN6288_Speed_Volume);
    // strcat((char*)Frame_Info, (char*)Words);
    memcpy(Frame_Info + 13, Words, len);
    SYN6288_Frame_Send(Frame_Info);
}

/**
 * @brief   发送数据到SYN6288使其播放指定文本
 * @param   Voicd_Data: 需要播放的文本
 * @retval  无
 * @note    波特率默认为9600
 */
void SYN6288_Frame_Send(uint8_t* Voice_Data){
    uint8_t Frame_Info[200]="";
    uint8_t Voice_Length;
    uint8_t Error_Check = 0;
    uint8_t temp = 0;
    Voice_Length=strlen((char*)Voice_Data);

    //Frame fixed configuration
    Frame_Info[0] = 0xfd;                   //帧头
    Frame_Info[1] = 0x00;                   //帧长度高字节
    Frame_Info[2] = Voice_Length + 3;       //帧长度低字节
                                            //如果不使用校验码，则为 +2
    Frame_Info[3] = 0x01;                   //控制指令
    Frame_Info[4] = 0x01 | SYN6288_BackMusic << 4;  //控制指令参数

    //校验码计算
    for (temp = 0; temp < 5;temp++){
        Error_Check = Error_Check ^ (Frame_Info[temp]);
    }
    for (temp = 0; temp < Voice_Length;temp++){
        Error_Check = Error_Check ^ (Voice_Data[temp]);
    }
    //发送帧
    memcpy(&Frame_Info[5], Voice_Data, Voice_Length);
    Frame_Info[5 + Voice_Length] = Error_Check;
    //如果不使用校验码，长度为 5 + Voice_Length
    SYN6288_SendCommand(&SYN6288_UART_Handler, Frame_Info, 5 + Voice_Length + 1, 0xFFFF);
}

/**
 * @brief   Configures SYN6288 settings
 * @param   Config: config information
 * @retval  none
 */
void SYN6288_Config_Set(uint8_t* Config){
    uint8_t Config_length;
    Config_length = strlen((char*)Config);
    SYN6288_SendCommand(&SYN6288_UART_Handler, Config, Config_length, 0xFFFF);
}

/**
 * @brief   SYN6288 state check
 * @param   none
 * @retval  0: IDLE     1: BUSY
 */
uint8_t SYN6288_State_Check(void){
    return HAL_GPIO_ReadPin(SYN6288_BUSY_PORT, SYN6288_BUSY_PIN);
}
