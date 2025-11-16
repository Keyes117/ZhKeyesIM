async function GetVerifyCode(call, callback)
{
    console.log("email is ", call.request.email);
    try{
        uniqueId = uuidv4();
        console.log("uniqueId is ", uniqueId);
        let text_str = '您的验证码是：' + uniqueId + '，请在10分钟内使用。';

        let mailOptions = {
            from: '1732302170@qq.com',
            to: call.request.email,
            subject: '验证码',
            text: text_str,
        };

        let send_res = await emailModule.SendMail(mailOptions);
        console.log("send res is ", send_res);

        callback(null, {email: call.request.email,
            error:const_module.Errors.Success
        });
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