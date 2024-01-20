// Ŭ�� ���� �� ���� �� ����
//#include <iostream>
//#include <WinSock2.h>
//#include <WS2tcpip.h>
//
//using namespace std;
//
//// Ws2_32.lib ��� ���̺귯�� ������ �����ض�
//#pragma comment(lib, "Ws2_32.lib")
//
//int main()
//{
//	// (1) WSA �ʱ�ȭ
//	WSADATA wsa;
//	int result;
//
//	// ctrl + shift + spacebar
//	result = WSAStartup(MAKEWORD(2, 2), &wsa);
//
//	if (result != 0)
//	{
//		cout << "WSAStartup ����" << '\n';
//		return -1;
//	}
//
//	// (2) ���� ����
//	// ���� ���� ����� ������
//	SOCKET mySocket = socket(AF_INET, SOCK_DGRAM, 0);
//
//	if (mySocket == SOCKET_ERROR)
//	{
//		cout << "socket ���� ����" << '\n';
//		WSACleanup();
//		return -1;
//	}
//
//	sockaddr_in destAddr = {};
//	destAddr.sin_family = AF_INET;
//	destAddr.sin_port = htons(12345);
//
//	inet_pton(AF_INET, "127.0.0.1", &destAddr.sin_addr); // #include <WS2tcpip.h> �߰��ؾ� ��
//
//	sockaddr_in recvAddr = {};
//	recvAddr.sin_family = AF_INET;
//	recvAddr.sin_port = htons(12345);
//	recvAddr.sin_addr.s_addr = INADDR_ANY;		// ������ IP�� ��ǻ�Ϳ� �ٿ��� ��� IP
//
//	// (3) ���� ���ε�
//	result = bind(mySocket, (sockaddr*)&recvAddr, sizeof(recvAddr)); // ���ε� -> ��Ʈ �ϳ��� �� ���� ����
//
//	if (result != 0)
//	{
//		cout << "���ε� ����" << '\n';
//		closesocket(mySocket);
//		WSACleanup();
//		return -1;
//	}
//
//	char inputStr[512] = "";
//	cout << "�۽��� ���� : ";
//	cin >> inputStr;
//
//	// (4) sendto
//	sendto(mySocket, inputStr, strlen(inputStr) + 1, 0, (struct sockaddr*)&destAddr, sizeof(destAddr)); // sendto�� ���ε� �ϰ� �ؾ� �� (Ŭ��)
//	// recv�� ���� �ϸ� ��⸦ �ع����� ������ send�� ���� ��
//
//	// (5) recvfrom
//	char buffer[1024] = "";
//	sockaddr_in senderAddr;
//	socklen_t senderAddrLen = sizeof(senderAddr);
//
//	int receivedBytes = recvfrom(mySocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&senderAddr, &senderAddrLen);
//
//	cout << "���ŵ� ���� : " << buffer << '\n';
//
//	closesocket(mySocket); // ���� ������ ���������Ƿ� ������ ���� closesocket�� �� ��� ��
//	WSACleanup(); // �������ϱ� cleanup
//
//	return 0;
//}


// ����Ƽ ����
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <map>
#include <string>

using namespace std;

// Ws2_32.lib ��� ���̺귯�� ������ �����ض�
#pragma comment(lib, "Ws2_32.lib")

// UTF8 -> UNICODE(widechar) -> ANSI
void UTF8to_ANSI(char buffer[], int _byteSize)
{
	// ���ڿ� ũ�� ����
	int wideBufferSize = MultiByteToWideChar(CP_UTF8, 0, buffer, _byteSize, nullptr, 0);
	
	// ũ�⸸ŭ�� �����ڵ��� ���ڿ� ���� �Ҵ�
	wchar_t* wideBuffer = new wchar_t[wideBufferSize + 1];

	// UTF8 -> UNICODE
	MultiByteToWideChar(CP_UTF8, 0, buffer, _byteSize, wideBuffer, wideBufferSize);
	wideBuffer[wideBufferSize] = L'\0';

	// WIDE CHAR�� CHAR�� ��ȯ
	// �����ڵ�� �ٲ� ���ڿ��� ũ�� ����
	int ansiLength = WideCharToMultiByte(CP_ACP, 0, wideBuffer, -1, NULL, 0, NULL, NULL);

	// buffer�� ��ȯ�ؼ� ����(UNICODE -> ANSI)
	WideCharToMultiByte(CP_ACP, 0, wideBuffer, -1, buffer, ansiLength, NULL, NULL);

	// NULL ����
	buffer[ansiLength] = L'\0';

	wideBuffer = nullptr;
	delete[] wideBuffer;
}

int main()
{
	// (1) WSA �ʱ�ȭ
	WSADATA wsa;
	int result;

	// ctrl + shift + spacebar
	result = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (result != 0)
	{
		cout << "WSAStartup ����" << '\n';
		return -1;
	}

	// (2) ���� ����
	// ���� ���� ����� ������
	SOCKET mySocket = socket(AF_INET, SOCK_DGRAM, 0);

	if (mySocket == INVALID_SOCKET)
	{
		cout << "socket ���� ����" << '\n';
		WSACleanup();
		return -1;
	}

	sockaddr_in destAddr = {};
	destAddr.sin_family = AF_INET;
	destAddr.sin_port = htons(12345);

	inet_pton(AF_INET, "127.0.0.1", &destAddr.sin_addr); // #include <WS2tcpip.h> �߰��ؾ� ��

	sockaddr_in recvAddr = {};
	recvAddr.sin_family = AF_INET;
	recvAddr.sin_port = htons(12345);
	recvAddr.sin_addr.s_addr = INADDR_ANY;		// ������ IP�� ��ǻ�Ϳ� �ٿ��� ��� IP

	// (3) ���� ���ε�
	result = bind(mySocket, (sockaddr*)&recvAddr, sizeof(recvAddr)); // ���ε� -> ��Ʈ �ϳ��� �� ���� ����

	if (result != 0)
	{
		cout << "���ε� ����" << '\n';
		closesocket(mySocket);
		WSACleanup();
		return -1;
	}

	cout << "���� ����" << '\n';

	map<string, sockaddr_in> userList;

	while (1)
	{
		char buffer[1024] = "";
		sockaddr_in senderAddr;
		socklen_t senderAddrLen = sizeof(senderAddr);

		int receivedBytes = recvfrom(mySocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&senderAddr, &senderAddrLen);

		// IP:Port�� ���ڿ��� �����
		char senderIP[256] = "";
		inet_ntop(AF_INET, &senderAddr.sin_addr, senderIP, sizeof(senderIP)); // ipv4�� ���� ǥ���� ���ڿ� ǥ������ �ٲٴ� �۾�

		char userInfo[256]; // '127.0.0.1:12345"
		sprintf_s(userInfo, "%s:%d", senderIP, senderAddr.sin_port);

		// recvfrom()�� �����ϸ� 0�� �ƴ� ���� ����
		if (receivedBytes < 0)
		{
			int errCode = WSAGetLastError();
			// ���´��� �����־��� WSAECONNRESET ������ ����
			if (errCode == WSAECONNRESET)
			{
				userList.erase(userInfo);
				cout << userInfo << " : ���� ����" << '\n';
				continue;
			}
		}
		else
		{
			cout << userInfo << " : ���� ���" << '\n';
			userList[userInfo] = senderAddr;
		}

		//// map�� �ִ� ������ Ȯ�� �� ������ �߰�
		//auto finder = userList.find(userInfo);
		//
		//// �� �ȿ� ������
		//if (finder == userList.end())
		//{
		//	cout << userInfo << " : ���� ���" << '\n';
		//	userList.insert({ userInfo, senderAddr });
		//}

		for (auto i : userList)
		{
			// map�� �ִ� ��� ������� ����
			sendto(mySocket, buffer, int(strlen(buffer) + 1), 0, (struct sockaddr*)&i.second, sizeof(i.second)); // sendto�� ���ε� �ϰ� �ؾ� �� (Ŭ��)
		}

		// ������ ���� �α׷� ���� �� ������ ����� �Ŵϱ� ������ ���� �����ؾ� ��
		// UTF8 -> ANSI ��ȯ
		UTF8to_ANSI(buffer, sizeof(buffer)); // ��ȯ�ؼ� cout ó��

		cout << buffer << '\n';

		// �����δ� Ŭ�󿡼� ������ �����ų ���� ����
		//if (strstr(buffer, "quit") != nullptr)
		//{
		//	break;
		//}
	}

	closesocket(mySocket); 
	WSACleanup();

	return 0;
}

