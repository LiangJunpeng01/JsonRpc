#pragma once

#include "../common/detail.hpp"

#include "requestor.hpp"

namespace Rpc
{
    namespace Client
    {
        class RpcCaller
        {

        public:
            using ptr = std::shared_ptr<RpcCaller>;

            using JsonAsyncResponse = std::future<Json::Value>;

            using JsonResponseCallback = std::function<void(const Json::Value &)>;

            RpcCaller(const Requestor::ptr &);

            bool call(const BaseConnection::ptr &, const std::string &, const Json::Value &, Json::Value &);                // 同步
            bool call(const BaseConnection::ptr &, const std::string &, const Json::Value &, JsonAsyncResponse &);          // 异步
            bool call(const BaseConnection::ptr &, const std::string &, const Json::Value &, const JsonResponseCallback &); // 回调

        private:
            Requestor::ptr _requestor;

            void Callback(std::shared_ptr<std::promise<Json::Value>>, const BaseMessage::ptr &);
            void CallerCallback(const JsonResponseCallback &, const BaseMessage::ptr &); // 用于回调请求的回调
        };

        RpcCaller::RpcCaller(const Requestor::ptr &requestor) : _requestor(requestor) {}

        bool RpcCaller::call(const BaseConnection::ptr &con, const std::string &method, const Json::Value &params, Json::Value &result)
        {
            /*
                1. 组织请求
                2. 发送请求
                3. 等待响应
            */

            DLOG("RpcCaller::call 同步 开始调用\n");
            // 组织请求
            auto req = MessageFactory::create<RpcRequest>();
            req->setId(UUID::uuid());
            req->setMType(MType::REQ_RPC);
            req->setMethod(method);
            req->setParams(params);
            BaseMessage::ptr rsp;
            DLOG("请求组织成功\n");

            DLOG("req: %s", req->serialize().c_str());
            // 发送请求
            if (!_requestor->send(con, std::dynamic_pointer_cast<BaseMessage>(req), rsp))
            {
                ELOG("Rpc同步请求失败\n");
                return false;
            }

            DLOG("请求发送\n");
            // 等待响应
            auto rsp_result = std::dynamic_pointer_cast<RpcResponse>(rsp);
            if (!rsp_result.get())
            {
                ELOG("RpcResponse 向下类型转换失败\n");
                return false;
            }

            if (rsp_result->rcode() != RCode::RCODE_OK)
            {
                ELOG("Rpc请求错误: %s\n", errStr(rsp_result->rcode()).c_str());
                return false;
            }

            result = rsp_result->result();
            DLOG("结果被获取\n");
            return true;
        }

        bool RpcCaller::call(const BaseConnection::ptr &con, const std::string &method, const Json::Value &params, JsonAsyncResponse &result)
        {
            auto req = MessageFactory::create<RpcRequest>();
            req->setId(UUID::uuid());
            req->setMType(MType::REQ_RPC);
            req->setMethod(method);
            req->setParams(params);

            auto json_promise = std::make_shared<std::promise<Json::Value>>();
            result = json_promise->get_future(); // 获取相关联的future对象 上层最终将得到这样的future对象从而再次对future进行处理从而得到最终结果

            Requestor::RequestCallback cb = std::bind(&RpcCaller::Callback, this, json_promise, std::placeholders::_1);
            // 将Callback函数进行绑定为一个新的函数对象 主要是用于与回调函数参数进行匹配
            // 回调函数类型为传入一个BaseMessage对象

            bool ret = _requestor->send(con, req, cb); // 进行消息发送
            if (!ret)                                  // 判断消息发送是否成功
            {
                ELOG("Rpc 异步请求失败\n");
                return false;
            }
            return true;
        }

        bool RpcCaller::call(const BaseConnection::ptr &con, const std::string &method, const Json::Value &params, const JsonResponseCallback &cb)
        {
            auto req = MessageFactory::create<RpcRequest>();
            req->setId(UUID::uuid());
            req->setMType(MType::REQ_RPC);
            req->setMethod(method);
            req->setParams(params);

            Requestor::RequestCallback req_cb = std::bind(&RpcCaller::CallerCallback, this, cb, std::placeholders::_1); // bind 成了一个 void(const Rpc::BaseMessage::ptr &)类型的函数

            if (!_requestor->send(con, req, req_cb))
            {
                ELOG("Rpc 回调请求失败\n");
                return false;
            }
            return true;
        }

        void RpcCaller::CallerCallback(const JsonResponseCallback &cb, const BaseMessage::ptr &msg)
        {
            auto rpc_rsp = std::dynamic_pointer_cast<RpcResponse>(msg);

            if (!rpc_rsp.get())
            {
                ELOG("Rpc 响应向下类型转换失败\n");
                return;
            }
            if (rpc_rsp->rcode() != RCode::RCODE_OK)
            {
                ELOG("Rpc 回调请求出错, %s\n", errStr(rpc_rsp->rcode()).c_str());
                return;
            }
            cb(rpc_rsp->result());
        }

        void RpcCaller::Callback(std::shared_ptr<std::promise<Json::Value>> result, const BaseMessage::ptr &msg)
        {
            auto rpc_rsp = std::dynamic_pointer_cast<RpcResponse>(msg);

            if (!rpc_rsp.get())
            {
                ELOG("Rpc 响应向下类型转换失败\n");
                return;
            }

            if (rpc_rsp->rcode() != RCode::RCODE_OK)
            {
                ELOG("Rpc 异步请求出错,%s\n", errStr(rpc_rsp->rcode()).c_str());
                return;
            }

            result->set_value(rpc_rsp->result()); // 主要通过异步 这里让上层以回调的方式处理result
                                                  // 当通过 set_value 设置参数时即表示调用
                                                  // 调用完毕后结果将进行存储在相关联的future对象中 用户可以直接调用future对象中的get()函数来直接获取结果

            /*
                这里本质上就是用户在调用call时将传递对应的参数 参数中用户需要传入一个future对象的引用以便于对应promise对象的相关联future对象的获取
                可以理解为用户使用一个future对象来获取最终的结果
            */
        }
    } // namespace client
} // namespace Rpc