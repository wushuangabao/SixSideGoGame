#ifndef SixSideGo_H
#define SixSideGo_H

#include <winsock2.h>
#include <Windows.h>  //���������ļ�����˳��ߵ��ᱨ��
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

	Chat* myChat; //����ʱ��ͨ�Žӿ�
	int enemyI;  //����ʱ�Է�����λ��
	int enemyJ;  //����ʱ�Է�����λ��
	int myI;  //����ʱ��������λ��
	int myJ;  //����ʱ��������λ��
	bool turn;  //����ʱ�ֵ������һ����true��ʾ�ڷ�

	bool isBlack;  //�Ƿ�ִ��
	vector<pair<int, int>> block;  //��¼һ�����������
	vector<pair<int, int>> points;  //��¼�����ӵ��λ��
	pair<int, int> posJieB;  //����к����ֹ�µ�λ��
	pair<int, int> posJieW;  //����а����ֹ�µ�λ��
	pair<int, int> resPos;  //���ʤ�������༭�������ֵ�λ��
	int count;  //�ܹ������ӵ���Ŀ
	int countB;  //����Ŀ��
	int countW;  //����Ŀ��
	int Side;  //�����εı߳�����С�����εı߳�Ϊ��λ1��
	int MaxLine;  //�������
	int MaxColumn;  //�������
	char record[108][55];  //���̼�¼���±��1��ʼ�������ڱ���ʱ��Ҫȷ����С��ֻ��д��������С����Side���ֵ13(106,53)������

	bool canDown(pair<int, int> pos, bool isBlack);  //�ж��Ƿ��������
	bool isLive(pair<int, int> pos, bool isBlack);  //�ж��Ƿ�Ϊ����
	bool notInBlock(pair<int, int>);  //�ж��Ƿ�û�б�ǣ�û�д���block�
	void start(bool isBlack, bool isOnline);  //��ʼ����
	void drawBG(int Side);  //������
	void drawBName(string name);//��ʾ������������
	void drawWName(string name);//��ʾ������������
	void drawChatText(string s);//��ʾ��������
	void clearDied();  //�������
	int callMenu(pair<int, int> pos);  //��ʾ�˵�
	int pauseHand();  //ͣһ�� ����1��ʾ�Ծֽ���
	void admitDefeat(bool isBlack);  //����
	int countNumB(pair<int, int>);  //��������һ��λ�ã����������Ŀ��
	int blackWin();  //�ж��Ƿ����Ӯ 0=lose 1=win 2=����
	void putOutResult(int winOrLose);  //�����Ŀ����Ӯ���
	pair<int, int> Chose(pair<int, int> pos);  //��pos��ʼѡ�㣬��ȡ���̲���
	void putChess(pair<int, int> pos, bool isBlack);  //����
	BOOL SetConsoleColor(WORD wAttributes);  //�����������/������ɫ
	void SetFont(pair<int, int> pos);  //��������

private:
	int chatLine;  //ʣ��ɹ����������Ϣ������
	int maxChatLine;  //�ɹ����������Ϣ���������ֵ

	pair<int, int> moveA(pair<int, int> pos);
	pair<int, int> moveD(pair<int, int> pos);
	pair<int, int> moveW(pair<int, int> pos);
	pair<int, int> moveS(pair<int, int> pos);
	pair<int, int> moveWD(pair<int, int> pos);
	pair<int, int> moveSD(pair<int, int> pos);
	static void SetOutPos(HANDLE hStd,pair<int, int> pos);  //�������λ��
	pair<int, int> posDrawChat();  //��ȡ��������¼��λ��
};

#endif