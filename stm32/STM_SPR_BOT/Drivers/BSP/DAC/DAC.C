/**
 ****************************************************************************************************
 * @file     DAC.C
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

#include "./BSP/DAC/DAC.H"
#include "./SYSTEM/delay/delay.h"

DAC_HandleTypeDef DAC12_HAL_Handle;

/********************************************* DAC 配置函数 *********************************************/

/**
 * @brief   DAC初始化
 * @param   outx: 要初始化的通道. 1,通道1; 2,通道2
 * @retval  none
 * @note    本函数支持 DAC1_OUT1/2通道初始化
 *          DAC的输入时钟来自APB1，时钟频率=120Mhz=8.3ns
 *          DAC在输出buffer关闭的时候，输出建立时间：tSETTING = 2us (H750数据手册有写)
 *          因此DAC输出的最高速度约为：500Khz
 *          若以10个点为一个周期，最大输出50Khz左右的波形
 */
void DAC12_Init(uint8_t outx)
{
    DAC_ChannelConfTypeDef dac_channel_str;
    GPIO_InitTypeDef gpio_init_str;

    __HAL_RCC_DAC12_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    gpio_init_str.Pin = (outx==1) ? GPIO_PIN_4 : GPIO_PIN_5;     /* STM32单片机，总是PA4=DAC1_OUT1，PA5=DAC1_OUT2 */
    gpio_init_str.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &gpio_init_str);
    
    DAC12_HAL_Handle.Instance = DAC1;
    HAL_DAC_Init(&DAC12_HAL_Handle);

    dac_channel_str.DAC_Trigger = DAC_TRIGGER_NONE;
    dac_channel_str.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;    /* DAC1输出缓冲关闭 */

    switch(outx){
        case 1:
            HAL_DAC_ConfigChannel(&DAC12_HAL_Handle, &dac_channel_str, DAC_CHANNEL_1);
            HAL_DAC_Start(&DAC12_HAL_Handle, DAC_CHANNEL_1);
            break;
        case 2:
            HAL_DAC_ConfigChannel(&DAC12_HAL_Handle, &dac_channel_str, DAC_CHANNEL_2);
            HAL_DAC_Start(&DAC12_HAL_Handle, DAC_CHANNEL_2);
            break;
        default:
            break;
    }
}

/********************************************* DAC 应用函数 *********************************************/

/**
 * @brief   DAC 设置通道1/2的输出电压
 * @param   hdac: DAC句柄
 * @param   outx: 要设置的通道. 1,通道1; 2,通道2
 * @param   val: 要设置的电压值，范围0~3300mV
 * @retval  none
 */
void DAC_Set_Voltage(DAC_HandleTypeDef hdac, uint8_t outx,uint16_t val)
{
    double temp = val;
    temp /= 1000;
    temp = temp * 4096 / 3.3;
    if(temp>=4096){
        temp = 4095;
    }
    /* 12位右对齐数据格式设置DAC值 */
    if(outx == 1){
        HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, temp);
    }else{
        HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, temp);
    }
}

/**
 * @brief   DAC12 设置通道1/2的输出电压
 * @param   hdac: DAC句柄
 * @param   outx: 要设置的通道. 1,通道1; 2,通道2
 * @param   val: 要设置的电压值，范围0~3300mV
 * @retval  none
 */
void DAC12_Set_Voltage(uint8_t outx, uint16_t val)
{
    double temp = val;
    temp /= 1000;
    temp = temp * 4096 / 3.3;
    if(temp>=4096){
        temp = 4095;
    }
    /* 12位右对齐数据格式设置DAC值 */
    if(outx == 1){
        HAL_DAC_SetValue(&DAC12_HAL_Handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, temp);
    }else{
        HAL_DAC_SetValue(&DAC12_HAL_Handle, DAC_CHANNEL_2, DAC_ALIGN_12B_R, temp);
    }
}

/**
 * @brief   DAC 指定通道输出三角波
 * @param   hdac: DAC句柄
 * @param   outx: 要设置的通道. 1,通道1; 2,通道2
 * @param   maxval: 三角波最大值，范围1~4095 -> 0~3.3V
 * @param   dt: 每个采样点的延时时间（us),输出频率 ≈ 1000/(dt*samples)khz,dt尽量不小于5us(延时会不准)
 * @param   samples: 一周期内采样点个数，samples必须小于等于 (maxval+1) * 2
 * @param   num:输出波形个数
 * @retval  none
 */
void DAC_Triangular_Wave(DAC_HandleTypeDef hdac, uint8_t outx, uint16_t maxval, uint16_t dt, uint16_t samples, uint16_t num)
{
    uint16_t i, j;
    float incval;   /* 电压递增值 */
    float curval;   /* 当前电压值 */
    /*参数检查*/
    if(maxval == 0 || maxval>4095 || samples>((maxval+1)*2)){
        return;
    }

    incval=((float)maxval+1) * 2  / samples;   /* 电压递增值 */

    for (i = 0; i < num; i++){
        for (j = 0; j < (samples / 2); j++){
            curval += incval;
            HAL_DAC_SetValue(&hdac, (outx==1)?DAC_CHANNEL_1:DAC_CHANNEL_2, DAC_ALIGN_12B_R, curval);
            delay_us(dt);
        }
        for (j = 0; j < (samples / 2); j++){
            curval -= incval;
            HAL_DAC_SetValue(&hdac, (outx==1)?DAC_CHANNEL_1:DAC_CHANNEL_2, DAC_ALIGN_12B_R, curval);
            delay_us(dt);
        }
    }
        
}

/**
 * @brief   DAC12 指定通道输出三角波
 * @param   outx: 要设置的通道. 1,通道1; 2,通道2
 * @param   maxval: 三角波最大值，范围1~4095
 * @param   dt: 每个采样点的延时时间（us),输出频率 ≈ 1000/(dt*samples)khz,dt尽量不小于5us(延时会不准)
 * @param   samples: 一周期内采样点个数，samples必须小于等于 (maxval+1) * 2
 * @param   num:输出波形个数
 * @retval  none
 */
void DAC12_Triangular_Wave(uint8_t outx, uint16_t maxval, uint16_t dt, uint16_t samples, uint16_t num)
{
    uint16_t i, j;
    float incval;   /* 电压递增值 */
    float curval;   /* 当前电压值 */
    /*参数检查*/
    if(maxval == 0 || maxval>4095 || samples>((maxval+1)*2)){
        return;
    }

    incval=((float)maxval+1) * 2  / samples;   /* 电压递增值 */

    for (i = 0; i < num; i++){
        for (j = 0; j < (samples / 2); j++){
            curval += incval;
            HAL_DAC_SetValue(&DAC12_HAL_Handle, (outx==1)?DAC_CHANNEL_1:DAC_CHANNEL_2, DAC_ALIGN_12B_R, curval);
            delay_us(dt);
        }
        for (j = 0; j < (samples / 2); j++){
            curval -= incval;
            HAL_DAC_SetValue(&DAC12_HAL_Handle, (outx==1)?DAC_CHANNEL_1:DAC_CHANNEL_2, DAC_ALIGN_12B_R, curval);
            delay_us(dt);
        }
    }
        
}
