#include"rpcconfig.h"
#include<iostream>

void RpcConfig::LoadConfigFile(const char *config_file){
    FILE *pf= fopen(config_file,"r");
    if(pf==nullptr){
        std::cout<<config_file<<" is not exit!"<<std::endl;
        exit(EXIT_FAILURE);
    }

    //1. 注释 2.正确的配置项 3. 去掉开头多余的空格
    while(!feof(pf)){
        char buf[512]={0};
        fgets(buf,512,pf);
        std::string src_buf(buf);
        //去掉字符串的空格
        Trim(src_buf);

        //判断#的注释
        if(src_buf[0]=='#' || src_buf.empty()){
            continue;
        }

        //判断配置项
        int idx=src_buf.find('=');
        //没有=号，不合法
        if(idx==-1){
            continue;
        }
        std::string key;
        std::string value;
        //substr(起始下标，需要截取的长度)
        //rpcserverip=127.0.0.1\n
        key=src_buf.substr(0,idx);
        Trim(key);
        int endidx=src_buf.find('\n',idx);
        value=src_buf.substr(idx+1,endidx-idx-1);
        Trim(value);
        m_configMap.insert({key,value});
    }
}

std::string RpcConfig::Load(std::string key){
    auto it=m_configMap.find(key);
    if(it==m_configMap.end()){
        return "";
    }
    return it->second;
}

void RpcConfig::Trim(std::string &src_buf){
    //去掉多余的空格
    int idx=src_buf.find_first_not_of(' ');
    if(idx!=-1){
        //说明字符串前面有空格
        src_buf=src_buf.substr(idx,src_buf.size()-idx);
    }
    idx=src_buf.find_last_not_of(' ');
    if(idx!=-1){
        //说明字符串后面有空格
        src_buf.substr(0,idx+1);
    }
}