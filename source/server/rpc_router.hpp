#pragma once

#include "../common/net.hpp"

#include "../common/message.hpp"

#include "../common/fields.hpp"

namespace Rpc
{
    namespace Server
    {
        /* -------------------- enum class VType --------------------- */
        enum class VType
        {
            BOOL = 0,
            INTEGRAL,
            NUMERIC,
            STRING,
            ARRAY,
            OBJECT
        };

        /* -------------------- class ServiceDescribe --------------------- */

        class ServiceDescribe // 服务描述
        {
        public:
            using ptr = std::shared_ptr<ServiceDescribe>;
            using ServiceCallback = std::function<void(const Json::Value &, Json::Value &)>; // 实际业务处理函数
            using ParamsDescribe = std::pair<std::string /* 参数字段名称 */, VType /* 参数字段类型 */>;

            ServiceDescribe(std::string &&mname, std::vector<ParamsDescribe> &&descs, VType vtype, ServiceCallback &&handler)
                : _method_name(std::move(mname)), _callback(std::move(handler)), _params(std::move(descs)), _result(vtype) {}
            const std::string &method();
            bool paramsCheck(const Json::Value &); // 进行参数校验
            bool call(const Json::Value &params, Json::Value &result);

        private:
            bool check(const Json::Value &, VType);
            bool resultCheck(const Json::Value &); // 进行返回值类型校验

        private:
            std::string _method_name;            // 方法名称
            ServiceCallback _callback;           // 实际业务回调
            std::vector<ParamsDescribe> _params; // 参数描述
            VType _result;                       // 返回值类型
        }; // class ServiceDerscibe

        bool ServiceDescribe::check(const Json::Value &val, VType vtype)
        {
            switch (vtype)
            {
            case VType::BOOL:
                return val.isBool();
            case VType::INTEGRAL:
                return val.isIntegral();
            case VType::NUMERIC:
                return val.isNumeric();
            case VType::STRING:
                return val.isString();
            case VType::ARRAY:
                return val.isArray();
            case VType::OBJECT:
                return val.isObject();
            }
            return false;
        }

        bool ServiceDescribe::paramsCheck(const Json::Value &params)
        {
            for (auto &param_desc : _params)
            {
                if (!params.isMember(param_desc.first))
                {
                    ELOG("%s 参数字段缺失\n", param_desc.first.c_str());
                    return false;
                }
                if (!check(params[param_desc.first], param_desc.second))
                {
                    ELOG("%s 参数字段类型不匹配\n", param_desc.first.c_str());
                    return false;
                }
            }
            return true;
        }

        bool ServiceDescribe::resultCheck(const Json::Value &result)
        {
            return check(result, _result);
        }

        bool ServiceDescribe::call(const Json::Value &params, Json::Value &result)
        {
            _callback(params, result);
            if (!resultCheck(result))
            {
                ELOG("响应信息校验失败\n");
                return false;
            }
            return true;
        }

        const std::string &ServiceDescribe::method()
        {
            return _method_name;
        }

        /* -------------------- class ServiceDescribeFactory --------------------- */

        class ServiceDescribeFactory // 建造者模式
        {
        public:
            // set 设置参数.......
            void setCallBack(const ServiceDescribe::ServiceCallback &);
            void setParamsDescribe(const std::string &, VType);
            void setResultType(VType);
            void setMethodName(const std::string &);

            ServiceDescribe::ptr build()
            {
                return std::make_shared<ServiceDescribe>(std::move(_method_name), std::move(_params), _resulttype, std::move(_callback));
            }

        private:
            std::string _method_name; // 方法名称

            ServiceDescribe::ServiceCallback _callback;           // 实际业务回调
            std::vector<ServiceDescribe::ParamsDescribe> _params; // 参数描述
            VType _resulttype;                                    // 返回值类型
        };
        void ServiceDescribeFactory::setMethodName(const std::string &mname)
        {
            _method_name = mname;
        }

        void ServiceDescribeFactory::setCallBack(const ServiceDescribe::ServiceCallback &callback)
        {
            _callback = callback;
        }
        void ServiceDescribeFactory::setParamsDescribe(const std::string &pname, VType vtype)
        {
            _params.push_back(ServiceDescribe::ParamsDescribe(pname, vtype));
        }
        void ServiceDescribeFactory::setResultType(VType vtype)
        {
            _resulttype = vtype;
        }

        /* -------------------- class ServiceManager --------------------- */

        class ServiceManager
        {
        public:
            using ptr = std::shared_ptr<ServiceManager>;
            void insert(const ServiceDescribe::ptr &);        // 新增一个服务
            ServiceDescribe::ptr select(const std::string &); // 选择一个服务
            void remove(const std::string &);                 // 删除一个服务
        private:
            std::mutex _mutex; // 保护临界资源_services
            std::unordered_map<std::string, ServiceDescribe::ptr> _services;
        }; // class ServiceManager

        void ServiceManager::insert(const ServiceDescribe::ptr &desc)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _services.insert({desc->method(), desc});
        }
        ServiceDescribe::ptr ServiceManager::select(const std::string &mname)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto desc = _services.find(mname);
            if (desc == _services.end())
            {
                return ServiceDescribe::ptr();
            }
            return desc->second;
        }
        void ServiceManager::remove(const std::string &mname)
        {

            std::unique_lock<std::mutex> lock(_mutex);
            _services.erase(mname);
        }

        /* -------------------- calss RpcRouter --------------------- */

        class RpcRouter
        {
        public:
            using ptr = std::shared_ptr<RpcRouter>;
            RpcRouter() : manager_(std::make_shared<ServiceManager>()) {}
            void onRpcRequest(const BaseConnection::ptr &, RpcRequest::ptr &); // 注册进Dispatcher的回调
            void registerMethod(const ServiceDescribe::ptr &);                 // 注册服务
            // 用户通过调用建造模式来创建一个 ServiceDescribe::ptr 来设置参数描述

        private:
            void response(const BaseConnection::ptr &, const RpcRequest::ptr &, const Json::Value &, RCode);

        private:
            ServiceManager::ptr manager_; // 服务管理器
        }; // class RpcRouter

        void RpcRouter::onRpcRequest(const BaseConnection::ptr &con, RpcRequest::ptr &req)
        {
            /*
                1. 查询客户端所发的请求描述 判断是否能提供服务
                2. 进行参数校验 判断参数是否正确
                3. 调用业务处理函数
                4. 处理完毕后组织响应 返回结果
            */
            // 判断是否能够提供服务
            auto service = manager_->select(req->method());
            if (!service.get())
            {
                // 失败 不存在可提供的服务 需要构建响应并返回
                ELOG("未找到对应服务: %s\n", req->method().c_str());
                return response(con, req, Json::Value(), RCode::RCODE_NOT_FOUND_SERVICE);
            }
            // 进行参数校验
            if (!service->paramsCheck(req->params()))
            {
                ELOG("参数校验失败: %s\n", req->method().c_str());
                return response(con, req, Json::Value(), RCode::RCODE_INVALID_PARAMS);
            }

            // 调用业务处理函数
            Json::Value result;
            if (!service->call(req->params(), result))
            {
                ELOG("返回值类型校验失败: %s\n", req->method().c_str());
                return response(con, req, Json::Value(), RCode::RCODE_INTERNAL_ERROR);
            }
            response(con, req, result, RCode::RCODE_OK);
        }

        void RpcRouter::response(const BaseConnection::ptr &con, const RpcRequest::ptr &req, const Json::Value &ret, RCode rcode)
        {
            // 只允许成功
            RpcResponse::ptr msg = MessageFactory::create<RpcResponse>();
            msg->setId(req->rid());
            msg->setMType(MType::RSP_RPC);
            msg->setRCode(rcode);
            msg->setResult(ret);
            con->send(msg);
        }

        void RpcRouter::registerMethod(const ServiceDescribe::ptr &desc)
        {
            manager_->insert(desc);
        }
    } // namespace Server

} // namespace Rpc
