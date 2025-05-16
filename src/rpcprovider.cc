#include"rpcprovider.h"
#include<google/protobuf/descriptor.h>
#include"rpcheader.pb.h"

/* 
service_name  -> service描述 (*service记录服务对象) ==> 一个服务对象可有多个服务方法 method_name -> method方法对象
*/
void RpcProvider::NotifyService(google::protobuf::Service *service){
    ServiceInfo service_info;

    //获取了服务对象的描述信息
    const google::protobuf::ServiceDescriptor *pserviceDesc = service->GetDescriptor();
    //获取服务的名字
    std::string service_name = pserviceDesc->name();
    //获取服务对象的方法数量
    int methodCnt=pserviceDesc->method_count();
    
    // std::cout<<"service_name:"<<service_name<<std::endl;
    LOG_INFO("service_name: %s",service_name.c_str());

    for(int i=0;i<methodCnt;i++){
        //获取指定下标的服务对象的 服务方法描述（抽象描述）
        const google::protobuf::MethodDescriptor *pmethodDesc=pserviceDesc->method(i);
        std::string method_name=pmethodDesc->name();
        service_info.m_methodMap.insert({method_name,pmethodDesc});

        // std::cout<<"method_name:"<<method_name<<std::endl;
        LOG_INFO("method_name: %s",method_name.c_str());
    }
    service_info.m_service=service;
    m_serviceMap.insert({service_name,service_info});

}

//启动rpc服务节点，开始提供rpc远程网络调用服务
void RpcProvider::Run(){
    //设置需要远程连接的IP和port，从RpcApplication中的初始化配置文件获取
    std::string ip=RpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port=atoi(RpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip,port);
    //创建TcpServer对象
    muduo::net::TcpServer server(&m_eventLoop,address,"RpcProvier");

    //绑定连接回调和消息读写回调方法
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection,this,std::placeholders:: _1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage,this,std::placeholders::_1,
            std::placeholders::_2,std::placeholders::_3));

    //设置muduo库的线程数量
    server.setThreadNum(4);

    //把当前rpc结点上要发布的方法全部注册到zookeeper服务中心，让客户端从zookeeper发现服务
    ZkClient zkcli;
    zkcli.Start(); //连接zkserver
    //service name为永久性结点， method name 为临时性结点；断开连接后，临时性结点会被自动删除
    for(auto &sp : m_serviceMap){
        //service name
        std::string service_path= "/"+sp.first;
        zkcli.Create(service_path.c_str(),nullptr,0);
        for(auto &mp: sp.second.m_methodMap){
            //method name  
            std::string method_path=service_path+"/"+mp.first;
            char method_path_data[128]={0}; //存储当前这个rpc服务结点主机的IP和port
            sprintf(method_path_data,"%s:%d",ip.c_str(),port);
            zkcli.Create(method_path.c_str(),method_path_data,sizeof(method_path_data),ZOO_EPHEMERAL);   
        }
    }

    std::cout<<"RpcProvider start service at ip:"<<ip<<" port:"<<port<<std::endl;

    //启动网络服务
    server.start();
    m_eventLoop.loop();
}

void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr&conn){
    //调用rpc服务请求是一个短连接，请求结束之后，关闭连接
    if(!conn->connected()){
        conn->shutdown();
    }

}

/*
在框架内部，RpcProvider和RpcConsumer协商好通信之间的protobuf数据类型
假设要传的数据：service_name  method_name  args  (UserSErviceLoginzhang san123456)  定义proto的message类型，进行数据的序列化和反序列化
service_name nethod_name args_size ==》为防止数据粘连问题（另一段数据一部分跑到某段数据尾部去了），需加上参数长度
两方最终约定的数据格式：
header_size(4个字节) + header_str(service_name nethod_name args_size) + agrs_str  
*/
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr&conn, muduo::net::Buffer* buffer, muduo::Timestamp){
    //接收到的远程请求的字符流  传输过来的数据包括 方法名字、参数列表  Login args
    std::string recv_buf=buffer->retrieveAllAsString();

    //从字符流中读取前4个字节的内容
    uint32_t header_size=0;
    recv_buf.copy((char*)&header_size,4,0);

    //根据header_size读取数据头的原始字符流
    std::string rpc_header_str=recv_buf.substr(4,header_size);
    rpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    if(rpcHeader.ParseFromString(rpc_header_str)){
        //反序列化成功
        service_name=rpcHeader.service_name();
        method_name=rpcHeader.method_name();
        args_size=rpcHeader.args_size();

    }else{
        //数据反序列化失败
        std::cout<<"rpc header str: "<<rpc_header_str<<"parse error!"<<std::endl;
        return;
    }

    //获取参数的字符流数据
    std::string args_str=recv_buf.substr(4 + header_size,args_size);

    //打印调试信息
    std::cout<<"================================================="<<std::endl;
    std::cout<<"header_size: "<<header_size<<std::endl;
    std::cout<<"rpc_header_str: "<<rpc_header_str<<std::endl;
    std::cout<<"service_name: "<<service_name<<std::endl;
    std::cout<<"method_name: "<<method_name<<std::endl;
    std::cout<<"args_str: "<<args_size<<std::endl;
    std::cout<<"================================================="<<std::endl;

    //获取service对象和method对象
    auto it=m_serviceMap.find(service_name);
    if(it==m_serviceMap.end()){
        std::cout<<service_name<<" is not exit!"<<std::endl;
        return;
    }
    auto mit=it->second.m_methodMap.find(method_name);
    if(mit==it->second.m_methodMap.end()){
        std::cout<<method_name<<" is not exit!"<<std::endl;
        return;
    }

    google::protobuf::Service *service = it->second.m_service; //获取servic对象
    const google::protobuf::MethodDescriptor *method = mit->second;//获取method对象

    //生成rpc方法 调用的请求和响应参数
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if(!request->ParseFromString(args_str)){
        std::cout<<"request parse error, content: "<<args_str<<std::endl;
        return;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    //给下面的method方法的调用，绑定一个Closure的回调函数
    google::protobuf::Closure *done 
        =google::protobuf::NewCallback<RpcProvider,const muduo::net::TcpConnectionPtr&,google::protobuf::Message*>
                                        (this, &RpcProvider::SendRpcResponse, conn, response);

    //在框架上根据远端rpc请求，调用当前rpc节点上发布的方法
    //例子：UserService().Login(controller,request,response,done)
    service->CallMethod(method,nullptr,request,response,done);



}

//Closure的回调操作
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response){
    std::string response_str;
    if(response->SerializeToString(& response_str)){
        //返回的数据response进行序列化，成功后，通过网络把结果返回给调用方
        conn->send(response_str);
    }
    else{
        std::cout<<"serialize response_str error!"<<std::endl;
    }
    conn->shutdown(); //模拟HTTP的短连接服务，由rpc服务提供者主动断开连接
}