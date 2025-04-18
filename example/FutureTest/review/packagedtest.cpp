#include <iostream>

#include <thread>

#include <future>

#include <chrono>

#include <memory>

int Add(int num1, int num2) {
  std::cout << "Into the function" << std::endl;
  return num1 + num2;
}

int main() {
  // std::cout << "hello world" << std::endl;

  /*
   * packaged_task 主要用于封装一个任务
   * 所封装的任务可使用 get_future 函数来获取对应的关联 future 对象
   */

  auto add_task =
      std::make_shared<std::packaged_task<int(int, int)>>(Add); // 封装任务包

  std::future<int> res = add_task->get_future(); // 获取任务包相关联future对象

  std::thread t1([add_task]() { (*add_task)(12, 23); }); // 传入

  std::cout << "主线程工作..." << std::endl;

  std::this_thread::sleep_for(std::chrono::seconds(2));

  std::cout << "异步结果: " << res.get() << std::endl;

  t1.join();

  return 0;
}
