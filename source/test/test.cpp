#include <iostream>

#include <string>

#include "../common/message.hpp"

#include "../common/net.hpp"

int Add(int num1, int num2) { return num1 + num2; }

void message_test() {

  /////////////////RpcRequest/////////////////

  Rpc::RpcRequest::ptr rrp = Rpc::MessageFactory::create<Rpc::RpcRequest>();
  rrp->setMethod("Add");
  Json::Value params_root;

  params_root["num1"] = 10;
  params_root["num2"] = 11;

  rrp->setParams(params_root);

  rrp->setId(Rpc::UUID::uuid());

  rrp->setMType(Rpc::MType::REQ_RPC);

  std::string body = rrp->serialize();

  std::cout << body << std::endl;

  Rpc::LVProtocol::ptr protocol = std::make_shared<Rpc::LVProtocoltocol>();
}

int main() {

  message_test();

  return 0;
}
