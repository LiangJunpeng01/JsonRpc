#include <muduo/net/TcpServer.h>

#include <muduo/net/EventLoop.h>

#include <muduo/net/TcpConnection.h>

#include <muduo/net/Buffer.h>

#include <iostream>

#include <string>

#include <unordered_map>

#include <functional>


class DictServer{
public:
    DictServer(int port);
    void start();
private:
    /* 连接建立时的回调 */
    void onConnection(const muduo::net::TcpConnectionPtr &);
    /* 消息处理时的回调 */
    void onMessage(const muduo::net::TcpConnectionPtr &, muduo::net::Buffer *, muduo::Timestamp);

private:
    muduo::net::EventLoop _baseloop; // 不能调换位置 _baseloop 用于构造 _server
    muduo::net::TcpServer _server; // 服务端
};

DictServer::DictServer(int port):
    _server(&_baseloop/* 事件监控对象 */,\
            muduo::net::InetAddress("0.0.0.0",port)/* 服务器所监听的地址与端口 */,\
            "DictServer"/* 服务器名称 */,\
            muduo::net::TcpServer::kNoReusePort/* 启用地址重用 */){ 
        _server.setConnectionCallback(std::bind(&DictServer::onConnection,this,std::placeholders::_1));
        _server.setMessageCallback(std::bind(&DictServer::onMessage,this,\
                                             std::placeholders::_1,\
                                             std::placeholders::_2,\
                                             std::placeholders::_3));
    }    


    void DictServer::start(){
        /* 此处顺序不能换 必须先启动服务器才能进行loop的事件循环 */
        _server.start();
        _baseloop.loop();
    }


void DictServer::onConnection(const muduo::net::TcpConnectionPtr &cb){
    if(cb->connected()){
        std::cout<<"连接建立!!\n";
    }
    else{
        std::cout<<"连接断开!!\n";
    }
}

void DictServer::onMessage(const muduo::net::TcpConnectionPtr &cb, muduo::net::Buffer *buf, muduo::Timestamp){
    /* 不作粘包等处理 */
    
    std::unordered_map<std::string, std::string> dict;
    dict["hello"] = "你好";
    dict["world"] = "世界";
    dict["coffee"] = "咖啡";

    std::string msg = buf->retrieveAllAsString();
    std::string ret;
    auto it = dict.find(msg);
    if(it == dict.end()){
        ret = "未匹配到对应单词";
    }
    else{
        ret = it->second;
    }
    cb->send(ret);
    return;

}


int main(){
    DictServer dics(8111);
    dics.start();
    return 0;
}






