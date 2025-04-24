#pragma once

#include "net.hpp"

#include "message.hpp"

#include <unordered_map>

#include <memory>

#include <mutex>

#include <functional>

/////////////////////// Dispatcher模块 ////////////////////////////

/*
 * 该模块主要用于对消息的分发与处理
 * 主要还是分发
 * 本质上由于客户端将接收到服务端的多种类型的响应
 * 而服务端将接收到来自客户端的多种类型请求 因此需要分类进行分发处理
 */

/*
 * 该模块的思路为
 * 1. 上层传递注册的回调函数的参数是一个con连接以及一个msg消息
 *    其中注册时需要保存消息类型与对应的回调函数, 其中在注册中出现了两种情况:
 *    (1). 如果注册的回调函数的参数是BaseMessage,
 *         那么表示用户在使用回调函数时对应的需要将BaseMessage通过dynamic_point_cast进行转换,
 *         反而提高用户使用成本;
 *    (2). 如果注册时回调函数中参数的消息类型是对应具体的类型,
 *         那么在哈希表中无法存储数据不一致的类型
 *
 * 2. 为了解决上述问题的第一个问题,
 *    首先得确保注册的回调函数中的参数必须是一个实体的msg类型, 如RpcRequest;
 *    而由实体msg类型所发生的哈希表无法保存不同类型数据解决方式是采用模板,
 *    而若是使用了模板同样在哈希表中无法存储不同的类型,
 *    因此解决方案是定义一个抽象类作为基类, 基类去派生一个对应的模板类,
 *    模板类重写基类的OnMessage函数, 哈希表的保存交给基类,
 *    实际上方法的调用交给派生类的OnMessage函数;
 *
 * 3. 当解决完上述问题后,
 *    最后一个问题是registerHandler函数将成为一个模板函数需要指定类,
 *    指定的类是一个具体消息类型的类,
 *    而对应的这个类会实例化参数中第二个参数的handler的类型,
 *    将handler的类型从一开始的可调用对象类型改成消息类型(因为是T handler)
 *    因此为了确保注册成功,
 *    CallbackT中还需要用函数包装器包装一个可以通过T类型实例化对应可调用对象类型的MessageCallback,
 *    故在注册时用户的模板实例化<>中传递具体的消息类型,
 *    而传入的第二个参数则是具体的可调用对象,
 *    注册过程中将会通过CallbackT中通过function包装器包装出一个消息类型用于接收对应的可调用对象,
 *    并放到存储父类指针的哈希表中(CallbackT中的OnMessage),
 *    当消息到来时将通过Dispatcher模块中的onMessage函数在哈希表中寻找对应的函数,
 *    当找到对应的消息类型时直接调用CallbackT中的OnMessage,
 *    这个onMessage将会通过dynamic_pointer_cast<T>的方式转换为对应的可调用对象,
 *    并传入参数进行调用;
 *
 */

namespace Rpc {
class Callback {
public:
  using ptr = std::shared_ptr<Callback>;
  virtual void onMessage(const BaseConnection::ptr &, BaseMessage::ptr &) = 0;
};

template <typename T> class CallbackT : public Callback {
public:
  using ptr = std::shared_ptr<CallbackT<T>>;
  /* 此处采用了模板函数
   原因为不同的消息类型有不同的处理方式
   不同的Message类型有不同的处理方式
   当接收到一个消息时，根据消息类型调用不同的处理函数
   例如：当接收到一个RpcRequest消息时，调用RpcRouter::onRpcRequest*/
  using MessageCallback = std::function<void(const BaseConnection::ptr &con,
                                             std::shared_ptr<T> &msg)>;

  CallbackT(const MessageCallback &handler) : _handler(handler) {} // 构造

  virtual void onMessage(const BaseConnection::ptr &con,
                         BaseMessage::ptr &msg) override {
    auto type_msg = std::dynamic_pointer_cast<T>(msg);
    _handler(con, type_msg);
  }

private:
  MessageCallback _handler;
};

class Dispatcher {
public:
  using ptr = std::shared_ptr<Dispatcher>;
  template <typename T>
  void registerHandler(MType, const typename CallbackT<T>::MessageCallback
                                  &); // 向外部暴露一个用于注册函数的接口

  void
  onMessage(const BaseConnection::ptr &,
            BaseMessage::ptr &); // 找到消息类型对应的业务处理函数, 进行回调处理

private:
  std::unordered_map<MType, Callback::ptr>
      _handlers; // 存储父类指针 通过父类指针指向子类对象的多态性质实
                 // 现存储不同类型的 Message 回调
  std::mutex _mutex;
};

template <typename T>
void Dispatcher::registerHandler(
    MType mtype, const typename CallbackT<T>::MessageCallback &handler) {
  std::unique_lock<std::mutex> lock(_mutex); // RAII
  auto cb = std::make_shared<CallbackT<T>>(
      handler); // handler 为不同的消息处理回调(可调用对象)
  _handlers.insert({mtype, cb});

  // DLOG("注册成功!! 注册类型为:%d\n", (int)mtype);
}

void Dispatcher::onMessage(const BaseConnection::ptr &con,
                           BaseMessage::ptr &msg) {
  std::unique_lock<std::mutex> lock(_mutex); // RAII
  auto it = _handlers.find(
      msg->mtype()); // 在哈希表中不同类型的消息有着不同的回调函数
                     // 根据消息类型找到不同的处理函数进行调用
  if (it == _handlers.end()) {
    ELOG("Dispatcher -- 未知消息类型: %d \n", (int)msg->mtype());
    DLOG("%s\n", msg->serialize().c_str());
    // 也考虑协议的一致性 安全性 判断客户端数据错误
    // 或是可能存在的恶意客户端
    con->shutdown();
    return;
  }
  it->second->onMessage(con, msg); // 父类指针指向子类对象(多态调用)
}
} // namespace Rpc
