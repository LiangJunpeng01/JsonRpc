#include "message.hpp"

#include "net.hpp"

#include <cassert>

void RpcRequestTest()
{
    Rpc::RpcRequest::ptr rrp = Rpc::MessageFactory::create<Rpc::RpcRequest>();
    Json::Value parmas;
    rrp->setMethod("Add");
    parmas["num1"] = 22;
    parmas["num2"] = 29;

    rrp->setParams(parmas);

    std::string rrpstr = rrp->serialize();

    std::cout << rrpstr << std::endl;

    Rpc::BaseMessage::ptr tmp = Rpc::MessageFactory::create(Rpc::MType::REQ_RPC);
    tmp->unserialize(rrpstr);
    Rpc::RpcRequest::ptr rrp1 = std::dynamic_pointer_cast<Rpc::RpcRequest>(tmp);
    assert(rrp1->check());
    std::cout << rrp1->method() << std::endl;
    parmas.clear();
    parmas = rrp1->params();
    std::cout << parmas["num1"].asInt() << std::endl;
    std::cout << parmas["num2"].asInt() << std::endl;
}

void TopicRequestTest()
{
    Rpc::TopicRequest::ptr trp1 = Rpc::MessageFactory::create<Rpc::TopicRequest>();

    trp1->setTopicKey("sport");
    trp1->setTopicOptype(Rpc::TopicOptype::TOPIC_PUBLISH);
    trp1->setTopicMsg("YaoMing");

    std::string trpstr = trp1->serialize();
    std::cout << trpstr << std::endl;

    Rpc::BaseMessage::ptr tmp = Rpc::MessageFactory::create<Rpc::TopicRequest>();
    tmp->unserialize(trpstr);
    Rpc::TopicRequest::ptr trp2 = std::dynamic_pointer_cast<Rpc::TopicRequest>(tmp);

    assert(trp2->check());

    std::cout << trp2->topicKey() << std::endl;
    std::cout << trp2->topicMsg() << std::endl;
    std::cout << (int)trp2->topicOptype() << std::endl;
}

void ServiceRequestTest()
{
    Rpc::ServiceRequest::ptr srp1 = Rpc::MessageFactory::create<Rpc::ServiceRequest>();

    srp1->setServiceOptype(Rpc::ServiceOptype::SERVICE_ONLINE);

    // srp1->setServiceOptype(Rpc::ServiceOptype::SERVICE_DISCOVERY);

    srp1->setHost({"127.0.0.1", 9090});

    srp1->setMethod("Div");

    std::string srpstr = srp1->serialize();

    std::cout << srpstr << std::endl;

    Rpc::BaseMessage::ptr tmp = Rpc::MessageFactory::create(Rpc::MType::REQ_SERVICE);

    tmp->unserialize(srpstr);

    Rpc::ServiceRequest::ptr srp2 = std::dynamic_pointer_cast<Rpc::ServiceRequest>(tmp);

    std::cout << srp2->method() << std::endl;

    std::cout << (int)srp2->serviceOptype() << std::endl;

    assert(tmp->check());

    std::cout << srp2->host().first << " : " << srp2->host().second << std::endl;
}

void RpcResponseTest()
{
    Rpc::RpcResponse::ptr rrp1 = Rpc::MessageFactory::create<Rpc::RpcResponse>();

    rrp1->setRCode(Rpc::RCode::RCODE_OK);

    //    Json::Value val;

    // val["num"] = 121;

    rrp1->setResult(123);

    std::string rrpstr = rrp1->serialize();

    std::cout << rrpstr << std::endl;

    Rpc::BaseMessage::ptr tmp = Rpc::MessageFactory::create(Rpc::MType::RSP_RPC);

    tmp->unserialize(rrpstr);

    Rpc::RpcResponse::ptr rrp2 = std::dynamic_pointer_cast<Rpc::RpcResponse>(tmp);

    std::cout << (int)rrp2->rcode() << std::endl;

    std::cout << rrp2->result() << std::endl;

    assert(rrp2->check());
}

void TopicResponseTest()
{
    Rpc::TopicResponse::ptr trp1 = Rpc::MessageFactory::create<Rpc::TopicResponse>();

    trp1->setRCode(Rpc::RCode::RCODE_INTERNAL_ERROR);

    std::string trpstr = trp1->serialize();

    std::cout << trpstr << std::endl;

    Rpc::BaseMessage::ptr tmp = Rpc::MessageFactory::create(Rpc::MType::RSP_TOPIC);

    tmp->unserialize(trpstr);

    Rpc::TopicResponse::ptr trp2 = std::dynamic_pointer_cast<Rpc::TopicResponse>(tmp);

    std::cout << (int)trp2->rcode() << std::endl;

    assert(trp2->check());
}

void ServiceResponseTest()
{
    Rpc::ServiceResponse::ptr srp1 = Rpc::MessageFactory::create<Rpc::ServiceResponse>();

    srp1->setServiceOptype(Rpc::ServiceOptype::SERVICE_ONLINE);

    srp1->setRCode(Rpc::RCode::RCODE_OK);

    srp1->setMethod("MOV");

    std::vector<Rpc::Address> hosts;

    srp1->setHosts({{"127.0.0.1", 8011}, {"127.0.0.1", 8010}});

    std::string srpstr = srp1->serialize();

    std::cout << srpstr << std::endl;

    Rpc::BaseMessage::ptr bmp = Rpc::MessageFactory::create(Rpc::MType::RSP_SERVICE);

    bmp->unserialize(srpstr);

    Rpc::ServiceResponse::ptr srp2 = std::dynamic_pointer_cast<Rpc::ServiceResponse>(bmp);

    assert(srp2->check());

    std::cout << (int)srp2->serviceOptype() << std::endl;

    std::cout << (int)srp2->rcode() << std::endl;

    std::cout << srp2->method() << std::endl;

    std::vector<Rpc::Address> _hosts;

    _hosts = srp2->hosts();

    for (auto &it : _hosts)
    {
        std::cout << it.first << " : " << it.second << std::endl;
    }
}

int main()
{
    // RpcRequestTest();
    // TopicRequestTest();
    // ServiceRequestTest();
    // RpcResponseTest();
    // TopicResponseTest();
    ServiceResponseTest();

    return 0;
}

// ------------------------

//
// #include <cstdio>
//
// #include <ctime>
//
// #include <iostream>
//
// #include <string>
//
// #include <jsoncpp/json/json.h>
//
// #include <memory>
//
// #include <sstream>
//
////
//// #define LOG1(msg) printf("%s\n",msg)
////
//// #define LOG2(fomat, msg) printf("[%s: %d] " fomat "\n",__FILE__,__LINE__,msg);
////
//// #define LDBG 0
////
//// #define LINF 1
////
//// #define LERR 2
////
//// #define LDEFAULT LINF
////
//// #define LOG(level, fomat, ...) {\
////    if(level >= LDEFAULT){\
////        time_t t = time(NULL);\
////                   struct tm *lt = localtime(&t);\
////                   char timestr[32] = {0};\
////                   strftime(timestr, 31, "%m-%d %T",lt);\
////                   printf("[%s][%s: %d] " fomat "\n",timestr,__FILE__,__LINE__,##__VA_ARGS__ );\
////    }\
////}
////
//// #define DLOG(fomat, ...) LOG(LDBG, fomat, ##__VA_ARGS__)
//// #define ILOG(fomat, ...) LOG(LINF, fomat, ##__VA_ARGS__)
//// #define ELOG(fomat, ...) LOG(LERR, fomat, ##__VA_ARGS__)
////
//
///////////////////////////////////////////
//
// bool serialize(const Json::Value &val, std::string &body)
//{
//    std::stringstream ss;
//    Json::StreamWriterBuilder swb; // 实例化工厂对象
//    std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());
//    int ret = sw->write(val, &ss);
//    if (ret != 0)
//    {
//        std::cout << "json serialize error\n";
//        return false;
//    }
//    body = ss.str();
//    return true;
//}
//
// bool unserialize(const std::string &body, Json::Value &val)
//{
//    Json::CharReaderBuilder crb; // 实例化工厂对象
//    std::unique_ptr<Json::CharReader> cr(crb.newCharReader());
//
//    std::string errs;
//    bool ret = cr->parse(body.c_str(), body.c_str() + body.size(), &val, &errs);
//    if (!ret)
//    {
//        std::cout << "json unserialize error\n";
//        return false;
//    }
//    return true;
//}
//
///////////////////////////////////////////
//
// #include <chrono>
//
// #include <random>
//
// #include <atomic>
//
// #include <iomanip>
//
// std::string uuid()
//{
//    std::stringstream ss;
//    // 1. 构造机器随机数对象
//    std::random_device rd;
//    // 2. 机器随机数对象作为种子构造随机数对象
//    std::mt19937 generate(rd());
//    // 3. 构造限定数据范围对象
//    std::uniform_int_distribution<int> distribution(0, 255);
//    // 4. 生成八个随机数
//    for (int i = 0; i < 8; ++i)
//    {
//        if (i == 4 || i == 6)
//            ss << "-";
//        ss << std::setw(2) << std::setfill('0') << std::hex << distribution(generate);
//    }
//    ss << "-";
//
//    // 5. 定义八字节序号 按照特定格式组织成为16进制数字字符串
//    static std::atomic<size_t> seq(1);
//    size_t cur = seq.fetch_add(1);
//
//    for (int i = 7; i >= 0; --i)
//    {
//        if (i == 5)
//            ss << "-";
//        ss << std::setw(2) << std::setfill('0') << std::hex << ((cur >> (i * 8)) & 0xFF);
//    }
//
//    return ss.str();
//}
//
// #include "abstract.hpp"
//
// #include "detail.hpp"
//
// #include "message.hpp"
//
// int main()
//{
//
//    for (int i = 0; i < 10; ++i)
//    {
//        std::cout << uuid() << std::endl;
//    }
//
//    // -----------------------------------
//
//    //    const char *name = "小明";
//    //    int age = 18;
//    //    const char *sex = "男";
//    //    float score[3] = {88, 77.5, 66};
//    //
//    //    Json::Value student;
//    //    student["姓名"] = name;
//    //    student["年龄"] = age;
//    //    student["性别"] = sex;
//    //    student["成绩"].append(score[0]);
//    //    student["成绩"].append(score[1]);
//    //    student["成绩"].append(score[2]);
//    //    Json::Value fav;
//    //    fav["书籍"] = "西游记";
//    //    fav["运动"] = "打篮球";
//    //    student["爱好"] = fav;
//    //    std::string body;
//    //
//    //    serialize(student, body);
//    //    std::cout << body << std::endl;
//    //
//    //    std::string str = R"({"姓名":"小黑", "年龄": 19, "成绩":[32, 45, 56]})";
//    //    Json::Value stu;
//    //    bool ret = unserialize(str, stu);
//    //    if (ret == false)
//    //        return -1;
//    //    std::cout << "姓名: " <<  stu["姓名"].asString() << std::endl;
//    //    std::cout << "年龄: " <<  stu["年龄"].asInt() << std::endl;
//    //    int sz = stu["成绩"].size();
//    //    for (int i = 0; i < sz; i++) {
//    //        std::cout << "成绩: " <<  stu["成绩"][i].asFloat() << std::endl;
//    //    }
//    //
//
//    // -----------------------------------
//
//    // printf("%s\n","hello world");
//
//    // LOG1("hello newworld");
//
//    // LOG2("%d",9999999);
//
//    ////LOG("%d %s",9999999,"hello world");
//
//    ////LOG("hello world");
//    //
//    // DLOG("hello world Debug");
//    // ILOG("hello world Info");
//    // ELOG("hello world Error");
//
//    return 0;
//}
//