/**
 ****************************************************************************************************
 * @file     MH_MALLOC.H
 * @author   HFKM
 * @version  V1.0
 * @date     2023-06-25
 * @license  Copyright (c) 2020-2032, HFKM
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 Mini Pro H750开发板
 *
 ****************************************************************************************************
 */

#ifndef __MH_MALLOC_H
#define __MH_MALLOC_H

#include "./SYSTEM/sys/sys.h"

/* 定义6个内存池 */
#define SRAM_IN    0        /* AXI内存池，AXI共512KB */
#define SRAM_1_2   1        /* SRAM1/2内存池，SRAM1+SRAM2，共256KB */
#define SRAM4      2        /* SRAM4内存池，SRAM4共64KB */
#define SRAM_DTCM  3        /* DTCM内存池，DTCM共128KB，此部分内存仅CPU和MDMA(通过AHBS)可以访问!!!! */
#define SRAM_ITCM  4        /* ITCM内存池，DTCM共64 KB，此部分内存仅CPU和MDMA(通过AHBS)可以访问!!!! */

#define SRAM_BANK  5        /* 定义支持的SRAM块数 */

/* 定义内存管理表类型，当外扩SDRAM时，必须使用uint32_t类型，否则可以定义成uint16_t，以节省空间 */
#define MT_TYPE    uint16_t


/**
 *  单块内存，内存管理所占用的全部大小计算公式如下：
 *  size = MEM1_MAX_SIZE + (MEM1_MAX_SIZE / MEM1_BLOCK_SIZE) * sizeof(MT_TYPE)
 *  以SRAM_IN为例，size = 474*1024 + (474*1024/64)*2 = 500544 ≈ 489KB
 * 
 *  已知总内存容量(size)，最大内存池的计算公式如下：
 *  MEM1_MAX_SIZE = (MEM1_BLOCK_SIZE * size) / (MEM1_BLOCK_SIZE + sizeof(MT_TYPE))
 *  以SRAM_1_2为例，MEM1_MAX_SIZE = (64 * 256) / (64 + 2) = 248.24KB ≈ 248KB
 */ 

/* mem1内存参数设定.mem1是H7内部的AXI内存. */
#define MEM1_BLOCK_SIZE         64                              /* 内存块大小为64字节 */
#define MEM1_MAX_SIZE           474*1024                        /* 最大管理内存 474K,H7的AXI内存总共512KB */
#define MEM1_ALLOC_TABLE_SIZE   MEM1_MAX_SIZE/MEM1_BLOCK_SIZE   /* 内存表大小 */

/* mem2内存参数设定.mem3是H7内部的SRAM1+SRAM2内存 */
#define MEM2_BLOCK_SIZE         64                              /* 内存块大小为64字节 */
#define MEM2_MAX_SIZE           248 *1024                       /* 最大管理内存248K,H7的SRAM1+SRAM2共256KB */
#define MEM2_ALLOC_TABLE_SIZE   MEM2_MAX_SIZE/MEM2_BLOCK_SIZE   /* 内存表大小 */

/* mem3内存参数设定.mem4是H7内部的SRAM4内存 */
#define MEM3_BLOCK_SIZE         64                              /* 内存块大小为64字节 */
#define MEM3_MAX_SIZE           62 *1024                        /* 最大管理内存62K,H7的SRAM4共64KB */
#define MEM3_ALLOC_TABLE_SIZE   MEM3_MAX_SIZE/MEM3_BLOCK_SIZE   /* 内存表大小 */

/* mem4内存参数设定.mem5是H7内部的DTCM内存,此部分内存仅CPU和MDMA可以访问!!!!!! */
#define MEM4_BLOCK_SIZE         64                              /* 内存块大小为64字节 */
#define MEM4_MAX_SIZE           124 *1024                       /* 最大管理内存124K,H7的DTCM共128KB */
#define MEM4_ALLOC_TABLE_SIZE   MEM4_MAX_SIZE/MEM4_BLOCK_SIZE   /* 内存表大小 */

/* mem5内存参数设定.mem6是H7内部的ITCM内存,此部分内存仅CPU和MDMA可以访问!!!!!! */
#define MEM5_BLOCK_SIZE         64                              /* 内存块大小为64字节 */
#define MEM5_MAX_SIZE           62 *1024                        /* 最大管理内存62K,H7的ITCM共64KB */
#define MEM5_ALLOC_TABLE_SIZE   MEM5_MAX_SIZE/MEM5_BLOCK_SIZE   /* 内存表大小 */


/* 定义NULL */
#ifndef NULL
#define NULL 0
#endif

/* 内存管理控制器 */
typedef struct{
    void (*init)(uint8_t);          /* 初始化 */
    uint16_t (*perused)(uint8_t);   /* 内存使用率 */
    uint8_t *mem_base[SRAM_BANK];   /* 内存池,管理SRAM_BANK个区域的内存 */
    MT_TYPE *mem_map[SRAM_BANK];    /* 内存管理状态表 */
    uint8_t mem_ready[SRAM_BANK];   /* 内存管理是否就绪 */
} _MH_Malloc_Dev_t;

extern _MH_Malloc_Dev_t mh_malloc_dev;                /* 内存管理控制器 */


void MH_mm_Init(uint8_t memx);                              /* 内存管理初始化(外/内部调用) */
uint16_t MH_mm_Perused(uint8_t memx);                       /* 获得内存使用率(外/内部调用) */

void MH_mm_Set(void *addr,uint8_t val,uint32_t len);        /* 内存管理填充 */
void MH_mm_Copy(void *des,void *src,uint32_t len);          /* 内存管理拷贝 */
void MH_Free(uint8_t memx,void *ptr);                       /* 释放内存(外部调用) */
void *MH_Malloc(uint8_t memx,uint32_t size);                /* 申请内存(外部调用) */
void *MH_Realloc(uint8_t memx,void *ptr,uint32_t size);     /* 重新申请内存(外部调用) */

#endif
