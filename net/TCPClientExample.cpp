// TCPClient使用示例
// 这个文件展示了如何使用TCPClient进行异步网络通信

#include "TCPClient.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

class IMClient
{
public:
    IMClient() 
    {
        setupCallbacks();
    }
    
    bool connectToServer(const std::string& serverIP, uint16_t port)
    {
        std::cout << "Connecting to IM server " << serverIP << ":" << port << std::endl;
        
        // 启用自动重连，最多重连10次，每次间隔3秒
        m_client.enableAutoReconnect(true, 3000, 10);
        
        // 启用心跳，每30秒发送一次
        m_client.enableHeartbeat(true, 30000);
        
        // 异步连接
        return m_client.connectAsync(serverIP, port);
    }
    
    void disconnect()
    {
        m_client.disconnect();
    }
    
    void sendMessage(const std::string& message)
    {
        if (m_client.isConnected()) {
            // 实际项目中，这里会按照IM协议格式化消息
            std::string formattedMsg = "MSG:" + message + "\r\n";
            m_client.send(formattedMsg);
            std::cout << "Sent: " << message << std::endl;
        } else {
            std::cout << "Not connected to server!" << std::endl;
        }
    }
    
    void start()
    {
        m_client.start();
    }
    
    void stop()
    {
        m_client.stop();
    }

private:
    void setupCallbacks()
    {
        // 连接成功回调
        m_client.setConnectionCallback([this](std::shared_ptr<TCPConnection>& conn) {
            std::cout << "✅ Connected to IM server successfully!" << std::endl;
            
            // 设置消息接收回调
            conn->setReadCallBack([this](Buffer& buffer) {
                handleIncomingMessage(buffer);
            });
            
            // 连接成功后发送登录信息
            sendLoginRequest();
        });
        
        // 连接断开回调
        m_client.setDisConnectionCallback([this](const std::shared_ptr<TCPConnection>& conn) {
            std::cout << "❌ Disconnected from IM server" << std::endl;
        });
        
        // 连接失败回调
        m_client.setConnectFailedCallback([this]() {
            std::cout << "❌ Failed to connect to IM server" << std::endl;
        });
    }
    
    void handleIncomingMessage(Buffer& buffer)
    {
        // 处理接收到的消息
        std::string data = buffer.retrieveAllAsString();
        std::cout << "Received: " << data << std::endl;
        
        // 在实际IM项目中，这里会解析协议并处理不同类型的消息
        // 例如：聊天消息、系统通知、在线状态等
    }
    
    void sendLoginRequest()
    {
        // 发送登录请求
        std::string loginMsg = "LOGIN:user123:password123\r\n";
        m_client.send(loginMsg);
    }

private:
    TCPClient m_client;
};

// 简单的客户端示例
void simpleClientExample()
{
    std::cout << "\n=== Simple Client Example ===" << std::endl;
    
    TCPClient client;
    
    // 设置回调
    client.setConnectionCallback([](std::shared_ptr<TCPConnection>& conn) {
        std::cout << "Connected! Sending hello message..." << std::endl;
        
        conn->setReadCallBack([](Buffer& buffer) {
            std::string data = buffer.retrieveAllAsString();
            std::cout << "Server response: " << data << std::endl;
        });
        
        conn->send("Hello Server!\r\n");
    });
    
    client.setConnectFailedCallback([]() {
        std::cout << "Connection failed!" << std::endl;
    });
    
    // 连接服务器
    if (client.connectAsync("127.0.0.1", 8080)) {
        client.start();
        
        // 运行10秒后断开
        std::this_thread::sleep_for(std::chrono::seconds(10));
        client.stop();
    }
}

// IM客户端示例
void imClientExample()
{
    std::cout << "\n=== IM Client Example ===" << std::endl;
    
    IMClient imClient;
    
    if (imClient.connectToServer("127.0.0.1", 8080)) {
        imClient.start();
        
        // 模拟用户发送消息
        std::this_thread::sleep_for(std::chrono::seconds(2));
        imClient.sendMessage("Hello everyone!");
        
        std::this_thread::sleep_for(std::chrono::seconds(2));
        imClient.sendMessage("How are you doing?");
        
        // 运行30秒
        std::this_thread::sleep_for(std::chrono::seconds(30));
        imClient.stop();
    }
}

// 多客户端连接示例
void multiClientExample()
{
    std::cout << "\n=== Multi Client Example ===" << std::endl;
    
    const int clientCount = 5;
    std::vector<std::unique_ptr<TCPClient>> clients;
    
    for (int i = 0; i < clientCount; ++i) {
        auto client = std::make_unique<TCPClient>();
        
        // 为每个客户端设置回调
        client->setConnectionCallback([i](std::shared_ptr<TCPConnection>& conn) {
            std::cout << "Client " << i << " connected!" << std::endl;
            
            conn->setReadCallBack([i](Buffer& buffer) {
                std::string data = buffer.retrieveAllAsString();
                std::cout << "Client " << i << " received: " << data << std::endl;
            });
            
            std::string msg = "Hello from client " + std::to_string(i) + "\r\n";
            conn->send(msg);
        });
        
        // 启动客户端
        if (client->connectAsync("127.0.0.1", 8080)) {
            client->start();
            clients.push_back(std::move(client));
        }
        
        // 错开连接时间
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    // 运行15秒
    std::this_thread::sleep_for(std::chrono::seconds(15));
    
    // 停止所有客户端
    for (auto& client : clients) {
        client->stop();
    }
}

int main()
{
    std::cout << "TCPClient Examples" << std::endl;
    std::cout << "Note: Make sure you have a TCP server running on port 8080" << std::endl;
    
    try {
        // 运行不同的示例
        simpleClientExample();
        imClientExample();
        // multiClientExample();  // 可以取消注释测试多客户端
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    
    std::cout << "Examples completed." << std::endl;
    return 0;
}

/*
使用说明：

1. 基本用法：
   TCPClient client;
   client.setConnectionCallback([](auto& conn) { // 连接成功 });
   client.connectAsync("127.0.0.1", 8080);
   client.start();

2. IM客户端模式：
   - 自动重连
   - 心跳保活
   - 消息处理

3. 编译和运行：
   - 确保链接了net库
   - 运行前启动一个TCP服务器在8080端口

4. 实际项目中的建议：
   - 根据具体协议格式化消息
   - 添加消息队列处理
   - 实现用户界面集成
   - 添加错误处理和日志
*/ 