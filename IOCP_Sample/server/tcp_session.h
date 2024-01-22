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

	// recv 요청 함수
	const int CallRecv();

	// send 요청 함수
	const int CallSend(void* buffer, int size);

	// 실제 recv가 완료되었을때
	size_t Recv(size_t recv_byte);

	// 실제 send가 완료되었을떄
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

