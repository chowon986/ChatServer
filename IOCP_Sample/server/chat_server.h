#pragma once
#include "iocp_server.h"
#include "chat_session.h"

struct BasePacket;

class ChatServer: public IocpServer
{
public:
	ChatServer();
	~ChatServer();

public:
	virtual void Init();

public:
	virtual TcpSession* CreateSession()
	{
		return new ChatSession;
	}

private:
	void Login(TcpSession* session, BasePacket* packet);
	void Chat(TcpSession* session, BasePacket* packet);
	void Dice(TcpSession* session, BasePacket* packet);
};

