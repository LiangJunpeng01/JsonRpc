#include <muduo/net/TcpServer.h>

#include <muduo/net/EventLoop.h>

#include <muduo/net/TcpConnection.h>

#include <muduo/net/Buffer.h>

#include <muduo/base/CountDownLatch.h>

#include <muduo/net/EventLoopThread.h>

#include <muduo/net/TcpClient.h>

#include "detail.hpp"

#include "fields.hpp"

#include "abstract.hpp"

#include <arpa/inet.h>

namespace Rpc
{
    class MuduoBuffer : public BaseBuffer
    {
    public:
        using ptr = std::shared_ptr<MuduoBuffer>;

        MuduoBuffer(muduo::net::Buffer *buf) : _buf(buf) {}

        /* 获取可读数据大小 */
        virtual size_t readableSize() override;

        /* 尝试读取4字节数据 (不清除缓冲区)*/
        virtual int32_t peekInt32() override;

        /* 删除缓冲区4字节数据 */
        virtual void retrieveInt32() override;

        /* 读取缓冲区4字节数据(取走后对四字节进行删除) */
        virtual int32_t readInt32() override;

        /* 取出特定长度大小的字符串 */
        virtual std::string retrieveAsString(size_t len) override;

    private:
        /* 功能实现 不对资源进行管理 */
        muduo::net::Buffer *_buf; // 不能使用智能指针 以免被释放
    };

    size_t MuduoBuffer::readableSize()
    {
        return _buf->readableBytes();
    }

    int32_t MuduoBuffer::peekInt32()
    {
        return _buf->peekInt32();
    }

    void MuduoBuffer::retrieveInt32()
    {
        _buf->retrieveInt32();
    }

    int32_t MuduoBuffer::readInt32()
    {
        return _buf->readInt32();
    }

    std::string MuduoBuffer::retrieveAsString(size_t len)
    {
        return _buf->retrieveAsString(len);
    }

    class BufferFactory
    {
        // 简单工厂类
    public:
        template <typename... Args>
        static BaseBuffer::ptr create(Args &&...args)
        {
            return std::make_shared<MuduoBuffer>(std::forward(args)...);
        }
    };

    class LVProtocol : public BaseProtocol
    {
    public:
        using ptr = std::shared_ptr<LVProtocol>;

        /* 判断缓冲区中数据是否能被处理 */
        virtual bool canProtocol(const BaseBuffer::ptr &) override; // 接收数据时的处理

        /* 对缓冲区中数据进行处理并使用一个BaseMessage的输出型参数进行接收 */
        virtual bool onMessage(const BaseBuffer::ptr &, BaseMessage::ptr &) override; // 接收数据时的处理

        /* 对消息进行序列化 返回一个序列化后的字符串 */
        virtual std::string serialize(const BaseMessage::ptr &) override; // 发送数据时的处理

    private:
        /*
            |--valueLength--|/// value ///|
                /                  ↓                   \
               |--Mtype--|--IDLength--|--MID--|--Body--|
        */
        int32_t valuelength_len = 4;

        int32_t mtype_len = 4;

        int32_t idlength_len = 4;
    };

    bool LVProtocol::canProtocol(const BaseBuffer::ptr &buf)
    {

        int32_t len = buf->peekInt32();
        if (buf->readableSize() < (len + valuelength_len)) // 当前可读数据大小小于总大小(不足完整数据)
        {
            return false;
        }
        return true;
    }

    bool LVProtocol::onMessage(const BaseBuffer::ptr &buf, BaseMessage::ptr &msg)
    {
        int32_t total_len = buf->readInt32();
        MType mtype = (MType)buf->readInt32();
        int32_t id_len = buf->readInt32();
        std::string mid = buf->retrieveAsString(id_len);
        int32_t body_len = total_len - id_len - idlength_len - mtype_len;
        std::string body = buf->retrieveAsString(body_len);

        msg = MessageFactory::create(mtype);
        if (msg.get() == nullptr)
        {
            ELOG("LVProtocol 构造消息对象时类型错误: %d\n", (int)mtype);
            return false;
        }

        if (!msg->unserialize(body))
        {
            ELOG("LVProtocol 反序列化失败\n");
            return false;
        }

        msg->setId(mid);

        msg->setMType(mtype);

        return true;
    }

    std::string LVProtocol::serialize(const BaseMessage::ptr &msg)
    {
        // 序列化
        std::string id = msg->rid();

        std::string body = msg->serialize();

        // 手动转成网络字节序 - muduo库会进行一次网络字节序转主机字节序 因此先手动转换为网络字节序 (跨平台一致性)
        MType mtype = (MType)htonl((int32_t)msg->mtype());

        int32_t id_length = htonl(id.size());

        int32_t total_length = htonl(body.size() + mtype_len + id_length + idlength_len);

        std::string result;
        result.reserve(total_length); // 预先开辟空间

        // 采用二进制的方式
        result.append((char *)&total_length, valuelength_len);
        result.append((char *)&mtype, mtype_len);
        result.append((char *)&id_length, idlength_len);
        result.append(id);
        result.append(body);

        return result;
    }

    class ProtocolFactory
    {
    public:
        template <typename... Args>
        static BaseProtocol::ptr create(Args &&...args)
        {
            return std::make_shared<LVProtocol>(std::forward(args)...);
        }
    };

    class MuduoConnection : public BaseConnection
    {
    public:
        using ptr = std::shared_ptr<MuduoConnection>;

        MuduoConnection(const muduo::net::TcpConnectionPtr &conn,
                        const BaseProtocol::ptr &protocol)
            : _conn(conn), _protocol(protocol) {}

        virtual bool connected() override; // 判断连接是否正常

        virtual void shutdown() override; // 关闭连接

        virtual void send(const BaseMessage::ptr &) override; // 发送消息

    private:
        muduo::net::TcpConnectionPtr _conn; // 连接对象

        BaseProtocol::ptr _protocol; // 协议对象 - 不需要对每个连接都创建一个协议对象
                                     // 主要用于提供序列化与反序列化的功能
    };

    bool MuduoConnection::connected()
    {
        return _conn->connected();
    }

    void MuduoConnection::shutdown()
    {
        _conn->shutdown();
    }

    void MuduoConnection::send(const BaseMessage::ptr &msg)
    {
        std::string body = _protocol->serialize(msg);
        _conn->send(body);
    }
    class ConnectionFactory
    {
    public:
        template <typename... Args>
        static BaseConnection::ptr create(Args &&...args)
        {
            return std::make_shared<MuduoConnection>(std::forward(args)...);
            // 传递参数包
        }
    };
} // namespace Rpc
