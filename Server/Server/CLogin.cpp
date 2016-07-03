#include "AppManager.h"
#include "NetMessage.pb.h"
#include "MessageDispatch.hpp"
#include "CLogin.h"
#include "NetFrame.h"
#include "GameClient.hpp"

std::unordered_map<int, NetPackage::CGCSendCreateRole> m_UserList;

DEFINE_INSTANCE(CLogin)

bool CLogin::Initialze()
{
	RegisterMessage();

	return true;
}

void CLogin::RegisterMessage()
{
	REGISTER_MH(NetPackage::CeC2GType::C2G_Login, CLogin::HandleLogin, this);
	REGISTER_MH(NetPackage::CeC2GType::C2G_CreateRole, CLogin::HandleCreateRole, this);
	REGISTER_MH(NetPackage::CeC2GType::C2G_SelectRole, CLogin::HandleSelectRole, this);
}

void CLogin::HandleLogin(int cmd, int connection, std::string recvMsg)
{
	NetPackage::CRequestLogin inData;

	if (false == inData.ParseFromString(recvMsg))
	{
		return ;
	}

	NetPackage::CRequestLoginRet loginRet;
	loginRet.set__username(inData._username());

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
}

void CLogin::HandleCreateRole(int cmd, int connection, std::string recvMsg)
{
	NetPackage::CGCSendCreateRole inData;

	if (false == inData.ParseFromString(recvMsg))
	{
		return ;
	}

	m_UserList[connection] = inData;

	NetPackage::CGCSendCreateRoleRet loginRet;
	loginRet.set__result(0);

	g_AppManager->SendClient(connection, NetPackage::CeG2CType::G2C_RoleCreateRet, &loginRet);

	NetPackage::CGSNotifyRoleBaseInfo roleRet;
	roleRet.set_m_un32objid(1);
	roleRet.set_m_n32rolenickname(inData._nickname());
	roleRet.set_m_n32careerid(inData._u32vocationid());
	roleRet.set_m_bgender(inData._bytegender());
	roleRet.set_m_un32coattypeid(1);
	roleRet.set_m_un32legguardid(1);
	roleRet.set_m_un32legguardid(1);
	roleRet.set_m_un32shoeid(1);
	roleRet.set_m_un32necklaceid(1);
	roleRet.set_m_un32ringid(1);
	roleRet.set_m_un32weaponid(1);
	roleRet.set_m_un32level(1);
	roleRet.set_m_un32exp(1);
	roleRet.set_m_un32curhp(1);
	roleRet.set_m_un32curmp(1);
	roleRet.set_m_un32curhpvessel(1);
	roleRet.set_m_un32curmpvessel(1);
	roleRet.set_m_un32maxhp(100);
	roleRet.set_m_un32maxmp(100);
	roleRet.set_m_un32maxhpvessel(100);
	roleRet.set_m_un32maxmpvessel(100);
	roleRet.set_m_un32max_packages(1);
	roleRet.set_wingid(1);

	g_AppManager->SendClient(connection, NetPackage::CeG2CType::G2C_RoleList, &roleRet);
}

void CLogin::HandleSelectRole(int cmd, int connection, std::string recvMsg)
{
	NetPackage::CGCAskSelectRole inData;

	if (false == inData.ParseFromString(recvMsg))
	{
		return ;
	}

	NetPackage::CGSNotifyChangeScene outData;
	outData.set_m_un32mapid(100000001);
	outData.set_m_un32sceneid(1);
	outData.set_m_un32clientno(19);
	outData.set_m_fposx(-2.12f);
	outData.set_m_fposy(-0.82f);
	outData.set_m_fposz(-0.01f);

	g_AppManager->SendClient(connection, NetPackage::CeG2CType::G2C_RoleChangeScene, &outData);
}