#include <iostream>

#include <functional>

#include <muduo/net/TcpClient.h>

#include <muduo/net/TcpConnection.h>

#include <muduo/net/Buffer.h>

#include <muduo/net/EventLoop.h>

#include <muduo/net/EventLoopThread.h>

#include <muduo/base/CountDownLatch.h> // 用于计数 防止过早调用Connection

#include <string>

#include <functional>

class DictClient {

public:
  DictClient(const std::string &sip, int sport);
  bool send(const std::string &msg);

private:
  void on_connection(const muduo::net::TcpConnectionPtr &);
  void on_message(const muduo::net::TcpConnectionPtr &, muduo::net::Buffer *,
                  muduo::Timestamp);

private:
  muduo::net::TcpConnectionPtr _con;       // 获取/管理连接
  muduo::CountDownLatch _count_down_latch; // 计数
  muduo::net::EventLoopThread _loopthread; // 用新线程管理事件监控
  muduo::net::EventLoop *_baseloop;        // 管理循环监控的线程
  muduo::net::TcpClient _client;
};

DictClient::DictClient(const std::string &sip, int sport)
    : _baseloop(_loopthread.startLoop()),
      _count_down_latch(1 /* 计数器初始化为1 */),
      _client(_baseloop, muduo::net::InetAddress(sip, sport), "DictClient") {

  _client.setConnectionCallback(
      std::bind(&DictClient::on_connection, this, std::placeholders::_1));
  _client.setMessageCallback(
      std::bind(&DictClient::on_message, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));

  _client.connect();        // 发起连接
  _count_down_latch.wait(); // 连接发起后进行等待 (connect本身为非阻塞)
}

bool DictClient::send(const std::string &msg) {
  if (!_con->connected()) { // 判断连接是否建立
    std::cout << "连接未建立" << std::endl;
    return false;
  }
  _con->send(msg);
  return true;
}

void DictClient::on_connection(const muduo::net::TcpConnectionPtr &cb) {
  if (cb->connected()) { // 使用connected判断连接是否建立
    std::cout << "链接建立" << std::endl;
    _count_down_latch.countDown();
    _con = cb;
  } else {
    std::cout << "链接未建立" << std::endl;
  }
}

void DictClient::on_message(const muduo::net::TcpConnectionPtr &cb,
                            muduo::net::Buffer *buf, muduo::Timestamp) {
  // 消息回调 - 客户端主要对服务端发出的响应进行读取与打印
  std::string res = buf->retrieveAllAsString();
  std::cout << res << std::endl;
}

int main() {
  DictClient client("127.0.0.1", 8099);
  while (1) {
    std::string msg;
    std::cin >> msg;
    client.send(msg);
  }
  return 0;
}
