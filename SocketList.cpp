#include "stdafx.h"

#include "SocketList.h"


SocketList::SocketList(bool IsServer, string ConnectIp) :isServer(IsServer)
{
	zerotime.tv_usec = 0;
	zerotime.tv_sec = 0;
	if (isServer)
		ConnectSocket = new ListenSocket;
	else
	{
		ConnectSocket = new DataSocket;
		DataSocket* dataSocket = dynamic_cast<DataSocket*> (ConnectSocket);
		dataSocket->RequestConnect(ConnectIp);
	}
}

SocketList::~SocketList()
{
	delete ConnectSocket;
	for (list<DataSocket*>::iterator itr = SerDatSockList.begin();itr != SerDatSockList.end(); itr++)
	{
		delete *itr;
	}
}

void SocketList::Send(string str)
{
	if (isServer)
		SendToAllClient(str);
	else
		SendToServer(str);
}

list<string> SocketList::Listening(DataSocket **RecSocket)
{
	list<string> retStrList;
	// 初始化rset为NULL
	FD_ZERO(&rset);
	// 添加连接所用套接字
	FD_SET(ConnectSocket->GetSocket(), &rset);
	if (isServer)
	{
		// 添加所有数据套接字链表
		for (list<DataSocket*>::iterator itr = SerDatSockList.begin();itr != SerDatSockList.end(); itr++)
		{
			FD_SET((*itr)->GetSocket(), &rset);
		}
	}
	// 查找在rset中是否有活动的套接字
	int i = select(0x7FFFFFFF, &rset, NULL, NULL, &zerotime);
	if (i > 0)
	{
		if (!isServer)  //客户机端
		{
			DataSocket* dataSocket = dynamic_cast<DataSocket*> (ConnectSocket);
			if (FD_ISSET(dataSocket->GetSocket(), &rset)) //接收到服务器发来的信息
			{
				string mes = dataSocket->ReceiveData();
				if (mes == "SocketClosed")
				{
					MessageBox(NULL, L"服务器已关闭", L"提示", 0);
					exit(0);
				}
				retStrList.push_back(mes);
				*RecSocket = dataSocket;
			}
		}
		else if (isServer) //服务器端
		{
			ListenSocket* listenSocket = dynamic_cast<ListenSocket*> (ConnectSocket);
			if (FD_ISSET(listenSocket->GetSocket(), &rset)) //监听Socket受到连接请求
			{
				// 接收连接
				DataSocket *dsock = listenSocket->AcceptConnect();

				// 添加接收的数据套接字到链表
				SerDatSockList.push_back(dsock);
				*RecSocket = dsock;

				retStrList.push_back("#Connected##end#");
			}
			// 遍历每个socket检查是否有活动
			for (list<DataSocket*>::iterator itr = SerDatSockList.begin();itr != SerDatSockList.end(); itr++)
			{
				if (FD_ISSET((*itr)->GetSocket(), &rset))
				{
					string mes = (*itr)->ReceiveData();
					if (mes == "SocketClosed")
					{
						myChat->nPeople--;
						delete (*itr);
						itr = SerDatSockList.erase(itr);
						if (itr == SerDatSockList.end())
							break;
						continue;
					}
					else
					{
						retStrList.push_back(mes);
						*RecSocket = *itr;
					}
				}
			}
			if (retStrList.size() != 0)
			{
				SendToAllClient(retStrList);
			}
		}
	}

	return retStrList;
}

void SocketList::SendToAllClient(list<string> StrList)
{
	for (list<string>::iterator sItr = StrList.begin(); sItr != StrList.end(); sItr++)
	{
		SendToAllClient(*sItr);
	}
}

void SocketList::SendToAllClient(string str)
{
	for (list<DataSocket*>::iterator itr = SerDatSockList.begin();
		itr != SerDatSockList.end(); itr++)
	{
		(*itr)->SendData(str);
	}
}

void SocketList::SendToServer(string str)
{
	DataSocket *dataSocket = dynamic_cast<DataSocket *>(ConnectSocket);
	dataSocket->SendData(str);
}