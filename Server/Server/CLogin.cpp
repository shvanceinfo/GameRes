#include "AppManager.h"
#include "NetMessage.pb.h"
#include "MessageDispatch.hpp"
#include "CLogin.h"
#include "NetFrame.h"
#include "GameClient.hpp"
#include "AppInitialze.h"

bool CLogin::Initialze()
{
	RegisterMessage();

	return true;
}

void CLogin::RegisterMessage()
{
	REGISTER_MH(NetPackage::CeC2GType::C2G_Login, BIND_FUNC(CLogin::HandleLogin, g_Login));
	REGISTER_MH(NetPackage::CeC2GType::C2G_CreateRole, BIND_FUNC(CLogin::HandleCreateRole, g_Login));
}

int CLogin::HandleLogin(int cmd, int connection, std::string recvMsg)
{
	NetPackage::CRequestLogin login;

	if (false == login.ParseFromString(recvMsg))
	{
		return 0;
	}

	NetPackage::CRequestLoginRet loginRet;
	loginRet.set__username(login._username());

	g_AppManager->SendClient(connection, NetPackage::CeG2CType::G2C_LoginRet, &loginRet);

// 	NetPackage::CGSNotifyRoleBaseInfo roleRet;
// 	roleRet.set_m_un32objid(1);
// 	roleRet.set_m_n32rolenickname("ÖÐ¹úÈË");
// 	roleRet.set_m_n32careerid(1);
// 	roleRet.set_m_bgender(1);
// 	roleRet.set_m_un32coattypeid(1);
// 	roleRet.set_m_un32legguardid(1);
// 	roleRet.set_m_un32legguardid(1);
// 	roleRet.set_m_un32shoeid(1);
// 	roleRet.set_m_un32necklaceid(1);
// 	roleRet.set_m_un32ringid(1);
// 	roleRet.set_m_un32weaponid(1);
// 	roleRet.set_m_un32level(1);
// 	roleRet.set_m_un32exp(1);
// 	roleRet.set_m_un32curhp(1);
// 	roleRet.set_m_un32curmp(1);
// 	roleRet.set_m_un32curhpvessel(1);
// 	roleRet.set_m_un32curmpvessel(1);
// 	roleRet.set_m_un32maxhp(100);
// 	roleRet.set_m_un32maxmp(100);
// 	roleRet.set_m_un32maxhpvessel(100);
// 	roleRet.set_m_un32maxmpvessel(100);
// 	roleRet.set_m_un32max_packages(1);
// 	roleRet.set_wingid(1);
// 
// 	g_AppManager->SendClient(connection, NetPackage::CeG2CType::G2C_RoleList, &roleRet);
	return 1;
}

int CLogin::HandleCreateRole(int cmd, int connection, std::string recvMsg)
{
	NetPackage::CGCSendCreateRole login;

	if (false == login.ParseFromString(recvMsg))
	{
		return 0;
	}

// 	NetPackage::CRequestLoginRet loginRet;
// 	loginRet.set__username(login._username());
// 
// 	g_AppManager->SendClient(connection, NetPackage::CeG2CType::G2C_LoginRet, &loginRet);
	return 1;
}