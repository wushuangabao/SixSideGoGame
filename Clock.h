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
	void timer(int max_sec);  //倒计时
	string getTime();  //获取系统时间
	int hours;
	int min;
	int sec;
};

#endif