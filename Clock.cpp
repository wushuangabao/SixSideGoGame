#include "stdafx.h"

#include "Clock.h"

Clock::Clock()
{
	time_t t = time(NULL);
	tm ti;
	localtime_s(&ti, &t);

	hours = ti.tm_hour;
	min = ti.tm_min;
	sec = ti.tm_sec;
}

void Clock::run()
{
	while (true)
	{
		Sleep(1000);
		if (++sec == 60)
		{
			sec = 0;
			min++;
			if (min == 60)
			{
				min = 0;
				hours++;
				if (hours == 24)
				{
					hours = 0;
				}
			}
		}
	}
}

void Clock::show()
{
	cout << hours << ": ";
	cout << min << ": ";
	cout << sec;
}

void Clock::timer(int sec)
{
	int i=sec;
	while (i)
	{
		cout << sec;
		Sleep(1000);
		i--;
	}
}

string Clock::getTime()
{
	time_t t = time(0);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%X", localtime(&t));
	return string(tmp);
}