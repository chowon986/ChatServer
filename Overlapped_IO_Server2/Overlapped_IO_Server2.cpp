#include <iostream>
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <set>
using namespace std;
#pragma comment (lib, "Ws2_32.lib") // cpp

/*
*	기본 자료형
	int, float, bool,
*/
/*
	사용자정의 자료형

	struct 구조체명
	{
		자료형 멤버변수명;
		자료형 멤버변수명2;
	};

	int num;

	구조체명 변수명;

	변수명.멤버변수명 = 값;
*/
struct ClientInfo
{
	WSAOVERLAPPED	overlapped;

	SOCKET			socket;
	char			buffer[512];
	WSABUF			dataBuf;
	DWORD			recvByte;
	DWORD			flags;
	BOOL			isSend;
};

/*
	자료구조

		구현이 쉽다.
		임의접근에 유리하다.
		삽입삭제에 유리하다.

		다 준수하면서 검색(탐색)에 능하다.
		map, set


		탐색,  검색

*/

set<ClientInfo*>	g_clientList;


void CALLBACK RecvCallBackFunc(DWORD _error, DWORD _recvLen, LPWSAOVERLAPPED _overlapped, DWORD _inflags)
{
	ClientInfo* info = (ClientInfo*)_overlapped;

	if (_recvLen == 0)
	{
		cout << "클라 연결 종료" << endl;
		closesocket(info->socket);

		g_clientList.erase(info);

		delete info;

		return;
	}

	if (_error == 0)
	{
		if (info->isSend == false)
		{
			cout << "받았다" << endl;
			cout << "수신 메시지 : " << info->buffer << endl;

			for (auto iter = g_clientList.begin(); iter != g_clientList.end(); iter++)
			{
				ClientInfo* data = *iter;

				strcpy_s(data->dataBuf.buf, 512, info->dataBuf.buf);
				data->dataBuf.len = _recvLen;

				WSASend(
					data->socket,
					&data->dataBuf,
					1,
					nullptr,
					0,
					&data->overlapped,
					RecvCallBackFunc);

				data->isSend = true;
			}
		}
		else
		{
			info->dataBuf.len = 512;

			WSARecv(
				info->socket,
				&info->dataBuf,
				1,
				&info->recvByte,
				&info->flags,
				&info->overlapped,
				RecvCallBackFunc);

			cout << "수신대기 시작" << endl;

			info->isSend = false;
		}
	}
}

int main()
{
	//	WSAStartup
	WSADATA	wsa;
	int		result;

	result = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (result != 0)
	{
		cout << "WSAStartup 에러" << endl;
		return -1;
	}

	//	소켓 생성
	SOCKET	mySocket = WSASocket(
		AF_INET,
		SOCK_STREAM,
		0,
		nullptr,
		0,
		WSA_FLAG_OVERLAPPED);

	if (mySocket == INVALID_SOCKET)
	{
		cout << "소켓 생성 에러" << endl;
		WSACleanup();
		return -1;
	}

	//	바인딩
	sockaddr_in		recviverAddr = {};
	recviverAddr.sin_family = AF_INET;
	recviverAddr.sin_port = htons(12345);
	recviverAddr.sin_addr.s_addr = INADDR_ANY;

	result = bind(
		mySocket,
		(sockaddr*)&recviverAddr,
		sizeof(recviverAddr));

	if (result != 0)
	{
		cout << "바인딩 에러" << endl;
		closesocket(mySocket);
		WSACleanup();
		return -1;
	}

	//	리슨
	if (listen(mySocket, SOMAXCONN) == SOCKET_ERROR)
	{
		cout << "리슨 에러" << endl;
		closesocket(mySocket);
		WSACleanup();
	}

	SOCKET clientSocket;

	while (true)
	{
		cout << "연결 대기 시작" << endl;

		//	어셉
		clientSocket = accept(mySocket, nullptr, nullptr);

		if (clientSocket == INVALID_SOCKET)
		{
			cout << "어셉 에러" << endl;
			closesocket(mySocket);
			WSACleanup();
			return -1;
		}

		cout << "클라 연결 성공" << endl;

		ClientInfo* info = new ClientInfo;
		info->socket = clientSocket;
		info->dataBuf.buf = info->buffer;
		info->dataBuf.len = 512;
		info->flags = 0;
		info->recvByte = 0;
		info->isSend = false;

		//	리스트에 클라 정보 저장
		g_clientList.insert(info);

		//	웰컴 멘트 제작
		strcpy_s(info->dataBuf.buf, info->dataBuf.len, "system : welcome");
		info->dataBuf.len = strlen("system : welcome");

		//	송신
		WSASend(
			info->socket,
			&info->dataBuf,
			1,
			nullptr,
			0,
			&info->overlapped,
			RecvCallBackFunc);

		info->isSend = true;
	}

	//	해제들
	closesocket(clientSocket);
	closesocket(mySocket);
	WSACleanup();

	return 0;
}