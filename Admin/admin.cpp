#include "admin.hpp"

bool AdminManager::ConnectDB() {
    mysql_init(&mysql);
    if(!mysql_real_connect(&mysql, db_ip.c_str(), db_user.c_str(), 
                          db_passwd.c_str(), db_name.c_str(), 0, NULL, 0)) {
        cout << "连接数据库失败!" << endl;
        return false;
    }
    cout << "连接数据库成功!" << endl;
    return true;
}

void AdminManager::PrintMenu() {
    cout << "\n+--------------------------------+" << endl;
    cout << "| 管理员操作界面                 |" << endl;
    cout << "+--------------------------------+" << endl;
    cout << "| 1.添加门票                     |" << endl;
    cout << "| 2.查看所有门票                 |" << endl;
    cout << "| 3.查看所有用户                 |" << endl;
    cout << "| 4.查看黑名单用户               |" << endl;
    cout << "| 5.将用户加入黑名单             |" << endl;
    cout << "| 6.将用户移出黑名单             |" << endl;
    cout << "| 7.退出                         |" << endl;
    cout << "+--------------------------------+" << endl;
    cout << "请输入操作编号: ";
}

void AdminManager::AddTicket() {
    string addr;
    int max;
    string use_date;
    
    cout << "\n添加门票信息:" << endl;
    cout << "场馆名称:";
    cin >> addr;
    
    cout << "总票数:";
    cin >> max;
    
    cout << "使用日期(YYYY-MM-DD):";
    cin >> use_date;
    
    string sql = "INSERT INTO ticket_info VALUES(0, '";
    sql += addr + "', " + to_string(max) + ", 0, '";
    sql += use_date + "', 1)";
    
    if(mysql_query(&mysql, sql.c_str()) != 0) {
        cout << "添加门票失败!" << endl;
        return;
    }
    cout << "添加门票成功!" << endl;
}

void AdminManager::ViewAllTickets() {
    string sql = "SELECT * FROM ticket_info";
    if(mysql_query(&mysql, sql.c_str()) != 0) {
        cout << "查询失败!" << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(&mysql);
    if(res == NULL) {
        cout << "获取结果失败!" << endl;
        return;
    }

    cout << "\n当前所有门票信息:" << endl;
    cout << "+-------+----------------+--------+------+---------------+-------+" << endl;
    cout << "|门票ID |场馆名称        |总票数 |已订票 |   使用日期    |状态   |" << endl;
    cout << "+-------+----------------+--------+------+---------------+-------+" << endl;

    MYSQL_ROW row;
    while((row = mysql_fetch_row(res))) {
        printf("|%-7s|%-20s|%-9s|%-6s|%-10s|%-7s|\n", row[0], row[1], row[2], row[3], row[4], row[5]);
        cout << "+-------+----------------+--------+------+---------------+-------+" << endl;
    }
    
    mysql_free_result(res);
}

void AdminManager::ViewAllUsers() {
    string sql = "SELECT * FROM user_info";
    if(mysql_query(&mysql, sql.c_str()) != 0) {
        cout << "查询失败!" << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(&mysql);
    if(res == NULL) {
        cout << "获取结果失败!" << endl;
        return;
    }

    cout << "\n所有用户信息:" << endl;
    cout << "+--------+-------------+------------+--------+--------+" << endl;
    cout << "|用户ID  |   手机号    |   用户名   |  密码  | 状态   |" << endl;
    cout << "+--------+-------------+------------+--------+--------+" << endl;

    MYSQL_ROW row;
    while((row = mysql_fetch_row(res))) {
        string status = (string(row[4]) == "1") ? "正常" : "黑名单";
        printf("|%-8s|%-13s|%-12s|%-8s|%-8s  |\n", 
               row[0], row[1], row[2], row[3], status.c_str());
        cout << "+--------+-------------+------------+--------+--------+" << endl;
    }
    
    mysql_free_result(res);
}

void AdminManager::ViewBlacklist() {
    string sql = "SELECT * FROM user_info WHERE status = 0";
    if(mysql_query(&mysql, sql.c_str()) != 0) {
        cout << "查询失败!" << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(&mysql);
    if(res == NULL) {
        cout << "获取结果失败!" << endl;
        return;
    }

    cout << "\n黑名单用户:" << endl;
    cout << "+--------+-------------+------------+" << endl;
    cout << "|用户ID  |   手机号    |   用户名   |" << endl;
    cout << "+--------+-------------+------------+" << endl;

    MYSQL_ROW row;
    while((row = mysql_fetch_row(res))) {
        printf("|%-8s|%-13s|%-12s|\n", row[0], row[1], row[2]);
        cout << "+--------+-------------+------------+" << endl;
    }
    
    mysql_free_result(res);
}

void AdminManager::AddToBlacklist() {
    string tel;
    cout << "请输入要加入黑名单的用户手机号: ";
    cin >> tel;

    // 先检查用户是否存在且状态为正常
    string check_sql = "SELECT status FROM user_info WHERE tel = '" + tel + "'";
    if(mysql_query(&mysql, check_sql.c_str()) != 0) {
        cout << "查询失败!" << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(&mysql);
    if(res == NULL) {
        cout << "获取结果失败!" << endl;
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    if(!row) {
        cout << "该用户不存在!" << endl;
        mysql_free_result(res);
        return;
    }

    if(string(row[0]) == "0") {
        cout << "该用户已经在黑名单中!" << endl;
        mysql_free_result(res);
        return;
    }

    mysql_free_result(res);

    // 更新用户状态为黑名单
    string sql = "UPDATE user_info SET status = 0 WHERE tel = '" + tel + "'";
    if(mysql_query(&mysql, sql.c_str()) != 0) {
        cout << "加入黑名单失败!" << endl;
        return;
    }
    cout << "已将用户加入黑名单!" << endl;
}

void AdminManager::RemoveFromBlacklist() {
    string tel;
    cout << "请输入要移出黑名单的用户手机号: ";
    cin >> tel;

    // 先检查用户是否存在且在黑名单中
    string check_sql = "SELECT status FROM user_info WHERE tel = '" + tel + "'";
    if(mysql_query(&mysql, check_sql.c_str()) != 0) {
        cout << "查询失败!" << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(&mysql);
    if(res == NULL) {
        cout << "获取结果失败!" << endl;
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    if(!row) {
        cout << "该用户不存在!" << endl;
        mysql_free_result(res);
        return;
    }

    if(string(row[0]) == "1") {
        cout << "该用户不在黑名单中!" << endl;
        mysql_free_result(res);
        return;
    }

    mysql_free_result(res);

    // 更新用户状态为正常
    string sql = "UPDATE user_info SET status = 1 WHERE tel = '" + tel + "'";
    if(mysql_query(&mysql, sql.c_str()) != 0) {
        cout << "移出黑名单失败!" << endl;
        return;
    }
    cout << "已将用户移出黑名单!" << endl;
}

void AdminManager::Run() {
    int choice;
    while(running) {
        PrintMenu();
        cin >> choice;
        
        switch(choice) {
            case ADD_TICKET:
                AddTicket();
                break;
            case VIEW_TICKETS:
                ViewAllTickets();
                break;
            case VIEW_USERS:
                ViewAllUsers();
                break;
            case VIEW_BLACKLIST:
                ViewBlacklist();
                break;
            case ADD_TO_BLACKLIST:
                AddToBlacklist();
                break;
            case REMOVE_FROM_BLACKLIST:
                RemoveFromBlacklist();
                break;
            case EXIT:
                running = false;
                cout << "退出管理系统!" << endl;
                break;
            default:
                cout << "无效的选择!" << endl;
                break;
        }
    }
}

int main() {
    AdminManager admin;
    if(!admin.ConnectDB()) {
        return 1;
    }
    admin.Run();
    return 0;
}
