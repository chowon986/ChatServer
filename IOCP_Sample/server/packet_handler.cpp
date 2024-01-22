#include "server_defines.h"
#include "packet_handler.h"
#include "tcp_session.h"
#include "stream_buffer.h"
#include "server_defines.h"

PacketHandler::PacketHandler()
{
}

PacketHandler::~PacketHandler()
{

}

bool PacketHandler::AddPacket(unsigned short packet_id, PACKET_FUNC callback)
{
	// �ߺ� id
	if (callback_list_.find(packet_id) != callback_list_.end())
		return false;

	callback_list_.insert({ packet_id , callback });

	return true;
}

bool PacketHandler::CallPacket(TcpSession* tcp_session)
{
	// ���ǿ��� ���� ��������
	StreamBuffer* stream = tcp_session->GetStreamBuffer();

	// ���� ������Ȯ��
	size_t buffer_size = stream->GetSize();
	while (buffer_size > PACKET_HEADER_SIZE)
	{
		// ����� �Ϻ� �������� (��Ʈ������ ������ ���� ����)
		BasePacket header;
		header = stream->PeekBuffer<BasePacket>();
		
		// ��� ũ�� üũ
		if (header.packet_len > buffer_size)
		{
			break;
		}
		
		// ��ϵ� ��Ŷ ã��
		auto item = callback_list_.find(header.packet_id);

		// ���� ��Ŷ
		if (item == callback_list_.end())
			return false;

		PACKET_FUNC cllback = item->second;

		// ��Ŷ ������ ���� (��Ʈ�� ���� ������ ����)
		char buffer[DATA_BUFSIZE];
		stream->Dequeue(buffer, header.packet_len);

		BasePacket* packet = reinterpret_cast<BasePacket*>(buffer);

		// ��ϵ� �Լ� ȣ��
		cllback(tcp_session, packet);

		// ���� ��Ȯ��
		buffer_size = stream->GetSize();
	}

	return true;
}