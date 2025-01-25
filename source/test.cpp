
#include <cstdio>

#include <ctime>

#include <iostream>

#include <string>

#include <jsoncpp/json/json.h>

#include <memory>

#include <sstream>

#define LOG1(msg) printf("%s\n",msg)

#define LOG2(fomat, msg) printf("[%s: %d] " fomat "\n",__FILE__,__LINE__,msg);

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


/////////////////////////////////////////

bool serialize(const Json::Value &val, std::string &body){
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

bool unserialize(const std::string &body, Json::Value &val){
    Json::CharReaderBuilder crb; // 实例化工厂对象
    std::unique_ptr<Json::CharReader> cr(crb.newCharReader());

    std::string errs;
    bool ret = cr->parse(body.c_str(), body.c_str()+body.size(), &val, &errs);
    if (!ret){
        std::cout<<"json unserialize error\n";
        return false;
    }
    return true;

}


int main(){

    const char *name = "小明";
    int age = 18; 
    const char *sex = "男";
    float score[3] = {88, 77.5, 66};

    Json::Value student;
    student["姓名"] = name;
    student["年龄"] = age;
    student["性别"] = sex;
    student["成绩"].append(score[0]);
    student["成绩"].append(score[1]);
    student["成绩"].append(score[2]);
    Json::Value fav;
    fav["书籍"] = "西游记";
    fav["运动"] = "打篮球";
    student["爱好"] = fav;
    std::string body;

    serialize(student, body);
    std::cout << body << std::endl;

    std::string str = R"({"姓名":"小黑", "年龄": 19, "成绩":[32, 45, 56]})";
    Json::Value stu;
    bool ret = unserialize(str, stu);
    if (ret == false) 
        return -1;
    std::cout << "姓名: " <<  stu["姓名"].asString() << std::endl;
    std::cout << "年龄: " <<  stu["年龄"].asInt() << std::endl;
    int sz = stu["成绩"].size();
    for (int i = 0; i < sz; i++) {
        std::cout << "成绩: " <<  stu["成绩"][i].asFloat() << std::endl;
    }    



// -----------------------------------

    //printf("%s\n","hello world");

    //LOG1("hello newworld");

    //LOG2("%d",9999999);

    ////LOG("%d %s",9999999,"hello world");

    ////LOG("hello world");
    //
    //DLOG("hello world Debug");
    //ILOG("hello world Info");
    //ELOG("hello world Error");

    


    return 0;
}
