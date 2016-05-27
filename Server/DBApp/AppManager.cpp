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

void AppManager::RecivedMessage(std::shared_ptr<uv::MessageInfo> value)
{
	m_MessageList.push_back(value);
}

void AppManager::StartDB()
{
	DBThreadPool.CreateThreadPool(10);
}

void AppManager::NewConnction(uint32_t conn)
{
	TaskMgr.PushConnction(conn, std::shared_ptr<DBConnection::Connction<MySqlConnection>>
		(new DBConnection::Connction<MySqlConnection>("127.0.0.1", 3306, "root", "root", "testdatabase")));
}

void AppManager::Close()
{
	status = CLOSE;
	DBThreadPool.ClosePool();
	DBThreadPool.wait();
}

void AppManager::OnUpdate()
{
	for (auto itr = m_MessageList.begin(); itr != m_MessageList.end(); itr++)
	{
		auto stat = m_UserStatus.find(itr->get()->conn);
		if (stat == m_UserStatus.end() || true == stat->second)
		{
			continue;
		}

		//TODO将收到的消息结构体解析成sql语句
		auto conn = TaskMgr.GetConnction(stat->first);
		if (conn == nullptr)
		{
			continue;
		}

		TaskInfo temp(conn, std::string("select * from student"));
		stat->second = true;
	}
}