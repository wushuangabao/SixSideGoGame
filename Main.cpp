#include "stdafx.h"

#include <winsock2.h>
#include <Windows.h>  //���������ļ�����˳��ߵ��ᱨ��
#include "Chat.h"


using namespace std;



int main()
{
	SetConsoleTitle(TEXT("��ԪΧ��"));
	
	Chat chat;
	chat.Menu();

	return 0;
}