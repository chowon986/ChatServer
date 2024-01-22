#include "chat_server.h"
#include <iostream>
#include "chat_session.h"
#include "packet_handler.h"

enum CHAT_PACKET
{
	LOGIN,
	CHAT,
	DICE,
};

struct LoginPacket : public BasePacket
{
	LoginPacket()
	{
		packet_id = LOGIN;
		packet_len = sizeof(LoginPacket);
	}
	char name[20];
};

struct ChatPacket : public BasePacket
{
	ChatPacket()
	{
		packet_id = CHAT;
		packet_len = sizeof(ChatPacket);
	}

	char name[20];
	char text[120];
};

struct DicePacket : public BasePacket
{
	DicePacket()
	{
		packet_id = DICE;
		packet_len = sizeof(ChatPacket);
	}
	char name[20];
};

ChatServer::ChatServer()
{
}

ChatServer::~ChatServer()
{

}

void ChatServer::Init()
{
	REGIST_PACKET(packek_handler_, CHAT_PACKET::LOGIN, ChatServer::Login);
	// packek_handler_->AddPacket(CHAT_PACKET::LOGIN, std::bind(&ChatServer::Login, this, std::placeholders::_1, std::placeholders::_2));
	REGIST_PACKET(packek_handler_, CHAT_PACKET::CHAT, ChatServer::Chat);
	REGIST_PACKET(packek_handler_, CHAT_PACKET::DICE, ChatServer::Dice);
}

void ChatServer::Login(TcpSession* session, BasePacket* packet)
{
	ChatSession* chat_session = reinterpret_cast<ChatSession*>(session);;
	LoginPacket* login_packet = reinterpret_cast<LoginPacket*>(packet);
	std::cout << "로그인 : " << login_packet->name << std::endl;

	chat_session->SetName(login_packet->name);

	// 로그인 알림
	for (auto iter = session_map_.begin(); iter != session_map_.end(); iter++)
	{
		ChatSession* other = reinterpret_cast<ChatSession*>(iter->second);

		other->CallSend(login_packet, sizeof(LoginPacket));
	}
}

void ChatServer::Chat(TcpSession* session, BasePacket* packet)
{
	ChatSession* chat_session = reinterpret_cast<ChatSession*>(session);;
	ChatPacket* chat_packet = reinterpret_cast<ChatPacket*>(packet);

	std::string sender_name = chat_session->GetName();
	std::string chat = chat_packet->text;

	ChatPacket response;

	memcpy_s(response.name, 20, sender_name.c_str(), sender_name.length()+1);
	memcpy_s(response.text, 120, chat.c_str(), chat.length()+1);

	// 전체 챗 공유
	for (auto iter = session_map_.begin(); iter != session_map_.end(); iter++)
	{
		ChatSession* other = reinterpret_cast<ChatSession*>(iter->second);

		other->CallSend(&response, sizeof(ChatPacket));
	}
}

void ChatServer::Dice(TcpSession* session, BasePacket* packet)
{
}