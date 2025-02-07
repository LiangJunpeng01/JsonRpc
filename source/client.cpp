#include "net.hpp"

#include <iostream>

#include <thread>

#include "dispatcher.hpp"

// void onMessage(const Rpc::BaseConnection::ptr &con, Rpc::BaseMessage::ptr &msg)
// {
//     // 对请求的消息进行处理
//     std::string body = msg->serialize();
//     std::cout << body << std::endl;
// }

void onRpcResponse(const Rpc::BaseConnection::ptr &con, Rpc::RpcResponse::ptr &msg)
{
    std::cout << "收到一个来自服务端的Rpc响应: \n";
    std::string body = msg->serialize();
    std::cout << body << std::endl;
}

void onTopicResponse(const Rpc::BaseConnection::ptr &con, Rpc::TopicResponse ::ptr &msg)
{
    std::cout << "收到一个来自服务端的Topic响应: \n";
    std::string body = msg->serialize();
    std::cout << body << std::endl;
}

int main()
{
    auto dispatcher = std::make_shared<Rpc::Dispatcher>();

    dispatcher->registerHandler<Rpc::RpcResponse>(Rpc::MType::RSP_RPC, onRpcResponse);
    dispatcher->registerHandler<Rpc::TopicResponse>(Rpc::MType::RSP_TOPIC, onTopicResponse);

    Rpc::BaseClient::ptr client = Rpc::ClientFactory::create("127.0.0.1", 8111); // 设置需要连接的地址

    auto messag_cb = std::bind(&Rpc::Dispatcher::onMessage, dispatcher.get(), std::placeholders::_1, std::placeholders::_2);

    client->setMessageCallback(messag_cb);

    client->connect();

    auto rpc_req = Rpc::MessageFactory::create<Rpc::RpcRequest>();
    rpc_req->setId("55555");
    rpc_req->setMType(Rpc::MType::REQ_RPC);
    rpc_req->setMethod("Mov");
    Json::Value root;
    root["num1"] = 111;
    root["num2"] = 222;
    rpc_req->setParams(root);

    client->send(rpc_req);

    auto topic_req = Rpc::MessageFactory::create<Rpc::TopicRequest>();
    topic_req->setId("66666");
    topic_req->setMType(Rpc::MType::REQ_TOPIC);
    topic_req->setTopicKey("sport");
    topic_req->setTopicMsg("YaoMing");
    topic_req->setTopicOptype(Rpc::TopicOptype::TOPIC_CREATE);
    client->send(topic_req);

    std::this_thread::sleep_for(std::chrono::seconds(50));

    ////////////////////////////////////////////////////////
    // Rpc::BaseClient::ptr client = Rpc::ClientFactory::create("127.0.0.1", 8111);

    // client->setMessageCallback(onMessage);

    // client->connect();

    // auto rpc_req = Rpc::MessageFactory::create<Rpc::RpcRequest>();
    // rpc_req->setId("12123");
    // rpc_req->setMType(Rpc::MType::RSP_RPC);
    // rpc_req->setMethod("Mov");
    // Json::Value root;
    // root["num1"] = 111;
    // root["num2"] = 222;
    // rpc_req->setParams(root);

    // client->send(rpc_req);

    // std::this_thread::sleep_for(std::chrono::seconds(5));

    return 0;
}