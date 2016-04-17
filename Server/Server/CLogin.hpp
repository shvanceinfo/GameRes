#ifndef __CLOGIN_H__
#define __CLOGIN_H__
#include <iostream>
#include <string>

class CLogin
{
public:
	static CLogin* GetInstance()
	{
		static CLogin m_Instance;
		return &m_Instance;
	}
	bool Initialze();
	void RegisterMessage();

	int HandleLogin(int cmd, int connection, std::string recvMsg);

private:
	CLogin(){};
	~CLogin(){}
};
#define g_Login CLogin::GetInstance()
#endif