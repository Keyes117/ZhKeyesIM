```mysql
-- 删除现有表（注意：会丢失所有数据）
DROP TABLE IF EXISTS `user`;
DROP TABLE IF EXISTS `user_id`;

-- 重新创建 user 表
CREATE TABLE `user` (
  `uid` INT NOT NULL COMMENT '用户ID，主键',
  `name` VARCHAR(50) NOT NULL COMMENT '用户名',
  `email` VARCHAR(100) NOT NULL COMMENT '邮箱地址',
  `pwd` VARCHAR(255) NOT NULL COMMENT '密码哈希值',
  `create_time` BIGINT DEFAULT NULL COMMENT '创建时间（Unix时间戳）',
  `last_login_time` BIGINT DEFAULT NULL COMMENT '最后登录时间（Unix时间戳）',
  PRIMARY KEY (`uid`),
  UNIQUE KEY `uk_name` (`name`),
  UNIQUE KEY `uk_email` (`email`),
  KEY `idx_name` (`name`),
  KEY `idx_email` (`email`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='用户信息表';

-- 重新创建 user_id 表
CREATE TABLE `user_id` (
  `id` INT NOT NULL DEFAULT 0 COMMENT '当前最大用户ID',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='用户ID自增表';

-- 初始化数据
INSERT INTO `user_id` (`id`) VALUES (10000);
```

