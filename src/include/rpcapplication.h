#pragma once

#include"rpcconfig.h"
#include"rpcchannel.h"
#include"rpccontroller.h"
//rpc框架的基础类
/*
这个基础类只需要一个就够了，框架其他类（日志信息、配置信息等）所共有的基础信息
所以使用单例模式
*/
class RpcApplication{
public:
    static void Init(int argc, char **argv);
    static RpcApplication& GetInstance();
    static RpcConfig& GetConfig();
private:
    RpcApplication(){};
    //把与拷贝构造相关的，delete掉
    RpcApplication(const RpcApplication&) =delete;
    RpcApplication(RpcApplication&&)=delete;

    static RpcConfig m_config;

};