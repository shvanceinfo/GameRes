// DBApp.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "AppManager.h"
#include "NetFrame.h"

int _tmain(int argc, _TCHAR* argv[])
{
	if (false == g_AppMgr->Run())
	{
		g_AppMgr->Close();
		return -1;
	}

	uv::TCPServer netServer("127.0.0.1", 9998);
	netServer.Run();

	while (true)
	{
		if (AppManager::STATUS::CLOSE == g_AppMgr->GetStatus())
		{
			netServer.Close();
			g_AppMgr->Close();
			break;
		}

		auto msg = netServer.GetRecvMessage();
 		if (nullptr != msg)
 		{
 			g_AppMgr->RecivedMessage(msg);
		}
		else
		{
			std::shared_ptr<uv::MessageInfo> msg1(new uv::MessageInfo(1,"222",3));
			g_AppMgr->RecivedMessage(msg1);
		}
		g_AppMgr->OnUpdate();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	return 0;
}

