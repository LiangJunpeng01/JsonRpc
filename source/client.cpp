#include "net.hpp"

#include <iostream>

#include <thread>

void onMessage(const Rpc::BaseConnection::ptr &con, Rpc::BaseMessage::ptr &msg)
{
    // 对请求的消息进行处理
    std::string body = msg->serialize();
    std::cout << body << std::endl;
}

int main()
{
    Rpc::BaseClient::ptr client = Rpc::ClientFactory::create("127.0.0.1", 8111);

    client->setMessageCallback(onMessage);

    client->connect();

    auto rpc_req = Rpc::MessageFactory::create<Rpc::RpcRequest>();
    rpc_req->setId("12123");
    rpc_req->setMType(Rpc::MType::RSP_RPC);
    rpc_req->setMethod("Mov");
    Json::Value root;
    root["num1"] = 111;
    root["num2"] = 222;
    rpc_req->setParams(root);

    client->send(rpc_req);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    return 0;
}