// DBApp.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "NetFrame.h"
#include "DatabaseThreadPool.h"
#include "ProducerConsumerQueue.h"
#include "NewConnection.h"

using namespace std;

void Run(uv::TCPServer *server)
{
	server->tcp4_echo_start();
}

int _tmain(int argc, _TCHAR* argv[])
{
	uv::TCPClient ptrClient("127.0.0.1", 9999);

	if (0 != ptrClient.tcp4_echo_start())
	{
		return 0;
	}



	uv::TCPServer ptrServer("127.0.0.1", 9998);
	std::thread netApp(Run, &ptrServer);

	//g_AppManager->SetEchoServere(&ptrServer);
	DBConnection::ConnectionMgr<DBConnection::Connection> mgr;
	mgr.PushConnction(1, new DBConnection::Connection("127.0.0.1", 3306, "root", "root", "testdatabase"));
	DatabaseThreadPool<TaskInfo> pool;
	pool.CreateThreadPool(10);

	TaskInfo temp(mgr.GetConnction(1), std::string("select * from student"));
	for (;;)
	{
		//std::this_thread::sleep_for(std::chrono::milliseconds(10));
		//pool.post(temp);
	}

	pool.wait();
	getchar();
	return 0;
}

