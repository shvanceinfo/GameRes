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
}

int CLogin::HandleLogin(int cmd, int connection, std::string recvMsg)
{
	NetPackage::CRequestLogin login;

	if (false == login.ParseFromString(recvMsg))
	{
		return 0;
	}

	NetPackage::CRequestLoginRet ret;
	ret.set__username(login._username());

	g_AppManager->SendClient(connection, NetPackage::CeG2CType::G2C_LoginRet, &ret);
	return 1;
}