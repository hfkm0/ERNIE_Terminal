#include "stdio.h"
#include "string.h"

/**
 * @brief   数据检索
 * @param   str:     查询数据
 * @param   str_len: 查询数据长度
 * @param   data:    源数据
 * @param   data_len:源数据长度
 * @retval  0:   失败      others:  偏移地址
 * @note    strstr无法满足使用需求
 */
int Data_find(char* str, int str_len, char* data, int data_len){
    int str_pos = 0;
    int data_pos = 0;

    for(; data_pos < data_len; data_pos++){
        if(data[data_pos] == str[0]){
            for(str_pos = 0; str_pos < str_len; str_pos++){
                if(data[data_pos + str_pos] != str[str_pos]){
                    str_pos = 0;
                    break;
                }
            }
            if(str_pos == str_len){
                return data_pos;
            }
        }
    }
    return 0;
}

int main(){
    char* str = "rsta";
    char* data = "rstbcdersfghijklmnopqrstauvwxyrstaz";

    int pos = 0;

    pos = Data_find(str, 4, data, 35);

    printf("%d\n", pos);

    return 0;
}