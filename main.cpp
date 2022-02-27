#include<iostream>
#include<stdio.h>

using namespace std;
extern int order_book();
extern int modify();
int main() {
	cout << "task1:" << endl;
	order_book();

	cout << "task2:" << endl;
	modify();
}