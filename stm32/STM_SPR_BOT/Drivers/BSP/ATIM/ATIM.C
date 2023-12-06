/**
 ****************************************************************************************************
 * @file     ATIM.C
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

#include "./BSP/ATIM/ATIM.H"

TIM_HandleTypeDef ATIM1_HAL_Handle;
TIM_BreakDeadTimeConfigTypeDef ATIM1_BKIN_init_struct;

uint8_t ATIM1_PWMIN_Channel1_State;
uint16_t ATIM1_PWMIN_Channel1_Psc;
uint32_t ATIM1_PWMIN_Channel1_Cycle;
uint32_t ATIM1_PWMIN_Channel1_High;

/* T = 2*(arr+1)*((psc+1)/ Tclk) */

/**
 * @brief   Initialize the Output Compare of TIM1 in channel 1
 * @param   Prescaler
 * @param   Period
 * @retval  none
 * @note    all init steps in this function
 *          There is no need to call the msp init function
 */
void ATIM1_OC_Channel1_Init(uint32_t Prescaler, uint32_t Period, uint32_t Pulse){
    GPIO_InitTypeDef ATIM1_OC_Channel1_gpio_init_struct;
    TIM_OC_InitTypeDef ATIM1_OC_Channel1_init_struct;
    //TIM1 Handler
    ATIM1_HAL_Handle.Instance = TIM1;
    ATIM1_HAL_Handle.Init.Period = Period;
    ATIM1_HAL_Handle.Init.Prescaler = Prescaler;
    ATIM1_HAL_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    ATIM1_HAL_Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    ATIM1_CLK_Enable();
    HAL_TIM_OC_Init(&ATIM1_HAL_Handle);

    //GPIO init
    ATIM1_OC_Channel1_gpio_init_struct.Alternate = ATIM1_OC_Channel1_AF;
    ATIM1_OC_Channel1_gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    ATIM1_OC_Channel1_gpio_init_struct.Pin = ATIM1_OC_Channel1_Pin;
    ATIM1_OC_Channel1_gpio_init_struct.Pull = GPIO_NOPULL;
    ATIM1_OC_Channel1_gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    ATIM1_OC_Channel1_Pin_CLK_Enable();
    HAL_GPIO_Init(ATIM1_OC_Channel1_Port, &ATIM1_OC_Channel1_gpio_init_struct);

    //TIM1 OC configure
    ATIM1_OC_Channel1_init_struct.OCMode = TIM_OCMODE_TOGGLE;
    ATIM1_OC_Channel1_init_struct.Pulse = Pulse;
    ATIM1_OC_Channel1_init_struct.OCPolarity = TIM_OCPOLARITY_HIGH;
    HAL_TIM_OC_ConfigChannel(&ATIM1_HAL_Handle, &ATIM1_OC_Channel1_init_struct,TIM_CHANNEL_1);

    //Enable CCR1 preload
    __HAL_TIM_ENABLE_OCxPRELOAD(&ATIM1_HAL_Handle, TIM_CHANNEL_1);

    HAL_TIM_OC_Start(&ATIM1_HAL_Handle, TIM_CHANNEL_1);
}

/**
 * @brief   Initialize the Output Compare of TIM1 in channel 2
 * @param   Prescaler
 * @param   Period
 * @retval  none
 * @note    all init steps in this function
 *          There is no need to call the msp init function
 */
void ATIM1_OC_Channel2_Init(uint32_t Prescaler, uint32_t Period, uint32_t Pulse){
    GPIO_InitTypeDef ATIM1_OC_Channel2_gpio_init_struct;
    TIM_OC_InitTypeDef ATIM1_OC_Channel2_init_struct;
    //TIM1 Handler
    ATIM1_HAL_Handle.Instance = TIM1;
    ATIM1_HAL_Handle.Init.Period = Period;
    ATIM1_HAL_Handle.Init.Prescaler = Prescaler;
    ATIM1_HAL_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    ATIM1_HAL_Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    ATIM1_CLK_Enable();
    HAL_TIM_OC_Init(&ATIM1_HAL_Handle);

    //GPIO init
    ATIM1_OC_Channel2_gpio_init_struct.Alternate = ATIM1_OC_Channel2_AF;
    ATIM1_OC_Channel2_gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    ATIM1_OC_Channel2_gpio_init_struct.Pin = ATIM1_OC_Channel2_Pin;
    ATIM1_OC_Channel2_gpio_init_struct.Pull = GPIO_NOPULL;
    ATIM1_OC_Channel2_gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    ATIM1_OC_Channel2_Pin_CLK_Enable();
    HAL_GPIO_Init(ATIM1_OC_Channel2_Port, &ATIM1_OC_Channel2_gpio_init_struct);

    //TIM1 OC configure
    ATIM1_OC_Channel2_init_struct.OCMode = TIM_OCMODE_TOGGLE;
    ATIM1_OC_Channel2_init_struct.Pulse = Pulse;
    ATIM1_OC_Channel2_init_struct.OCPolarity = TIM_OCPOLARITY_HIGH;
    HAL_TIM_OC_ConfigChannel(&ATIM1_HAL_Handle, &ATIM1_OC_Channel2_init_struct,TIM_CHANNEL_2);

    //Enable CCR1 preload
    __HAL_TIM_ENABLE_OCxPRELOAD(&ATIM1_HAL_Handle, TIM_CHANNEL_2);

    HAL_TIM_OC_Start(&ATIM1_HAL_Handle, TIM_CHANNEL_2);
}

/**
 * @brief   Initialize the Output Compare of TIM1 in channel 3
 * @param   Prescaler
 * @param   Period
 * @retval  none
 * @note    all init steps in this function
 *          There is no need to call the msp init function
 */
void ATIM1_OC_Channel3_Init(uint32_t Prescaler, uint32_t Period, uint32_t Pulse){
    GPIO_InitTypeDef ATIM1_OC_Channel3_gpio_init_struct;
    TIM_OC_InitTypeDef ATIM1_OC_Channel3_init_struct;
    //TIM1 Handler
    ATIM1_HAL_Handle.Instance = TIM1;
    ATIM1_HAL_Handle.Init.Period = Period;
    ATIM1_HAL_Handle.Init.Prescaler = Prescaler;
    ATIM1_HAL_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    ATIM1_HAL_Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    ATIM1_CLK_Enable();
    HAL_TIM_OC_Init(&ATIM1_HAL_Handle);

    //GPIO init
    ATIM1_OC_Channel3_gpio_init_struct.Alternate = ATIM1_OC_Channel3_AF;
    ATIM1_OC_Channel3_gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    ATIM1_OC_Channel3_gpio_init_struct.Pin = ATIM1_OC_Channel3_Pin;
    ATIM1_OC_Channel3_gpio_init_struct.Pull = GPIO_NOPULL;
    ATIM1_OC_Channel3_gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    ATIM1_OC_Channel3_Pin_CLK_Enable();
    HAL_GPIO_Init(ATIM1_OC_Channel3_Port, &ATIM1_OC_Channel3_gpio_init_struct);

    //TIM1 OC configure
    ATIM1_OC_Channel3_init_struct.OCMode = TIM_OCMODE_TOGGLE;
    ATIM1_OC_Channel3_init_struct.Pulse = Pulse;
    ATIM1_OC_Channel3_init_struct.OCPolarity = TIM_OCPOLARITY_HIGH;
    HAL_TIM_OC_ConfigChannel(&ATIM1_HAL_Handle, &ATIM1_OC_Channel3_init_struct,TIM_CHANNEL_3);

    //Enable CCR1 preload
    __HAL_TIM_ENABLE_OCxPRELOAD(&ATIM1_HAL_Handle, TIM_CHANNEL_3);

    HAL_TIM_OC_Start(&ATIM1_HAL_Handle, TIM_CHANNEL_3);
}

/**
 * @brief   Initialize the Output Compare of TIM1 in channel 4
 * @param   Prescaler
 * @param   Period
 * @retval  none
 * @note    all init steps in this function
 *          There is no need to call the msp init function
 */
void ATIM1_OC_Channel4_Init(uint32_t Prescaler, uint32_t Period, uint32_t Pulse){
    GPIO_InitTypeDef ATIM1_OC_Channel4_gpio_init_struct;
    TIM_OC_InitTypeDef ATIM1_OC_Channel4_init_struct;
    //TIM1 Handler
    ATIM1_HAL_Handle.Instance = TIM1;
    ATIM1_HAL_Handle.Init.Period = Period;
    ATIM1_HAL_Handle.Init.Prescaler = Prescaler;
    ATIM1_HAL_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    ATIM1_HAL_Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    ATIM1_CLK_Enable();
    HAL_TIM_OC_Init(&ATIM1_HAL_Handle);

    //GPIO init
    ATIM1_OC_Channel4_gpio_init_struct.Alternate = ATIM1_OC_Channel4_AF;
    ATIM1_OC_Channel4_gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    ATIM1_OC_Channel4_gpio_init_struct.Pin = ATIM1_OC_Channel4_Pin;
    ATIM1_OC_Channel4_gpio_init_struct.Pull = GPIO_NOPULL;
    ATIM1_OC_Channel4_gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    ATIM1_OC_Channel4_Pin_CLK_Enable();
    HAL_GPIO_Init(ATIM1_OC_Channel4_Port, &ATIM1_OC_Channel4_gpio_init_struct);

    //TIM1 OC configure
    ATIM1_OC_Channel4_init_struct.OCMode = TIM_OCMODE_TOGGLE;
    ATIM1_OC_Channel4_init_struct.Pulse = Pulse;
    ATIM1_OC_Channel4_init_struct.OCPolarity = TIM_OCPOLARITY_HIGH;
    HAL_TIM_OC_ConfigChannel(&ATIM1_HAL_Handle, &ATIM1_OC_Channel4_init_struct,TIM_CHANNEL_4);

    //Enable CCR1 preload
    __HAL_TIM_ENABLE_OCxPRELOAD(&ATIM1_HAL_Handle, TIM_CHANNEL_4);

    HAL_TIM_OC_Start(&ATIM1_HAL_Handle, TIM_CHANNEL_4);
}

/**
 * @brief   Initialize the PWMN of TIM1 in channel 1
 * @param   Prescaler
 * @param   Period
 * @param   Pulse
 * @retval  none
 * @note    all init steps in this function
 * @note    There is no need to call the msp init function
 */
void ATIM1_PWMN_Channel1_Init(uint32_t Prescaler, uint32_t Period, uint32_t Pulse){
    GPIO_InitTypeDef ATIM1_PWMN_Channel1_gpio_init_struct;
    TIM_OC_InitTypeDef ATIM1_PWMN_Channel1_OC_init_struct;
    //GPIO init
    ATIM1_PWMN_Channel1_gpio_init_struct.Alternate = ATIM1_PWMN_Channel1_AF;
    ATIM1_PWMN_Channel1_gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    ATIM1_PWMN_Channel1_gpio_init_struct.Pin = ATIM1_PWMN_Channel1_Pin;
    ATIM1_PWMN_Channel1_gpio_init_struct.Pull = GPIO_NOPULL;
    ATIM1_PWMN_Channel1_Pin_CLK_Enable();
    HAL_GPIO_Init(ATIM1_PWMN_Channel1_Port,&ATIM1_PWMN_Channel1_gpio_init_struct);
    ATIM1_PWMN_Channel1_gpio_init_struct.Alternate = ATIM1_PWMN_Channel1N_AF;
    ATIM1_PWMN_Channel1_gpio_init_struct.Pin = ATIM1_PWMN_Channel1N_Pin;
    ATIM1_PWMN_Channel1N_Pin_CLK_Enable();
    HAL_GPIO_Init(ATIM1_PWMN_Channel1N_Port,&ATIM1_PWMN_Channel1_gpio_init_struct);
    ATIM1_PWMN_Channel1_gpio_init_struct.Alternate = ATIM1_PWMN_BKIN_AF;
    ATIM1_PWMN_Channel1_gpio_init_struct.Pin = ATIM1_PWMN_BKIN_Pin;
    ATIM1_PWMN_Channel1_gpio_init_struct.Pull = GPIO_PULLUP;
    ATIM1_PWMN_BKIN_Pin_CLK_Enable();
    HAL_GPIO_Init(ATIM1_PWMN_BKIN_Port, &ATIM1_PWMN_Channel1_gpio_init_struct);

    //TIM1 init
    ATIM1_HAL_Handle.Instance = TIM1;
    ATIM1_HAL_Handle.Init.Period = Period;
    ATIM1_HAL_Handle.Init.Prescaler=Prescaler;
    ATIM1_HAL_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    ATIM1_HAL_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
    ATIM1_HAL_Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    ATIM1_CLK_Enable();
    HAL_TIM_PWM_Init(&ATIM1_HAL_Handle);

    //TIM1 OC init
    ATIM1_PWMN_Channel1_OC_init_struct.OCMode = TIM_OCMODE_PWM1;
    ATIM1_PWMN_Channel1_OC_init_struct.Pulse = Pulse;
    ATIM1_PWMN_Channel1_OC_init_struct.OCPolarity = TIM_OCPOLARITY_LOW;
    ATIM1_PWMN_Channel1_OC_init_struct.OCNPolarity = TIM_OCNPOLARITY_LOW;
    ATIM1_PWMN_Channel1_OC_init_struct.OCIdleState = TIM_OCIDLESTATE_SET;
    ATIM1_PWMN_Channel1_OC_init_struct.OCNIdleState= TIM_OCNIDLESTATE_SET;
    HAL_TIM_PWM_ConfigChannel(&ATIM1_HAL_Handle, &ATIM1_PWMN_Channel1_OC_init_struct, TIM_CHANNEL_1);

    //TIM1 BKIN init
    ATIM1_BKIN_init_struct.OffStateRunMode = TIM_OSSR_DISABLE;
    ATIM1_BKIN_init_struct.OffStateIDLEMode = TIM_OSSI_DISABLE;
    ATIM1_BKIN_init_struct.LockLevel = TIM_LOCKLEVEL_OFF;
    ATIM1_BKIN_init_struct.BreakState = TIM_BREAK_ENABLE;
    ATIM1_BKIN_init_struct.BreakPolarity = TIM_BREAKPOLARITY_LOW;
    ATIM1_BKIN_init_struct.BreakFilter = 0;
    ATIM1_BKIN_init_struct.Break2State = TIM_BREAK2_DISABLE;
    ATIM1_BKIN_init_struct.AutomaticOutput = TIM_AUTOMATICOUTPUT_ENABLE;
    HAL_TIMEx_ConfigBreakDeadTime(&ATIM1_HAL_Handle, &ATIM1_BKIN_init_struct);

    HAL_TIM_PWM_Start(&ATIM1_HAL_Handle, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&ATIM1_HAL_Handle, TIM_CHANNEL_1);
}

/**
 * @brief   Initialize the Output Compare of TIM1 in channel 4
 * @param   CCR : Output Comparison value, set PWM period
 * @param   DTG : Dead time, set Dead time
 * @retval  none
 * @note    DTG[7:5]=0xx : Dead time = DTG[7:0] * tDTS
 * @note    DTG[7:5]=10x : Dead time = (64 + DTG[5:0]) * 2 * tDTS
 * @note    DTG[7:5]=110 : Dead time = (32 + DTG[4:0]) * 8 * tDTS
 * @note    DTG[7:5]=111 : Dead time = (32 + DTG[4:0]) * 16 * tDTS
 * @note    tDTS = 2^CKD[1:0] / Tclk  (CKD[1:0] = 2 , tDTS = 16.67ns)
 */
void ATIM1_PWMN_DT_Set(uint16_t CCR,uint8_t DTG){
    ATIM1_BKIN_init_struct.DeadTime = DTG;
    HAL_TIMEx_ConfigBreakDeadTime(&ATIM1_HAL_Handle, &ATIM1_BKIN_init_struct);
    __HAL_TIM_MOE_ENABLE(&ATIM1_HAL_Handle);
    ATIM1_HAL_Handle.Instance->CCR1 = CCR;
}

/**
 * @brief   Initialize the PWMIN of TIM1 in channel 1
 * @param   Prescaler
 * @param   Period
 * @param   Pulse
 * @retval  none
 * @note    all init steps in this function
 * @note    There is no need to call the msp init function
 */
void ATIM1_PWMIN_Channel1_Init(void){
    GPIO_InitTypeDef ATIM1_PWMIN_Channel1_gpio_init_struct;
    TIM_IC_InitTypeDef ATIM1_PWMIN_Channel1_ic_init_struct;
    TIM_SlaveConfigTypeDef ATIM1_PWMIN_Channel1_slaveconfig_init_struct;

    //GPIO init
    ATIM1_PWMIN_Channel1_gpio_init_struct.Alternate = ATIM1_PWMIN_Channel1_AF;
    ATIM1_PWMIN_Channel1_gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    ATIM1_PWMIN_Channel1_gpio_init_struct.Pin = ATIM1_PWMIN_Channel1_Pin;
    ATIM1_PWMIN_Channel1_gpio_init_struct.Pull = GPIO_PULLDOWN;
    ATIM1_PWMIN_Channel1_gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    ATIM1_PWMIN_Channel1_Pin_CLK_Enable();
    HAL_GPIO_Init(ATIM1_PWMIN_Channel1_Port,&ATIM1_PWMIN_Channel1_gpio_init_struct);
    
    //TIM init
    ATIM1_PWMIN_Channel1_Psc = 0;
    ATIM1_HAL_Handle.Instance = TIM1;
    ATIM1_HAL_Handle.Init.Period = 65535;
    ATIM1_HAL_Handle.Init.Prescaler = 0;
    ATIM1_HAL_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    ATIM1_CLK_Enable();
    HAL_TIM_IC_Init(&ATIM1_HAL_Handle);

    //IC init
    ATIM1_PWMIN_Channel1_ic_init_struct.ICFilter = 0;
    ATIM1_PWMIN_Channel1_ic_init_struct.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    ATIM1_PWMIN_Channel1_ic_init_struct.ICPrescaler = TIM_ICPSC_DIV1;
    ATIM1_PWMIN_Channel1_ic_init_struct.ICSelection = TIM_ICSELECTION_DIRECTTI;
    HAL_TIM_IC_ConfigChannel(&ATIM1_HAL_Handle,
                             &ATIM1_PWMIN_Channel1_ic_init_struct,
                             TIM_CHANNEL_1);
    ATIM1_PWMIN_Channel1_ic_init_struct.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
    ATIM1_PWMIN_Channel1_ic_init_struct.ICSelection = TIM_ICSELECTION_INDIRECTTI;
    HAL_TIM_IC_ConfigChannel(&ATIM1_HAL_Handle,&ATIM1_PWMIN_Channel1_ic_init_struct,TIM_CHANNEL_2);

    //Salve Config
    ATIM1_PWMIN_Channel1_slaveconfig_init_struct.InputTrigger = TIM_TS_TI1FP1;
    ATIM1_PWMIN_Channel1_slaveconfig_init_struct.SlaveMode = TIM_SLAVEMODE_RESET;
    ATIM1_PWMIN_Channel1_slaveconfig_init_struct.TriggerFilter = 0;
    ATIM1_PWMIN_Channel1_slaveconfig_init_struct.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
    HAL_TIM_SlaveConfigSynchro(&ATIM1_HAL_Handle,&ATIM1_PWMIN_Channel1_slaveconfig_init_struct);

    //NVIC Config
    HAL_NVIC_SetPriority(TIM1_UP_IRQn, 1, 3);
    HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);
    HAL_NVIC_SetPriority(TIM1_CC_IRQn, 1, 3);   //Only in TIM1 and TIM5
    HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);           //Only in TIM1 and TIM5

    __HAL_TIM_ENABLE_IT(&ATIM1_HAL_Handle, TIM_IT_UPDATE);
    HAL_TIM_IC_Start_IT(&ATIM1_HAL_Handle, TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&ATIM1_HAL_Handle, TIM_CHANNEL_2);
}

/**
 * @brief   TIM1 Update interrupt handler service function
 * @param   none
 * @retval  none
 */
void ATIM1_PWMIN_Channel1_Restart(uint32_t Prescaler){
    sys_intx_disable();
    ATIM1_PWMIN_Channel1_State = 0;
    ATIM1_PWMIN_Channel1_Psc = Prescaler;
    __HAL_TIM_SET_PRESCALER(&ATIM1_HAL_Handle, ATIM1_PWMIN_Channel1_Psc);
    __HAL_TIM_SET_COUNTER(&ATIM1_HAL_Handle, 0);

    __HAL_TIM_ENABLE_IT(&ATIM1_HAL_Handle, TIM_IT_UPDATE);
    __HAL_TIM_ENABLE_IT(&ATIM1_HAL_Handle, TIM_IT_CC1);
    __HAL_TIM_ENABLE(&ATIM1_HAL_Handle);

    ATIM1_HAL_Handle.Instance->SR = 0;

    sys_intx_enable();
}

/**
 * @brief   TIM1 Update interrupt handler service function
 * @param   none
 * @retval  none
 */
void TIM1_PWMIN_INT_Service(void){
    static uint8_t ATIM1_PWMIN_Channel1_Run = 0;
    if(ATIM1_PWMIN_Channel1_State){
        ATIM1_PWMIN_Channel1_Psc = 0;
        ATIM1_HAL_Handle.Instance->SR = 0;
        __HAL_TIM_SET_COUNTER(&ATIM1_HAL_Handle,0);
        return;
    }
    if(__HAL_TIM_GET_FLAG(&ATIM1_HAL_Handle,TIM_IT_UPDATE)!=RESET){
        __HAL_TIM_CLEAR_FLAG(&ATIM1_HAL_Handle, TIM_FLAG_UPDATE);
        if(__HAL_TIM_GET_FLAG(&ATIM1_HAL_Handle,TIM_IT_CC1)==RESET){
            ATIM1_PWMIN_Channel1_Run = 0;
            if(ATIM1_PWMIN_Channel1_Psc==0){
                ATIM1_PWMIN_Channel1_Psc++;
            }else if(ATIM1_PWMIN_Channel1_Psc==65535){
                ATIM1_PWMIN_Channel1_Psc = 0;
            }else if(ATIM1_PWMIN_Channel1_Psc>32767){
                ATIM1_PWMIN_Channel1_Psc = 65535;
            }else{
                ATIM1_PWMIN_Channel1_Psc += ATIM1_PWMIN_Channel1_Psc;
            }
            __HAL_TIM_SET_PRESCALER(&ATIM1_HAL_Handle, ATIM1_PWMIN_Channel1_Psc);
            __HAL_TIM_SET_COUNTER(&ATIM1_HAL_Handle, 0);
            ATIM1_HAL_Handle.Instance->SR = 0;
            return;
        }
    }
    if(ATIM1_PWMIN_Channel1_Run==0){
        if(__HAL_TIM_GET_FLAG(&ATIM1_HAL_Handle,TIM_IT_CC1)!=RESET){
            ATIM1_PWMIN_Channel1_Run = 1;
        }
        ATIM1_HAL_Handle.Instance->SR = 0;
        return;
    }
    if(ATIM1_PWMIN_Channel1_State == 0){
        if(__HAL_TIM_GET_FLAG(&ATIM1_HAL_Handle,TIM_IT_CC1)!=RESET) {
            ATIM1_PWMIN_Channel1_High =HAL_TIM_ReadCapturedValue(&ATIM1_HAL_Handle, TIM_CHANNEL_2)+1;
            ATIM1_PWMIN_Channel1_Cycle =HAL_TIM_ReadCapturedValue(&ATIM1_HAL_Handle, TIM_CHANNEL_1)+1;
            if(ATIM1_PWMIN_Channel1_High<ATIM1_PWMIN_Channel1_Cycle){
                ATIM1_PWMIN_Channel1_State = 1;
                ATIM1_PWMIN_Channel1_Psc = ATIM1_HAL_Handle.Instance->PSC;
                if(ATIM1_PWMIN_Channel1_Psc==0){
                    ATIM1_PWMIN_Channel1_High++;
                    ATIM1_PWMIN_Channel1_Cycle++;
                }
                ATIM1_PWMIN_Channel1_Run = 0;
                ATIM1_HAL_Handle.Instance->CR1 &= ~(1 << 0);
                __HAL_TIM_DISABLE_IT(&ATIM1_HAL_Handle, TIM_IT_UPDATE);
                __HAL_TIM_DISABLE_IT(&ATIM1_HAL_Handle, TIM_IT_CC1);
                __HAL_TIM_DISABLE_IT(&ATIM1_HAL_Handle, TIM_IT_CC2);
            }else{
                ATIM1_PWMIN_Channel1_Restart(0);
            }
        }
		ATIM1_HAL_Handle.Instance->SR = 0;
    }
    ATIM1_HAL_Handle.Instance->SR = 0;
}

/**
 * @brief   TIM1 Update interrupt handler service function
 * @param   none
 * @retval  none
 */
void TIM1_UP_IRQHandler(void){
    TIM1_PWMIN_INT_Service();
}

/**
 * @brief   TIM1 Capture compare interrupt handler service function
 * @param   none
 * @retval  none
 */
void TIM1_CC_IRQHandler(void){
    TIM1_PWMIN_INT_Service();
}



