#include "AppManager.h"
#include "NetMessage.pb.h"
#include "MessageDispatch.hpp"

void AppManager::RecivedMessage(uint32_t conn, char *buf)
{
	if (buf == nullptr)
	{
		this->UserLoginOut(conn);
		return;
	}

	//½âÎö°üÍ·
	NetPackage::CNetHead header;
	if (false == header.ParsePartialFromString(buf))
	{
		return;
	}

	DISPATCH_MH(header._assistantcmd(), conn, header._body());
}

void AppManager::SendClient(uint32_t conn, std::string sendMsg)
{
	char* msg = (char*)malloc(sendMsg.length() + 1);
	memset(msg, 0, sendMsg.length() + 1);
	memcpy_s(msg, sendMsg.length(), sendMsg.c_str(), sendMsg.length());
	std::shared_ptr<char*> ptrMsg = std::make_shared<char*>(msg);
	m_EchoServer->PushSendMessage(conn, *ptrMsg);
}

void AppManager::SendClient(uint32_t conn, uint32_t cmd, void* ptrMsg)
{
	google::protobuf::Message *pbody = (google::protobuf::Message *)ptrMsg;
	std::string oBody = "";

	if (pbody != nullptr)
	{
		oBody = pbody->SerializeAsString();
	}

	NetPackage::CNetHead header;
	header.set__assistantcmd(cmd);
	header.set__length(oBody.length());
	header.set__body(oBody.c_str());

	std::string oHead = header.SerializeAsString();
	char* msg = (char*)malloc(oHead.length() + 1);
	memset(msg, 0, oHead.length() + 1);
	memcpy_s(msg, oHead.length(), oHead.c_str(), oHead.length());
	std::shared_ptr<char*> oMsg = std::make_shared<char*>(msg);

	m_EchoServer->PushSendMessage(conn, *oMsg);
}

void AppManager::SetEchoServere(uv::TCPServer* echo)
{
	m_EchoServer = echo;
}