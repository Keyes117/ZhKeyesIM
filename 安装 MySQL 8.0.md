在 Ubuntu 上安装 MySQL 的步骤：

## 安装 MySQL 8.0

*# 1. 更新包索引*

sudo apt update

*# 2. 安装 MySQL 8.0*

sudo apt install -y mysql-server-8.0

*# 3. 启动并设置自启*

sudo systemctl start mysql

sudo systemctl enable mysql

*# 4. 检查 MySQL 状态*

sudo systemctl status mysql

## 安全配置（首次安装后必须执行）

*# 运行安全配置脚本*

sudo mysql_secure_installation

配置选项：

- 设置 root 密码强度验证策略

- 设置 root 密码

- 移除匿名用户

- 禁止 root 远程登录

- 移除测试数据库

- 重新加载权限表

## 配置 root 用户

### 方式1：使用 sudo

*# 以 root 权限登录（无需密码）*

sudo mysql

*# 在 MySQL 中执行*

ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY '你的密码';

FLUSH PRIVILEGES;

EXIT;

### 方式2：设置 root 密码

sudo mysql

ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY '你的密码';

FLUSH PRIVILEGES;

EXIT;

## 常用 MySQL 命令

### 服务管理

*# 启动 MySQL*

sudo systemctl start mysql

*# 停止 MySQL*

sudo systemctl stop mysql

*# 重启 MySQL*

sudo systemctl restart mysql

*# 查看状态*

sudo systemctl status mysql

*# 查看 MySQL 版本*

mysql --version

### 登录 MySQL

*# 使用 root 登录*

sudo mysql -u root -p

*# 使用普通用户登录*

mysql -u username -p

### MySQL 基本操作

*-- 显示所有数据库*

SHOW DATABASES;

*-- 创建数据库*

CREATE DATABASE mydb;

*-- 使用数据库*

USE mydb;

*-- 显示所有表*

SHOW TABLES;

*-- 创建用户*

CREATE USER 'username'@'localhost' IDENTIFIED BY 'password';

*-- 授予权限*

GRANT ALL PRIVILEGES ON mydb.* TO 'username'@'localhost';

FLUSH PRIVILEGES;

*-- 查看用户*

SELECT user, host FROM mysql.user;

*-- 删除用户*

DROP USER 'username'@'localhost';

## 配置远程访问

*# 编辑 MySQL 配置文件*

sudo nano /etc/mysql/mysql.conf.d/mysqld.cnf

找到并注释或修改：

\# bind-address = 127.0.0.1

bind-address = 0.0.0.0

*-- 在 MySQL 中创建远程用户*

CREATE USER 'username'@'%' IDENTIFIED BY 'password';

GRANT ALL PRIVILEGES ON *.* TO 'username'@'%';

FLUSH PRIVILEGES;

*# 重启 MySQL*

sudo systemctl restart mysql

*# 开放防火墙端口（如果使用 ufw）*

sudo ufw allow 3306/tcp

## 卸载 MySQL

*# 停止服务*

sudo systemctl stop mysql

*# 卸载 MySQL*

sudo apt remove --purge mysql-server mysql-client mysql-common mysql-server-core-* mysql-client-core-*

*# 删除配置文件和数据*

sudo rm -rf /etc/mysql /var/lib/mysql

*# 清理残留*

sudo apt autoremove

sudo apt autoclean

## 常见问题排查

### 忘记 root 密码

*# 1. 停止 MySQL*

sudo systemctl stop mysql

*# 2. 以安全模式启动*

sudo mysqld_safe --skip-grant-tables &

*# 3. 登录 MySQL（无需密码）*

mysql -u root

*# 4. 重置密码*

USE mysql;

ALTER USER 'root'@'localhost' IDENTIFIED BY '新密码';

FLUSH PRIVILEGES;

EXIT;

*# 5. 重启 MySQL*

sudo systemctl restart mysql

### 查看 MySQL 日志

*# 错误日志*

sudo tail -f /var/log/mysql/error.log

*# 查看 MySQL 配置*

sudo cat /etc/mysql/mysql.conf.d/mysqld.cnf

## 使用 Docker 运行 MySQL

*# 拉取 MySQL 镜像*

docker pull mysql:8.0

*# 运行 MySQL 容器*

docker run -d \

 --name mysql-server \

 -e MYSQL_ROOT_PASSWORD=yourpassword \

 -e MYSQL_DATABASE=mydb \

 -e MYSQL_USER=user \

 -e MYSQL_PASSWORD=password \

 -p 3306:3306 \

 mysql:8.0

*# 进入容器*

docker exec -it mysql-server mysql -u root -p

## 验证安装

*# 检查 MySQL 是否运行*

sudo systemctl status mysql

*# 测试连接*

mysql -u root -p -e "SELECT VERSION();"

安装完成后，建议运行 mysql_secure_installation 进行安全配置。

## 方法4：修改配置文件（永久生效）

*# 编辑 MySQL 配置文件*

sudo nano /etc/mysql/mysql.conf.d/mysqld.cnf

在 [mysqld] 部分添加：

[mysqld]

validate_password.policy=LOW

validate_password.length=6

validate_password.mixed_case_count=0

validate_password.number_count=0

validate_password.special_char_count=0

*# 重启 MySQL*

sudo systemctl restart mysql

## 完整操作步骤（推荐）

*-- 1. 查看当前策略*

SHOW VARIABLES LIKE 'validate_password%';

*-- 2. 临时降低策略要求*

SET GLOBAL validate_password.policy = LOW;

SET GLOBAL validate_password.length = 6;

SET GLOBAL validate_password.mixed_case_count = 0;

SET GLOBAL validate_password.number_count = 0;

SET GLOBAL validate_password.special_char_count = 0;

*-- 3. 设置密码*

ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY '!#zyq1230';

FLUSH PRIVILEGES;

*-- 4. 验证*

SELECT user, host, plugin FROM mysql.user WHERE user='root';

## 密码策略级别说明

- LOW：只检查长度（默认 8）

- MEDIUM：检查长度、数字、大小写、特殊字符

- STRONG：在 MEDIUM 基础上还检查字典文件