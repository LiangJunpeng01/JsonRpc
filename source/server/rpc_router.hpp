#pragma once

#include "../common/net.hpp"

#include "../common/message.hpp"

#include "../common/fields.hpp"

/*
 * 在RpcRouter模块中, 其中RpcRouter模块是暴露给用户所使用,
 * 主要暴露两个接口, 分别为register注册接口与onRpcRequest接口
 * onRpcRequest接口用于注册进Dispatcher模块,
 * Register接口用于给用户进行具体方法的注册
 * 当一条请求被Dispatcher分发给RpcRouter路由模块后,
 * 准确是调用RpcRouter模块中的onRpcRequest函数被调用
 * 在onRpcRequest函数中,
 * 会检查ServiceManage中的哈希表中是否存在对应方法名的方法, 若是有则进行调用,
 * 若是没有则直接返回一个Response表示调用出错
 *
 * 和具体方法有关的服务管理ServiceManage中存在一个哈希表用于存储方法名称与具体方法描述的映射(pair)
 * 同时提供了增删查的功能
 *
 * 定义了一个新的类为ServiceDescribe作为具体方法的描述,
 * 具体方法描述中包含服务的名称, 服务的具体调用函数, 服务的参数,
 * 以及服务的返回值与各种检查, 枚举类Vtype也用于对各字段的检查,
 * ServiceDescribeFactory是一个建造者模式用于实例化出一个ServiceDescribe实例对象
 *
 */

namespace Rpc {
namespace Server {
/* -------------------- enum class VType --------------------- */
enum class VType {
  BOOL = 0, // 布尔类型
  INTEGRAL, // 整形
  NUMERIC,  // 浮点数类型
  STRING,   // 字符串类型
  ARRAY,    // 数组类型
  OBJECT    // Json 对象类型
};

/* -------------------- class ServiceDescribe --------------------- */

class ServiceDescribe // 服务描述
{
  /*
   * ServiceDescribe 模块主要是用于描述一个具体方法的, 如Add等
   * 并提供相应的检查机制, 即对参数的校验
   */
public:
  using ptr = std::shared_ptr<ServiceDescribe>;
  using ServiceCallback = std::function<void(
      const Json::Value &,
      Json::Value &)>; // 实际业务处理函数, 两个Json对象其中一个是参数,
                       // 一个是返回值, 输入型(第一个)参数为参数,
                       // 输出型参数(第二个)为返回值
  using ParamsDescribe =
      std::pair<std::string /* 参数字段名称 */, VType /* 参数字段类型 */>;

  ServiceDescribe(std::string &&mname, std::vector<ParamsDescribe> &&descs,
                  VType vtype, ServiceCallback &&handler)
      : _method_name(std::move(mname)), _callback(std::move(handler)),
        _params(std::move(descs)), _result(vtype) {}
  const std::string &method();
  bool paramsCheck(const Json::Value &); // 进行参数校验
  bool call(const Json::Value &params, Json::Value &result);

private:
  bool check(const Json::Value &, VType);
  bool resultCheck(const Json::Value &); // 进行返回值类型校验

private:
  std::string _method_name;  // 方法名称
  ServiceCallback _callback; // 实际业务回调
  std::vector<ParamsDescribe>
      _params; // 一个服务可能需要多个参数,
               // 因此采用一个vector容器来存储对应的参数,
               // 其中ParamsDescribe是一个pair, 用来映射参数与其对应类型的字段
  VType _result; // 返回值类型描述
}; // class ServiceDerscibe

bool ServiceDescribe::check(const Json::Value &val, VType vtype) {
  // 传入一个Vtype用于校验, 随后根据switch-case来判断vtype的具体类型,
  // 判断val参数类型是否与预置类型一致,
  switch (vtype) {
  case VType::BOOL:
    return val.isBool();
  case VType::INTEGRAL:
    return val.isIntegral();
  case VType::NUMERIC:
    return val.isNumeric();
  case VType::STRING:
    return val.isString();
  case VType::ARRAY:
    return val.isArray();
  case VType::OBJECT:
    return val.isObject();
  }
  return false;
}

bool ServiceDescribe::paramsCheck(const Json::Value &params) {
  // 主要用于判断所描述参数字段是否存在, 且类型是否一致
  for (auto &param_desc :
       _params) { // 通过范围for去遍历该服务中的参数描述vector<pair<>>
                  // _params来检查字段是否存在
    if (!params.isMember(param_desc.first)) { // 检查字段是否存在, pair<>::first
                                              // 中存储的是一个string类型
      ELOG("%s 参数字段缺失\n", param_desc.first.c_str());
      return false;
    }
    if (!check(
            params[param_desc.first],
            param_desc
                .second)) { // check函数需要传入两个参数,
                            // 分别为参数名称对应的参数值与参数类型ValueType
                            // 通过取出对应的值,
                            // 并在check函数中进行构造成Value对象,
                            // 直接对val进行isXXXX的类型判断来判断类型是否正确
      ELOG("%s 参数字段类型不匹配\n", param_desc.first.c_str());
      return false;
    }
  }
  return true;
}

bool ServiceDescribe::resultCheck(const Json::Value &result) {
  return check(result, _result);
}

bool ServiceDescribe::call(const Json::Value &params, Json::Value &result) {
  // 调用业务处理函数, result为输出型参数, 结果返回后需要对结果的类型进行判断
  _callback(params, result);
  if (!resultCheck(result)) {
    ELOG("响应信息校验失败\n");
    return false;
  }
  return true;
}

const std::string &ServiceDescribe::method() { return _method_name; }

/* -------------------- class ServiceDescribeFactory --------------------- */

class ServiceDescribeFactory // 建造者模式
{
  /*
   * 该类主要通过set函数将参数设置进ServiceDescribeFactory建造者中
   * 再通过该建造者模式中的build去构造一个ServiceDescribe实例
   */
public:
  // set 设置参数.......
  void setCallBack(const ServiceDescribe::ServiceCallback &);
  void setParamsDescribe(const std::string &, VType);
  void setResultType(VType);
  void setMethodName(const std::string &);

  ServiceDescribe::ptr build() {
    return std::make_shared<ServiceDescribe>(std::move(_method_name),
                                             std::move(_params), _resulttype,
                                             std::move(_callback));
  }

private:
  std::string _method_name; // 方法名称

  ServiceDescribe::ServiceCallback _callback;           // 实际业务回调
  std::vector<ServiceDescribe::ParamsDescribe> _params; // 参数描述
  VType _resulttype;                                    // 返回值类型
};
void ServiceDescribeFactory::setMethodName(const std::string &mname) {
  _method_name = mname;
}

void ServiceDescribeFactory::setCallBack(
    const ServiceDescribe::ServiceCallback &callback) {
  _callback = callback;
}
void ServiceDescribeFactory::setParamsDescribe(const std::string &pname,
                                               VType vtype) {
  _params.push_back(ServiceDescribe::ParamsDescribe(pname, vtype));
}
void ServiceDescribeFactory::setResultType(VType vtype) { _resulttype = vtype; }

/* -------------------- class ServiceManager --------------------- */

class ServiceManager {
public:
  using ptr = std::shared_ptr<ServiceManager>;
  void insert(const ServiceDescribe::ptr &);        // 新增一个服务
  ServiceDescribe::ptr select(const std::string &); // 选择一个服务
  void remove(const std::string &);                 // 删除一个服务
private:
  std::mutex _mutex; // 保护临界资源_services
  std::unordered_map<std::string, ServiceDescribe::ptr>
      _services; // 用于存储具体服务名称与服务描述的映射的哈希表
}; // class ServiceManager

void ServiceManager::insert(const ServiceDescribe::ptr &desc) {
  std::unique_lock<std::mutex> lock(_mutex);
  _services.insert({desc->method(), desc});
}
ServiceDescribe::ptr ServiceManager::select(const std::string &mname) {
  std::unique_lock<std::mutex> lock(
      _mutex); // 选择对应的服务的服务描述并返回对应的服务描述
  auto desc = _services.find(mname);
  if (desc == _services.end()) {
    return ServiceDescribe::ptr();
  }
  return desc->second;
}
void ServiceManager::remove(
    const std::string &mname) { // 删除哈希表中的对应的某个服务

  std::unique_lock<std::mutex> lock(_mutex);
  _services.erase(mname);
}

/* -------------------- calss RpcRouter --------------------- */

class RpcRouter {
public:
  using ptr = std::shared_ptr<RpcRouter>;
  RpcRouter() : manager_(std::make_shared<ServiceManager>()) {}
  void onRpcRequest(const BaseConnection::ptr &,
                    RpcRequest::ptr &); // 注册进Dispatcher的回调
  void registerMethod(const ServiceDescribe::ptr &); // 注册服务
  // 用户通过调用建造模式来创建一个 ServiceDescribe::ptr 来设置参数描述

private:
  void response(const BaseConnection::ptr &, const RpcRequest::ptr &,
                const Json::Value &, RCode);

private:
  ServiceManager::ptr manager_; // 服务管理器
}; // class RpcRouter

// Rpc::Router::onRpcRequest 函数是注册进Dispatcher中的函数,
// 当消息到来时将会检查消息的MType,
// 当消息为RpcRequest时在哈希表中寻找映射并调用onRpcRequest函数对Rpc请求进行处理
void RpcRouter::onRpcRequest(const BaseConnection::ptr &con,
                             RpcRequest::ptr &req) {
  /*
      1. 查询客户端所发的请求描述 判断是否能提供服务
      2. 进行参数校验 判断参数是否正确
      3. 调用业务处理函数
      4. 处理完毕后组织响应 返回结果
  */
  // 判断是否能够提供服务
  auto service = manager_->select(
      req->method()); // 传入一个method方法名称在哈希表中寻找是否存在可提供的方法

  if (!service.get()) {
    // 失败 不存在可提供的服务 需要构建响应并返回
    ELOG("未找到对应服务: %s\n", req->method().c_str());
    return response(con, req, Json::Value(), RCode::RCODE_NOT_FOUND_SERVICE);
  }
  // 进行参数校验
  if (!service->paramsCheck(
          req->params())) { // params() 是一个JsonObject对象类型

    ELOG("参数校验失败: %s\n", req->method().c_str());
    return response(con, req, Json::Value(), RCode::RCODE_INVALID_PARAMS);
  }

  // 调用业务处理函数
  Json::Value result;
  if (!service->call(req->params(), result)) {
    ELOG("返回值类型校验失败: %s\n", req->method().c_str());
    return response(con, req, Json::Value(), RCode::RCODE_INTERNAL_ERROR);
  }
  // 组织响应 发送给客户端
  response(con, req, result, RCode::RCODE_OK);
}

void RpcRouter::response(const BaseConnection::ptr &con,
                         const RpcRequest::ptr &req, const Json::Value &ret,
                         RCode rcode) {
  // 只允许成功
  RpcResponse::ptr msg = MessageFactory::create<RpcResponse>();
  msg->setId(req->rid());
  msg->setMType(MType::RSP_RPC);
  msg->setRCode(rcode);
  msg->setResult(ret);
  con->send(msg);
}

void RpcRouter::registerMethod(const ServiceDescribe::ptr &desc) {
  manager_->insert(desc);
}
} // namespace Server

} // namespace Rpc
