#pragma once

#include "../common/message.hpp"

#include "../common/fields.hpp"

#include "../common/net.hpp"

#include <future>

#include <unordered_map>

namespace Rpc
{
    namespace client
    {
        class Requestor
        { // 面向用户提供
          // 主要提供send发送接口( 需要提供两个重载 回调发送和异步发送 )
          // 其次是提供一个对服务端响应的一个onResponse 通过 onResponse将消息传送给Dispatcher进行请求分发从而进行下一步的处理
        public:
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

            void onResponse(BaseConnection::ptr &, BaseMessage::ptr &); // 注册进Dispatcher中的对响应进行的业务处理 需要调用 Callback

            /* 为用户提供两个send接口 通过send接口向服务端发送数据*/
            bool send(const BaseConnection::ptr &, const BaseMessage::ptr &, const RequestCallback &); // 回调请求

            bool send(const BaseConnection::ptr &, const BaseMessage::ptr &, const AsyncResponse &); // 异步请求

        private:
            std::mutex _mutex;                                                       // 保护_request_describe 临界资源
            std::unordered_map<std::string, RequestDescribe::ptr> _request_describe; // rid 与 请求描述的映射关系
        }; // class Requestor

        void Requestor::onResponse(BaseConnection::ptr &, BaseMessage::ptr &)
        {
            // 通过rid找到对应的请求描述
        }

        bool Requestor::send(const BaseConnection::ptr &connection, const BaseMessage::ptr &request, const RequestCallback &callback)
        {
        }

        bool Requestor::send(const BaseConnection::ptr &connection, const BaseMessage::ptr &request, const AsyncResponse &async_resp)
        {
        }
    } // namespace client

} // namespace Rpc
