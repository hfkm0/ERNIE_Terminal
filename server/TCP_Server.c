#include <arpa/inet.h>
#include <iconv.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT 6233        // 服务器端口
#define MAX_CLIENTS 10          // 最大用户端数量

int server_fd;                  //server 文件描述符
int client_fds[MAX_CLIENTS];    //clients 文件描述符
int num_clients = 0;            //clients 数量
pthread_mutex_t mutex;          //互斥锁

/*系统函数*/
void* client_handler(void* arg);    // 用户端线程函数
void* input_thread(void* arg);      // 输出线程函数

/*应用函数*/
char* utf8_to_gbk(char* utf8_str, int len);  // UTF8字符转GBK

/*回调处理函数*/
int Audio_Receive_Handler(int client_fd, char* payload);  // 音频数据接收处理函数

int main() {
    struct sockaddr_in server_addr;
    pthread_t input_tid, client_tids[MAX_CLIENTS];

    printf("TCP服务端\n");
	printf("IP: 124.223.xxx.xxx\n");
    printf("端口号为 6xxx\n");
	// printf("服务端可同时与用户端进行接收与发送数据\n");
    // printf("输入到服务器标准输入区的每个字符都会进入发送区\n");
    // printf("每按下一次退格键可清除一个发送区字符\n");

    // 创建套接字
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    printf("套接字创建完成...\n");

    // 设置套接字选项，允许端口重用
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    printf("端口复用完成...\n");

    // 绑定地址和端口
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) <
        0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    printf("地址和端口绑定完成...\n");

    // 监听连接
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("监听连接完成...\n");

    // 初始化互斥锁
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("pthread_mutex_init");
        exit(EXIT_FAILURE);
    }
    printf("互斥锁初始化完成...\n");

    // 创建输入线程
    if (pthread_create(&input_tid, NULL, input_thread, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    printf("输入线程创建完成...\n");
    printf("端口监听中:\n");
    // 处理连接请求
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd;
    while (1) {
        // 接收连接
        if ((client_fd = accept(server_fd, (struct sockaddr*)&client_addr,
                                &client_addr_len)) < 0) {
            perror("accept");
            printf("连接异常!\n");
            exit(EXIT_FAILURE);
        }
        printf("收到连接请求...\n");
        // 创建用户端线程
        if (num_clients >= MAX_CLIENTS) {
            printf("用户端连接已达最大值,拒绝本次连接!\n");
            close(client_fd);
        } else {
            printf("接受连接\n");
            pthread_mutex_lock(&mutex);
            client_fds[num_clients++] = client_fd;
            pthread_mutex_unlock(&mutex);
            pthread_create(&client_tids[num_clients - 1], NULL, client_handler,
                           &client_fd);
        }
    }
    printf(".\n");
    pthread_mutex_destroy(&mutex);
    close(server_fd);
    return 0;
}

/**
 * @brief 用户端线程函数
 * @param arg 用户端文件描述符
 * @retval none
 */
void* client_handler(void* arg) {
    int i = 0;
    int client_fd = *(int*)arg;
    char buffer[1024];
    int bytes_received;
    printf("用户端连接,用户编号%d\n\n", client_fd);
    while ((bytes_received = recv(client_fd, buffer, sizeof(buffer), 0)) > 0) {
        buffer[bytes_received] = '\0';
        printf("用户%2d的消息:\n%s\n\n", client_fd, buffer);
        pthread_mutex_lock(&mutex);
        // 接收消息处理逻辑
        /*终止消息*/
        if (strstr(buffer, "client_stop")) {
            pthread_mutex_unlock(&mutex);
            memset(buffer,0,sizeof(buffer));
            break;
        }
        /*音频消息*/
        if(strstr(buffer,"audio")){
            bytes_received = Audio_Receive_Handler(client_fd, buffer);
        }
        /*解锁*/
        pthread_mutex_unlock(&mutex);
    }
    close(client_fd);
    pthread_mutex_lock(&mutex);
    for (i = 0; i < num_clients; i++) {
        if (client_fds[i] == client_fd) {
            client_fds[i] = 0;
        }
    }
    printf("用户%2d离线\n", client_fd);
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
    return NULL;
}

/**
 * @brief 输入线程函数
 * @param arg 参数
 * @retval none
 */
void* input_thread(void* arg) {
    int i = 0;
    fd_set read_fds;
    char input_buffer[1024];
    char* send_buffer;
    int bytes_received;

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        for (i = 0; i < num_clients; i++) {
            int fd = client_fds[i];
            if (fd != 0) {
                FD_SET(fd, &read_fds);
            }
        }
        if (select(FD_SETSIZE, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            bytes_received =
                read(STDIN_FILENO, input_buffer, sizeof(input_buffer));
            input_buffer[--bytes_received] = '\0';
            send_buffer = utf8_to_gbk(input_buffer, -1);
            if (send_buffer == NULL) {
                printf("发送字符串错误!请重新输入!\n");
                continue;
            }
            printf("\n服务器消息:\n%s\n\n", input_buffer);
            // 发送消息处理逻辑
            if (strstr(input_buffer, "server_stop")) {
                break;
            }else{
                pthread_mutex_lock(&mutex);
                for (i = 0; i < num_clients; i++) {
                    int fd = client_fds[i];
                    if (fd != 0) {
                        send(fd, send_buffer, strlen(send_buffer), 0);
                    }
                }
                pthread_mutex_unlock(&mutex);
            }
        }
    }
    printf("服务退出\n");
    for (i = 0; i < num_clients; i++) {
        int fd = client_fds[i];
        if (fd != 0) {
            printf("用户%2d离线\n", client_fds[i]);
        }
    }
    pthread_mutex_destroy(&mutex);
    close(server_fd);
    printf("关闭\n");
    exit(1);
}


/**
 * @brief   UTF8字符串转GBK
 * @param   utf8_str:   UTF8字符串
 * @param   len:        字符串长度
 * @retval  转换后的GBK字符串指针
 * @note    会在字符串结尾带上0x0D 0x0A
 *          如果len为-1则自动计算字符串长度
 */
char* utf8_to_gbk(char* utf8_str, int len) {
    static char* gbk_str;
    size_t utf8_len;
    size_t gbk_len;

    if(len == -1){
        utf8_len = strlen(utf8_str);
    }else{
        utf8_len = len;
    }
    gbk_len = utf8_len * 2;
    
    // 分配转换后的GBK字符串内存
    gbk_str = (char*)malloc(gbk_len + 1);
    memset(gbk_str, 0, gbk_len + 1);
    // 初始化iconv转换句柄
    iconv_t cd = iconv_open("GBK", "UTF-8");
    if (cd == (iconv_t)(-1)) {
        perror("iconv_open");
        free(gbk_str);
        return NULL;
    }
    char* src_str = (char*)utf8_str;
    char* dst_str = gbk_str;
    size_t src_len = utf8_len;
    size_t dst_len = gbk_len;
    // 调用iconv进行转换
    size_t result = iconv(cd, &src_str, &src_len, &dst_str, &dst_len);
    if (result == (size_t)(-1)) {
        printf("字符错误!\n");
        free(gbk_str);
        iconv_close(cd);
        return NULL;
    }
    strcat(gbk_str, "\r\n");
    // 关闭iconv句柄
    iconv_close(cd);
    return gbk_str;
}

/**
 * @brief  音频数据接收处理函数 
 * @param  client_fd: 用户端文件描述符
 * @param  payload:   音频数据
 * @retval bytes_received: 接收到的字节数
 * @note   接收协议： 接收到的数据格式为"audio_start" + 音频大小(10个数字，字节数) + 音频数据
 *         接收数据完成后将音频数据保存为"audio.wav"
 */
int Audio_Receive_Handler(int client_fd, char* payload) {
    int i = 0;
    FILE* audio_fp;
    int bytes_received = 0;
    int audio_size = 0;
    char* audio_buffer;
    int last_audio_bytes_received = 0;
    int audio_bytes_received = 0;
    char name_buffer[24];
    static int audio_count = 0;
    int recognize_len = 0;
    char recognize_res[1024];

    printf("接收来自用户端%d的音频(其余消息阻塞)\n", client_fd);
    /*获取音频大小*/
    /* 格式 "audio+xxxxxxxx" */
    printf("接收:%s\n\n", payload);
    audio_size = atoi(payload + 5);
    printf("音频大小为%d字节\n", audio_size);
    /*接收音频数据*/
    audio_buffer = (char*)malloc(audio_size);

    printf("开始接收音频数据...\n");

    while (audio_bytes_received < audio_size) {
        bytes_received =
            recv(client_fd, audio_buffer + audio_bytes_received, audio_size, 0);
        audio_bytes_received += bytes_received;
        printf("%d\n", audio_bytes_received);
    }

    printf("音频数据接收完成,共%d字节\n", audio_bytes_received);
    /*保存音频*/
    sprintf(name_buffer, "audio.txt", audio_count++);
    audio_fp = fopen(name_buffer, "wb");
    fwrite(audio_buffer, 1, audio_bytes_received, audio_fp);
    fclose(audio_fp);

    audio_fp = fopen("receive_done.txt", "wb");
    fwrite("done", 1, 4, audio_fp);
    fclose(audio_fp);

    printf("音频保存完成\n\n", audio_count);
    /*清理*/
    free(audio_buffer);
    memset(payload, 0, sizeof(payload));

    /* 下传PCM音频方式，可后期使用STOS时用 */
    // /* 等待语音识别结果 */
    // printf("等待语音文件...\n");
    // while(1){
    //     if((fopen("Voice_process_done.txt", "rb")) == NULL);
    //     else break;
    //     usleep(30000); // 0.03s
    // }
    // printf("语音文件已生成\n下传中...\n");

    // /* 下传语音文件 */
    // FILE* fp_pcm = fopen("result_audio.txt", "rb");
    // char buffer_pcm[2048];
    // int bytes_read = 0;
    // int pcm_size = 0;
    // int pcm_send_size = 0;
    // char buffer_header[30] = "ready+00000000end";

    // fseek(fp_pcm, 0, SEEK_END);
    // pcm_size = ftell(fp_pcm);
    // fseek(fp_pcm, 0, SEEK_SET);

    // sprintf(buffer_header, "ready+%08dend", pcm_size);

    // printf("Header = %s\n", buffer_header);

    // send(client_fd, buffer_header, 17, 0);
    // usleep(20000);
    // printf("Header sent\n");
    
    // while((bytes_read = fread(buffer_pcm, 1, 2048, fp_pcm)) > 0){
    //     pcm_send_size += bytes_read;
    //     if(pcm_send_size > 320000) break;
    //     send(client_fd, buffer_pcm, bytes_read, 0);
    //     usleep(5000);
    //     printf("pcm_send_size=%d\n", pcm_send_size);
    // }
    // printf("下传完成\n");
    // fclose(fp_pcm);

    // remove("Voice_process_done.txt");
    // remove("result_audio.txt");

    /* 等待语音识别结果 */
    printf("等待语音文件...\n");
    while(1){
        if((fopen("Voice_process_done.txt", "rb")) == NULL);
        else break;
        usleep(30000); // 0.03s
    }
    printf("语音文件已生成\n下传中...\n");

    /* 下传语音文件 */
    FILE* fd_utf8;
    char buffer_gbk[2048];
    char* utf8_str;
    char* gbk_str;
    int utf8_len = 0;
    char buffer_header[30] = "ready+00000000end";

    fd_utf8 = fopen("ernie_result.txt", "r");

    fseek(fd_utf8, 0, SEEK_END);
    utf8_len = ftell(fd_utf8);
    fseek(fd_utf8, 0, SEEK_SET);

    utf8_str = (char*)malloc(utf8_len);

    fread(utf8_str, 1, utf8_len, fd_utf8);

    printf("utf8_str = %s\n", utf8_str);
    printf("utf8_len = %d\n", utf8_len);

    gbk_str = utf8_to_gbk(utf8_str, utf8_len);

    sprintf(buffer_header, "ready+%08dend", strlen(gbk_str));

    printf("Header = %s\n", buffer_header);

    send(client_fd, buffer_header, 17, 0);
    usleep(200000);
    printf("Header sent\n");
    
    send(client_fd, gbk_str, strlen(gbk_str), 0);

    printf("下传完成\n");
    fclose(fd_utf8);

    remove("Voice_process_done.txt");
    remove("result_audio.txt");

    printf("done\n");

    return bytes_received;
}
