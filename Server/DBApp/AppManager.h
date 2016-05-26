#ifndef __APP_MANAGER_H__
#define __APP_MANAGER_H__

#include <stdint.h>
#include "NewConnection.h"
#include "DatabaseThreadPool.h"

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
	bool LoadConfigValue();
	void StartDB();
	void NewConnction();
	void Close();
	void RecivedMessage(uint32_t conn, char *buf, uint32_t len);
	void OnUpdate(){};
	STATUS GetStatus(){ return status; }
private:
	
	DatabaseThreadPool<TaskInfo>	DBThreadPool;
	DBConnection::ConnectionMgr<DBConnection::Connection>	TaskMgr;
	static AppManager* m_Instance;
	STATUS status = NONE;
};
#define g_AppMgr AppManager::GetInstance()
#endif