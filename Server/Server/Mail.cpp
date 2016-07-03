#include "Mail.h"
#include "NetMessage.pb.h"

DEFINE_INSTANCE(MaillManager)

bool MaillManager::Initialze()
{
	return true;
}

void MaillManager::RegisterMessage()
{

}

void MaillManager::HandleGetMailList(int cmd, int connection, std::string recvMsg)
{
	NetPackage::CGSNotifyEMailList inData;

	if (false == inData.ParseFromString(recvMsg))
	{
		return;
	}
}