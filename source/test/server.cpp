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

//////////////////////////// DispatcherTest ////////////////////////////

void dispatcherOnRpcRequest(const Rpc::BaseConnection::ptr &con,
                            Rpc::RpcRequest::ptr &msg) {
  std::cout << "get a rpc_request:" << std::endl;
  std::string body = msg->serialize();
  std::cout << body << std::endl;

  Rpc::RpcResponse::ptr rrp = Rpc::MessageFactory::create<Rpc::RpcResponse>();
  rrp->setId(Rpc::UUID::uuid());
  rrp->setMType(Rpc::MType::RSP_RPC);
  rrp->setRCode(Rpc::RCode::RCODE_OK);

  con->send(rrp);
}

void dispatcherOnTopicRequest(const Rpc::BaseConnection::ptr &con,
                              Rpc::TopicRequest::ptr &msg) {
  std::cout << "get a topic_request:" << std::endl;
  std::string body = msg->serialize();
  std::cout << body << std::endl;

  Rpc::TopicResponse::ptr trp =
      Rpc::MessageFactory::create<Rpc::TopicResponse>();
  trp->setId(Rpc::UUID::uuid());
  trp->setMType(Rpc::MType::RSP_TOPIC);
  trp->setRCode(Rpc::RCode::RCODE_OK);

  con->send(trp);
}

void dispatcherTest() {
  auto dispatcher = std::make_shared<Rpc::Dispatcher>();
  dispatcher->registerHandler<Rpc::RpcRequest>(Rpc::MType::REQ_RPC,
                                               dispatcherOnRpcRequest);
  dispatcher->registerHandler<Rpc::TopicRequest>(Rpc::MType::REQ_TOPIC,
                                                 dispatcherOnTopicRequest);

  auto server = Rpc::ServerFactory::create(9090);
  auto message_cb =
      std::bind(&Rpc::Dispatcher::onMessage, dispatcher.get(),
                std::placeholders::_1, std::placeholders::_2); // 绑定this指针

  server->setMessageCallback(message_cb);

  server->start();
}

int main() {

  //  serverTest();

  dispatcherTest();

  return 0;
}
