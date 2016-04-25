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

	return false;
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
		return true;
	}

	NetPackage::CRequestLoginRet ret;
	ret.set__username(login._username());
	std::string out_data = ret.SerializeAsString();

	g_AppManager->SendClient(connection, out_data);
	return 1;
}