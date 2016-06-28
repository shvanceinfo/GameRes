#ifndef __MAIL_H__
#define __MAIL_H__

#include "AppInitialze.h"

class MaillManager : public AppBase
{
public:
	static MaillManager* GetInstance()
	{
		static MaillManager m_Instance;
		return &m_Instance;
	}

	virtual bool Initialze();
	void RegisterMessage();
	//��ȡ�ʼ���Ϣ
	void HandleGetMailList(int cmd, int connection, std::string recvMsg);
private:
	MaillManager(){}
	~MaillManager(){}
};
#define g_Mail MaillManager::GetInstance()

#endif