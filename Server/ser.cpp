#include "ser.hpp"
//--mysql_client
bool mysql_client::mysql_ConnectServer(){
    MYSQL* mysql = mysql_init(&mysql_con);
    if(mysql == NULL){
        cout << "mysql init failed" << endl;
        return false;
    }
    mysql = mysql_real_connect(mysql, db_ips.c_str(), db_username.c_str(), db_passwd.c_str(), db_dbname.c_str(), 0, NULL, 0);
    if(mysql == NULL){
        cout << "mysql connect failed" << endl;
        return false;
    }
    return true;
}
bool mysql_client::mysql_register(const string &username, const string &passwd, const string &usertel){
    string sql = "insert into user_info values(0,'";
    sql += usertel;
    sql += "','";
    sql += username;
    sql += "','";
    sql += passwd;
    sql += "',1)";
    //sql = "insert into user_info values(0,'12345678901','zbk','123456',1)";
    int res = mysql_query(&mysql_con, sql.c_str());
    if(res != 0){
        cout << "mysql query failed" << endl;
        return false;
    }
    return true;
}
bool mysql_client::mysql_login(const string &usertel, const string &passwd, string &username){
    string sql = "select uesrname,passwd from user_info where tel = '";
    sql += usertel;
    sql += "'";
    //sql = "select uesrname,passwd from user_info where tel = '12345678901'";
    int res = mysql_query(&mysql_con, sql.c_str());
    if(res != 0){
        cout << "mysql query failed" << endl;
        return false;
    }
    MYSQL_RES* res_ptr = mysql_store_result(&mysql_con);
    if(res_ptr == NULL){
        cout << "mysql store result failed" << endl;
        return false;
    }
    int num = mysql_num_rows(res_ptr);
    if(num != 1){
        cout << "mysql num rows failed" << endl;
        mysql_free_result(res_ptr);
        return false;
    }
    MYSQL_ROW row = mysql_fetch_row(res_ptr);
    string pass = row[1];
    if(pass != passwd){
        cout << "passwd error" << endl;
        mysql_free_result(res_ptr);
        return false;
    }
    username = row[0];
    return true;
}
bool mysql_client::mysql_Show_Ticket(Json::Value &resval){
    string sql = "select tk_id,addr,max,num,use_date from ticket_info";
    if(mysql_query(&mysql_con, sql.c_str()) != 0){
        cout << "mysql query failed" << endl;
        return false;
    }
    MYSQL_RES* res_ptr = mysql_store_result(&mysql_con);
    if(res_ptr == NULL){
        cout << "mysql store result failed" << endl;
        return false;
    }
    
    int num = mysql_num_rows(res_ptr);
    if(num == 0){
        resval["status"] = "OK";
        resval["num"] = 0;
        return true;
    }
    resval["status"] = "OK";
    resval["num"] = num;
    for(int i = 0; i < num; i++){
        MYSQL_ROW row = mysql_fetch_row(res_ptr);
        Json::Value tmp;
        tmp["tk_id"] = row[0];
        tmp["addr"] = row[1];
        tmp["max"] = row[2];
        tmp["num"] = row[3];
        tmp["use_date"] = row[4];
        resval["arr"].append(tmp);
    }
    return true;
}
bool mysql_client::mysql_Show_My_Ticket(Json::Value &resval, const string &tel){
    string sql = "select yd_id,ticket_info.tk_id, addr,use_date from ticket_info,sub_ticket where sub_ticket.tel = '";
    sql += tel;
    sql += "' and sub_ticket.tk_id = ticket_info.tk_id";
    
    if(mysql_query(&mysql_con, sql.c_str()) != 0){
        cout << "mysql query failed" << endl;
        return false;
    }
    MYSQL_RES* res_ptr = mysql_store_result(&mysql_con);
    if(res_ptr == NULL){
        cout << "mysql store result failed" << endl;
        return false;
    }
    int num = mysql_num_rows(res_ptr);
    if(num == 0){
        resval["status"] = "OK";
        resval["num"] = 0;
        return true;
    }
    resval["status"] = "OK";
    resval["num"] = num;
    for(int i = 0; i < num; i++){
        MYSQL_ROW row = mysql_fetch_row(res_ptr);
        Json::Value tmp;
        tmp["id"] = row[0];
        tmp["tk_id"] = row[1];
        tmp["addr"] = row[2];
        tmp["use_date"] = row[3];
        resval["arr"].append(tmp);
    }
    return true;
}
bool mysql_client::mysql_Cancel_Ticket(int index, const string &tel){
    // 检查预订序号是否存在
    string check_sql = "select count(*) from sub_ticket where yd_id = ";
    check_sql += to_string(index);
    
    if(mysql_query(&mysql_con, check_sql.c_str()) != 0){
        cout << "check yd_id query failed" << endl;
        return false;
    }
    
    MYSQL_RES* check_res = mysql_store_result(&mysql_con);
    if(check_res == NULL){
        cout << "check result failed" << endl;
        return false;
    }
    
    MYSQL_ROW check_row = mysql_fetch_row(check_res);
    int exists = atoi(check_row[0]);
    mysql_free_result(check_res);
    
    if(exists == 0){
        cout << "yudingbucunzai"<< endl;
        return false;
    }

    // 取消预订
    string sql = "select tk_id from sub_ticket where yd_id = ";
    sql += to_string(index);
    sql += " and tel = '";
    sql += tel;
    sql += "'";
    if(mysql_query(&mysql_con, sql.c_str()) != 0){
        cout << "mysql query failed" << endl;
        return false;
    }
    MYSQL_RES* res_ptr = mysql_store_result(&mysql_con);
    if(res_ptr == NULL){
        cout << "mysql store result failed" << endl;
        return false;
    }
    int num = mysql_num_rows(res_ptr);
    if(num != 1){
        cout << "mysql num rows failed" << endl;
        return false;
    }
    MYSQL_ROW row = mysql_fetch_row(res_ptr);
    int tk_id = atoi(row[0]);
    mysql_user_begin();
    string s1 = string("select max,num from ticket_info where tk_id = ") + to_string(tk_id);
    if(mysql_query(&mysql_con, s1.c_str()) != 0){
        cout << "max,num failed" << endl;
        mysql_user_rollback();
        return false;
    }
    res_ptr = mysql_store_result(&mysql_con);
    if(res_ptr == NULL){
        cout << "result failed" << endl;
        mysql_user_rollback();
        return false;
    }
    num = mysql_num_rows(res_ptr);
    if(num != 1){
        cout << "num failed" << endl;
        mysql_user_rollback();
        return false;
    }
    row = mysql_fetch_row(res_ptr);
    string str_max = row[0];//总票数
    string str_num = row[1];//已预定
    int tk_max = atoi(str_max.c_str());
    int tk_num = atoi(str_num.c_str());
    if(tk_num <= 0){
        cout << "no ticket have" << endl;
        mysql_user_rollback();
        return false;
    }
    tk_num--;
    string s2 = string("update ticket_info set num = ") + to_string(tk_num) + string(" where tk_id = ") + to_string(tk_id);
    if(mysql_query(&mysql_con, s2.c_str()) != 0){
        cout << "update failed" << endl;
        mysql_user_rollback();
        return false;
    }
    string s3 = string("delete from sub_ticket where yd_id = ")
                + to_string(index) + string(" and tel = '") + tel + string("'");
    if(mysql_query(&mysql_con, s3.c_str()) != 0){
        cout << "delete failed" << endl;
        mysql_user_rollback();
        return false;
    }
    mysql_user_commit();
    return true;
}
bool mysql_client::mysql_user_begin(){
    if(mysql_query(&mysql_con, "begin") != 0){
        cout << "mysql begin failed" << endl;
        return false;
    }
    return true;
}
bool mysql_client::mysql_user_commit(){
    if(mysql_query(&mysql_con, "commit") != 0){
        cout << "mysql commit failed" << endl;
        return false;
    }
    return true;
}
bool mysql_client::mysql_user_rollback(){
    if(mysql_query(&mysql_con, "rollback") != 0){
        cout << "mysql rollback failed" << endl;
        return false;
    }
    return true;
}
bool mysql_client::mysql_Subscribe_Ticket(int tk_id, string tel){
    string check_sql = "select count(*) from ticket_info where tk_id = ";
    check_sql += to_string(tk_id);
    
    if(mysql_query(&mysql_con, check_sql.c_str()) != 0){
        cout << "check tk_id query failed" << endl;
        return false;
    }
    
    MYSQL_RES* check_res = mysql_store_result(&mysql_con);
    if(check_res == NULL){
        cout << "check result failed" << endl;
        return false;
    }
    
    MYSQL_ROW check_row = mysql_fetch_row(check_res);
    int exists = atoi(check_row[0]);
    mysql_free_result(check_res);
    
    if(exists == 0){
        cout << "xuhaobucunzai" << endl;
        return false;
    }
    
    mysql_user_begin();//开启事务
    string s1 = string("select max,num from ticket_info where tk_id = ") + to_string(tk_id);
    if(mysql_query(&mysql_con, s1.c_str()) != 0){
        cout << "max,num failed" << endl;
        mysql_user_rollback();
        return false;
    }
    MYSQL_RES* res_ptr = mysql_store_result(&mysql_con);
    if(res_ptr == NULL){
        cout << "result failed" << endl;
        mysql_user_rollback();
        return false;
    }
    int num = mysql_num_rows(res_ptr);
    if(num != 1){
        cout << "num failed" << endl;
        mysql_user_rollback();
        return false;
    }
    MYSQL_ROW row = mysql_fetch_row(res_ptr);
    string str_max = row[0];//总票数
    string str_num = row[1];//已预定
    int tk_max = atoi(str_max.c_str());
    int tk_num = atoi(str_num.c_str());
    if(tk_max <= tk_num){
        cout << "no ticket have" << endl;
        mysql_user_rollback();
        return false;
    }
    tk_num++;
    string s2 = string("update ticket_info set num = ") + to_string(tk_num) + string(" where tk_id = ") + to_string(tk_id);
    if(mysql_query(&mysql_con, s2.c_str()) != 0){
        cout << "update failed" << endl;
        mysql_user_rollback();
        return false;
    }
    string s3 = string("insert into sub_ticket values(0,") + to_string(tk_id) + string(",'") + tel + string("',now())");
    if(mysql_query(&mysql_con, s3.c_str()) != 0){
        cout << "insert failed" << endl;
        mysql_user_rollback();
        return false;
    }
    mysql_user_commit();
    return true;
}   
//--socket_listen
bool socket_listen::socket_init() {
#ifdef _WIN32
    // Windows 平台初始化 Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSAStartup failed" << endl;
        return false;
    }
#endif

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
#ifdef _WIN32
    if (sockfd == INVALID_SOCKET) {
#else
    if (sockfd == -1) {
#endif
        cout << "socket create failed" << endl;
#ifdef _WIN32
        WSACleanup();
#endif
        return false;
    }

    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(m_port);
    saddr.sin_addr.s_addr = inet_addr(m_ips.c_str());

    int res = bind(sockfd, (struct sockaddr*)&saddr, sizeof(saddr));
#ifdef _WIN32
    if (res == SOCKET_ERROR) {
#else
    if (res == -1) {
#endif
        cout << "bind failed" << endl;
#ifdef _WIN32
        closesocket(sockfd);
        WSACleanup();
#else
        close(sockfd);
#endif
        return false;
    }

    res = listen(sockfd, LIS_MAX);
#ifdef _WIN32
    if (res == SOCKET_ERROR) {
#else
    if (res == -1) {
#endif
        cout << "listen failed" << endl;
#ifdef _WIN32
        closesocket(sockfd);
        WSACleanup();
#else
        close(sockfd);
#endif
        return false;
    }
    return true;
}
int socket_listen::accept_client() {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int c = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
#ifdef _WIN32
    if (c == INVALID_SOCKET) {
#else
    if (c == -1) {
#endif
        cout << "accept failed" << endl;
        return -1;
    }
    return c;
}
//--socket_connect
void socket_connect::Send_err(){
    Json::Value val;
    val["status"] = "ERR";
    send(c, val.toStyledString().c_str(), val.toStyledString().size(), 0);
}
void socket_connect::Send_ok(){
    Json::Value val;
    val["status"] = "OK";
    send(c, val.toStyledString().c_str(), val.toStyledString().size(), 0);
}
void socket_connect::login(){
    string tel,passwd;
    string user_name;
    tel = val["usertel"].asString();
    passwd = val["passward"].asString();
    if(tel.empty() || passwd.empty()){
        cout << "login:empty" << endl;
        Send_err();
        return;
    }
    if(tel.size() != 11){
        cout << "login:tel" << endl;
        Send_err();
        return;
    }
    if(passwd.size() < 6 || passwd.size() > 16){
        cout << "login:passwd" << endl;
        Send_err();
        return;
    }
    mysql_client cli;
    if(!cli.mysql_ConnectServer()){
        cout << "mysql connect failed" << endl;
        Send_err();
        return;
    }
    if(!cli.mysql_login(tel, passwd, user_name)){
        cout << "mysql login failed" << endl;
        Send_err();
        return;
    }
    Json::Value val;
    val["status"] = "OK";
    val["username"] = user_name;
    send(c, val.toStyledString().c_str(), val.toStyledString().size(), 0);
    return;
}
void socket_connect::register_(){
    string tel,passwd,name;
    tel = val["usertel"].asString();
    passwd = val["passward"].asString();
    name = val["username"].asString();
    if(tel.empty() || passwd.empty() || name.empty()){
        cout << "register:empty" << endl;
        Send_err();
        return;
    }
    if(tel.size() != 11){
        cout << "register:tel" << endl;
        Send_err();
        return;
    }
    if(passwd.size() < 6 || passwd.size() > 16){
        cout << "register:passwd" << endl;
        Send_err();
        return;
    }
    mysql_client cli;
    if(!cli.mysql_ConnectServer()){
        cout << "mysql connect failed" << endl;
        Send_err();
        return;
    }
    if(!cli.mysql_register(name, passwd, tel)){
        cout << "mysql register failed" << endl;
        Send_err();
        return;
    }
    Send_ok();
    return;
}
void socket_connect::exit_(){
    cout << "exit" << endl;
}
void socket_connect::view(){
    Json::Value resval;
    mysql_client cli;
    if(!cli.mysql_ConnectServer()){
        cout << "mysql connect failed" << endl;
        Send_err();
        return;
    }
    if(!cli.mysql_Show_Ticket(resval)){
        cout << "mysql show ticket failed" << endl;
        Send_err();
        return;
    }
    send(c, resval.toStyledString().c_str(), resval.toStyledString().size(), 0);
    return;
}
void socket_connect::order(){
    //client --> tk_id,tel
    int tk_id = val["index"].asInt();
    string tel = val["tel"].asString();
    if(tel.empty() || tk_id < 0){
        cout << "order:empty" << endl;
        Send_err();
        return;
    }
    mysql_client cli;
    if(!cli.mysql_ConnectServer()){
        cout << "mysql connect failed" << endl;
        Send_err();
        return;
    }
    if(!cli.mysql_Subscribe_Ticket(tk_id, tel)){
        cout << "mysql subscribe ticket failed" << endl;
        Send_err();
        return;
    }
    Send_ok();
    return;
}
void socket_connect::view_my(){
    Json::Value resval;
    mysql_client cli;
    if(!cli.mysql_ConnectServer()){
        cout << "mysql connect failed" << endl;
        Send_err();
        return;
    }
    if(!cli.mysql_Show_My_Ticket(resval, val["tel"].asString())){
        cout << "mysql show my ticket failed" << endl;
        Send_err();
        return;
    }
    send(c, resval.toStyledString().c_str(), resval.toStyledString().size(), 0);
    return;
}
void socket_connect::cancel(){
    int index = val["index"].asInt();
    string tel = val["tel"].asString();
    
    mysql_client cli;
    if(!cli.mysql_ConnectServer()){
        cout << "mysql connect failed" << endl;
        Send_err();
        return;
    }
    if(!cli.mysql_Cancel_Ticket(index, tel)){
        cout << "mysql cancel ticket failed" << endl;
        Send_err();
        return;
    }
    Send_ok();
}

void socket_connect::Recv_data(){
    char buf[256] = {0};
    int len = recv(c, buf, sizeof(buf), 0);
    if(len <= 0){
        cout << "client close" << endl;
        delete this;
        return;
    }
    //测试
    cout << "recv: " << buf << endl;
    Json::Reader reader;
    if(!reader.parse(buf, val)){
        cout << "Recv_data:Json解析失败" << endl;
        Send_err();
    }
    int ops = val["type"].asInt();
    switch(ops){
        case LOGIN:
            login();
            break;
        case REGISTER:
            register_();
            break;
        case EXIT:
            exit_();
            break;
        case VIEW:
            view();
            break;
        case ORDER:
            order();
            break;
        case VIEW_MY:
            view_my();
            break;
        case CANCEL:
            cancel();
            break;
        default:
            cout << "error" << endl;
            break;
    }
    //解析

    //Json::Value val;
    //val["status"] = "OK";
    //send(c, val.toStyledString().c_str(), val.toStyledString().size(), 0);
}
//--callback
#if _WIN32
void SOCK_CON_CALLBACK(intptr_t sockfd, short event, void* arg){
#elif __linux__
void SOCK_CON_CALLBACK(int sockfd, short event, void* arg) {
#endif
    socket_connect* q = (socket_connect*)arg;
    if(event & EV_READ){
        q->Recv_data();
    }
}
#if _WIN32
void SOCK_LIS_CALLBACK(intptr_t sockfd, short event, void* arg){
#elif __linux__
void SOCK_LIS_CALLBACK(int sockfd, short event, void* arg) {
#endif
    socket_listen* p = (socket_listen*)arg;
    if(p == NULL){
        cout << "arg is NULL" << endl;
        return;
    }
    //处理读事件
    if(event & EV_READ){
        int c = p->accept_client();
        if(c == -1){
            cout << "accept failed" << endl;
            return;
        }
        cout << "accept a client" << endl;
        //创建一个新的event
        socket_connect* q = new socket_connect(c);
        struct event* c_ev = event_new(p->Get_base(), c, EV_READ|EV_PERSIST, SOCK_CON_CALLBACK, q);
        if(c_ev == NULL){
            cout << "event new failed" << endl;
#if _WIN32
            _close(c);
#elif __linux__
			close(c);
#endif
            delete q;
            return;
        }
        q->Set_ev(c_ev);
        //添加事件
        event_add(c_ev, NULL);
    }
}

int main(void) {
    // 监听套接字
    socket_listen sock_ser;
    if (!sock_ser.socket_init()) {
        cout << "socket init failed" << endl;
        exit(1);
    }

    // 创建 libevent base
    struct event_base* base = event_init();
    if (base == NULL) {
        cout << "event init failed" << endl;
#ifdef _WIN32
        WSACleanup(); // Windows 需要清理 Winsock
#endif
        exit(1);
    }

    // 设置 socket_listen 的 base
    sock_ser.Set_base(base);

    // 添加 sockfd 到 libevent
    struct event* sock_ev = event_new(base, sock_ser.Get_sockfd(), EV_READ | EV_PERSIST, SOCK_LIS_CALLBACK, &sock_ser);
    if (sock_ev == NULL) {
        cout << "event_new failed" << endl;
#ifdef _WIN32
        WSACleanup(); // Windows 需要清理 Winsock
#endif
        event_base_free(base);
        exit(1);
    }

    event_add(sock_ev, NULL);

    // 启动事件循环
    event_base_dispatch(base);

    // 释放资源
    event_free(sock_ev);    // 释放事件
    event_base_free(base);  // 释放 base

#ifdef _WIN32
    WSACleanup(); // Windows 需要清理 Winsock
#endif

    return 0;
}