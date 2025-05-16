#include "test.pb.h"
#include <iostream>
using namespace fix;

int main(){
    // LoginResponse rps;
    // ResultCode *rc=rps.mutable_result();
    // rc->set_errcode(1);
    // rc->set_errmsg("登录处理失败了");

    GetFriendListsResponse rps;
    ResultCode *rc=rps.mutable_result();
    rc->set_errcode(0);
    //列表的用法
    user *user1=rps.add_friend_list();
    user1->set_age(20);
    user1->set_name("li si");
    user1->set_sex(user::MAN);

    user *user2=rps.add_friend_list();
    user2->set_age(22);
    user2->set_name("wang wu");
    user2->set_sex(user::MAN);

    std::cout<<rps.friend_list_size()<<std::endl;
    std::cout<<rps.friend_list(1).name()<<std::endl;
    return 0;
}




int main1(){
    //封装对象的数据
    LoginRequest req;
    req.set_name("zhang san");
    req.set_pwd("123456");

    //对象数据序列化 =》 char*
    std::string send_str;
    if(req.SerializeToString(&send_str)){
        std::cout<<send_str.c_str()<<std::endl;
    }

    //反序列化
    LoginRequest reqB;
    if(reqB.ParseFromString(send_str)){
        std::cout<<reqB.name()<<std::endl;
        std::cout<<reqB.pwd()<<std:: endl;
    }
    return 0;
}