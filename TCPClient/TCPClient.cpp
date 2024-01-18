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
	sockaddr_in receiverAddr = {};
	receiverAddr.sin_family = AF_INET;
	receiverAddr.sin_port = htons(12345);
	inet_pton(AF_INET, "127.0.0.1", &receiverAddr.sin_addr); // #include <WS2tcpip.h> 추가해야 함

	// (4) 연결 시도
	result = connect(mySocket, (sockaddr*)&receiverAddr, sizeof(receiverAddr));

	if (result == SOCKET_ERROR)
	{
		cout << "connect 에러" << '\n';
		closesocket(mySocket);
		WSACleanup();
		return -1;
	}

	cout << "서버에 연결됨" << endl;
	
	while (true)
	{
		string msg;
		
		cout << "입력 : ";
		getline(cin, msg);

		if (msg == "exit")
		{
			break;
		}

		// msg.c_str() : 문자열의 첫 위치
		send(mySocket, msg.c_str(), msg.length(), 0);

		char buffer[512] = "";
		int recvBytes = recv(mySocket, buffer, sizeof(buffer), 0);

		if (recvBytes <= 0)
		{
			cout << "연결 문제" << '\n';
			break;
		}

		cout << "수신 메시지 : " << buffer << '\n';
	}

	closesocket(mySocket);
	WSACleanup();
	return 0;
}