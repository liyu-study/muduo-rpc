#include"zookeeperutil.h"
#include"rpcapplication.h"
#include<semaphore.h>
#include<iostream>

//全局的watcher观察器  zkserver 给 zkclient的停止
void global_watcher(zhandle_t *zh, int type, int state, const char* path,void *watcherCtx){
    if(type==ZOO_SESSION_EVENT){ //回调的消息类型，是和会话相关的消息类型
        if(state==ZOO_CONNECTED_STATE){ //server和client 连接成功
            sem_t *sem=(sem_t*)zoo_get_context(zh); //获取当前句柄的上下文
            sem_post(sem); //给信号量资源加1
        }
    }
}

ZkClient::ZkClient():m_zhandel(nullptr){}
ZkClient::~ZkClient(){
    if(m_zhandel!=nullptr){
        zookeeper_close(m_zhandel);
    }
}

void ZkClient::Start(){
    //读取配置文件中的目标IP和端口
    std::string host=RpcApplication::GetInstance().GetConfig().Load("zookeeperip");
    std::string port=RpcApplication::GetInstance().GetConfig().Load("zookeeperport");
    std::string connstr=host+":"+port;

    /*
    使用的是zookeeper_mt:多线程版本的库
    zookeeperAPI客户端程序 提供三个线程： 调用API线程（即本线程）； 网络I/O线程 zookeeper_init底层调用了pthread_create 通过网络I/O发送
    还有一个watcher回调线程
    */
    m_zhandel=zookeeper_init(connstr.c_str(),global_watcher,30000,nullptr,nullptr,0); //这只是创建句柄成功，并不是连接成功
    //底层是异步实现的，客户端发送连接请求，成功后会执行指定的回调方法（global_watcher）
    if(m_zhandel==nullptr){
        std::cout<<"zookeeper_init error!"<<std::endl;
        exit(EXIT_FAILURE);
    }

    sem_t sem;
    sem_init(&sem,0,0);
    zoo_set_context(m_zhandel, &sem); //给句柄绑定信号量

    sem_wait(&sem); //信号量资源减1，当连接成功后，资源加1 再减1 ，互斥访问
    std::cout<<"zookeeper_init success!"<<std::endl;
}

void ZkClient::Create(const char *path, const char *data, int datalen, int state){
    char path_buf[128];
    int buf_len=sizeof(path_buf);
    int flag;
    flag=zoo_exists(m_zhandel, path,0,nullptr); //判断path代表的znode结点是否存在，若存在，不重复创建
    if(flag==ZNONODE){
        //创建结点
        flag=zoo_create(m_zhandel, path,data,datalen, 
            &ZOO_OPEN_ACL_UNSAFE, state, path_buf,buf_len);
        

        if(flag==ZOK){
            std::cout<<"znode create success... path:"<<path<<std::endl;
        }
        else{
            std::cout<<"flag: "<<flag<<std::endl;
            std::cout<<"znode create error... path: "<<path<<std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

//根据指定的path，获取znode结点的值
std::string ZkClient::GetData(const char *path){
    char buffer[64];
    int buflen=sizeof(buffer);
    int flag = zoo_get(m_zhandel, path,0,buffer,&buflen,nullptr);  //同步的方式获取结点的值
    if(flag!=ZOK){
        std::cout<<"get znode error ! path: "<<path<<std::endl;
        return"";
    }else{
        return buffer;
    }
    
}