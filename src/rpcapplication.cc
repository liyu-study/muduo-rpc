#include "rpcapplication.h"
#include<iostream>
#include<unistd.h>
#include<string>

RpcConfig RpcApplication::m_config;
void ShowArgHelp(){
    std::cout<<"format: command -i <configfile>"<<std::endl;
}

void RpcApplication::Init(int argc, char **argv){
    //参数数量不够
    if(argc<2){
        ShowArgHelp();
        exit(EXIT_FAILURE);
    }
    //判断输出的命令行参数是否符合要求
    int c=0;
    std::string config_file;
    while((c=getopt(argc,argv,"i:"))!=-1){
        switch(c){
            case 'i':
                config_file=optarg;
                break;
            case '?':
                ShowArgHelp();
                exit(EXIT_FAILURE);
            case ':':
                ShowArgHelp();
                exit(EXIT_FAILURE);
            default:
                break;
        }
    }

    //开始加载配置文件 rpcserver_ip  rpcserver_port  zookeeper_ip zookeeper_port 
    m_config.LoadConfigFile(config_file.c_str());
    // std::cout<<"rpcserverip:"<<m_config.Load("rpcserverip")<<std::endl;
    // std::cout<<"rpcserverport:"<<m_config.Load("rpcserverport")<<std::endl;
    // std::cout<<"zookeeperip:"<<m_config.Load("zookeeperip")<<std::endl;
    // std::cout<<"zookeeperport:"<<m_config.Load("zookeeperport")<<std::endl;
}

RpcApplication& RpcApplication::GetInstance(){
    static RpcApplication app;
    return app;
}

RpcConfig& RpcApplication::GetConfig(){
    return m_config;
}
