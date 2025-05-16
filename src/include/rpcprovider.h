//防止重复包含的
#pragma once 
#include"google/protobuf/service.h"
#include<memory>
#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include<muduo/net/InetAddress.h>
#include"rpcapplication.h"
#include<muduo/net/TcpConnection.h>
#include<string>
#include<functional>
#include<unordered_map>
#include"logger.h"
#include"zookeeperutil.h"

//框架提供的专门服务发布 rpc网络服务对象类, 需要考虑高并发，使用muduo库
class RpcProvider
{
public:
    //框架提供给外部使用的，可以发布rpc方法的接口函数
    void NotifyService(google::protobuf::Service *service);

    //启动rpc服务节点，开始提供rpc远程网络调用服务
    void Run();

private:
    //组合eventLoop
    muduo::net::EventLoop m_eventLoop;

    //service服务类型信息
    struct ServiceInfo{
        google::protobuf::Service *m_service; //保存服务对象
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_methodMap; //保存服务方法
    };
    //存储注册成功的服务对象和其服务方法的所有信息，服务名字和服务结构体信息； 结构体存储：服务对象 和 方法名字、方法对象的map
    std::unordered_map<std::string,ServiceInfo> m_serviceMap;


    //新的socket连接回调
    void OnConnection(const muduo::net::TcpConnectionPtr&);

    //已经建立连接的用户 的读写事件回调。 如果远处有一个rpc服务调用请求，这个方法就会响应
    void OnMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp);

    //Closure的回调操作，用于序列化rpc的响应和网络发送
    void SendRpcResponse(const muduo::net::TcpConnectionPtr&, google::protobuf::Message*);
};

