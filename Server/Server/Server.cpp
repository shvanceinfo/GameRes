// Server.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "NetFrame.h"
#include "AppManager.h"
#include "GameClient.hpp"
#include <thread>

//////////////////////////////////////////////////////////////////////////
//���NetFrame������NetFrame����stdafx����
//////////////////////////////////////////////////////////////////////////

void Run(uv::TCPServer *server)
{
	server->tcp4_echo_start();
}

int _tmain(int argc, _TCHAR* argv[])
{
	uv::TCPServer ptrServer("127.0.0.1", 9999);
	std::thread netApp(Run, &ptrServer);
	g_AppManager->SetEchoServere(&ptrServer);
	
	if (false == g_AppManager->StartDemo())
	{
		netApp.join();
		return -1;
	}
	while (true)
 	{
		auto msg = ptrServer.GetRecvMessage();
		if (msg != nullptr)
		{
			g_AppManager->RecivedMessage(msg->conn, msg->buf, msg->len);
		}
		g_AppManager->OnUpdate();
		std::this_thread::sleep_for(std::chrono::microseconds(10));
 	}

	netApp.join();
	return 0;
}

