/**
 ****************************************************************************************************
 * @file     Command.H
 * @author   HFKM
 * @version  V1.0
 * @date     2023-04-10
 * @license  Copyright (c) 2020-2032, HFKM
 ****************************************************************************************************
 * @attention
 * 
 * 实验平台:梁山派GD32F450(实际MCU为GD32f470ZGT6)
 * 
 ****************************************************************************************************
 */

#ifndef __COMMAND_H
#define __COMMAND_H


/*Main函数中指令处理函数格式*******************************/
// /**
//  * @brief       串口1接收数据处理
//  * @param       none
//  * @retval      none
//  */
// void USART0_Command(void){
//     uint16_t pos;
//     for (pos = 0; pos < command_num;pos++){
//         if(strstr((char*)USART0_Rec_Data, (char*)command_name_list[pos]->name)){
//             command_name_list[pos]->func((char*)USART0_Rec_Data);
//             break;
//         }
//     }
// }



typedef struct {
    char *name;
    char **args;
    void (*func)(char* args);
} command_type;

/*具体函数随具体使用而决定*/
// void led_ctrl(char* args);
void Quick_Fix(char* args);

extern const char command_num;
extern const command_type* command_name_list[];

#endif
