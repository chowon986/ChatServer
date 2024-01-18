#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <map>
#include <string>

using namespace std;

// Ws2_32.lib 라는 라이브러리 파일을 포함해라
#pragma comment(lib, "Ws2_32.lib")

// UTF8 -> UNICODE(widechar) -> ANSI
void UTF8to_ANSI(char buffer[], int _byteSize)
{
	// 문자열 크기 측정
	int wideBufferSize = MultiByteToWideChar(CP_UTF8, 0, buffer, _byteSize, nullptr, 0);

	// 크기만큼의 유니코드형 문자열 동적 할당
	wchar_t* wideBuffer = new wchar_t[wideBufferSize + 1];

	// UTF8 -> UNICODE
	MultiByteToWideChar(CP_UTF8, 0, buffer, _byteSize, wideBuffer, wideBufferSize);
	wideBuffer[wideBufferSize] = L'\0';

	// WIDE CHAR를 CHAR로 변환
	// 유니코드로 바꾼 문자열의 크기 측정
	int ansiLength = WideCharToMultiByte(CP_ACP, 0, wideBuffer, -1, NULL, 0, NULL, NULL);

	// buffer에 변환해서 적용(UNICODE -> ANSI)
	WideCharToMultiByte(CP_ACP, 0, wideBuffer, -1, buffer, ansiLength, NULL, NULL);

	// NULL 문자
	buffer[ansiLength] = L'\0';

	wideBuffer = nullptr;
	delete[] wideBuffer;
}

int main()
{
	// (1) WSA 초기화
	WSADATA wsa;
	int result;

	result = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (result != 0)
	{
		cout << "WSAStartup 에러" << '\n';
		return -1;
	}

	// (2) 소켓 생성
	SOCKET mySocket = socket(AF_INET, SOCK_STREAM, 0); // UDP는 SOCK_DGRAM, TCP는 SOCK_STREAM

	if (mySocket == SOCKET_ERROR)
	{
		cout << "socket 생성 에러" << '\n';
		WSACleanup();
		return -1;
	}

	// (3) 주소 설정
	sockaddr_in destAddr = {};
	destAddr.sin_family = AF_INET;
	destAddr.sin_port = htons(12345);

	inet_pton(AF_INET, "127.0.0.1", &destAddr.sin_addr); // #include <WS2tcpip.h> 추가해야 함

	sockaddr_in recvAddr = {};
	recvAddr.sin_family = AF_INET;
	recvAddr.sin_port = htons(12345);
	recvAddr.sin_addr.s_addr = INADDR_ANY;		

	// (4) 소켓 바인딩
	result = bind(mySocket, (sockaddr*)&recvAddr, sizeof(recvAddr)); // 바인딩 -> 포트 하나에 한 번만 가능

	if (result != 0)
	{
		cout << "바인딩 에러" << '\n';
		closesocket(mySocket);
		WSACleanup();
		return -1;
	}

	cout << "서버 시작" << '\n';

	// listen -> 문지기 소켓 만들기
	if(listen(mySocket, SOMAXCONN) == SOCKET_ERROR)
	{
		cerr << "listen 에러" << '\n';
		closesocket(mySocket);
		WSACleanup();
		return -1;
	}

	// accept
	cout << "연결 대기 시작" << '\n';

	auto clientSocket = accept(mySocket, nullptr, nullptr); // 클라 연결 됐어

	if (clientSocket == INVALID_SOCKET)
	{
		cerr << "accept 에러" << '\n';
		closesocket(mySocket);
		WSACleanup();
		return -1;
	}

	cout << "클라 연결 성공" << '\n';

	while (true)
	{
		char buffer[512] = "";
		int recvBytes = recv(clientSocket, buffer, sizeof(buffer), 0);

		if (recvBytes <= 0)
		{
			cout << "클라 연결 문제" << '\n';
			break;
		}

		cout << "서버에서 수신한 메시지 : " << buffer << '\n';
		send(clientSocket, buffer, strlen(buffer), 0);
	}

	closesocket(mySocket);
	WSACleanup();
	return 0;
}