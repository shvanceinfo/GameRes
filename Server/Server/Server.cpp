// Server.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "NetFrame.h"
#include "Connection.hpp"
#include "GameClient.hpp"

struct NETAPP::ClientMessage;

int _tmain(int argc, _TCHAR* argv[])
{
	NETAPP::SetupSignalHandler();
	NETAPP::IOServeicePtr io_service(new boost::asio::io_service());
	NETAPP::EchoServerPtr echo_server(new NETAPP::EchoServer(io_service));
 
 	if (!echo_server->Start("127.0.0.1", 9999))
 	{
 		return -1;
 	}

	auto AppMagr = AppManager<abcTest>::GetInstance();
	AppMagr->SetEchoServere(echo_server);
	if (false == AppMagr->StartDemo())
	{
		return -1;
	}
	
 	boost::thread_group asio_threads;
	asio_threads.create_thread(boost::bind(NETAPP::AsioThreadMain, io_service));
 
	while (!NETAPP::g_shutdown_server)
 	{
		std::shared_ptr<NETAPP::ClientMessage> recvMsg = nullptr;
		if (true == echo_server->PopRecvMessage(recvMsg))
		{
			AppMagr->RecivedMessage(recvMsg);
		}
		AppMagr->OnUpdate();
		std::this_thread::sleep_for(std::chrono::microseconds(10));
 	}
 	echo_server->Stop();
 	asio_threads.join_all();
	return 0;
}

