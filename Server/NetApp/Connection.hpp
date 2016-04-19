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
	void UserLogin(uint32_t conn){}
	//�������
	void UserLoginOut(uint32_t conn){}
	//��ҽ�����Ϣ
	void RecivedMessage(uint32_t conn, char *buf)
	{
		if (buf == nullptr)
		{
			this->UserLoginOut(conn);
			return;
		}

		//������ͷ
		NetPackage::CNetHead header;
		if (false == header.ParsePartialFromString(buf))
		{
			return;
		}

		DISPATCH_MH(header._assistantcmd(), conn, header._body());
	}
	//��ҷ�����Ϣ
	void SendClient(uint32_t conn, std::string sendMsg)
	{
		m_EchoServer->PushSendMessage(conn, const_cast<char*>(sendMsg.c_str()));
	}
	//ͨ������ID��ȡClient
	std::shared_ptr<T> GetClientByConnection(uint32_t conn){}
	//ͨ�����ΨһID��ȡClient
	std::shared_ptr<T> GetClientByUserId(uint64_t userId){}
	//ʱ���������
	void OnUpdate(){}
	//
	void SetEchoServere(uv::TCPServer* echo)
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
	uv::TCPServer* m_EchoServer;
};

#define g_AppManager(T) AppManager<T>::GetInstance()

#endif