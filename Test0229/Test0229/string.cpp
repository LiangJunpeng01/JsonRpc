#define _CRT_SECURE_NO_WARNINGS 1

#include "string.h"

#include <cstring>
#include "string.h"
namespace Sweet {  // ��Ȼ�������ļ����ǿ���ʹ��ͬһ�������ռ�

// ���캯��
    Sweet::string::string(const char* str)
        // ��������Ϊ string(const char* str=""); �Ѷ���ȱʡֵ
        : _size(strlen(str)), _capacity(_size) {
        _str = new char[_capacity + 1];  // ����ռ�ʱӦ�ö�����1���ֽ��������'\0'
        cout << "Sweet::string::string(const char* str) ֱ�ӹ���" << endl;
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

    // ����
    Sweet::string::~string() {
        delete[] _str;
        _str = nullptr;
        _capacity = _size = 0;
    }

    //--------------------------

    // size()����
    const size_t Sweet::string::size() const { return _size; }
    //---------------------------

    // capacity()����
    const size_t Sweet::string::capacity() const { return _capacity; }
    //---------------------------

    // c_str()����
    const char* Sweet::string::c_str() const { return _str; }
    //---------------------------

    // ������ begin() end() cbegin() cend() ��iterator ��typedef��
    iterator Sweet::string::begin() { return _str; }

    iterator Sweet::string::end() { return _str + _size; }

    const_iterator Sweet::string::begin() const { return _str; }
    const_iterator Sweet::string::end() const { return _str + _size; }

    //---------------------------

    // reserve()����(���ݣ���ǰ����)
    void Sweet::string::reserve(size_t n) {
        if (n > _capacity) {
            char* tmp = new char[n + 1];
            strcpy(tmp, _str);
            delete[] _str;
            _str = tmp;
        }
        _capacity = n;  // ����_capacity�Ķ���Ӧ���ڸú����г��֣����򽫻�����ڴ����
    }
    //---------------------------

    // ��������

    Sweet::string::string(const string& str) : _str(nullptr) {
        cout << "Sweet::string::string(const string& str) ��������" << endl;
        string tmp(str._str);
        swap(tmp);
    }
    //---------------------------


    // �ƶ�����
    Sweet::string::string(string&& str) : _str(nullptr) {
        cout << "Sweet::string::string(string&& str) �ƶ�����" << endl;
        swap(str);
    }

    //---------------------------

    // push_back()����
    void Sweet::string::push_back(const char ch) {
        if (_size == _capacity) {
            reserve(_capacity == 0 ? 4 : _capacity * 2);
            // reserve( _capacity = _capacity == 0 ? 4 : _capacity * 2); - erro -
            //  ���ǲ���reserve�и���_capacity�������⣻������ֵ��ڴ����Ϊ�����������е�����
            // ��Ϊ���ݵ�����Ϊn>_capacity
            // ������Ҳ��ӵĸ�����_capacity��n>_capacity��������������
        }
        _str[_size++] = ch;
        _str[_size] = '\0';
    }
    //---------------------------

    // append()�����������ַ�����
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
        char ch)  // �����ﷵ��һ������*�ķ���ֵ��Ϊ�˿���֧��������ֵ
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

    //[]����
    char& Sweet::string::operator[](
        size_t i)  // ���������汾��ֻ���汾��ɶ���д�汾 - �ɶ���д
    {
        //[]Խ�罫�ᱻ������ֹ
        assert(i <= _size);
        return _str[i];
    }
    const char& Sweet::string::operator[](size_t i) const  // ֻ��
    {
        //[]Խ�罫�ᱻ������ֹ
        assert(i <= _size);
        return _str[i];
    }
    //---------------------------
    // operator��С�Ƚ�����< ,<=, > ,>= ,== ,!=
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

    // clear()��պ���
    void Sweet::string::clear() {
        _str[0] = '\0';
        _size = 0;
    }
    //---------------------------

    // insert���뺯��
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
        // ��������Ҫ��ʱ����_size����
        // ��Ϊ����������д�ӡ��ʱ�������������ʹ�õ��Ƿ�Χfor
        // ����Χfor��ʹ���б��������Զ��滻�ɵ�����������������end()������ʹ����_size
        // �Ĳ���, �ʸղŲŻ���ִ�ӡ��ȫ���ǵײ��c_str���Դ�ӡ��

        return *this;
    }

    //---------------------------

    string& Sweet::string::erase(size_t pos, size_t len) {
        assert(pos <= _size);
        // len==npos�����

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