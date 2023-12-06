/**
 ****************************************************************************************************
 * @file     ESP01S.C
 * @author   HFKM
 * @version  V1.0
 * @date     2023-10-30
 * @license  Copyright (c) 2020-2032, HFKM
 ****************************************************************************************************
 * @attention
 *
 ****************************************************************************************************
 */

#include "ESP01S.H"
#include "./SYSTEM/delay/delay.h"
#include  "string.h"

#define AP_SSID     "Zhihao"
#define AP_PASSWD   "czh36578"

#define TCP_IP      "124.223.113.195"
#define TCP_PORT    6233

/**
 * @brief   数据检索
 * @param   str:     查询数据
 * @param   str_len: 查询数据长度
 * @param   data:    源数据
 * @param   data_len:源数据长度
 * @retval  0:   失败      others:  偏移地址
 * @note    strstr无法满足使用需求
 */
uint16_t Data_find(uint8_t* str, uint16_t str_len, uint8_t* data, uint16_t data_len){
    uint16_t str_pos = 0;
    uint16_t data_pos = 0;

    for(; data_pos < data_len; data_pos++){
        if(data[data_pos] == str[0]){
            for(str_pos = 0; str_pos < str_len; str_pos++){
                if(data[data_pos + str_pos] != str[str_pos]){
                    str_pos = 0;
                    break;
                }
            }
            if(str_pos == str_len){
                return data_pos;
            }
        }
    }
    return 0;
}

/**
 * @brief   ESP01S指令发送
 * @param   command:    指令
 * @param   keyword:    相应关键字
 * @param   timeout:    超时时间(单位100ms)
 * @retval  0:   成功      others:  失败
 */
uint8_t ESP01S_Command(uint8_t* command,uint8_t* keyword, uint16_t timeout)
{
    USART2_RX_DATA_Struct.State = 0;
    HAL_UART_Transmit(&ESP01S_UART, command, strlen((char*)command), 0xFFFF);

    while(1){
        if(Data_find(keyword, strlen((char*)keyword), USART2_RX_DATA_Struct.pDatas,USART2_RX_DATA_Struct.State & 0x3fff) != NULL){
            memset(USART2_RX_DATA_Struct.pDatas, 0, USART2_RX_DATA_Struct.State&0x3FFF);
            USART2_RX_DATA_Struct.State = 0;
            return 0;
        }
        delay_ms(100);
        if(timeout-- == 0){
            return 1;
        }        
    }
}

/**
 * @brief   ESP01S初始化
 * @param   none
 * @retval  0:   初始化成功      others:  初始化失败
 * @note    ESP01S使用的芯片为ESP8266
 *          初始化顺序:
 *              0.  ATE0  关闭回显
 *              1.  AT  测试AT指令是否正常
 *              2.  AT+CWMODE_CUR=1  设置为STA模式
 *              3.  AT+CWJAP_CUR=AP_SSID,AP_PASSWD  连接AP
 *              4.  AT+CIFSR  获取IP地址
 *              5.  AT+CIPMUX=0 设置为单连接模式
 *              6.  AT+CIPSTART="TCP",TCP_IP,TCP_PORT  连接服务器
 *              7.  AT+CIPMODE=1    设置透传模式
 *              8.  AT+CIPSEND      开启透传
 */
uint8_t ESP01S_Init(void)
{
    /* ATE0 */
    if(ESP01S_Command((uint8_t*)"ATE0\r\n", (uint8_t*)"OK", 20)){ return 1;}

    /* AT */
    if(ESP01S_Command((uint8_t*)"AT\r\n", (uint8_t*)"OK", 20)){ return 2;}

    /* AT+CWMODE_CUR=1 */
    if(ESP01S_Command((uint8_t*)"AT+CWMODE_CUR=1\r\n", (uint8_t*)"OK", 20)){ return 3;};

    /* AT+CWJAP_CUR=AP_SSID,AP_PASSWD */
    if(ESP01S_Command((uint8_t*)"AT+CWJAP_CUR=\"Zhihao\",\"czh36578\"\r\n", (uint8_t*)"OK", 100)){ return 4;};

    /* AT+CIFSR */
    if(ESP01S_Command((uint8_t*)"AT+CIFSR\r\n", (uint8_t*)"OK", 50)){ return 5;};

    /* AT+CIPMUX=0 */
    if(ESP01S_Command((uint8_t*)"AT+CIPMUX=0\r\n", (uint8_t*)"OK", 20)){ return 6;};

    /* AT+CIPSTART="TCP",TCP_IP,TCP_PORT */
    if(ESP01S_Command((uint8_t*)"AT+CIPSTART=\"TCP\",\"124.223.113.195\",6233\r\n", (uint8_t*)"OK", 100)){ return 7;};

    /* AT+CIPMODE=0 */
    if(ESP01S_Command((uint8_t*)"AT+CIPMODE=0\r\n", (uint8_t*)"OK", 20)){ return 6;};
    
    // /* AT+CIPSEND */
    // if(ESP01S_Command((uint8_t*)"AT+CIPSEND\r\n", (uint8_t*)">", 30)){ return 6;};

    return 0;
}

/**
 * @brief   ESP01S复位
 * @param   none
 * @retval  none
 * @note    ESP01S复位后会发送大量无用数据
 *          复位完成后会发送 \r\nready\r\n
 */
void ESP01S_Reset(void)
{
    HAL_UART_Transmit(&ESP01S_UART, (uint8_t*)"AT+RST\r\n", 8, 0xFFFF);
    while(1){
        if(Data_find((uint8_t*)"ready", 5, USART2_RX_DATA_Struct.pDatas,USART2_RX_DATA_Struct.State & 0x3fff) != NULL){
            memset(USART2_RX_DATA_Struct.pDatas, 0, USART2_RX_DATA_Struct.State&0x3FFF);
            USART2_RX_DATA_Struct.State = 0;
            return;
        }
        delay_ms(100);
    }
}

/**
 * @brief   ESP01S发送字节数据
 * @param   data
 * @param   data_len
 * @param   command
 * @param   command_len
 * @retval  none
 * @note    ESP01S发送数据，数据长度最大2048字节
 *          直接给出指令，省去每次数字转字符
 */
void ESP01S_Transmit(uint8_t* data, uint16_t data_len, uint8_t* command, uint16_t command_len)
{
    HAL_UART_Transmit(&ESP01S_UART, command, command_len, 0xFFFF); 
    while(1){
        if(Data_find((uint8_t*)">", 1, USART2_RX_DATA_Struct.pDatas, USART2_RX_DATA_Struct.State & 0x3fff) != NULL){
            memset(USART2_RX_DATA_Struct.pDatas, 0, USART2_RX_DATA_Struct.State&0x3FFF);
            USART2_RX_DATA_Struct.State = 0;
            break;
        }
        delay_ms(1);
    }
    /* 不等就等死 */
    delay_ms(5);
    HAL_UART_Transmit(&ESP01S_UART, data, data_len, 0xFFFF);
    while(1){
        if(Data_find((uint8_t*)"OK", 2, USART2_RX_DATA_Struct.pDatas, USART2_RX_DATA_Struct.State & 0x3fff) != NULL){
            memset(USART2_RX_DATA_Struct.pDatas, 0, USART2_RX_DATA_Struct.State&0x3FFF);
            USART2_RX_DATA_Struct.State = 0;
            /* 不等就等死 */
            delay_ms(5);
            return;
        }
        delay_ms(1);
    }
}

