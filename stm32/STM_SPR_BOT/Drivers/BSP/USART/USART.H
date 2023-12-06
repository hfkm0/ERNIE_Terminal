/**
 ****************************************************************************************************
 * @file     USART.H
 * @author   HFKM
 * @version  V1.0
 * @date     2023-02-20
 * @license  Copyright (c) 2020-2032, HFKM
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 Mini Pro H750开发板
 *
 ****************************************************************************************************
 */

#ifndef __USART_H
#define __USART_H

#include "stm32h7xx.h"
#include "stdio.h"

#define USART_USE_COMMAND 0

/***************************************************************************************************/
//UART RECIVE STRUCT
typedef struct
{
    UART_HandleTypeDef *huart;
    DMA_HandleTypeDef *hdma;
    uint8_t  *pBuffer;//Recive Buffer
    uint8_t  *pDatas;//Data Point
    uint16_t BufferSize;
    uint16_t DataSize;
    uint16_t State;
} UART_RX_DATA_STRUCT;

//UART TRANSMIT STRUCT
typedef struct
{
    UART_HandleTypeDef *huart;
    DMA_HandleTypeDef *hdma;
    uint8_t  *pDatas;//Data Point
    uint16_t DataSize;
    uint16_t State;
} UART_TX_DATA_STRUCT;

/***************************************************************************************************/
//The main USART to upper computer
#define USART_MAIN                       USART1

/**************************************USART1*******************************************************/
//USART1
#define USART1_CLK_ENABLE()              do {__HAL_RCC_USART1_CLK_ENABLE();} while (0)
extern UART_HandleTypeDef                USART1_HAL_Handler;
//USART1_TX
#define USART1_TX_PORT                   GPIOA
#define USART1_TX_PIN                    GPIO_PIN_9
#define USART1_TX_AF                     GPIO_AF7_USART1
#define USART1_TX_GPIO_CLK_ENABLE()      do {__HAL_RCC_GPIOA_CLK_ENABLE();} while (0)
//USART1_RX_DATA
#define USART1_RX_PORT                   GPIOA
#define USART1_RX_PIN                    GPIO_PIN_10
#define USART1_RX_AF                     GPIO_AF7_USART1
#define USART1_RX_GPIO_CLK_ENABLE()      do {__HAL_RCC_GPIOA_CLK_ENABLE();} while (0)
//Recieve Configuration
#define USART1_RX_DATA_IT_ENABLE         1
#define USART1_RX_SIZE                   1
#define USART1_RX_DATA_SIZE              255
#define USART1_RX_DATA_IF_OVER           (USART1_RX_DATA_State&0x8000)
#define USART1_RX_DATA_REC_LEN           (USART1_RX_DATA_State&0x3fff)
extern UART_RX_DATA_STRUCT               USART1_RX_DATA_Struct;
//Transmit Configuration
#define USART1_TX_DATA_SIZE              255
extern uint8_t                           USART1_TX_DATA[USART1_TX_DATA_SIZE];
extern UART_TX_DATA_STRUCT               USART1_TX_DATA_Struct;
//Transmit DMA
#define USART1_TX_DMA_CLK_ENABLE()       do {__HAL_RCC_DMA2_CLK_ENABLE();} while (0)
#define USART1_TX_DMA                    DMA2_Stream7
#define USART1_TX_DMA_IRQn               DMA2_Stream7_IRQn
#define USART1_TX_DMA_REQ                DMA_REQUEST_USART1_TX
//Receive DMA
#define USART1_RX_DMA_CLK_ENABLE()       do {__HAL_RCC_DMA2_CLK_ENABLE();} while (0)
#define USART1_RX_DMA                    DMA2_Stream6
#define USART1_RX_DMA_IRQn               DMA2_Stream6_IRQn
#define USART1_RX_DMA_REQ                DMA_REQUEST_USART1_RX

extern char USART1_TX_DATA_Finish_Flag;
extern char USART1_RX_DATA_Finish_Flag;

/**************************************USART2*******************************************************/
//USART2
#define USART2_CLK_ENABLE()              do {__HAL_RCC_USART2_CLK_ENABLE();} while (0)
extern UART_HandleTypeDef                USART2_HAL_Handler;
//USART2_TX
#define USART2_TX_PORT                   GPIOA
#define USART2_TX_PIN                    GPIO_PIN_2
#define USART2_TX_AF                     GPIO_AF7_USART2
#define USART2_TX_GPIO_CLK_ENABLE()      do {__HAL_RCC_GPIOA_CLK_ENABLE();} while (0)
//USART2_RX_DATA
#define USART2_RX_PORT                   GPIOA
#define USART2_RX_PIN                    GPIO_PIN_3
#define USART2_RX_AF                     GPIO_AF7_USART2
#define USART2_RX_GPIO_CLK_ENABLE()      do {__HAL_RCC_GPIOA_CLK_ENABLE();} while (0)
//Recieve Configuration
#define USART2_RX_DATA_IT_ENABLE         1
#define USART2_RX_SIZE                   1
#define USART2_RX_DATA_SIZE              4096
#define USART2_RX_DATA_IF_OVER           (USART2_RX_DATA_State&0x8000)
#define USART2_RX_DATA_REC_LEN           (USART2_RX_DATA_State&0x3fff)
extern UART_RX_DATA_STRUCT               USART2_RX_DATA_Struct;
//Transmit Configuration
#define USART2_TX_DATA_SIZE              255
extern uint8_t                           USART2_TX_DATA[USART2_TX_DATA_SIZE];
extern UART_TX_DATA_STRUCT               USART2_TX_DATA_Struct;
//Transmit DMA
#define USART2_TX_DMA_CLK_ENABLE()       do {__HAL_RCC_DMA2_CLK_ENABLE();} while (0)
#define USART2_TX_DMA                    DMA2_Stream5
#define USART2_TX_DMA_IRQn               DMA2_Stream5_IRQn
#define USART2_TX_DMA_REQ                DMA_REQUEST_USART2_TX
//Receive DMA
#define USART2_RX_DMA_CLK_ENABLE()       do {__HAL_RCC_DMA2_CLK_ENABLE();} while (0)
#define USART2_RX_DMA                    DMA2_Stream4
#define USART2_RX_DMA_IRQn               DMA2_Stream4_IRQn
#define USART2_RX_DMA_REQ                DMA_REQUEST_USART2_RX

extern char USART2_TX_DATA_Finish_Flag;
extern char USART2_RX_DATA_Finish_Flag;

/**************************************USART6*******************************************************/
//USART6
#define USART6_CLK_ENABLE()              do {__HAL_RCC_USART6_CLK_ENABLE();} while (0)
extern UART_HandleTypeDef                USART6_HAL_Handler;
//USART6_TX
#define USART6_TX_PORT                   GPIOC
#define USART6_TX_PIN                    GPIO_PIN_6
#define USART6_TX_AF                     GPIO_AF7_USART6
#define USART6_TX_GPIO_CLK_ENABLE()      do {__HAL_RCC_GPIOC_CLK_ENABLE();} while (0)
//USART6_RX_DATA
#define USART6_RX_PORT                   GPIOC
#define USART6_RX_PIN                    GPIO_PIN_7
#define USART6_RX_AF                     GPIO_AF7_USART6
#define USART6_RX_GPIO_CLK_ENABLE()      do {__HAL_RCC_GPIOC_CLK_ENABLE();} while (0)
//Recieve Configuration
#define USART6_RX_DATA_IT_ENABLE         1
#define USART6_RX_SIZE                   1
#define USART6_RX_DATA_SIZE              4096
#define USART6_RX_DATA_IF_OVER           (USART6_RX_DATA_State&0x8000)
#define USART6_RX_DATA_REC_LEN           (USART6_RX_DATA_State&0x3fff)
extern UART_RX_DATA_STRUCT               USART6_RX_DATA_Struct;
//Transmit Configuration
#define USART6_TX_DATA_SIZE              255
extern uint8_t                           USART6_TX_DATA[USART6_TX_DATA_SIZE];
extern UART_TX_DATA_STRUCT               USART6_TX_DATA_Struct;
//Transmit DMA
#define USART6_TX_DMA_CLK_ENABLE()       do {__HAL_RCC_DMA2_CLK_ENABLE();} while (0)
#define USART6_TX_DMA                    DMA2_Stream3
#define USART6_TX_DMA_IRQn               DMA2_Stream2_IRQn
#define USART6_TX_DMA_REQ                DMA_REQUEST_USART6_TX
//Receive DMA
#define USART6_RX_DMA_CLK_ENABLE()       do {__HAL_RCC_DMA2_CLK_ENABLE();} while (0)
#define USART6_RX_DMA                    DMA2_Stream3
#define USART6_RX_DMA_IRQn               DMA2_Stream2_IRQn
#define USART6_RX_DMA_REQ                DMA_REQUEST_USART6_RX


/***************************************************************************************************/
/*
//USARTx
#define USARTx_CLK_ENABLE()              do {__HAL_RCC_USARTx_CLK_ENABLE();} while (0)
#define USARTx_RX_IT_ENABLE 1
//USARTx_TX
#define USARTx_TX_PORT                   GPIOx
#define USARTx_TX_PIN                    GPIO_PIN_x
#define USARTx_TX_AF                     GPIO_AFX_USARTx
#define USARTX_TX_GPIO_CLK_ENABLE()      do {__HAL_RCC_GPIOA_CLK_ENABLE();} while (0)
//USART1_RX_DATA
#define USARTx_RX_PORT                   GPIOx
#define USARTx_RX_PIN                    GPIO_PIN_x
#define USARTx_RX_AF                     GPIO_AFx_USARTx
#define USARTX_RX_GPIO_CLK_ENABLE()      do {__HAL_RCC_GPIOx_CLK_ENABLE();} while (0)
*/

extern char USART1_Mode;

/*USART1 Func*/
void USART1_Init(uint32_t BaudRate);
void USART_Command(void);

void USART1_DMA_Transmit_Init(uint32_t pDatas_align, uint32_t mdata_align,uint16_t EN_INT);
void USART1_DMA_Transmit_Start(uint16_t ndtr, uint8_t* pDatas);
void USART1_DMA_Receive_Init(uint32_t pDatas_align, uint32_t mdata_align,uint16_t EN_INT);
void USART1_DMA_Receive_Start(uint16_t ndtr, uint8_t* pDatas) ;

uint32_t USART1_Test_Demo(void);

/*USART2 Func*/
void USART2_Init(uint32_t BaudRate);

void USART2_DMA_Transmit_Init(uint32_t pDatas_align, uint32_t mdata_align,uint16_t EN_INT);
void USART2_DMA_Transmit_Start(uint16_t ndtr, uint8_t* pDatas);
void USART2_DMA_Receive_Init(uint32_t pDatas_align, uint32_t mdata_align,uint16_t EN_INT);
void USART2_DMA_Receive_Start(uint16_t ndtr, uint8_t* pDatas) ;

/*USART6 Func*/
void USART6_Init(uint32_t BaudRate, uint8_t it);

#endif

