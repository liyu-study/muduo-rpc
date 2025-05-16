#pragma once

#include<unordered_map>
#include<string>

//框架读取配置文件类 rpcserver_ip  rpcserver_port  zookeeper_ip zookeeper_port 
class RpcConfig{
public:
    //加载解析配置文件
    void LoadConfigFile(const char *config_file);
    //根据key去获取map中的value
    std::string Load(std::string key);
private:
    //用key-value的形式存储配置文件信息
    std::unordered_map<std::string,std::string> m_configMap;
    //去掉字符串的空格
    void Trim(std::string &str);

};