#pragma once

#include<google/protobuf/service.h>
#include<google/protobuf/message.h>
#include<google/protobuf/descriptor.h>
#include"zookeeperutil.h"

class MyRpcChannel: public google::protobuf::RpcChannel
{
public:
    void CallMethod(const google::protobuf::MethodDescriptor *,google::protobuf::RpcController*,
                    const google::protobuf::Message*, google::protobuf::Message*, google::protobuf::Closure*);
private:
};