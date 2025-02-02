#pragma once

#include <iostream>

#include <memory>

#include "abstract.hpp"

#include "detail.hpp"

#include "fields.hpp"

namespace Rpc{
class JsonMessag : public BaseMessage{
public:
    using ptr = ::std::shared_ptr<JsonMessag>;

    virtual std::string serialize() override;

    virtual bool unserialize(const std::string&) override;

protected:
    Json::Value _body; // 一条消息中body所需要的字段
};

std::string JsonMessag::serialize(){
    std::string body;
    bool ret = JSON::serialize(_body, body);
    if(!ret){
        ELOG("序列化失败\n");
    }
    return body;
}

bool JsonMessag::unserialize(const std::string &body){
    return JSON::unserialize(body, _body);
}


// JsonRequest 上的消息检查需要根据不同的请求进行消息检查
// 需要在派生类当中进行详细的请求消息检查
class JsonRequest : public JsonMessag{
public:
    using ptr = ::std::shared_ptr<JsonRequest>;
};

// JsonResponse 需要进行检查项的实现
// 派生类中的每个Response都需要进行rcode的检查
// 因此JsonResponse主要用于检查rcode
class JsonResponse : public JsonMessag{
public:
    using ptr = ::std::shared_ptr<JsonResponse>;

    // 主要针对响应状态码字段进行检查 是否正确 是否有效
    virtual bool check() override;
};

bool JsonResponse::check(){
    if(_body[KEY_RCODE].isNull()){
        // rcode 字段不存在
        ELOG("不存在 rcode 字段\n");
        return false;
    }
    if(!_body[KEY_RCODE].isIntegral()){
        // rcode 字段类型错误
        ELOG("rcode 类型错误\n");
        return false;
    }
    return true;
}


class RpcRequest : public JsonRequest{
public:
    using ptr = std::shared_ptr<RpcRequest>;
    
    virtual bool check() override; // 服务端检测 Rpc 请求中 method 与 params 字段是否存在且正确
    
    std::string method(); // 服务端获取方法字段

    void setMethod(const std::string &method); // 客户端设置方法字段

    Json::Value params(); // 服务端获取参数字段

    void setParams(const Json::Value &params); // 客户端设置参数字段

};

bool RpcRequest::check(){
    if(_body[KEY_METHOD].isNull() || !_body[KEY_METHOD].isString()){
        ELOG("method 字段不存在 || method 字段类型错误");
        return false;
    }
    if(_body[KEY_PARAMS].isNull() || !_body[KEY_PARAMS].isString()){
        ELOG("params 字段不存在 || params 字段类型错误");
        return false;
    }
    return true;
}

std::string RpcRequest::method(){
    return _body[KEY_METHOD].asString();
}


void RpcRequest::setMethod(const std::string &method){
    _body[KEY_METHOD] = method;
}

Json::Value RpcRequest::params(){
    return _body[KEY_PARAMS];
}

void RpcRequest::setParams(const Json::Value &params){
    _body[KEY_PARAMS] = params;
}



class TopicRequest : public JsonRequest{
public:
    using ptr = ::std::shared_ptr<TopicRequest>;

    virtual bool check() override; 

    std::string topicKey(); 

    void setTopicKey(const std::string &); 

    Json::Value params(); 

    void setParams(const Json::Value &); 
};



} // namespace Rpc
