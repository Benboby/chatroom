# chatroom

在学习Linux C++网络编程的过程中实现的一个小型聊天室，有学习别人的代码。

**写在前面：此代码主要用于巩固学习网络编程的api使用而写，比较粗糙，只实现了大概通讯的功能，由于在准备面试复习，很多细节问题都没有进行考虑，如tcp粘包等问题，因此不具备参考和实际使用价值！！！（日后会对细节问题进行补充更新）**

- 客户端用select实现，服务端用epoll实现高并发
- 用类对客户端和服务端进行封装
- 添加打印日志功能

使用：
- make server
- make client
- ./server
- ./client
