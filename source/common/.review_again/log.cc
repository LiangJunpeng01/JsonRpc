#include <iostream>

#include <cstdio>

#include <ctime>

#define LDBG 0

#define LINF 1

#define LERR 2

#define LDEFAULT LDBG /* 设置默认的日志等级为DBG */

#define LOG(level, format, ...)                                                             \
  {                                                                                         \
    if (level >= LDEFAULT) {                                                                \
      time_t t = time(NULL); /* 获取当前时间, 并赋值给一个time_t类型对象 */ \
      struct tm *lt = std::localtime(                                                       \
          &t); /* 从time_t 类型中反序列化为一个 struct tm 对象 */              \
      char timestr[32] = {0};                                                               \
      strftime(timestr, 31, "%m-%d : %T", lt); /* 格式设置为 月- 日 : 时间 */      \
      printf("[%s][%s : %d]\t" format "\n", timestr, __FILE__, __LINE__,                    \
             ##__VA_ARGS__);                                                                \
    }                                                                                       \
  }

#define DLOG(format, ...) LOG(LDBG, format, ##__VA_ARGS__)
#define ILOG(format, ...) LOG(LINF, format, ##__VA_ARGS__)
#define ELOG(format, ...) LOG(LERR, format, ##__VA_ARGS__)

int main() {

  std::cout << "log" << std::endl;

  LOG(LDBG, "%s", "LOG1");

  return 0;
}
