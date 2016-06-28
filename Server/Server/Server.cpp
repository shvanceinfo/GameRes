// Server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "NetFrame.h"
#include "AppManager.h"

//////////////////////////////////////////////////////////////////////////
//如果NetFrame报错，则将NetFrame放在stdafx下面
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//libuv起草的http
//https://github.com/liigo/tinyweb/blob/master/tinyweb3.c
//适合入门的tinyhttp源码
//https://github.com/cbsheng/tinyhttpd/blob/master/httpd.c
//http-parser使用简介
//http://blog.rootk.com/post/tutorial-for-http-parser.html
//////////////////////////////////////////////////////////////////////////

int _tmain(int argc, _TCHAR* argv[])
{
	if (false == g_AppManager->Run())
	{
		return -1;
	}

	uv::TCPServer netServer("127.0.0.1", 9999);
	netServer.Run();

	//uv::TCPClient netClient(1, "127.0.0.1", 9998);
	//netClient.Run();

	g_AppManager->SetEchoServere(&netServer);

	while (true)
 	{
		if (AppManager::STATUS::CLOSED == g_AppManager->GetStatus())
		{
			break;
		}

		auto msg = netServer.GetRecvMessage();
		if (msg != nullptr)
		{
			g_AppManager->RecivedMessage(msg->conn, msg->buf, msg->len);
		}
		g_AppManager->OnUpdate();

		std::this_thread::sleep_for(std::chrono::microseconds(10));
 	}

	netServer.Close();
	//netClient.Close();
	return 0;
}

