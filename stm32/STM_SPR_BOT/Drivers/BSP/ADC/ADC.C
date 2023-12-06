/**
 ****************************************************************************************************
 * @file     ADC.C
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

#include "ADC.H"
#include "./SYSTEM/delay/delay.h"

/*ADC GLOBLE VARIABLE*******************************************************************************/
ADC_HandleTypeDef ADC1_HAL_Handle;

/*ADC1 Channel19 PA5********************************************************************************/
DMA_HandleTypeDef DMA1_CHANNEL7_HAL_Handle;

/**
 * @brief       ADC1 Channel19 PA5 初始化
 * @param       none
 * @retval      none
 * @note        使用函数ADC1_Channel19_Read()读取ADC值时配置通道
 */
void ADC1_Channel19_Init(void){
    GPIO_InitTypeDef adc_gpio_init_str;

    /*GPIO init*/
    ADC1_CHANNEL19_GPIO_CLK_ENABLE();
    adc_gpio_init_str.Mode = GPIO_MODE_ANALOG;
    adc_gpio_init_str.Pin = ADC1_CHANNEL19_GPIO_PIN;
    HAL_GPIO_Init(ADC1_CHANNEL19_GPIO_PORT, &adc_gpio_init_str);

    /*ADC init*/
    ADC1_CHANNEL19_ADC_CLK_ENABLE();
    __HAL_RCC_ADC_CONFIG(ADC1_CHANNEL19_SELECT_CLK);
    ADC1_HAL_Handle.Instance=ADC1_CHANNEL19_ADC;
    //Input clock DIV2 :    adc_ker_ck=per_ck/2=32Mhz
    ADC1_HAL_Handle.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV2;
    ADC1_HAL_Handle.Init.ContinuousConvMode = ENABLE;
    ADC1_HAL_Handle.Init.DiscontinuousConvMode = DISABLE;
    ADC1_HAL_Handle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_ONESHOT;
    ADC1_HAL_Handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    ADC1_HAL_Handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    ADC1_HAL_Handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    ADC1_HAL_Handle.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
    ADC1_HAL_Handle.Init.LowPowerAutoWait = DISABLE;
    ADC1_HAL_Handle.Init.NbrOfConversion = 1;
    ADC1_HAL_Handle.Init.NbrOfDiscConversion = 0;
    ADC1_HAL_Handle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
    ADC1_HAL_Handle.Init.OversamplingMode= DISABLE;
    ADC1_HAL_Handle.Init.Resolution = ADC_RESOLUTION_16B;
    ADC1_HAL_Handle.Init.ScanConvMode = ADC_SCAN_DISABLE;
    HAL_ADC_Init(&ADC1_HAL_Handle);
    HAL_ADCEx_Calibration_Start(&ADC1_HAL_Handle, ADC_CALIB_OFFSET,
                                ADC_SINGLE_ENDED);    
}

/**
 * @brief       ADC1 Channel19 PA5 使用DMA发送 外设到内存 初始化
 * @param       memory_address:内存地址    
 * @param       length:数据长度
 * @param       dma_mode:DMA模式 0:循环模式 1:一次模式
 * @retval      none
 */ 
void ADC1_Channel19_DMA_Init(uint32_t memory_address,uint32_t length,uint16_t dma_mode)
{
    /*GPIO init*/
    GPIO_InitTypeDef adc_gpio_init_str;

    ADC1_CHANNEL19_GPIO_CLK_ENABLE();
    adc_gpio_init_str.Mode = GPIO_MODE_ANALOG;
    adc_gpio_init_str.Pin = ADC1_CHANNEL19_GPIO_PIN;
    HAL_GPIO_Init(ADC1_CHANNEL19_GPIO_PORT, &adc_gpio_init_str);

    /*ADC init*/
    ADC1_CHANNEL19_ADC_CLK_ENABLE();
    __HAL_RCC_ADC_CONFIG(ADC1_CHANNEL19_SELECT_CLK);
    ADC1_HAL_Handle.Instance=ADC1_CHANNEL19_ADC;
    //Input clock DIV2 :    adc_ker_ck=per_ck/2=32Mhz
    ADC1_HAL_Handle.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV2;
    ADC1_HAL_Handle.Init.ContinuousConvMode = ENABLE;
    ADC1_HAL_Handle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_ONESHOT;
    ADC1_HAL_Handle.Init.DiscontinuousConvMode = DISABLE;
    ADC1_HAL_Handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    ADC1_HAL_Handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    ADC1_HAL_Handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    ADC1_HAL_Handle.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
    ADC1_HAL_Handle.Init.LowPowerAutoWait = DISABLE;
    ADC1_HAL_Handle.Init.NbrOfConversion = 1;
    ADC1_HAL_Handle.Init.NbrOfDiscConversion = 0;
    ADC1_HAL_Handle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
    ADC1_HAL_Handle.Init.OversamplingMode= DISABLE;
    ADC1_HAL_Handle.Init.Resolution = ADC_RESOLUTION_16B;
    ADC1_HAL_Handle.Init.ScanConvMode = ADC_SCAN_DISABLE;
    HAL_ADC_Init(&ADC1_HAL_Handle);
    HAL_ADCEx_Calibration_Start(&ADC1_HAL_Handle, ADC_CALIB_OFFSET,
                                ADC_SINGLE_ENDED);    
                                
    /*ADC channel init*/
    /*频道配置需在ADC初始化后进行*/
    ADC_ChannelConfTypeDef adc_channel_cfg_str;
    adc_channel_cfg_str.Channel = ADC1_CHANNEL19_CHANNEL;
    adc_channel_cfg_str.Rank = ADC_REGULAR_RANK_1;
    // adc_channel_cfg_str.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;
    adc_channel_cfg_str.SamplingTime = ADC_SAMPLETIME_8CYCLES_5;
    adc_channel_cfg_str.SingleDiff = ADC_SINGLE_ENDED;
    adc_channel_cfg_str.OffsetNumber = ADC_OFFSET_NONE;
    adc_channel_cfg_str.Offset = 0;
    adc_channel_cfg_str.OffsetRightShift=DISABLE;
    adc_channel_cfg_str.OffsetSignedSaturation=DISABLE;

    HAL_ADC_ConfigChannel(&ADC1_HAL_Handle, &adc_channel_cfg_str);

    /*DMA init*/
    ADC1_CHANNEL19_DMA_CLK_ENABLE();
    DMA1_CHANNEL7_HAL_Handle.Instance = ADC1_CHANNEL19_DMA;
    DMA1_CHANNEL7_HAL_Handle.Init.Request = ADC1_CHANNEL19_DMA_REQ;
    DMA1_CHANNEL7_HAL_Handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
    DMA1_CHANNEL7_HAL_Handle.Init.PeriphInc = DMA_PINC_DISABLE;
    DMA1_CHANNEL7_HAL_Handle.Init.MemInc = DMA_MINC_ENABLE;
    DMA1_CHANNEL7_HAL_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    DMA1_CHANNEL7_HAL_Handle.Init.MemDataAlignment = DMA_PDATAALIGN_HALFWORD;
    dma_mode ? (DMA1_CHANNEL7_HAL_Handle.Init.Mode = DMA_CIRCULAR)
             : (DMA1_CHANNEL7_HAL_Handle.Init.Mode = DMA_NORMAL);
    DMA1_CHANNEL7_HAL_Handle.Init.Priority = DMA_PRIORITY_HIGH;
    DMA1_CHANNEL7_HAL_Handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    
    HAL_DMA_Init(&DMA1_CHANNEL7_HAL_Handle);
    __HAL_LINKDMA(&ADC1_HAL_Handle, DMA_Handle, DMA1_CHANNEL7_HAL_Handle);

    /*DMA interrupt init*/
    HAL_NVIC_SetPriority(ADC1_CHANNEL19_DMA_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(ADC1_CHANNEL19_DMA_IRQn);

    /*Start ADC*/
    HAL_DMA_Start_IT(&DMA1_CHANNEL7_HAL_Handle, (uint32_t)&ADC1_CHANNEL19_ADC->DR, memory_address, 0);
    delay_ms(10);
    HAL_ADC_Start_DMA(&ADC1_HAL_Handle, (uint32_t *)memory_address, 0);
}

/**
 * @brief       ADC1 Channel19 PA5 DMA读取启动
 * @param       ndtr:   DMA传输数量
 * @retval      none
 */ 
void ADC1_Channel19_DMA_Start(uint16_t ndtr){
    /*Close adc*/
    ADC1_CHANNEL19_ADC->CR &= ~(1 << 0);
    /*Close dma*/
    ADC1_CHANNEL19_DMA->CR &= ~(1 << 0);
    /*Ensure that dma is configurable*/
    while(ADC1_CHANNEL19_DMA->CR & 0x01);
    /*The number of the data should be sent*/
    ADC1_CHANNEL19_DMA->NDTR = ndtr;
    /*Start dma*/
    ADC1_CHANNEL19_DMA->CR |= (1 << 0);
    /*Restart ADC*/
    ADC1_CHANNEL19_ADC->CR |= (1 << 0);
    /*Start normal convert channel*/
    ADC1_CHANNEL19_ADC->CR |= (1 << 2);
}


/**
 * @brief       ADC1 Channel19 PA5读取
 * @param       none
 * @retval      none
 */ 
uint32_t ADC1_Channel19_Read(void){
    ADC_ChannelConfTypeDef adc_channel_cfg_str;
    adc_channel_cfg_str.Channel = ADC1_CHANNEL19_CHANNEL;
    adc_channel_cfg_str.Rank = ADC_REGULAR_RANK_1;
    adc_channel_cfg_str.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;
    adc_channel_cfg_str.SingleDiff = ADC_SINGLE_ENDED;
    adc_channel_cfg_str.OffsetNumber = ADC_OFFSET_NONE;
    adc_channel_cfg_str.Offset = 0;
    HAL_ADC_ConfigChannel(&ADC1_HAL_Handle, &adc_channel_cfg_str);
    HAL_ADC_Start(&ADC1_HAL_Handle);
    HAL_ADC_PollForConversion(&ADC1_HAL_Handle, 10);
    return HAL_ADC_GetValue(&ADC1_HAL_Handle);
}

/**
 * @brief       ADC1 Channel19 PA5 初始化, 时钟触发转发方式
 * @param       memory_address : 内存地址
 * @param       length         : 单次转换长度
 * @retval      none
 * @note        使用函数ADC1_Channel19_Read()读取ADC值时配置通道
 */
void ADC1_Channel19_TimTri_Init(uint32_t memory_address, uint16_t length)
{
    GPIO_InitTypeDef adc_gpio_init_str;

    /*GPIO init*/
    ADC1_CHANNEL19_GPIO_CLK_ENABLE();
    adc_gpio_init_str.Mode = GPIO_MODE_ANALOG;
    adc_gpio_init_str.Pin = ADC1_CHANNEL19_GPIO_PIN;
    HAL_GPIO_Init(ADC1_CHANNEL19_GPIO_PORT, &adc_gpio_init_str);


    /*ADC init*/
    ADC1_CHANNEL19_ADC_CLK_ENABLE();
    __HAL_RCC_ADC_CONFIG(ADC1_CHANNEL19_SELECT_CLK);
    ADC1_HAL_Handle.Instance=ADC1_CHANNEL19_ADC;
    //Input clock DIV2 :    adc_ker_ck=per_ck/2=32Mhz
    ADC1_HAL_Handle.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV2;
    ADC1_HAL_Handle.Init.ContinuousConvMode = DISABLE;
    ADC1_HAL_Handle.Init.DiscontinuousConvMode = DISABLE;
    ADC1_HAL_Handle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
    ADC1_HAL_Handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    ADC1_HAL_Handle.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T1_CC1;
    ADC1_HAL_Handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    ADC1_HAL_Handle.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
    ADC1_HAL_Handle.Init.LowPowerAutoWait = DISABLE;
    ADC1_HAL_Handle.Init.NbrOfConversion = 1;
    ADC1_HAL_Handle.Init.NbrOfDiscConversion = 0;
    ADC1_HAL_Handle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
    ADC1_HAL_Handle.Init.OversamplingMode= DISABLE;
    ADC1_HAL_Handle.Init.Resolution = ADC_RESOLUTION_16B;
    ADC1_HAL_Handle.Init.ScanConvMode = ADC_SCAN_DISABLE;
    HAL_ADC_Init(&ADC1_HAL_Handle);
    HAL_ADCEx_Calibration_Start(&ADC1_HAL_Handle, ADC_CALIB_OFFSET,
                                ADC_SINGLE_ENDED);    

    /*ADC channel init*/
    /*频道配置需在ADC初始化后进行*/
    ADC_ChannelConfTypeDef adc_channel_cfg_str;
    adc_channel_cfg_str.Channel = ADC1_CHANNEL19_CHANNEL;
    adc_channel_cfg_str.Rank = ADC_REGULAR_RANK_1;
    // adc_channel_cfg_str.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;
    adc_channel_cfg_str.SamplingTime = ADC_SAMPLETIME_8CYCLES_5;
    adc_channel_cfg_str.SingleDiff = ADC_SINGLE_ENDED;
    adc_channel_cfg_str.OffsetNumber = ADC_OFFSET_NONE;
    adc_channel_cfg_str.Offset = 0;
    adc_channel_cfg_str.OffsetRightShift=DISABLE;
    adc_channel_cfg_str.OffsetSignedSaturation=DISABLE;

    HAL_ADC_ConfigChannel(&ADC1_HAL_Handle, &adc_channel_cfg_str);
    
    /*DMA init*/
    ADC1_CHANNEL19_DMA_CLK_ENABLE();
    DMA1_CHANNEL7_HAL_Handle.Instance = ADC1_CHANNEL19_DMA;
    DMA1_CHANNEL7_HAL_Handle.Init.Request = ADC1_CHANNEL19_DMA_REQ;
    DMA1_CHANNEL7_HAL_Handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
    DMA1_CHANNEL7_HAL_Handle.Init.PeriphInc = DMA_PINC_DISABLE;
    DMA1_CHANNEL7_HAL_Handle.Init.MemInc = DMA_MINC_ENABLE;
    DMA1_CHANNEL7_HAL_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    DMA1_CHANNEL7_HAL_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    DMA1_CHANNEL7_HAL_Handle.Init.Mode = DMA_CIRCULAR;
    DMA1_CHANNEL7_HAL_Handle.Init.Priority = DMA_PRIORITY_HIGH;
    DMA1_CHANNEL7_HAL_Handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    
    HAL_DMA_Init(&DMA1_CHANNEL7_HAL_Handle);
    __HAL_LINKDMA(&ADC1_HAL_Handle, DMA_Handle, DMA1_CHANNEL7_HAL_Handle);

    /*DMA interrupt init*/
    HAL_NVIC_SetPriority(ADC1_CHANNEL19_DMA_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(ADC1_CHANNEL19_DMA_IRQn);

    /*Start ADC*/
    HAL_DMA_Start_IT(&DMA1_CHANNEL7_HAL_Handle, (uint32_t)&ADC1_CHANNEL19_ADC->DR, memory_address, length);
    
    HAL_ADC_Start_DMA(&ADC1_HAL_Handle, (uint32_t *)memory_address, length);
}

/******************************************CallBack Functions**************************************/

char ADC1_Channel19_DMA_Finish_Flag = 0;

/**
* @brief ADC DMA 采集中断服务函数
* @param 无
* @retval 无
*/
void DMA1_Stream7_IRQHandler(void)
{
 if (__HAL_DMA_GET_FLAG(&DMA1_CHANNEL7_HAL_Handle, DMA_FLAG_TCIF3_7))
 {
    __HAL_DMA_CLEAR_FLAG(&DMA1_CHANNEL7_HAL_Handle, DMA_FLAG_TCIF3_7|DMA_FLAG_HTIF3_7); /* 清除 DMA1 数据流 7 传输完成中断 */
    ADC1_Channel19_DMA_Finish_Flag = 1;
    /*Close adc*/
    ADC1_CHANNEL19_ADC->CR &= ~(1 << 0);
    /*Close dma*/
    ADC1_CHANNEL19_DMA->CR &= ~(1 << 0);
 }
}


