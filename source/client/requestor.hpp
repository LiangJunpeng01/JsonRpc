#pragma once

#include "../common/message.hpp"

#include "../common/fields.hpp"

#include "../common/net.hpp"

#include <future>

#include <unordered_map>

namespace Rpc {
namespace Client {
class Requestor {
  // 面向用户提供
  // 主要提供send发送接口( 需要提供两个重载 回调发送和异步发送 )
  // 其次是提供一个对服务端响应的一个onResponse
  // 通过onResponse将消息传送给Dispatcher进行请求分发从而进行下一步的处理
public:
  using ptr = std::shared_ptr<Requestor>;

  /*
   * 下面一个Callback与一个Async的future分别表示:
   * 1. RequestCallback: 用于回调请求
   * 2. AsyncResponse: 用于异步请求
   */
  using RequestCallback = std::function<void(
      const BaseMessage::ptr &)>; // 用function包装器包装一个函数类型

  using AsyncResponse = std::future<BaseMessage::ptr>; // 对future进行封装

  struct RequestDescribe // 内部类 用来描述请求信息
  {
    using ptr = std::shared_ptr<RequestDescribe>; // 指针描述
    RType rtype; // 请求类型 请求类型有两种, 分别为异步请求类型与回调请求类型
    BaseMessage::ptr request; // 请求消息
    std::promise<BaseMessage::ptr>
        response; // 通过get_future()可以获取一个与该promise相关联的future对象
    RequestCallback callback; // 消息类型
  }; // struct RequestDescribe

  void
  onResponse(const BaseConnection::ptr &,
             BaseMessage::ptr &); // 注册进Dispatcher中的对响应进行的业务处理

  /* 为用户提供三个send接口 通过send接口向服务端发送数据*/
  bool send(const BaseConnection::ptr &, const BaseMessage::ptr &,
            const RequestCallback &); // 回调请求

  bool send(const BaseConnection::ptr &, const BaseMessage::ptr &,
            AsyncResponse &); // 异步请求

  bool send(const BaseConnection::ptr &, const BaseMessage::ptr &,
            BaseMessage::ptr &); // 同步请求

private:
  /*
   * 在 Requestor 类中存在一个用来进行映射的哈希表
   * 需要为用户提供管理该哈希表的接口
   *
   * 对临界资源进行访问都需要保证线程安全
   */
  RequestDescribe::ptr
  newDestribe(const BaseMessage::ptr &, RType,
              const RequestCallback &cb = nullptr); // 创建一个请求描述

  RequestDescribe::ptr
  getDescribe(const std::string &); // 通过rid找到对应的请求描述

  void removeDescribe(const std::string &); // 通过rid删除对应的请求描述
private:
  std::mutex _mutex; // 保护_request_describe 临界资源
  std::unordered_map<std::string, RequestDescribe::ptr>
      _request_describes; // rid 与 请求描述的映射关系

}; // class Requestor

void Requestor::onResponse(const BaseConnection::ptr &con,
                           BaseMessage::ptr &res) {
  /*
   通过rid找到对应的请求描述
   若未找到则表示该响应不存在对应的请求描述
   若找到则判断该响应对应的请求描述的rtype
   若rtype为RType::SYNC 则调用callback
   若rtype为RType::ASYNC 则调用promise的set_value
   处理完毕后删除对应的请求描述 以免内存泄漏
  */
  auto describe = getDescribe(res->rid());
  if (describe.get() == nullptr) {
    ELOG("not found request describe for response rid:{ %s }\n",
         res->rid().c_str());
    return; // 未找到对应的请求描述 无需处理
  }
  if (describe->rtype == RType::REQ_CALLBACK) {
    if (describe->callback) // 若callback存在 则调用callback
    {
      describe->callback(res);    // 同步请求 调用callback
      removeDescribe(res->rid()); // 删除对应的请求描述
    } else {
      ELOG("callback is empty"); // 若callback为空 则不调用callback
    }
  } else if (describe->rtype == RType::REQ_ASYNC) {
    describe->response.set_value(res); // 异步请求 调用promise的set_value
    removeDescribe(res->rid());        // 删除对应的请求描述
  } else {
    ELOG("unknown request type:{ %d }\n", (int)describe->rtype);
  }
  removeDescribe(res->rid());
}

bool Requestor::send(const BaseConnection::ptr &con,
                     const BaseMessage::ptr &req,
                     const RequestCallback &callback) {

  auto rdp =
      newDestribe(req, RType::REQ_CALLBACK, callback); // 创建一个请求描述
                                                       // 留了一手 做个判断
  if (!rdp.get()) {
    ELOG("Requestor Destribe 对象构造失败\n");
    return false;
  }
  con->send(req); // 发送请求
  return true;
}

bool Requestor::send(const BaseConnection::ptr &con,
                     const BaseMessage::ptr &req, AsyncResponse &async_resp) {
  auto rdp = newDestribe(req, RType::REQ_ASYNC); // 异步请求
  if (rdp.get() ==
      nullptr) { // 一般情况下没有创建失败的可能
                 // 因为一般构造失败时对应的 newDescribe
                 // 函数中的 make_shared 将会提前抛异常终止程序向下运行
    ELOG("Requestor Destribe 对象构造失败\n");
    return false;
  }
  DLOG("Rqeuestor Destribe 对象构造成功\n");
  ////////DEBUG↓//////////////
  // auto tmprdp = std::dynamic_pointer_cast<RpcRequest>(rdp->request);
  // Json::Value tmpparms = tmprdp->params();

  // DLOG("\nMTypd: %d\nMethod: %s\nParams: [%d,%d]\nRid: %s\n",
  // tmprdp->mtype(), tmprdp->method().c_str(), tmpparms["num1"].asInt(),
  // tmpparms["num2"].asInt(), tmprdp->rid().c_str());
  ////////DEBUG↑//////////////
  DLOG("Requestor::send 异步send 准备send");

  con->send(req);
  async_resp = rdp->response.get_future();
  return true;
}

bool Requestor::send(const BaseConnection::ptr &con,
                     const BaseMessage::ptr &req,
                     BaseMessage::ptr &rsp) // 同步请求
{                                           // 这里是一个同步操作
  AsyncResponse rsp_future; // 用于接收异步send中的future对象
  DLOG("Requestor 同步send 准备send");
  // DLOG("req: %s", req->serialize().c_str());

  bool ret = send(con, req, rsp_future);
  if (!ret) {
    DLOG("Requestor 同步send 失败\n");
    return false;
  }
  con->send(req);
  rsp = rsp_future.get(); // 可以直接get进行阻塞(同步) 需要将rsp进行返回
  return true;
}

Requestor::RequestDescribe::ptr
Requestor::newDestribe(const BaseMessage::ptr &req, RType rtype,
                       const RequestCallback &cb) {
  std::unique_lock<std::mutex> lock(_mutex); // RAII

  auto describe = std::make_shared<RequestDescribe>();
  describe->request = req;
  describe->rtype = rtype;
  describe->callback = cb;
  _request_describes[req->rid()] = describe;
  return describe;
}

Requestor::RequestDescribe::ptr Requestor::getDescribe(const std::string &rid) {
  std::unique_lock<std::mutex> lock(_mutex); // RAII
  auto it = _request_describes.find(rid);
  if (it == _request_describes.end()) {
    return Requestor::RequestDescribe::ptr(); // 返回一个空的智能指针
  }
  return it->second; // 找到 返回对应的请求描述
}

void Requestor::removeDescribe(const std::string &rid) {
  std::unique_lock<std::mutex> lock(_mutex); // RAII
  _request_describes.erase(rid);             // 删除对应的请求描述
}
} // namespace Client

} // namespace Rpc
