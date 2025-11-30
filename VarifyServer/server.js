const grpc = require('@grpc/grpc-js');
const crypto = require('crypto');
const { v4: uuidv4 } = require('uuid');
const message_proto = require('./proto');
const email_module = require('./email');
const const_module = require('./const');
const redis_module = require('./redis');
// 生成唯一ID的函数（使用 crypto 生成随机字符串）

async function GetVerifyCode(call, callback)
{
    console.log("email is ", call.request.email)
    try{
        let query_res = await redis_module.GetRedis(const_module.code_prefix + call.request.email);
        console.log("query_res is ", query_res)
        let uniqueId = query_res;
        if(query_res == null)
        {
            uniqueId = uuidv4();
            if(uniqueId.length > 4)
            {
                uniqueId = uniqueId.substring(0, 4);
            }
            let bres = await redis_module.SetRedisExpire(const_module.code_prefix + call.request.email, uniqueId, 600);
            if(!bres)
            {
                callback(null, {email: call.request.email,
                    error:const_module.Errors.RedisError
                });
                return ;
            }

        }
        
        console.log("uniqueId is ", uniqueId);
        let text_str = '您的验证码是：' + uniqueId + '，请在10分钟内使用。';

        let mailOptions = {
            from: '1732302170@qq.com',
            to: call.request.email,
            subject: '验证码',
            text: text_str,
        };

        let send_res = await email_module.SendMail(mailOptions);
        if(send_res)
        {
            console.log("send res is ", send_res);

            callback(null, {email: call.request.email,
                error:const_module.Errors.Success
            });
        }
        else
        {
            callback(null, {email: call.request.email,
                error:const_module.Errors.EmailError
            });
        }

    }
    catch(error)
    {
        console.log("error is ", error);

        callback(null, {email: call.request.email,
            error:const_module.Errors.Exception
        });
    }
}

function main()
{
    var server = new grpc.Server();
    server.addService(message_proto.VerifyService.service, {GetVerifyCode: GetVerifyCode});
    server.bindAsync('0.0.0.0:50051', grpc.ServerCredentials.createInsecure(), () =>{
    
        server.start();
        console.log("server is running on port 50051");
    })
}

// 启动服务器
main();