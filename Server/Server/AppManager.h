#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <iostream>
#include <memory>
#include <unordered_map>
#include "NetFrame.h"

#define BLOCK_RING_BUFFER_SIZE 1024 * 64

class uv::TCPServer;

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

class MessageHead
{
public:
	uint16_t len = 0;
	uint16_t cmd = 0;
	char* body = nullptr;

	bool Parse(char* msg, uint16_t length);
	char* Pack(const char* msg, uint16_t length, uint16_t cmdIn);
};

class AppManager
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
	bool StartDemo();
	//��ҵ�½
	void UserLogin(uint32_t conn){}
	//�������
	void UserLoginOut(uint32_t conn){}
	//��ҽ�����Ϣ
	void RecivedMessage(uint32_t conn, char *buf, uint32_t len);
	//��ҷ�����Ϣ
	void SendClient(uint32_t conn, std::string sendMsg);
	void SendClient(uint32_t conn, uint32_t cmd, void* ptrMsg);
	//ͨ������ID��ȡClient
	//std::shared_ptr<T> GetClientByConnection(uint32_t conn){}
	//ͨ�����ΨһID��ȡClient
	//std::shared_ptr<T> GetClientByUserId(uint64_t userId){}
	//ʱ���������
	void OnUpdate(){}
	//
	void SetEchoServere(uv::TCPServer* echo);
private:
	//��������ѽ�����Ϣ����:TODO�������������е����⣬������ʱ���������ζ���
	std::unordered_map<uint32_t, std::shared_ptr<MsgBlock>>	m_UserRecvMessage;
	//������������б�:uint32Ϊ����ID,TΪClient����
	//std::unordered_map<uint32_t, std::shared_ptr<T>> m_ClientConnection;
	//������������б�:uint64_tΪ���ΨһID��TΪClient����
	//std::unordered_map<uint64_t, std::shared_ptr<T>> m_ClientUserId;
	//����boost::shared_ptr<EchoServer>
	uv::TCPServer* m_EchoServer;
};

#define g_AppManager AppManager::GetInstance()

#endif