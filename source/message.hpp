#pragma once

#include <iostream>

#include <memory>

#include "abstract.hpp"

#include "detail.hpp"

#include "fields.hpp"

namespace Rpc{
class JsonMessag : public BaseMessage{
public:
    using ptr = std::shared_ptr<JsonMessag>;

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



} // namespace Rpc
