#ifndef SixSideGo_H
#define SixSideGo_H

#include <winsock2.h>
#include <Windows.h>  //以上两个文件引用顺序颠倒会报错
#include <vector>
#include "Chat.h"

using namespace std;

class Chat;

class SSGo
{
public:
	SSGo(char c,int side, Chat* chat);
	SSGo();
	~SSGo();

	Chat* myChat; //联网时的通信接口
	int enemyI;  //联网时对方下棋位置
	int enemyJ;  //联网时对方下棋位置
	int myI;  //联网时己方下棋位置
	int myJ;  //联网时己方下棋位置
	bool turn;  //联网时轮到下棋的一方，true表示黑方

	bool isBlack;  //是否执黑
	vector<pair<int, int>> block;  //记录一整块棋的容器
	vector<pair<int, int>> points;  //记录可落子点的位置
	pair<int, int> posJieB;  //打劫中黑棋禁止下的位置
	pair<int, int> posJieW;  //打劫中白棋禁止下的位置
	pair<int, int> resPos;  //输出胜负结果或编辑聊天文字的位置
	int count;  //总共的落子点数目
	int countB;  //黑子目数
	int countW;  //白子目数
	int Side;  //六边形的边长（设小六边形的边长为单位1）
	int MaxLine;  //最大行数
	int MaxColumn;  //最大列数
	char record[108][55];  //棋盘记录。下标从1开始。数组在编译时就要确定大小，只能写常数。大小按照Side最大值13(106,53)给出。

	bool canDown(pair<int, int> pos, bool isBlack);  //判断是否可以落子
	bool isLive(pair<int, int> pos, bool isBlack);  //判断是否为活棋
	bool notInBlock(pair<int, int>);  //判断是否没有标记（没有存在block里）
	void start(bool isBlack, bool isOnline);  //开始对弈
	void drawBG(int Side);  //画棋盘
	void drawBName(string name);//显示黑棋棋手名字
	void drawWName(string name);//显示白棋棋手名字
	void drawChatText(string s);//显示聊天内容
	void clearDied();  //清除死子
	int callMenu(pair<int, int> pos);  //显示菜单
	int pauseHand();  //停一手 返回1表示对局结束
	void admitDefeat(bool isBlack);  //认输
	int countNumB(pair<int, int>);  //给定黑棋一个位置，数这块黑棋的目数
	int blackWin();  //判断是否黑棋赢 0=lose 1=win 2=和棋
	void putOutResult(int winOrLose);  //输出点目、输赢结果
	pair<int, int> Chose(pair<int, int> pos);  //从pos开始选点，读取键盘操作
	void putChess(pair<int, int> pos, bool isBlack);  //落子
	BOOL SetConsoleColor(WORD wAttributes);  //设置输出文字/背景颜色
	void SetFont(pair<int, int> pos);  //设置字体

private:
	int chatLine;  //剩余可供输出聊天消息的行数
	int maxChatLine;  //可供输出聊天消息的行数最大值

	pair<int, int> moveA(pair<int, int> pos);
	pair<int, int> moveD(pair<int, int> pos);
	pair<int, int> moveW(pair<int, int> pos);
	pair<int, int> moveS(pair<int, int> pos);
	pair<int, int> moveWD(pair<int, int> pos);
	pair<int, int> moveSD(pair<int, int> pos);
	static void SetOutPos(HANDLE hStd,pair<int, int> pos);  //设置输出位置
	pair<int, int> posDrawChat();  //获取输出聊天记录的位置
};

#endif