#include "AppManager.h"
#include "NetMessage.pb.h"
#include "MessageDispatch.hpp"

bool MessageHead::Parse(char* msg, uint32_t len)
{
	if (len <= (sizeof(len)+sizeof(cmd)))
	{
		return false;
	}

	memcpy(&len, msg, sizeof(len));
	memcpy(&cmd, msg + sizeof(len), sizeof(cmd));
	
	int len_body = len - sizeof(len) + sizeof(cmd) + 1;
	body = new char[len_body];
	memset(body, 0, len_body);

	memcpy(body, msg + sizeof(len)+sizeof(cmd), len_body - 1);

	return true;
}

char* MessageHead::Pack()
{
	char* msg = new char[sizeof(cmd) + sizeof(len) + ]
}

void AppManager::RecivedMessage(uint32_t conn, char *buf, uint32_t len)
{
	if (buf == nullptr || len == 0)
	{
		this->UserLoginOut(conn);
		return;
	}

	MessageHead head;
	if (false == head.Parse(buf, len))
	{
		return;
	}

	//½âÎö°üÍ·
// 	NetPackage::CNetHead header;
// 	if (false == header.ParsePartialFromString(buf))
// 	{
// 		return;
// 	}

	DISPATCH_MH(head.cmd, conn, head.body);
}

void AppManager::SendClient(uint32_t conn, std::string sendMsg)
{
	char* msg = (char*)malloc(sendMsg.length() + 1);
	memset(msg, 0, sendMsg.length() + 1);
	memcpy_s(msg, sendMsg.length(), sendMsg.c_str(), sendMsg.length());
	std::shared_ptr<char*> ptrMsg = std::make_shared<char*>(msg);
	m_EchoServer->PushSendMessage(conn, *ptrMsg, sendMsg.length());
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

	m_EchoServer->PushSendMessage(conn, *oMsg, oHead.length());
}

void AppManager::SetEchoServere(uv::TCPServer* echo)
{
	m_EchoServer = echo;
}