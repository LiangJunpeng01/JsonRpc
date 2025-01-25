#pragma once

/*
 * 项目中需要使用的一些小功能
 *  * 1. 日志宏
 *  * 2. Json 序列化反序列化
 *  * 3. uuid 生成
 *
 * */



/*  -- LOG --  */

#include <stdio.h>

#include <time.h>

namespace RPC{

#define LDBG 0 

#define LINF 1

#define LERR 2

#define LDEFAULT LINF

#define LOG(level, fomat, ...) {\
    if(level >= LDEFAULT){\
        time_t t = time(NULL);\
        struct tm *lt = localtime(&t);\
        char timestr[32] = {0};\
        strftime(timestr, 31, "%m-%d %T",lt);\
        printf("[%s][%s: %d] " fomat "\n",timestr,__FILE__,__LINE__,##__VA_ARGS__ );\
    }\
}

#define DLOG(fomat, ...) LOG(LDBG, fomat, ##__VA_ARGS__)
#define ILOG(fomat, ...) LOG(LINF, fomat, ##__VA_ARGS__)
#define ELOG(fomat, ...) LOG(LERR, fomat, ##__VA_ARGS__)


/*  -- JsonUtil --  */



#include <iostream>

#include <string>

#include <jsoncpp/json/json.h>

#include <memory>

#include <sstream>



class JsonUtil{
public:

    static bool serialize(const Json::Value &, std::string &);

    static bool unserialize(const std::string &, Json::Value &);

};


bool JsonUtil::serialize(const Json::Value &val, std::string &body){
    std::stringstream ss;
    Json::StreamWriterBuilder swb; // 实例化工厂对象
    std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter()); 
    int ret = sw->write(val,&ss);
    if(ret != 0){
        std::cout<<"json serialize error\n";
        return false;
    }
    body = ss.str();
    return true;
}

bool JsonUtil::unserialize(const std::string &body, Json::Value &val){
    Json::CharReaderBuilder crb; // 实例化工厂对象
    std::unique_ptr<Json::CharReader> cr(crb.newCharReader());

    std::string errs;
    bool ret = cr->parse(body.c_str(), body.c_str()+body.size(), &val, &errs);
    if (!ret){
        ELOG("json unserialize error");
        return false;
    }
    return true;

}
}
