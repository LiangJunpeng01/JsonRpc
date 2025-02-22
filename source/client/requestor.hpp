#pragma once

#include "../common/message.hpp"

#include "../common/fields.hpp"

#include "../common/net.hpp"

#include <future>

#include <unordered_map>

namespace Rpc
{
    namespace Client
    {
        class Requestor
        { // 面向用户提供
          // 主要提供send发送接口( 需要提供两个重载 回调发送和异步发送 )
          // 其次是提供一个对服务端响应的一个onResponse 通过 onResponse将消息传送给Dispatcher进行请求分发从而进行下一步的处理
        public:
            using ptr = std::shared_ptr<Requestor>;

            using RequestCallback = std::function<void(const BaseMessage::ptr &)>;
            using AsyncResponse = std::future<BaseMessage::ptr>;
            struct RequestDescribe
            {
                using ptr = std::shared_ptr<RequestDescribe>;
                RType rtype;
                BaseMessage::ptr request;                // 请求消息
                std::promise<BaseMessage::ptr> response; // 通过get_future()可以获取一个与该promise相关联的future对象
                RequestCallback callback;
            }; // struct RequestDescribe

            void onResponse(const BaseConnection::ptr &, BaseMessage::ptr &); // 注册进Dispatcher中的对响应进行的业务处理 需要调用 Callback

            /* 为用户提供两个send接口 通过send接口向服务端发送数据*/
            bool send(const BaseConnection::ptr &, const BaseMessage::ptr &, const RequestCallback &); // 回调请求

            bool send(const BaseConnection::ptr &, const BaseMessage::ptr &, AsyncResponse &); // 异步请求

            bool send(const BaseConnection::ptr &, const BaseMessage::ptr &, BaseMessage::ptr &); // 同步请求

        private:
            RequestDescribe::ptr newDestribe(const BaseMessage::ptr &, RType, const RequestCallback &cb = nullptr); // 创建一个请求描述

            RequestDescribe::ptr getDescribe(const std::string &); // 通过rid找到对应的请求描述

            void removeDescribe(const std::string &); // 通过rid删除对应的请求描述
        private:
            std::mutex _mutex;                                                        // 保护_request_describe 临界资源
            std::unordered_map<std::string, RequestDescribe::ptr> _request_describes; // rid 与 请求描述的映射关系

        }; // class Requestor

        void Requestor::onResponse(const BaseConnection::ptr &con, BaseMessage::ptr &res)
        {
            /*
             通过rid找到对应的请求描述
             若未找到则表示该响应不存在对应的请求描述
             若找到则判断该响应对应的请求描述的rtype
             若rtype为RType::SYNC 则调用callback
             若rtype为RType::ASYNC 则调用promise的set_value
             处理完毕后删除对应的请求描述 以免内存泄漏
            */
            auto describe = getDescribe(res->rid());
            if (describe.get() == nullptr)
            {
                ELOG("not found request describe for response rid:{ %s }\n", res->rid().c_str());
                return; // 未找到对应的请求描述 无需处理
            }
            if (describe->rtype == RType::REQ_CALLBACK)
            {
                if (describe->callback) // 若callback存在 则调用callback
                {
                    describe->callback(res);    // 同步请求 调用callback
                    removeDescribe(res->rid()); // 删除对应的请求描述
                }
                else
                {
                    ELOG("callback is empty"); // 若callback为空 则不调用callback
                }
            }
            else if (describe->rtype == RType::REQ_ASYNC)
            {
                describe->response.set_value(res); // 异步请求 调用promise的set_value
                removeDescribe(res->rid());        // 删除对应的请求描述
            }
            else
            {
                ELOG("unknown request type:{ %d }\n", (int)describe->rtype);
            }
            removeDescribe(res->rid());
        }

        bool Requestor::send(const BaseConnection::ptr &con, const BaseMessage::ptr &req, const RequestCallback &callback)
        {

            auto rdp = newDestribe(req, RType::REQ_CALLBACK, callback); // 创建一个请求描述
                                                                        // 留了一手 做个判断
            if (!rdp.get())
            {
                ELOG("Requestor Destribe 对象构造失败\n");
                return false;
            }
            con->send(req); // 发送请求
            return true;
        }

        bool Requestor::send(const BaseConnection::ptr &con, const BaseMessage::ptr &req, AsyncResponse &async_resp)
        {
            auto rdp = newDestribe(req, RType::REQ_ASYNC); // 异步请求
            if (rdp.get() == nullptr)
            { // 一般情况下没有创建失败的可能
              // 因为一般构造失败时对应的 newDescribe
              // 函数中的 make_shared 将会提前抛异常终止程序向下运行
                ELOG("Requestor Destribe 对象构造失败\n");
                return false;
            }
            async_resp = rdp->response.get_future();
            return true;
        }

        bool Requestor::send(const BaseConnection::ptr &con, const BaseMessage::ptr &req, BaseMessage::ptr &rsp) // 同步请求
        {                                                                                                        // 这里是一个同步操作
            AsyncResponse rsp_future;                                                                            // 用于接收异步send中的future对象
            bool ret = send(con, req, rsp_future);
            if (!ret)
            {
                return false;
            }

            rsp = rsp_future.get(); // 可以直接get进行阻塞(同步) 需要将rsp进行返回
            return true;
        }

        Requestor::RequestDescribe::ptr Requestor::newDestribe(const BaseMessage::ptr &req, RType rtype, const RequestCallback &cb)
        {
            std::unique_lock<std::mutex> lock(_mutex); // RAII

            auto describe = std::make_shared<RequestDescribe>();
            describe->request = req;
            describe->rtype = rtype;
            describe->callback = cb;
            _request_describes[req->rid()] = describe;
            return describe;
        }

        Requestor::RequestDescribe::ptr Requestor::getDescribe(const std::string &rid)
        {
            std::unique_lock<std::mutex> lock(_mutex); // RAII
            auto it = _request_describes.find(rid);
            if (it == _request_describes.end())
            {
                return Requestor::RequestDescribe::ptr(); // 返回一个空的智能指针
            }
            return it->second; // 找到 返回对应的请求描述
        }

        void Requestor::removeDescribe(const std::string &rid)
        {
            std::unique_lock<std::mutex> lock(_mutex); // RAII
            _request_describes.erase(rid);             // 删除对应的请求描述
        }
    } // namespace client

} // namespace Rpc
