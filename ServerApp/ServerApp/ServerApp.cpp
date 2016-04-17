// ServerApp.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "NetSocket.h"
#include "iostream"
#include "thread"
//http://blog.csdn.net/wqvbjhc/article/details/25923481

void testClient()
{
	uv::TCPServer* server = new uv::TCPServer();
	server->Start("127.0.0.1", 7566);
}

int _tmain(int argc, _TCHAR* argv[])
{
// 	std::string straddr = "0.0.0.0";
// 	LIBUV::NetInterFace *socket = LIBUV::NetInterFace::GetInstance();
// 	socket->Start(straddr, 7566);

	std::thread tcpServer(testClient);

	while (true)
	{

	}

	printf("endl");
	getchar();
	return 0;
}

