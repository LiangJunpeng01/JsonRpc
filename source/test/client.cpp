#include "../common/net.hpp"

#include <iostream>

#include <thread>

#include "../common/dispatcher.hpp"

#include "../client/requestor.hpp"

#include "../client/rpc_caller.hpp"

int main()
{

    // 实例化出一个requestor模块和caller模块
    // caller模块负责让上层用户调用对应的方法
    // requestor模块负责为caller模块提供底层支持 并存储对应的请求响应的请求描述对应的映射
    // 上层通过 Rpc::Client::RpcCaller::call 发送请求 在请求发送时将根据实际的调用函数来进行对应的请求发送

    auto client = Rpc::ClientFactory::create("127.0.0.1", 8111); // 实例化一个客户端对象

    auto requestor = std::make_shared<Rpc::Client::Requestor>(); //

    auto caller = std::make_shared<Rpc::Client::RpcCaller>(requestor); // 用户通过RpcCaller::call 发送请求

    auto dispatcher = std::make_shared<Rpc::Dispatcher>(); // 事件派发器 此处的事件派发器用来接收对应的RpcResponse并调用对应的回调来处理该RpcResponse

    auto cb_rsp = std::bind(&Rpc::Client::Requestor::onResponse, requestor.get(), std::placeholders::_1, std::placeholders::_2); // 需要注册进Dispatcher中
    // registerHandler中需要传递一个MessageCallback类型的可调用对象 void(MType mtype, const typename CallbackT<T>::MessageCallback &handler)
    // 由于是成员函数 存在一个隐含的this指针 故需要传递requestor.get()来获取切实的this指针 同时需要预留两个参数 因此设置placeholders::_1 , _2

    // TODO 这里的转换是如何转换
    dispatcher->registerHandler<Rpc::BaseMessage>(Rpc::MType::RSP_RPC, cb_rsp);
    // 当Dispatcher接收到一个响应时 Dispatcher中的onResponse将会进行分发 根据响应类型去调用对应的回调
    // 此处调用的是requestor中的onResponse进行处理
    // 因此Requestor::onResponse将要提前注册给Dispatcher中
    DLOG("registerHandler 注册成功\n");
    /*
        在这里本质的原因是所注册的函数cb_rsp的类型需要与registerHandler相符 bind 对Requestor::onResponse函数进行绑定 主要是绑定this指针
        而Requestor::onResponse 函数的第二个参数本身的类型是一个BaseMassage类型
        而registerHandler注册时模板参数传RpcResponse时对应的registerHandler注册的可调用对象的参数即为RpcResponse
        与Requestor::onResponse 函数的第二个参数不符合条件 因此就会错误
    */

    auto con = client->connection(); // 获取一个连接 用于发送
    DLOG("连接获取成功\n");

    Json::Value parmas; // 设置参数
    parmas["num1"] = 23;
    parmas["num2"] = 46;

    Json::Value result; // 用于接收结果

    if (!caller->call(con, "Add", parmas, result)) // 同步请求
    {
        DLOG("请求调用失败\n");
        return -1;
    }

    DLOG("开始调用\n");
    return 0;
}

/* // void onMessage(const Rpc::BaseConnection::ptr &con, Rpc::BaseMessage::ptr &msg)
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
} */