#ifndef __MAIL_H__
#define __MAIL_H__

#include "AppInitialze.h"
#include "AppManager.h"

class MaillManager : public AppBase
{
	DEFINE_DEMO_CLASS(MaillManager)
public:
	virtual bool Initialze();
	void RegisterMessage();
	//拉取邮件信息
	void HandleGetMailList(int cmd, int connection, std::string recvMsg);
};
#define g_Mail MaillManager::GetInstance()

#endif