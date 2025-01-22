#include <iostream>

#include <future>

#include <unistd.h>

#include <thread>

#include <chrono>

int Add(int a,int b){
    std::cout << "into Add\n";
    return a+b;
}

int main()
{
    std::future<int>ret1 = std::async(std::launch::async,Add,5,10); // 进行异步非阻塞调用
    
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout<<ret1.get()<<std::endl; // 获取结果并打印
                                    
    std::cout<<"-----------------------------------"<<std::endl; 

    std::future<int>ret2 = std::async(std::launch::deferred,Add,9,80); // 同步 不创建线程
    
    sleep(2);

    std::cout<<ret2.get()<<std::endl; // 获取结果时才调用可调用对象

    return 0;
}

