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
	void HandleLogin(int cmd, int connection, std::string recvMsg);
	//��Ҵ���
	void HandleCreateRole(int cmd, int connection, std::string recvMsg);
	//ȫ�ý�ɫ
	void HandleSelectRole(int cmd, int connection, std::string recvMsg);
	//��ȡ���桢�ͷ���Ϣ����ά����
	void HandleAskNotice(int cmd, int connection, std::string recvMsg);
	
	//�����ƶ�TODO
	//GCAskMove
	//������볡��TODO����Map������
	//GCAskEnterScene
private:
	CLogin(){};
	~CLogin(){}
};

#define g_Login CLogin::GetInstance()
#endif