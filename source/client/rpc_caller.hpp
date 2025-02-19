#pragma once

#include "../common/detail.hpp"

#include "requestor.hpp"

namespace Rpc
{
    namespace client
    {
        class RpcCaller
        {
        private:
            Requestor::ptr _requestor;

        public:
            using ptr = std::shared_ptr<RpcCaller>;

            using JsonAsyncResponse = std::future<Json::Value>;

            using JsonResponseCallback = std::function<void(const Json::Value &)>;

            RpcCaller(const Requestor::ptr &);

            bool call(const BaseConnection::ptr &, const std::string &, const Json::Value &, Json::Value &);          // 同步
            bool call(const BaseConnection::ptr &, const std::string &, const Json::Value &, JsonAsyncResponse &);    // 异步
            bool call(const BaseConnection::ptr &, const std::string &, const Json::Value &, JsonResponseCallback &); // 回调
        };

        RpcCaller::RpcCaller(const Requestor::ptr &requestor) : _requestor(requestor) {}

        bool RpcCaller::call(const BaseConnection::ptr &con, const std::string &method, const Json::Value &params, Json::Value &result)
        {
            /*
                1. 组织请求
                2. 发送请求
                3. 等待响应
            */

            // 组织请求
            auto req = MessageFactory::create<RpcRequest>();
            req->setId(UUID::uuid());
            req->setMType(MType::REQ_RPC);
            req->setMethod(method);
            req->setParams(params);
            BaseMessage::ptr rsp;

            // 发送请求
            if (!_requestor->send(con, std::dynamic_pointer_cast<BaseMessage>(req), rsp))
            {
                ELOG("Rpc同步请求失败\n");
                return false;
            }

            // 等待响应
            auto rsp_result = std::dynamic_pointer_cast<RpcResponse>(rsp);
            if (!rsp_result.get())
            {
                ELOG("RpcResponse 向下类型转换失败\n");
                return false;
            }

            if (rsp_result->rcode() != RCode::RCODE_OK)
            {
                ELOG("Rpc请求错误 :%s\n", errStr(rsp_result->rcode()).c_str());
                return false;
            }
            result = rsp_result->result();
            return true;
        }
        bool RpcCaller::call(const BaseConnection::ptr &con, const std::string &method, const Json::Value &params, JsonAsyncResponse &result)
        {
        }
        bool RpcCaller::call(const BaseConnection::ptr &con, const std::string &method, const Json::Value &params, JsonResponseCallback &result)
        {
        }
    } // namespace client
} // namespace Rpc