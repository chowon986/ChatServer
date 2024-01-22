#include "chat_server.h"

int main()
{	
	ChatServer server;

	server.Init();

	server.Start(12345);
}
