/**
 ****************************************************************************************************
 * @file     MPU.C
 * @author   HFKM
 * @version  V1.0
 * @date     2023-03-08
 * @license  Copyright (c) 2020-2032, HFKM
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 Mini Pro H750开发板
 *
 ****************************************************************************************************
 */

#include "./BSP/MPU/MPU.H"

/**
 * @brief   Set MPU protection for the specified area
 * @param   BaseAddress: The BaseAddress of the area which be protected by the MPU
 * @param   Size: The size of the area which be protected by the MPU( The vlaue must be a multiple of 32,in bytes)
 * @param   Number: The number of the area which be protected by the MPU( 0~7 )
 * @param   DisableExec: Whether to deny instruction access 0: Allow 1:Deny
 * @param   AccessPermission: The access permission
 *  @arg    0: No access
 *  @arg    1: Only privielged read and write access is supported
 *  @arg    2: Disbale user write access
 *  @arg    3: Full access
 *  @arg    4: Impossible to predict ( Disbale this setting )
 *  @arg    5: Only privielged read access is supported
 *  @arg    6: Read only
 * @param   IsShareable: Whether enable share
 * @param   IsCacheable: Whether enable cache
 * @param   IsBufferable: Whether enable buffer
 * @retval  0,success   1,failure
 */
uint8_t MPU_Set_Protection(uint32_t BaseAddress,
                           uint32_t Size,
                           uint32_t Number,
                           uint8_t DisableExec,
                           uint8_t AccessPermission,
                           uint8_t IsShareable,
                           uint8_t IsCacheable,
                           uint8_t IsBufferable) {
    MPU_Region_InitTypeDef MPU_Region_init_struct;
    HAL_MPU_Disable();

    MPU_Region_init_struct.Enable = MPU_REGION_ENABLE;
    MPU_Region_init_struct.BaseAddress = BaseAddress;
    MPU_Region_init_struct.Size = Size;
    MPU_Region_init_struct.Number = Number;
    MPU_Region_init_struct.DisableExec = DisableExec;
    MPU_Region_init_struct.SubRegionDisable = 0x00;
    MPU_Region_init_struct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_Region_init_struct.AccessPermission = AccessPermission;
    MPU_Region_init_struct.IsBufferable = IsBufferable;
    MPU_Region_init_struct.IsCacheable = IsCacheable;
    MPU_Region_init_struct.IsShareable = IsShareable;

    HAL_MPU_ConfigRegion(&MPU_Region_init_struct);
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
    return 0;
}

/**
 * @brief       Set the storage areas to be protected
 * @note        Some storage areas must be protected by MPU.Otherwise,program may run abnomally
 *              For example,MCU Screen does not display,camera dtata acquisition error and so on
 * @param       none
 * @retval      none
 */
void MPU_Memory_Protection(void)
{
    /* 保护整个DTCM,共128K字节,允许指令访问,禁止共用,允许cache,允许缓冲 */
    MPU_Set_Protection(0x20000000, MPU_REGION_SIZE_128KB, MPU_REGION_NUMBER1, MPU_INSTRUCTION_ACCESS_ENABLE,
                       MPU_REGION_FULL_ACCESS, MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);

    /* 保护整个AXI SRAM,共512K字节,允许指令访问,禁止共用,允许cache,允许缓冲 */
    MPU_Set_Protection(0x24000000, MPU_REGION_SIZE_512KB,MPU_REGION_NUMBER2, MPU_INSTRUCTION_ACCESS_ENABLE,
                       MPU_REGION_FULL_ACCESS, MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);

    /* 保护整个SRAM1~SRAM3,共288K字节,允许指令访问,禁止共用,允许cache,允许缓冲 */
    MPU_Set_Protection(0x30000000, MPU_REGION_SIZE_512KB,MPU_REGION_NUMBER3, MPU_INSTRUCTION_ACCESS_ENABLE,
                       MPU_REGION_FULL_ACCESS, MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);

    /* 保护整个SRAM4,共64K字节,允许指令访问,禁止共用,允许cache,允许缓冲 */
    MPU_Set_Protection(0x38000000, MPU_REGION_SIZE_64KB, MPU_REGION_NUMBER4, MPU_INSTRUCTION_ACCESS_ENABLE,
                       MPU_REGION_FULL_ACCESS, MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);

    /* 保护MCU LCD屏所在的FMC区域,,共64M字节,允许指令访问,禁止共用,禁止cache,禁止缓冲 */
    MPU_Set_Protection(0x60000000, MPU_REGION_SIZE_64MB, MPU_REGION_NUMBER5, MPU_INSTRUCTION_ACCESS_ENABLE,
                       MPU_REGION_FULL_ACCESS, MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_NOT_CACHEABLE, MPU_ACCESS_NOT_BUFFERABLE);

    /* 保护SDRAM区域,共64M字节,允许指令访问,禁止共用,允许cache,允许缓冲 */
    MPU_Set_Protection(0XC0000000, MPU_REGION_SIZE_64MB, MPU_REGION_NUMBER6, MPU_INSTRUCTION_ACCESS_ENABLE,
                       MPU_REGION_FULL_ACCESS, MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);

    /* 保护整个NAND FLASH区域,共256M字节,禁止指令访问,禁止共用,禁止cache,禁止缓冲 */
    MPU_Set_Protection(0X80000000, MPU_REGION_SIZE_256MB, MPU_REGION_NUMBER7, MPU_INSTRUCTION_ACCESS_DISABLE,
                       MPU_REGION_FULL_ACCESS, MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_NOT_CACHEABLE, MPU_ACCESS_NOT_BUFFERABLE);
}

/**
 * @brief   MPU MemManage service function
 * @param   None
 * @retval  None
 * @note    After entering this interrupt,the program cannot be resumed and needs to be reset
 */
void MemManage_Handler(void){
    printf("Mem Access Error!\r\n");
    delay_ms(1000);
    printf("Soft Reseting...\r\n");
    delay_ms(1000);
    NVIC_SystemReset();
}
