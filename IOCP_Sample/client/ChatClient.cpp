#include <iostream>
#include <string>
#include <ws2tcpip.h>
#include <WinSock2.h>
#include <thread>

using namespace std;

#pragma comment(lib, "Ws2_32.lib")

#define CHAT_HEIGHT 20


enum CHAT_PACKET
{
	LOGIN,
	CHAT,
};

#pragma pack(push, 1)
struct BasePacket
{

	unsigned short packet_len;
	unsigned short packet_id;
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

struct testPacket : public BasePacket
{
	testPacket()
	{
		packet_id = CHAT;
		packet_len = sizeof(testPacket);
	}

	char a;
	short b;
};
#pragma pack(pop)

void gotoxy(int x, int y) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(hConsole, pos);
}

void ClearConsole(int x, int y)
{
	// 콘솔 핸들 가져오기
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// 좌표 설정
	COORD coord;
	coord.X = x;
	coord.Y = y;

	// 콘솔 버퍼에 문자열 쓰기
	DWORD written;

	FillConsoleOutputCharacter(hConsole, ' ', 100, coord, &written);
}

void PrintToConsole(int x, int y, const char* str) {
	// 콘솔 핸들 가져오기
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// 좌표 설정
	COORD coord;
	coord.X = x;
	coord.Y = y;

	// 문자열의 길이 계산
	DWORD length = strlen(str);

	// 콘솔 버퍼에 문자열 쓰기
	DWORD written;
	FillConsoleOutputCharacter(hConsole, ' ', 100, coord, &written);
	WriteConsoleOutputCharacterA(hConsole, str, length, coord, &written);
}

void recvThread(SOCKET mySocket)
{
	char buffer[512] = "";
	int renderCnt = 1;
	while (true)
	{
		int recvBytes = recv(mySocket, buffer, sizeof(buffer), 0);

		if (recvBytes <= 0)
		{
			return;
		}
		BasePacket header;
		memcpy_s(&header, sizeof(BasePacket), buffer, sizeof(BasePacket));

		if (header.packet_id == CHAT_PACKET::LOGIN)
		{
			LoginPacket login;
			memcpy_s(&login, sizeof(LoginPacket), buffer, recvBytes);

			string login_text = "[" + std::string(login.name) + "]님이 로그인 하셨습니다";
			PrintToConsole(0, renderCnt, login_text.c_str());
			
		}
		else
		{
			ChatPacket chat;
			memcpy_s(&chat, sizeof(ChatPacket), buffer, recvBytes);
			string renderchat = "[" + std::string(chat.name) + "]" + chat.text;
			PrintToConsole(0, renderCnt, renderchat.c_str());
		}

		renderCnt++;

		if (renderCnt > CHAT_HEIGHT)
		{
			renderCnt = 1;
		}
	}
}

int main()
{
	testPacket t; 
	LoginPacket login_123packet;
	ChatPacket cha123t;

	//	WSAStartup	
	WSADATA wsa;
	int		result;

	result = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (result != 0)
	{
		cout << "WSAStartup 에러" << endl;
		return -1;
	}

	//	socket
	SOCKET mySocket = socket(AF_INET, SOCK_STREAM, 0);

	if (mySocket == INVALID_SOCKET)
	{
		cerr << "socket 생성 에러" << endl;
		WSACleanup();

		return -1;
	}

	// 수신자의 주소 설정
	sockaddr_in     receiverAddr = {};
	receiverAddr.sin_family = AF_INET;					//	수신 프로토콜
	receiverAddr.sin_port = htons(12345);				//	수신자의 포트 번호
	inet_pton(AF_INET, "127.0.0.1", &receiverAddr.sin_addr);

	//	연결 시도
	result = connect(mySocket, (sockaddr*)&receiverAddr, sizeof(receiverAddr));

	if (result == SOCKET_ERROR)
	{
		cerr << "연결 에러" << endl;
		closesocket(mySocket);
		WSACleanup();

		return -1;
	}


	cout << "서버에 연결됨" << endl;

	LoginPacket login_packet;

	cout << "닉네임 : ";
	cin >> login_packet.name;

	// 로그인 패킷 전송
	send(mySocket,(char*)(&login_packet), login_packet.packet_len, 0);


	thread th(recvThread, mySocket);

	ChatPacket chat;
	while (true)
	{
		gotoxy(0, CHAT_HEIGHT + 1);
		ClearConsole(0, CHAT_HEIGHT + 1);
		cout << "입력 : ";
		cin >> chat.text;

		if (strcmp(chat.text,"exit") == 0)
		{
			break;
		}

		send(mySocket, (char*)&chat, chat.packet_len, 0);
	}

	//	recv send
	closesocket(mySocket);
	WSACleanup();
	return 1;
}