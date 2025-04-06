#include <iostream>

#include <sstream>

#include <cstdlib>

#include <jsoncpp/json/json.h>

#include <memory>

#include <string>

int main(){

    /*
     * 序列化操作
    */

    Json::Value root;
    root["name"] = "张三";
    root["class"] = "四班";
    root["age"] = 18;
    root["sex"] = "男";
    Json::Value hobit;
    hobit["movies"] = "泰坦尼克号";
    hobit["read"] = "三国演义"; 
    root["hobit"] = hobit;

    // 建造者对象
    Json::StreamWriterBuilder swb;
    swb.settings_["emitUTF8"] = true;

    // 通过建造者对象实例化出一个序列化对象
    std::unique_ptr<Json::StreamWriter> usw(swb.newStreamWriter());

    // 使用stringstream流接收
    std::stringstream ss;

    // 进行序列化
    usw->write(root, &ss);
    std::cout<<ss.str()<<std::endl;


    /*
     * 反序列化操作
     * */
    printf("#############################################\n");

    Json::Value nroot; // 新的 Json 对象
    Json::CharReaderBuilder crb; // 创建一个CharReader建造者
    std::string parse_error; // 接收反序列化失败的字符串
    std::string str = ss.str(); // 存储stringstream对象中的string
    std::unique_ptr<Json::CharReader> crp(crb.newCharReader()); // 实例化一个CharReader对象
    // bool f = crp->parse(ss.str().c_str(), ss.str().c_str()+ss.str().size(), &nroot, &parse_error);
    // 这里解析失败的原因是因为地址不同 stringstream每次调用str() 时将会给出不同的内存地址
    bool f = crp->parse(str.c_str(), str.c_str()+str.size(), &nroot, &parse_error);
    if(!f){
        std::cout<<"parse error: "<<parse_error<<std::endl;
        exit(-1);
    }
    
    Json::Value nhobit = nroot["hobit"];
    std::cout<<nroot["name"].asCString()<<std::endl;
    std::cout<<nroot["age"].asInt()<<std::endl;
    std::cout<<nroot["sex"].asString()<<std::endl;
    std::cout<<nroot["class"].asString()<<std::endl;
    std::cout<<nroot["hobit"]["movies"].asString()<<std::endl;
    std::cout<<nroot["hobit"]["read"].asString()<<std::endl;
     
    return 0;
}
