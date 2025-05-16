#pragma once 
#include<queue>
#include<thread>
#include<mutex>
#include<condition_variable>

//异步写日志的日志队列
//模板写的，实现在头文件中，而不是源文件
template<typename T>
class LockQueue{
public:
    //多个线程互斥访问队列，写入日志信息
    void Push(const T &data){
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(data);
        m_condvariable.notify_one(); //因为只有一个线程将队列中的日志写出去，所以提醒一个线程
    }

    //一个线程读日志队列，写日志文件
    T Pop(){
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_queue.empty())
        {
            //日志队列为空，等待队列有信息，不抢夺资源
            m_condvariable.wait(lock);
        }

        T data=m_queue.front();
        m_queue.pop();
        return data;
        
    }
private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condvariable;
};