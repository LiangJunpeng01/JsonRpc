/*
 * 主要针对于项目的类型字段信息的定义
 */

#pragma once

#include <string>

namespace Rpc {
/* 请求字段宏定义 */

// RPC请求
#define KEY_METHOD "method" /* 方法名称 */

#define KEY_PARAMS "parameters" /* 方法参数 */

// Topic请求
#define KEY_TOPIC_KEY "topic_key" /* 主题名称 */

#define KEY_TOPIC_MSG "topic_msg" /* 主题消息 */

#define KEY_OPTYPE "optype" /* 主题操作类型 */

// Service 请求
#define KEY_HOST "host" /* 主机 */

#define KEY_HOST_IP "ip" /* IP */

#define KEY_HOST_PORT "port" /* 端口号 */

// RPC 响应
#define KEY_RCODE "rcode" /* 响应状态码 */

#define KEY_RESULT "result" /* 响应结果 */

// #######################################################
// #######################################################
// #######################################################

/* 消息类型 */
// 一条消息请求中包含mtype字段与body字段
// 需要根据mtype字段来识别或设置对应的body格式
enum class MType {
  REQ_RPC = 0, // RPC 请求
  RSP_RPC,     // RPC 响应
  REQ_TOPIC,   // 主题请求
  RSP_TOPIC,   // 主题响应
  REQ_SERVICE, // 服务请求
  RSP_SERVICE  // 服务响应
};

/* 响应状态码 */
enum class RCode {
  RCODE_OK = 0,
  RCODE_PARSE_FAILED,      // 解析错误
  RCODE_ERROR_MSGTYPE,     // 无效的消息类型
  RCODE_INVALID_MSG,       // 无效的消息
  RCODE_DISCONNECTED,      // 未连接
  RCODE_INVALID_PARAMS,    // 无效参数
  RCODE_NOT_FOUND_SERVICE, // 未找到服务
  RCODE_INVALID_OPTYPE,    // 无效类型
  RCODE_NOT_FOUND_TOPIC,   // 未找到主题
  RCODE_INTERNAL_ERROR     // 服务器内部错误
};

std::string errStr(RCode code) {
  switch (code) {
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
enum class RType {
  //        REQ_SYNC = 0, // 同步请求
  /*
     无需同步请求
     同步请求可以在异步请求中通过promise 返回 future对象通过get进行一个同步操作
     因此提供同步请求操作是一个冗余操作
  */
  REQ_ASYNC = 0, // 异步请求
  REQ_CALLBACK   // 回调请求
};

/* 主题操作类型 */
enum class TopicOptype {
  TOPIC_CREATE = 0, // 主题创建
  TOPIC_REMOVE,     // 主题删除
  TOPIC_SUBSCRIBE,  // 主题订阅
  TOPIC_CANCEL,     // 主题取消订阅
  TOPIC_PUBLISH     // 主题发布
};

/* 服务操作类型 */
enum class ServiceOptype {
  SERVICE_REGISTRY,  // 服务注册
  SERVICE_DISCOVERY, // 服务发现
  SERVICE_ONLINE,    // 服务上线
  SERVICE_OFFLINE    // 服务下线
};

} // namespace Rpc
