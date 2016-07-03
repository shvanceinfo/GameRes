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
	//��ȡ�ʼ���Ϣ
	void HandleGetMailList(int cmd, int connection, std::string recvMsg);
};
#define g_Mail MaillManager::GetInstance()

#endif