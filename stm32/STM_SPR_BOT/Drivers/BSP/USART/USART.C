/**
 ****************************************************************************************************
 * @file     USART.C
 * @author   HFKM
 * @version  V1.0
 * @date     2023-02-20
 * @license  Copyright (c) 2020-2032, HFKM
 ****************************************************************************************************
 * @attention        
 ****************************************************************************************************
 */

#include "./BSP/USART/USART.H"
#include "string.h"
#include "stdlib.h"

#if USART_USE_COMMAND == 1

#include "./BSP/COMMAND/Command.H"

#endif

/***************************************************************************************************/
/* 加入以下代码, 支持printf函数, 而不需要选择use MicroLIB */
#if 1
#if (__ARMCC_VERSION >= 6010050)           /* 使用AC6编译器时 */
__asm(".global __use_no_semihosting\n\t"); /* 声明不使用半主机模式 */
__asm(".global __ARM_use_no_argv \n\t"); /* AC6下需要声明main函数为无参数格式，否则部分例程可能出现半主机模式
                                          */

#else
/* 使用AC5编译器时, 要在这里定义__FILE 和 不使用半主机模式 */
#pragma import(__use_no_semihosting)

struct __FILE {
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

#endif

/* 不使用半主机模式，至少需要重定义_ttywrch\_sys_exit\_sys_command_string函数,以同时兼容AC6和AC5模式
 */
int _ttywrch(int ch) {
    ch = ch;
    return ch;
}

/* 定义_sys_exit()以避免使用半主机模式 */
void _sys_exit(int x) {
    x = x;
}

char* _sys_command_string(char* cmd, int len) {
    return NULL;
}

/* FILE 在 stdio.h里面定义. */
FILE __stdout;

/* 重定义fputc函数, printf函数最终会通过调用fputc输出字符串到串口 */
int fputc(int ch, FILE* f) {
    while ((USART_MAIN->ISR & 0X40) == 0)
        ; /* 等待上一个字符发送完成 */

    USART_MAIN->TDR = (uint8_t)ch; /* 将要发送的字符 ch 写入到DR寄存器 */
    return ch;
}
#endif
/***************************************************************************************************/

/******************************************USART1***************************************************/

UART_HandleTypeDef USART1_HAL_Handler;

/*Recieve struct*/
uint8_t USART1_RX_Buffer[USART1_RX_SIZE];
uint8_t USART1_RX_DATA[USART1_RX_DATA_SIZE];
uint16_t USART1_RX_DATA_State=0;
DMA_HandleTypeDef USART1_RX_DMA_HAL_Handler;

UART_RX_DATA_STRUCT USART1_RX_DATA_Struct={
    .huart=&USART1_HAL_Handler,
    .hdma=&USART1_RX_DMA_HAL_Handler,
    .pBuffer=USART1_RX_Buffer,
    .pDatas=USART1_RX_DATA,
    .BufferSize=USART1_RX_SIZE,
    .DataSize=USART1_RX_DATA_SIZE,
    .State=0,
};

/*Transmit struct*/
uint8_t USART1_TX_DATA[USART1_TX_DATA_SIZE];
DMA_HandleTypeDef USART1_TX_DMA_HAL_Handler;

UART_TX_DATA_STRUCT USART1_TX_DATA_Struct={
    .huart=&USART1_HAL_Handler,
    .hdma=&USART1_TX_DMA_HAL_Handler,
    .pDatas=USART1_TX_DATA,
    .DataSize=USART1_TX_DATA_SIZE,
    .State=0,
};

/**
 * @brief   Initialize the USART1 Configuration
 * @param   BaudRate :eg : 115200
 * @retval  none
 */
void USART1_Init(uint32_t BaudRate) {
    USART1_HAL_Handler.Instance = USART1;
    USART1_HAL_Handler.Init.BaudRate = BaudRate;
    USART1_HAL_Handler.Init.Mode = UART_MODE_TX_RX;
    USART1_HAL_Handler.Init.WordLength = UART_WORDLENGTH_8B;
    USART1_HAL_Handler.Init.StopBits = UART_STOPBITS_1;
    USART1_HAL_Handler.Init.Parity = UART_PARITY_NONE;
    USART1_HAL_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    HAL_UART_Init(&USART1_HAL_Handler);
    HAL_UART_Receive_IT(&USART1_HAL_Handler, (uint8_t*)USART1_RX_Buffer,
                        USART1_RX_SIZE);
}

/**
 * @brief   Rewrite the function that initialize the USART GPIO Configuration
 * @param   *huart: UART Handle
 * @retval  none
 * @note    This function will be called by HAL_UART_Init()
 */
void HAL_UART_MspInit(UART_HandleTypeDef* huart) {
    GPIO_InitTypeDef Gpio_init_struct;
    if (huart->Instance == USART1) {
        USART1_CLK_ENABLE();
        USART1_TX_GPIO_CLK_ENABLE();
        USART1_RX_GPIO_CLK_ENABLE();
        Gpio_init_struct.Pin = USART1_TX_PIN;
        Gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        Gpio_init_struct.Pull = GPIO_PULLUP;
        Gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        Gpio_init_struct.Alternate = USART1_TX_AF;
        HAL_GPIO_Init(USART1_TX_PORT, &Gpio_init_struct);
        Gpio_init_struct.Pin = USART1_RX_PIN;
        Gpio_init_struct.Alternate = USART1_RX_AF;
        HAL_GPIO_Init(USART1_RX_PORT, &Gpio_init_struct);
#if USART1_RX_DATA_IT_ENABLE
        HAL_NVIC_EnableIRQ(USART1_IRQn);
        HAL_NVIC_SetPriority(USART1_IRQn, 1, 1);
#endif
    }
}

/**
 * @brief   USART1 使用DMA发送
 * @param   pDatas_align : DMA_PeripheralDataSize_Byte
 * @param   mdata_align : DMA_MemoryDataSize_Byte
 * @param   EN_INT :  1 ENABLE or 0 DISABLE the DMA interrupt
 * @retval  none
 * @note    This function must be called before USART1_Init()
 *          可使用HAL_DMA_Start()或HAL_DMA_Start_IT()启动DMA传输
 */
void USART1_DMA_Transmit_Init(uint32_t pDatas_align, uint32_t mdata_align,uint16_t EN_INT) {
    USART1_TX_DMA_CLK_ENABLE();

    USART1_TX_DATA_Struct.hdma->Instance=USART1_TX_DMA;
    USART1_TX_DATA_Struct.hdma->Init.Request=USART1_TX_DMA_REQ;
    USART1_TX_DATA_Struct.hdma->Init.Direction=DMA_MEMORY_TO_PERIPH;
    USART1_TX_DATA_Struct.hdma->Init.PeriphInc=DMA_PINC_DISABLE;
    USART1_TX_DATA_Struct.hdma->Init.MemInc=DMA_MINC_ENABLE;
    switch(pDatas_align){
        case 8 :{USART1_TX_DATA_Struct.hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;     break;}
        case 16:{USART1_TX_DATA_Struct.hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; break;}
        case 32:{USART1_TX_DATA_Struct.hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;     break;}
    }
    switch(mdata_align){
        case 8 :{USART1_TX_DATA_Struct.hdma->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;     break;}
        case 16:{USART1_TX_DATA_Struct.hdma->Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD; break;}
        case 32:{USART1_TX_DATA_Struct.hdma->Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;     break;}
    }
    USART1_TX_DATA_Struct.hdma->Init.Mode=DMA_NORMAL;
    USART1_TX_DATA_Struct.hdma->Init.Priority=DMA_PRIORITY_HIGH;
    USART1_TX_DATA_Struct.hdma->Init.FIFOMode=DMA_FIFOMODE_DISABLE;
    USART1_TX_DATA_Struct.hdma->Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;
    USART1_TX_DATA_Struct.hdma->Init.MemBurst=DMA_MBURST_SINGLE;
    USART1_TX_DATA_Struct.hdma->Init.PeriphBurst=DMA_PBURST_SINGLE;

    HAL_DMA_Init(USART1_TX_DATA_Struct.hdma);
    __HAL_LINKDMA(USART1_TX_DATA_Struct.huart,hdmatx,*USART1_TX_DATA_Struct.hdma);
    if(EN_INT){
        HAL_NVIC_SetPriority(USART1_TX_DMA_IRQn,1,1);
        HAL_NVIC_EnableIRQ(USART1_TX_DMA_IRQn);
        // __HAL_DMA_ENABLE_IT(USART1_TX_DATA_Struct.hdma,DMA_IT_HT);
        __HAL_DMA_ENABLE_IT(USART1_TX_DATA_Struct.hdma,DMA_IT_TC);
    }
}

/**
 * @brief   USART1 使用DMA接收
 * @param   pDatas_align : DMA_PeripheralDataSize_Byte
 * @param   mdata_align : DMA_MemoryDataSize_Byte
 * @param   EN_INT :  1 ENABLE or 0 DISABLE the DMA interrupt
 * @retval  none
 * @note    This function must be called before USART1_Init()
 *          可使用HAL_DMA_Start()或HAL_DMA_Start_IT()启动DMA传输
 */
void USART1_DMA_Receive_Init(uint32_t pDatas_align, uint32_t mdata_align,uint16_t EN_INT) {
    USART1_RX_DMA_CLK_ENABLE();

    USART1_RX_DATA_Struct.hdma->Instance=USART1_RX_DMA;
    USART1_RX_DATA_Struct.hdma->Init.Request=USART1_RX_DMA_REQ;
    USART1_RX_DATA_Struct.hdma->Init.Direction=DMA_PERIPH_TO_MEMORY;
    USART1_RX_DATA_Struct.hdma->Init.PeriphInc=DMA_PINC_DISABLE;
    USART1_RX_DATA_Struct.hdma->Init.MemInc=DMA_MINC_ENABLE;
    switch(pDatas_align){
        case 8 :{USART1_RX_DATA_Struct.hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;     break;}
        case 16:{USART1_RX_DATA_Struct.hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; break;}
        case 32:{USART1_RX_DATA_Struct.hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;     break;}
    }
    switch(mdata_align){
        case 8 :{USART1_RX_DATA_Struct.hdma->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;     break;}
        case 16:{USART1_RX_DATA_Struct.hdma->Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD; break;}
        case 32:{USART1_RX_DATA_Struct.hdma->Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;     break;}
    }
    USART1_RX_DATA_Struct.hdma->Init.Mode=DMA_NORMAL;
    USART1_RX_DATA_Struct.hdma->Init.Priority=DMA_PRIORITY_HIGH;
    USART1_RX_DATA_Struct.hdma->Init.FIFOMode=DMA_FIFOMODE_DISABLE;
    USART1_RX_DATA_Struct.hdma->Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;
    USART1_RX_DATA_Struct.hdma->Init.MemBurst=DMA_MBURST_SINGLE;
    USART1_RX_DATA_Struct.hdma->Init.PeriphBurst=DMA_PBURST_SINGLE;

    HAL_DMA_Init(USART1_RX_DATA_Struct.hdma);
    __HAL_LINKDMA(USART1_RX_DATA_Struct.huart,hdmarx,*USART1_RX_DATA_Struct.hdma);
    if(EN_INT){
        HAL_NVIC_SetPriority(USART1_RX_DMA_IRQn,1,1);
        HAL_NVIC_EnableIRQ(USART1_RX_DMA_IRQn);
        // __HAL_DMA_ENABLE_IT(USART1_RX_DATA_Struct.hdma,DMA_IT_HT);
        __HAL_DMA_ENABLE_IT(USART1_RX_DATA_Struct.hdma,DMA_IT_TC);
    }
}


/**
 * @brief   USART1 Transmit dma start
 * @param   ndtr: the number of data to be transmitted
 * @retval  none
 */
void USART1_DMA_Transmit_Start(uint16_t ndtr, uint8_t* pDatas) {
    HAL_UART_Transmit_DMA(&USART1_HAL_Handler, pDatas, ndtr);
}

/**
 * @brief   USART1 receive dma start
 * @param   ndtr: the number of data to be received
 * @retval  none
 */
void USART1_DMA_Receive_Start(uint16_t ndtr, uint8_t* pDatas) {
    HAL_UART_Receive_DMA(&USART1_HAL_Handler, pDatas, ndtr);
}

/**
 * @brief   USART1 Rx Callback Function
 * @param   none
 * @retval  The length of the recieve data
 * @note    The Recieved RX_DATA should be end with a return (0x0d) and a newline
 * (0x0a) return(0x0d) then newline(0x0a)
 */
uint32_t USART1_Test_Demo(void) {
    if (USART1_RX_DATA_IF_OVER) {
        uint32_t length = USART1_RX_DATA_REC_LEN;
        printf("The message you send is:\r\n");
        HAL_UART_Transmit(&USART1_HAL_Handler,(uint8_t*) USART1_RX_DATA,
                          length,1000);
        printf("\r\n\r\n");
        USART1_RX_DATA_State = 0;
        return length;
    }
    return 0;
}


/******************************************USART2***************************************************/

UART_HandleTypeDef USART2_HAL_Handler;
uint8_t USART2_RX_Buffer[USART2_RX_SIZE];
uint8_t USART2_RX_DATA[USART2_RX_DATA_SIZE];
DMA_HandleTypeDef USART2_RX_DMA_HAL_Handler;

UART_RX_DATA_STRUCT USART2_RX_DATA_Struct={
    .huart=&USART2_HAL_Handler,
    .hdma=&USART2_RX_DMA_HAL_Handler,
    .pBuffer=USART2_RX_Buffer,
    .pDatas=USART2_RX_DATA,
    .BufferSize=1,
    .DataSize=256,
    .State=0,
};

/*Transmit struct*/
uint8_t USART2_TX_DATA[USART2_TX_DATA_SIZE];
DMA_HandleTypeDef USART2_TX_DMA_HAL_Handler;

UART_TX_DATA_STRUCT USART2_TX_DATA_Struct={
    .huart=&USART2_HAL_Handler,
    .hdma=&USART2_TX_DMA_HAL_Handler,
    .pDatas=USART2_TX_DATA,
    .DataSize=USART2_TX_DATA_SIZE,
    .State=0,
};

/**
 * @brief   Initialize the USART2 Configuration
 * @param   BaudRate :eg : 115200
 * @retval  none
 */
void USART2_Init(uint32_t BaudRate) {
    GPIO_InitTypeDef Gpio_init_struct;

    /*GPIO init*/
    USART2_CLK_ENABLE();
    USART2_TX_GPIO_CLK_ENABLE();
    USART2_RX_GPIO_CLK_ENABLE();
    Gpio_init_struct.Pin = USART2_TX_PIN;
    Gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    Gpio_init_struct.Pull = GPIO_PULLUP;
    Gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    Gpio_init_struct.Alternate = USART2_TX_AF;
    HAL_GPIO_Init(USART2_TX_PORT, &Gpio_init_struct);
    Gpio_init_struct.Pin = USART2_RX_PIN;
    Gpio_init_struct.Alternate = USART2_RX_AF;
    HAL_GPIO_Init(USART2_RX_PORT, &Gpio_init_struct);
#if USART2_RX_DATA_IT_ENABLE
    HAL_NVIC_EnableIRQ(USART2_IRQn);
    HAL_NVIC_SetPriority(USART2_IRQn, 1, 1);
#endif

    /*USART init*/
    USART2_HAL_Handler.Instance = USART2;
    USART2_HAL_Handler.Init.BaudRate = BaudRate;
    USART2_HAL_Handler.Init.Mode = UART_MODE_TX_RX;
    USART2_HAL_Handler.Init.WordLength = UART_WORDLENGTH_8B;
    USART2_HAL_Handler.Init.StopBits = UART_STOPBITS_1;
    USART2_HAL_Handler.Init.Parity = UART_PARITY_NONE;
    USART2_HAL_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    HAL_UART_Init(&USART2_HAL_Handler);
    HAL_UART_Receive_IT(&USART2_HAL_Handler, (uint8_t*)USART2_RX_Buffer,
                        USART2_RX_SIZE);
}


/**
 * @brief   USART2 使用DMA发送
 * @param   pDatas_align : DMA_PeripheralDataSize_Byte
 * @param   mdata_align : DMA_MemoryDataSize_Byte
 * @param   EN_INT :  1 ENABLE or 0 DISABLE the DMA interrupt
 * @retval  none
 * @note    This function must be called before USART2_Init()
 *          可使用HAL_DMA_Start()或HAL_DMA_Start_IT()启动DMA传输
 */
void USART2_DMA_Transmit_Init(uint32_t pDatas_align, uint32_t mdata_align,uint16_t EN_INT) {
    USART2_TX_DMA_CLK_ENABLE();

    USART2_TX_DATA_Struct.hdma->Instance=USART2_TX_DMA;
    USART2_TX_DATA_Struct.hdma->Init.Request=USART2_TX_DMA_REQ;
    USART2_TX_DATA_Struct.hdma->Init.Direction=DMA_MEMORY_TO_PERIPH;
    USART2_TX_DATA_Struct.hdma->Init.PeriphInc=DMA_PINC_DISABLE;
    USART2_TX_DATA_Struct.hdma->Init.MemInc=DMA_MINC_ENABLE;
    switch(pDatas_align){
        case 8 :{USART2_TX_DATA_Struct.hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;     break;}
        case 16:{USART2_TX_DATA_Struct.hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; break;}
        case 32:{USART2_TX_DATA_Struct.hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;     break;}
    }
    switch(mdata_align){
        case 8 :{USART2_TX_DATA_Struct.hdma->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;     break;}
        case 16:{USART2_TX_DATA_Struct.hdma->Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD; break;}
        case 32:{USART2_TX_DATA_Struct.hdma->Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;     break;}
    }
    USART2_TX_DATA_Struct.hdma->Init.Mode=DMA_NORMAL;
    USART2_TX_DATA_Struct.hdma->Init.Priority=DMA_PRIORITY_HIGH;
    USART2_TX_DATA_Struct.hdma->Init.FIFOMode=DMA_FIFOMODE_DISABLE;
    USART2_TX_DATA_Struct.hdma->Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;
    USART2_TX_DATA_Struct.hdma->Init.MemBurst=DMA_MBURST_SINGLE;
    USART2_TX_DATA_Struct.hdma->Init.PeriphBurst=DMA_PBURST_SINGLE;

    HAL_DMA_Init(USART2_TX_DATA_Struct.hdma);
    __HAL_LINKDMA(USART2_TX_DATA_Struct.huart,hdmatx,*USART2_TX_DATA_Struct.hdma);
    if(EN_INT){
        HAL_NVIC_SetPriority(USART2_TX_DMA_IRQn,1,1);
        HAL_NVIC_EnableIRQ(USART2_TX_DMA_IRQn);
        // __HAL_DMA_ENABLE_IT(USART2_TX_DATA_Struct.hdma,DMA_IT_HT);
        __HAL_DMA_ENABLE_IT(USART2_TX_DATA_Struct.hdma,DMA_IT_TC);
    }
}

/**
 * @brief   USART2 使用DMA接收
 * @param   pDatas_align : DMA_PeripheralDataSize_Byte
 * @param   mdata_align : DMA_MemoryDataSize_Byte
 * @param   EN_INT :  1 ENABLE or 0 DISABLE the DMA interrupt
 * @retval  none
 * @note    This function must be called before USART2_Init()
 *          可使用HAL_DMA_Start()或HAL_DMA_Start_IT()启动DMA传输
 */
void USART2_DMA_Receive_Init(uint32_t pDatas_align, uint32_t mdata_align,uint16_t EN_INT) {
    USART2_TX_DMA_CLK_ENABLE();

    USART2_RX_DATA_Struct.hdma->Instance=USART2_RX_DMA;
    USART2_RX_DATA_Struct.hdma->Init.Request=USART2_RX_DMA_REQ;
    USART2_RX_DATA_Struct.hdma->Init.Direction=DMA_PERIPH_TO_MEMORY;
    USART2_RX_DATA_Struct.hdma->Init.PeriphInc=DMA_PINC_DISABLE;
    USART2_RX_DATA_Struct.hdma->Init.MemInc=DMA_MINC_ENABLE;
    switch(pDatas_align){
        case 8 :{USART2_RX_DATA_Struct.hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;     break;}
        case 16:{USART2_RX_DATA_Struct.hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; break;}
        case 32:{USART2_RX_DATA_Struct.hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;     break;}
    }
    switch(mdata_align){
        case 8 :{USART2_RX_DATA_Struct.hdma->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;     break;}
        case 16:{USART2_RX_DATA_Struct.hdma->Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD; break;}
        case 32:{USART2_RX_DATA_Struct.hdma->Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;     break;}
    }
    USART2_RX_DATA_Struct.hdma->Init.Mode=DMA_NORMAL;
    USART2_RX_DATA_Struct.hdma->Init.Priority=DMA_PRIORITY_HIGH;
    USART2_RX_DATA_Struct.hdma->Init.FIFOMode=DMA_FIFOMODE_DISABLE;
    USART2_RX_DATA_Struct.hdma->Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;
    USART2_RX_DATA_Struct.hdma->Init.MemBurst=DMA_MBURST_SINGLE;
    USART2_RX_DATA_Struct.hdma->Init.PeriphBurst=DMA_PBURST_SINGLE;

    HAL_DMA_Init(USART2_RX_DATA_Struct.hdma);
    __HAL_LINKDMA(USART2_RX_DATA_Struct.huart,hdmarx,*USART2_RX_DATA_Struct.hdma);
    if(EN_INT){
        HAL_NVIC_SetPriority(USART2_RX_DMA_IRQn,1,1);
        HAL_NVIC_EnableIRQ(USART2_RX_DMA_IRQn);
        // __HAL_DMA_ENABLE_IT(USART2_RX_DATA_Struct.hdma,DMA_IT_HT);
        __HAL_DMA_ENABLE_IT(USART2_RX_DATA_Struct.hdma,DMA_IT_TC);
    }
}


/**
 * @brief   USART2 Transmit dma start
 * @param   ndtr: the number of data to be transmitted
 * @retval  none
 */
void USART2_DMA_Transmit_Start(uint16_t ndtr, uint8_t* pDatas) {
    HAL_UART_Transmit_DMA(&USART2_HAL_Handler, pDatas, ndtr);
}

/**
 * @brief   USART2 receive dma start
 * @param   ndtr: the number of data to be received
 * @retval  none
 */
void USART2_DMA_Receive_Start(uint16_t ndtr, uint8_t* pDatas) {
    HAL_UART_Receive_DMA(&USART2_HAL_Handler, pDatas, ndtr);
}

/******************************************USART6***************************************************/

UART_HandleTypeDef USART6_HAL_Handler;
uint8_t USART6_RX_Buffer[USART6_RX_SIZE];
uint8_t USART6_RX_DATA[USART6_RX_DATA_SIZE];
DMA_HandleTypeDef USART6_RX_DMA_HAL_Handler;

UART_RX_DATA_STRUCT USART6_RX_DATA_Struct={
    .huart=&USART6_HAL_Handler,
    .hdma=&USART6_RX_DMA_HAL_Handler,
    .pBuffer=USART6_RX_Buffer,
    .pDatas=USART6_RX_DATA,
    .BufferSize=1,
    .DataSize=256,
    .State=0,
};

/*Transmit struct*/
uint8_t USART6_TX_DATA[USART6_TX_DATA_SIZE];
DMA_HandleTypeDef USART6_TX_DMA_HAL_Handler;

UART_TX_DATA_STRUCT USART6_TX_DATA_Struct={
    .huart=&USART6_HAL_Handler,
    .hdma=&USART6_TX_DMA_HAL_Handler,
    .pDatas=USART6_TX_DATA,
    .DataSize=USART6_TX_DATA_SIZE,
    .State=0,
};

/**
 * @brief   Initialize the USART6 Configuration
 * @param   BaudRate :eg : 115200
 * @param   it : 1 ENABLE or 0 DISABLE the USART interrupt
 * @retval  none
 */
void USART6_Init(uint32_t BaudRate, uint8_t it) {
    GPIO_InitTypeDef Gpio_init_struct;

    /*GPIO init*/
    USART6_CLK_ENABLE();
    USART6_TX_GPIO_CLK_ENABLE();
    USART6_RX_GPIO_CLK_ENABLE();
    Gpio_init_struct.Pin = USART6_TX_PIN;
    Gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    Gpio_init_struct.Pull = GPIO_PULLUP;
    Gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    Gpio_init_struct.Alternate = USART6_TX_AF;
    HAL_GPIO_Init(USART6_TX_PORT, &Gpio_init_struct);
    Gpio_init_struct.Pin = USART6_RX_PIN;
    Gpio_init_struct.Alternate = USART6_RX_AF;
    HAL_GPIO_Init(USART6_RX_PORT, &Gpio_init_struct);
    if(it){
        HAL_NVIC_EnableIRQ(USART6_IRQn);
        HAL_NVIC_SetPriority(USART6_IRQn, 1, 1);
    }

    /*USART init*/
    USART6_HAL_Handler.Instance = USART6;
    USART6_HAL_Handler.Init.BaudRate = BaudRate;
    USART6_HAL_Handler.Init.Mode = UART_MODE_TX_RX;
    USART6_HAL_Handler.Init.WordLength = UART_WORDLENGTH_8B;
    USART6_HAL_Handler.Init.StopBits = UART_STOPBITS_1;
    USART6_HAL_Handler.Init.Parity = UART_PARITY_NONE;
    USART6_HAL_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    HAL_UART_Init(&USART6_HAL_Handler);
    if(it){
        HAL_UART_Receive_IT(&USART6_HAL_Handler, (uint8_t*)USART6_RX_Buffer,
                        USART6_RX_SIZE);
    }
}


/**
 * @brief   USART6 使用DMA发送
 * @param   pDatas_align : DMA_PeripheralDataSize_Byte
 * @param   mdata_align : DMA_MemoryDataSize_Byte
 * @param   EN_INT :  1 ENABLE or 0 DISABLE the DMA interrupt
 * @retval  none
 * @note    This function must be called before USART6_Init()
 *          可使用HAL_DMA_Start()或HAL_DMA_Start_IT()启动DMA传输
 */
void USART6_DMA_Transmit_Init(uint32_t pDatas_align, uint32_t mdata_align,uint16_t EN_INT) {
    USART6_TX_DMA_CLK_ENABLE();

    USART6_TX_DATA_Struct.hdma->Instance=USART6_TX_DMA;
    USART6_TX_DATA_Struct.hdma->Init.Request=USART6_TX_DMA_REQ;
    USART6_TX_DATA_Struct.hdma->Init.Direction=DMA_MEMORY_TO_PERIPH;
    USART6_TX_DATA_Struct.hdma->Init.PeriphInc=DMA_PINC_DISABLE;
    USART6_TX_DATA_Struct.hdma->Init.MemInc=DMA_MINC_ENABLE;
    switch(pDatas_align){
        case 8 :{USART6_TX_DATA_Struct.hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;     break;}
        case 16:{USART6_TX_DATA_Struct.hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; break;}
        case 36:{USART6_TX_DATA_Struct.hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;     break;}
    }
    switch(mdata_align){
        case 8 :{USART6_TX_DATA_Struct.hdma->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;     break;}
        case 16:{USART6_TX_DATA_Struct.hdma->Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD; break;}
        case 32:{USART6_TX_DATA_Struct.hdma->Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;     break;}
    }
    USART6_TX_DATA_Struct.hdma->Init.Mode=DMA_NORMAL;
    USART6_TX_DATA_Struct.hdma->Init.Priority=DMA_PRIORITY_HIGH;
    USART6_TX_DATA_Struct.hdma->Init.FIFOMode=DMA_FIFOMODE_DISABLE;
    USART6_TX_DATA_Struct.hdma->Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;
    USART6_TX_DATA_Struct.hdma->Init.MemBurst=DMA_MBURST_SINGLE;
    USART6_TX_DATA_Struct.hdma->Init.PeriphBurst=DMA_PBURST_SINGLE;

    HAL_DMA_Init(USART6_TX_DATA_Struct.hdma);
    __HAL_LINKDMA(USART6_TX_DATA_Struct.huart,hdmatx,*USART6_TX_DATA_Struct.hdma);
    if(EN_INT){
        HAL_NVIC_SetPriority(USART6_TX_DMA_IRQn,1,1);
        HAL_NVIC_EnableIRQ(USART6_TX_DMA_IRQn);
        // __HAL_DMA_ENABLE_IT(USART6_TX_DATA_Struct.hdma,DMA_IT_HT);
        __HAL_DMA_ENABLE_IT(USART6_TX_DATA_Struct.hdma,DMA_IT_TC);
    }
}

/**
 * @brief   USART6 使用DMA接收
 * @param   pDatas_align : DMA_PeripheralDataSize_Byte
 * @param   mdata_align : DMA_MemoryDataSize_Byte
 * @param   EN_INT :  1 ENABLE or 0 DISABLE the DMA interrupt
 * @retval  none
 * @note    This function must be called before USART6_Init()
 *          可使用HAL_DMA_Start()或HAL_DMA_Start_IT()启动DMA传输
 */
void USART6_DMA_Receive_Init(uint32_t pDatas_align, uint32_t mdata_align,uint16_t EN_INT) {
    USART6_TX_DMA_CLK_ENABLE();

    USART6_RX_DATA_Struct.hdma->Instance=USART6_RX_DMA;
    USART6_RX_DATA_Struct.hdma->Init.Request=USART6_RX_DMA_REQ;
    USART6_RX_DATA_Struct.hdma->Init.Direction=DMA_PERIPH_TO_MEMORY;
    USART6_RX_DATA_Struct.hdma->Init.PeriphInc=DMA_PINC_DISABLE;
    USART6_RX_DATA_Struct.hdma->Init.MemInc=DMA_MINC_ENABLE;
    switch(pDatas_align){
        case 8 :{USART6_RX_DATA_Struct.hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;     break;}
        case 16:{USART6_RX_DATA_Struct.hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; break;}
        case 32:{USART6_RX_DATA_Struct.hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;     break;}
    }
    switch(mdata_align){
        case 8 :{USART6_RX_DATA_Struct.hdma->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;     break;}
        case 16:{USART6_RX_DATA_Struct.hdma->Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD; break;}
        case 32:{USART6_RX_DATA_Struct.hdma->Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;     break;}
    }
    USART6_RX_DATA_Struct.hdma->Init.Mode=DMA_NORMAL;
    USART6_RX_DATA_Struct.hdma->Init.Priority=DMA_PRIORITY_HIGH;
    USART6_RX_DATA_Struct.hdma->Init.FIFOMode=DMA_FIFOMODE_DISABLE;
    USART6_RX_DATA_Struct.hdma->Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;
    USART6_RX_DATA_Struct.hdma->Init.MemBurst=DMA_MBURST_SINGLE;
    USART6_RX_DATA_Struct.hdma->Init.PeriphBurst=DMA_PBURST_SINGLE;

    HAL_DMA_Init(USART6_RX_DATA_Struct.hdma);
    __HAL_LINKDMA(USART6_RX_DATA_Struct.huart,hdmarx,*USART6_RX_DATA_Struct.hdma);
    if(EN_INT){
        HAL_NVIC_SetPriority(USART6_RX_DMA_IRQn,1,1);
        HAL_NVIC_EnableIRQ(USART6_RX_DMA_IRQn);
        // __HAL_DMA_ENABLE_IT(USART6_RX_DATA_Struct.hdma,DMA_IT_HT);
        __HAL_DMA_ENABLE_IT(USART6_RX_DATA_Struct.hdma,DMA_IT_TC);
    }
}


/**
 * @brief   USART6 Transmit dma start
 * @param   ndtr: the number of data to be transmitted
 * @retval  none
 */
void USART6_DMA_Transmit_Start(uint16_t ndtr, uint8_t* pDatas) {
    HAL_UART_Transmit_DMA(&USART6_HAL_Handler, pDatas, ndtr);
}

/**
 * @brief   USART6 receive dma start
 * @param   ndtr: the number of data to be received
 * @retval  none
 */
void USART6_DMA_Receive_Start(uint16_t ndtr, uint8_t* pDatas) {
    HAL_UART_Receive_DMA(&USART6_HAL_Handler, pDatas, ndtr);
}

/******************************************General Functions**************************************/

#if USART_USE_COMMAND == 1

/**
 * @brief       串口接收指令处理
 * @param       none
 * @retval      none
 */
void USART_Command(void){
    uint16_t pos;
    UART_RX_DATA_STRUCT* uart_rx_data_str;

    if(USART1_RX_DATA_Struct.State&0x8000){
        uart_rx_data_str=&USART1_RX_DATA_Struct;
    }else if(USART2_RX_DATA_Struct.State&0x8000){
        uart_rx_data_str=&USART2_RX_DATA_Struct;
    }else{
        return;
    }
	
    if((uart_rx_data_str->State)&0x8000){
        for (pos = 0; pos < command_num;pos++){
            if(strstr((char*)(uart_rx_data_str->pDatas), (char*)command_name_list[pos]->name)){
                command_name_list[pos]->func((char*)(uart_rx_data_str->pDatas));
                break;
            }
        }
        uart_rx_data_str->State = 0;
    }
}

#endif

/******************************************CallBack Functions**************************************/


char USART1_Mode = 0;

/**
 * @brief   USART1 Interrupt
 * @param   none
 * @retval  none
 */
void USART1_IRQHandler(void) {
    uint32_t time_out = 0;
    uint32_t maxDelay = 0x1FFFF;

    HAL_UART_IRQHandler(&USART1_HAL_Handler);

    if(USART1_Mode == 0){
        while (HAL_UART_GetState(&USART1_HAL_Handler) != HAL_UART_STATE_READY) {
            time_out++;
            if (time_out > maxDelay) {
                break;
            }
        }
        time_out = 0;
        __HAL_UART_CLEAR_OREFLAG(&USART1_HAL_Handler);
        while (HAL_UART_Receive_IT(&USART1_HAL_Handler,
                                (uint8_t*)USART1_RX_Buffer,
                                USART1_RX_SIZE) != HAL_OK) {
            time_out++;
            if (time_out > maxDelay) {
                break;
            }
        }
    }

}


/**
 * @brief   USART2 Interrupt
 * @param   none
 * @retval  none
 */
void USART2_IRQHandler(void) {
    uint32_t time_out = 0;
    uint32_t maxDelay = 0x1FFFF;

    HAL_UART_IRQHandler(&USART2_HAL_Handler);

    while (HAL_UART_GetState(&USART2_HAL_Handler) != HAL_UART_STATE_READY) {
        time_out++;
        if (time_out > maxDelay) {
            break;
        }
    }
    time_out = 0;
    __HAL_UART_CLEAR_OREFLAG(&USART2_HAL_Handler);
    while (HAL_UART_Receive_IT(&USART2_HAL_Handler,
                               (uint8_t*)USART2_RX_Buffer,
                               USART2_RX_SIZE) != HAL_OK) {
        time_out++;
        if (time_out > maxDelay) {
            break;
        }
    }
}

/**
 * @brief   USART6 Interrupt
 * @param   none
 * @retval  none
 */
void USART6_IRQHandler(void) {
    uint32_t time_out = 0;
    uint32_t maxDelay = 0x1FFFF;

    HAL_UART_IRQHandler(&USART6_HAL_Handler);

    while (HAL_UART_GetState(&USART6_HAL_Handler) != HAL_UART_STATE_READY) {
        time_out++;
        if (time_out > maxDelay) {
            break;
        }
    }
    time_out = 0;
    __HAL_UART_CLEAR_OREFLAG(&USART6_HAL_Handler);
    while (HAL_UART_Receive_IT(&USART6_HAL_Handler,
                               (uint8_t*)USART6_RX_Buffer,
                               USART6_RX_SIZE) != HAL_OK) {
        time_out++;
        if (time_out > maxDelay) {
            break;
        }
    }
}

/**
 *  @brief  串口接收模式
 *  @param   0：标准"\r\n"结尾协议
 *  @param   1: ESP01S透传协议
 *  @param   2: ESP01S接收协议，等待接收符号 ":" (ESP01S接收格式为 "+IPD,xxxx:datas")
 */
uint8_t UART_Rec_mode = 0;

extern uint8_t voice_valid;
extern uint16_t Voice_Value[];
extern uint32_t download_data_length;
// uint16_t download_data_length_one_time = 0;
uint32_t recieved_length = 0;
char* current_pos = NULL;
uint32_t offset = 0;

uint32_t temp = 0;
uint32_t temp_array[1000];

/**
 * @brief   HAL_UART_RxCpltCallback
 * @param   none
 * @retval  none
 * @note    The Recieved RX_DATA should be end with a return (0x0d) and a newline
 * (0x0a) return(0x0d) then newline(0x0a)
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
    static uint8_t flag = 0;        /* 选择使用 */
    UART_RX_DATA_STRUCT *uart_rx_data_str;

    if (huart->Instance == USART1) {
        uart_rx_data_str = &USART1_RX_DATA_Struct;
        USART1_Mode = 0;
    }else if(huart->Instance == USART2){
        uart_rx_data_str = &USART2_RX_DATA_Struct;
    }else if(huart->Instance == USART6){
        uart_rx_data_str = &USART6_RX_DATA_Struct;
    }
    else{
        return;
    }

    if(uart_rx_data_str->pBuffer!=0){
        if(UART_Rec_mode == 0){
            if ((uart_rx_data_str->State & 0x8000) == 0) {
                if ((uart_rx_data_str->State & 0x4000) == 0) {
                    if (uart_rx_data_str->pBuffer[0] == 0x0d) {
                        uart_rx_data_str->State |= 0x4000;
                    } else {
                        uart_rx_data_str->pDatas[uart_rx_data_str->State & 0x3fff] =
                            uart_rx_data_str->pBuffer[0];
                        uart_rx_data_str->State++;
                        if (uart_rx_data_str->State > (uart_rx_data_str->DataSize - 1)) {
                            uart_rx_data_str->State = 0;
                        }
                    }
                } else {
                    if (uart_rx_data_str->pBuffer[0] == 0x0a) {
                        uart_rx_data_str->State |= 0x8000;
                        if(uart_rx_data_str->State & 0x3fff == 0){
                            uart_rx_data_str->State = 0;
                        }
                    } else {
                        uart_rx_data_str->State = 0;
                    }
                }
            }
        }else if(UART_Rec_mode == 1){
            uart_rx_data_str->pDatas[uart_rx_data_str->State++ & 0x3fff] =
                            uart_rx_data_str->pBuffer[0];
        }else if(UART_Rec_mode == 2){
            /* ESP01S接收格式为 "+IPD,xxxx:datas" */
            /* 其中xxxx为接收到的数据量，长度不定可为1~1xxx */
            /* 服务器发送的数据格式为 "ready+xxxxxxxx\r\n " */
            /* 其中xxxxxxxx为字节数 */
            if(flag == 0){
                uart_rx_data_str->pDatas[uart_rx_data_str->State++ & 0x3fff] =
                            uart_rx_data_str->pBuffer[0];
                if(uart_rx_data_str->pBuffer[0] == ':'){
                    flag = 1;
                    // offset = 0;
                    recieved_length += atoi(strstr((char*)uart_rx_data_str->pDatas, "IPD") + 4);
                    temp_array[temp++] = recieved_length;

                    memset(uart_rx_data_str->pDatas, 0, uart_rx_data_str->State&0x3fff);
                    uart_rx_data_str->State = 0;

                }
            }else if(flag == 1){
                *((int8_t*)Voice_Value + offset++) = uart_rx_data_str->pBuffer[0];

                if(offset == recieved_length){
                    flag = 0;
                    if(offset == download_data_length){
                        voice_valid = 2;
						recieved_length = 0;
						offset = 0;
                    }
                }
            }
            
        }
    }
}

char USART1_TX_DATA_Finish_Flag = 0;
char USART1_RX_DATA_Finish_Flag = 0;

/**
 * @brief       DMA2 Channel7 中断服务函数
 * @param       none
 * @retval      none
 */ 
void DMA2_Stream7_IRQHandler(void){
    if(__HAL_DMA_GET_FLAG(USART1_TX_DATA_Struct.hdma, DMA_FLAG_TCIF3_7)){
        __HAL_DMA_CLEAR_FLAG(USART1_TX_DATA_Struct.hdma, DMA_FLAG_TCIF3_7);
        //需要关闭串口的DMA传输，不然会处于忙碌状态，无法进入下一次传输(此处为DMA Norlmal模式)
        HAL_UART_DMAStop(USART1_TX_DATA_Struct.huart);
        USART1_TX_DATA_Finish_Flag = 1;
    }
}

/**
 * @brief       DMA2 Channel6 中断服务函数
 * @param       none
 * @retval      none
 */ 
void DMA2_Stream6_IRQHandler(void){
    if(__HAL_DMA_GET_FLAG(USART1_RX_DATA_Struct.hdma, DMA_FLAG_TCIF2_6)){
        __HAL_DMA_CLEAR_FLAG(USART1_RX_DATA_Struct.hdma, DMA_FLAG_TCIF2_6);
        //需要关闭串口的DMA传输，不然会处于忙碌状态，无法进入下一次传输(此处为DMA Norlmal模式)
        HAL_UART_DMAStop(USART1_RX_DATA_Struct.huart);
        USART1_RX_DATA_Finish_Flag = 1;
    }else if(__HAL_DMA_GET_FLAG(USART1_RX_DATA_Struct.hdma, DMA_FLAG_HTIF2_6)){
        __HAL_DMA_CLEAR_FLAG(USART1_RX_DATA_Struct.hdma, DMA_FLAG_HTIF2_6);
    }
}

char USART2_TX_DATA_Finish_Flag = 0;
char USART2_RX_DATA_Finish_Flag = 0;

/**
 * @brief       DMA2 Channel5 中断服务函数
 * @param       none
 * @retval      none
 */ 
void DMA2_Stream5_IRQHandler(void){
    if(__HAL_DMA_GET_FLAG(USART2_TX_DATA_Struct.hdma, DMA_FLAG_TCIF1_5)){
        __HAL_DMA_CLEAR_FLAG(USART2_TX_DATA_Struct.hdma, DMA_FLAG_TCIF1_5);
        //需要关闭串口的DMA传输，不然会处于忙碌状态，无法进入下一次传输(此处为DMA Norlmal模式)
        HAL_UART_DMAStop(USART2_TX_DATA_Struct.huart);
        USART2_TX_DATA_Finish_Flag = 1;
    }
}

/**
 * @brief       DMA2 Channel4 中断服务函数
 * @param       none
 * @retval      none
 */ 
void DMA2_Stream4_IRQHandler(void){
    if(__HAL_DMA_GET_FLAG(USART2_RX_DATA_Struct.hdma, DMA_FLAG_TCIF0_4)){
        __HAL_DMA_CLEAR_FLAG(USART2_RX_DATA_Struct.hdma, DMA_FLAG_TCIF0_4);
        //需要关闭串口的DMA传输，不然会处于忙碌状态，无法进入下一次传输(此处为DMA Norlmal模式)
        HAL_UART_DMAStop(USART2_TX_DATA_Struct.huart);
        USART2_RX_DATA_Finish_Flag = 1;
    }
}

