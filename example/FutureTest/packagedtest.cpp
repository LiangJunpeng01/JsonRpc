#include <iostream>

#include <future>

#include <unistd.h>

#include <thread>

#include <chrono>

int Add(int a,int b){
    std::cout << "into Add\n";
    return a+b;
}

/*
   - 封装任务
   - 执行任务
   - 通过`std::future`对任务执行结果进行返回;
*/ 

int main()
{
    // 1.   直接调用与同步无异
    // /* 封装任务 */
    // std::packaged_task<int(int,int)> add(Add);
    //
    // /* 获取任务包关联的future */
    // std::future<int> res = add.get_future();
    //
    // /* 执行任务 */
    // add(10,20);
    //
    // /* 获取执行结果 */
    // std::cout<<res.get()<<std::endl;

    // 2.
    /* 封装任务 */
    auto add = std::make_shared<std::packaged_task<int(int,int)>>(Add); 
    // 采用智能指针防止出现生命周期结束导致误调用
    
    /* 获取任务包关联的future */
    std::future<int> res = add->get_future();
    
    /* 创建一个新线程执行任务 */
    std::thread thr([add](){
                    (*add)(10,20);
                    });

    /* 获取执行结果 */
    std::cout<<res.get()<<std::endl;

    thr.join();

    return 0;
}

