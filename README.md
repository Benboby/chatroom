# chatroom

在学习Linux C++网络编程的过程中实现的一个小型聊天室，有学习别人的代码。

**写在前面：此代码主要用于学习网络编程的api使用而写，很多细节问题都没有进行考虑，如tcp粘包等问题，因此不具备实际使用价值！！！**

- 客户端用select实现，服务端用epoll实现高并发
- 用类对客户端和服务端进行封装
- 添加打印日志功能

使用：
- make server
- make client
- ./server
- ./client
