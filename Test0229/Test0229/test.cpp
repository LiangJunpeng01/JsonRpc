#include<iostream>

using namespace std;

int main()
{
	cout << "hello world" << endl;
	int a = 10;
	int&& arr = move(a);
	cout << arr << endl;
	cout << a << endl;
	return 0;
}