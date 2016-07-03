#ifndef __CLOGIN_H__
#define __CLOGIN_H__
#include <iostream>
#include <string>
#include "AppInitialze.h"
#include "AppManager.h"

class CLogin : public AppBase
{
	DEFINE_DEMO_CLASS(CLogin)
public:
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
};

#define g_Login CLogin::GetInstance()
#endif