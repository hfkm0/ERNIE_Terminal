/**
 ****************************************************************************************************
 * @file     Command.C
 * @author   HFKM
 * @version  V1.0
 * @date     2023-04-10
 * @license  Copyright (c) 2020-2032, HFKM
 ****************************************************************************************************
 * @attention
 * 
 * 实验平台:正点原子 Mini Pro H750开发板
 * 
 ****************************************************************************************************
 */

#include "Command.H"

/*用到的头文件随具体使用而决定*/
#include "stm32h7xx.h"
#include "./BSP/USART/USART.H"

/*指令结构表*****************************************/

// LED控制,格式 ledx on/off
// char *command_led_args[2] = {" on", " off"};
// const command_type command_led_ctrl={
//     .name = "led",
//     .args = command_led_args,
//     .func = led_ctrl
// };


//快速修改参数，不检查准确格式
//格式:  QK+xx xxx.xxx   xx为参数编号，xxx.xxx为参数值
char *command_quick_fix_args[1]={" "};
const command_type command_quick_fix={
    .name = "QK+",
    .args = command_quick_fix_args,
    .func = Quick_Fix
};

/*指令集合******************************************/
const char command_num = 1;
const command_type *command_name_list[] = {
    // &command_led_ctrl
    &command_quick_fix
};


/*指令函数随具体使用而决定***************************/

// /**
//  * @brief       LED控制
//  * @param       none
//  * @retval      none
//  * @note        指令格式 ledx on/off
//  */
// void led_ctrl(char* args){
//     uint16_t led_num = args[3]-'0';
//     uint16_t state = 0;
//     if(led_num==0||led_num>4){return;}

//     if(args[5]=='o'&&args[6]=='n'){
//         state=1;
//     }else if(args[5]=='o'&&args[6]=='f'&&args[7]=='f'){
//         state=0;
//     }else{return;}

//     switch (led_num){
//     case 1:LED1_std(state);break;
//     case 2:LED2_std(state);break;
//     case 3:LED3_std(state);break;
//     case 4:LED4_std(state);break;
//     }
// }


/**
 * @brief       快速参数修正
 * @param       none
 * @retval      none
 * @note        快速修改参数，不检查准确格式
 *              格式:  QK+xx xxx.xxx   xx为参数编号，xxx.xxx为参数值
 * @note        0:
 */
void Quick_Fix(char* args){
    uint8_t id = 0;
    float value = 0;

    id=(args[3]-'0')*10+args[4]-'0';
    value = (args[6] - '0') * 100 + (args[7] - '0') * 10 + (args[8] - '0') +
            ((float)(args[10] - '0')) / 10 + ((float)(args[11] - '0')) / 100 +
            ((float)(args[12] - '0')) / 1000;

    switch (id){
    case 0:
        value = value;
        break;
    }
}
