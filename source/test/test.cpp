#include <iostream>

#include <string>

#include "../common/message.hpp"

#include "../common/net.hpp"

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

  /* 实例化Protocol对象 */
  Rpc::BaseProtocol::ptr protocol = Rpc::ProtocolFactory::create();

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

  std::cout << "###################################" << std::endl;

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

int main() {

  rpc_message_test();

  return 0;
}
