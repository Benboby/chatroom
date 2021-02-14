//
// Created by benboby on 2021/2/14.
//

#include "client.h"

// 客户端类构造函数
Client::Client() {
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    sock = 0;
    pid = 0;
    isClientwork = true;
}

// 连接服务器
void Client::Connect() {
    std::cout << "Connect Server: " << SERVER_IP << " : " << SERVER_PORT << std::endl;

    // 创建socket
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("sock error");
        exit(-1);
    }

    // 连接服务端
    if (connect(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        perror("connect error");
        exit(-1);
    }

    // 创建管道，其中fd[0]用于父进程读，fd[1]用于子进程写
    if (pipe(pipe_fd) < 0) {
        perror("pipe error");
        exit(-1);
    }

    FD_ZERO(&sockset);
    FD_SET(sock, &sockset);
    FD_SET(pipe_fd[0], &sockset);

}

void Client::solvechild() {
    close(pipe_fd[0]);

    // 如果客户端运行正常则不断读取输入发送给服务端
    while (isClientwork) {
        memset(send_buf, 0, BUF_SIZE);
        fgets(send_buf, BUF_SIZE, stdin);
        // 客户输出exit,退出
        if (strncasecmp(send_buf, "EXIT", 4) == 0) {
            isClientwork = false;
        }
        // 子进程将信息写入管道
        else {
            if (write(pipe_fd[1], send_buf, strlen(send_buf) - 1) < 0) {
                perror("fork error");
                exit(-1);
            }
        }
    }
}

void Client::solvefather() {
    close(pipe_fd[1]);

    while (isClientwork) {
        int ret = select(std::max(sock, pipe_fd[0]) + 1, &sockset, NULL, NULL, NULL);
        if (ret < 0) {
            perror("select error");
            exit(-1);
        }

        // 对于服务端可读事件，recv读取
        if (FD_ISSET(sock, &sockset)) {
            memset(recv_buf, 0, BUF_SIZE);
            //接受服务端消息
            int ret = recv(sock, recv_buf, BUF_SIZE, 0);

            // 服务端关闭
            if (ret <= 0) {
                std::cout << "Server closed connection: " << sock << std::endl;
                close(sock);
                isClientwork = 0;
            } else {
                std::cout << recv_buf << std::endl;
                std::cout << "(Input 'exit' to exit the chat room)" << std::endl;
            }
        }
        // 子进程写入事件发生，父进程处理并发送服务端
        if (FD_ISSET(pipe_fd[0], &sockset)) {
            //父进程从管道中读取数据
            memset(send_buf, 0, BUF_SIZE);
            int ret = read(pipe_fd[0], send_buf, BUF_SIZE);
            // ret = 0
            if (ret == 0)
                isClientwork = 0;
            else {
                // 将信息发送给服务端
                send(sock, send_buf, BUF_SIZE, 0);
            }
        }

        //重新设置 sockset. (即将 sockset 清空, 并将 sockfd 和 pipe_fd[0] 加入)
        FD_ZERO(&sockset);
        FD_SET(sock, &sockset);
        FD_SET(pipe_fd[0], &sockset);
    }
}

void Client::Start() {

    // 连接服务器
    Connect();

    // 创建子进程
    pid = fork();
    if (pid < 0) {
        perror("fork error");
        close(sock);
        exit(-1);
    } else if (pid == 0) {
        solvechild();
    } else {
        solvefather();
    }
    if (pid) {
        //关闭父进程的管道和sock
        close(pipe_fd[0]);
        close(sock);
    } else {
        //关闭子进程的管道
        close(pipe_fd[1]);
    }
}

int main() {
    Client client;
    client.Start();
    return 0;
}
