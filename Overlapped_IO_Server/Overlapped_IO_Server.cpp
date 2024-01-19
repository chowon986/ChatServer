// Overlapped_IO_Server

#include <iostream>
#include <WS2tcpip.h>
#include <WinSock2.h>

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

struct ClientInfo
{
	WSAOVERLAPPED overlapped;

	SOCKET socket;
	char buffer[512];
	WSABUF dataBuf;
	DWORD recvByte = 0;
	DWORD flags = 0;
};

void CALLBACK RecvCallBackFunc(DWORD _error, DWORD _recvLen, LPWSAOVERLAPPED _overlapped, DWORD _inflags);

void CALLBACK SendCallBackFunc(DWORD _error, DWORD _recvLen, LPWSAOVERLAPPED _overlapped, DWORD _inflags)
{
	ClientInfo* info = (ClientInfo*)_overlapped;

	int result = WSARecv(info->socket, &info->dataBuf, 1, &info->recvByte, &info->flags, &info->overlapped, RecvCallBackFunc);

	if (result == SOCKET_ERROR)
	{
		if (WSAGetLastError() == SOCKET_ERROR )
		{
			cout << "비동처리 정상 완료" << endl;
		}
	}
}

void CALLBACK RecvCallBackFunc(DWORD _error, DWORD _recvLen, LPWSAOVERLAPPED _overlapped, DWORD _inflags)
{
	// 비동기하다가 callback이 호출된다는거임
	ClientInfo* info = (ClientInfo*)_overlapped;

	cout << "받았다" << endl;

	if (_recvLen == 0) // 클라이언트 연결이 종료되었다면
	{
		cout << "클라이언트 연결 종료" << endl;
		closesocket(info->socket);

		delete info;

		return;
	}

	if (_error == 0)
	{
		cout << "수신 내용 : " << info->buffer << endl;
		WSASend(info->socket, &info->dataBuf, 1, &info->recvByte, 0, &info->overlapped, SendCallBackFunc);
	}
}

int main()
{
	// 결과가 오면 AAA라는 함수를 나가 호출해 => CallBack

	// WSAStartup (Windows 소켓 기능을 초기화)
	WSADATA wsa;
	int result;
	
	// MAKEWORD는 WORD를 만드는 거임
	// WORD는 unsigned short임
	result = WSAStartup(MAKEWORD(2, 2), &wsa);
	
	if (result != 0) // 0이 아니면 에러임
	{
		cout << "WSAStartup 에러" << endl;
		return -1;
	}

	// Socket 생성
	// 비동기 소켓을 만들어야 해서 WSASocket()를 사용해야 함
	SOCKET mySocket = WSASocket(PF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
	// PF_INET는 AF_INET와 같음
	// SOCK_STREAM : TCP 통신
	// WSA_FLAG_OVERLAPPED : 비동기 IO로 만들겠습니다.

	if (mySocket == INVALID_SOCKET)
	{
		cout << "소켓 생성 에러" << endl;
		WSACleanup();
		return -1;
	}

	// Bind
	// 수신용 정보 입력
	// AF_INET = Address Family - Internet
	// PF_INET = Protocol Family - Internet
	sockaddr_in receiverAddr			= {};
	receiverAddr.sin_family				= AF_INET;
	receiverAddr.sin_port				= htons(12345);
	receiverAddr.sin_addr.s_addr		= INADDR_ANY;

	result = bind(mySocket, (sockaddr*)&receiverAddr, sizeof(receiverAddr));

	if (result != 0)
	{
		cout << "바인딩 에러" << endl;
		closesocket(mySocket);
		WSACleanup();
		return -1;
	}

	// Listen
	if (listen(mySocket, SOMAXCONN) == SOCKET_ERROR)
	{
		cout << "리슨 에러" << endl;
		closesocket(mySocket);
		WSACleanup();
		return -1;
	}


	// Accept
	SOCKET clientSocket;
	while (true)
	{
		cout << "연결 대기 시작" << endl;
		clientSocket = accept(mySocket, nullptr, nullptr);

		if (clientSocket == INVALID_SOCKET)
		{
			cout << "Accept 에러" << endl;
			closesocket(mySocket);
			WSACleanup();
			return -1;
		}

		cout << "클라 연결 성공" << endl;

		// 수신 대기

		ClientInfo* info = new ClientInfo();
		info->socket = clientSocket;
		info->dataBuf.buf = info->buffer;
		info->dataBuf.len = 512;
		info->flags = 0;
		info->recvByte = 0;


		// WSARecv의 두 번째 인자의 자료형이 LPWSABUF임
		// LPWSABUF 안에 버퍼랑 길이 넣어주고 해당 값을 두 번째 인자에 넣어줌
		result = WSARecv(info->socket, &info->dataBuf, 1, &info->recvByte, &info->flags, &info->overlapped, RecvCallBackFunc);

		if (result == SOCKET_ERROR)
		{
			// WSA_IO_PENDING : 비동기 처리가 되어서 나옴
			if (WSAGetLastError() == WSA_IO_PENDING)
			{
				cout << "수신 성공" << endl;
			}
			else
			{
				break;
			}
		}
	}

	// 송신

	// 해제
	closesocket(clientSocket);
	closesocket(mySocket);
	WSACleanup();

	return 0;
}