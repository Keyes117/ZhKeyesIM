# ZhKeyesIM

## GateServer

### 获取验证码服务

#### 接受客户端消息

```json
{
	"email":"xxxx@xxx.com"
}
```

### RPC服务返回消息

```
{
	"error":"0",
	"email":"xxxx@xxx.com",
	"code":"1000"
}
```

- error: 返回值为0 ，则无错误， 返回值为1则有错误
- email: 接收验证码 的邮箱地址
- code: rpc服务错误码
  - 10000 ：成功
  - 10001 ：redis 获取键值错误
  - 10002 ：邮件服务错误
  - 10003 ：服务端异常

## Redis

### 本机调试用Redis服务密码

123456

### 启动 Redis 服务
sudo systemctl start redis

### 或者使用 redis-server（取决于系统）
sudo systemctl start redis-server

### 设置开机自启
sudo systemctl enable redis

### 查看服务状态
sudo systemctl status redis

### 停止服务
sudo systemctl stop redis

### 重启服务
sudo systemctl restart redis

### 检查 Redis 进程
ps aux | grep redis

### 检查端口是否监听
netstat -tlnp | grep 6379
### 或使用 ss 命令
ss -tlnp | grep 6379

### 使用 redis-cli 测试连接
redis-cli ping

如果返回 PONG，说明 Redis 正在运行

### 