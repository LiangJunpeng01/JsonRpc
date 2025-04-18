#include <iostream>

#include <functional>

#include <memory>

#include <muduo/net/TcpConnection.h>

#include <muduo/net/EventLoop.h>

#include <muduo/net/Buffer.h>

#include <muduo/net/TcpServer.h>

#include <string>

#include <unordered_map>

class DictServer {
public:
  // 向外暴露的接口, 构造以及启动
  DictServer(int port /*端口号*/);
  void start();

private:
  void
  on_connection(const muduo::net::TcpConnectionPtr &); // 服务端的链接处理回调
  void on_message(const muduo::net::TcpConnectionPtr &, muduo::net::Buffer *,
                  muduo::Timestamp); // 服务端的消息处理回调

private:
  muduo::net::EventLoop _baseloop; // 事件监控对象
  muduo::net::TcpServer _server;   // 服务器对象
};

DictServer::DictServer(int port)
    : _server(&_baseloop, muduo::net::InetAddress("0.0.0.0", port),
              "DictServer" /*可将地址重用取消 默认情况为地址重用*/) {
  _server.setConnectionCallback(std::bind(
      &DictServer::on_connection, this,
      std::placeholders::_1)); // 需要1个参数, 成员函数存在隐含的this指针
  _server.setMessageCallback(std::bind(
      &DictServer::on_message, this, std::placeholders::_1,
      std::placeholders::_2,
      std::placeholders::_3)); // 需要3个参数, 成员函数存在隐含的this指针
}

void DictServer::on_connection(const muduo::net::TcpConnectionPtr &cb) {
  if (cb->connected()) { // 使用connected判断连接是否建立
    std::cout << "链接建立" << std::endl;
  } else {
    std::cout << "链接未建立" << std::endl;
  }
}
void DictServer::on_message(
    const muduo::net::TcpConnectionPtr &cb, muduo::net::Buffer *buf,
    muduo::Timestamp /* Timestamp 用不着 暂时不需要定时时间 */) {
  std::unordered_map<std::string, std::string> dict;
  dict["hello"] = "你好";
  dict["apple"] = "苹果";
  dict["banana"] = "香蕉";
  dict["coffe"] = "咖啡";

  std::string msg = buf->retrieveAllAsString(); // 全部取出
  std::string ret;

  std::cout << "获取翻译内容: " << msg << std::endl;
  auto it = dict.find(msg); // 判断是否找到

  if (it == dict.end()) {
    ret = "未匹配";
    std::cout << "内容: " << msg << " 不匹配词库" << std::endl;
  } else {
    ret = it->second;
  }
  cb->send(ret); // 发送即可
  return;
}

void DictServer::start() {
  // EventLoop 与 start
  _server.start();  // 先启动服务器
  _baseloop.loop(); // 再进行事件监控
}

int main() {
  //  std::cout << "server" <<std::endl;

  DictServer server(8099);
  server.start();
  return 0;
}
