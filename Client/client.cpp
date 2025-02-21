#include "client.hpp"

bool socket_client::Connect_server(){
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
        cout << "socket create failed" << endl;
        return false;
    }
    struct sockaddr_in saddr; //服务器地址
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(ips.c_str());
    int res = connect(sockfd, (struct sockaddr*)&saddr, sizeof(saddr));
    if(res == -1){
        cout << "connect failed" << endl;
        return false;
    }
    cout << "connect success" << endl;
    return true;
}
void socket_client::print_info(){
    if(dl_flg){
        cout<<endl;
        cout<< "+------+-------------------------------------+"<<endl;
        printf("| 已登陆,用户%7s,欢迎                  |\n",username.c_str());
        cout<< "+------+-------------------------------------+"<<endl;
        cout<< "| 1.查看  2.预定  3.查看我的  4.取消  5.退出 |"<<endl;
        cout<< "+--------------------------------------------+"<<endl;
        cout<< " 请输入操作编号: ";
        cin>>user_op;
        user_op += OFFSET;
        if(user_op == 8){ //退出 + OFFSET
            user_op = EXIT;
        }
    }
    else{
        cout<<endl;
        cout<<"+--------------------------------+"<<endl;
        cout<<"| 未登陆,游客                    |"<<endl;
        cout<<"+--------------------------------+"<<endl;
        cout<<"| 1.登陆      2.注册      3.退出 |"<<endl;
        cout<<"+--------------------------------+"<<endl;
        cout<<"请输入操作编号: ";
        cin>>user_op;
    }
}
void socket_client::login(){
    string tel,passwd;
LOGINTEL:
    cout<<"请输入手机号:"<<endl;
    cin>>tel;
    if(tel.empty()){
        cout<<"手机号不能为空"<<endl;
        goto LOGINTEL;
    }
    if(tel.size() != 11){
        cout<<"请输入正确的手机号"<<endl;
        goto LOGINTEL;
    }
LOGINPWD:
    cout<<"请输入密码:"<<endl;
    cin>>passwd;
    if(passwd.empty()){
        cout<<"密码不能为空"<<endl;
        goto LOGINPWD;
    }
    Json::Value val;
    val["type"] = LOGIN;
    val["usertel"] = tel;
    val["passward"] = passwd;
    string str = val.toStyledString();
    send(sockfd, str.c_str(), str.size(), 0);
    char buff[256] = {0};
    int n = recv(sockfd, buff, 255, 0);
    if(n <= 0){
        cout<<"ser close"<<endl;
        return;
    }
    Json::Reader reader;
    Json::Value another;
    if(!reader.parse(buff, another)){
        cout<<"json解析失败"<<endl;
        return;
    }
    string res = another["status"].asString();
    if(res == "OK"){
        dl_flg = true;
        username = another["username"].asString();
        usertel = tel;
        cout<<"登陆成功"<<endl;
    }
    else{
        cout<<"登陆失败"<<endl;
        return;
    }
}
void socket_client::register_(){
    cout<<"注册区域:CN"<<endl;
REGTELAGE:
    cout<<"请输入手机号:"<<endl;
    cin>>usertel;
    if(usertel.empty()){
        cout<<"手机号不能为空"<<endl;
        goto REGTELAGE;
    }
    if(usertel.size() != 11){
        cout<<"请输入正确的手机号"<<endl;
        goto REGTELAGE;
    }
REGNMEAGE:
    cout<<"请输入用户名:"<<endl;
    cin>>username;
    if(username.empty()){
        cout<<"用户名不能为空"<<endl;
        goto REGNMEAGE;
    }
    string passward;
REGPWDAGE:
    cout<<"请输入密码:"<<endl;
    cout<<"密码长度6-16,必须包含数字,大小写字母"<<endl;
    cin>>passward;
    if(passward.size() < 6){
        cout<<"密码长度不能小于6"<<endl;
        goto REGPWDAGE;
    }
    if(passward.size() > 16){
        cout<<"密码长度不能大于16"<<endl;
        goto REGPWDAGE;
    }
    if(passward.find_first_of("0123456789") == string::npos){
        cout<<"密码必须包含数字"<<endl;
        goto REGPWDAGE;
    }
    if(passward.find_first_of("abcdefghijklmnopqrstuvwxyz") == string::npos){
        cout<<"密码必须包含小写字母"<<endl;
        goto REGPWDAGE;
    }
    if(passward.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ") == string::npos){
        cout<<"密码必须包含大写字母"<<endl;
        goto REGPWDAGE;
    }
REGCONPWD:
    cout<<"请确认密码:"<<endl;
    string confirm_passward;
    cin>>confirm_passward;
    if(passward != confirm_passward){
        cout<<"两次密码不一致"<<endl;
        cout<<"您是否记得您的密码?(Y/N)"<<endl;
        char c;
        cin>>c;
        if(c == 'Y' || c == 'N'){
            goto REGPWDAGE;
        }
        else{
            goto REGCONPWD;
        }
    }
    Json::Value val;
    val["type"] = REGISTER;
    val["username"] = username;
    val["usertel"] = usertel;
    val["passward"] = passward;

    string str = val.toStyledString();
    send(sockfd, str.c_str(), str.size(), 0);
    char buff[256] = {0};
    recv(sockfd, buff, 255, 0);
    if(strlen(buff) <= 0){
        cout<<"ser close"<<endl;
        return;
    }
    Json::Reader reader;
    Json::Value another;
    if(!reader.parse(buff, another)){
        cout<<"json解析失败"<<endl;
        return;
    }
    string res = another["status"].asString();
    if(res == "OK"){
        dl_flg = true;
        cout<<"注册成功"<<endl;
    }
    else{
        cout<<"注册失败"<<endl;
        return;
    }
}
void socket_client::exit_(){
    cout<<"exit"<<endl;
    running = false;
}
void socket_client::view(){
    Json::Value val;
    val["type"] = VIEW;
    string str = val.toStyledString();
    send(sockfd, str.c_str(), str.size(), 0);
    char buff[4096] = {0};
    int n = recv(sockfd, buff, 4095, 0);
    if(n <= 0){
        cout<<"ser close"<<endl;
        return;
    }
    m_val.clear();
    Json::Reader Read;
    if(!Read.parse(buff, m_val)){
        cout<<"json解析失败"<<endl;
        return;
    }
    string res = m_val["status"].asString();
    if(res != "OK"){
        cout<<"查看失败"<<endl;
        return;
    }
    int num = m_val["num"].asInt();
    if(num == 0){
        cout<<"暂无数据"<<endl;
        return;
    }
    cout<<"+-------------------------------------------------------+"<<endl;
    cout<<"|序号\t|名称\t\t|总票数\t|已预定\t|时间\t\t|"<<endl;
    cout<<"+-------------------------------------------------------+"<<endl;
    for(int i = 0; i < num; i++){
        cout<<"|"<<m_val["arr"][i]["tk_id"].asString()<<"\t";
        cout<<"|"<<m_val["arr"][i]["addr"].asString()<<"\t";
        cout<<"|"<<m_val["arr"][i]["max"].asString()<<"\t";
        cout<<"|"<<m_val["arr"][i]["num"].asString()<<"\t"; 
        cout<<"|"<<m_val["arr"][i]["use_date"].asString()<<"\t"<<"|"<<endl;       
        cout<<"+-------------------------------------------------------+"<<endl;
    }
}
void socket_client::order(){
    view();
    cout<<"请输入要预定的序号:"<<endl;
    int index = 0;
    cin>>index;
    
    Json::Value val;
    val["type"] = ORDER;
    val["tel"] = usertel;
    val["index"] = index;
    string str = val.toStyledString();
    send(sockfd, str.c_str(), str.size(), 0);

    char buff[256] = {0};
    int n = recv(sockfd, buff, 255, 0);
    if(n <= 0){
        cout<<"ser close"<<endl;
        return;
    }
    Json::Reader reader;
    Json::Value another;
    if(!reader.parse(buff, another)){
        cout<<"json解析失败"<<endl;
        return;
    }
    string res = another["status"].asString();
    if(res == "OK"){
        cout<<"预定成功"<<endl;
    }
    else{
        cout<<"预定失败"<<endl;
        return;
    }
}
void socket_client::view_my(){
    Json::Value val;
    val["type"] = VIEW_MY;
    val["tel"] = usertel;  // 发送用户手机号用于查询该用户的预定
    string str = val.toStyledString();
    send(sockfd, str.c_str(), str.size(), 0);

    char buff[4096] = {0};
    int n = recv(sockfd, buff, 4095, 0);
    if(n <= 0){
        cout<<"服务器断开连接"<<endl;
        return;
    }

    m_val.clear();
    Json::Reader Read;
    if(!Read.parse(buff, m_val)){
        cout<<"json解析失败"<<endl;
        return;
    }

    string res = m_val["status"].asString();
    if(res != "OK"){
        cout<<"查看失败"<<endl;
        return;
    }

    int num = m_val["num"].asInt();
    if(num == 0){
        cout<<"您还没有预定记录"<<endl;
        return;
    }
    cout<<"+---------------+-----------------------+-----------------------+"<<endl;
    cout<<"|预定编号\t|场馆名称\t\t|预定日期\t\t|"<<endl;
    cout<<"+---------------+-----------------------+-----------------------+"<<endl;
    for(int i = 0; i < num; i++){
        cout<<"|"<<m_val["arr"][i]["id"].asString()<<"\t\t";
        cout<<"|"<<m_val["arr"][i]["addr"].asString()<<"\t\t";
        cout<<"|"<<m_val["arr"][i]["use_date"].asString()<<"\t\t"<<"|"<<endl;
        cout<<"+---------------+-----------------------+-----------------------+"<<endl;
    }
}
void socket_client::cancel(){
    view_my();
    cout<<"请输入要取消的序号:"<<endl;
    int index = 0;
    cin>>index;

    Json::Value val;
    val["type"] = CANCEL;
    val["tel"] = usertel;
    val["index"] = index;
    string str = val.toStyledString();
    send(sockfd, str.c_str(), str.size(), 0);

    char buff[256] = {0};
    int n = recv(sockfd, buff, 255, 0);
    if(n <= 0){
        cout<<"服务器断开连接"<<endl;
        return;
    }
    Json::Reader reader;
    Json::Value another;
    if(!reader.parse(buff, another)){
        cout<<"json解析失败"<<endl;
        return;
    }
    string res = another["status"].asString();
    if(res == "OK"){
        cout<<"取消成功"<<endl;
    }
    else{
        cout<<"取消失败：预订序号不存在或已被取消"<<endl;
        return;
    }
}
void socket_client::Run(){
    while(running){
        print_info();
        switch(user_op){
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
                cout<<"输入错误"<<endl;
                break;
        }
    }
}
int main()
{
    socket_client cli;
    if(!cli.Connect_server()){
        exit(1);
    }
    cli.Run();
    exit(0);
}