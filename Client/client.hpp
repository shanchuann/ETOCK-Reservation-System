#ifdef _WIN32
// Windows 特定的代码
#include <windows.h>
#include<iostream>
#include<string.h>
#include<string>
#include <io.h>
#include <process.h>
#include<stdlib.h>
#include <WinSock2.h>
#include<event.h>
#include <json/json.h>
#include <mysql.h>
#elif __linux__
// Linux 特定的代码
#include <iostream>
#include <string.h>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <event.h>
#include <jsoncpp/json/json.h>
#include <mysql/mysql.h>
#else
#error "Unsupported operating system"
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