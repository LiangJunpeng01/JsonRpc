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

// //ʲô����ֵ?Ϊʲô����ֵ����?
// /*
//     ��ֵ������趨ʱ��Ϊ���ڸ�ֵ���ŵ���� �ʳ�Ϊ��ֵ
//     ����ֵ������Ϊ��ֵ���ŵ��Ҳ�ʱ���Ϊ��ֵ

//     ����C++11�汾����(����)��revalue reference�ĸ����ֵ����
//     ��������ֵ���ù�����ֵ����ֵ֮��Ĺ�ϵ���ٺ͸�ֵ�����й�
//       -��ֵ: ��C++11�� ��ֵ�Ķ��岻�ٽ�����ֵ�����й� ��ֵʵ������ָ���б�ʶ���Ķ��� �ҿ���ȡ��ַ
//         ���ҿ��Խ������� ͨ����ֵ����ȷ�����ڴ�λ��

//       -��ֵ: ��C++11��,��ֵ�Ķ�����չΪ�����б�ʶ������ʱ���� ���߿��Ա��ƶ������ܱ���ֵ�Ķ���,��ֵ
//         ͨ������ʱ��������û������ �ʲ���ͨ��ȡ��ַ����ȡ���ַ

// */

// int main() {
//   // cout << "hello world" << endl;

//   int a1 = 1, b1 = 2;
//   a1 = b1;//��C++11֮ǰ���������a1Ϊ��ֵb1Ϊ��ֵ 
//   //����C++11��ĸ���� a1�� b1��Ϊ��ֵ

//   //--------

//   // ���µ�p b c *p��Ϊ��ֵ
//   int *p = new int(0);
//   int b = 1;
//   const int c = 2;

//   //���������ֵ��������
//   int*& rp = p;
//   int& rb = b;
//   const int& rc = c;
//   int& pvalue = *p;

//   return 0;
// }


// ########################################

// // ʲô����ֵ? Ϊʲô����ֵ����?
// /*
//   ��ֵ��������C++11���������������,������Ե�����Ҳʹ����ֵ����ֵ�ı��ʷ����ı�
//   ��ֵ����ֵһ��Ҳ��һ�ֱ�ʾ���ݵı��ʽ;
//   һ�����ֵ���������泣��,���ʽ����ֵ,��������ֵ�ȵ�;
//   ��ֵ���Գ����ڸ�ֵ���ŵ��ұߵ��ǲ��ܳ����ڸ�ֵ���ŵ��������ֵ����ͨ��ȡ��ַ������ȡ�����ַ

// */

// int main() {
//   double x = 1.1, y = 2.2;

//   //��������ֵ
//   999;
//   x + y;
//   fmin(x, y);//���� <math.h>ͷ�ļ���<cmath> �ú������ص���һ����ʱ���� ���г���

//   //����ֵ������ֵ����
//   int &&rr1 = 999;
//   double &&rr2 = x + y;
//   double &&rr3 = fmin(x ,y);

//     /*
//         ������ֱ���
//         ��ֵ���ܷ����ڸ�ֵ���ŵ����
//         10 = 1;
//         x + y = 2.2;
//         fmin(x, y) = 21;
//     */

//   return 0;
// }

// ########################################

// ��ֵ��������ֵ����
/*
    - ��ֵ����
    ��ֵ���üȿ���������ֵҲ����������ֵ
    ��ֵ������������ֵʱ���Խ���ֱ������
    ������ֵ����������Ҫ������ֵ�������ô��г����Ե���ֵʱ��Ҫʹ��const���� ����Ȩ�޵ķŴ�

    - ��ֵ����
    ��ֵ���ÿ�ֱ��������ֵ���䲻��ֱ��������ֵ��
    ����ֱ��������ֵ���ᷢ��error����(��ֵ�������г����Ե���ֵ)
    ��������Ҫʹ����ֵ����������ֵʱ����ʹ�� move() ʹ�ñ������ܹ������õĹ��̵��н���ֵʶ�����ֵ
    move() ���������ᵽ

*/

// int main() {
//     //��ֵ����
//     int a = 0;
//     int b = 23;
//     const int c = 30;
//     int &ar = a;
//     int &br = b; 
//     const int &cr = c; //const �������þ��г��Ե���ֵ
//     const double &dr = 2.22;//const����������ֵ

//     //��ֵ����
//     int &&crr = 30;
//     char &&charrr = 'c';

//     cout << b << endl;
//     // int &&brr = b; ֱ�����ý��ᱨ��
//     int &&brr = move(b);
//     cout << b << endl;

//     return 0;

//  } 
// void Func(int &a) { cout << "��ֵ���� : void Func(int &a)" << endl; }
// void Func(int &&a) { cout << "��ֵ���� : void Func(int &&a)" << endl; }

// int main() {
//   int a = 10;
//   int b = 20;
//
//
//  ���ں����Ľ��ղ������Ͳ�ͬ�ʹ�������
//   Func(a);  // ��ֵ���� : void Func(int &a)
//   Func(a + b);  //a+bΪ��ʱ���� ��ֵ���� : void Func(int &&a)
//   return 0;
// }

//   //--------

//int main()
//{
//
//    //����ֵ��һ����Է�Ϊ����
//    /*
//        1.����ֵ - �����������е����泣��
//        2.����ֵ - ����ֵһ��ָ�Զ��������е���ʱ����(��������)
//    */
//    string s1("hello");
//    string s2 = "world"; //"world"Ϊ��ֵ ��Ϊ����ֵ
//    string s3 = s1 + s2; //s3Ϊ��ֵ s1+s2Ϊ��ֵ����һ����ʱ���� ��Ϊ����ֵ
//
//    //�ô��������Զ���д��string������ʾ
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


//// to_string����
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