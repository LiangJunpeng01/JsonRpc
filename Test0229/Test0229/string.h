#pragma once

#define _CRT_SECURE_NO_WARNINGS 1

#pragma once 

#include <assert.h>

#include <iostream>

using namespace std;

namespace Sweet {

    typedef char* iterator;              // 正向非const迭代器
    typedef const char* const_iterator;  // 正向const迭代器
    class string {
    public:
        // 构造函数
        string(const char* str = "");

        // 拷贝构造
        string(const string& str);

        // 移动构造
        string(string&& str);

        // 析构
        ~string();

        //[]重载
        char& operator[](size_t i);  // 存在两个版本，只读版本与可读可写版本 -
                                     // 可读可写
        const char& operator[](size_t i) const;  // 只读

        // 迭代器 begin() end() cbegin() cend() （iterator 用typedef）
        //---{
        iterator begin();
        iterator end();
        const_iterator begin() const;
        const_iterator end() const;

        //}---

        void swap(string& str);

        // size()函数
        const size_t size() const;

        // capacity()函数
        const size_t capacity() const;

        // c_str()函数
        const char* c_str() const;

        // operator大小比较重载< ,<=, > ,>= ,== ,!=
        bool operator<(const string& d) const;
        bool operator==(const string& d) const;
        bool operator<=(const string& d) const;
        bool operator>(const string& d) const;
        bool operator>=(const string& d) const;
        bool operator!=(const string& d) const;

        // reserve()函数(扩容，提前扩容)
        void reserve(size_t n = 0);

        // push_back()函数
        void push_back(const char ch);

        // append()函数（插入字符串）
        void append(const char* str);

        // operator+=操作符重载（复用push_back()和append()函数）
        string& operator+=(
            char ch);  // 在这里返回一个本类*的返回值是为了可以支持连续赋值
        string& operator+=(const char* str);

        // operator+操作符重载（复用push_back()和append()函数）
        string operator+(
            char ch);  // 在这里返回一个本类*的返回值是为了可以支持连续赋值

        // insert()插入函数
        string& insert(size_t pos, const char* str);

        // erase()擦除函数
        string& erase(size_t pos, size_t len = npos);

        // clear()清空函数
        void clear();

    private:
        char* _str;
        size_t _size;
        size_t _capacity;

        const static int npos =
            -1;  // 一般来说，静态成员变量不能在类中定义，但是int类型例外。
    };

}  // namespace Sweet

// operator流插入流提取<<>>重载
/*operator*/
ostream& operator<<(ostream& out, const Sweet::string& st);
istream& operator>>(istream& in, Sweet::string& st);
