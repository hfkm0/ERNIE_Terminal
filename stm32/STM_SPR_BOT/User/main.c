/**
 ****************************************************************************************************
 * @file     main.c
 * @author   HFKM
 * @version  V1.0
 * @date     2023-10-26
 * @brief    硬件ADC语音上传，服务器语音处理下传, DAC播放
 * @license  Copyright (c) 2020-2032, HFKM
 ****************************************************************************************************
 * @attention
 * 
 * 项目使用MCU：STM32H750VBT6
 * 
 ****************************************************************************************************
 */

#define DEBUG           0

/* DEBUG宏为总开关，以下DEBUG不需要的注释掉 */
#if DEBUG == 1
#define DEBUG_DATA_TO_UART
// #define DEBUG_NO_ESP01S    
// #define DEBUG_NO_ADC
// #define DEBUG_NO_Upload
// #define DEBUG_NO_Download
// #define DEBUG_NO_Play     
// #define USART2_RX_DATA_Struct USART1_RX_DATA_Struct
// #define USART1_RX_DATA_Struct USART2_RX_DATA_Struct
#endif

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/USART/USART.H"
#include "./BSP/MPU/MPU.H"
#include "./BSP/ATIM/ATIM.H"
#include "./BSP/BTIM/BTIM.H"
#include "./BSP/LCD/lcd.h"
#include "./BSP/ADC/ADC.H"
#include "./BSP/DAC/DAC.H"
#include "./BSP/KEY/KEY.H"
#include "./BSP/ESP01S/ESP01S.H"
#include "./BSP/SYN6288/SYN6288.H"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#define MAX_TIME                100
#define ADC_SAMPING_RATE        1600                       /* 1/10 采样率 */

void Custom_Init(void);
void Custom_Loop(void);

void Key_Scan(void);
void Key_CallBack(KEY_VALUE_ENUM key);

void PCM_Upload(void);
void PCM_Download(void);
void PCM_Play_I2S(void);
void PCM_Play_SYN(void);

void PCM_Upload_Uart(void);

uint32_t Sampling_points = ADC_SAMPING_RATE * MAX_TIME;    /* 采样点数 */
uint16_t Voice_Value[ADC_SAMPING_RATE * MAX_TIME];         /* ADC采样值 */

uint8_t* GBK_Value[1000];

uint8_t ADC_State = 0;                                     /* ADC状态, 0:未启动 1:执行中 2:执行完毕   */
uint32_t ADC_Sampling_Count = 0;                           /* ADC采样计数 */
uint16_t remain_MAX_TIME = 0;

extern uint8_t DAC_Done_flag;

char BLANK_NULL[] = "                        ";

extern uint8_t UART_Rec_mode;

uint8_t voice_valid = 0;                                   /* 0：未上传数据 1：已上传数据,等待下传指令 2：已接收下传数据 3： */
uint32_t download_data_length = 0;                         /* 下载数据长度 */

int main(void)
{
    sys_cache_enable();                
    HAL_Init();                        
    sys_stm32_clock_init(240, 2, 2, 4);                /* 480Mhz */
    delay_init(480);                   
    MPU_Memory_Protection();

    Custom_Init();                    
    Custom_Loop();
}

/**
 * @brief       Custom initialization
 * @param       none
 * @retval      none
 */
void Custom_Init(void) {
    #ifndef DEBUG_NO_ESP01S
    uint8_t error_code = 0;
    #endif

    USART1_DMA_Transmit_Init(8, 16, 1);
    USART1_DMA_Receive_Init(8, 16, 1);
    USART1_Init(921600);                             /* 上位机通信用 */

    USART2_DMA_Transmit_Init(8, 16, 1);
    USART2_DMA_Receive_Init(8, 8, 1);
    USART2_Init(921600);                             /* 上位机通信用 */

    USART6_Init(9600, 0);

    SYN6288_Busy_Init();
	
    lcd_init();                                     

	DAC12_Init(1);                                   /* 12位DAC初始化 */

	ATIM1_OC_Channel1_Init(75-1, 100-1, 50);         /* 16Khz Sampling rate */
    
    KEY0_Init();
    KEY1_Init();

    BTIM6_Init(150-1, 200-1);                        /* 8Khz */

    #ifndef DEBUG_NO_ESP01S
    while(1){
        lcd_show_string(0, 0, 240, 16, 16, BLANK_NULL, BLACK);
        lcd_show_string(0, 20, 240, 16, 16, BLANK_NULL, BLACK);
        lcd_show_string(0, 0, 240, 16, 16, "ESP01S Initing...", BLACK);
        UART_Rec_mode = 1;
        ESP01S_Reset();
        error_code = ESP01S_Init();
        if(!error_code){
            lcd_show_string(0, 0, 240, 16, 16, BLANK_NULL, BLACK);
            lcd_show_string(0, 0, 240, 16, 16, "ESP01S Init OK", BLACK);
            UART_Rec_mode = 0;
            break;
        }else{
            lcd_show_string(0, 0, 240, 16, 16, BLANK_NULL, BLACK);
            lcd_show_string(0, 0, 240, 16, 16, "error_code:", BLACK);
            lcd_show_num(90, 0, error_code, 2, 16, BLACK);
            lcd_show_string(0, 20, 240, 16, 16, BLANK_NULL, BLACK);
            lcd_show_string(0, 20, 240, 16, 16, "ESP01S Init error", BLACK);
        }
        delay_ms(1000);
    }
    #endif

    lcd_clear(WHITE);
}

/**
 * @brief       Custom loop
 * @param       none
 * @retval      none
 */
void Custom_Loop(void) {
    lcd_show_string(0, 0, 240, 16, 16, "STM_ERNIE_BOT", BLACK);

    while(1){
        if(ADC_State == 1  && voice_valid == 0){
            #ifndef DEBUG_NO_ADC
            if(ADC1_Channel19_DMA_Finish_Flag == 1){
                ADC1_Channel19_DMA_Finish_Flag = 0;
                remain_MAX_TIME--;
                if(remain_MAX_TIME == 0){
                    ADC_State = 2;
                    HAL_ADC_Stop_DMA(&ADC1_HAL_Handle);
                    lcd_show_string(0, 20, 240, 16, 16, (char*)BLANK_NULL, BLACK);
                    lcd_show_string(0, 20, 240, 16, 16, "Record Done!", BLACK);
                    
                }else{
                    /* 数据不满MAX_TIME时间，继续转换 */
                    ADC_Sampling_Count += ADC_SAMPING_RATE;
                    ADC1_Channel19_TimTri_Init((uint32_t)Voice_Value + (MAX_TIME - remain_MAX_TIME) * ADC_SAMPING_RATE * 2, ADC_SAMPING_RATE);
                }
            }
            #else
            ADC_State = 2;
            lcd_show_string(0, 20, 240, 16, 16, (char*)BLANK_NULL, BLACK);
            lcd_show_string(0, 20, 240, 16, 16, "Record Done!", BLACK);
            #endif
        }
        else if(ADC_State == 2){
            #ifndef DEBUG_NO_Upload
            #ifndef DEBUG_DATA_TO_UART
                PCM_Upload();
            #else
                PCM_Upload_Uart();
            #endif
            #else
            lcd_show_string(0, 20, 240, 16, 16, (char*)BLANK_NULL, BLACK);
            lcd_show_string(0, 40, 240, 16, 16, (char*)BLANK_NULL, BLACK);
            lcd_show_string(0, 40, 240, 16, 16, "Uploading Done!", BLACK);
            lcd_show_string(0, 60, 240, 16, 16, (char*)BLANK_NULL, BLACK);
            voice_valid = 1;
            #endif
            ADC_State = 0;
        }

        /* voice_valid变量可以被优化掉，加上voice_valid方便后续功能添加 */
        if(voice_valid == 1){
            #ifndef DEBUG_NO_Download
            PCM_Download();
            #else
            lcd_show_string(0, 40, 240, 16, 16, (char*)BLANK_NULL, BLACK);
            lcd_show_string(0, 40, 240, 16, 16, "Downloading Done!", BLACK);
            lcd_show_string(0, 60, 240, 16, 16, (char*)BLANK_NULL, BLACK);
            #endif
        }else if(voice_valid == 2){
            #ifndef DEBUG_NO_Play
            // PCM_Play_I2S();
            PCM_Play_SYN();
            #else
            lcd_show_string(0, 40, 240, 16, 16, (char*)BLANK_NULL, BLACK);
            lcd_show_string(0, 40, 240, 16, 16, "Playing Done!", BLACK);
            #endif
            voice_valid = 0;
        }

        Key_Scan();
        delay_ms(10);
    }
}

/**
 * @brief       按键扫描
 * @param       none
 * @retval      none
 */
void Key_Scan(void)
{
    KEY_VALUE_ENUM key = KEY_VALUE_NONE;
    static uint8_t key_press = 0;
    key = KEY_Scan((KEY_SCAN_MODE)0);
    if(key_press == 0){
        if(key != KEY_VALUE_NONE){
            key_press = 1;
        }
    }else if(key_press == 1){
        if(key == KEY_VALUE_NONE){
            key_press = 0;
        }else{
            Key_CallBack(key);
            key_press = 2;
        }
    }else{
        if(key == KEY_VALUE_NONE){
            key_press = 0;
        }
    }
    
}

/**
 * @brief       按键回调函数
 * @param       key : trigger key
 * @retval      none
 */
void Key_CallBack(KEY_VALUE_ENUM key) {
    switch (key) {
        case KEY_VALUE_KEY_UP: {
            break;
        }
        case KEY_VALUE_KEY0: {
            if(ADC_State == 0){
                ADC_State = 1;
                voice_valid = 0;
                #ifndef DEBUG_NO_ADC
                ADC_Sampling_Count = 0;
                remain_MAX_TIME = MAX_TIME;
                ADC_Sampling_Count += ADC_SAMPING_RATE;
                ADC1_Channel19_TimTri_Init((uint32_t)Voice_Value + (MAX_TIME - remain_MAX_TIME) * ADC_SAMPING_RATE * 2, ADC_SAMPING_RATE);
                lcd_show_string(0, 20, 240, 16, 16, (char*)BLANK_NULL, BLACK);
                lcd_show_string(0, 20, 240, 16, 16, "Recording...", BLACK);
                #endif
            }else if(ADC_State == 1){
                /* ADC采样期间再次按下KEY0，采样终止 */
                ADC_State = 2;
                /* Close ADC */
                while(ADC1_Channel19_DMA_Finish_Flag != 1);
                ADC1_Channel19_DMA_Finish_Flag = 0;

                HAL_ADC_Stop_DMA(&ADC1_HAL_Handle);

                lcd_show_string(0, 20, 240, 16, 16, (char*)BLANK_NULL, BLACK);
                lcd_show_string(0, 20, 240, 16, 16, "Record Done!", BLACK);
            }
            break;
        }
        case KEY_VALUE_KEY1: {
            PCM_Play_SYN();
            break;
        }
    }
}

/**
 * @brief       将采集到的音频数据通过串口上传
 * @param       none
 * @retval      none
 * @note        ESP01S每次发包最大负载为2048字节
 */
void PCM_Upload(void)
{
    uint32_t length;
    uint32_t offset = 0;
    uint8_t command[20];
    uint8_t header[20];
    /* 前128字节丢弃，有00 00数据，原因未知 */
    length = ADC_Sampling_Count * 2 - 128;
    offset += 128;

    lcd_show_string(0, 20, 240, 16, 16, (char*)BLANK_NULL, BLACK);
    lcd_show_string(0, 40, 240, 16, 16, (char*)BLANK_NULL, BLACK);
    lcd_show_string(0, 60, 240, 16, 16, (char*)BLANK_NULL, BLACK);
    lcd_show_string(0, 80, 240, 16, 16, (char*)BLANK_NULL, BLACK);
    lcd_show_num(0, 40, length, 8, 16, BLACK);
    lcd_show_string(0, 60, 240, 16, 16, "Uploading...", BLACK);

    /* 向服务器发送音频数据长度 */
    /* 格式 "audio+xxxxxxxx" */
    UART_Rec_mode = 1;
    sprintf((char*)header, "audio+%.8d\r\n", length);
    ESP01S_Transmit((uint8_t*)header, 16, "AT+CIPSEND=16\r\n", 15);
    delay_ms(10);
    
    while(length > 2048){
        ESP01S_Transmit((uint8_t*)Voice_Value + offset, 2048, "AT+CIPSEND=2048\r\n", 17);
        length -= 2048;
        offset += 2048; 
    }
    sprintf((char*)command, "AT+CIPSEND=%d\r\n", length);
    ESP01S_Transmit((uint8_t*)Voice_Value + offset, length, command, strlen((char*)command));
    UART_Rec_mode = 0;
    lcd_show_string(0, 40, 240, 16, 16, "Uploading Done!", BLACK);
    lcd_show_string(0, 60, 240, 16, 16, (char*)BLANK_NULL, BLACK);

    voice_valid = 1;
}

/**
 * @brief       接收来自服务器下发的音频数据
 * @param       none
 * @retval      none
 * @note        接收时会有部分来自ESP01S发送的提示字符
 *              需要将这部分滤除
 */
void PCM_Download(void)
{
    char* pos = NULL;

    lcd_show_string(0, 40, 240, 16, 16, (char*)BLANK_NULL, BLACK);
    lcd_show_string(0, 40, 240, 16, 16, "Downloading...", BLACK);
    lcd_show_string(0, 60, 240, 16, 16, (char*)BLANK_NULL, BLACK);
    
    UART_Rec_mode = 1;
    
    while(1){
        if((pos = strstr((char*)USART2_RX_DATA_Struct.pDatas, "end")) != NULL){
            /* ESP01S接收格式为 "+IPD,xxxx:datas" */
            /* 其中xxxx为接收到的数据量，长度不定可为1~1xxx */
            /* 服务器发送的数据格式为 "ready+xxxxxxxxend\r\n " */
            /* 其中xxxxxxxx为字节数 */
            UART_Rec_mode = 2;
            download_data_length = atoi((char*)pos - 9);       

            memset(USART2_RX_DATA_Struct.pDatas, 0, USART2_RX_DATA_Struct.State&0x3fff);
            USART2_RX_DATA_Struct.State = 0;

            lcd_show_string(0, 60, 240, 16, 16, (char*)BLANK_NULL, BLACK);
            lcd_show_num(0, 60, download_data_length, 8, 16, BLACK);

            while(voice_valid != 2);
            break;
        }
    }

    lcd_show_string(0, 40, 240, 16, 16, (char*)BLANK_NULL, BLACK);
    lcd_show_string(0, 40, 240, 16, 16, "Downloading Done!", BLACK);
    lcd_show_string(0, 60, 240, 16, 16, (char*)BLANK_NULL, BLACK);

    UART_Rec_mode = 0;
}

/**
 * @brief       播放来自服务器下发的音频数据, 使用I2S DAC播放
 * @param       none
 * @retval      none
 * @note        使用DAC播放
 *              上传数据为16Khz采样率，16bit采样精度，单声道
 *              下载数据为 8Khz采样率，12bit采样精度，单声道
 *              下载数据低采样率是为了容纳更多的语音时长
 *              下载数据采样精度低是因为DAC精度为12bit，实际占用2字节
 */
void PCM_Play_I2S(void)
{
    if(!download_data_length){
        return;
    }
    lcd_show_string(0, 40, 240, 16, 16, (char*)BLANK_NULL, BLACK);
    lcd_show_string(0, 40, 240, 16, 16, "Playing...", BLACK);
    lcd_show_string(0, 60, 240, 16, 16, (char*)BLANK_NULL, BLACK);
    
    HAL_TIM_Base_Start(&BTIM6_HAL_Handle);
    HAL_TIM_Base_Start_IT(&BTIM6_HAL_Handle);
    
    while(!DAC_Done_flag);
    DAC_Done_flag = 0;
    lcd_show_string(0, 40, 240, 16, 16, (char*)BLANK_NULL, BLACK);
    lcd_show_string(0, 40, 240, 16, 16, "Playing Done!", BLACK);
}

/**
 * @brief       播放来自服务器下发的音频数据, 使用SYN6288播放
 * @param       none
 * @retval      none
 * @note        使用DAC播放
 *              上传数据为16Khz采样率，16bit采样精度，单声道
 *              下载数据为 8Khz采样率，12bit采样精度，单声道
 *              下载数据低采样率是为了容纳更多的语音时长
 *              下载数据采样精度低是因为DAC精度为12bit，实际占用2字节
 */
void PCM_Play_SYN(void)
{
    uint8_t gbk_voice[80];
    uint16_t length;
    uint16_t pos = 0;

    if(!download_data_length){
        return;
    }
    lcd_show_string(0, 40, 240, 16, 16, (char*)BLANK_NULL, BLACK);
    lcd_show_string(0, 40, 240, 16, 16, "Playing...", BLACK);
    lcd_show_string(0, 60, 240, 16, 16, (char*)BLANK_NULL, BLACK);
    
    /* 末尾两字节为 0x0D 0x0A */
    length = download_data_length - 2;

    while(length > 160){
        memcpy(gbk_voice, (uint8_t*)Voice_Value + pos, 160);
        SYN6288_Say(gbk_voice, 160);
        length -= 160;
        pos += 160;

        delay_ms(100);

        while(SYN6288_State_Check()){
            delay_ms(10);
        };
    }
    memcpy(gbk_voice, (uint8_t*)Voice_Value + pos, length);
    SYN6288_Say(gbk_voice, length);
    while(SYN6288_State_Check());
    
    lcd_show_string(0, 40, 240, 16, 16, (char*)BLANK_NULL, BLACK);
    lcd_show_string(0, 40, 240, 16, 16, "Playing Done!", BLACK);
}

/**
 * @brief       音频数据串口上传调试
 * @param       none
 * @retval      none
 */
void PCM_Upload_Uart(void)
{
    uint32_t length;
    uint32_t offset = 0;
    length = ADC_Sampling_Count * 2;

    lcd_show_string(0, 20, 240, 16, 16, (char*)BLANK_NULL, BLACK);
    lcd_show_num(0, 20, length, 8, 16, BLACK);
    lcd_show_string(0, 40, 240, 16, 16, (char*)BLANK_NULL, BLACK);
    lcd_show_string(0, 40, 240, 16, 16, "Uploading...", BLACK);

    while(length > 65535){
        USART1_DMA_Transmit_Start(65535, (uint8_t*)Voice_Value + offset);
        length -= 65535;
        offset += 65535;
        while(USART1_TX_DATA_Finish_Flag == 0);
        USART1_TX_DATA_Finish_Flag = 0;
    }
    USART1_DMA_Transmit_Start(length, (uint8_t*)Voice_Value + offset);
    while(USART1_TX_DATA_Finish_Flag == 0);
    USART1_TX_DATA_Finish_Flag = 0;
    lcd_show_string(0, 20, 240, 16, 16, "Uploading Done!", BLACK);
    lcd_show_string(0, 40, 240, 16, 16, (char*)BLANK_NULL, BLACK);
}
