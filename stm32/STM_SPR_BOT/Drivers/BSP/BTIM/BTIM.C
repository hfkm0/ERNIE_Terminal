/**
 ****************************************************************************************************
 * @file     BTIM.C
 * @author   HFKM
 * @version  V1.0
 * @date     2023-02-25
 * @brief    BTIM-HAL库版本 实验
 * @license  Copyright (c) 2020-2032, HFKM
 ****************************************************************************************************
 * @attention
 * 
 * 实验平台:正点原子 Mini Pro H750开发板
 * 
 ****************************************************************************************************
 */

#include "./BSP/BTIM/BTIM.H"
#include "./BSP/DAC/DAC.H"

TIM_HandleTypeDef BTIM6_HAL_Handle;
uint32_t BTIM6_Int_Count = 0;

/**
 * @brief   Initialize the Base Tim6
 * @param   Prescaler: if Prescaler is 1,CK_INT x= 2
 *                                  else,CK_INT x= 1
 * @param   Period: Reload value.If counter' value equals it,reset.    
 * @retval  none
 * @note    Tout=(Prescaler+1)*(Period+1)/Tclk
 *          500ms: Prescaler=23999
 *                 Period=4999
 */
void BTIM6_Init(uint32_t Prescaler, uint32_t Period){
    BTIM6_HAL_Handle.Instance = TIM6;
    BTIM6_HAL_Handle.Init.Prescaler = Prescaler;;
    BTIM6_HAL_Handle.Init.Period = Period;
    BTIM6_HAL_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_Base_Init(&BTIM6_HAL_Handle);
}

/**
 * @brief   MSP initiales of the Base Tim
 * @param   htim: Tim handle
 * @retval  none
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim){
    if(htim->Instance == TIM6){
        __HAL_RCC_TIM6_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 3, 3);
        HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
    }
}

extern uint16_t Voice_Value[];
extern uint32_t download_data_length;
uint32_t i = 0;
uint16_t value = 0;
uint8_t DAC_Done_flag = 0;

/**
 * @brief   TIM6 interrupt service function
 * @param   none
 * @retval  none
 */
void TIM6_DAC_IRQHandler(void) {
    BTIM6_HAL_Handle.Instance = TIM6;
    if(__HAL_TIM_GET_FLAG(&BTIM6_HAL_Handle, TIM_FLAG_UPDATE) != RESET) {
        if(__HAL_TIM_GET_IT_SOURCE(&BTIM6_HAL_Handle, TIM_IT_UPDATE) !=RESET) {
            __HAL_TIM_CLEAR_IT(&BTIM6_HAL_Handle, TIM_IT_UPDATE);
        }
        value = (*(Voice_Value + i++)) / 16;
        HAL_DAC_SetValue(&DAC12_HAL_Handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, value);    

        if (i == download_data_length) {
            HAL_TIM_Base_Stop(&BTIM6_HAL_Handle);
            HAL_TIM_Base_Stop_IT(&BTIM6_HAL_Handle);
            HAL_DAC_SetValue(&DAC12_HAL_Handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0);
            i = 0;
            DAC_Done_flag = 1;
        }
    }
}

/**
 * @brief   MSP initiales of the Base Tim
 * @param   htim: Tim handle
 * @retval  none
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if(htim->Instance == TIM6){
    }
}



