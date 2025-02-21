# ETOCK Reservation System(Easy To Chack预约系统)
> 目前仅支持Linux系统，请在使用前安装相应c库

## 启动方式

### Client客户端
- Client g++ -o client client.cpp -ljsoncpp
- ./client

### Server服务器
- g++ -o ser ser.cpp -levent -ljsoncpp -lmysqlclient
- ./ser
