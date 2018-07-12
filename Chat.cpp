#include "stdafx.h"

#include <winsock2.h>
#include <Windows.h>  //���������ļ�����˳��ߵ��ᱨ��
#include "Chat.h"

//////////////////////////////////////
//����������
//////////////////////////////////////
//pair<int, int> Chat::ChatOutPos = pair<int, int>(0, 13); //�С���
//pair<int, int> Chat::SendOutPos = pair<int, int>(1, 0);
//pair<int, int> Chat::MemOutPos = pair<int, int>(0, 2);
DB* Chat::db = NULL;
SocketList* Chat::Room = NULL;
int MaxPeople = 2;  //ÿ�������������
const string ServerIp = "127.0.0.1";
SSGo myGo;  //���ʹ��ָ�롢new��ʽ�������󣬺������system�����ᱨ��

///////////////////////////////////////
//���졢��������
///////////////////////////////////////
Chat::Chat() :RecSocket(NULL)
{
	BName = "";
	WName = "";
	nPeople = 0;
	side = 0;
	isStart = false;
	isTyping = false;
	alreadyChat = false;
	StartSocketLib;
}

Chat::~Chat() 
{
	CloseSocketLib; 
	if (isServer)
	{
		delete db;
	}
}

///////////////////////////////////////
//��ҳ�˵�
///////////////////////////////////////
int Chat::Menu()
{
	cout << " 1: Creat a Six Sided Go game room." << endl;
	cout << " 2: Enter a Six Sided Go game room." << endl;
	cout << " 3: Create your name and password." << endl;
	cout << " 4: I am Game Server." << endl;
	cout << " Chose what you want to do: ";
	while (true)
	{
		int select = 0;
		cin >> select;
		if (select == 1)
		{
			CreateRoom(false);
			break;
		}
		else if (select == 2)
		{
			EnterRoom(false);
			break;
		}
		else if (select == 3)
		{
			createAcc();
			break;
		}
		else if (select == 4)
		{
			CreateServer();
			break;
		}
		else
		{
			cout << "Error number! Please enter a new number: ";
		}
	}
	return 0;
}

///////////////////////////////////////
//��¼��Ĳ˵�
///////////////////////////////////////
int Chat::Menu2()
{
	system("cls");
	side = 0;
	nPeople = 0;
	isStart = false;
	isTyping = false;
	BName = "";
	WName = "";
	cout << " 1: Creat a Six Sided Go game room." << endl;
	cout << " 2: Enter a Six Sided Go game room." << endl;
	//cout << " 3: Check the information of my account." << endl;
	cout << " Chose what you want to do: ";
	while (true)
	{
		int select = 0;
		cin >> select;
		if (select == 1)
		{
			CreateRoom(true);
			break;
		}
		else if (select == 2)
		{
			EnterRoom(true);
			break;
		}
		/*else if (select == 3)
		{
			��ʾ�˺���Ϣ
			break;
		}*/
		else
		{
			cout << "Error number! Please enter a new number: ";
		}
	}
	return 0;
}

///////////////////////////////////////
//�����˺�����
///////////////////////////////////////
void Chat::createAcc()
{
	isServer = false;
	string name, passwd;
	while (true)
	{
		cout << endl << " What's your name? ";
		cin >> name;

		//���ȼ��
		if (name.length() > 14)
			cout << " Password can't be longer than 14!" << endl;
		else
			break;
	}
	while (true)
	{
		cout << endl << " What's your password? ";
		cin >> passwd;
		if (passwd == "???")
			cout << " Password can't be\"???\"!" << endl;
		else if (passwd.length() > 30)
			cout << " Password can't be longer than 30!" << endl;
		else
			break;
	}

	//�����˺����룬Ȼ��ص����˵�
	Room = new SocketList(false, ServerIp);
	string *str = new string;
	*str = "#CreName#" + name + "#CrePswd#" + passwd + "#end#";
	Room->Send(*str);
	delete str;
	Sleep(1000);
	detectingMsg();
	Menu();
}

///////////////////////////////////////
//��¼
///////////////////////////////////////
bool Chat::login()
{
	int t = 0;
	while (t < 5)
	{
		string name, passwd;
		cout << endl << " What's your name? ";
		cin >> name;
		cout << endl << " What's your password? ";
		cin >> passwd;
		if (passwd == "???")//���벻�����ó�??? ��Ϊ���ǲ�ѯ����ʧ�ܵķ���ֵ�����Ϊ��ȫ©����
		{
			cout << " Password can't be\"???\"!" << endl;
			break;
		}
		if (isServer)
		{
			if (name != "GameServer")
			{
				cout << " Your name is not server manager's name!" << endl;
			}
			else
			{
				db = new DB;
				if (passwd == db->passwdOf(name))
				{
					myName = name;
					return true;
				}
				else
					cout << " Name or password error!" << endl;
			}
		}
		else  //�ͻ���
		{
			//�����˺����룬�����¼
			string *str = new string;
			*str = "#ConName#" + name + "#ConPswd#" + passwd + "#end#";
			Room->Send(*str);
			delete str;
			Sleep(1000);
			detectingMsg();
			if (name==BName || name == WName)
			{
				myName = name;
				return true;
			}
		}
		t++;
	}
	return false;
}

///////////////////////////////////////
//����������
///////////////////////////////////////
void Chat::CreateServer()
{
	isServer = true;
	if (login())
	{
		Room = new SocketList(true);
		Room->myChat = this;
		cout << endl << " Server is created!" << endl;
		Fresh();
	}
}

///////////////////////////////////////
//�������䣨�ڣ�
///////////////////////////////////////
void Chat::CreateRoom(bool hasLogin)
{
	isServer = false;
	if(hasLogin)
	{
		Room->Send("#ConName#" + myName + "#ConPswd#???#end#");
		Sleep(500);
		detectingMsg();
		cout << endl << " Wait for another person come in..." << endl;
	}
	else 
	{
		Room = new SocketList(false, ServerIp);

		if (login())
		{
			cout << endl << " Wait for another person come in..." << endl;
			Fresh();
		}
		else
		{
			system("pause");
		}
	}
}

///////////////////////////////////////
//���뷿�䣨�ף�
///////////////////////////////////////
void Chat::EnterRoom(bool hasLogin)
{
	isServer = false;
	if (hasLogin)
	{
		Room->Send("#ConName#" + myName + "#ConPswd#???#end#");
		Sleep(500);
		detectingMsg();
		cout << endl << " Wait for the Black to input SIDE of chessboard..." << endl;
	}
	else
	{
		Room = new SocketList(false, ServerIp);
		if (login())
		{
			cout << endl << " Wait for the Black to input SIDE of chessboard..." << endl;
			Fresh();
		}
		else
		{
			system("pause");
		}
	}
}

////////////////////////////////////////
//ˢ�£���ȡ��Ϣ������
////////////////////////////////////////
void Chat::Fresh()
{
	string *str = new string;
	bool bGoThread = false;
	bool cinThread = false;
	while (true)
	{
		detectingMsg();
		//���ڴ�������
		if (isTyping && !cinThread)
		{
			cinThread = true;
			CinThread = ThreadLib::Create(CinString, (void*)str);
		}
		if (*str != "")
		{
			*str = "##" + myName + "(" + db->getTime() + "): "+ *str + "#end#";
			RecStrList.push_back(*str);
			freshRecStrList();
			Room->Send(*str);
			*str = "";
			cinThread = false;
		}
		//�����û������ʼ������̣߳�����������myNameֵ���������������̣߳�
		if (!bGoThread)
		{
			if (isStart)
			{
				bGoThread = true;
				ThreadLib::ThreadID GoThread;
				if (myName == BName)
					GoThread = ThreadLib::Create(playGoB);
				else if (myName == WName)
					GoThread = ThreadLib::Create(playGoW);
			}
		}
		//������߳��Ѿ��򿪹���������Ѿ�����
		else if(!isStart && !isServer)
		{
			bGoThread = false;
			Menu2();
		}
		Sleep(100);
	}
	delete str;
}

///////////////////////////////////////
//��ʼ����
///////////////////////////////////////
void Chat::playGoB(void *Receive)
{
	myGo.start(true, true);
}

void Chat::playGoW(void *Receive)
{
	myGo.start(false, true);
}

///////////////////////////////////////
//�������칦��
///////////////////////////////////////
void Chat::CinString(void *Receive)
{
	string *Result = static_cast<string*>(Receive); //static_cast��ָ��Receiveת����string���͵�ָ��
	*Result = "";
	cin >> *Result;
}

////////////////////////////////////////
//��Ϣ���
////////////////////////////////////////
vector<string> Chat::SeparateMsg(string str)
{
	vector<string> retStrVec;
	int pos;
	while ((pos = str.find("#end#")) != string::npos)
	{
		retStrVec.push_back(str.substr(0, pos));
		str.erase(0, pos + 5);
	}
	if (str != "")
		retStrVec.push_back(str);
	return retStrVec;
}

////////////////////////////////////////
//������������ִ��ָ��
////////////////////////////////////////
void Chat::detectingInstruct(string str)
{
	if (str == "#Connected#")
	{
		if (isServer && RecSocket != NULL)
		{
			if (nPeople >= MaxPeople)
			{
				RecSocket->SendData("#RoomFull##end#");
			}
		}
		////��������ǹ�ս��
		//string newName = "select from mysql";
		//NameInRoom.push_back(newName);
		//ShowName();
		//Room->Send("#ClearNameInRoom##end#");
		//for (vector<string>::iterator itr = NameInRoom.begin();
		//itr != NameInRoom.end();itr++)
		//Room->Send("#ShowName#" + *itr + "#end#");
	}
	else if (str.substr(0, 9) == "#CreName#") //��0��ʼ������Ϊ9
	{
		int len = str.length();
		int i;
		for (i = 9;i < len;i++)
			if (str.substr(i, 9) == "#CrePswd#")
				break;
		string name = str.substr(9, i - 9);
		string pswd = str.substr(i + 9, len);
		//�������
		if (db->hasPeople(name))
			RecSocket->SendData("#IDex##end#");
		else
		{
			db->CreAcc(name, pswd);
		}
	}
	else if (str.substr(0, 9) == "#ConName#")
	{
		int len = str.length();
		int i;
		for (i = 9;i < len;i++)
			if (str.substr(i, 9) == "#ConPswd#")
				break;
		string name = str.substr(9, i - 9);
		string pswd = str.substr(i + 9, len);

		//�����Ǵ���������ˣ�ִ��
		if (nPeople == 0)
		{
			if (pswd == db->passwdOf(name) || pswd == "???")
			{
				nPeople++;  //����ͻ������ߣ���������nPeople���1
				BName = name;
				string str = "#ShowNameB#" + name + "#end#";
				Room->Send(str);
			}
			else
			{
				RecSocket->SendData("#NoID##end#");
			}
		}
		//�ڶ������������ִ��
		else if (nPeople == 1)
		{
			if (name == BName)  //ͬһ�˺��ظ���¼
			{
				RecSocket->SendData("#NoID##end#");
			}
			if (pswd == db->passwdOf(name) || pswd == "???")
			{
				nPeople++;
				WName = name;
				string str = "#ShowNameW#" + name + "#end#";
				Room->Send(str);
				str = "#ShowNameB#" + BName + "#end#";
				RecSocket->SendData(str);
			}
			else
			{
				RecSocket->SendData("#NoID##end#");
			}
		}
	}
	else if (str.substr(0, 12) == "#CreateSide#")
	{
		string side_s = str.substr(12, str.length());
		int side_i = atoi(side_s.c_str());
		if (side != side_i)
		{
			Room->Send("#ConfirmSide#" + side_s + "#end#");
			side = side_i;
		}
		else
		{
			//��ʾ˫��ͬ���˿�ʼ�Ծ�
			Room->Send("#ConfirmSide#0#end#");
			isStart = true;
		}
	}
	else if (str.substr(0, 13) == "#ConfirmSide#")
	{
		string side_s = str.substr(13, str.length());
		if (side == atoi(side_s.c_str()))
		{
			//��ʾ˫��ͬ���˿�ʼ�Ծ�
			Room->Send("#ConfirmSide#0#end#");
			isStart = true;
			db->CreateNewGame(BName,WName,side);
		}
	}
	else if (str.substr(0, 3) == "#I#" && isStart)
	{
		int len = str.length();
		int i;
		for (i = 3;i < len;i++)
			if (str.substr(i, 3) == "#J#")
				break;
		string i_s = str.substr(3, i - 3);
		string j_s = str.substr(i + 3, len);
		int ii= atoi(i_s.c_str());
		int j = atoi(j_s.c_str());
		//�������Ӯ��Ϣ������¼����
		if (ii == 0 && j == 1) //��������
		{
			db->AddResult(WName,0);
			nPeople = 0;
			isStart = false;
			isTyping = false;
		}
		else if (ii == 0 && j == 2)  //��������
		{
			db->AddResult(BName,0);
			nPeople = 0;
			isStart = false;
			isTyping = false;
		}
		//���ݿⴴ������
		else
		    db->AddRecord(ii, j);
	}
	else if (str.substr(0, 3) == "#R#" && isStart)
	{
		int len = str.length();
		int num;
		string winner= str.substr(3, 1);
		if (winner == "B")
			winner = BName;
		else if(winner == "W")
			winner = WName;
		if (len == 4)
			num = 0;
		else
		{
			string s = str.substr(4, len);
			num = atoi(s.c_str());
		}
		db->AddResult(winner, num);
		nPeople = 0;
		isStart = false;
		isTyping = false;
	}
	else if (str.substr(0, 2) == "##")
	{
		string s = str.substr(2, str.length());
		db->AddChat(s);
	}
	/*else if (str == "#GetNameB")
	{
	RecSocket->SendData("#ShowNameB##end#");
	}*/
}

////////////////////////////////////////
//�ͻ��˼�����ִ��ָ��
////////////////////////////////////////
bool Chat::detectingServer(string str)
{
	if (str == "#NoID#")
	{
		MessageBox(NULL, L"�˺�/������󣬻��ظ���¼", L"��ʾ", 0);
		return true;
	}
	else if (str == "#IDex#")
	{
		MessageBox(NULL, L"���˺��Ѿ�����", L"��ʾ", 0);
		return true;
	}
	else if (str == "#RoomFull#")
	{
		MessageBox(NULL, L"��������", L"��ʾ", 0);
		//exit(0);
		return true;
	}
	else if (str.substr(0, 13) == "#ConfirmSide#")
	{
		//����Ѿ���ʼ�Ծ֣�ָ����Ч
		if (isStart)
			return true;
		string side_s = str.substr(13, str.length());
		int side_i = atoi(side_s.c_str());
		//�յ���ʼ�Ծֵ�ָ��
		if (side_i == 0)
		{
			isStart = true;
			myGo = SSGo(' ', side, this);
			myGo.drawBG(side);
			myGo.drawBName(BName);
			myGo.drawWName(WName);
		}
		//����յ��޸�sideֵ����
		else if (side != side_i)
		{
			cout << endl << " Confirm to play a " << side_s << "-size Six Sided Go? (Y/N)";
			char ch;
			while (true)
			{
				cin >> ch;
				if (ch == 'Y' || ch == 'y')
				{
					side = side_i;
					Room->Send("#ConfirmSide#" + side_s + "#end#");
					break;
				}
				else if (ch == 'N' || ch == 'n')
				{
					cout << "Enter the SIDE you want (if not suitable, it will be 13): ";
					cin >> side;
					Room->Send("#CreateSide#" + to_string(side) + "#end#");
					break;
				}
				cout << " Please enter Y or N !" << endl;
			}
		}
		return true;
	}
	else if (str.substr(0, 3) == "#I#")
	{
		int len = str.length();
		int ii;
		for (ii = 3;ii < len;ii++)
			if (str.substr(ii, 3) == "#J#")
				break;
		string i_s = str.substr(3, ii - 3);
		string j_s = str.substr(ii + 3, len);
		myGo.enemyI = atoi(i_s.c_str());
		myGo.enemyJ = atoi(j_s.c_str());

		return true;
	}
	else if (str.substr(0, 7) == "#JieBI#")
	{
		int len = str.length();
		int ii;
		for (ii = 7;ii < len;ii++)
			if (str.substr(ii, 3) == "#J#")
				break;
		string i_s = str.substr(7, ii - 7);
		string j_s = str.substr(ii + 3, len);
		myGo.posJieB = pair<int, int>(atoi(i_s.c_str()), atoi(j_s.c_str()));

		return true;
	}
	else if (str.substr(0, 7) == "#JieWI#")
	{
		int len = str.length();
		int ii;
		for (ii = 7;ii < len;ii++)
			if (str.substr(ii, 3) == "#J#")
				break;
		string i_s = str.substr(7, ii - 7);
		string j_s = str.substr(ii + 3, len);
		myGo.posJieW = pair<int, int>(atoi(i_s.c_str()), atoi(j_s.c_str()));

		return true;
	}
	else if (str == "#ClearNameInRoom#")
	{
		BName = "";
		WName = "";
		if (isStart)
		{
			myGo.drawBName(BName);
			myGo.drawWName(WName);
		}
		return true;
	}
	else if (str.substr(0, 11) == "#ShowNameB#")
	{
		if (isStart)
			myGo.drawBName(BName);
		else
		{
			BName = str.substr(11, str.length());
			nPeople++;
		}
		return true;
	}
	else if (str.substr(0, 11) == "#ShowNameW#")
	{
		if (isStart)
			myGo.drawWName(WName);
		else
		{
			WName = str.substr(11, str.length());
			nPeople++;
			//����ͻ����Ǻڷ�
			if (myName == BName && myName != "")
			{
				//��ʼѡ�����̴�С
				string *str = new string;
				cout << endl << " A person just come in. Enter the SIDE (if not suitable, it will be 13): ";
				cin >> side;
				if (side > 13 || side < 3)
				{
					side = 13;
				}
				*str = "#CreateSide#" + to_string(side) + "#end#";
				Room->Send(*str);
				delete(str);
			}
		}
		return true;
	}
	else if (str.substr(0, 2) == "##")
	{
		string s = str.substr(2, str.length());
		//������Ϣ�����Լ���Ҫ�����ظ����ͳһ��Ϣ
		if (myName == str.substr(2, myName.length()))
		{
			if (!alreadyChat)
			{
				myGo.drawChatText(s);
				alreadyChat = true;
			}
			else
				alreadyChat = false;
		}
		else
			myGo.drawChatText(s);
	}
	/*else if (str == "#GetNameB")
	{
	RecSocket->SendData("#ShowNameB##end#");
	}*/
	return false;
}

////////////////////////////////////////
//ˢ�£�����RecStrList����
////////////////////////////////////////
void Chat::freshRecStrList()
{
	for (list<string>::iterator itr = RecStrList.begin();itr != RecStrList.end();itr++)
	{
		vector<string> msgVector = SeparateMsg(*itr);
		for (vector<string>::iterator it = msgVector.begin();it != msgVector.end();it++)
		{
			if (isServer)
			{
				detectingInstruct(*it);
				cout << *it << endl;
			}
			else
			{
				detectingServer(*it);
			}
		}
	}

	//SetOutPos(SendOutPos);
}

////////////////////////////////////////
//������Ϣ
////////////////////////////////////////
void Chat::detectingMsg()
{
	RecStrList = Room->Listening(&RecSocket);
	if (RecStrList.size() != 0)
	{
		freshRecStrList();
	}
}

////////////////////////////////////////
//��ȡʱ��
////////////////////////////////////////
string Chat::getTime()
{
	Clock timeClock;
	return timeClock.getTime();
}
