// uuid 通常由32位16进制数字字符组成
// 标准形式包含32个16进制数字字符, 以连字号分为五段, 形式为8-4-4-4-12的32个字符

#include <iostream>

#include <random>

#include <sstream>

#include <iomanip>

#include <string>

#include <atomic>

class _uuid {
public:
  static std::string uuid();

  std::string operator()() const;

  friend std::ostream &operator<<(std::ostream &os, const _uuid &obj);
};

inline std::string _uuid::uuid() {

  // 进行组织
  std::stringstream ss;

  std::stringstream ssstr;

  std::random_device rd; // 构造机器随机数对象

  std::mt19937 generate(rd()); // 这里是构造一个std::mt19937算法类型的随机数对象

  std::uniform_int_distribution<int> distribution(
      0, 255); // 构造一个用于限定随机数生成范围的对象

  for (int i = 0; i < 8; ++i) {
    if (i == 4 || i == 6) {
      ss << "-";

      // // DEBUG -- begin
      // ssstr << "-";
      // // DEBUG -- end
    }

    ss << std::setw(2) << std::setfill('0') << std::hex
       << distribution(generate);

    // // DEBUG -- begin
    // int tmp = distribution(generate);

    // ss << std::setw(2) << std::setfill('0') << std::hex << tmp;
    // ssstr << std::setw(2) << std::setfill('0') << tmp;
    // // DEBUG -- end
  }
  ss << "-";

  // DEBUG -- begin
  // std::cout << ss.str() << std::endl;
  // std::cout << ssstr.str() << std::endl;
  // DEBUG -- end
  static std::atomic<size_t> seq(1);
  size_t cur = seq.fetch_add(1); // 功能类似于后置++, 获取当前值后自增
  for (int i = 7; i >= 0; --i) {
    if (i == 5)
      ss << "-";
    ss << std::setw(2) << std::setfill('0') << std::hex
       << ((cur >> (i * 8)) & 0xFF);
  }

  return ss.str();
}

std::string _uuid::operator()() const { return uuid(); }

std::ostream &operator<<(std::ostream &os, const _uuid &obj) {
  os << obj(); // 调用 operator() 获取字符串并输出
  return os;
}

int main() {

  // std::cout << "uuid" << std::endl;
  for (int i = 0; i < 17; ++i)
    std::cout << _uuid() << std::endl;

  return 0;
}
