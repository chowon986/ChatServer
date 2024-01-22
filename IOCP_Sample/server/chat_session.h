#pragma once
#include <string>
#include "tcp_session.h"

class ChatSession : public TcpSession
{
public:
	ChatSession() {}
	~ChatSession() {}

public:
	inline std::string GetName()
	{
		return name_;
	}

	inline void SetName(char* name)
	{
		name_ = name;
	}


private:
	std::string name_;
};

