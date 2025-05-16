#include<iostream>
#include "rpcapplication.h"
#include"friend.pb.h"

int main(int argc, char** argv){
    //整个程序启动后，想使用rpc框架来享受rpc服务调用，一定要先调用框架的初始化函数（只初始化一次）
    RpcApplication::Init(argc,argv);

    //调用远程发布的rpc方法
    fix::FriendServiceRpc_Stub stub(new MyRpcChannel());
    //rpc方法的请求参数
    fix::GetFriendsListRequest request;
    request.set_userid(100);
    //rpc方法的响应
    fix::GetFriendsListResponse response;
    RpcController controller;
    //发起rpc方法的调用     同步的rpc调用过程
    stub.GetFriendsList(&controller,&request,&response,nullptr);//RpcChannel->callMethod 集中处理所有的rpc方法调用的参数序列化和网络发送

    //调用完成，读调用结果
    if(controller.Failed()){
        std::cout<<controller.ErrorText()<<std::endl;
    }
    else{
        if(response.result().errcode()==0){
            std::cout<<"rpc GetFriendsList response success!"<<std::endl;
            int size=response.friends_size();
            for(int i=0;i<size;i++){
                std::cout<<"index: "<<(i+1)<<"name: "<<response.friends(i)<<std::endl;
            }
        }
        else{
            std::cout<<"rpc login response error: "<<response.result().errmsg()<<std::endl;
        }
    }
    

    return 0;
}

