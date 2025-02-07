#include "net.hpp"

#include <iostream>

#include "dispatcher.hpp"

// void onMessage(const Rpc::BaseConnection::ptr &con, Rpc::BaseMessage::ptr &msg)
// {
//     // TODO -- 可能需要重新解读
//     // 对请求的一个处理
//     /*
//         具体处理情况:
//         MuduoServer 是对muduo库中server的封装
//         当事件触发时muduo将会调用MuduoServer设置的回调
//         这个回调的onMessage将会通过protocol做:
//         1. 判断数据量是否足够
//         2. 判断MuduoServer中的哈希表是否存在
//             muduo::net::TcpConnectionPtr和Rpc::BaseConnection::ptr的映射
//         3. 获取遍历哈希表后的BaseConnection::ptr
//         4. 调用BaseProtocol::onMessage传入一个BaseBuffer::ptr作输入参数
//             并通过输出参数的方式获取BaseMessage::ptr
//         5. 判断上层是否设置MuduoServer的message回调，有则调用
//             调用回调时传递BaseMessage::ptr与BaseConnection::ptr
//
//         // 此回调函数即为上述"上层是否设置message回调"
//     */
//
//     // 对请求的消息进行处理
//     std::string body = msg->serialize();
//     std::cout << body << std::endl;
//
//     // 构建一个响应 并调用send
//     auto rpc_rsp = Rpc::MessageFactory::create<Rpc::RpcResponse>();
//     rpc_rsp->setId("12123");
//     rpc_rsp->setMType(Rpc::MType::RSP_RPC);
//     rpc_rsp->setRCode(Rpc::RCode::RCODE_OK);
//     rpc_rsp->setResult(100);
//
//     con->send(rpc_rsp);
// }
//

void onRpcRequest(const Rpc::BaseConnection::ptr &con, Rpc::RpcRequest::ptr &msg)
{
    std::cout << "收到一个来自客户端的Rpc请求: \n";
    std::string body = msg->serialize();
    std::cout << body << std::endl;
    std::cout << "reqmethod: " << msg->method() << std::endl;

    //////////////////////////////
    auto rpc_rsp = Rpc::MessageFactory::create<Rpc::RpcResponse>();

    rpc_rsp->setId("1212123");
    rpc_rsp->setMType(Rpc::MType::RSP_RPC);
    rpc_rsp->setRCode(Rpc::RCode::RCODE_OK);
    rpc_rsp->setResult("this is a RpcResponse");
    con->send(rpc_rsp);
}

void onTopicRequest(const Rpc::BaseConnection::ptr &con, Rpc::TopicRequest::ptr &msg)
{
    std::cout << "收到一个来自客户端的Topic请求: \n";
    std::string body = msg->serialize();
    std::cout << body << std::endl;
    //////////////////////////////
    std::cout << "topicmsg: " << msg->topicMsg() << std::endl;
    auto topic_rsp = Rpc::MessageFactory::create<Rpc::TopicResponse>();

    topic_rsp->setId("99999");
    topic_rsp->setMType(Rpc::MType::RSP_TOPIC);
    topic_rsp->setRCode(Rpc::RCode::RCODE_OK);

    con->send(topic_rsp);
}

int main()
{

    auto dispatcher = std::make_shared<Rpc::Dispatcher>();

    // 注册回调函数
    dispatcher->registerHandler<Rpc::RpcRequest>(Rpc::MType::REQ_RPC, onRpcRequest);
    dispatcher->registerHandler<Rpc::TopicRequest>(Rpc::MType::REQ_TOPIC, onTopicRequest);

    Rpc::BaseServer::ptr server = Rpc::ServerFactory::create(8111);

    // 进行bind绑定(this指针)确保不会出现因参数数量不正确导致无法正确传参
    auto message_cb = std::bind(&Rpc::Dispatcher::onMessage, dispatcher.get(), std::placeholders::_1, std::placeholders::_2);

    server->setMessageCallback(message_cb);

    server->start();

    return 0;
}
