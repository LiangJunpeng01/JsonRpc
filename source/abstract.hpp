#pragma once

#include "fields.hpp"

namespace RPC{

class BaseMessage{
public:
    virtual ~BaseMessage(){}

    /* 设置消息ID */
    virtual void setId(const std::string &id){
        _rid = id;
    }

    /* 获得消息ID */
    virtual std::string rid() {return _rid;}

    /* 设置消息类型 */
    virtual void setMType(MType mtype){ 
        _mtype = mtype;
    }

    /* 获得消息类型 */
    virtual MType mtype(){ return _mtype; }

    /* 序列化 */ 
    virtual std::string serialize() = 0;

    /* 反序列化 */
    virtual bool unserialize() = 0;

    /* 反序列化后检查消息字段是否完整 */
    virtual bool check() = 0;
private:
    MType _mtype; // 消息类型
    std::string _rid; // 消息id
};

class BaseBuffer{
private:
    /* 获取可读数据大小 */
    virtual size_t readableSize() = 0;

    /* 尝试读取4字节数据 (不清除缓冲区)*/
    virtual int32_t peekInt32() = 0;

    /* 删除缓冲区4字节数据 */
    virtual void retrieveInt32() = 0;

    /* 读取缓冲区4字节数据(取走后对四字节进行删除) */
    virtual int32_t readInt32() = 0;
public:

};


} // namespace RPC

