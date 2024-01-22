// 전처리기
// #으로 시작하고 ;을 붙이지 않음
// include
// define 
// pragma once 등

#pragma once
#include <ws2tcpip.h>
#include <WinSock2.h>
#include <iostream>
#include <stdio.h>


#define DATA_BUFSIZE 2048

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