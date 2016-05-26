#ifndef __APP_MANAGER_H__
#define __APP_MANAGER_H__

#include <stdint.h>
#include "NewConnection.h"
#include "DatabaseThreadPool.h"


class AppManager
{
public:
	static AppManager* GetInstance();
	bool Run();
	bool LoadConfigValue();
	void StartDB();
	void NewConnction();
	void CloseDB();
	void RecivedMessage(uint32_t conn, char *buf, uint32_t len);
	void OnUpdate(){};
private:
	
	DatabaseThreadPool<TaskInfo>	DBThreadPool;
	DBConnection::ConnectionMgr<DBConnection::Connection>	TaskMgr;
	static AppManager* m_Instance;
};
#define g_AppMgr AppManager::GetInstance()
#endif