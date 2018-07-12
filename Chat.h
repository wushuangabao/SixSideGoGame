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

	int nPeople;  //��������
	static DB* db;  //���ݿ�ӿ�
	static SocketList* Room;  //����Socketͨ�Žӿ�
	bool isStart;  //�Ƿ�ʼ���
	bool isTyping;  //�Ƿ��ڴ�������
	ThreadLib::ThreadID CinThread;  //���������߳�

	static void CinString(void *Receive);  //�������칦�ܣ��ͻ��ˣ�
	static void playGoB(void *Receive);  //��ʼ���ģ��ͻ��ˣ�
	static void playGoW(void *Receive);  //��ʼ���ģ��ͻ��ˣ�
	int Menu();  //��ҳ�˵�
	int Menu2();  //��¼֮��Ĳ˵����ͻ��������һ���壩
	void createAcc();  //�����˺�
	bool login();  //��֤�˺�����
	void CreateServer();  //����������
	void CreateRoom(bool hasLogin);  //�������䣬�����ͻ���
	void EnterRoom(bool hasLogin);//���뷿�䣬�����ͻ���
	void detectingMsg();  //����Ƿ��յ�����Ϣ

private:
	int side;  //���̱߳�
	bool isServer;  //�Ƿ�Ϊ������
	bool alreadyChat;  //�Ƿ��Ѿ������
	string myName;  //�Լ�������
	string BName;  //�ڷ�������
	string WName;  //�׷�������
	//vector<string> NameInRoom;//�����ڷ����е����֣�ûд��ս������ʱ�ò�����
	DataSocket* RecSocket;  //���淢����Ϣ��Socket
	list<string> RecStrList;  //�����������Ϣ������

	pair<int, int> ChatOutPos;  //�����¼�������ʼλ��
	pair<int, int> SendOutPos;  //���Ϳ��������������ʼλ��
	pair<int, int> MemOutPos;  //�����������λ��
	list<string> ChatRecords;  //���������¼
	int RecordMax;  //����ʾ�������¼����ַ���ֵ����С���Ҳ������¼���йأ�

	void Fresh();  //ˢ�£�ѭ����ȡ��Ϣ��һЩָ�
	vector<string> SeparateMsg(string str);  //��Ϣ���
	void detectingInstruct(string str);  //�����������Ϣ������ָ��
	bool detectingServer(string str);  //�ͻ��˼����Ϣ������ָ��������¼
	void freshRecStrList();  //ˢ�£�����RecStrList����
	string getTime();  //��ȡϵͳʱ��
};


#endif