# ZhKeyesIM

.\b2.exe install --toolset=msvc-14.3 address-model=64 --build-type=complete --prefix="D:\resource\boost_1_81_0" link=shared runtime-link=shared threading=multi debug release

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