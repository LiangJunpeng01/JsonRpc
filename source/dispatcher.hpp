#pragma once

#include "net.hpp"

#include "message.hpp"

#include <unordered_map>

#include <memory>

#include <mutex>

#include <functional>

namespace Rpc
{
    class Callback
    {
    public:
        using ptr = std::shared_ptr<Callback>;
        virtual void onMessage(const BaseConnection::ptr &, BaseMessage::ptr &) = 0;
    };

    template <typename T>
    class CallbackT : public Callback
    {

    public:
        using ptr = std::shared_ptr<CallbackT<T>>;
        using MessageCallback = std::function<void(const BaseConnection::ptr &, std::shared_ptr<T> &)>;
        CallbackT(const MessageCallback &handler) : _handler(handler) {}
        virtual void onMessage(const BaseConnection::ptr &con, BaseMessage::ptr &msg) override
        {
            auto type_msg = std::dynamic_pointer_cast<T>(msg);
            _handler(con, type_msg);
        }

    private:
        MessageCallback _handler;
    };

    class Dispatcher
    {
    public:
        using ptr = std::shared_ptr<Dispatcher>;
        template <typename T>
        void registerHandler(MType, const typename CallbackT<T>::MessageCallback &);

        void onMessage(const BaseConnection::ptr &, BaseMessage::ptr &);

    private:
        std::unordered_map<MType, Callback::ptr> _handlers; // 存储父类指针 通过父类指针指向子类对象的多态性质实
                                                            // 现存储不同类型的 Message 回调
        std::mutex _mutex;
    };

    template <typename T>
    void Dispatcher::registerHandler(MType mtype, const typename CallbackT<T>::MessageCallback &handler)
    {
        std::unique_lock<std::mutex> lock(_mutex);         // RAII
        auto cb = std::make_shared<CallbackT<T>>(handler); // handler 为不同的消息处理回调(可调用对象)
        _handlers.insert({mtype, cb});

        // DLOG("注册成功!! 注册类型为:%d\n", (int)mtype);
    }

    void Dispatcher::onMessage(const BaseConnection::ptr &con, BaseMessage::ptr &msg)
    {
        std::unique_lock<std::mutex> lock(_mutex); // RAII
        auto it = _handlers.find(msg->mtype());
        if (it == _handlers.end())
        {
            ELOG("Dispatcher -- 未知消息类型: %d \n", (int)msg->mtype());
            DLOG("%s\n", msg->serialize().c_str());
            // 也考虑协议的一致性 安全性 判断客户端数据错误
            // 或是可能存在的恶意客户端
            con->shutdown();
            return;
        }
        it->second->onMessage(con, msg); // 父类指针指向子类对象(多态调用)
    }
} // namespace Rpc