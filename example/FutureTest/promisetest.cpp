#include <iostream>

#include <future>

#include <unistd.h>

#include <thread>

#include <chrono>

int Add(int a,int b){
    std::cout << "into Add\n";
    return a+b;
}



int main(){
    // 1. 实例化制定结果的promise对象
    std::promise<int> pro;

    // 2. 通过promise对象获取相关联的future对象
    std::future<int> res = pro.get_future();

    // 3. 在任意位置给promise设置数据 
    std::thread thr([&pro](){
        int sum = Add(10,30);
        pro.set_value(sum);
                    });
    // 4. 获取结果
    std::cout<<res.get()<<std::endl;

    thr.join(); // 等待线程
    return 0;
}
