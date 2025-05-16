#pragma once 

#include<zookeeper/zookeeper.h>
#include<semaphore>
#include<string>

class ZkClient{
public:
    ZkClient();
    ~ZkClient();
    void Start();//zkclient启动连接zkserver
    //zkserver根据指定的 path 创建znode结点
    void Create(const char* path, const char* data,int datalen, int state=0); 
    //根据参数指定的节点路径，获取znode的值
    std::string GetData(const char* path);
private:
    zhandle_t *m_zhandel; //zk客户端的句柄

};