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

	//玩家登陆
	void HandleLogin(int cmd, int connection, std::string recvMsg);
	//玩家创角
	void HandleCreateRole(int cmd, int connection, std::string recvMsg);
	//全用角色
	void HandleSelectRole(int cmd, int connection, std::string recvMsg);
private:
	CLogin(){};
	~CLogin(){}
};

#define g_Login CLogin::GetInstance()
#endif