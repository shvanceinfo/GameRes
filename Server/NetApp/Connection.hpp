#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include "NetFrame.h"
#include "MessageDispatch.hpp"
#include "NetMessage.pb.h"
#include "Observer.hpp"
#include "..\Server\CLogin.hpp"

#define BLOCK_RING_BUFFER_SIZE 1024 * 64
struct MsgBlock
{
public:
	//��ȡ����
	bool Read(char* out);
	//д����
	int Write(char* in);
private:
	//�������ݳ���
	uint32_t total = 0;
	//��λ��
	char *pRead = nullptr;
	//дλ��
	char *pWrite = nullptr;
	//��������
	unsigned char buffer[BLOCK_RING_BUFFER_SIZE];
};

class ServerAppSubject : public Subject
{
public:
	virtual void Attach(Observer* obj);
	virtual void Detach(Observer* obj);
private:
	std::list<Observer*> m_Initialze;	//��ʼ���б�
};

template<typename T>
class AppManager : public ServerAppSubject
{
public:
	static AppManager* GetInstance()
	{
		static AppManager m_instance;
		return &m_instance;
	}
protected:
	AppManager(){}
	~AppManager(){}
private:
	AppManager(const AppManager&) = delete;
	AppManager& operator=(const AppManager&) = delete;

public:
	//App��
	bool StartDemo()
	{
		if (false == g_Login->Initialze())
		{
			return false;
		}
		return true;
	}
	//��ҵ�½
	void UserLogin(uint32_t userId){}
	//�������
	void UserLoginOut(uint32_t userId){}
	//��ҽ�����Ϣ
	void RecivedMessage(std::shared_ptr<NETAPP::ClientMessage> recvMsg)
	{
		if (recvMsg->buffer == nullptr)
		{
			this->UserLoginOut(recvMsg->clientID);
			return;
		}

		//������ͷ
		NetPackage::CNetHead header;
		if (false == header.ParsePartialFromString(*recvMsg->buffer))
		{
			return;
		}

		DISPATCH_MH(header._assistantcmd(), recvMsg->clientID, header._body());
	}
	//��ҷ�����Ϣ
	void SendClient(uint32_t connection, std::string sendMsg)
	{
		std::shared_ptr<NETAPP::ClientMessage> msg(new NETAPP::ClientMessage);
		msg->clientID = connection;
		msg->buffer = std::make_shared<std::string>(sendMsg);

		m_EchoServer->AddSendMssage(msg);
	}
	//ͨ������ID��ȡClient
	std::shared_ptr<T> GetClientByConnection(uint32_t connectionID){}
	//ͨ�����ΨһID��ȡClient
	std::shared_ptr<T> GetClientByUserId(uint64_t userId){}
	//ʱ���������
	void OnUpdate(){}
	//
	void SetEchoServere(boost::shared_ptr<NETAPP::EchoServer> echo)
	{
		m_EchoServer = echo;
	}
private:
	//��������ѽ�����Ϣ����:TODO�������������е����⣬������ʱ���������ζ���
	std::unordered_map<uint32_t, std::shared_ptr<MsgBlock>>	m_UserRecvMessage;
	//������������б�:uint32Ϊ����ID,TΪClient����
	std::unordered_map<uint32_t, std::shared_ptr<T>> m_ClientConnection;
	//������������б�:uint64_tΪ���ΨһID��TΪClient����
	std::unordered_map<uint64_t, std::shared_ptr<T>> m_ClientUserId;
	//����boost::shared_ptr<EchoServer>
	boost::shared_ptr<NETAPP::EchoServer> m_EchoServer;
};

#define g_AppManager(T) AppManager<T>::GetInstance()

#endif