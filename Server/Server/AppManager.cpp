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
	m_EchoServer->PushSendMessage(conn, const_cast<char*>(sendMsg.c_str()));
}

void AppManager::SetEchoServere(uv::TCPServer* echo)
{
	m_EchoServer = echo;
}