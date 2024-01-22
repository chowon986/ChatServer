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

// 작업자 스레드 함수
void IOCPWorker(HANDLE iocp) {
	DWORD bufferLen;
	Client* client;
	OVERLAPPED* overlapped;
	thread::id workerNumber = this_thread::get_id();

	cout << "iocp 스레드 시작 : " << workerNumber << endl;
	while (true) {
		// GetQueuedCompletionStatus 함수에서 멈춰있다가 이벤트가 발생되면 return함
		BOOL success = GetQueuedCompletionStatus(iocp, &bufferLen, (PULONG_PTR)&client, &overlapped, INFINITE);
		IO_DATA* ioData = (IO_DATA*)overlapped;
		
		cout << "Worker : " << workerNumber << " IO_TYPE : " << (int)ioData->ioType << " Buffer Size : " << bufferLen << endl;

		if (bufferLen == 0)
		{
			cout << "클라 연결 종료" << endl;
			closesocket(client->socket);
			clientMap.erase(client->socket);
			delete client;
			return;
		}

		if (ioData->ioType == IO_TYPE::SEND) continue;

		// recv 받은 buffer send로 복사
		// 받은거 그대로 모든 client에게 send
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

		// recv 재호출
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
		cout << "WSAStartup 에러" << endl;
		return -1;
	}

	//	비동기용 소캣 생성 .ssm
	SOCKET mySocket = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

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
	receiverAddr.sin_addr.s_addr = INADDR_ANY;			//	수신할 IP

	//	bind
	// 소켓 바인딩
	result = bind(mySocket, (sockaddr*)&receiverAddr, sizeof(receiverAddr));

	if (result != 0)
	{
		cerr << "binding 에러" << endl;
		closesocket(mySocket);
		WSACleanup();
		return -1;
	}

	//	listen
	if (listen(mySocket, SOMAXCONN) == SOCKET_ERROR)
	{
		cerr << "listen 에러" << endl;
		closesocket(mySocket);
		WSACleanup();
		return -1;
	}

	// IOCP 생성
	HANDLE iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// IO를 처리할 스레드 생성
	vector<thread*> workers;

	for (int i = 0; i < WORKER_COUNT; i++)
	{
		thread* worker = new thread(IOCPWorker,iocp);
		workers.push_back(worker);
	}

	cout << "accept 대기 시작" << endl;
	while (true)
	{
		auto clientSocket = accept(mySocket, nullptr, nullptr);

		if (clientSocket == INVALID_SOCKET)
		{
			cerr << "accept 에러" << endl;
			closesocket(mySocket);
			WSACleanup();
			return -1;
		}

		cout << "클라 연결 성공" << endl;

		// 클라이언트 생성
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
		
		// 관리자에 추가
		clientMap.insert({ client->socket, client });

		// 클라이언트 소켓에 IOCP 연결
		// // Socket이아닌 HANDLE로 넣는 이유는 IOCP는 파일 입출력 소켓 입출력등 모든 IO에서 사용할 수 있기때문
		// ULONG_PRT로 변환하여 넘긴 client정보는 이후에 CALLBACK에서 사용가능하다.
		CreateIoCompletionPort((HANDLE)client->socket, iocp, (ULONG_PTR)client, 0);

		// 최초 recv
		if (WSARecv(client->socket, &client->recvInfo.dataBuf, 1, NULL, &client->flags, &client->recvInfo.overlapped, NULL) == SOCKET_ERROR)
		{
			auto lastError = WSAGetLastError();
			if (lastError == WSA_IO_PENDING)
			{
				cout << "정상처리" << endl;
			}
			else
			{
				// TODO : 문제있는 상황
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