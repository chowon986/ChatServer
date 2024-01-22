#include "server_defines.h"
#include "tcp_session.h"
#include "stream_buffer.h"

TcpSession::TcpSession()
{
	send_info_ = new IO_DATA;
	recv_info_ = new IO_DATA;
	recv_buffer_ = new StreamBuffer;
}

TcpSession::~TcpSession()
{
	delete recv_buffer_;
	delete recv_info_;
	delete send_info_;
}


void TcpSession::Init(SOCKET socket)
{
	flags_ = 0;
	socket_ = socket;

	// recv ���� �ʱ�ȭ
	recv_info_->dataBuf.buf = recv_info_->buffer;
	recv_info_->dataBuf.len = DATA_BUFSIZE;
	recv_info_->ioType = IO_TYPE::RECV;
	ZeroMemory(&(recv_info_->overlapped), sizeof(OVERLAPPED));

	// send ���� �ʱ�ȭ
	send_info_->dataBuf.buf = send_info_->buffer;
	send_info_->dataBuf.len = DATA_BUFSIZE;
	send_info_->ioType = IO_TYPE::SEND;
	ZeroMemory(&(send_info_->overlapped), sizeof(OVERLAPPED));
}

void TcpSession::Close()
{
	closesocket(socket_);
}

// recv ��û �Լ�
const int TcpSession::CallRecv()
{
	if (WSARecv(socket_, &recv_info_->dataBuf, 1, NULL, &flags_, &recv_info_->overlapped, NULL) == SOCKET_ERROR)
	{
		last_error_code_ = WSAGetLastError();
		if (last_error_code_ == WSA_IO_PENDING)
		{
			// ����
		}
		else
		{
			// ����	
		}

		return last_error_code_;
	}

	return false;
}

// send ��û �Լ�
const int TcpSession::CallSend(void* buffer, int size)
{
	// ���ۿ� ����
	memcpy_s(send_info_->buffer, DATA_BUFSIZE, buffer, size);
	send_info_->dataBuf.len = size;
	if (WSASend(socket_, &send_info_->dataBuf, 1, NULL, 0, &send_info_->overlapped, NULL) == SOCKET_ERROR)
	{
		last_error_code_ = WSAGetLastError();
		if (last_error_code_ == WSA_IO_PENDING)
		{
			// ����
		}
		else
		{
			// ����	
		}

		return last_error_code_;
	}

	return false;
}

// ���� recv�� �Ϸ�Ǿ�����
size_t TcpSession::Recv(size_t recv_byte)
{
	return recv_buffer_->Enqueue(recv_info_->buffer, recv_byte);
}