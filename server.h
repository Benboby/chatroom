//
// Created by benboby on 2021/2/14.
//

#ifndef CHATROOM_SERVER_H
#define CHATROOM_SERVER_H

#include <set>
#include <iostream>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "log.h"

// 默认服务器端IP地址
#define SERVER_IP "127.0.0.1"

// 服务器端口号
#define SERVER_PORT 8888

// 提醒你是聊天室中唯一的客户
#define CAUTION "Now it's just you in the chat room!!!"

const int EPOLL_SIZE = 5000;
const int BUF_SIZE = 4096;
const int backlog = 5;

Logger logger("server.log");

class Server {
public:
    Server();
    ~Server() = default;

    struct epoll_event events[EPOLL_SIZE];
    void init();
    void addfd(int epollfd, int fd, bool enable_et);
    bool handle_broadcast(int clientfd);
    void handle_events(int events_count);
    void handle_accept();
    void start_server();

private:
    struct sockaddr_in serverAddr;
    int listen_fd;
    int epfd;
    std::set<int> clients_list;
};

#endif //CHATROOM_SERVER_H
