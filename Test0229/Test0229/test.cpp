//#include<iostream>
//
//using namespace std;
//
//int main()
//{
//	cout << "hello world" << endl;
//	int a = 10;
//	int&& arr = move(a);
//	cout << arr << endl;
//	cout << a << endl;
//	return 0;
//}


#include <iostream>

#include <cmath>

#include <string>

#include "string.h"

#include <list>

using namespace std;

// //什么是左值?为什么是左值引用?
// /*
//     左值在最初设定时即为处于赋值符号的左侧 故称为左值
//     而右值被放置为赋值符号的右侧故被称为右值

//     而再C++11版本更新(引入)了revalue reference的概念即右值引用
//     在引入右值引用过后左值与右值之间的关系不再和赋值符号有关
//       -左值: 在C++11后 左值的定义不再仅仅赋值符号有关 左值实际上是指具有标识符的对象 且可以取地址
//         并且可以进行引用 通常左值具有确定的内存位置

//       -右值: 在C++11中,右值的定义拓展为不具有标识符的临时对象 或者可以被移动但不能被赋值的对象,右值
//         通常是临时创建的且没有命名 故不能通过取地址来获取其地址

// */

// int main() {
//   // cout << "hello world" << endl;

//   int a1 = 1, b1 = 2;
//   a1 = b1;//在C++11之前该种情况下a1为左值b1为右值 
//   //但在C++11后的概念当中 a1与 b1都为左值

//   //--------

//   // 以下的p b c *p皆为左值
//   int *p = new int(0);
//   int b = 1;
//   const int c = 2;

//   //对上面的左值进行引用
//   int*& rp = p;
//   int& rb = b;
//   const int& rc = c;
//   int& pvalue = *p;

//   return 0;
// }


// ########################################

// // 什么是右值? 为什么是右值引用?
// /*
//   右值引用是在C++11后所引入的新特性,这个特性的引入也使得左值与右值的本质发生改变
//   右值与左值一样也是一种表示数据的表达式;
//   一般的右值引用有字面常量,表达式返回值,函数返回值等等;
//   右值可以出现在赋值符号的右边但是不能出现在赋值符号的左边且右值不能通过取地址符号来取到其地址

// */

// int main() {
//   double x = 1.1, y = 2.2;

//   //常见的右值
//   999;
//   x + y;
//   fmin(x, y);//包含 <math.h>头文件或<cmath> 该函数返回的是一个临时变量 具有常性

//   //对右值进行右值引用
//   int &&rr1 = 999;
//   double &&rr2 = x + y;
//   double &&rr3 = fmin(x ,y);

//     /*
//         将会出现报错
//         右值不能放置于赋值符号的左侧
//         10 = 1;
//         x + y = 2.2;
//         fmin(x, y) = 21;
//     */

//   return 0;
// }

// ########################################

// 左值引用与右值引用
/*
    - 左值引用
    左值引用既可以引用左值也可以引用右值
    左值引用在引用左值时可以进行直接引用
    但是左值引用若是需要引用右值或是引用带有常属性的左值时需要使用const引用 避免权限的放大

    - 右值引用
    右值引用可直接引用右值但其不能直接引用左值；
    若是直接引用左值将会发生error报错(左值包括具有常属性的左值)
    但若是需要使用右值引用引用左值时可以使用 move() 使得编译器能够在引用的过程当中将左值识别成右值
    move() 将在下面提到

*/

// int main() {
//     //左值引用
//     int a = 0;
//     int b = 23;
//     const int c = 30;
//     int &ar = a;
//     int &br = b; 
//     const int &cr = c; //const 引用引用具有常性的左值
//     const double &dr = 2.22;//const引用引用右值

//     //右值引用
//     int &&crr = 30;
//     char &&charrr = 'c';

//     cout << b << endl;
//     // int &&brr = b; 直接引用将会报错
//     int &&brr = move(b);
//     cout << b << endl;

//     return 0;

//  } 
// void Func(int &a) { cout << "左值引用 : void Func(int &a)" << endl; }
// void Func(int &&a) { cout << "右值引用 : void Func(int &&a)" << endl; }

// int main() {
//   int a = 10;
//   int b = 20;
//
//
//  由于函数的接收参数类型不同故构成重载
//   Func(a);  // 左值引用 : void Func(int &a)
//   Func(a + b);  //a+b为临时对象 右值引用 : void Func(int &&a)
//   return 0;
// }

//   //--------

//int main()
//{
//
//    //在右值中一般可以分为两种
//    /*
//        1.字面值 - 即内置类型中的字面常量
//        2.将亡值 - 将亡值一般指自定义类型中的临时对象(即将销毁)
//    */
//    string s1("hello");
//    string s2 = "world"; //"world"为右值 且为字面值
//    string s3 = s1 + s2; //s3为左值 s1+s2为右值且是一个临时对象 且为将亡值
//
//    //该处将引用自定义写的string进行演示
//    Sweet::string st1("hello world");
//    Sweet::string st2("hello world");
//    Sweet::string ret1 = move(st1);
//    Sweet::string st3 = (st2 + 'c');
//
//    return 0;
//}


//   //--------

//
//int&& func1() {
//    int a = 10;
//    return move(a);
//}
//
//int main() {
//    int a = func1();
//    cout << a << endl;
//    return 0;
//}


//   //--------


//// to_string函数
//namespace Sweet
//{
//    Sweet::string to_string(int value) {
//        bool flag = true;
//        if (value < 0) {
//            flag = false;
//            value = 0 - value;
//        }
//
//        Sweet::string str;
//
//        while (value) {
//            int x = value % 10;
//            value /= 10;
//            str += ('0' + x);
//        }
//
//        if (!flag) {
//            str += '-';
//        }
//
//        std::reverse(str.begin(), str.end());
//        return str;
//    }
//} // namespace Sweet
//
//Sweet::string to_string(int value) {
//    bool flag = true;
//    if (value < 0) {
//        flag = false;
//        value = 0 - value;
//    }
//
//    Sweet::string str;
//
//    while (value) {
//        int x = value % 10;
//        value /= 10;
//        str += ('0' + x);
//    }
//
//    if (!flag) {
//        str += '-';
//    }
//
//    std::reverse(str.begin(), str.end());
//    return str;
//}
//int main() {
//    Sweet::string s1 = Sweet::to_string(1234);
//    return 0;
//}

//------------------------------

int main() {
    list<Sweet::string> ls1;
    Sweet::string s1("hello world");
    ls1.push_back(s1);
    ls1.push_back(move(s1));
    return 0;
}