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
	//读取数据
	bool Read(char* out);
	//写数据
	int Write(char* in);
private:
	//已有数据长度
	uint32_t total = 0;
	//读位置
	char *pRead = nullptr;
	//写位置
	char *pWrite = nullptr;
	//数据内容
	unsigned char buffer[BLOCK_RING_BUFFER_SIZE];
};

class ServerAppSubject : public Subject
{
public:
	virtual void Attach(Observer* obj);
	virtual void Detach(Observer* obj);
private:
	std::list<Observer*> m_Initialze;	//初始化列表
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
	//App起动
	bool StartDemo()
	{
		if (false == g_Login->Initialze())
		{
			return false;
		}
		return true;
	}
	//玩家登陆
	void UserLogin(uint32_t conn){}
	//玩家下线
	void UserLoginOut(uint32_t conn){}
	//玩家接收消息
	void RecivedMessage(uint32_t conn, char *buf)
	{
		if (buf == nullptr)
		{
			this->UserLoginOut(conn);
			return;
		}

		//解析包头
		NetPackage::CNetHead header;
		if (false == header.ParsePartialFromString(buf))
		{
			return;
		}

		DISPATCH_MH(header._assistantcmd(), conn, header._body());
	}
	//玩家发送消息
	void SendClient(uint32_t conn, std::string sendMsg)
	{
		m_EchoServer->PushSendMessage(conn, const_cast<char*>(sendMsg.c_str()));
	}
	//通过连接ID获取Client
	std::shared_ptr<T> GetClientByConnection(uint32_t conn){}
	//通过玩家唯一ID获取Client
	std::shared_ptr<T> GetClientByUserId(uint64_t userId){}
	//时间迭代更新
	void OnUpdate(){}
	//
	void SetEchoServere(uv::TCPServer* echo)
	{
		m_EchoServer = echo;
	}
private:
	//所有玩家已接收消息队列:TODO由于现在数据有点问题，所以暂时不开启环形对列
	std::unordered_map<uint32_t, std::shared_ptr<MsgBlock>>	m_UserRecvMessage;
	//所有在线玩家列表:uint32为连接ID,T为Client类型
	std::unordered_map<uint32_t, std::shared_ptr<T>> m_ClientConnection;
	//所有在线玩家列表:uint64_t为玩家唯一ID，T为Client类型
	std::unordered_map<uint64_t, std::shared_ptr<T>> m_ClientUserId;
	//保存boost::shared_ptr<EchoServer>
	uv::TCPServer* m_EchoServer;
};

#define g_AppManager(T) AppManager<T>::GetInstance()

#endif