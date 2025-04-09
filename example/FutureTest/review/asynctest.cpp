#include <iostream>

#include <future>

#include <thread>

#include <chrono>

int Add(int num1, int num2) {
  std::cout << "into the function" << std::endl;
  return num1 + num2;
}

int main() {
  // std::cout << "hello world" << std::endl;
  /*
   *
   * future 本质是一个用户获取异步操作结果的容器
   * 可直接通过 future::get() 的成员函数获取异步操作的结果
   * asycn 的策略 async为异步, deferred为同步策略
   */

  // 立即启动异步任务，无需额外线程
  std::future<int> res = std::async(std::launch::async, Add, 12, 23);
  // auto res = std::async(std::launch::deferred, Add, 12, 23);

  // 主线程继续执行其他操作（体现了并行性）
  std::cout << "主线程在工作..." << std::endl;

  // 模拟耗时操作（注意: 真正异步场景下应避免 sleep 来等待）
  std::this_thread::sleep_for(std::chrono::seconds(3));

  // 需要结果时通过 get() 等待
  std::cout << "异步结果: " << res.get() << std::endl;

  return 0;
}
