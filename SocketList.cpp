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
	// ��ʼ��rsetΪNULL
	FD_ZERO(&rset);
	// ������������׽���
	FD_SET(ConnectSocket->GetSocket(), &rset);
	if (isServer)
	{
		// ������������׽�������
		for (list<DataSocket*>::iterator itr = SerDatSockList.begin();itr != SerDatSockList.end(); itr++)
		{
			FD_SET((*itr)->GetSocket(), &rset);
		}
	}
	// ������rset���Ƿ��л���׽���
	int i = select(0x7FFFFFFF, &rset, NULL, NULL, &zerotime);
	if (i > 0)
	{
		if (!isServer)  //�ͻ�����
		{
			DataSocket* dataSocket = dynamic_cast<DataSocket*> (ConnectSocket);
			if (FD_ISSET(dataSocket->GetSocket(), &rset)) //���յ���������������Ϣ
			{
				string mes = dataSocket->ReceiveData();
				if (mes == "SocketClosed")
				{
					MessageBox(NULL, L"�������ѹر�", L"��ʾ", 0);
					exit(0);
				}
				retStrList.push_back(mes);
				*RecSocket = dataSocket;
			}
		}
		else if (isServer) //��������
		{
			ListenSocket* listenSocket = dynamic_cast<ListenSocket*> (ConnectSocket);
			if (FD_ISSET(listenSocket->GetSocket(), &rset)) //����Socket�ܵ���������
			{
				// ��������
				DataSocket *dsock = listenSocket->AcceptConnect();

				// ��ӽ��յ������׽��ֵ�����
				SerDatSockList.push_back(dsock);
				*RecSocket = dsock;

				retStrList.push_back("#Connected##end#");
			}
			// ����ÿ��socket����Ƿ��л
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