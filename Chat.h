#ifndef CHAT_H
#define CHAT_H

#include <algorithm>
#include <iostream>
#include <conio.h>
#include "db.h"
#include "Clock.h"
#include "SixSideGo.h"
#include "TCPSocket.h"
#include "SocketList.h"
#include "ThreadLib.h"

using namespace std;

class Chat
{
public:
	Chat();
	~Chat();

	int nPeople;  //房中人数
	static DB* db;  //数据库接口
	static SocketList* Room;  //本机Socket通信接口
	bool isStart;  //是否开始棋局
	bool isTyping;  //是否在打字聊天
	ThreadLib::ThreadID CinThread;  //打字聊天线程

	static void CinString(void *Receive);  //打字聊天功能（客户端）
	static void playGoB(void *Receive);  //开始对弈（客户端）
	static void playGoW(void *Receive);  //开始对弈（客户端）
	int Menu();  //首页菜单
	int Menu2();  //登录之后的菜单（客户端下完第一盘棋）
	void createAcc();  //创建账号
	bool login();  //验证账号密码
	void CreateServer();  //创建服务器
	void CreateRoom(bool hasLogin);  //创建房间，创建客户端
	void EnterRoom(bool hasLogin);//进入房间，创建客户端
	void detectingMsg();  //检测是否收到新消息

private:
	int side;  //棋盘边长
	bool isServer;  //是否为服务器
	bool alreadyChat;  //是否已经输出过
	string myName;  //自己的名字
	string BName;  //黑方的名字
	string WName;  //白方的名字
	//vector<string> NameInRoom;//出现在房间中的名字（没写观战功能暂时用不到）
	DataSocket* RecSocket;  //保存发来消息的Socket
	list<string> RecStrList;  //保存待处理消息的链表

	pair<int, int> ChatOutPos;  //聊天记录输出的起始位置
	pair<int, int> SendOutPos;  //发送框中文字输出的起始位置
	pair<int, int> MemOutPos;  //房中名字输出位置
	list<string> ChatRecords;  //保存聊天记录
	int RecordMax;  //可显示的聊天记录最大字符数值（大小与右侧聊天记录区有关）

	void Fresh();  //刷新（循环获取消息及一些指令）
	vector<string> SeparateMsg(string str);  //消息拆包
	void detectingInstruct(string str);  //服务器检查信息，处理指令
	bool detectingServer(string str);  //客户端检查信息，处理指令或聊天记录
	void freshRecStrList();  //刷新，处理RecStrList内容
	string getTime();  //获取系统时间
};


#endif