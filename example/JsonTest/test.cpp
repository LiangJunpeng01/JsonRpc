#include <iostream>

#include <sstream>

#include <jsoncpp/json/json.h>


int main()
{
    Json::Value root; // 创建Json::Value对象
                      
    /* 为中间类对象赋值  */
    root["name"] = "张三";
    root["age"] = 18;
    root["result"].append(99.2);
    root["result"].append(88.1);
    root["result"].append(77.3);

    Json::Value hobby;
    hobby["read"] = "西游记";
    hobby["movie"] = "哈利波特";
    hobby["singer"] = "ladygaga";

    root["hobby"] = hobby;

    /* 创建 Json::StreamWriterBuilder 工厂类 */
    Json::StreamWriterBuilder swb;
    swb.settings_["emitUTF8"] = true; // 设置序列化时采用 UTF8 (可选)

    /* 利用 Json::StreamWriterBuilder 工厂类实例化出 Json::StreamWriter 对象 */
    std::unique_ptr<Json::StreamWriter>sw(swb.newStreamWriter()) ;

    std::stringstream ss;

    /* 序列化 */
    sw->write(root,&ss);

    std::cout<<ss.str()<<std::endl;

    std::cout<<"---------------------------------------\n";

    /* 创建Json::CharReaderBuilder 工厂类 */
    Json::CharReaderBuilder crb; 
    crb.settings_["emitUTF8"] = true;

    /*利用 Json::CharReaderBuilder 工厂类实例化出 Json::CharReader 对象 */
    std::unique_ptr<Json::CharReader> cr(crb.newCharReader());
    
    /* 创建中间类对象用于反序列化的接收 */
    Json::Value newroot;

    std::string parseerr; // 接收反序列化时的错误
    std::string str = ss.str();

    /* 反序列化 */
    bool ret = cr->parse(str.c_str(),str.c_str()+str.size(),&newroot,&parseerr);

    // if(ret).....  可对ret进行差错处理
    
    /* 将数据填充至中间类对象  */
    Json::Value newhobby = newroot["hobby"];
    std::cout<<"> name: "<<newroot["name"].asString()<<std::endl;
    std::cout<<"> age: "<<newroot["age"].asInt()<<std::endl;
    std::cout<<"> result: ";
    for(int i = 0;i<newroot["result"].size();++i) 
        std::cout<<newroot["result"][i].asFloat()<<"  ";
    std::cout<<"\n";

    std::cout<<"> hobby: \n";
    std::cout<<"\t> read: "<<newhobby["read"].asString()<<std::endl;
    std::cout<<"\t> movie: "<<newhobby["movie"].asString()<<std::endl;
    std::cout<<"\t> singer: "<<newhobby["singer"].asString()<<std::endl;

        
    return 0;
}

