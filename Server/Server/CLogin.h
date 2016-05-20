#ifndef __CLOGIN_H__
#define __CLOGIN_H__
#include <iostream>
#include <string>
#include "AppInitialze.h"

class CLogin : public AppBase
{
public:
	static CLogin* GetInstance()
	{
		static CLogin m_Instance;
		return &m_Instance;
	}
	virtual bool Initialze();
	void RegisterMessage();

	//��ҵ�½
	int HandleLogin(int cmd, int connection, std::string recvMsg);
	//��Ҵ���
	int HandleCreateRole(int cmd, int connection, std::string recvMsg);
private:
	CLogin(){};
	~CLogin(){}
};

#define g_Login CLogin::GetInstance()
#endif