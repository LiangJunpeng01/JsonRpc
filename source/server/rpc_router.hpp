#pragma once

#include "../common/net.hpp"

#include "../common/message.hpp"

namespace Rpc
{
    namespace Server
    {
        enum class VType
        {
            BOOL = 0,
            INTEGRAL,
            NUMERIC,
            STRING,
            ARRAY,
            OBJECT
        };

        class ServiceDescribe // 服务描述
        {
        public:
            using ptr = std::shared_ptr<ServiceDescribe>;
            using ServiceCallback = std::function<void(const Json::Value &, Json::Value &)>; // 实际业务处理函数
            using ParamsDescribe = std::pair<std::string /* 参数字段名称 */, VType /* 参数字段类型 */>;

            ServiceDescribe(const std::string &&mname, std::vector<ParamsDescribe> &&descs, VType vtype, ServiceCallback &&handler)
                : _method_name(std::move(mname)), _callback(std::move(handler)), _params(std::move(descs)), _result(vtype) {}

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

        class ServiceDescribeFactory // 建造者模式
        {
        public:
            // set 设置参数.......
            void setCallBack(const ServiceDescribe::ServiceCallback &);
            void setParamsDescribe(const std::string &, VType);
            void setResultType(VType);
            void setMethodName(std::string &);

            ServiceDescribe::ptr build()
            {
                return std::make_shared<ServiceDescribe>(_method_name, _params, _resulttype, _callback);
            }

        private:
            std::string _method_name; // 方法名称

            ServiceDescribe::ServiceCallback _callback;           // 实际业务回调
            std::vector<ServiceDescribe::ParamsDescribe> _params; // 参数描述
            VType _resulttype;                                    // 返回值类型
        };
        void ServiceDescribeFactory::setMethodName(std::string &mname)
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

        class ServiceManager
        {
        public:
            using ptr = std::shared_ptr<ServiceManager>;
            ServiceDescribe::ptr create(); // 新增一个服务
            ServiceDescribe::ptr select(); // 选择一个服务
            void remove();                 // 删除一个服务
        private:
            std::mutex _mutex; // 保护临界资源_services
            std::unordered_map<std::string, ServiceDescribe::ptr> _services;
        }; // class ServiceManager

        class RpcRouter
        {
        public:
            using ptr = std::shared_ptr<RpcRouter>;
            void onRpcRequest(const BaseConnection::ptr &, const RpcRequest::ptr &); // 注册进Dispatcher的回调
            void registerMethod(ServiceDescribe::ptr);                               // 注册服务
            // 用户通过调用建造模式来创建一个 ServiceDescribe::ptr 来设置参数描述

        private:
            ServiceManager::ptr manager_; // 服务管理器
        }; // class RpcRouter

        void RpcRouter::onRpcRequest(const BaseConnection::ptr &con, const RpcRequest::ptr &req)
        {
            /*
                1. 查询客户端所发的请求描述 判断是否能提供服务
                2. 进行参数校验 判断参数是否正确
                3. 调用业务处理函数
                4. 处理完毕后组织响应 返回结果
            */

            // TODO

            std::string method = req->method(); // 获取方法名称字段
            Json::Value params = req->params(); // 获取 Json::Value 类型的参数字段

            // TODO
            /*
            通过建造者模式 创造一个对应的方法描述类型 ServiceDescribe
            将方法描述类型通过 ServiceDescribe ->paramsCheck() 校验
            校验成功后调用业务处理函数 ServiceCallback
            */
        }

    } // namespace Server

} // namespace Rpc
