#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H

// 平台检测
#ifdef _WIN32
    // Windows 平台
#include <windows.h>
#include <winsock2.h>
#include <io.h>
#include <process.h>
#include <mysql.h>
#pragma comment(lib, "ws2_32.lib") // 链接 Winsock 库
#elif __linux__
    // Linux 平台
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <mysql/mysql.h>
#else
#error "Unsupported operating system"
#endif

// 公共头文件
#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>

// 第三方库头文件
#ifdef _WIN32
#include <event2/event.h> // Windows 上的 libevent
#include <json/json.h>    // Windows 上的 jsoncpp
#elif __linux__
#include <event.h>        // Linux 上的 libevent
#include <jsoncpp/json/json.h> // Linux 上的 jsoncpp
#endif
using namespace std;
const int OFFSET = 3;

enum OP_TYPE{
    LOGIN = 1,//登陆
    REGISTER, //注册
    EXIT,     //退出
    VIEW,     //查看
    ORDER,    //预定
    VIEW_MY,  //查看我的预定
    CANCEL    //取消预定 
};
class socket_client{
public:
    socket_client(){
        sockfd = -1;
        ips = "127.0.0.1";
        port = 6000;
        dl_flg = false;
        user_op = 0;
        running = true;
    }
    socket_client(string ips, short port){
        this->ips = ips;
        this->port = port;
        sockfd = -1;
        dl_flg = false;
        user_op = 0;
        running = true;
    }
    void print_info();
    ~socket_client(){
        close(sockfd);
    }
    bool Connect_server();
    void Run();
    void login();
    void register_();
    void exit_();
    void view();
    void order();
    void view_my();
    void cancel();
private:
    string ips;
    short port;
    int sockfd;
    bool dl_flg;
    bool running;
    string username;
    string usertel;

    int user_op;

    Json::Value m_val;
    //最大的预定数
    Json::Value m_max;
};
#endif // SOCKET_CLIENT_H