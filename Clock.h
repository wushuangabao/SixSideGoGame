#ifndef MYCLOCK_H
#define MYCLOCK_H

#include<iostream>
#include<time.h>
#include <windows.h>

using namespace std;

class Clock
{
public:
	Clock();
	void run();
	void show();
	void timer(int max_sec);  //����ʱ
	string getTime();  //��ȡϵͳʱ��
	int hours;
	int min;
	int sec;
};

#endif