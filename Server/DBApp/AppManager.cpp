#include "AppManager.h"

AppManager* AppManager::m_Instance = nullptr;

bool AppManager::Run()
{
	if (false == LoadConfigValue())
	{
		return false;
	}

	StartDB();

	status = RUNNING;

	return true;
}

bool AppManager::LoadConfigValue()
{
	return true;
}

AppManager* AppManager::GetInstance()
{
	if (nullptr == m_Instance)
	{
		m_Instance = new AppManager();
	}

	return m_Instance;
}

void AppManager::RecivedMessage(uint32_t conn, char *buf, uint32_t len)
{

}

void AppManager::StartDB()
{
	DBThreadPool.CreateThreadPool(10);
	//TaskInfo temp(TaskMgr.GetConnction(1), std::string("select * from student"));
}

void AppManager::NewConnction()
{
	TaskMgr.PushConnction(1, new DBConnection::Connection("127.0.0.1", 3306, "root", "root", "testdatabase"));
}

void AppManager::Close()
{
	status = CLOSE;
	DBThreadPool.ClosePool();
	DBThreadPool.wait();
}