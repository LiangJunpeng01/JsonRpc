#include "../common/net.hpp"

#include <iostream>

#include <thread>

#include <chrono>

#include "../common/message.hpp"

#include "../common/dispatcher.hpp"

#include "../client/requestor.hpp"

#include "../client/rpc_caller.hpp"

//////////////////////////// ServerTest ////////////////////////////

void onClientMessage(const Rpc::BaseConnection::ptr &conn,
                     Rpc::BaseMessage::ptr &msg) {
  std::cout << "client get a new message" << std::endl;
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

  std::this_thread::sleep_for(std::chrono::seconds(3));

  client->shutdown();
}

//////////////////////////// DispatcherTest ////////////////////////////

void dispatcherOnRpcResponse(const Rpc::BaseConnection::ptr &con,
                             Rpc::RpcResponse::ptr &msg) {
  std::cout << "get a rpc_response:" << std::endl;
  std::string body = msg->serialize();
  std::cout << body << std::endl;
}

void dispatcherOnTopicResponse(const Rpc::BaseConnection::ptr &con,
                               Rpc::TopicResponse::ptr &msg) {

  std::cout << "get a topic_response:" << std::endl;
  std::string body = msg->serialize();
  std::cout << body << std::endl;
}
void dispatcherTest() {
  auto dispatcher = std::make_shared<Rpc::Dispatcher>();
  dispatcher->registerHandler<Rpc::RpcResponse>(Rpc::MType::RSP_RPC,
                                                dispatcherOnRpcResponse);

  dispatcher->registerHandler<Rpc::TopicResponse>(Rpc::MType::RSP_TOPIC,
                                                  dispatcherOnTopicResponse);
  auto client = Rpc::ClientFactory::create("127.0.0.1", 9090);

  auto message_cb = std::bind(&Rpc::Dispatcher::onMessage, dispatcher.get(),
                              std::placeholders::_1, std::placeholders::_2);
  client->setMessageCallback(message_cb);

  client->connect();

  Rpc::RpcRequest::ptr rrp = Rpc::MessageFactory::create<Rpc::RpcRequest>();
  rrp->setId(Rpc::UUID::uuid());
  rrp->setMType(Rpc::MType::REQ_RPC);
  rrp->setMethod("This is Test Method");
  Json::Value params_1;
  params_1["test num1"] = 12;
  params_1["test num2"] = 34;
  rrp->setParams(params_1);

  Rpc::TopicRequest::ptr trp = Rpc::MessageFactory::create<Rpc::TopicRequest>();
  trp->setId(Rpc::UUID::uuid());
  trp->setMType(Rpc::MType::REQ_TOPIC);
  trp->setTopicOptype(Rpc::TopicOptype::TOPIC_PUBLISH);
  trp->setTopicKey("sport");
  trp->setTopicMsg("This is a Test for topic - (sport)");

  client->send(rrp);
  client->send(trp);

  std::this_thread::sleep_for(std::chrono::seconds(2));

  client->shutdown();
}

int main() {

  // clientTest();

  dispatcherTest();

  return 0;
}
