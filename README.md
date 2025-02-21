# ETOCK Reservation System (Easy To Check预约系统)

> 基于C++实现的票务预约系统，支持用户注册、登录、票务查询、预订及取消功能  
> 系统依赖：`libevent`, `jsoncpp`, `mysqlclient`  
> 当前版本：v1.0 (仅支持Linux系统)

---

## 协议设计

### 请求/响应格式
- **数据格式**: JSON  
- **操作类型** (`type`字段):  
  | 类型       | 值  | 说明               |
  |-----------|-----|-------------------|
  | LOGIN     | 1   | 用户登录           |
  | REGISTER  | 2   | 用户注册           |
  | EXIT      | 3   | 退出系统           |
  | VIEW      | 4   | 查看所有票务       |
  | ORDER     | 5   | 预定票务           |
  | VIEW_MY   | 6   | 查看个人预定记录   |
  | CANCEL    | 7   | 取消预定           |

---

## 模块说明

### Server 服务器
#### 功能
1. 使用 `libevent` 实现高并发网络通信
2. 通过MySQL管理用户数据 (`user_info`)、票务数据 (`ticket_info`)、预定记录 (`sub_ticket`)
3. 处理客户端请求，返回JSON格式响应

#### 核心类
- **`mysql_client`**: 封装数据库操作  
  - `mysql_ConnectServer()`: 连接数据库  
  - `mysql_login()/mysql_register()`: 用户登录/注册  
  - `mysql_Subscribe_Ticket()/mysql_Cancel_Ticket()`: 票务预定/取消  
  - 事务管理: `mysql_user_begin()`, `mysql_user_commit()`, `mysql_user_rollback()`

- **`socket_listen`**: 监听端口，接受客户端连接  
- **`socket_connect`**: 处理客户端请求，解析JSON并调用对应方法

#### 数据库表结构

```sql
-- 用户表
create table user_info(
    userid int primary key not null unique auto_increment,
    tel char(11) not null unique,
    uesrname varchar(20) not null,
    passwd varchar(17) not null,
    status tinyint not null
);

-- 票务表
create table ticket_info(
    tk_id int primary key not null unique auto_increment,
    addr varchar(20),
    max int, 
    num int,
    use_date date,
    status TINYINT
);

-- 预定记录表
create table sub_ticket(
    yd_id int primary key not null unique auto_increment,
    tk_id int not null,
    tel char(11) not null,
    curr_time DATETIME not null
);
```
#### 数据库表

##### 用户表

<img src=".\assets\user_info.png" alt="用户表" style="zoom:200%;" />

##### 票务表

<img src=".\assets\ticket_info.png" alt="票务表" style="zoom: 200%;" />

##### 预定记录表

<img src=".\assets\sub_ticket.png" alt="预定记录" style="zoom: 200%;" />

### Client 客户端

#### 功能
1. 提供命令行交互界面
2. 发送JSON请求，解析服务器响应
3. 支持两种界面状态：
   - **未登录**: 显示登录/注册选项
   - **已登录**: 显示票务操作选项

#### 核心方法
- `Connect_server()`: 连接服务器  
- `login()/register_()`: 登录/注册逻辑  
- `view()/order()/view_my()/cancel()`: 票务操作  
- 输入验证：手机号格式、密码复杂度（需包含数字、大小写字母）

#### 界面示例

##### 未登录

<img src=".\assets\未登录.png" alt="未登录" style="zoom:200%;" />

##### 已登录

<img src=".\assets\已登录.png" alt="已登录" style="zoom: 150%;" />

### Admin 管理员模块
#### 核心功能
1. **票务管理**
   - 添加新票务（场馆名称、总票数、使用日期）
   - 查看所有票务信息（含实时预订统计）
2. **用户管理**
   - 查看所有注册用户信息
   - 查看/管理黑名单用户（加入/移出黑名单）

#### 核心类说明
- **`AdminManager`**: 管理员功能主控类
  - `ConnectDB()`: 直连MySQL数据库（无需通过服务端）
  - `Run()`: 控制台交互主循环
  - 黑名单管理方法: 
    - `AddToBlacklist()`: 通过手机号封禁用户
    - `RemoveFromBlacklist()`: 恢复用户权限

#### 操作界面示例
<img src=".\assets\管理员.png" alt="管理员" style="zoom:200%;" />

#### 典型操作流程
1. **添加票务**
   ```text
   场馆名称: 国家大剧院
   总票数: 500
   使用日期(YYYY-MM-DD): 2023-12-25
   → 自动初始化已预订数为0
   ```

2. **用户封禁**
   ```text
   输入手机号: 13812345678
   → 自动验证用户存在性
   → 更新user_info.status字段
   ```

#### 注意事项
1. 管理员系统直接操作数据库，需确保：
   ```cpp
   // admin.hpp中的数据库配置与实际环境一致
   db_ip = "127.0.0.1";       // MySQL服务器IP
   db_user = "root";          // 数据库账号
   db_passwd = "zbk";         // 数据库密码
   ```
2. 票务状态字段控制逻辑：
   - 状态为0时用户端不可见
   - 可通过`UPDATE ticket_info SET status=0 WHERE tk_id=1`手动下架票务

#### 安全建议
1. 生产环境应使用独立数据库账号并限制权限
2. 建议增加操作日志记录功能
3. 敏感操作（如黑名单管理）可增加二次确认

---

## 编译与运行

### 依赖安装
```bash
# Ubuntu
sudo apt install libevent-dev libjsoncpp-dev libmysqlclient-dev
```

### Server
```bash
g++ -o ser ser.cpp -levent -ljsoncpp -lmysqlclient
./ser
```

### Client
```bash
g++ -o client client.cpp -ljsoncpp
./client
```

### Admin
```bash
g++ -o admin admin.cpp -lmysqlclient
./admin
```
---

## 注意事项

1. 确保MySQL服务已启动，且数据库`Project_DB`存在（配置见`ser.cpp`中`mysql_client`构造函数）
2. 若登录失败，检查SQL语句拼写（如`uesrname`应为`username`）
3. 服务器默认监听`127.0.0.1:6000`，可按需修改`socket_listen`类参数
