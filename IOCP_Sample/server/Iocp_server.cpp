#include "server_defines.h"
#include "Iocp_server.h"
#include "packet_handler.h"
#include "tcp_session.h"


#pragma comment(lib, "Ws2_32.lib")


IocpServer::IocpServer()
	:iocp_(nullptr), 
	listen_socket_(0)

{
	packek_handler_ = new PacketHandler;
	Init();
}

IocpServer::~IocpServer()
{
	delete packek_handler_;
}

void IocpServer::worker() {
	DWORD buffer_len;
	TcpSession* session;
	OVERLAPPED* overlapped;
	std::thread::id worker_number = std::this_thread::get_id();
	while (true) {
		// GetQueuedCompletionStatus �Լ����� �����ִٰ� �̺�Ʈ�� �߻��Ǹ� return��
		BOOL success = GetQueuedCompletionStatus(iocp_, &buffer_len, (PULONG_PTR)&session, &overlapped, INFINITE);
		IO_DATA* ioData = (IO_DATA*)overlapped;

		if (buffer_len == 0)
		{
			CloseSession(session);
			continue;
		}

		if (ioData->ioType == IO_TYPE::RECV)
		{
			// recv �Ϸ� ó��
			size_t enq_size = session->Recv(buffer_len);

			// ���۰� Ǯ����Ȳ
			if (enq_size != buffer_len)
			{
				CloseSession(session);
				continue;
			}

			// ��Ŷ ó��
			bool invalid = packek_handler_->CallPacket(session);
			if (invalid == false)
			{
				CloseSession(session);
				continue;
			}

			// ���� ��ȣ��
			session->CallRecv();
		}
	}
}

void IocpServer::Start(unsigned short port)
{
	std::cout << "���� ����" << std::endl;

	port_ = port;
	initSocket();
	InitIocp();
	Run();
}

void IocpServer::Run()
{
	while (true)
	{
		SOCKET client_socket = accept(listen_socket_, nullptr, nullptr);

		if (client_socket == INVALID_SOCKET)
		{
			closesocket(listen_socket_);
			WSACleanup();
			return;
		}


		// Ŭ���̾�Ʈ ����
		TcpSession* session = CreateSession();
		session->Init(client_socket);

		// �����ڿ� �߰�
		session_map_.insert({ client_socket, session });

		// Ŭ���̾�Ʈ ���Ͽ� IOCP ����
		CreateIoCompletionPort((HANDLE)client_socket, iocp_, (ULONG_PTR)session, 0);

		// ���� recv
		int last_error_code = session->CallRecv();
	}

	for (int i = 0; i < WORKER_COUNT; i++)
	{
		workers_[i]->detach();
		delete workers_[i];
	}

	closesocket(listen_socket_);
	WSACleanup();
}

int IocpServer::initSocket()
{
	//	WSAStartup	
	WSADATA wsa;
	int		result;

	result = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (result != 0)
	{
		return -1;
	}

	//	�񵿱�� ��Ĺ ���� .ssm
	listen_socket_ = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (listen_socket_ == INVALID_SOCKET)
	{
		WSACleanup();

		return -1;
	}

	// �������� �ּ� ����
	sockaddr_in     receiverAddr = {};
	receiverAddr.sin_family = AF_INET;					//	���� ��������
	receiverAddr.sin_port = htons(port_);				//	�������� ��Ʈ ��ȣ
	receiverAddr.sin_addr.s_addr = INADDR_ANY;			//	������ IP

	//	bind
	// ���� ���ε�
	result = bind(listen_socket_, (sockaddr*)&receiverAddr, sizeof(receiverAddr));

	if (result != 0)
	{
		closesocket(listen_socket_);
		WSACleanup();
		return -1;
	}

	//	listen
	if (listen(listen_socket_, SOMAXCONN) == SOCKET_ERROR)
	{
		closesocket(listen_socket_);
		WSACleanup();
		return -1;
	}

	return 0;
}

void IocpServer::InitIocp()
{
	// IOCP ����
	iocp_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	for (int i = 0; i < WORKER_COUNT; i++)
	{
		std::thread* worker = new std::thread(&IocpServer::worker, this);
		workers_.push_back(worker);
	}
}

void IocpServer::CloseSession(TcpSession* session)
{
	session->Close();
	session_map_.erase(session->GetSocket());
	delete session;
}