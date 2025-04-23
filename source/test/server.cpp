#include <iostream>

#include "../common/message.hpp"

#include "../common/net.hpp"

#include "../common/dispatcher.hpp"

//////////////////////////// ServerTest ////////////////////////////

void onServerMessage(const Rpc::BaseConnection::ptr &conn,
                     Rpc::BaseMessage::ptr &msg) {
  std::string body = msg->serialize();
  std::cout << body << std::endl;
  Rpc::RpcResponse::ptr rrp = Rpc::MessageFactory::create<Rpc::RpcResponse>();

  rrp->setId(Rpc::UUID::uuid());
  rrp->setResult("Test result");
  rrp->setMType(Rpc::MType::RSP_RPC);
  rrp->setRCode(Rpc::RCode::RCODE_OK);

  conn->send(rrp);
}

void serverTest() {
  auto server = Rpc::ServerFactory::create(9090);

  server->setMessageCallback(onServerMessage);

  server->start();
}

int main() {

  serverTest();

  return 0;
}
