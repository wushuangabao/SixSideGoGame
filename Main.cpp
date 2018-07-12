#include "stdafx.h"

#include <winsock2.h>
#include <Windows.h>  //以上两个文件引用顺序颠倒会报错
#include "Chat.h"


using namespace std;



int main()
{
	SetConsoleTitle(TEXT("六元围棋"));
	
	Chat chat;
	chat.Menu();

	return 0;
}