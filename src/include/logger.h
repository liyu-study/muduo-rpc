#pragma once
#include<string>
#include"lockqueue.h"

enum LogLevel{
    INFO,
    ERROR,
};
class Logger{
public:
    
    static Logger& GetInstance();
    //设置日志级别
    void SetLogLevel(LogLevel level);
    // 写日志
    void Log(std::string msg);
private:
    int m_loglevel;//记录日志级别
    LockQueue<std::string> m_lckQue;//日志缓冲队列
    //提供单例模式，将所有的拷贝构造删除掉，防止通过拷贝构造生成新对象
    Logger();
    Logger(const Logger&)=delete;
    Logger(const Logger&&)=delete;
};

//定义宏，方便用户使用,以可变参的形式
#define LOG_INFO(logmsgformat,...)\
    do{ \
        Logger &logger=Logger::GetInstance(); \
        logger.SetLogLevel(INFO); \
        char c[1024]={0}; \
        snprintf(c,1024,logmsgformat,##__VA_ARGS__); \
        logger.Log(c); \
    }while(0)
    
#define LOG_ERR(logmsgformat,...) \
    do{ \
        Logger &logger=Logger::GetInstance(); \
        logger.SetLogLevel(ERROR); \
        char c[1024]={0}; \
        snprintf(c,1024,logmsgformat,##__VA_ARGS__); \
        logger.Log(c); \
    }while(0)