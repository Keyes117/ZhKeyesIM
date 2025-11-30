const config_module = require('./config');
const Redis = require('ioredis');

const RedisClient = new Redis({
    host: config_module.redis_host,
    port: config_module.redis_port,
    password: config_module.redis_password,

     retryStrategy: function(times) {
        // 重试策略：最多重试 3 次
        if (times > 3) {
            console.error("Redis: Max retry times reached, giving up");
            return null; // 停止重试
        }
        const delay = Math.min(times * 200, 2000);
        console.log(`Redis: Retrying connection in ${delay}ms (attempt ${times})`);
        return delay;
    },
    reconnectOnError: function(err) {
        // 某些错误可以自动重连
        const targetError = 'READONLY';
        if (err.message.includes(targetError)) {
            return true; // 自动重连
        }
        return false;
    }

});

/**
 * 监听连接成功事件
 */
RedisClient.on('connect', function() {
    console.log("Redis: Connected successfully");
});

/**
 * 监听就绪事件
 */
RedisClient.on('ready', function() {
    console.log("Redis: Ready to accept commands");
});

/**
 * 监听错误信息
 */
RedisClient.on('error', function(err) {
    console.error("RedisClient connect error");
    // RedisClient.quit();
});

/**
 * 监听关闭事件
 */
RedisClient.on('close', function() {
    console.log("Redis: Connection closed");
});

/**
 * 根据key 获取value
 * @param {*}key
 * @returns
 */
async function GetRedis(key)
{
    try{
        //get 实际返回一个 promise, 这里就需要awiat 等get操作完成
        const result = await RedisClient.get(key)
        if(result === null)
        {
            console.log('result:', '<'+result+'>', 'This key cannot be find...')
            return null
        }
        console.log('result:', '<'+result+'>', 'Get key success! ...')
        return result
    }
    catch(error)
    {
        console.log('GetReids error is', error)
        return null
    }
}

/**
 * 查询key 值 是否存在
 * @param {*}key
 * @returns
 */
async function QueryRedis(key)
{
    try{
        //get 实际返回一个 promise, 这里就需要awiat 等get操作完成
        const result = await RedisClient.exists(key)
        if(result === null)
        {
            console.log('result:', '<'+result+'>', 'This key cannot be find...')
            return null
        }
        console.log('result:', '<'+result+'>', 'Get key success! ...')
        return result
    }
    catch(error)
    {
        console.log('QueryRedis error is', error)
        return null
    }
}


/**
 * 设置key 和 Value 的过期时间
 * @param {*}key
 * @param {*}value
 * @param {*}exptime
 * @returns
 */
async function SetRedisExpire(key, value, exptime)
{
    try{
        await RedisClient.set(key, value)

        await RedisClient.expire(key, exptime)
        return true;
    }
    catch(error)
    {
        console.log('SetRedisExpire error is', error)
        return false
    }
}

/**
 * 退出函数
 */
function Quit()
{
    RedisClient.quit();
}

module.exports = {GetRedis, QueryRedis, SetRedisExpire, Quit}