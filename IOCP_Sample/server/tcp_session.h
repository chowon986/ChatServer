#pragma once

struct IO_DATA;
class  StreamBuffer;

class TcpSession
{
public:
	TcpSession();
	~TcpSession();

public:
	void Init(SOCKET socket);
	void Close();

	// recv ��û �Լ�
	const int CallRecv();

	// send ��û �Լ�
	const int CallSend(void* buffer, int size);

	// ���� recv�� �Ϸ�Ǿ�����
	size_t Recv(size_t recv_byte);

	// ���� send�� �Ϸ�Ǿ�����
	//void Send();

public:
	inline const int GetLastError()
	{
		return last_error_code_;
	}

	inline const SOCKET GetSocket()
	{
		return socket_;
	}

	inline StreamBuffer* GetStreamBuffer()
	{
		return recv_buffer_;
	}

private:
	SOCKET socket_;

	IO_DATA* send_info_;
	IO_DATA* recv_info_;
	StreamBuffer* recv_buffer_;

	unsigned long flags_;
	int last_error_code_;
};

