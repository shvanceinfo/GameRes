#include "MessageDispatch.hpp"
#include "NetMessage.pb.h"
#include "CLogin.hpp"
#include "NetFrame.h"
#include "Connection.hpp"
#include "GameClient.hpp"

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
		return true;
	}

	NetPackage::CRequestLoginRet ret;
	ret.set__username(login._username());
	std::string out_data = ret.SerializeAsString();

	g_AppManager(abcTest)->SendClient(connection, out_data);
	return 1;
}