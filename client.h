//
// Created by benboby on 2021/2/14.
//

#ifndef CHATROOM_CLIENT_H
#define CHATROOM_CLIENT_H

#include <iostream>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>
#include <string.h>

#include "log.h"

// 默认服务器端IP地址
#define SERVER_IP "127.0.0.1"

// 服务器端口号
#define SERVER_PORT 8888

const int BUF_SIZE = 4096;

Logger logger("client.log");

// 客户端类，用来连接服务器发送和接收消息
class Client {
private:
    int sock;
    int pid;
    int pipe_fd[2];
    bool isClientwork;
    char recv_buf[BUF_SIZE];
    char send_buf[BUF_SIZE];
    struct sockaddr_in serverAddr;
    fd_set sockset;

public:
    Client();
    ~Client() = default;
    void Connect();
    void solvechild();
    void solvefather();
    void Start();
};

#endif //CHATROOM_CLIENT_H
