// DBApp.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "NetFrame.h"
#include "AppManager.h"

int _tmain(int argc, _TCHAR* argv[])
{
	if (false == g_AppMgr->Run())
	{
		g_AppMgr->CloseDB();
		return -1;
	}

	uv::TCPServer netServer("127.0.0.1", 9998);
	netServer.Run();
	
	for (;;)
	{
		auto msg = netServer.GetRecvMessage();
 		if (nullptr != msg)
 		{
 			g_AppMgr->RecivedMessage(msg->conn, msg->buf, msg->len);
 		}
		g_AppMgr->OnUpdate();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	netServer.Close();
	g_AppMgr->CloseDB();
	return 0;
}

