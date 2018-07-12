#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "Platform.h"
#include <string>
#include <Windows.h>


class BaseSocket
{
	friend class SocketList;
public:
	virtual ~BaseSocket();
	int GetSocket();
protected:
	int sock;
	const int port;
	struct sockaddr_in socketaddress;
	BaseSocket(int PassSocket = -1);
private:
	void CreateSock();
	void CloseSock();
};

class DataSocket : public BaseSocket
{
public:
	DataSocket(int PassSocket = -1);
	void RequestConnect(std::string address);//��������
	int SendData(std::string str);//��������
	std::string ReceiveData();//��������
};

class ListenSocket : public BaseSocket
{
public:
	ListenSocket();
	DataSocket* AcceptConnect();//��������
private:
	void BindSocket();//��Socket
	void Listening();//����������������
};

#endif
