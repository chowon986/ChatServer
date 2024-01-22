#include <iostream>
#include <ws2tcpip.h>
#include <WinSock2.h>
#include <thread>
#include <vector>
#include <string>
#include <map>

using namespace std;

#pragma comment(lib, "Ws2_32.lib")

#define WORKER_COUNT 8
#define DATA_BUFSIZE 512

enum class IO_TYPE
{
	RECV,
	SEND
};

struct IO_DATA
{
	WSAOVERLAPPED overlapped;
	char buffer[DATA_BUFSIZE];
	WSABUF dataBuf;
	IO_TYPE ioType;
};

struct Client {
	SOCKET socket;
	IO_DATA sendInfo;
	IO_DATA recvInfo;
	DWORD flags;
};

map<SOCKET, Client*> clientMap;

// �۾��� ������ �Լ�
void IOCPWorker(HANDLE iocp) {
	DWORD bufferLen;
	Client* client;
	OVERLAPPED* overlapped;
	thread::id workerNumber = this_thread::get_id();

	cout << "iocp ������ ���� : " << workerNumber << endl;
	while (true) {
		// GetQueuedCompletionStatus �Լ����� �����ִٰ� �̺�Ʈ�� �߻��Ǹ� return��
		BOOL success = GetQueuedCompletionStatus(iocp, &bufferLen, (PULONG_PTR)&client, &overlapped, INFINITE);
		IO_DATA* ioData = (IO_DATA*)overlapped;
		
		cout << "Worker : " << workerNumber << " IO_TYPE : " << (int)ioData->ioType << " Buffer Size : " << bufferLen << endl;

		if (bufferLen == 0)
		{
			cout << "Ŭ�� ���� ����" << endl;
			closesocket(client->socket);
			clientMap.erase(client->socket);
			delete client;
			return;
		}

		if (ioData->ioType == IO_TYPE::SEND) continue;

		// recv ���� buffer send�� ����
		// ������ �״�� ��� client���� send
		string sockNumber = "["+ to_string(client->socket) + "] ";
		for (auto iter = clientMap.begin(); iter != clientMap.end(); iter++)
		{
			Client* sendClient = iter->second;
			memcpy_s(sendClient->sendInfo.buffer, DATA_BUFSIZE, sockNumber.c_str(), sockNumber.length()+1);
			strcat_s(sendClient->sendInfo.buffer, client->recvInfo.buffer);
			sendClient->sendInfo.dataBuf.len = strlen(sendClient->sendInfo.buffer) + 1;

			if (WSASend(sendClient->socket, &sendClient->sendInfo.dataBuf, 1, NULL, 0, &sendClient->sendInfo.overlapped, NULL) == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSA_IO_PENDING)
				{
					printf("Error - Fail WSASend(error_code : %d)\n", WSAGetLastError());
				}
			}
		}

		// recv ��ȣ��
		if (WSARecv(client->socket, &client->recvInfo.dataBuf, 1, NULL, &client->flags, &client->recvInfo.overlapped, NULL) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				printf("Error - Fail WSARecv(error_code : %d)\n", WSAGetLastError());
			}
		}
	}
}

int main()
{
	//	WSAStartup	
	WSADATA wsa;
	int		result;

	result = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (result != 0)
	{
		cout << "WSAStartup ����" << endl;
		return -1;
	}

	//	�񵿱�� ��Ĺ ���� .ssm
	SOCKET mySocket = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (mySocket == INVALID_SOCKET)
	{
		cerr << "socket ���� ����" << endl;
		WSACleanup();

		return -1;
	}

	// �������� �ּ� ����
	sockaddr_in     receiverAddr = {};
	receiverAddr.sin_family = AF_INET;					//	���� ��������
	receiverAddr.sin_port = htons(12345);				//	�������� ��Ʈ ��ȣ
	receiverAddr.sin_addr.s_addr = INADDR_ANY;			//	������ IP

	//	bind
	// ���� ���ε�
	result = bind(mySocket, (sockaddr*)&receiverAddr, sizeof(receiverAddr));

	if (result != 0)
	{
		cerr << "binding ����" << endl;
		closesocket(mySocket);
		WSACleanup();
		return -1;
	}

	//	listen
	if (listen(mySocket, SOMAXCONN) == SOCKET_ERROR)
	{
		cerr << "listen ����" << endl;
		closesocket(mySocket);
		WSACleanup();
		return -1;
	}

	// IOCP ����
	HANDLE iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// IO�� ó���� ������ ����
	vector<thread*> workers;

	for (int i = 0; i < WORKER_COUNT; i++)
	{
		thread* worker = new thread(IOCPWorker,iocp);
		workers.push_back(worker);
	}

	cout << "accept ��� ����" << endl;
	while (true)
	{
		auto clientSocket = accept(mySocket, nullptr, nullptr);

		if (clientSocket == INVALID_SOCKET)
		{
			cerr << "accept ����" << endl;
			closesocket(mySocket);
			WSACleanup();
			return -1;
		}

		cout << "Ŭ�� ���� ����" << endl;

		// Ŭ���̾�Ʈ ����
		Client* client = new Client;
		client->socket = clientSocket;

		client->flags = 0;

		client->recvInfo.dataBuf.buf = client->recvInfo.buffer;
		client->recvInfo.dataBuf.len = DATA_BUFSIZE;
		client->recvInfo.ioType = IO_TYPE::RECV;
		ZeroMemory(&(client->recvInfo.overlapped), sizeof(OVERLAPPED));

		client->sendInfo.dataBuf.buf = client->sendInfo.buffer;
		client->sendInfo.dataBuf.len = DATA_BUFSIZE;
		client->sendInfo.ioType = IO_TYPE::SEND;
		ZeroMemory(&(client->sendInfo.overlapped), sizeof(OVERLAPPED));
		
		// �����ڿ� �߰�
		clientMap.insert({ client->socket, client });

		// Ŭ���̾�Ʈ ���Ͽ� IOCP ����
		// // Socket�̾ƴ� HANDLE�� �ִ� ������ IOCP�� ���� ����� ���� ����µ� ��� IO���� ����� �� �ֱ⶧��
		// ULONG_PRT�� ��ȯ�Ͽ� �ѱ� client������ ���Ŀ� CALLBACK���� ��밡���ϴ�.
		CreateIoCompletionPort((HANDLE)client->socket, iocp, (ULONG_PTR)client, 0);

		// ���� recv
		if (WSARecv(client->socket, &client->recvInfo.dataBuf, 1, NULL, &client->flags, &client->recvInfo.overlapped, NULL) == SOCKET_ERROR)
		{
			auto lastError = WSAGetLastError();
			if (lastError == WSA_IO_PENDING)
			{
				cout << "����ó��" << endl;
			}
			else
			{
				// TODO : �����ִ� ��Ȳ
				break;
			}
		}
	}
	
	for (int i = 0; i < WORKER_COUNT; i++)
	{
		workers[i]->detach();
		delete workers[i];
	}

	closesocket(mySocket);
	WSACleanup();
	return 1;
}