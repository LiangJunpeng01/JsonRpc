#ifndef DETAIL_HPP

#define DETAIL_HPP
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

namespace Rpc{

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


/*  -- JSON --  */

#include <iostream>

#include <string>

#include <jsoncpp/json/json.h>

#include <memory>

#include <sstream>

class JSON{
public:

    static bool serialize(const Json::Value &, std::string &);

    static bool unserialize(const std::string &, Json::Value &);

};


bool JSON::serialize(const Json::Value &val, std::string &body){
    std::stringstream ss;
    Json::StreamWriterBuilder swb; // 实例化工厂对象
    std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter()); 
    int ret = sw->write(val,&ss);
    if(ret != 0){
        ELOG("json serialize error\n");
        return false;
    }
    body = ss.str();
    return true;
}

bool JSON::unserialize(const std::string &body, Json::Value &val){
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

/*  -- UUID --  */

#include <chrono>

#include <random>

#include <atomic>

#include <iomanip>

class UUID{
public:
    static std::string uuid(); 
};

std::string uuid(){
    std::stringstream ss;
    // 1. 构造机器随机数对象
    std::random_device rd;
    // 2. 机器随机数对象作为种子构造随机数对象
    std::mt19937 generate(rd());
    // 3. 构造限定数据范围对象
    std::uniform_int_distribution<int> distribution(0,255);
    // 4. 生成八个随机数 
    for(int i = 0; i<8; ++i){
        if(i == 4 || i == 6) ss << "-";
        ss << std::setw(2) << std::setfill('0') << std::hex << distribution(generate);
    }
    ss << "-";

    // 5. 定义八字节序号 按照特定格式组织成为16进制数字字符串
    static std::atomic<size_t> seq(1);
    size_t cur = seq.fetch_add(1);

    for(int i = 7; i>=0; --i){
        if(i == 5) ss << "-";
        ss << std::setw(2) << std::setfill('0') << std::hex << ((cur >> (i*8)) & 0xFF);
    }

    return ss.str();
}

}

#endif
