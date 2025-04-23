#include "../common/net.hpp"

#include <iostream>

#include <thread>

#include "../common/message.hpp"

#include "../common/dispatcher.hpp"

#include "../client/requestor.hpp"

#include "../client/rpc_caller.hpp"

//////////////////////////// ServerTest ////////////////////////////

void onClientMessage(const Rpc::BaseConnection::ptr &conn,
                     Rpc::BaseMessage::ptr &msg) {
  std::string body = msg->serialize();

  std::cout << body << std::endl;
}

void clientTest() {
  auto client = Rpc::ClientFactory::create("127.0.0.1", 9090);

  client->setMessageCallback(onClientMessage);

  client->connect();

  Rpc::RpcRequest::ptr rpc_req = Rpc::MessageFactory::create<Rpc::RpcRequest>();

  rpc_req->setId(Rpc::UUID::uuid());

  rpc_req->setMType(Rpc::MType::REQ_RPC);

  rpc_req->setMethod("Add");

  Json::Value root;
  root["num1"] = 666;
  root["num2"] = 777;
  root["tip"] = "This is a Test for server/tset";
  rpc_req->setParams(root);

  client->send(rpc_req);
}

int main() {
  clientTest();
  return 0;
}
