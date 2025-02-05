
#include "net.hpp"

#include <iostream>

void onMessage(const Rpc::BaseConnection::ptr &con, Rpc::BaseMessage::ptr &msg)
{
    // 对请求的一个处理
    /*
        具体处理情况:
        MuduoServer 是对muduo库中server的封装
        当事件触发时muduo将会调用MuduoServer设置的回调
        这个回调的onMessage将会通过protocol做:
        1. 判断数据量
        2. 判断MuduoServer中的哈希表是否存
            在muduo::net::TcpConnectionPtr和Rpc::BaseConnection::ptr的映射
        3. 获取遍历哈希表后的BaseConnection::ptr
        4. 调用BaseProtocol::onMessage传入一个BaseBuffer::ptr作输入性参数
            并通过输出型参数的方式获取BaseMessage::ptr
        5. 判断上层是否设置MuduoServer的message回调 有则调用
            调用回调时传递BaseMessage::ptr与BaseConnection::ptr


        // 此回调函数即为上述"上层是否设置message回调"
    */

    // 对请求的消息进行处理
    std::string body = msg->serialize();
    std::cout << body << std::endl;

    // 构建一个响应 并调用send
    auto rpc_rsp = Rpc::MessageFactory::create<Rpc::RpcResponse>();
    rpc_rsp->setId("12123");
    rpc_rsp->setMType(Rpc::MType::RSP_RPC);
    rpc_rsp->setRCode(Rpc::RCode::RCODE_OK);
    rpc_rsp->setResult(100);

    con->send(rpc_rsp);
}

int main()
{
    Rpc::BaseServer::ptr server = Rpc::ServerFactory::create(8111);

    server->setMessageCallback(onMessage);

    server->start();

    return 0;
}