#pragma once

#include <iostream>

#include <memory>

#include "abstract.hpp"

#include "detail.hpp"

#include "fields.hpp"

namespace Rpc
{
    class JsonMessag : public BaseMessage
    {
    public:
        using ptr = std::shared_ptr<JsonMessag>;

        virtual std::string serialize() override;

        virtual bool unserialize(const std::string &) override;

    protected:
        Json::Value _body; // 一条消息中body所需要的字段
    };

    std::string JsonMessag::serialize()
    {
        std::string body;
        bool ret = JSON::serialize(_body, body);
        if (!ret)
        {
            ELOG("序列化失败\n");
        }
        return body;
    }

    bool JsonMessag::unserialize(const std::string &body)
    {
        return JSON::unserialize(body, _body);
    }

    // JsonRequest 上的消息检查需要根据不同的请求进行消息检查
    // 需要在派生类当中进行详细的请求消息检查
    class JsonRequest : public JsonMessag
    {
    public:
        using ptr = std::shared_ptr<JsonRequest>;
    };

    // JsonResponse 需要进行检查项的实现
    // 派生类中的每个Response都需要进行rcode的检查
    // 因此JsonResponse主要用于检查rcode
    class JsonResponse : public JsonMessag
    {
    public:
        using ptr = std::shared_ptr<JsonResponse>;

        // 主要针对响应状态码字段进行检查 是否正确 是否有效
        virtual bool check() override;
    };

    bool JsonResponse::check()
    {
        if (_body[KEY_RCODE].isNull())
        {
            // rcode 字段不存在
            ELOG("不存在 rcode 字段\n");
            return false;
        }
        if (!_body[KEY_RCODE].isIntegral())
        {
            // rcode 字段类型错误
            ELOG("rcode 类型错误\n");
            return false;
        }
        return true;
    }

    /* -------------RpcRequest------------- */

    class RpcRequest : public JsonRequest
    {
    public:
        using ptr = std::shared_ptr<RpcRequest>;

        virtual bool check() override; // 服务端检测 Rpc 请求中 method 与 params 字段是否存在且正确

        std::string method(); // 服务端获取方法字段

        void setMethod(const std::string &method); // 客户端设置方法字段

        Json::Value params(); // 服务端获取参数字段

        void setParams(const Json::Value &params); // 客户端设置参数字段
    };

    bool RpcRequest::check()
    {
        if (_body[KEY_METHOD].isNull() || !_body[KEY_METHOD].isString())
        {
            ELOG("method 字段不存在 || method 字段类型错误");
            return false;
        }
        if (_body[KEY_PARAMS].isNull() || !_body[KEY_PARAMS].isString())
        {
            ELOG("params 字段不存在 || params 字段类型错误");
            return false;
        }
        return true;
    }

    std::string RpcRequest::method()
    {
        return _body[KEY_METHOD].asString();
    }

    void RpcRequest::setMethod(const std::string &method)
    {
        _body[KEY_METHOD] = method;
    }

    Json::Value RpcRequest::params()
    {
        return _body[KEY_PARAMS];
    }

    void RpcRequest::setParams(const Json::Value &params)
    {
        _body[KEY_PARAMS] = params;
    }

    /* -------------TopicRequest------------- */

    class TopicRequest : public JsonRequest
    {
    public:
        using ptr = std::shared_ptr<TopicRequest>;

        virtual bool check() override;

        std::string topicKey();

        void setTopicKey(const std::string &);

        TopicOptype topicOptype();

        void setTopicOptype(TopicOptype);

        std::string topicMsg();

        void setTopicMsg(const std::string &);
    };
    bool TopicRequest::check()
    {
        if (_body[KEY_TOPIC_KEY].isNull() || (!_body[KEY_TOPIC_KEY].isString()))
        {
            ELOG("%s 字段不存在或类型错误\n", KEY_TOPIC_KEY);
            return false;
        }
        if (_body[KEY_OPTYPE].isNull() || (!_body[KEY_OPTYPE].isIntegral()))
        {
            ELOG("%s 字段不存在或类型错误\n", KEY_OPTYPE);
            return false;
        }
        if (_body[KEY_OPTYPE].asInt() == (int)TopicOptype::TOPIC_PUBLISH)
        {
            if (_body[KEY_TOPIC_MSG].isNull() || (!_body[KEY_TOPIC_MSG].isString()))
            {
                ELOG("%s 字段不存在或类型错误\n", KEY_TOPIC_MSG);
                return false;
            }
        }
        return true;
    }

    std::string TopicRequest::topicKey()
    {
        return _body[KEY_TOPIC_KEY].asString();
    }

    void TopicRequest::setTopicKey(const std::string &key)
    {
        _body[KEY_TOPIC_KEY] = key;
    }

    TopicOptype TopicRequest::topicOptype()
    {
        return (TopicOptype)_body[KEY_OPTYPE].asInt();
    }

    void TopicRequest::setTopicOptype(TopicOptype optype)
    {
        _body[KEY_OPTYPE] = (int)optype;
    }

    std::string TopicRequest::topicMsg()
    {
        return _body[KEY_TOPIC_MSG].asString();
    }

    void TopicRequest::setTopicMsg(const std::string &msg)
    {
        _body[KEY_TOPIC_MSG] = msg;
    }

    /* -------------ServiceRequest------------- */

    typedef std::pair<std::string, int> Address;

    class ServiceRequest : public JsonRequest
    {
    public:
        using ptr = std::shared_ptr<ServiceRequest>;

        virtual bool check() override;

        std::string method();

        void setMethod(const std::string &);

        ServiceOptype serviceOptype();

        void setServiceOptype(ServiceOptype);

        Address host();

        void setHost(const Address &);
    };

    bool ServiceRequest::check()
    {
        if (_body[KEY_METHOD].isNull() || (!_body[KEY_METHOD].isString()))
        {
            ELOG("ServiceRequest %s 字段不存在或类型错误\n", KEY_METHOD);
            return false;
        }
        if (_body[KEY_OPTYPE].isNull() || (!_body[KEY_OPTYPE].isIntegral()))
        {
            ELOG("ServiceRequest %s 字段不存在或类型错误\n", KEY_OPTYPE);
            return false;
        }
        if (_body[KEY_HOST].isNull() || (!_body[KEY_HOST].isObject()))
        {
            ELOG("ServiceRequest %s 字段不存在或类型错误\n", KEY_HOST);
            return false;
        }
        if (_body[KEY_HOST_IP].isNull() || (!_body[KEY_HOST_IP].isString()))
        {
            ELOG("ServiceRequest %s 字段不存在或类型错误\n", KEY_HOST_IP);
            return false;
        }
        if (_body[KEY_HOST_PORT].isNull() || (!_body[KEY_HOST_PORT].isIntegral()))
        {
            ELOG("ServiceRequest %s 字段不存在或类型错误\n", KEY_HOST_PORT);
            return false;
        }

        return true;
    }

    std::string ServiceRequest::method()
    {
        return _body[KEY_METHOD].asString();
    }

    void ServiceRequest::setMethod(const std::string &method)
    {
        _body[KEY_METHOD] = method;
    }

    ServiceOptype ServiceRequest::serviceOptype()
    {
        return (ServiceOptype)_body[KEY_OPTYPE].asInt();
    }

    void ServiceRequest::setServiceOptype(ServiceOptype optype)
    {
        _body[KEY_OPTYPE] = (int)optype;
    }

    Address ServiceRequest::host()
    {
        std::pair<std::string, int> host;
        host.first = _body[KEY_HOST_IP].asString();
        host.second = _body[KEY_HOST_PORT].asInt();
        return host;
    }

    void ServiceRequest::setHost(const Address &host)
    {
        Json::Value hostValue;
        hostValue[KEY_HOST_IP] = host.first;
        hostValue[KEY_HOST_PORT] = host.second;
    }

} // namespace Rpc
