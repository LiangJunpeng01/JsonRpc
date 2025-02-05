#pragma onceo

#include "net.hpp"

#include "message.hpp"

#include <unordered_map>

#include <memory>

#include <mutex>

namespace Rpc
{
    class Dispatcher
    {
    public:
        using ptr = std::shared_ptr<Dispatcher>;
        void registerHandler(MType, const MessageCallback &);

        void onMessage(const BaseConnection::ptr &, BaseMessage::ptr &);

    private:
        std::unordered_map<MType, MessageCallback> _handlers;
        std::mutex _mutex;
    };

    void Dispatcher::registerHandler(MType mtype, const MessageCallback &cb)
    {
        std::unique_lock<std::mutex> lock(_mutex); // RAII
        _handlers.insert({mtype, cb});
    }

    void Dispatcher::onMessage(const BaseConnection::ptr &con, BaseMessage::ptr &msg)
    {

        auto it = _handlers.find(msg->mtype());
        if (it == _handlers.end())
        {
            ELOG("Dispatcher -- 未知消息类型\n");
            // 也考虑协议的一致性 安全性 判断客户端数据错误
            // 或是可能存在的恶意客户端
            con->shutdown();
            return;
        }
        it->second(con, msg);
    }
} // namespace Rpc