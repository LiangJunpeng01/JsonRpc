#pragma once

#include <string>

namespace Rpc
{
/* 请求字段宏定义 */

// RPC请求
#define KEY_METHOD "method"

#define KEY_PARAMS "parameters"

// Topic请求
#define KEY_TOPIC_KEY "topic_key"

#define KEY_TOPIC_MSG "topic_msg"

#define KEY_OPTYPE "optype"

// Service 请求
#define KEY_HOST "host"

#define KEY_HOST_IP "ip"

#define KEY_HOST_PORT "port"

// RPC 响应
#define KEY_RCODE "rcode"

#define KEY_RESULT "result"

    /* 消息类型 */
    // 一条消息请求中包含mtype字段与body字段 需要根据mtype字段来识别或设置对应的body格式

    enum class MType
    {
        REQ_RPC = 0,
        RSP_RPC,
        REQ_TOPIC,
        RSP_TOPIC,
        REQ_SERVICE,
        RSP_SERVICE
    };

    /* 响应状态码 */
    enum class RCode
    {
        RCODE_OK = 0,
        RCODE_PARSE_FAILED,
        RCODE_ERROR_MSGTYPE,
        RCODE_INVALID_MSG,
        RCODE_DISCONNECTED,
        RCODE_INVALID_PARAMS,
        RCODE_NOT_FOUND_SERVICE,
        RCODE_INVALID_OPTYPE,
        RCODE_NOT_FOUND_TOPIC,
        RCODE_INTERNAL_ERROR
    };

    std::string errStr(RCode code)
    {
        switch (code)
        {
        case RCode::RCODE_OK:
            return "处理成功";

        case RCode::RCODE_PARSE_FAILED:
            return "消息解析失败";

        case RCode::RCODE_ERROR_MSGTYPE:
            return "消息类型错误";

        case RCode::RCODE_INVALID_MSG:
            return "无效信息";

        case RCode::RCODE_DISCONNECTED:
            return "连接已断开";

        case RCode::RCODE_INVALID_PARAMS:
            return "无效RPC参数";

        case RCode::RCODE_NOT_FOUND_SERVICE:
            return "未找到服务";

        case RCode::RCODE_INVALID_OPTYPE:
            return "操作类型无效";

        case RCode::RCODE_NOT_FOUND_TOPIC:
            return "主题不存在";

        case RCode::RCODE_INTERNAL_ERROR:
            return "内部错误";
        default:
            return "未知错误";
        }
    }

    /* 请求类型 */
    enum class RType
    {
        REQ_SYNC = 0, // 同步请求
        REQ_ASYNC,    // 异步请求
        REQ_CALLBACK  // 回调请求
    };

    /* 主题操作类型 */
    enum class TopicOptype
    {
        TOPIC_CREATE = 0, // 主题创建
        TOPIC_REMOVE,     // 主题删除
        TOPIC_SUBSCRIBE,  // 主题订阅
        TOPIC_CANCEL,     // 主题取消订阅
        TOPIC_PUBLISH     // 主题发布
    };

    /* 服务操作类型 */
    enum class ServiceOptype
    {
        SERVICE_REGISTRY,  // 服务注册
        SERVICE_DISCOVERY, // 服务发现
        SERVICE_ONLINE,    // 服务上线
        SERVICE_OFFLINE    // 服务下线
    };

}
