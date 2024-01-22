#include "stream_buffer.h"
#include <windows.h>

StreamBuffer::StreamBuffer() 
	:max_(STREAM_BUFSIZE)
{
	ZeroMemory(buffer_, STREAM_BUFSIZE);
	head_ = 0;
	tail_ = 0;
	size_ = 0;
}

StreamBuffer::~StreamBuffer()
{

}

size_t StreamBuffer::Enqueue(char* data, size_t data_size)
{
	// ���� ������ ������
	size_t available_size = 0;

	// ���� ������ġ
	size_t start_pos = 0;

	size_t copy_size = data_size;

	// tail�� head���� Ŭ�� (�Ϲ����� ��Ȳ)
	if (tail_ >= head_)
	{
		available_size = max_ - tail_;
		start_pos = tail_;
	}
	// ���۰� �ڷ� �ѹ��� ���ư� ���
	else
	{
		available_size = head_ - tail_;
		start_pos = tail_;
	}

	if (copy_size > available_size)
		copy_size = available_size;


	if (max_ - size_ < copy_size)
		return 0;

	// ��ġ ����
	tail_ += copy_size;
	size_ += copy_size;

	if (tail_ == max_)
	{
		tail_ = 0;
	}

	// ���� ������ ������ ��ŭ ����
	memcpy_s(&buffer_[start_pos], available_size, data, available_size);


	// ������ ����� ���� ������ ���� �۴ٸ� ���ȣ��
	size_t call_size = 0;
	if (copy_size < data_size)
	{
		call_size = Enqueue(&data[copy_size], data_size - copy_size);
	}

	return (copy_size + call_size);
}

void StreamBuffer::Dequeue(char* get_buffer, size_t get_size)
{
	// ���� ������ ������
	size_t available_size = 0;


	if (size_ < get_size) 
		get_size = size_;

	// tail�� head���� Ŭ�� (�Ϲ����� ��Ȳ)
	if (tail_ >= head_)
		available_size = tail_ - head_;
	// ���۰� �ڷ� �ѹ��� ���ư� ���
	else
		available_size = max_ - head_;


	if (available_size > get_size)
		available_size = get_size;

	memcpy_s(get_buffer, get_size, &buffer_[head_], available_size);


	size_ -= get_size;
	head_ += get_size;
	get_size -= available_size;

	if (head_ == max_)
		head_ = 0;


	if (size_ == 0)
	{
		head_ = 0;
		tail_ = 0;
	}

	if (get_size > 0)
	{
		Dequeue(&get_buffer[available_size], get_size);
	}
}