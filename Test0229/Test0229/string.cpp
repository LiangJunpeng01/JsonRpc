#define _CRT_SECURE_NO_WARNINGS 1

#include "string.h"

#include <cstring>
#include "string.h"
namespace Sweet {  // 虽然在两个文件但是可以使用同一个命名空间

// 构造函数
    Sweet::string::string(const char* str)
        // 函数声明为 string(const char* str=""); 已定有缺省值
        : _size(strlen(str)), _capacity(_size) {
        _str = new char[_capacity + 1];  // 申请空间时应该多申请1个字节用来存放'\0'
        cout << "Sweet::string::string(const char* str) 直接构造" << endl;
        strcpy(_str, str);
    }

    void Sweet::string::swap(string& str) {
        ::swap(str._str, _str);
        ::swap(str._size, _size);
        ::swap(str._capacity, _capacity);
    }

    //---------------------------

    Sweet::string Sweet::string::operator+(char ch) {
        string tmp(*this);
        tmp += ch;
        return tmp;
    }
    //---------------------------

    // 析构
    Sweet::string::~string() {
        delete[] _str;
        _str = nullptr;
        _capacity = _size = 0;
    }

    //--------------------------

    // size()函数
    const size_t Sweet::string::size() const { return _size; }
    //---------------------------

    // capacity()函数
    const size_t Sweet::string::capacity() const { return _capacity; }
    //---------------------------

    // c_str()函数
    const char* Sweet::string::c_str() const { return _str; }
    //---------------------------

    // 迭代器 begin() end() cbegin() cend() （iterator 用typedef）
    iterator Sweet::string::begin() { return _str; }

    iterator Sweet::string::end() { return _str + _size; }

    const_iterator Sweet::string::begin() const { return _str; }
    const_iterator Sweet::string::end() const { return _str + _size; }

    //---------------------------

    // reserve()函数(扩容，提前扩容)
    void Sweet::string::reserve(size_t n) {
        if (n > _capacity) {
            char* tmp = new char[n + 1];
            strcpy(tmp, _str);
            delete[] _str;
            _str = tmp;
        }
        _capacity = n;  // 更新_capacity的动作应该在该函数中出现，否则将会出现内存错误
    }
    //---------------------------

    // 拷贝构造

    Sweet::string::string(const string& str) : _str(nullptr) {
        cout << "Sweet::string::string(const string& str) 拷贝构造" << endl;
        string tmp(str._str);
        swap(tmp);
    }
    //---------------------------


    // 移动构造
    Sweet::string::string(string&& str) : _str(nullptr) {
        cout << "Sweet::string::string(string&& str) 移动构造" << endl;
        swap(str);
    }

    //---------------------------

    // push_back()函数
    void Sweet::string::push_back(const char ch) {
        if (_size == _capacity) {
            reserve(_capacity == 0 ? 4 : _capacity * 2);
            // reserve( _capacity = _capacity == 0 ? 4 : _capacity * 2); - erro -
            //  若是不在reserve中更新_capacity而是在这；这里出现的内存错误为将会跳过所有的扩容
            // 因为扩容的条件为n>_capacity
            // 而这里也间接的更新了_capacity，n>_capacity条件都将不满足
        }
        _str[_size++] = ch;
        _str[_size] = '\0';
    }
    //---------------------------

    // append()函数（插入字符串）
    void Sweet::string::append(const char* str) {
        const size_t len = strlen(str);
        if (_size + len > _capacity) {
            reserve(_capacity + len);
            // reserve(_capacity += len);
        }
        strcpy(_str + _size, str);
        _size += len;
    }
    //---------------------------

    string& Sweet::string::operator+=(
        char ch)  // 在这里返回一个本类*的返回值是为了可以支持连续赋值
    {
        push_back(ch);
        return *this;
    }
    //---------------------------

    string& Sweet::string::operator+=(const char* str) {
        append(str);
        return *this;
    }
    //---------------------------

    //[]重载
    char& Sweet::string::operator[](
        size_t i)  // 存在两个版本，只读版本与可读可写版本 - 可读可写
    {
        //[]越界将会被断言阻止
        assert(i <= _size);
        return _str[i];
    }
    const char& Sweet::string::operator[](size_t i) const  // 只读
    {
        //[]越界将会被断言阻止
        assert(i <= _size);
        return _str[i];
    }
    //---------------------------
    // operator大小比较重载< ,<=, > ,>= ,== ,!=
    bool Sweet::string::operator<(const string& d) const {
        return strcmp(this->_str, d._str) < 0;
    }
    bool Sweet::string::operator==(const string& d) const {
        return strcmp(this->_str, d._str) == 0;
    }
    bool Sweet::string::operator<=(const string& d) const {
        return *this < d || *this == d;
    }
    bool Sweet::string::operator>(const string& d) const { return !(*this <= d); }
    bool Sweet::string::operator>=(const string& d) const { return !(*this < d); }
    bool Sweet::string::operator!=(const string& d) const { return !(*this == d); }

    //---------------------------

    // clear()清空函数
    void Sweet::string::clear() {
        _str[0] = '\0';
        _size = 0;
    }
    //---------------------------

    // insert插入函数
    string& Sweet::string::insert(size_t pos, const char* str) {
        Sweet::string tmp(str);
        assert(pos <= _size);
        int len = strlen(str);
        if (_size + len > _capacity) {
            reserve(_capacity + len);
        }
        for (size_t i = _size; i >= pos; --i) {
            _str[i + len] = _str[i];
        }

        for (auto ch : tmp) {
            _str[pos++] = ch;
        }

        _size += len;
        // 在这里需要及时更新_size参数
        // 因为在流插入进行打印的时候，流插入的重载使用的是范围for
        // 而范围for在使用中编译器会自动替换成迭代器，而迭代器的end()重载中使用了_size
        // 的参数, 故刚才才会出现打印不全但是底层和c_str可以打印。

        return *this;
    }

    //---------------------------

    string& Sweet::string::erase(size_t pos, size_t len) {
        assert(pos <= _size);
        // len==npos的情况

        if (len >= (_size - pos)) {
            _str[pos] = '\0';
            _size = pos;
        }
        else {
            for (size_t i = pos; i <= _size - len; i++) {
                _str[i] = _str[i + len];
            }
            _size -= len;
        }

        return *this;
    }

}  // namespace Sweet

ostream& operator<<(ostream& out, const Sweet::string& st) {
    for (auto ch : st) {
        cout << ch;
    }
    return out;
}

istream& operator>>(istream& in, Sweet::string& st) {
    st.clear();
    char ch = in.get();
    while (ch != ' ' && ch != '\n') {
        st += ch;
        ch = in.get();
    }
    return in;
}