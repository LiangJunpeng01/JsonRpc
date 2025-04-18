#include <muduo/net/TcpClient.h>

#include <muduo/net/TcpConnection.h>

#include <muduo/net/EventLoop.h>

#include <muduo/net/Buffer.h>

#include <muduo/base/CountDownLatch.h>

#include <muduo/net/EventLoopThread.h>

#include <iostream>

#include <string>

#include <functional>

class DictClient {
public:
  DictClient(const std::string &, int); // 构造函数
  void start();                         // 运行客户端
  bool send(const std::string &);       // 发送数据
private:
  /* 连接时回调 */
  void onConnection(const muduo::net::TcpConnectionPtr &);

  /* 处理时回调 */
  void onMessage(const muduo::net::TcpConnectionPtr &, muduo::net::Buffer *,
                 muduo::Timestamp);

private:
  /* 用于获取连接 */
  muduo::net::TcpConnectionPtr _conn;

  /* 计数同步 */
  muduo::CountDownLatch _downlatch;

  /* 事件监控 */
  muduo::net::EventLoopThread _loopthread;
  muduo::net::EventLoop *_baseloop;

  /* 客户端 */
  muduo::net::TcpClient _client;
};

DictClient::DictClient(const std::string &ip, int port)
    : _baseloop(_loopthread.startLoop()), _downlatch(1), // 初始化计数器为 1
      _client(_baseloop, muduo::net::InetAddress(ip, port), "DictClient") {
  _client.setConnectionCallback(
      std::bind(&DictClient::onConnection, this, std::placeholders::_1));
  _client.setMessageCallback(
      std::bind(&DictClient::onMessage, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));

  _client.connect(); // 获取连接
  _downlatch.wait(); // 等待计数 计数器不为0时阻塞执行流
}

void DictClient::onConnection(const muduo::net::TcpConnectionPtr &cb) {
  if (cb->connected()) {
    std::cout << "连接建立!!\n";
    _downlatch.countDown(); // --计数器
    _conn = cb;
  } else {
    std::cout << "连接断开!!\n";
    _conn.reset(); // reset 是unique_ptr的接口
  }
}

bool DictClient::send(const std::string &msg) {
  if (!_conn->connected()) {
    std::cout << "连接已经断开 发送失败\n";
    return false;
  }
  _conn->send(msg);
  return true;
}

void DictClient::onMessage(const muduo::net::TcpConnectionPtr &cb,
                           muduo::net::Buffer *buf, muduo::Timestamp) {
  std::string res = buf->retrieveAllAsString();
  std::cout << res << std::endl;
}

int main() {
  DictClient client("127.0.0.1", 8111);
  while (1) {
    std::string msg;
    std::cin >> msg;
    client.send(msg);
  }
  return 0;
}
