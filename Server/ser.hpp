#ifndef CROSS_PLATFORM_PROJECT_H
#define CROSS_PLATFORM_PROJECT_H

// 平台检测
#ifdef _WIN32
    // Windows 平台
#include <windows.h>
#include <winsock2.h>
#include <io.h>
#include <process.h>
#include <mysql.h>
#pragma comment(lib, "ws2_32.lib") // 链接 Winsock 库
#pragma comment(lib, "libmysql.lib") // 链接 MySQL 库
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
// 常量定义
const int LIS_MAX = 10;

// 枚举类型
enum OP_TYPE {
    LOGIN = 1, // 登陆
    REGISTER,  // 注册
    EXIT,      // 退出
    VIEW,      // 查看
    ORDER,     // 预定
    VIEW_MY,   // 查看我的预定
    CANCEL     // 取消预定
};

// 类声明
class mysql_client {
public:
    mysql_client();
    ~mysql_client();
    bool mysql_ConnectServer();
    bool mysql_register(const std::string& username, const std::string& passwd, const std::string& usertel);
    bool mysql_login(const std::string& usertel, const std::string& passwd, std::string& username);
    bool mysql_Show_Ticket(Json::Value& resval);
    bool mysql_Subscribe_Ticket(int tk_id, std::string tel);
    bool mysql_Show_My_Ticket(Json::Value& resval, const std::string& tel);
    bool mysql_Cancel_Ticket(int index, const std::string& tel);

private:
    bool mysql_user_begin();
    bool mysql_user_commit();
    bool mysql_user_rollback();
    MYSQL mysql_con;
    std::string db_ips;
    std::string db_username;
    std::string db_dbname;
    std::string db_passwd;
};

class socket_listen {
public:
    socket_listen();
    socket_listen(std::string ips, short port);
    bool socket_init();
    int accept_client();
    void Set_base(struct event_base* b);
    struct event_base* Get_base() const;
    int Get_sockfd() const;

private:
    int sockfd;
    short m_port;
    std::string m_ips;
    struct event_base* base;
};

class socket_connect {
public:
    socket_connect(int fd);
    int Get_fd() const;
    void Set_ev(struct event* ev);
    ~socket_connect();
    void Recv_data();
    void Send_err();
    void Send_ok();
    void login();
    void register_();
    void exit_();
    void view();
    void order();
    void view_my();
    void cancel();

private:
    int c;
    struct event* c_ev;
    Json::Value val;
};

#endif // CROSS_PLATFORM_PROJECT_H