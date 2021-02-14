//
// Created by benboby on 2021/2/14.
//

#include "server.h"

// 服务端类构造函数
Server::Server() {
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    listen_fd = 0;
    epfd = 0;
}

// 注册新的fd到epollfd中
// 参数enable_et表示是否启用ET模式，如果为True则启用，否则使用LT模式
void Server::addfd(int epollfd, int fd, bool enable_et)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    if(enable_et)
        ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    // 设置socket为nonblocking模式
    // 执行完就转向下一条指令，不管函数有没有返回。
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0)| O_NONBLOCK);
    std::cout << "fd added to epoll!\n\n" << std::endl;
}

// 初始化服务端并启动监听
void Server::init() {
    //创建监听socket
    listen_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("listen_fd");
        exit(-1);
    }

    //绑定地址
    if (bind(listen_fd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind error");
        exit(-1);
    }

    //监听
    int ret = listen(listen_fd, 5);
    if (ret < 0) {
        perror("listen error");
        exit(-1);
    }

    std::cout << "Start to listen: " << SERVER_IP << std::endl;

    //在内核中创建事件表
    epfd = epoll_create(EPOLL_SIZE);

    if (epfd < 0) {
        perror("epfd error");
        exit(-1);
    }

    //往事件表里添加监听事件
    addfd(epfd, listen_fd, true);

}

void Server::handle_events (int events_count) {
    for (int i = 0; i < events_count; ++i) {
        int sockfd = events[i].data.fd;
        // 新用户
        if (sockfd == listen_fd) {
            handle_accept();
        }
        // 广播
        else {
            if (!handle_broadcast(sockfd)) {
                perror("error");
                close(listen_fd);
                close(epfd);
                exit(-1);
            }
        }
    }
}

void Server::handle_accept() {
    struct sockaddr_in client_address;
    socklen_t client_addrLength = sizeof(struct sockaddr_in);
    int clientfd = accept(listen_fd, (struct sockaddr *) &client_address, &client_addrLength);

    std::cout << "client connection from: "
              << inet_ntoa(client_address.sin_addr) << ":"
              << ntohs(client_address.sin_port) << ", clientfd = "
              << clientfd << std::endl;

    addfd(epfd, clientfd, true);

    clients_list.insert(clientfd);
    std::cout << "Add new clientfd = " << clientfd << " to epoll" << std::endl;
    std::cout << "Now there are " << clients_list.size() << " clients int the chat room" << std::endl;

    // 服务端发送欢迎信息
    char message[BUF_SIZE];
    memset(message, 0, BUF_SIZE);
    sprintf(message, "Welcome! Your chat ID is: Client #%d", clientfd);
    if (!send(clientfd, message, BUF_SIZE, 0)) {
        perror("send error");
        close(listen_fd);
        close(epfd);
        exit(-1);
    }
}

// 发送广播消息给所有客户端
bool Server::handle_broadcast(int clientfd) {
    // buf[BUF_SIZE] 接收新消息
    // message[BUF_SIZE] 保存格式化的消息
    char buf[BUF_SIZE], msg[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    memset(msg, 0, BUF_SIZE);

    // 接收新消息
    std::cout << "read from client(clientID = " << clientfd << ")" << std::endl;
    int len = recv(clientfd, buf, BUF_SIZE, 0);

    // 如果客户端关闭了连接
    if (len <= 0) {
        close(clientfd);

        // 在客户端列表中删除该客户端
        clients_list.erase(clients_list.find(clientfd));

        // 通知用户有人退出
        sprintf(msg, "ClientID = %d closed.\nnow there are %d client in the char room", clientfd, clients_list.size());
        for (auto fd : clients_list) {
            if (send(fd, msg, BUF_SIZE, 0) < 0) {
                return false;
            }
        }
    }
    // 发送广播消息给所有客户端
    else {
        // 只有一个人
        if (clients_list.size() == 1) {

            send(clientfd, CAUTION, strlen(CAUTION), 0);
            return len;
        }
        // 格式化发送的消息内容
        sprintf(msg, "ClientID %d >> %s", clientfd, buf);

        // 遍历客户端列表依次发送消息，需要判断不要给来源客户端发
        for (auto fd : clients_list) {
            if (fd != clientfd) {
                if (send(fd, msg, BUF_SIZE, 0) < 0) {
                    return false;
                }
            }
        }
    }
    return true;
}

// 启动服务端
void Server::start_server() {
    // 服务端初始化
    init();

    while (true) {
        // events_count表示就绪事件的数目
        int events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);
        if (events_count < 0) {
            perror("epoll failure");
            break;
        }
        std::cout << "events_count =\n" << events_count << std::endl;
        handle_events(events_count);
    }
    close(listen_fd);
    close(epfd);
}

int main() {
    Server server;
    server.start_server();
    return 0;
}