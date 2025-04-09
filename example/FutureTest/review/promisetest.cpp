#include <iostream>

#include <thread>

#include <chrono>

#include <future>

int Add(int num1, int num2) {
  std::cout << "Into the function" << std::endl;
  return num1 + num2;
}

int main() {
  /*
   * 本质上promise是用来封装结果
   * 通过promise::set_value() 来将结果封装到对应的promise对象中
   * 可以间接通过与该promise的相关联对象获取promise封装的结果实现异步操作
   */

  std::promise<int> pro;

  std::future<int> res = pro.get_future(); // 获取相关联的future对象

  std::thread thr([&pro]() { pro.set_value(Add(12, 23)); });

  std::cout << "主线程工作中..." << std::endl;

  std::this_thread::sleep_for(std::chrono::seconds(2));

  std::cout << "异步结果:: " << res.get() << std::endl;

  thr.join();

  return 0;
}
