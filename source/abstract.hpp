#pragma once

#include <iostream>

#include "fields.hpp"

#include <memory>

#include <functional>

namespace Rpc
{

    class BaseMessage
    {
    public:
        using ptr = std::shared_ptr<BaseMessage>;

        virtual ~BaseMessage() {}

        /* 设置消息ID */
        virtual void setId(const std::string &id)
        {
            _rid = id;
        }

        /* 获得消息ID */
        virtual std::string rid() { return _rid; }

        /* 设置消息类型 */
        virtual void setMType(MType mtype)
        {
            _mtype = mtype;
        }

        /* 获得消息类型 */
        virtual MType mtype() { return _mtype; }

        /* 序列化 */
        virtual std::string serialize() = 0;

        /* 反序列化 */
        virtual bool unserialize(const std::string &) = 0;

        /* 反序列化后检查消息字段是否完整 */
        virtual bool check() = 0;

    private:
        MType _mtype;     // 消息类型
        std::string _rid; // 消息id
    };

    class BaseBuffer
    {
    public:
        /* 获取可读数据大小 */
        virtual size_t readableSize() = 0;

        /* 尝试读取4字节数据 (不清除缓冲区)*/
        virtual int32_t peekInt32() = 0;

        /* 删除缓冲区4字节数据 */
        virtual void retrieveInt32() = 0;

        /* 读取缓冲区4字节数据(取走后对四字节进行删除) */
        virtual int32_t readInt32() = 0;

        /* 取出特定长度大小的字符串 */
        virtual std::string retrieveAsString(size_t len) = 0;

        using ptr = std::shared_ptr<BaseBuffer>;
    };

    class BaseProtocol
    {

    public:
        using ptr = std::shared_ptr<BaseProtocol>;

        /* 判断缓冲区中数据是否能被处理 */
        virtual bool canProtocol(const BaseBuffer::ptr &buf) = 0; // 接收数据时的处理

        /* 对缓冲区中数据进行处理并使用一个BaseMessage的输出型参数进行接收 */
        virtual bool onMessage(const BaseBuffer::ptr &buf, BaseMessage::ptr &msg) = 0; // 接收数据时的处理

        /* 对消息进行序列化 返回一个序列化后的字符串 */
        virtual std::string serialize(const BaseMessage::ptr &msg) = 0; // 发送数据时的处理
    };

    class BaseConnection
    {
    public:
        using ptr = std::shared_ptr<BaseConnection>;

        /* 判断连接是否正常 */
        virtual bool connected() = 0;

        /* 关闭连接 */
        virtual void shutdown() = 0;

        /* 发送消息 */
        virtual void send(const BaseMessage::ptr &msg) = 0;
    };

    /* 连接接收回调处理 */
    using ConnectionCallback = std::function<void(const BaseConnection::ptr &)>;

    /* 连接关闭回调处理 */
    using CloseCallback = std::function<void(const BaseConnection::ptr &)>;

    /* 消息接收回调处理 */
    using MessageCallback = std::function<void(const BaseConnection::ptr &, BaseBuffer::ptr &)>;

    class BaseServer
    {
    public:
        using ptr = std::shared_ptr<BaseServer>;

        /* 设置连接处理回调 */
        virtual void setConnectionCallback(const ConnectionCallback &cb)
        {
            _cb_connection = cb;
        }

        /* 设置关闭连接处理回调 */
        virtual void setCloseCallback(const CloseCallback &cb)
        {
            _cb_close = cb;
        }

        /* 设置消息处理回调 */
        virtual void setMessageCallback(const MessageCallback &cb)
        {
            _cb_message = cb;
        }

        /* 服务器启动 */
        virtual void start() = 0;

    private:
        ConnectionCallback _cb_connection; // 连接处理回调

        CloseCallback _cb_close; // 连接关闭回调

        MessageCallback _cb_message; // 消息处理回调
    };

    class BaseClient
    {
    public:
        using ptr = std::shared_ptr<BaseClient>;

        /* 设置连接处理回调 */
        virtual void setConnectionCallback(const ConnectionCallback &cb)
        {
            _cb_connection = cb;
        }

        /* 设置关闭连接处理回调 */
        virtual void setCloseCallback(const CloseCallback &cb)
        {
            _cb_close = cb;
        }

        /* 设置消息处理回调 */
        virtual void setMessageCallback(const MessageCallback &cb)
        {
            _cb_message = cb;
        }

        /* 发起连接 */
        virtual void connect() = 0;

        /* 连接关闭 */
        virtual void shutdown() = 0;

        /* 消息发送 */
        virtual void send(const BaseMessage::ptr &) = 0;

        /* 获取连接 */
        virtual BaseConnection::ptr connection() = 0;

        /* 连接建立是否正常 */
        virtual bool connected() = 0;

    private:
        ConnectionCallback _cb_connection; // 连接处理回调

        CloseCallback _cb_close; // 连接关闭回调

        MessageCallback _cb_message; // 消息处理回调
    };

} // namespace Rpc
