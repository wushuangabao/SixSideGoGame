#include "stdafx.h"
#include "SixSideGo.h"

using namespace std;

/////////////////////////////////////
//字体相关
/////////////////////////////////////
struct CONSOLE_FONT
{
	DWORD index;
	COORD dim;  //坐标
};
typedef BOOL(WINAPI *PROCSETCONSOLEFONT)(HANDLE, DWORD);
typedef BOOL(WINAPI *PROCGETCONSOLEFONTINFO)(HANDLE, BOOL, DWORD, CONSOLE_FONT*);
typedef COORD(WINAPI *PROCGETCONSOLEFONTSIZE)(HANDLE, DWORD);
typedef DWORD(WINAPI *PROCGETNUMBEROFCONSOLEFONTS)();
typedef BOOL(WINAPI *PROCGETCURRENTCONSOLEFONT)(HANDLE, BOOL, CONSOLE_FONT*);
PROCSETCONSOLEFONT SetConsoleFont;
PROCGETCONSOLEFONTINFO GetConsoleFontInfo;
PROCGETCONSOLEFONTSIZE GConsoleFontSize;
PROCGETNUMBEROFCONSOLEFONTS GetNumberOfConsoleFonts;
PROCGETCURRENTCONSOLEFONT GCurrentConsoleFont;

///////////////////////////////////////
//设置字体大小
//这样似乎能避免出现“改变控制台窗口高度导致棋盘下半部分消失”bug
///////////////////////////////////////
void SSGo::SetFont(pair<int,int> pos)
{
	int nNumFont;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	HMODULE hKernel32 = GetModuleHandle(L"kernel32");
	SetConsoleFont = (PROCSETCONSOLEFONT)GetProcAddress(hKernel32, "SetConsoleFont");
	GetConsoleFontInfo = (PROCGETCONSOLEFONTINFO)GetProcAddress(hKernel32, "GetConsoleFontInfo");
	//GConsoleFontSize = (PROCGETCONSOLEFONTSIZE)GetProcAddress(hKernel32, "GetConsoleFontSize");
	GetNumberOfConsoleFonts = (PROCGETNUMBEROFCONSOLEFONTS)GetProcAddress(hKernel32, "GetNumberOfConsoleFonts");
	//GCurrentConsoleFont = (PROCGETCURRENTCONSOLEFONT)GetProcAddress(hKernel32, "GetCurrentConsoleFont");
	nNumFont = GetNumberOfConsoleFonts();  //字体总数
	CONSOLE_FONT *pFonts = new CONSOLE_FONT[nNumFont];
	GetConsoleFontInfo(hConsole, 0, nNumFont, pFonts);//获取并存储字体信息

	int temp;
    if (Side > 8)
		temp = 11;  //小字号
	else if (Side > 4)
		temp = 14;  //中字号
	else
		temp = 17;  //大字号
	char ch;
	pair<int, int> underResPos(resPos.first, resPos.second + 1);
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);  //定义并实例化输入句柄hStd
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwRes;  //相当于unsigned long
	INPUT_RECORD keyRec;

	SetConsoleFont(hConsole, pFonts[temp].index);
	if (pos.first == 0)  //传入的参数pos.first==0表示默认
	{
		return;
	}
	SetOutPos(hOut, resPos);
	cout << "Enter A or D to set the Font Size, C to confirm.";
	while (true)  //按A、D左右移动设置字号大小，C确认
	{
		ReadConsoleInput(hIn, &keyRec, 1, &dwRes);
		if (keyRec.EventType == KEY_EVENT)
		{ 
			if (keyRec.Event.KeyEvent.bKeyDown)
			{
				ch = keyRec.Event.KeyEvent.uChar.AsciiChar;
				if (ch == 'A' || ch == 'a')
				{
					if (temp <= 0)
					{
						SetOutPos(hOut, underResPos);
						cout << "Temp number cannot be smaller!";
						continue;
					}
					SetOutPos(hOut, underResPos);
					cout << "                              ";
					temp--;
					SetConsoleFont(hConsole, pFonts[temp].index);
				}
				else if (ch == 'D' || ch == 'd')
				{
					if (temp >= nNumFont - 1)
					{
						SetOutPos(hOut, underResPos);
						cout << "Temp number cannot be bigger!";
						continue;
					}
					SetOutPos(hOut, underResPos);
					cout << "                              ";
					temp++;
					SetConsoleFont(hConsole, pFonts[temp].index);
				}
				else if (ch == 'C' || ch == 'c')
				{
					SetOutPos(hOut, resPos);
					cout << "                                                 ";
					break;
				}
			}
		}
	}
	SetOutPos(hOut, pos);
}

//////////////////////////////////////
//设置输出文字、背景颜色
//////////////////////////////////////
BOOL SSGo::SetConsoleColor(WORD wAttributes)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsole == INVALID_HANDLE_VALUE)
		return FALSE;
	return SetConsoleTextAttribute(hConsole, wAttributes);
}

//////////////////////////////////////
//构造函数 用某个字符填充棋盘外部区域
//////////////////////////////////////
SSGo::SSGo(char c,int side, Chat* chat):posJieB (pair<int, int>(0, 0)),posJieW (pair<int, int>(0, 0))
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	pair<int, int> pos(0, 0);
	SetOutPos(hOut,pos);
	enemyI = 0;
	enemyJ = 0;
	myI = 0;
	myJ = 0;
	turn = true;
	myChat = chat;

	if (side > 13 || side < 3)
		Side = 13;
	else
	    Side = side;  //六边形的边长（设小六边形的边长为单位1）
	MaxLine = 4 * Side + 1;  //最大行数
	MaxColumn = 8 * Side + 2;  //最大列数
	resPos.first = 0;
	resPos.second = MaxLine + 2;

	for (int i = 0;i <= 108;i++)
	{
		for (int j = 0;j <= 55;j++)
		{
			if(j<=MaxLine+2 && i<=MaxColumn+2)
			    record[i][j] = c;
			else
				record[i][j] = ' ';
		}
	}
}

SSGo::SSGo() { myChat = NULL; }

//////////////////////////////////////
//析构函数
//////////////////////////////////////
SSGo::~SSGo()
{
	for (int i = 0;i <= 108;i++)
	{
		for (int j = 0;j <= 55;j++)
			record[i][j] = '\0';
	}
}

//////////////////////////////////////
//设置输出位置
//////////////////////////////////////
void SSGo::SetOutPos(HANDLE hStd,pair<int, int> pos)
{
	COORD setPS;  //结构体，表示在控制台屏幕上的坐标。
	setPS.X = pos.first;
	setPS.Y = pos.second;
	SetConsoleCursorPosition(hStd, setPS);
}

//////////////////////////////////////
//选点\操作
//////////////////////////////////////
pair<int, int> SSGo::Chose(pair<int, int> pos)
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);  //定义并实例化“标准输出”句柄hStd
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);  //定义并实例化输入句柄hStd
	DWORD dwRes;  //相当于unsigned long
	INPUT_RECORD keyRec;
	char ch;
	pair<int, int> Pos = pos;
	SetOutPos(hOut, Pos);
	while (true)
	{
		ReadConsoleInput(hIn, &keyRec, 1, &dwRes);
		if (keyRec.EventType == KEY_EVENT)
		{
			// 只在按下时判断，弹起时不判断  
			if (keyRec.Event.KeyEvent.bKeyDown)
			{
				//基础功能键----------------------------------
				switch (keyRec.Event.KeyEvent.wVirtualKeyCode)
				{
				case VK_RETURN:   // 按回车 选定落子点
					if (canDown(Pos, isBlack))
					{
						putChess(Pos, isBlack);
						return Pos;
					}
					break;
				case VK_SPACE:    // 按空格 todo 功能：暂停选点操作，回到聊天状态 返回(0,0)
					myChat->isTyping = true;
					SetOutPos(hOut, pair<int, int>(resPos.first, resPos.second + 1));
					while (myChat->isTyping)
					{
						Sleep(500);
					}
					SetOutPos(hOut, Pos);
					//CloseHandle(hIn);
					//CloseHandle(hOut);
					//return pair<int, int>(0, 0);
					break;
				case VK_ESCAPE:   // 按ESC键时 呼叫菜单 返回的坐标表示对应的操作
					int menu = callMenu(Pos);
					if (menu == 1)  //停一手，且前一手对方也停一手（此时已经输出完结果，需要结束start里的循环）
					{
						return pair<int, int>(3, 0);
					}
					else if (menu == 0)  //停一手，且前一手对方没停一手
					{
						if(isBlack)
						    return pair<int, int>(1, 0);
						else
							return pair<int, int>(2, 0);
					}
					else if (menu == 2)  //认输
					{
						if (isBlack)
							return pair<int, int>(0, 1);
						else
							return pair<int, int>(0, 2);
					}
					break;
				}
				//wsad移动-----------------------------------
 				ch = keyRec.Event.KeyEvent.uChar.AsciiChar;  
				if (ch=='w')
				{
					Pos = moveW(Pos);
					SetOutPos(hOut, Pos);
				}
				else if (ch == 's')
				{
					Pos = moveS(Pos);
					SetOutPos(hOut, Pos);
				}
				else if (ch == 'a')
				{
					Pos = moveA(Pos);
					SetOutPos(hOut, Pos);
				}
				else if (ch == 'd')
				{
					Pos = moveD(Pos);
					SetOutPos(hOut, Pos);
				}
			}
		}
	}
}

//////////////////////////////////////
//判断是否可以落子
//////////////////////////////////////
bool SSGo::canDown(pair<int, int> pos, bool isBlack)
{
	//黑棋有打劫
	if(posJieB.first!=0 && isBlack)
	{
		if (pos == posJieB)
		{
			return false;
		}
		myChat->Room->Send("#JieBI#" + to_string(0) + "#J#" + to_string(0) + "#end#");
		posJieB.first = 0;
		posJieB.second = 0;
	}
	//白有打劫
	if (posJieW.first != 0 && !isBlack)
	{
		if (pos == posJieW)
		{
			return false;
		}
		myChat->Room->Send("#JieWI#" + to_string(0) + "#J#" + to_string(0) + "#end#");
		posJieW.first = 0;
		posJieW.second = 0;
	}

	int i = pos.first;  //第i列
	int j = pos.second;  //第j行
	if (record[i][j] != 'n')
		return false;
	pair<int, int> tempPos(0,0);
	pair<int, int> enemyPos[3];
	bool oneBeEat = false;  //被吃敌子是否只有一子
	enemyPos[0] = tempPos;enemyPos[1] = tempPos;enemyPos[2] = tempPos;
	int n_enemy = 0;
	bool enemyAllLive = true;
	bool candown = false;
	bool beJie = false;
	char tempChar;
	char myChar, enChar;
	if (isBlack)
	{
		myChar = 'B';
		enChar = 'W';
	}
	else
	{
		myChar = 'W';
		enChar = 'B';
	}

	record[i][j] = myChar;  //假设落子

	//wsad四个方向检查敌子或nu：
	tempPos = moveW(pos);
	if((tempChar = record[tempPos.first][tempPos.second]) == 'n')
		candown = true;
	else if (tempChar == enChar)
	{    //如果没有记录，就记录enChar的位置
		int a;
		for (a = 0;a < 3;a++)
			if (tempPos == enemyPos[a])
				break;
		if (a == 3)
		{
			enemyPos[n_enemy] = tempPos;
			n_enemy++;
		}
	}
	tempPos = moveS(pos);
	if ((tempChar = record[tempPos.first][tempPos.second]) == 'n')
		candown = true;
	else if (tempChar == enChar)
	{    //如果没有记录，就记录enChar的位置
		int a;
		for (a = 0;a < 3;a++)
			if (tempPos == enemyPos[a])
				break;
		if (a == 3)
		{
			enemyPos[n_enemy] = tempPos;
			n_enemy++;
		}
	}
	tempPos = moveA(pos);
	if ((tempChar = record[tempPos.first][tempPos.second]) == 'n')
		candown = true;
	else if (tempChar == enChar)
	{    //如果没有记录，就记录enChar的位置
		int a;
		for (a = 0;a < 3;a++)
			if (tempPos == enemyPos[a])
				break;
		if (a == 3)
		{
			enemyPos[n_enemy] = tempPos;
			n_enemy++;
		}
	}
	tempPos = moveD(pos);
	if ((tempChar = record[tempPos.first][tempPos.second]) == 'n')
		candown = true;
	else if (tempChar == enChar)
	{    //如果没有记录，就记录enChar的位置
		int a;
		for (a = 0;a < 3;a++)
			if (tempPos == enemyPos[a])
				break;
		if (a == 3)
		{
			enemyPos[n_enemy] = tempPos;
			n_enemy++;
		}
	}

	//判断周围敌子死活
	for (int a = 0;a < n_enemy;a++)
	{
		if (record[enemyPos[a].first][enemyPos[a].second] == 'n')
			continue;
		if (!isLive(enemyPos[a], !isBlack))
		{
			enemyAllLive = false;
			if (block.size() == 1)
				oneBeEat = true;
			clearDied();  //清除死子
		}
	}

	//判断是否形成打劫
	if (n_enemy == 3)
	{
		int numBeEat = 0;
		pair<int, int> posJie;
		for (int a = 0;a < n_enemy;a++)
			if (record[enemyPos[a].first][enemyPos[a].second] == 'n')
			{
				numBeEat++;
				posJie = enemyPos[a];
			}
		if (numBeEat == 1 && oneBeEat)  //打劫成立
		{
			if (isBlack)
			{
				posJieW = posJie;
				myChat->Room->Send("#JieWI#" + to_string(posJie.first) + "#J#" + to_string(posJie.second) + "#end#");
			}
			else
			{
				posJieB = posJie;
				myChat->Room->Send("#JieBI#" + to_string(posJie.first) + "#J#" + to_string(posJie.second) + "#end#");
			}
		}
	}

	if (enemyAllLive)  //如果全部敌子为活棋
	{
		if (isLive(pos, isBlack))
			return true;
		else
		{
			record[i][j] = 'n';
			return false;
		}
	}
	else
		return true;
	if(candown)
		return true;
}

//////////////////////////////////////
//判断是否为活棋
//////////////////////////////////////
bool SSGo::isLive(pair<int, int> pos, bool isBlack)
{
	char ch = record[pos.first][pos.second];
	char myChar, enChar;
	if (isBlack)
	{
		myChar = 'B';
		enChar = 'W';
	}
	else
	{
		myChar = 'W';
		enChar = 'B';
	}

	if (ch == myChar)
	{
		if (notInBlock(pos))  //没标记过
		{
			block.push_back(pos);
			if (isLive(moveW(pos), isBlack))
			{
				block.clear();
				return true;
			}
			if (isLive(moveS(pos), isBlack))
			{
				block.clear();
				return true;
			}
			if (isLive(moveA(pos), isBlack))
			{
				block.clear();
				return true;
			}
			if (isLive(moveD(pos), isBlack))
			{
				block.clear();
				return true;
			}
		}
		else
			return false;
	}
	else if (ch == 'n')
	{
		block.clear();
		return true;
	}
	else if (ch == enChar)
	{
		return false;
	}
	return false;
}

bool SSGo::notInBlock(pair<int, int> pos)
{
	for (vector<int>::size_type ix = 0; ix != block.size(); ix++)
		if (block[ix] == pos)
			return false;
	return true;
}

//////////////////////////////////////
//点目，判断是否黑棋赢（中国规则，不贴目，
//目前算法只有无棋可下的情况下才准确）
//////////////////////////////////////
int SSGo::blackWin()
{
	int c,i,j;
	countB = 0;
	for (c = 0;c < count;c++)  //遍历棋盘上所有位置
	{
		i = points[c].first;
		j = points[c].second;
		if (record[i][j] == 'B' && notInBlock(points[c]))
		{
			countB += countNumB(points[c]);
		}
	}
	countW = count - countB;
	if (countB < countW)
		return 0;
	else if (countB > countW)
		return 1;
	else
		return 2;
}

int SSGo::countNumB(pair<int, int> pos)
{
	int res = 0;
	if (!notInBlock(pos))
		return res;
	int i = pos.first;
	int j = pos.second;
	char ch = record[i][j];
	if (ch=='B' || ch=='n')
	{
		block.push_back(pos);
		res++;
		res += countNumB(moveW(pos));
		res += countNumB(moveS(pos));
		res += countNumB(moveA(pos));
		res += countNumB(moveD(pos));
	}	
	return res;
}

//////////////////////////////////////
//清除死子（也就是block中的点）
//////////////////////////////////////
void SSGo::clearDied()
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);  //定义并实例化“标准输出”句柄hStd
	for (vector<int>::size_type ix = 0; ix != block.size(); ix++)
	{
		int i = block[ix].first;
		int j = block[ix].second;
		SetOutPos(hOut, block[ix]);
		cout << "nu";
		record[i][j] = 'n';
		record[i + 1][j] = 'u';
		SetOutPos(hOut, block[ix]);
		//CloseHandle(hOut);
	}
	block.clear();
}

//////////////////////////////////////
//落子
//////////////////////////////////////
void SSGo::putChess(pair<int, int> pos, bool isBlack)
{
	int i = pos.first;
	int j = pos.second;

	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);  //定义并实例化“标准输出”句柄hStd
	SetOutPos(hOut, pos);
	if (isBlack)
	{
		SetConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY | BACKGROUND_BLUE);
		cout << "Bb";
		record[i][j] = 'B';
		record[i+1][j] = 'b';
		SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
	else
	{
		SetConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN);
		cout << "Ww";
		record[i][j] = 'W';
		record[i + 1][j] = 'w';
		SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
	//CloseHandle(hOut);//关闭句柄使得之后的CloseHandle(hOut)会引发异常（重复关闭）

	//落的子是对手的子，则判断周围自己的子死活
	if (this->isBlack != isBlack)  
	{
		pair<int, int> tempP;
		char myCh;
		if (this->isBlack)
			myCh = 'B';
		else
			myCh = 'W';
		tempP = moveW(pos);
		if (record[tempP.first][tempP.second] == myCh)
			if (!isLive(tempP, this->isBlack))
				clearDied();
		tempP = moveS(pos);
		if (record[tempP.first][tempP.second] == myCh)
			if (!isLive(tempP, this->isBlack))
				clearDied();
		tempP = moveA(pos);
		if (record[tempP.first][tempP.second] == myCh)
			if (!isLive(tempP, this->isBlack))
				clearDied();
		tempP = moveD(pos);
		if (record[tempP.first][tempP.second] == myCh)
			if (!isLive(tempP, this->isBlack))
				clearDied();
	}
	//落的是自己的子，则广播通信协议
	else
	    myChat->Room->Send("#I#" + to_string(i) + "#J#" + to_string(j) + "#end#");

	SetOutPos(hOut, pos);
}

//////////////////////////////////////
//画棋盘
//////////////////////////////////////
void SSGo::drawBG(int Side)
{ 
	//发送DOS命令
	system("cls");
	int width = MaxColumn + 32;
	int high = MaxLine + 8;
	string tmp = "mode con cols=" + to_string(width) + " lines=" + to_string(high);
	system(tmp.c_str());

	int max_line = MaxLine;
	int mid_line = Side + Side + 1;
	int n;
	int col;
	int n_char, n_skip;
	bool b;  //true表示前一半
	for (n = 1;n <= max_line;n++)
	{
		if (n <= mid_line)  //前一半
		{
			n_char = 4 * Side + n + n;
			n_skip = Side + Side - n + 1;
			b = true;
		}
		else  //后一半
		{
			n_char = 12 * Side - n - n + 4;
			n_skip = -Side - Side + n - 1;
			b = false;
		}
		/*for (int i = 1;i <= n_skip;i++)
			record[i][n] = ' ';*/
		if (n % 2 == 1)  //奇数
		{
			int cas = n_char % 6;
			int i = n_char / 6;
			bool bb = true;
			if (cas == 0)
			{
				if (n == 1 || n == max_line)
				{
					//cout << "----nu";
					record[n_skip + 1][n] = '-';
					record[n_skip + 2][n] = '-';
					record[n_skip + 3][n] = '-';
					record[n_skip + 4][n] = '-';
					record[n_skip + 5][n] = 'n';
					record[n_skip + 6][n] = 'u';
					col = n_skip + 6;
					for (int j = 1;j < i-1;j++)
					{
						if (bb)
						{
							//cout << "------";
							record[col + 1][n] = '-';
							record[col + 2][n] = '-';
							record[col + 3][n] = '-';
							record[col + 4][n] = '-';
							record[col + 5][n] = '-';
							record[col + 6][n] = '-';
							col = col + 6;
							bb = false;
						}
						else
						{
							//cout << "nu--nu";
							record[col + 1][n] = 'n';
							record[col + 2][n] = 'u';
							record[col + 3][n] = '-';
							record[col + 4][n] = '-';
							record[col + 5][n] = 'n';
							record[col + 6][n] = 'u';
							col = col + 6;
							bb = true;
						}
					}
					if (bb)
					{
						//cout << "------";
						record[col + 1][n] = '-';
						record[col + 2][n] = '-';
						record[col + 3][n] = '-';
						record[col + 4][n] = '-';
						record[col + 5][n] = '-';
						record[col + 6][n] = '-';
					}
					else
					{
						//cout << "nu----";
						record[col + 1][n] = 'n';
						record[col + 2][n] = 'u';
						record[col + 3][n] = '-';
						record[col + 4][n] = '-';
						record[col + 5][n] = '-';
						record[col + 6][n] = '-';
					}
				}
				else
				{
					col = n_skip;
					for (int j = 1;j <= i;j++)
					{
						if (bb)
						{
							//cout << "nu--nu";
							record[col + 1][n] = 'n';
							record[col + 2][n] = 'u';
							record[col + 3][n] = '-';
							record[col + 4][n] = '-';
							record[col + 5][n] = 'n';
							record[col + 6][n] = 'u';
							col = col + 6;
							bb = false;
						}
						else
						{
							//cout << "      ";
							record[col + 1][n] = ' ';
							record[col + 2][n] = ' ';
							record[col + 3][n] = ' ';
							record[col + 4][n] = ' ';
							record[col + 5][n] = ' ';
							record[col + 6][n] = ' ';
							col = col + 6;
							bb = true;
						}
					}
				}
			}
			else if (cas == 4)
			{
				if (n == mid_line)
				{
					//cout << "| ";
					record[1][n] = '|';
					record[2][n] = ' ';
					col = 2;
					for (int j = 1;j <= i;j++)
					{
						if (bb)
						{
							//cout << "      ";
							record[col + 1][n] = ' ';
							record[col + 2][n] = ' ';
							record[col + 3][n] = ' ';
							record[col + 4][n] = ' ';
							record[col + 5][n] = ' ';
							record[col + 6][n] = ' ';
							col = col + 6;
							bb = false;
						}
						else
						{
							//cout << "nu--nu";
							record[col + 1][n] = 'n';
							record[col + 2][n] = 'u';
							record[col + 3][n] = '-';
							record[col + 4][n] = '-';
							record[col + 5][n] = 'n';
							record[col + 6][n] = 'u';
							col = col + 6;
							bb = true;
						}
					}
					//cout << " |";
					record[col + 1][n] = ' ';
					record[col + 2][n] = '|';
				}
				else if (n == 1 || n == max_line)
				{
					//cout << "nu";
					record[n_skip+1][n] = 'n';
					record[n_skip+2][n] = 'u';
					col = n_skip+2;
					for (int j = 1;j <= i;j++)
					{
						if (bb)
						{
							//cout << "------";
							record[col + 1][n] = '-';
							record[col + 2][n] = '-';
							record[col + 3][n] = '-';
							record[col + 4][n] = '-';
							record[col + 5][n] = '-';
							record[col + 6][n] = '-';
							col = col + 6;
							bb = false;
						}
						else
						{
							//cout << "nu--nu";
							record[col + 1][n] = 'n';
							record[col + 2][n] = 'u';
							record[col + 3][n] = '-';
							record[col + 4][n] = '-';
							record[col + 5][n] = 'n';
							record[col + 6][n] = 'u';
							col = col + 6;
							bb = true;
						}
					}
					//cout << "nu";
					record[col + 1][n] = 'n';
					record[col + 2][n] = 'u';
				}
				else 
				{
					//cout << "nu";
					record[n_skip + 1][n] = 'n';
					record[n_skip + 2][n] = 'u';
					col = n_skip+2;
					for (int j = 1;j <= i;j++)
					{
						if (bb)
						{
							//cout << "      ";
							record[col + 1][n] = ' ';
							record[col + 2][n] = ' ';
							record[col + 3][n] = ' ';
							record[col + 4][n] = ' ';
							record[col + 5][n] = ' ';
							record[col + 6][n] = ' ';
							col = col + 6;
							bb = false;
						}
						else
						{
							//cout << "nu--nu";
							record[col + 1][n] = 'n';
							record[col + 2][n] = 'u';
							record[col + 3][n] = '-';
							record[col + 4][n] = '-';
							record[col + 5][n] = 'n';
							record[col + 6][n] = 'u';
							col = col + 6;
							bb = true;
						}
					}
					//cout << "nu";
					record[col+1][n] = 'n';
					record[col+2][n] = 'u';
				}
			}
			else if (cas == 2)
			{
				if (n == mid_line)
				{
					//cout << "|   ";
					record[1][n] = '|';
					record[2][n] = ' ';
					record[3][n] = ' ';
					record[4][n] = ' ';
					col = 4;
					for (int j = 1;j < i;j++)
					{
						if (bb)
						{
							//cout << "nu--nu";
							record[col + 1][n] = 'n';
							record[col + 2][n] = 'u';
							record[col + 3][n] = '-';
							record[col + 4][n] = '-';
							record[col + 5][n] = 'n';
							record[col + 6][n] = 'u';
							col = col + 6;
							bb = false;
						}
						else
						{
							//cout << "      ";
							record[col + 1][n] = ' ';
							record[col + 2][n] = ' ';
							record[col + 3][n] = ' ';
							record[col + 4][n] = ' ';
							record[col + 5][n] = ' ';
							record[col + 6][n] = ' ';
							col = col + 6;
							bb = true;
						}
					}
					//cout << "   |";
					record[col + 1][n] = ' ';
					record[col + 2][n] = ' ';
					record[col+3][n] = ' ';
					record[col+4][n] = '|';
				}
				else if (n == 1 || n == max_line)
				{
					//cout << "----";
					record[n_skip + 1][n] = '-';
					record[n_skip + 2][n] = '-';
					record[n_skip + 3][n] = '-';
					record[n_skip + 4][n] = '-';
					col = n_skip + 4;
					for (int j = 1;j < i;j++)
					{
						if (bb)
						{
							//cout << "nu--nu";
							record[col + 1][n] = 'n';
							record[col + 2][n] = 'u';
							record[col + 3][n] = '-';
							record[col + 4][n] = '-';
							record[col + 5][n] = 'n';
							record[col + 6][n] = 'u';
							col = col + 6;
							bb = false;
						}
						else
						{
							//cout << "------";
							record[col + 1][n] = '-';
							record[col + 2][n] = '-';
							record[col + 3][n] = '-';
							record[col + 4][n] = '-';
							record[col + 5][n] = '-';
							record[col + 6][n] = '-';
							col = col + 6;
							bb = true;
						}
					}
					//cout << "----";
					record[col + 1][n] = '-';
					record[col + 2][n] = '-';
					record[col + 3][n] = '-';
					record[col + 4][n] = '-';
				}
				else
				{
					if (b)
					{
						//cout << "/   ";
						record[n_skip + 1][n] = '/';
						record[n_skip + 2][n] = ' ';
						record[n_skip + 3][n] = ' ';
						record[n_skip + 4][n] = ' ';
					}
					else
					{
						//cout << "\\   ";
						record[n_skip + 1][n] = '\\';
						record[n_skip + 2][n] = ' ';
						record[n_skip + 3][n] = ' ';
						record[n_skip + 4][n] = ' ';
					}
					col = n_skip + 4;
					for (int j = 1;j < i;j++)
					{
						if (bb)
						{
							//cout << "nu--nu";
							record[col + 1][n] = 'n';
							record[col + 2][n] = 'u';
							record[col + 3][n] = '-';
							record[col + 4][n] = '-';
							record[col + 5][n] = 'n';
							record[col + 6][n] = 'u';
							col = col + 6;
							bb = false;
						}
						else
						{
							//cout << "      ";
							record[col + 1][n] = ' ';
							record[col + 2][n] = ' ';
							record[col + 3][n] = ' ';
							record[col + 4][n] = ' ';
							record[col + 5][n] = ' ';
							record[col + 6][n] = ' ';
							col = col + 6;
							bb = true;
						}
					}
					if (b)
					{
						//cout << "   \\";
						record[col + 1][n] = ' ';
						record[col + 2][n] = ' ';
						record[col + 3][n] = ' ';
						record[col + 4][n] = '\\';
					}
					else
					{
						//cout << "   /";
						record[col + 1][n] = ' ';
						record[col + 2][n] = ' ';
						record[col + 3][n] = ' ';
						record[col + 4][n] = '/';
					}
				}
			}
		}
		else  //偶数
		{
			int cas = (n_char - 2) % 6;
			int i = (n_char - 2) / 6;
			bool bb;
			if (b)
				bb = true;
			else
				bb = false;
			if (cas == 0)
			{
				col = n_skip;
				for (int j = 1;j <= i;j++)
				{
					if (bb)
					{
						//cout << "/     ";
						record[col + 1][n] = '/';
						record[col + 2][n] = ' ';
						record[col + 3][n] = ' ';
						record[col + 4][n] = ' ';
						record[col + 5][n] = ' ';
						record[col + 6][n] = ' ';
						col = col + 6;
						bb = false;
					}
					else
					{
						//cout << "\\     ";
						record[col + 1][n] = '\\';
						record[col + 2][n] = ' ';
						record[col + 3][n] = ' ';
						record[col + 4][n] = ' ';
						record[col + 5][n] = ' ';
						record[col + 6][n] = ' ';
						col = col + 6;
						bb = true;
					}
				}
				record[col + 1][n] = ' ';
				if (b)					
					record[col + 2][n] = '\\';
				else
					record[col + 2][n] = '/';
			}
			else if (cas == 4)
			{
				col = n_skip;
				if (b)
					record[col + 1][n] = '/';
				else
					record[col + 1][n] = '\\';
				record[col + 2][n] = ' ';
				col = col + 2;
				for (int j = 1;j <= i;j++)
				{
					if (bb)
					{
						//cout << "\\     ";
						record[col + 1][n] = '\\';
						record[col + 2][n] = ' ';
						record[col + 3][n] = ' ';
						record[col + 4][n] = ' ';
						record[col + 5][n] = ' ';
						record[col + 6][n] = ' ';
						col = col + 6;
						bb = false;
					}
					else
					{
						//cout << "/     ";
						record[col + 1][n] = '/';
						record[col + 2][n] = ' ';
						record[col + 3][n] = ' ';
						record[col + 4][n] = ' ';
						record[col + 5][n] = ' ';
						record[col + 6][n] = ' ';
						col = col + 6;
						bb = true;
					}
				}
				record[col + 1][n] = ' ';
				if (b)
				{
					record[col + 2][n] = '/';
					record[col + 3][n] = ' ';
					record[col + 4][n] = '\\';
				}
				else
				{
					record[col + 2][n] = '\\';
					record[col + 3][n] = ' ';
					record[col + 4][n] = '/';
				}
			}
			else if (cas == 2)
			{
				col = n_skip;
				if (b)
				{
					record[col + 1][n] = '/';
					record[col + 2][n] = ' ';
					record[col + 3][n] = ' ';
					record[col + 4][n] = ' ';
					col = col + 4;
				}
				else
				{
					record[col + 1][n] = '\\';
					record[col + 2][n] = ' ';
					record[col + 3][n] = ' ';
					record[col + 4][n] = ' ';
					col = col + 4;
				}
				for (int j = 1;j < i;j++)
				{
					if (bb)
					{
						//cout << "/     ";
						record[col + 1][n] = '/';
						record[col + 2][n] = ' ';
						record[col + 3][n] = ' ';
						record[col + 4][n] = ' ';
						record[col + 5][n] = ' ';
						record[col + 6][n] = ' ';
						col = col + 6;
						bb = false;
					}
					else
					{
						//cout << "\\     ";
						record[col + 1][n] = '\\';
						record[col + 2][n] = ' ';
						record[col + 3][n] = ' ';
						record[col + 4][n] = ' ';
						record[col + 5][n] = ' ';
						record[col + 6][n] = ' ';
						col = col + 6;
						bb = true;
					}
				}
				record[col + 1][n] = ' ';
				record[col + 3][n] = ' ';
				record[col + 4][n] = ' ';
				record[col + 5][n] = ' ';
				if (b)
				{
					record[col + 2][n] = '\\';
					record[col + 6][n] = '\\';
				}
				else
				{
					record[col + 2][n] = '/';
					record[col + 6][n] = '/';
				}
			}
		}
	}
	count = 0;

	for (n = 0;n <= MaxLine+1; n++)
	{
		for (col = 0;col <= MaxColumn+1;col++)
		{
			cout << record[col][n];
			if (record[col][n] == 'n')
			{
				count++;
				pair<int, int> pos(col, n);
				points.push_back(pos);
			}
		}
		cout << endl;
	}

	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	pair<int, int> pos(0, 0);
	SetFont(pos);  //设置默认的字体
	pos.first = MaxColumn + 2;
	pos.second = 1;
	SetOutPos(hOut, pos);
	cout << "Black        v.s        White";
	pos.second = 3;
	SetOutPos(hOut, pos);
	cout << "Don't move the borderline!";
	pos.second = 4;
	SetOutPos(hOut, pos);
	cout << "Use the English input method,";
	pos.second = 5;
	SetOutPos(hOut, pos);
	cout << "W S A D to move the cursor,";
	pos.second = 6;
	SetOutPos(hOut, pos);
	cout << "Enter to put your chess,";
	pos.second = 7;
	SetOutPos(hOut, pos);
	cout << "Esc to call the menu,";
	pos.second = 8;
	SetOutPos(hOut, pos);
	cout << "Space to type texts and chat.";

	chatLine = high - 13;
	maxChatLine = chatLine;
}

//////////////////////////////////////
//棋手名字显示
//////////////////////////////////////
void SSGo::drawBName(string name)
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	pair<int, int> pos(MaxColumn + 2, 2);
	SetOutPos(hOut, pos);
	cout << name;
}
void SSGo::drawWName(string name)
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	int len = strlen(name.c_str());
	pair<int, int> pos(MaxColumn + 31 - len, 2);
	SetOutPos(hOut, pos);
	cout << name;
}

//////////////////////////////////////
//聊天内容显示
//////////////////////////////////////
void SSGo::drawChatText(string s)
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	//将s拆分为长度29以内的字符串数组
	int len = s.length();
	int i = len / 29;

	//清除消息编辑区
	pair<int, int> typePos(resPos.first, resPos.second + 1);
	SetOutPos(hOut,typePos);
	string ss = "";
	for (int a = 0;a < len;a++)
		ss = ss + " ";
	cout << ss;

	//如果剩下的行数不够，从最上面1行开始，覆盖原来的记录。
	if (i > chatLine)
	{
		chatLine = maxChatLine;
		s = s + "                             ";
		len += 29;
		i++;
	}
	
	SetOutPos(hOut, posDrawChat());

	string str;
    for (int a = 0;a < i;a++)
	{
		str = s.substr(29 * a, 29);
		cout << str;
		if(a!=i-1)
		    SetOutPos(hOut, posDrawChat());
	}
	int lastLen = len - 29 * i;
	str = s.substr(29 * i, lastLen);
	if (len % 29 != 0)
	{
		SetOutPos(hOut, posDrawChat());
		lastLen = 29 - lastLen;
		for (int a = 0;a < lastLen;a++)
		{
			str = str + " ";
		}
		cout << str;
	}

	myChat->isTyping = false;
}

//////////////////////////////////////
//获取输出聊天记录的位置
////////>//////////////////////////////
pair<int, int> SSGo::posDrawChat()
{
	int i = MaxColumn + 2;
	int j = 9 + maxChatLine - chatLine;
	chatLine--;
	return pair<int, int>(i, j);
}

//////////////////////////////////////
//Esc菜单
//////////////////////////////////////
int SSGo::callMenu(pair<int, int> pos)
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetOutPos(hOut, resPos);
	cout << "Chose what you want to do by input these numbers:" << endl;
	cout << "1: Pause a hand (you can do it only in your turn)" << endl;
	cout << "2: Admit defeat, 3: Change the styles of font.";

	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);  //实例化输入句柄hStd
	DWORD dwRes;
	INPUT_RECORD keyRec;
	char ch;

	while (true)
	{
		ReadConsoleInput(hIn, &keyRec, 1, &dwRes);
		if (keyRec.EventType == KEY_EVENT)
		{
			if (keyRec.Event.KeyEvent.bKeyDown)
			{
				ch = keyRec.Event.KeyEvent.uChar.AsciiChar;
				if (ch == '1')
				{
					SetOutPos(hOut, resPos);
					cout << "                                                 " << endl;
					cout << "                                                 " << endl;
					cout << "                                                 ";
					if (isBlack == turn)
					{
						int bothPause = pauseHand();
						SetOutPos(hOut, pos);
						return bothPause;  //1或0
					}
					else
					{
						cout << "  It's not your turn!";
					}
				}
				else if (ch == '2')
				{
					SetOutPos(hOut, resPos);
					cout << "                                                 " << endl;
					cout << "                                                 " << endl;
					cout << "                                                 ";
					admitDefeat(isBlack);
					return 2;
				}
				else if (ch == '3')
				{
					SetOutPos(hOut, resPos);
					cout << "                                                 " << endl;
					cout << "                                                 " << endl;
					cout << "                                                 ";
					SetFont(pos);
					return 3;
				}
				//todo:调取聊天记录
				/*else if (ch == '4')
				{
					SetOutPos(hOut, resPos);
					cout << "                                                 " << endl;
					cout << "                                                 " << endl;
					cout << "                                                 ";                                              ";
					
					SetOutPos(hOut, pos);
					return 4;
				}*/
			}
		}
	}//end while
}

//////////////////////////////////////
//停一手
//////////////////////////////////////
int SSGo::pauseHand()
{
	bool bothPause = false;
	if (isBlack)
	{
		myChat->Room->Send("#I#1#J#0#end#");
		if (enemyI == 2 && enemyJ == 0)
			bothPause = true;
	}
	else
	{
		myChat->Room->Send("#I#2#J#0#end#");
		if (enemyI == 1 && enemyJ == 0)
			bothPause = true;
	}
	//如果对方也停一手
	if(bothPause)
	//输赢判断
	{
		putOutResult(blackWin());
		return 1;
	}
	return 0;
}

////////////////////////////////////////
//输出点目结果
////////////////////////////////////////
void SSGo::putOutResult(int winOrLose)
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetOutPos(hOut, resPos);
	if (winOrLose == 1)
	{
		cout << " *****Black win!***** " << endl;
		myChat->Room->Send("#R#B" + to_string(countB - countW) + "#end#");
	}
	else if (winOrLose == 0)
	{
		cout << " *****White win!***** " << endl;
		myChat->Room->Send("#R#W" + to_string(countB - countW) + "#end#");
	}
	else if (winOrLose == 2)
	{
		cout << " Black and white are tie!" << endl;
		myChat->Room->Send("#R#=#end#");
	}
	cout << " Number of black's points is " << countB << "," << endl;
	cout << " while number of white's points is " << countW << ". " << endl;
	system("pause");
}

////////////////////////////////////////
//认输
////////////////////////////////////////
void SSGo::admitDefeat(bool black)
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetOutPos(hOut, resPos);
	if (black)
	{
		cout << "  Black admits that he lose!" << endl;
		if (isBlack)
		{
			cout << " You lose !" << endl;
			myChat->Room->Send("#I#0#J#1#end#");
		}
		else
			cout << " You win !" << endl;
	}
	else
	{
		cout << "  White admits that he lose!" << endl;
		if(isBlack)
			cout << " You win !" << endl;
		else
		{
			cout << " You lose !" << endl;
			myChat->Room->Send("#I#0#J#2#end#");
		}
	}
	system("pause");
}

////////////////////////////////////////
//开始对弈
////////////////////////////////////////
void SSGo::start(bool Black, bool isOnline)
{
	isBlack = Black;  //设置黑白
	
	//初始化选点位置
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	pair<int, int> Pos;
	int x_mid, y_mid = 0;
	int n_mid = count / 2, n_count = 0;
	bool tmp_bool = true;
	while (tmp_bool)
	{
		y_mid++; //行 放在外循环
		for (x_mid = 1;x_mid <= MaxColumn;x_mid++)
		{
			if (record[x_mid][y_mid] == 'n')
			{
				n_count++;
				if (n_count >= n_mid)
				{
					tmp_bool = false;
					break;
				}
			}
		}
	}
	Pos.first = x_mid; Pos.second = y_mid;
	SetOutPos(hOut, Pos);

	pair<int, int> tempPos = Pos;
	//单机版
	if (!isOnline)
	{
		while (true)
		{
			if ((tempPos.first) != 0)
				tempPos = Chose(tempPos);//从上次光标在的位置开始选点
			else
				tempPos = Chose(Pos);    //从初始选点位置开始选点
			isBlack = !isBlack;  //黑白转换
		}
	}
	//联网版
	else
	{
		while (true)
		{
			//轮到对方
			if(isBlack!=turn)
			{
				SetOutPos(hOut, resPos);
				if (isBlack)
					cout << " The White's turn...  " << endl;
				else
					cout << " The Black's turn...  " << endl;

				HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);  //定义并实例化输入句柄hStd
				DWORD dwRes;  //相当于unsigned long
				INPUT_RECORD keyRec;
				
				while (true)
				{
					//在对方的回合按空格可以打字聊天
					ReadConsoleInput(hIn, &keyRec, 1, &dwRes);
					if (keyRec.EventType == KEY_EVENT)
					{
						if (keyRec.Event.KeyEvent.bKeyDown && keyRec.Event.KeyEvent.wVirtualKeyCode == VK_SPACE)
						{
							myChat->isTyping = true;
							SetOutPos(hOut, pair<int, int>(resPos.first, resPos.second + 1));
						}
					}
					//如果enemyI / J的数值在另一线程中被改变，表示对方已经落子 / 认输 / 投降
					if (enemyI != myI || enemyJ != myJ)
					{
						SetOutPos(hOut, resPos);
						if (isBlack)
							cout << " The Black's turn..." << endl;
						else
							cout << " The White's turn..." << endl;

						//停止打字，等待打字线程结束
						myChat->isTyping = false;
						ThreadLib::WaitForFinish(myChat->CinThread);

						//对方停一手，那么回合转换或者棋局结束
						if (enemyJ == 0)
						{
							//黑棋停一手，己方是白棋
							if (enemyI == 1 && isBlack == false)
							{
								SetOutPos(hOut, resPos);
								cout << " 黑棋停一手...       " << endl;
								if (myI == 2 && myJ == 0)
								{
									putOutResult(blackWin());
									myChat->isStart = false;
									return;
								}
								else
								{
									turn = !turn;
									break;
								}
							}
							//白棋停一手，己方是黑棋
							else if (enemyI == 2 && isBlack == true)
							{
								SetOutPos(hOut, resPos);
								cout << " 白棋停一手...       " << endl;
								if (myI == 1 && myJ == 0)
								{
									putOutResult(blackWin());
									myChat->isStart = false;
									return;
								}
								else
								{
									turn = !turn;
									break;
								}
							}
						}
						//对方认输
						if (enemyI == 0)
						{
							if (enemyJ == 1)
							{
								admitDefeat(true);
								myChat->isStart = false;
								return;
							}
							else if (enemyJ == 2)
							{
								admitDefeat(false);
								myChat->isStart = false;
								return;
							}
						}
						tempPos = pair<int, int>(enemyI, enemyJ);
						putChess(tempPos, !isBlack);
						turn = !turn;
						break;
					}
					Sleep(50);
				}
			}
			//轮到己方
			else
			{
				SetOutPos(hOut, resPos);
				if(isBlack)
					cout << " The Black's turn..." << endl;
				else
				    cout << " The White's turn..." << endl;
				//上一步Chose进行了落子操作
				if (myI != 0 && myJ != 0)
				{
					tempPos = Chose(tempPos);
				}
				//上一步进行了其他操作
				else
					tempPos = Chose(Pos);    //从初始选点位置开始选点

				myI = tempPos.first;
				myJ = tempPos.second;
				enemyI = myI;
				enemyJ = myJ;
				turn = !turn;
				//如果这个Chose返回特定结果，则棋局结束
				if (myI == 3 && myJ == 0)  //双方先后都停一手
				{
					myChat->isStart = false;
					return;
				}
				if (myI == 0 && (myJ == 1 || myJ == 2))  //己方认输
				{
					myChat->isStart = false;
					return;
				}
				Sleep(50);
			}
		}
	}
}

///////////////////////////////////////
//WSAD寻点
///////////////////////////////////////
pair<int, int> SSGo::moveA(pair<int, int> pos)
{
	int i = pos.first ;  //列号
	int j = pos.second ;  //行号
	if (record[i - 1][j] == '-')
	{
		pair<int, int> pt(i - 4 , j );
		if (record[i - 4][j] == '-')
			return moveA(pt);
		else
			return pt;
	}
	else
	{
		pair<int, int> pt;
		if(j == MaxLine)
		{
			pt = moveW(pos);
			if (pt.first != pos.first)  //向上有路
				return pt;
		}
		if (j == 1)
		{
			pt = moveS(pos);
			if (pt.first != pos.first)  //向下有路
				return  pt;
		}
		pt = pos;
		return pt;
	}
}

pair<int, int> SSGo::moveD(pair<int, int> pos)
{
	int i = pos.first ;  //列号
	int j = pos.second ;  //行号
	if (record[i + 2][j] == '-')
	{
		pair<int, int> pt(i + 4 , j );
		if (record[i + 4][j] == '-')
			return moveD(pt);
		else
			return pt;
	}
	else
	{
		pair<int, int> pt;
		if (j > MaxLine / 2)
		{
			return moveWD(pos);
		}
		if (j < MaxLine / 2)
		{
			return moveSD(pos);
		}
		pt = pos;
		return pt;
	}
}

pair<int, int> SSGo::moveW(pair<int, int> pos)
{
	int i = pos.first;  //列号
	int j = pos.second;  //行号
	int ii = i - 2;int jj = j - 1;
	char cc;
	for (int a = 0;a <= 3;a++)
	{
		ii++;
		if (record[ii][jj] == '\\')
		{
			ii--;jj--;
			cc = record[ii][jj];
			break;
		}
		else if (record[ii][jj] == '/')
		{
			ii++;jj--;
			cc = record[ii][jj];
			break;
		}
	}
	if(jj==(j-1))  //上方无路径
	{
		pair<int, int> pt(i, j);
		return pt;
	}
	if (cc == '\\' || cc=='|' || cc=='/')
	{
		pair<int, int> pt(ii, jj);
		return moveW(pt);
	}
	else if (cc == 'u' || cc == 'b' || cc == 'w')
	{
		pair<int, int> pt(ii-1, jj);
		return pt;
	}
	else if (cc == 'n' || cc == 'B' || cc == 'W')
	{
		pair<int, int> pt(ii, jj);
		return pt;
	}
	else if (cc == '-')
	{
		if (record[ii - 1][jj] == '-')
			ii = ii - 5;
		else
			ii = ii + 4;
		pair<int, int> pt(ii, jj);
		return pt;
	}
	else
	{
		pair<int, int> pt(i, j);
		return pt;
	}
}

pair<int, int> SSGo::moveS(pair<int, int> pos)
{
	int i = pos.first;  //列号
	int j = pos.second;  //行号
	int ii = i - 2;int jj = j + 1;
	char cc;
	for (int a = 0;a <= 3;a++)
	{
		ii++;
		if (record[ii][jj] == '\\')
		{
			ii++;jj++;
			cc = record[ii][jj];
			break;
		}
		else if (record[ii][jj] == '/')
		{
			ii--;jj++;
			cc = record[ii][jj];
			break;
		}
	}
	if (jj == (j + 1))  //下方无路径
	{
		pair<int, int> pt(i, j);
		return pt;
	}
	if (cc == '\\' || cc == '|' || cc == '/')
	{
		pair<int, int> pt(ii, jj);
		return moveS(pt);
	}
	else if (cc == 'u' || cc == 'b' || cc == 'w')
	{
		pair<int, int> pt(ii - 1, jj);
		return pt;
	}
	else if (cc == 'n' || cc == 'B' || cc == 'W')
	{
		pair<int, int> pt(ii, jj);
		return pt;
	}
	else if (cc == '-')
	{
		if (record[ii - 1][jj] == '-')
			ii = ii - 5;
		else
			ii = ii + 4;
		pair<int, int> pt(ii, jj);
		return pt;
	}
	else
	{
		pair<int, int> pt(i,j);
		return pt;
	}
}

pair<int, int> SSGo::moveWD(pair<int, int> pos)
{
	int i = pos.first;  //列号
	int j = pos.second;  //行号
	int ii = i + 3;int jj = j - 1;
	char cc;
	for (int a = 0;a <= 3;a++)
	{
		ii--;
		if (record[ii][jj] == '\\')
		{
			ii--;jj--;
			cc = record[ii][jj];
			break;
		}
		else if (record[ii][jj] == '/')
		{
			ii++;jj--;
			cc = record[ii][jj];
			break;
		}
	}
	if (jj == (j - 1))  //上方无路径
	{
		pair<int, int> pt(i, j);
		return pt;
	}
	if (cc == '\\' || cc == '|' || cc == '/')
	{
		pair<int, int> pt(ii, jj);
		return moveW(pt);
	}
	else if (cc == 'u' || cc == 'b' || cc == 'w')
	{
		pair<int, int> pt(ii - 1, jj);
		return pt;
	}
	else if (cc == 'n' || cc == 'B' || cc == 'W')
	{
		pair<int, int> pt(ii, jj);
		return pt;
	}
	else if (cc == '-')
	{
		if (record[ii - 1][jj] == '-')
			ii = ii - 5;
		else
			ii = ii + 4;
		pair<int, int> pt(ii, jj);
		return pt;
	}
	else
	{
		pair<int, int> pt(i, j);
		return pt;
	}
}

pair<int, int> SSGo::moveSD(pair<int, int> pos)
{
	int i = pos.first;  //列号
	int j = pos.second;  //行号
	int ii = i + 3;int jj = j + 1;
	char cc;
	for (int a = 0;a <= 3;a++)
	{
		ii--;
		if (record[ii][jj] == '\\')
		{
			ii++;jj++;
			cc = record[ii][jj];
			break;
		}
		else if (record[ii][jj] == '/')
		{
			ii--;jj++;
			cc = record[ii][jj];
			break;
		}
	}
	if (jj == (j + 1))  //下方无路径
	{
		pair<int, int> pt(i, j);
		return pt;
	}
	if (cc == '\\' || cc == '|' || cc == '/')
	{
		pair<int, int> pt(ii, jj);
		return moveS(pt);
	}
	else if (cc == 'u' || cc == 'b' || cc == 'w')
	{
		pair<int, int> pt(ii - 1, jj);
		return pt;
	}
	else if (cc == 'n' || cc == 'B' || cc == 'W')
	{
		pair<int, int> pt(ii, jj);
		return pt;
	}
	else if (cc == '-')
	{
		if (record[ii - 1][jj] == '-')
			ii = ii - 5;
		else
			ii = ii + 4;
		pair<int, int> pt(ii, jj);
		return pt;
	}
	else
	{
		pair<int, int> pt(i, j);
		return pt;
	}
}