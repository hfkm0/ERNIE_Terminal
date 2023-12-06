/**
 ****************************************************************************************************
 * @file     MH_MALLOC.C
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

#include "../Middlewares/MH_MALLOC/MH_MALLOC.H"

#if !(__ARMCC_VERSION >= 6010050)   /* 使用AC5编译器时 */
/* 内存池(64字节对齐) */
static __align(64) uint8_t mem1_base[MEM1_MAX_SIZE];                                    /* 内部 SRAM 内存池 */
static __align(64) uint8_t mem2_base[MEM2_MAX_SIZE] __attribute__((at(0x30000000)));    /* 内部 SRAM1+SRAM2 内存池 */
static __align(64) uint8_t mem3_base[MEM3_MAX_SIZE] __attribute__((at(0x38000000)));    /* 内部 SRAM4 内存池 */
static __align(64) uint8_t mem4_base[MEM4_MAX_SIZE] __attribute__((at(0x20000000)));    /* 内部 DTCM 内存池 */
static __align(64) uint8_t mem5_base[MEM5_MAX_SIZE] __attribute__((at(0x00000000)));    /* 内部 ITCM 内存池 */

/* 内存管理表 */
static MT_TYPE mem1_map_base[MEM1_ALLOC_TABLE_SIZE];                                                            /* 内部 SRAM 内存池MAP */
static MT_TYPE mem2_map_base[MEM2_ALLOC_TABLE_SIZE] __attribute__((at(0x30000000 + MEM2_MAX_SIZE)));    /* 内部 SRAM1+SRAM2 内存池MAP */
static MT_TYPE mem3_map_base[MEM3_ALLOC_TABLE_SIZE] __attribute__((at(0x38000000 + MEM3_MAX_SIZE)));    /* 内部 SRAM4 内存池MAP */
static MT_TYPE mem4_map_base[MEM4_ALLOC_TABLE_SIZE] __attribute__((at(0x20000000 + MEM4_MAX_SIZE)));    /* 内部 DTCM 内存池MAP */
static MT_TYPE mem5_map_base[MEM5_ALLOC_TABLE_SIZE] __attribute__((at(0x00000000 + MEM5_MAX_SIZE)));    /* 内部 ITCM 内存池MAP */

#else   /* 使用AC6编译器时 */
/* 内存池(64字节对齐) */
static __ALIGNED(64) uint8_t mem1_base[MEM1_MAX_SIZE];                                                         /* 内部 SRAM 内存池 */
static __ALIGNED(64) uint8_t mem2_base[MEM2_MAX_SIZE] __attribute__((section(".bss.ARM.__at_0x30000000")));    /* 内部 SRAM1+SRAM2 内存池 */
static __ALIGNED(64) uint8_t mem3_base[MEM3_MAX_SIZE] __attribute__((section(".bss.ARM.__at_0x38000000")));    /* 内部 SRAM4 内存池 */
static __ALIGNED(64) uint8_t mem4_base[MEM4_MAX_SIZE] __attribute__((section(".bss.ARM.__at_0x20000000")));    /* 内部 DTCM 内存池 */
static __ALIGNED(64) uint8_t mem5_base[MEM5_MAX_SIZE] __attribute__((section(".bss.ARM.__at_0x00000000")));    /* 内部 ITCM 内存池 */

/* 内存管理表 */
static MT_TYPE mem1_map_base[MEM1_ALLOC_TABLE_SIZE];                                                           /* 内部 SRAM 内存池MAP */
static MT_TYPE mem2_map_base[MEM2_ALLOC_TABLE_SIZE] __attribute__((section(".bss.ARM.__at_0x3003E000")));      /* 内部 SRAM1+SRAM2 内存池MAP */
static MT_TYPE mem3_map_base[MEM3_ALLOC_TABLE_SIZE] __attribute__((section(".bss.ARM.__at_0x3800F800")));      /* 内部 SRAM4 内存池MAP */
static MT_TYPE mem4_map_base[MEM4_ALLOC_TABLE_SIZE] __attribute__((section(".bss.ARM.__at_0x2001F000")));      /* 内部 DTCM 内存池MAP */
static MT_TYPE mem5_map_base[MEM5_ALLOC_TABLE_SIZE] __attribute__((section(".bss.ARM.__at_0x0000F800")));      /* 内部 ITCM 内存池MAP */

#endif

/* 内存管理参数 */
const uint32_t mem_tbl_size[SRAM_BANK] = {
    MEM1_ALLOC_TABLE_SIZE, MEM2_ALLOC_TABLE_SIZE, MEM3_ALLOC_TABLE_SIZE,
    MEM4_ALLOC_TABLE_SIZE, MEM5_ALLOC_TABLE_SIZE};          /* 内存表大小 */

const uint32_t mem_blk_size[SRAM_BANK] = {MEM1_BLOCK_SIZE, MEM2_BLOCK_SIZE,
                                          MEM3_BLOCK_SIZE, MEM4_BLOCK_SIZE,
                                          MEM5_BLOCK_SIZE}; /* 内存分块大小 */

const uint32_t mem_size[SRAM_BANK] = {MEM1_MAX_SIZE, MEM2_MAX_SIZE,
                                      MEM3_MAX_SIZE, MEM4_MAX_SIZE,
                                      MEM5_MAX_SIZE};       /* 内存总大小 */

/* 内存管理控制器 */
_MH_Malloc_Dev_t mh_malloc_dev =
{
    MH_mm_Init,                                                                  /* 内存初始化 */
    MH_mm_Perused,                                                               /* 内存使用率 */
    mem1_base, mem2_base, mem3_base, mem4_base, mem5_base,                       /* 内存池 */
    mem1_map_base, mem2_map_base, mem3_map_base, mem4_map_base, mem5_map_base,   /* 内存管理状态表 */
    0, 0, 0, 0, 0,                                                               /* 内存管理未就绪 */
};


/**************************************** MM 配置函数 ****************************************************/

/**
 * @brief       内存管理拷贝
 * @param       *des : 目的地址
 * @param       *src : 源地址
 * @param       len    : 需要复制的内存长度(字节为单位)
 * @retval      无
 */
void MH_mm_Copy(void *des,void *src,uint32_t len)
{
    uint8_t *xdes = (uint8_t*)des;
    uint8_t *xsrc = (uint8_t*)src;

    while(len--){
        *xdes++ = *xsrc++;
    }
}

/**
 * @brief       内存管理填充
 * @param       *addr : 内存首地址
 * @param       *val  : 要设置的值
 * @param       len   : 需要设置的内存大小(字节为单位)
 * @retval      无
 */
void MH_mm_Set(void *addr,uint8_t val,uint32_t len)
{
    uint8_t *xaddr = (uint8_t*)addr;

    while(len--){
        *xaddr++ = val;
    }
}

/**
 * @brief       内存管理初始化
 * @param       memx : 所属内存块
 * @retval      无
 */
void MH_mm_Init(uint8_t memx)
{
    uint8_t mtt_size = sizeof(MT_TYPE); /* 获取mem_map数组的类型长度(uint16_t or uint32_t) */
    MH_mm_Set(mh_malloc_dev.mem_map[memx],0,mem_tbl_size[memx]*mtt_size); /* 内存状态表数据清零 */
    mh_malloc_dev.mem_ready[memx] = 1; /* 内存管理初始化OK */

}                  

/**
 * @brief       申请内存(内部调用)
 * @param       memx : 所属内存块
 * @param       size : 需要申请的内存大小(字节)
 * @retval      内存偏移地址
 * @note        0 ~ 0xFFFFFFFE  :有效的内存偏移地址
 *              0xFFFFFFFF      :无效的内存偏移地址
 */
static uint32_t MH_mm_Malloc(uint8_t memx,uint32_t size)
{
    signed long offset = 0;
    uint32_t num_mem_blk;                /* 内存块数 */
    uint32_t con_mem_blk = 0;            /* 连续内存块数 */
    uint32_t i;                   

    if(!mh_malloc_dev.mem_ready[memx]){ /* 内存未初始化,先执行初始化 */
        mh_malloc_dev.init(memx);
    }

    if(size == 0){
        return 0xFFFFFFFF; /* 不需要分配 */
    }

    num_mem_blk = size/mem_blk_size[memx]; /* 获取需要分配的连续内存块数 */

    if(size % mem_blk_size[memx]){ /* 若不能整除,则分配一个多余的内存块 */
        num_mem_blk++;
    }

    for (offset = mem_tbl_size[memx] - 1; offset >= 0;offset--){ /*搜索整个内存控制区 */
        if(!mh_malloc_dev.mem_map[memx][offset]){
            con_mem_blk++;          /* 连续空内存块数增加 */
        }else{
            con_mem_blk = 0;        /* 连续内存块清零 */
        }

        if(con_mem_blk == num_mem_blk){ /* 找到了连续num_mem_blk个空内存块 */
            for (i = 0; i < num_mem_blk; i++){
                mh_malloc_dev.mem_map[memx][offset + i] = num_mem_blk; /* 标注内存块非空 */
            }
            return (offset * mem_blk_size[memx]); /* 返回偏移地址 */
        }
    }
    return 0xFFFFFFFF;
}

/**
 * @brief       释放内存(内部调用)
 * @param       memx   : 所属内存块
 * @param       offset : 内存地址偏移
 * @retval      释放结果
 *   @arg       0, 释放成功;
 *   @arg       1, 释放失败;
 *   @arg       2, 超区域了(失败);
 */
static uint8_t MH_mm_Free(uint8_t memx,uint32_t offset)
{
    int i;
    
    if(!mh_malloc_dev.mem_ready[memx]){ /* 内存未初始化,先执行初始化 */
        mh_malloc_dev.init(memx);
        return 1;
    }
    
    if(offset < mem_size[memx]){    /* 偏移在内存池内 */
        int index = offset / mem_blk_size[memx];    /* 偏移所在内存块号码 */
        int num_mem_blk = mh_malloc_dev.mem_map[memx][index]; /* 内存块数量 */

        for (i = 0; i < num_mem_blk;i++){   /* 内存块清零 */
            mh_malloc_dev.mem_map[memx][index + i] = 0; 
        }
        return 0;
    }else{
        return 2;   /* 偏移超区 */
    }

}

/**
 * @brief       释放内存(外部调用)
 * @param       memx   : 所属内存块
 * @param       ptr    : 内存首地址
 * @retval      none
 */
void MH_Free(uint8_t memx,void *ptr)
{
    uint32_t offset;

    if(ptr == NULL){
        return;
    }

    offset = (uint32_t)ptr - (uint32_t)mh_malloc_dev.mem_base[memx];
    MH_mm_Free(memx,offset);    /* 释放内存 */
}

/**
 * @brief       分配内存(外部调用)
 * @param       memx : 所属内存块
 * @param       size : 要分配的内存大小(字节)
 * @retval      分配到的内存首地址.
 */
void *MH_Malloc(uint8_t memx,uint32_t size)
{
    uint32_t offset;
    offset = MH_mm_Malloc(memx, size);

    if(offset == 0xFFFFFFFF){   /* 申请出错 */
        return NULL;
    }else{
        return (void*)((uint32_t)mh_malloc_dev.mem_base[memx] + offset); /* 返回偏移地址 */
    }

}

/**
 * @brief       重新分配内存(外部调用)
 * @param       memx : 所属内存块
 * @param       ptr : 旧内存首地址
 * @param       size : 要分配的内存大小(字节)
 * @retval      新分配到的内存首地址.
 */
void *MH_Realloc(uint8_t memx,void *ptr,uint32_t size)
{
    uint32_t offset;
    offset = MH_mm_Malloc(memx, size);

    if(offset == 0xFFFFFFF) /* 申请出错 */
    {
        return NULL;
    }else{
        MH_mm_Copy((void*)((uint32_t)mh_malloc_dev.mem_base[memx]+offset),ptr,size);
        MH_Free(memx, ptr); /* 释放旧内存 */
        return (void*)((uint32_t)mh_malloc_dev.mem_base[memx] + offset); /* 返回偏移地址 */
    }

}

/**
 * @brief       获取内存使用率
 * @param       memx : 所属内存块
 * @retval      使用率(扩大了10倍,0~1000,代表0.0%~100.0%)
 */
uint16_t MH_mm_Perused(uint8_t memx)
{
    uint32_t used = 0;
    uint32_t i;

    for (i = 0; i < mem_tbl_size[memx]; i++) {
        if(mh_malloc_dev.mem_map[memx][i]){
            used++;
        }
    }

    return (used * 1000) / (mem_tbl_size[memx]);
}

