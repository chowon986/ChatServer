// 클라 서버 한 곳에 다 구현
//#include <iostream>
//#include <WinSock2.h>
//#include <WS2tcpip.h>
//
//using namespace std;
//
//// Ws2_32.lib 라는 라이브러리 파일을 포함해라
//#pragma comment(lib, "Ws2_32.lib")
//
//int main()
//{
//	// (1) WSA 초기화
//	WSADATA wsa;
//	int result;
//
//	// ctrl + shift + spacebar
//	result = WSAStartup(MAKEWORD(2, 2), &wsa);
//
//	if (result != 0)
//	{
//		cout << "WSAStartup 에러" << '\n';
//		return -1;
//	}
//
//	// (2) 소켓 생성
//	// 여기 소켓 통신이 들어갈거임
//	SOCKET mySocket = socket(AF_INET, SOCK_DGRAM, 0);
//
//	if (mySocket == SOCKET_ERROR)
//	{
//		cout << "socket 생성 에러" << '\n';
//		WSACleanup();
//		return -1;
//	}
//
//	sockaddr_in destAddr = {};
//	destAddr.sin_family = AF_INET;
//	destAddr.sin_port = htons(12345);
//
//	inet_pton(AF_INET, "127.0.0.1", &destAddr.sin_addr); // #include <WS2tcpip.h> 추가해야 함
//
//	sockaddr_in recvAddr = {};
//	recvAddr.sin_family = AF_INET;
//	recvAddr.sin_port = htons(12345);
//	recvAddr.sin_addr.s_addr = INADDR_ANY;		// 수신할 IP는 컴퓨터에 붙여진 모든 IP
//
//	// (3) 소켓 바인딩
//	result = bind(mySocket, (sockaddr*)&recvAddr, sizeof(recvAddr)); // 바인딩 -> 포트 하나에 한 번만 가능
//
//	if (result != 0)
//	{
//		cout << "바인딩 에러" << '\n';
//		closesocket(mySocket);
//		WSACleanup();
//		return -1;
//	}
//
//	char inputStr[512] = "";
//	cout << "송신할 내용 : ";
//	cin >> inputStr;
//
//	// (4) sendto
//	sendto(mySocket, inputStr, strlen(inputStr) + 1, 0, (struct sockaddr*)&destAddr, sizeof(destAddr)); // sendto는 바인딩 하고 해야 함 (클라)
//	// recv를 먼저 하면 대기를 해버리기 때문에 send를 먼저 함
//
//	// (5) recvfrom
//	char buffer[1024] = "";
//	sockaddr_in senderAddr;
//	socklen_t senderAddrLen = sizeof(senderAddr);
//
//	int receivedBytes = recvfrom(mySocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&senderAddr, &senderAddrLen);
//
//	cout << "수신된 내용 : " << buffer << '\n';
//
//	closesocket(mySocket); // 소켓 생성에 성공했으므로 끝나기 전에 closesocket을 해 줘야 함
//	WSACleanup(); // 다했으니까 cleanup
//
//	return 0;
//}


// 유니티 연결
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

	// ctrl + shift + spacebar
	result = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (result != 0)
	{
		cout << "WSAStartup 에러" << '\n';
		return -1;
	}

	// (2) 소켓 생성
	// 여기 소켓 통신이 들어갈거임
	SOCKET mySocket = socket(AF_INET, SOCK_DGRAM, 0);

	if (mySocket == INVALID_SOCKET)
	{
		cout << "socket 생성 에러" << '\n';
		WSACleanup();
		return -1;
	}

	sockaddr_in destAddr = {};
	destAddr.sin_family = AF_INET;
	destAddr.sin_port = htons(12345);

	inet_pton(AF_INET, "127.0.0.1", &destAddr.sin_addr); // #include <WS2tcpip.h> 추가해야 함

	sockaddr_in recvAddr = {};
	recvAddr.sin_family = AF_INET;
	recvAddr.sin_port = htons(12345);
	recvAddr.sin_addr.s_addr = INADDR_ANY;		// 수신할 IP는 컴퓨터에 붙여진 모든 IP

	// (3) 소켓 바인딩
	result = bind(mySocket, (sockaddr*)&recvAddr, sizeof(recvAddr)); // 바인딩 -> 포트 하나에 한 번만 가능

	if (result != 0)
	{
		cout << "바인딩 에러" << '\n';
		closesocket(mySocket);
		WSACleanup();
		return -1;
	}

	cout << "서버 시작" << '\n';

	map<string, sockaddr_in> userList;

	while (1)
	{
		char buffer[1024] = "";
		sockaddr_in senderAddr;
		socklen_t senderAddrLen = sizeof(senderAddr);

		int receivedBytes = recvfrom(mySocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&senderAddr, &senderAddrLen);

		// IP:Port로 문자열을 만들고
		char senderIP[256] = "";
		inet_ntop(AF_INET, &senderAddr.sin_addr, senderIP, sizeof(senderIP)); // ipv4의 이진 표현을 문자열 표현으로 바꾸는 작업

		char userInfo[256]; // '127.0.0.1:12345"
		sprintf_s(userInfo, "%s:%d", senderIP, senderAddr.sin_port);

		// recvfrom()는 실패하면 0이 아닌 수가 나옴
		if (receivedBytes < 0)
		{
			int errCode = WSAGetLastError();
			// 보냈더니 끊겨있어라면 WSAECONNRESET 에러가 나옴
			if (errCode == WSAECONNRESET)
			{
				userList.erase(userInfo);
				cout << userInfo << " : 연결 해제" << '\n';
				continue;
			}
		}
		else
		{
			cout << userInfo << " : 연결 등록" << '\n';
			userList[userInfo] = senderAddr;
		}

		//// map에 있는 것인지 확인 후 없으면 추가
		//auto finder = userList.find(userInfo);
		//
		//// 맵 안에 없으면
		//if (finder == userList.end())
		//{
		//	cout << userInfo << " : 연결 등록" << '\n';
		//	userList.insert({ userInfo, senderAddr });
		//}

		for (auto i : userList)
		{
			// map에 있는 모든 사람에게 전송
			sendto(mySocket, buffer, int(strlen(buffer) + 1), 0, (struct sockaddr*)&i.second, sizeof(i.second)); // sendto는 바인딩 하고 해야 함 (클라)
		}

		// 보내고 나서 로그로 보낼 때 문제가 생기는 거니까 보내고 나서 수정해야 함
		// UTF8 -> ANSI 변환
		UTF8to_ANSI(buffer, sizeof(buffer)); // 변환해서 cout 처리

		cout << buffer << '\n';

		// 실제로는 클라에서 서버를 종료시킬 일이 없음
		//if (strstr(buffer, "quit") != nullptr)
		//{
		//	break;
		//}
	}

	closesocket(mySocket); 
	WSACleanup();

	return 0;
}

