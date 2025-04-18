#pragma once

#include <muduo/net/TcpServer.h>

#include <muduo/net/EventLoop.h>

#include <muduo/net/TcpConnection.h>

#include <muduo/net/Buffer.h>

#include <muduo/base/CountDownLatch.h>

#include <muduo/net/EventLoopThread.h>

#include <muduo/net/TcpClient.h>

#include "detail.hpp"

#include "fields.hpp"

#include "abstract.hpp"

#include <arpa/inet.h>

#include <unordered_map>

#include <mutex>

#include "message.hpp"

namespace Rpc {

/* -------------------- MuduoBuffer -------------------- */

class MuduoBuffer : public BaseBuffer {
public:
  using ptr = std::shared_ptr<MuduoBuffer>;

  MuduoBuffer(muduo::net::Buffer *buf) : _buf(buf) {}

  /* 获取可读数据大小 */
  virtual size_t readableSize() override;

  /* 尝试读取4字节数据 (不清除缓冲区)*/
  virtual int32_t peekInt32() override;

  /* 删除缓冲区4字节数据 */
  virtual void retrieveInt32() override;

  /* 读取缓冲区4字节数据(取走后对四字节进行删除) */
  virtual int32_t readInt32() override;

  /* 取出特定长度大小的字符串 */
  virtual std::string retrieveAsString(size_t len) override;

private:
  /* 功能实现 不对资源进行管理 */
  muduo::net::Buffer *_buf; // 不能使用智能指针 以免被释放
};

size_t MuduoBuffer::readableSize() { return _buf->readableBytes(); }

int32_t MuduoBuffer::peekInt32() { return _buf->peekInt32(); }

void MuduoBuffer::retrieveInt32() { _buf->retrieveInt32(); }

int32_t MuduoBuffer::readInt32() { return _buf->readInt32(); }

std::string MuduoBuffer::retrieveAsString(size_t len) {
  return _buf->retrieveAsString(len);
}

class BufferFactory {
  // 简单工厂类
public:
  template <typename... Args> static BaseBuffer::ptr create(Args &&...args) {
    return std::make_shared<MuduoBuffer>(std::forward<Args>(args)...);
  }
};

/* -------------------- LVProtocol -------------------- */

class LVProtocol : public BaseProtocol {
public:
  using ptr = std::shared_ptr<LVProtocol>;

  /* 判断缓冲区中数据是否能被处理 */
  virtual bool
  canProtocol(const BaseBuffer::ptr &) override; // 接收数据时的处理

  /* 对缓冲区中数据进行处理并使用一个BaseMessage的输出型参数进行接收 */
  virtual bool onMessage(const BaseBuffer::ptr &,
                         BaseMessage::ptr &) override; // 接收数据时的处理

  /* 对消息进行序列化 返回一个序列化后的字符串 */
  virtual std::string
  serialize(const BaseMessage::ptr &) override; // 发送数据时的处理

private:
  /*
      |--valueLength--|/// value ///|
          /                  ↓                   \
         |--Mtype--|--IDLength--|--MID--|--Body--|
  */
  int32_t valuelength_len = 4;

  int32_t mtype_len = 4;

  int32_t idlength_len = 4;
};

bool LVProtocol::canProtocol(const BaseBuffer::ptr &buf) {
  if (buf->readableSize() <
      valuelength_len) { // 读取当前buffer中可读数据大小,
                         // 判断是否能正常取出数据总长度(前4bytes) 不能则返回
    DLOG("当前可读数据:%ld\n", buf->readableSize());

    ELOG("数据不足\n");
    return false;
  }

  int32_t len = buf->peekInt32(); // 读取前四个字节 (同样用于判断大小)
  if (buf->readableSize() <
      (len + valuelength_len)) // 协议的格式为L-V, 因此判断L+4
                               // 的大小是否为一条完整数据
  // 当前可读数据大小小于总大小(不足完整数据)
  {
    DLOG("缓冲区数据不足 当前缓冲区可读大小: %ld, 需读大小: %d + 4\n",
         buf->readableSize(), len);
    return false;
  }
  return true;
}

bool LVProtocol::onMessage(const BaseBuffer::ptr &buf, BaseMessage::ptr &msg) {
  int32_t total_len = buf->readInt32();
  MType mtype = (MType)buf->readInt32();
  // DLOG("LVProtocol::onMessage->mtype: %d\n", (int)mtype);
  int32_t id_len = buf->readInt32();
  std::string mid = buf->retrieveAsString(id_len);
  int32_t body_len = total_len - id_len - idlength_len - mtype_len;
  std::string body = buf->retrieveAsString(body_len);

  msg = MessageFactory::create(mtype);
  if (msg.get() == nullptr) {
    ELOG("LVProtocol 构造消息对象时类型错误: %d\n", (int)mtype);
    return false;
  }

  if (!msg->unserialize(body)) {
    ELOG("LVProtocol 反序列化失败\n");
    return false;
  }

  msg->setId(mid);

  msg->setMType(mtype);

  return true;
}

std::string LVProtocol::serialize(const BaseMessage::ptr &msg) {

  // DLOG("LVProtocol 序列化前msg->mtype: %d\n", (int)msg->mtype());
  // TODO

  // 序列化
  std::string id = msg->rid();

  std::string body = msg->serialize();

  // DLOG("Protocol 序列化成功: %s\n", body.c_str());

  // 手动转成网络字节序 - muduo库会进行一次网络字节序转主机字节序
  // 因此先手动转换为网络字节序 (跨平台一致性)
  MType h_mtype = msg->mtype();
  MType n_mtype = (MType)htonl((int32_t)msg->mtype());

  int32_t h_id_length = id.size();
  int32_t n_id_length = htonl(id.size());

  int32_t total_length =
      htonl(body.size() + mtype_len + h_id_length + idlength_len);

  std::string result;
  result.reserve(total_length); // 预先开辟空间

  // 采用二进制的方式
  result.append((char *)&total_length, valuelength_len);
  result.append((char *)&n_mtype, mtype_len);
  result.append((char *)&n_id_length, idlength_len);
  result.append(id);
  result.append(body);

  return result;
}

class ProtocolFactory {
public:
  template <typename... Args> static BaseProtocol::ptr create(Args &&...args) {
    return std::make_shared<LVProtocol>(std::forward<Args>(args)...);
  }
};

/* -------------------- MuduoConnection -------------------- */

class MuduoConnection : public BaseConnection {
public:
  using ptr = std::shared_ptr<MuduoConnection>;

  MuduoConnection(const muduo::net::TcpConnectionPtr &conn,
                  const BaseProtocol::ptr &protocol)
      : _conn(conn), _protocol(protocol) {}

  virtual bool connected() override; // 判断连接是否正常

  virtual void shutdown() override; // 关闭连接

  virtual void send(const BaseMessage::ptr &) override; // 发送消息

private:
  muduo::net::TcpConnectionPtr _conn; // 连接对象

  BaseProtocol::ptr _protocol; // 协议对象 - 不需要对每个连接都创建一个协议对象
                               // 主要用于提供序列化与反序列化的功能
};

bool MuduoConnection::connected() { return _conn->connected(); }

void MuduoConnection::shutdown() {
  _conn->shutdown();
  DLOG("shutdown连接");
}

void MuduoConnection::send(const BaseMessage::ptr &msg) {
  std::string body = _protocol->serialize(msg);

  ////////DEBUG↓//////////////
  auto tmprdp = std::dynamic_pointer_cast<RpcRequest>(msg);
  Json::Value tmpparms = tmprdp->params();

  // DLOG("\nMTypd: %d\nMethod: %s\nParams: [%d,%d]\nRid: %s\n",
  // tmprdp->mtype(), tmprdp->method().c_str(), tmpparms["num1"].asInt(),
  // tmpparms["num2"].asInt(), tmprdp->rid().c_str());
  ////////DEBUG↑//////////////

  _conn->send(body);
  DLOG("MuduoConnection::send: \n");
}
class ConnectionFactory {
public:
  template <typename... Args>
  static BaseConnection::ptr create(Args &&...args) {
    return std::make_shared<MuduoConnection>(std::forward<Args>(args)...);
    // 传递参数包
  }
};

/* -------------------- MuduoServer -------------------- */

class MuduoServer : public BaseServer {
public:
  using ptr = std::shared_ptr<MuduoServer>;

  MuduoServer(int port)
      : _server(&_baseloop /* 事件监控对象 */,
                muduo::net::InetAddress("0.0.0.0",
                                        port) /* 服务器所监听的地址与端口 */,
                "MuduoServer" /* 服务器名称 */,
                muduo::net::TcpServer::kNoReusePort /* 启用地址重用 */),
        _protocol(ProtocolFactory::create()) {}

  /* 服务器启动 */
  virtual void start();

private:
  void onConnection(const muduo::net::TcpConnectionPtr &);

  void onMessage(const muduo::net::TcpConnectionPtr &, muduo::net::Buffer *,
                 muduo::Timestamp);

private:
  muduo::net::EventLoop _baseloop; // 事件监控对象
  muduo::net::TcpServer _server;   // 服务端
  std::mutex _mutex;

  // 公共资源 需要互斥锁保护线程安全
  std::unordered_map<muduo::net::TcpConnectionPtr, BaseConnection::ptr> _conns;
  BaseProtocol::ptr _protocol;

  const size_t maxSize = (1 << 16);
};

void MuduoServer::start() {
  _server.setConnectionCallback(
      std::bind(&MuduoServer::onConnection, this, std::placeholders::_1));
  _server.setMessageCallback(
      std::bind(&MuduoServer::onMessage, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));

  DLOG("服务器启动");
  _server.start();
  _baseloop.loop();
  DLOG("开始时间监听");
}

void MuduoServer::onConnection(const muduo::net::TcpConnectionPtr &con) {
  if (con->connected()) {
    std::cout << "连接建立\n";
    BaseConnection::ptr muduo_con = ConnectionFactory::create(con, _protocol);

    {
      std::unique_lock<std::mutex> lock(_mutex); // RAII

      _conns.insert({con, muduo_con}); // 管理映射关系
    }

    if (_cb_connection)
      _cb_connection(muduo_con);
  } else {
    std::cout << "连接断开\n";
    BaseConnection::ptr muduo_con;
    {
      std::unique_lock<std::mutex> lock(_mutex); // RAII
      auto it = _conns.find(con);
      if (it == _conns.end()) {
        return;
      }
      muduo_con = it->second;
      _conns.erase(con);
    }
    if (_cb_close)
      _cb_close(muduo_con);
  }
}

void MuduoServer::onMessage(const muduo::net::TcpConnectionPtr &con,
                            muduo::net::Buffer *buf, muduo::Timestamp) {
  auto base_buf = BufferFactory::create(buf);

  DLOG("服务端接收到新数据\n");
  while (1) {

    if (!_protocol->canProtocol(base_buf)) {
      // 数据不足
      DLOG("数据不足\n");
      if (base_buf->readableSize() > maxSize) {
        // 数据错误导致数据堆积 远超最大数据存储大小
        con->shutdown();
        ELOG("数据错误导致数据堆积 超过最大数据可读大小\n");
        return;
      }

      break;
    }
    BaseMessage::ptr msg;
    if (!_protocol->onMessage(base_buf, msg)) {
      // 数据错误
      con->shutdown();
      // 硬性关闭连接主要考虑数据完整性 安全性 资源管理
      // 与协议要求保证数据不会出现错误
      ELOG("数据错误\n");
      return;
    }

    BaseConnection::ptr base_con;
    {
      // RAII
      std::unique_lock<std::mutex> lock(_mutex);

      auto it = _conns.find(con);

      if (it == _conns.end()) // 未找到该连接的映射关系
      {
        DLOG("未找到该连接的映射\n");
        con->shutdown(); // 关闭连接
        return;
      }

      DLOG("base_con 获取成功\n");
      base_con = it->second; // 找到映射关系 获取本地管理连接
    }

    if (_cb_message) // 如果用户设置了消息回调则调用消息回调
    {
      DLOG("_cb_message回调被设置 调用该回调");
      _cb_message(base_con, msg);
      DLOG("回调已调用");
    }
  }
}

class ServerFactory {
public:
  template <typename... Args> static BaseServer::ptr create(Args &&...args) {
    return std::make_shared<MuduoServer>(std::forward<Args>(args)...);
  }
};

/* -------------------- MuduoClient -------------------- */

class MuduoClient : public BaseClient {
public:
  using ptr = std::shared_ptr<MuduoClient>;

  MuduoClient(const std::string &ip, int port)
      : _baseloop(_loopthread.startLoop()), _downlatch(1), // 初始化计数器为 1
        _client(_baseloop, muduo::net::InetAddress(ip, port), "MuduoClient"),
        _protocol(ProtocolFactory::create()) {}

  /* 发起连接 */
  void connect() override;

  /* 连接关闭 */
  void shutdown() override;

  /* 消息发送 */
  bool send(const BaseMessage::ptr &) override;

  /* 获取连接 */
  BaseConnection::ptr connection() override;

  /* 连接建立是否正常 */
  bool connected() override;

private:
  void onConnection(const muduo::net::TcpConnectionPtr &);

  void onMessage(const muduo::net::TcpConnectionPtr &, muduo::net::Buffer *,
                 muduo::Timestamp);

private:
  /* 用于获取连接 */
  BaseConnection::ptr _conn;

  /* 计数同步 */
  muduo::CountDownLatch _downlatch;

  /* 事件监控 */
  muduo::net::EventLoopThread _loopthread;
  muduo::net::EventLoop *_baseloop;

  /* 客户端 */
  muduo::net::TcpClient _client;

  BaseProtocol::ptr _protocol; // 协议对象

  const size_t maxSize = (1 << 16); // 最大数据存储大小
};

void MuduoClient::onConnection(const muduo::net::TcpConnectionPtr &con) {
  if (con->connected()) {
    std::cout << "连接建立!!\n";
    _downlatch.countDown(); // --计数器
    _conn = ConnectionFactory::create(con, _protocol);
  } else {
    std::cout << "连接断开!!\n";
    _conn.reset(); // reset 是unique_ptr的接口
  }
}

void MuduoClient::onMessage(const muduo::net::TcpConnectionPtr &con,
                            muduo::net::Buffer *buf, muduo::Timestamp) {
  auto base_buf = BufferFactory::create(buf);

  while (1) {
    if (!_protocol->canProtocol(base_buf)) {
      // 数据不足
      if (base_buf->readableSize() > maxSize) {
        // 数据错误导致数据堆积 远超最大数据存储大小
        con->shutdown();
        ELOG("数据错误导致数据堆积 超过最大数据可读大小\n");
        return;
      }
      break;
    }
    BaseMessage::ptr msg;
    if (!_protocol->onMessage(base_buf, msg)) {
      // 数据错误
      con->shutdown();
      // 硬性关闭连接主要考虑数据完整性 安全性 资源管理
      // 与协议要求保证数据不会出现错误
      ELOG("数据错误\n");
      return;
    }

    // DLOG("MuduoClient::onMessage - msg->mtype: %d\n", (int)msg->mtype());

    if (_cb_message) // 如果用户设置了消息回调则调用消息回调
    {
      _cb_message(_conn, msg);
    }
  }
}

void MuduoClient::connect() {
  _client.setConnectionCallback(
      std::bind(&MuduoClient::onConnection, this, std::placeholders::_1));
  _client.setMessageCallback(
      std::bind(&MuduoClient::onMessage, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));

  _client.connect(); // 获取连接
  _downlatch.wait(); // 等待计数 计数器不为0时阻塞执行流
}

void MuduoClient::shutdown() {
  _client.disconnect();
  DLOG("shutdown 客客端关闭连接");
}

/* 消息发送 */
bool MuduoClient::send(const BaseMessage::ptr &msg) {
  if (connected()) {
    _conn->send(msg);
    return true;
  }

  ELOG("连接断开");
  return false;
}
/* 获取连接 */
BaseConnection::ptr MuduoClient::connection() { return _conn; }
/* 连接建立是否正常 */
bool MuduoClient::connected() { return (_conn && _conn->connected()); }

class ClientFactory {
public:
  template <typename... Args> static BaseClient::ptr create(Args &&...args) {
    return std::make_shared<MuduoClient>(std::forward<Args>(args)...);
  }
};
} // namespace Rpc
