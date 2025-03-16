#ifndef CROSS_PLATFORM_PROJECT_H
#define CROSS_PLATFORM_PROJECT_H
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#ifdef _WIN32
    // Windows 平台
#include <winsock2.h>
#include <windows.h>
#include <io.h>
#include <process.h>
#include "D:/Server/mysql-9.2.0/mysql-9.2.0-winx64/include/mysql.h"
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
#include <D:/cpp language/ETOCK-Reservation-System/libevent/include/event.h> // Windows 上的 libevent
#include <D:/jsoncpp/include/json/json.h>    // Windows 上的 jsoncpp
#elif __linux__
#include <event.h>        // Linux 上的 libevent
#include <jsoncpp/json/json.h> // Linux 上的 jsoncpp
#endif
using namespace std;

const int LIS_MAX = 10;

enum OP_TYPE {
    LOGIN = 1,//登陆
    REGISTER, //注册
    EXIT,     //退出
    VIEW,     //查看
    ORDER,    //预定
    VIEW_MY,  //查看我的预定
    CANCEL    //取消预定 
};

class mysql_client {
public:
#ifdef _Linux
    mysql_client() {
        db_ips = "127.0.0.1";
        db_username = "root";
        db_dbname = "Project_DB";
        db_passwd = "zbk";
    }
#elif _WIN32
    mysql_client() {
        db_ips = "127.0.0.1";
        db_username = "root";
        db_dbname = "Project_DB";
        db_passwd = "zbk20050702";
    }
#endif
    ~mysql_client() {
        mysql_close(&mysql_con);
    }
    bool mysql_ConnectServer();
    bool mysql_register(const string& username, const string& passwd, const string& usertel);
    bool mysql_login(const string& usertel, const string& passwd, string& username);
    bool mysql_Show_Ticket(Json::Value& resval);
    bool mysql_Subscribe_Ticket(int tk_id, string tel);
    bool mysql_Show_My_Ticket(Json::Value& resval, const string& tel);
    bool mysql_Cancel_Ticket(int index, const string& tel);

private:
    bool mysql_user_begin();
    bool mysql_user_commit();
    bool mysql_user_rollback();
    MYSQL mysql_con;
    string db_ips;
    string db_username;
    string db_dbname;
    string db_passwd;
};

class socket_listen {
public:
    socket_listen() {
        sockfd = -1;
        m_port = 6000;
        m_ips = "127.0.0.1";
    }
    socket_listen(string ips, short port) {
        m_ips = ips;
        m_port = port;
        sockfd = -1;
    }
    bool socket_init();
    int accept_client();

    void Set_base(struct event_base* b) {
        base = b;
    }
    struct event_base* Get_base() const {
        return base;
    }
    int Get_sockfd() const {
        return sockfd;
    }

private:
    int sockfd;
    short m_port;
    string m_ips;
    struct event_base* base;
};

class socket_connect {
public:
    socket_connect(int fd) {
        c = fd;
        c_ev = NULL;
    }
    int Get_fd() const {
        return c;
    }
    void Set_ev(struct event* ev) {
        c_ev = ev;
    }
    ~socket_connect() {
        event_free(c_ev);
#ifdef _WIN32
        closesocket(c); // Windows 使用 closesocket
#else
        close(c); // Linux 使用 close
#endif
    }
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