#pragma once

#include <iostream>

#include <memory>

#include <string>

#include <vector>

#include "fields.hpp"

#include "detail.hpp"

#include "abstract.hpp"

namespace Rpc {
class JsonMessag : public BaseMessage {
  /*
   * JsonMessag 主要针对 BaseMessage 进行封装
   * JsonMessage 作为 JsonRequest 与 JsonResponse 的父类
   * 需要用有他们共通的属性, 即 _body 字段
   * 同时所有的消息类型都需要以 _id, _mtype, _body 字段存在
   * 因此在 JsonMessage 中需要进行序列化与反序列化
   * 重点是将 JsonMessage 中的 Json::Value 类型的 _body 字段进行序列化为一个
   * string 类型对象
   * 反序列化为将一个 string 类型对象反序列化为一个 Json::Value 对象
   */
public:
  using ptr = std::shared_ptr<JsonMessag>;

  virtual std::string serialize() override;

  virtual bool unserialize(const std::string &) override;

protected:
  Json::Value _body; // 一条消息中body所需要的字段
};

std::string JsonMessag::serialize() {
  /*
   * 此处的 serialize 本质上是将不同消息的Json类型统一序列化为一个string对象,
   * 这个string对象将成为 L-V Protocol 中的body部分
   */
  std::string body;
  bool ret = JSON::serialize(_body, body);
  if (!ret) {
    ELOG("序列化失败\n");
  }
  return body;
}

bool JsonMessag::unserialize(const std::string &body) {
  return JSON::unserialize(body, _body);
}

// JsonRequest 上的消息检查需要根据不同的请求进行消息检查
// 需要在派生类当中进行详细的请求消息检查
class JsonRequest : public JsonMessag {
  /*
   * 在JsonRequest 中没有什么需要进行重写或者添加的内容
   * 在请求类型的消息中的检查, 由于不同类型请求的格式不同
   * 因此请求的 check() 重写需要交由各个类型进行
   */
public:
  using ptr = std::shared_ptr<JsonRequest>;
};

// JsonResponse 需要进行检查项的实现
// 派生类中的每个Response都需要进行rcode的检查
// 因此JsonResponse主要用于检查rcode
class JsonResponse : public JsonMessag {
public:
  using ptr = std::shared_ptr<JsonResponse>;
  /*
   * JsonResponse 中主要用于获取 rcode 与 检查,设置 rcode
   * 本质原因是所有的响应消息都存在 rcode 字段
   * 此处设置, 检查, 获取 可提高代码复用性
   * 对于rcode字段主要检查是否存在, 以及是否正确
   */

  // 主要针对响应状态码字段进行检查 是否正确 是否有效(类型是否正确)
  virtual bool check() override;

  virtual RCode rcode();

  virtual void setRCode(RCode);
};
RCode JsonResponse::rcode() { return (RCode)_body[KEY_RCODE].asInt(); }

void JsonResponse::setRCode(RCode _rcode) { _body[KEY_RCODE] = (int)_rcode; }

bool JsonResponse::check() {
  if (_body[KEY_RCODE].isNull()) {
    // rcode 字段不存在
    ELOG("不存在 rcode 字段\n");
    return false;
  }
  if (!_body[KEY_RCODE].isIntegral()) {
    // rcode 字段类型错误
    ELOG("rcode 类型错误\n");
    return false;
  }
  return true; // rcode 字段存在且类型字段类型正确
}

/* -------------RpcRequest------------- */

class RpcRequest : public JsonRequest {
  /*
   * RpcRequest 字段为 JsonRequest 的派生类
   * 主要负责设置字段与获取字段以及对字段进行检查
   * 在 RpcRequest 中存在字段 Method方法, 与Parmas参数字段
   * body:{
   *        method: "xxxxx"(string 字符串类型)
   *        Parmas: {
   *                  params1: xxxxx
   *                  params2: xxxxx
   *                  ....
   *        }(params字段为Json::Value类型字段)
   * }
   */
public:
  using ptr = std::shared_ptr<RpcRequest>;

  virtual bool
  check() override; // 服务端检测 Rpc 请求中 method 与 params 字段是否存在且正确

  std::string method(); // 服务端获取方法字段

  void setMethod(const std::string &); // 客户端设置方法字段

  Json::Value params(); // 服务端获取参数字段

  void setParams(const Json::Value &); // 客户端设置参数字段
};

bool RpcRequest::check() {
  // 主要来检查 method 字段与 params 字段的是否存在于类型是否正确
  if (_body[KEY_METHOD].isNull() ||
      (!_body[KEY_METHOD].isString())) { // 判断Method字段是否存在,
                                         // 且参数类型是否错误(不为字符串)
    ELOG("method 字段不存在 || method 字段类型错误");
    return false;
  }
  if (_body[KEY_PARAMS].isNull() ||
      (!_body[KEY_PARAMS]
            .isObject())) { // 判断Params字段是否存在,
                            // 且参数类型是否错误(不为Json::Value对象)
    ELOG("params 字段不存在 || params 字段类型错误");
    return false;
  }
  return true;
}

std::string RpcRequest::method() { return _body[KEY_METHOD].asString(); }

void RpcRequest::setMethod(const std::string &_method) {
  _body[KEY_METHOD] = _method;
}

Json::Value RpcRequest::params() { return _body[KEY_PARAMS]; }

void RpcRequest::setParams(const Json::Value &_params) {
  _body[KEY_PARAMS] = _params;
}

/* -------------TopicRequest------------- */

class TopicRequest : public JsonRequest {
  /*
   * 同样的主题请求类型中的body字段中包含了三个字段, 分别为key, msg以及optype
   * 分别为主题, 消息以及操作类型
   * body:{
   *    key: "xxxxx"(string 字符串类型)
   *    msg: "xxxxx"(string 字符串类型)
   *    optype: (枚举类型字段 int)
   * }
   * 该类型主要为客户端和服务端提供以下服务:
   *  - 客户端:
   *           设置主题字段
   *           设置操作类型字段
   *           设置消息字段
   *  - 服务端:
   *           检查各字段
   *           获取主题字段
   *           获取操作类型字段
   *           获取消息字段
   */
public:
  using ptr = std::shared_ptr<TopicRequest>;

  virtual bool check() override;

  std::string topicKey();

  void setTopicKey(const std::string &);

  TopicOptype topicOptype();

  void setTopicOptype(TopicOptype);

  std::string topicMsg();

  void setTopicMsg(const std::string &);
};
bool TopicRequest::check() {
  if (_body[KEY_TOPIC_KEY].isNull() ||
      (!_body[KEY_TOPIC_KEY]
            .isString())) { // 检查topic_key字段是否存在或类型是否正确
    ELOG("%s 字段不存在或类型错误\n", KEY_TOPIC_KEY);
    return false;
  }
  if (_body[KEY_OPTYPE].isNull() ||
      (!_body[KEY_OPTYPE].isIntegral())) { // 检查key_optype
                                           // 操作类型字段是否存在或类型是否正确
    ELOG("%s 字段不存在或类型错误\n", KEY_OPTYPE);
    return false;
  }
  if (_body[KEY_OPTYPE].asInt() ==
      (int)TopicOptype::TOPIC_PUBLISH) { // 检查如果操作类型为发布时,
                                         // 是否存在消息字段,
                                         // 且消息字段类型是否正确
    if (_body[KEY_TOPIC_MSG].isNull() || (!_body[KEY_TOPIC_MSG].isString())) {
      ELOG("发布请求中 %s 字段不存在或类型错误\n", KEY_TOPIC_MSG);
      return false;
    }
  }
  return true;
}

std::string TopicRequest::topicKey() { return _body[KEY_TOPIC_KEY].asString(); }

void TopicRequest::setTopicKey(const std::string &_key) {
  _body[KEY_TOPIC_KEY] = _key;
}

TopicOptype TopicRequest::topicOptype() {
  return (TopicOptype)_body[KEY_OPTYPE].asInt();
}

void TopicRequest::setTopicOptype(TopicOptype _optype) {
  _body[KEY_OPTYPE] = (int)_optype;
}

std::string TopicRequest::topicMsg() { return _body[KEY_TOPIC_MSG].asString(); }

void TopicRequest::setTopicMsg(const std::string &_msg) {
  _body[KEY_TOPIC_MSG] = _msg;
}

/* -------------ServiceRequest------------- */

typedef std::pair<std::string, int> Address; // pair 保存对应的ip与port

class ServiceRequest : public JsonRequest {
  /*
   * Service 的请求主要用来进行服务的注册, 发现, 上线, 下线
   * 通常Service请求的结构为如下:
   * body:{
   *      optype: xxxxx (枚举类型, int类型)
   *      method: "Addxxxxx" (string类型 为对应的服务方法)
   *      host(服务发现无该字段): {
   *            ip: "xxx.xxx.xxx.xxx"(string类型字段)
   *            host: xxxx (int类型字段)
   *      }(Json对象类型)
   * }
   */
public:
  using ptr = std::shared_ptr<ServiceRequest>;

  virtual bool check() override; // 服务端进行请求的检查

  std::string method(); // 服务端获取服务名称

  void setMethod(const std::string &); // 客户端设置方法名称

  ServiceOptype serviceOptype(); // 服务端获取操作类型

  void setServiceOptype(ServiceOptype); // 客户端设置操作类型

  Address host(); // 服务端获取host字段

  void setHost(const Address &); // 客户端设置host字段
};

bool ServiceRequest::check() {
  if (_body[KEY_METHOD].isNull() ||
      (!_body[KEY_METHOD]
            .isString())) { // 检查method字段是否存在, 以及类型是否错误
    ELOG("ServiceRequest %s 字段不存在或类型错误\n", KEY_METHOD);
    return false;
  }
  if (_body[KEY_OPTYPE].isNull() ||
      (!_body[KEY_OPTYPE].isIntegral())) { // 检查optype操作类型字段是否存在,
                                           // 以及操作类型的类型是否正确
    ELOG("ServiceRequest %s 字段不存在或类型错误\n", KEY_OPTYPE);
    return false;
  }
  if (_body[KEY_OPTYPE]
          .asInt() != // 判断服务类型是否为服务发现, 服务发现无host字段
      (int)ServiceOptype::SERVICE_DISCOVERY) // 发现服务无需传递主机信息
                                             // 需要注册中心返回给客户端
  {
    if (_body[KEY_HOST].isNull() ||
        (!_body[KEY_HOST].isObject())) { // 若操作类型不为发现,
                                         // 判断host是否字段以及字段类型是否正确
      ELOG("ServiceRequest 服务发现时 %s 字段不存在或类型错误\n", KEY_HOST);
      return false;
    }
    if (_body[KEY_HOST][KEY_HOST_IP]
            .isNull() || // 若操作类型不为发现且存在host字段且host字段类型正确则判断host对象中的ip类型是否存在,
                         // 且类型是否正确
        (!_body[KEY_HOST][KEY_HOST_IP].isString())) {
      ELOG("ServiceRequest 服务发现时 %s 字段不存在或类型错误\n", KEY_HOST_IP);
      return false;
    }
    if (_body[KEY_HOST][KEY_HOST_PORT]
            .isNull() || // 若操作类型不为发现且存在host字段且host字段类型正确则判断host对象中的port类型是否存在且类型是否正确
        (!_body[KEY_HOST][KEY_HOST_PORT].isIntegral())) {
      ELOG("ServiceRequest 服务发现时 %s 字段不存在或类型错误\n",
           KEY_HOST_PORT);
      return false;
    }
  }

  return true;
}

std::string ServiceRequest::method() {
  return _body[KEY_METHOD].asString();
} // 返回method字段

void ServiceRequest::setMethod(const std::string &_method) { // 设置method字段
  _body[KEY_METHOD] = _method;
}

ServiceOptype ServiceRequest::serviceOptype() { // 获取操作类型字段
  return (ServiceOptype)_body[KEY_OPTYPE].asInt();
}

void ServiceRequest::setServiceOptype(
    ServiceOptype _optype) { // 设置操作类型字段
  _body[KEY_OPTYPE] = (int)_optype;
}

Address ServiceRequest::host() { // 返回host字段
  std::pair<std::string, int> host;
  host.first = _body[KEY_HOST][KEY_HOST_IP].asString();
  host.second = _body[KEY_HOST][KEY_HOST_PORT].asInt();
  return host;
}

void ServiceRequest::setHost(const Address &_host) { // 设置host字段
  Json::Value hostValue;
  hostValue[KEY_HOST_IP] = _host.first;
  hostValue[KEY_HOST_PORT] = _host.second;
  _body[KEY_HOST] = hostValue;
}

///////////////////// response ////////////////////////

/* -------------RpcResponse------------- */

class RpcResponse : public JsonResponse {
  /*
   * PrcResponse 主要负责为服务端提供组织Rpc响应与为客户端检查响应,
   * 并提取出响应中的各个字段的类 其结构为
   * body:{
   *      rcode: xxxxxx (枚举类型字段, int类型)
   *      result: {
   *              xxxxxx
   *      } (Json对象类型, 为方便进行返回结果)
   * }
   * 其 rcode 为返回时的响应状态码
   * result 字段则为返回时的结果
   *
   */
public:
  using ptr = std::shared_ptr<RpcResponse>;

  virtual bool check() override;

  Json::Value result();

  void setResult(const Json::Value &);
};

bool RpcResponse::check() {
  if (_body[KEY_RCODE].isNull() ||
      (!_body[KEY_RCODE].isIntegral())) { // 检查rcode字段是否存在且类型是否正确
    ELOG("RpcResopnse %s 字段不存在或类型错误\n", KEY_RCODE);
    return false;
  }
  if (_body[KEY_RESULT].isNull()) { // 检查result字段是否存在且类型是否正确
    ELOG("RpcResopnse %s 字段不存\n", KEY_RESULT);
    return false;
  }
  return true;
}

Json::Value RpcResponse::result() {
  return _body[KEY_RESULT];
} // 客户端获取result字段

void RpcResponse::setResult(const Json::Value &_result) {
  _body[KEY_RESULT] = _result;
} // 服务端设置result字段

/* -------------TopicResponse------------- */

class TopicResponse : public JsonResponse {
  /*
   * Topic 响应没有过多需要检查或设置的, 原生的rcode
   * 可以直接调用父类的check进行检查
   */
public:
  using ptr = std::shared_ptr<TopicResponse>;
};

/* -------------ServiceResponse------------- */

class ServiceResponse : public JsonResponse {
  /*
   * ServiceResponse 为服务端提供响应的设置, 为客户端提供响应信息的提取
   * 对于ServiceResponse而言, 其结构分为两种, 主要是区分发现服务与非发现服务
   * 对非发现服务而言, 对应的响应需要提供对应的操作类型与响应码
   * body{
   *      rccode: xxxx(枚举类型, int)
   *      optype: xxxx(枚举类型, int)
   * }
   *
   * 对于发现服务而言, 需要为客户端响应更多的信息,
   * 以客户端可以直接同过信息对提供该服务的服务端进行连接并调用服务,
   * body{
   *      rcode: xxxx(枚举类型, int)
   *      optype: xxxx(枚举类型, int)
   *      method: "xxxx"(string类型, 想要发现的服务)
   *      hosts: vector<pair<xxx, xxx>>(提供该服务的主机信息列表)
   * }
   */
public:
  using ptr = std::shared_ptr<ServiceResponse>;

  virtual bool check() override;

  std::string method();

  void setMethod(const std::string &);

  std::vector<Address> hosts();

  void setHosts(std::vector<Address>);

  ServiceOptype serviceOptype();

  void setServiceOptype(ServiceOptype);
};

bool ServiceResponse::check() {
  if (_body[KEY_RCODE].isNull() ||
      (!_body[KEY_RCODE]
            .isIntegral())) { // 客户端调用check检查响应的rcode是否存在且是否类型错误
    ELOG("ServiceResopnse %s 字段不存在或类型错误\n", KEY_RCODE);
    return false;
  }
  if (_body[KEY_OPTYPE].isNull() ||
      (!_body[KEY_OPTYPE]
            .isInt())) { // 客户端调用check检查响应的optype字段是否存在且类型是否错误
    ELOG("ServiceResopnse %s 字段不存在或类型错误\n", KEY_OPTYPE);
    return false;
  }

  if ((_body[KEY_OPTYPE].asInt() ==
       (int)ServiceOptype::SERVICE_DISCOVERY) /*判断是否为服务发现类型*/
          && /*服务发现类型需要判断方法字段与主机信息字段是否存在且类型是否有效*/
          (_body[KEY_METHOD].isNull()) ||
      (!_body[KEY_METHOD].isString()) || (_body[KEY_HOST].isNull()) ||
      (!_body[KEY_HOST].isArray())) {
    ELOG("Discoverer ServiceResopnse 信息字段错误\n");
    return false;
  }
  return true;
}

std::string ServiceResponse::method() {
  return _body[KEY_METHOD].asString();
} // 获取method字段

void ServiceResponse::setMethod(const std::string &_method) { // 设置method字段
  _body[KEY_METHOD] = _method;
}

void ServiceResponse::setHosts(std::vector<Address> _hosts) { // 设置_hosts字段
  for (auto &it : _hosts) { // 需要遍历vector
    Json::Value val;
    val[KEY_HOST_IP] = it.first;
    val[KEY_HOST_PORT] = it.second;
    _body[KEY_HOST].append(val);
  }
}

std::vector<Address> ServiceResponse::hosts() {
  std::vector<Address> _hosts;
  int sz = _body[KEY_HOST].size();
  for (int i = 0; i < sz; ++i) {
    Address addr;
    addr.first = _body[KEY_HOST][i][KEY_HOST_IP].asString();
    addr.second = _body[KEY_HOST][i][KEY_HOST_PORT].asInt();
    _hosts.push_back(addr);
  }
  return _hosts;
}

ServiceOptype ServiceResponse::serviceOptype() {
  return (ServiceOptype)_body[KEY_OPTYPE].asInt();
}

void ServiceResponse::setServiceOptype(ServiceOptype _optype) {
  _body[KEY_OPTYPE] = (int)_optype;
}

class MessageFactory { // 建立工厂类, 方便不同的消息类型的创建
public:
  /*模板工厂*/
  template <typename T, typename... Args>
  static std::shared_ptr<T> create(Args &&...args) {
    return std::make_shared<T>(std::forward(args)...);
  }

  /*简单工厂类*/
  static Rpc::BaseMessage::ptr create(Rpc::MType);
};
inline Rpc::BaseMessage::ptr MessageFactory::create(Rpc::MType mtype) {
  switch (mtype) {
  case MType::REQ_RPC:
    return std::make_shared<RpcRequest>();
  case MType::RSP_RPC:
    return std::make_shared<RpcResponse>();
  case MType::REQ_TOPIC:
    return std::make_shared<TopicRequest>();
  case MType::RSP_TOPIC:
    return std::make_shared<TopicResponse>();
  case MType::REQ_SERVICE:
    return std::make_shared<ServiceRequest>();
  case MType::RSP_SERVICE:
    return std::make_shared<ServiceResponse>();
  }
  return BaseMessage::ptr();
}

} // namespace Rpc
