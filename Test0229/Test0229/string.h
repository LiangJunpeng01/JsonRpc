#pragma once

#define _CRT_SECURE_NO_WARNINGS 1

#pragma once 

#include <assert.h>

#include <iostream>

using namespace std;

namespace Sweet {

    typedef char* iterator;              // �����const������
    typedef const char* const_iterator;  // ����const������
    class string {
    public:
        // ���캯��
        string(const char* str = "");

        // ��������
        string(const string& str);

        // �ƶ�����
        string(string&& str);

        // ����
        ~string();

        //[]����
        char& operator[](size_t i);  // ���������汾��ֻ���汾��ɶ���д�汾 -
                                     // �ɶ���д
        const char& operator[](size_t i) const;  // ֻ��

        // ������ begin() end() cbegin() cend() ��iterator ��typedef��
        //---{
        iterator begin();
        iterator end();
        const_iterator begin() const;
        const_iterator end() const;

        //}---

        void swap(string& str);

        // size()����
        const size_t size() const;

        // capacity()����
        const size_t capacity() const;

        // c_str()����
        const char* c_str() const;

        // operator��С�Ƚ�����< ,<=, > ,>= ,== ,!=
        bool operator<(const string& d) const;
        bool operator==(const string& d) const;
        bool operator<=(const string& d) const;
        bool operator>(const string& d) const;
        bool operator>=(const string& d) const;
        bool operator!=(const string& d) const;

        // reserve()����(���ݣ���ǰ����)
        void reserve(size_t n = 0);

        // push_back()����
        void push_back(const char ch);

        // append()�����������ַ�����
        void append(const char* str);

        // operator+=���������أ�����push_back()��append()������
        string& operator+=(
            char ch);  // �����ﷵ��һ������*�ķ���ֵ��Ϊ�˿���֧��������ֵ
        string& operator+=(const char* str);

        // operator+���������أ�����push_back()��append()������
        string operator+(
            char ch);  // �����ﷵ��һ������*�ķ���ֵ��Ϊ�˿���֧��������ֵ

        // insert()���뺯��
        string& insert(size_t pos, const char* str);

        // erase()��������
        string& erase(size_t pos, size_t len = npos);

        // clear()��պ���
        void clear();

    private:
        char* _str;
        size_t _size;
        size_t _capacity;

        const static int npos =
            -1;  // һ����˵����̬��Ա�������������ж��壬����int�������⡣
    };

}  // namespace Sweet

// operator����������ȡ<<>>����
/*operator*/
ostream& operator<<(ostream& out, const Sweet::string& st);
istream& operator>>(istream& in, Sweet::string& st);
