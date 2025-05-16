#include<iostream>
#include<string>
#include"friend.pb.h"
#include"rpcapplication.h"
#include"rpcprovider.h"
#include<vector>
#include"logger.h"

using namespace std; //一般不怎么使用命名空间，容易命名冲突

/*
UserService原来是一个本地服务，提供了进程内的两个本地方法，Login和GetFriendLists
*/

class FriendService : public fix::FriendServiceRpc{
public:
    //本地的登录方法
    vector<string> GetFriendsList(uint32_t userid){
        cout<<"doing GetFriendsList service! userid: "<<userid<<endl;
        vector<string> vec;
        vec.push_back("gao yang");
        vec.push_back("liu hong");
        vec.push_back("wang shuo");
        return vec;
    }

    //下面这些方法都是框架直接调用的，是业务代码，框架怎么调用这些方法，才是框架代码
    //重写基类UserServiceRpc的虚函数 ，这个login方法是rpc方法，可以不与本地的login同名
    void GetFriendsList(::google::protobuf::RpcController* controller,
                       const ::fix::GetFriendsListRequest* request,
                       ::fix::GetFriendsListResponse* response,
                       ::google::protobuf::Closure* done)
    {
        //框架给业务上报了请求参数GetFriendsRequest，应用获取相应的数据做本地业务
        uint32_t userid=request->userid();

        //做本地业务，此时的GetFriendsList是在本地的 提供给caller调用的 方法
        vector<string> friendsList=GetFriendsList(userid);

        //把响应写入 包括错误码、错误信息
        fix::ResultCode *rc=response->mutable_result(); 
        rc->set_errcode(0);
        rc->set_errmsg("");
        for(string &str:friendsList){
            string *p=response->add_friends();
            *p=str;
        }

        //执行回调操作 执行响应对象数据的序列化 和 网络发送 （都是由框架代码完成的）
        done->Run();

    }
};

//假设框架已备好，使用框架去做事情

int main(int argc,char** argv){

    //调用框架的初始化操作
    RpcApplication::Init(argc,argv);

    //把UserServie对象发布到rpc节点上，provider可看成 是一个rpc网络服务对象
    RpcProvider provider;
    provider.NotifyService(new FriendService());

    //启动一个rpc服务发布结点 Run以后，进程进入阻塞状态，等待进程远程rpc调用请求的返回
    provider.Run();
    return 0;
}
