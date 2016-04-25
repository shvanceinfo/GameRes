// Server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "NetFrame.h"
#include "AppManager.h"
#include "GameClient.hpp"
#include <thread>

//struct NETAPP::ClientMessage;

void Run(uv::TCPServer *server)
{
	server->tcp4_echo_start();
}

int _tmain(int argc, _TCHAR* argv[])
{
	uv::TCPServer ptrServer("127.0.0.1", 9999);
	std::thread netApp(Run, &ptrServer);
	

	/*NETAPP::SetupSignalHandler();
	NETAPP::IOServeicePtr io_service(new boost::asio::io_service());
	NETAPP::EchoServerPtr echo_server(new NETAPP::EchoServer(io_service));
 
 	if (!echo_server->Start("127.0.0.1", 9998))
 	{
 		return -1;
 	}*/

	g_AppManager->SetEchoServere(&ptrServer);
	if (false == g_AppManager->StartDemo())
	{
		return -1;
	}
	
//  	boost::thread_group asio_threads;
// 	asio_threads.create_thread(boost::bind(NETAPP::AsioThreadMain, io_service));
 
	while (true)
 	{
		auto msg = ptrServer.GetRecvMessage();
		if (msg != nullptr)
		{
			g_AppManager->RecivedMessage(msg->conn, msg->buf);
		}
		g_AppManager->OnUpdate();
		std::this_thread::sleep_for(std::chrono::microseconds(10));
 	}
	netApp.join();
 	//asio_threads.join_all();
	return 0;
}

