#pragma once

#include <iostream>

#include "fields.hpp"

#include <memory>

#include <functional>

namespace Rpc {

class BaseMessage {
  /*
   * 作为整体消息类型的基类
   * 负责抽象整个消息系列类型
   * 首先所有消息的主要内容是消息的ID以及消息的类型
   * 因此该类中存在两个重要的成员, 分别为id以及类型
   * 无论是消息的组织还是消息的反序列化都需要这两个成员
   * 其次在BaseMessage的子类将会是JsonMessage,
   * 反序列化后检查消息字段是否完整
   *
   */
public:
  using ptr = std::shared_ptr<BaseMessage>;

  virtual ~BaseMessage() {}

  /* 设置消息ID */
  virtual void setId(const std::string &id) { _rid = id; }

  /* 获得消息ID */
  virtual std::string rid() { return _rid; }

  /* 设置消息类型 */
  virtual void setMType(MType mtype) {
    // ELOG("BaseMessage::setMtype:%d\n", (int)mtype);
    _mtype = mtype;
  }

  /* 获得消息类型 */
  virtual MType mtype() { return _mtype; }

  /* 序列化 */
  virtual std::string serialize() = 0;

  /* 反序列化 */
  virtual bool unserialize(const std::string &) = 0;

  /* 反序列化后检查消息字段是否完整 */
  virtual bool check() = 0;

private:
  MType _mtype;     // 消息类型
  std::string _rid; // 消息id
};

class BaseBuffer {
  /*
   * BaseBuffer 主要的目的是封装Muduo库中的Buffer的基类
   * Muduo 库主要基于事件触发, 因此当消息事件或者连接事件到来时,
   * 对应的消息内容将直接存储在Buffer中 因此需要从Buffer中读取对应的数据
   * 所有的消息数据都在Buffer中, 因此需要封装Muduo::Buffer,
   * 调用对应的函数来获取对应数据(Buffer的封装较为简单)
   */
public:
  /* 获取可读数据大小 */
  virtual size_t readableSize() = 0;

  /* 尝试读取4字节数据 (不清除缓冲区)*/
  virtual int32_t peekInt32() = 0;

  /* 删除缓冲区4字节数据 */
  virtual void retrieveInt32() = 0;

  /* 读取缓冲区4字节数据(取走后对四字节进行删除) */
  virtual int32_t readInt32() = 0;

  /* 取出特定长度大小的字符串 */
  virtual std::string retrieveAsString(size_t len) = 0;

  using ptr = std::shared_ptr<BaseBuffer>;
};

class BaseProtocol {
  /*
   * BaseProtocol 是为了给LV_Protocol 做准备
   * 实际上这个协议是用来制定一个规则,
   * 在Protocol模块中的成员变量都是用来定义规则的 如各个部分定长的长度
   * 同时Protocol定义了一套来判断, 并解析数据的一套方式,
   * 包括最终序列化为二进制流的string对象
   */
public:
  using ptr = std::shared_ptr<BaseProtocol>;

  /* 判断缓冲区中数据是否能被处理 */
  virtual bool canProtocol(const BaseBuffer::ptr &buf) = 0; // 接收数据时的处理

  /* 对缓冲区中数据进行处理并使用一个BaseMessage的输出型参数进行接收 */
  virtual bool onMessage(const BaseBuffer::ptr &buf,
                         BaseMessage::ptr &msg) = 0; // 接收数据时的处理

  /* 对消息进行序列化 返回一个序列化后的字符串 */
  virtual std::string
  serialize(const BaseMessage::ptr &msg) = 0; // 发送数据时的处理
};

class BaseConnection {
public:
  using ptr = std::shared_ptr<BaseConnection>;

  /* 判断连接是否正常 */
  virtual bool connected() = 0;

  /* 关闭连接 */
  virtual void shutdown() = 0;

  /* 发送消息 */
  virtual void send(const BaseMessage::ptr &msg) = 0;
};

/* 连接接收回调处理 */
using ConnectionCallback = std::function<void(const BaseConnection::ptr &)>;

/* 连接关闭回调处理 */
using CloseCallback = std::function<void(const BaseConnection::ptr &)>;

/* 消息接收回调处理 */
using MessageCallback =
    std::function<void(const BaseConnection::ptr &, BaseMessage::ptr &)>;

class BaseServer {
public:
  using ptr = std::shared_ptr<BaseServer>;

  /* 设置连接处理回调 */
  virtual void setConnectionCallback(const ConnectionCallback &cb) {
    _cb_connection = cb;
  }

  /* 设置关闭连接处理回调 */
  virtual void setCloseCallback(const CloseCallback &cb) { _cb_close = cb; }

  /* 设置消息处理回调 */
  virtual void setMessageCallback(const MessageCallback &cb) {
    _cb_message = cb;
  }

  /* 服务器启动 */
  virtual void start() = 0;

protected:
  ConnectionCallback _cb_connection; // 连接处理回调

  CloseCallback _cb_close; // 连接关闭回调

  MessageCallback _cb_message; // 消息处理回调
};

class BaseClient {
public:
  using ptr = std::shared_ptr<BaseClient>;

  /* 设置连接处理回调 */
  virtual void setConnectionCallback(const ConnectionCallback &cb) {
    _cb_connection = cb;
  }

  /* 设置关闭连接处理回调 */
  virtual void setCloseCallback(const CloseCallback &cb) { _cb_close = cb; }

  /* 设置消息处理回调 */
  virtual void setMessageCallback(const MessageCallback &cb) {
    _cb_message = cb;
  }

  /* 发起连接 */
  virtual void connect() = 0;

  /* 连接关闭 */
  virtual void shutdown() = 0;

  /* 消息发送 */
  virtual bool send(const BaseMessage::ptr &) = 0;

  /* 获取连接 */
  virtual BaseConnection::ptr connection() = 0;

  /* 连接建立是否正常 */
  virtual bool connected() = 0;

protected:
  ConnectionCallback _cb_connection; // 连接处理回调

  CloseCallback _cb_close; // 连接关闭回调

  MessageCallback _cb_message; // 消息处理回调
};

} // namespace Rpc
