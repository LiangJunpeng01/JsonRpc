#include <iostream>

#include <string>

#include <vector>

#include "../common/message.hpp"

#include "../common/net.hpp"

#include "../common/detail.hpp"

/* 实例化Protocol对象 */
Rpc::BaseProtocol::ptr protocol = Rpc::ProtocolFactory::create();

int Add(int num1, int num2) { return num1 + num2; }

void rpc_message_test() {

  /////////////////RpcRequest/////////////////

  /* 构造请求 */
  Rpc::RpcRequest::ptr rrp = Rpc::MessageFactory::create<Rpc::RpcRequest>();
  rrp->setMethod("Add");
  Json::Value params_root;

  params_root["num1"] = 10;
  params_root["num2"] = 11;

  rrp->setParams(params_root);

  rrp->setId(Rpc::UUID::uuid());

  rrp->setMType(Rpc::MType::REQ_RPC);

  std::string body = rrp->serialize();

  /* 打印请求 */
  std::cout << body << std::endl;

  /* 进行序列化 */
  protocol->serialize(rrp);

  Rpc::BaseMessage::ptr bmp_ = Rpc::MessageFactory::create(Rpc::MType::REQ_RPC);

  bmp_->unserialize(body);

  // 检查
  if (bmp_->check()) {
    std::cout << "check 结果正常" << std::endl;
  }

  // 基类指针转换为子类指针
  Rpc::RpcRequest::ptr rrp_ = std::dynamic_pointer_cast<Rpc::RpcRequest>(bmp_);

  // 接收参数
  Json::Value params = rrp_->params();
  std::cout << rrp_->method() << std::endl;
  int num1 = params["num1"].asInt();
  int num2 = params["num2"].asInt();
  std::cout << num1 << " " << num2 << std::endl;

  std::cout << "-------------------------------------------" << std::endl;

  /////////////////RpcResponse/////////////////

  Rpc::RpcResponse::ptr rsp = Rpc::MessageFactory::create<Rpc::RpcResponse>();

  int result = Add(num1, num2);

  rsp->setId(Rpc::UUID::uuid());

  rsp->setMType(Rpc::MType::RSP_RPC);

  rsp->setResult(result);

  rsp->setRCode(Rpc::RCode::RCODE_OK);

  std::string rsp_body = rsp->serialize();

  std::cout << rsp_body << std::endl;

  Rpc::BaseMessage::ptr bmp_rsp =
      Rpc::MessageFactory::create(Rpc::MType::RSP_RPC);

  bmp_rsp->unserialize(rsp_body);

  if (bmp_rsp->check()) {
    std::cout << "响应check成功" << std::endl;
  }

  Rpc::RpcResponse::ptr rsp_ =
      std::dynamic_pointer_cast<Rpc::RpcResponse>(bmp_rsp);

  result = rsp_->result().asInt();
  int rcode = (int)rsp_->rcode();

  std::cout << result << " " << rcode << std::endl;
}

void topic_message_test() {
  /////////////////////// Request ///////////////////////

  // 构建请求
  Rpc::TopicRequest::ptr trp = Rpc::MessageFactory::create<Rpc::TopicRequest>();

  trp->setMType(Rpc::MType::REQ_TOPIC);

  trp->setId(Rpc::UUID::uuid());

  trp->setTopicKey("sport");

  trp->setTopicMsg("This is a test for topic");

  trp->setTopicOptype(Rpc::TopicOptype::TOPIC_SUBSCRIBE);

  //  trp->setTopicOptype(Rpc::TopicOptype::TOPIC_PUBLISH); // 发布需要msg字段

  std::string req_body = trp->serialize();

  std::cout << req_body << std::endl;

  Rpc::BaseMessage::ptr bmp =
      Rpc::MessageFactory::create(Rpc::MType::REQ_TOPIC);

  bmp->unserialize(req_body);

  if (bmp->check()) {
    std::cout << "bmp->check() ok" << std::endl;
  } else
    return;

  Rpc::TopicRequest::ptr trp_ =
      std::dynamic_pointer_cast<Rpc::TopicRequest>(bmp);

  if (trp_->check()) {
    std::cout << "trp_->chech() ok" << std::endl;
  } else
    return;

  std::cout << trp_->topicKey() << std::endl;
  std::cout << trp_->topicMsg() << std::endl;
  std::cout << (int)trp_->topicOptype() << std::endl;
  std::cout << "-------------------------------------------" << std::endl;

  /////////////////////// Response ///////////////////////

  Rpc::TopicResponse::ptr rsp =
      Rpc::MessageFactory::create<Rpc::TopicResponse>();

  rsp->setId(Rpc::UUID::uuid());

  rsp->setMType(Rpc::MType::RSP_TOPIC);

  rsp->setRCode(Rpc::RCode::RCODE_OK);

  std::string rsp_body = rsp->serialize();

  std::cout << rsp_body << std::endl;

  Rpc::BaseMessage::ptr rsp_ =
      Rpc::MessageFactory::create(Rpc::MType::RSP_TOPIC);

  rsp_->unserialize(rsp_body);

  if (rsp_->check())
    std::cout << "rsp_->check() ok" << std::endl;
  else
    return;

  Rpc::TopicResponse::ptr rsp_trp =
      std::dynamic_pointer_cast<Rpc::TopicResponse>(rsp_);

  if (rsp_trp->check()) {
    std::cout << "rsp_trp->check() ok" << std::endl;
  } else
    return;

  std::cout << (int)rsp->rcode() << std::endl;
}

void service_message_test() {

  /////////////////////// Request ///////////////////////
  Rpc::ServiceRequest::ptr req =
      Rpc::MessageFactory::create<Rpc::ServiceRequest>();

  req->setMType(Rpc::MType::REQ_SERVICE);

  req->setId(Rpc::UUID::uuid());

  req->setServiceOptype(Rpc::ServiceOptype::SERVICE_REGISTRY);

  req->setHost({"127.0.0.1", 8088});

  req->setMethod("Add");

  std::string req_body = req->serialize();

  std::cout << req_body << std::endl;

  Rpc::BaseMessage::ptr bmp_req =
      Rpc::MessageFactory::create(Rpc::MType::REQ_SERVICE);

  bmp_req->unserialize(req_body);

  if (bmp_req->check()) {
    ILOG("bmp_req->check() ok");
  } else {
    ELOG("bmp_req->check() ng");
  }

  Rpc::ServiceRequest::ptr srp_req =
      std::dynamic_pointer_cast<Rpc::ServiceRequest>(req);

  if (srp_req->check()) {
    ILOG("srp_req->check() ok");
  } else {
    ELOG("srp_req->check() ng");
  }

  std::cout << (int)srp_req->serviceOptype() << " " << srp_req->method()
            << std::endl;
  std::cout << "IP: " << srp_req->host().first << std::endl
            << "Port: " << srp_req->host().second << std::endl;

  /////////////////////// Response ///////////////////////

  Rpc::ServiceResponse::ptr rsp =
      Rpc::MessageFactory::create<Rpc::ServiceResponse>();

  std::vector<Rpc::Address> hosts;

  hosts.push_back({"127.0.0.1", 8088});
  hosts.push_back({"127.0.0.2", 8098});
  hosts.push_back({"127.0.0.3", 8088});

  rsp->setHosts(hosts);
}

int main() {

  rpc_message_test(); // Rpc 消息测试

  std::cout << std::endl
            << "-------------------------------------------" << std::endl;
  std::cout << "#######################################" << std::endl;
  std::cout << "#######################################" << std::endl;
  std::cout << "-------------------------------------------" << std::endl
            << std::endl;

  topic_message_test(); // Topic 消息测试

  std::cout << std::endl
            << "-------------------------------------------" << std::endl;
  std::cout << "#######################################" << std::endl;
  std::cout << "#######################################" << std::endl;
  std::cout << "-------------------------------------------" << std::endl
            << std::endl;

  service_message_test();
  return 0;
}
