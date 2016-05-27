#ifndef __APP_MANAGER_H__
#define __APP_MANAGER_H__

#include <stdint.h>
#include "NetFrame.h"
#include "NewConnection.h"
#include "DatabaseThreadPool.h"

struct TaskInfo
{
public:
	TaskInfo(){};
	TaskInfo(std::shared_ptr<DBConnection::Connction<MySqlConnection>> conn_, std::string && cmd_) : conn(conn_), cmd(cmd_){}
	std::shared_ptr<DBConnection::Connction<MySqlConnection>> conn = nullptr;
	std::string cmd = "";
};

class AppManager
{
protected:
	AppManager(){}
	~AppManager(){}
private:
	AppManager(const AppManager&) = delete;
	AppManager& operator=(const AppManager&) = delete;

public:
	enum STATUS
	{
		NONE = 0,
		RUNNING,
		CLOSE,
	};
	static AppManager* GetInstance();
	bool Run();
	void Close();
	void StartDB();
	bool LoadConfigValue();
	void NewConnction(uint16_t conn);
	void RecivedMessage(std::shared_ptr<uv::MessageInfo> value);
	void OnUpdate();
	STATUS GetStatus(){ return status; }
private:
	
	static AppManager*	m_Instance;
	STATUS	status	= NONE;											//服务器状态
	std::unordered_map<uint32_t, bool>				m_UserStatus;	//玩家当前指令执行情况：uint32_t为玩家UID或公共模块ID，bool为true时表明当前正在执行指令
	std::list<std::shared_ptr<uv::MessageInfo>>		m_MessageList;	//接收的消息数组
	DatabaseThreadPool<TaskInfo>					DBThreadPool;	//线程池
	DBConnection::ConnectionMgr<MySqlConnection>	TaskMgr;
};
#define g_AppMgr AppManager::GetInstance()
#endif