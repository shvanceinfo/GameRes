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

	//玩家登陆
	void HandleLogin(int cmd, int connection, std::string recvMsg);
	//玩家创角
	void HandleCreateRole(int cmd, int connection, std::string recvMsg);
	//全用角色
	void HandleSelectRole(int cmd, int connection, std::string recvMsg);
	//拉取公告、客服信息、运维公告
	void HandleAskNotice(int cmd, int connection, std::string recvMsg);
	
	//请求移动TODO
	//GCAskMove
	//请求进入场景TODO放在Map类里面
	//GCAskEnterScene
};

#define g_Login CLogin::GetInstance()
#endif