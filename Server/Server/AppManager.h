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
	//App起动
	bool StartDemo();
	//玩家登陆
	void UserLogin(uint32_t conn){}
	//玩家下线
	void UserLoginOut(uint32_t conn){}
	//玩家接收消息
	void RecivedMessage(uint32_t conn, char *buf);
	//玩家发送消息
	void SendClient(uint32_t conn, std::string sendMsg);
	void SendClient(uint32_t conn, uint32_t cmd, void* ptrMsg);
	//通过连接ID获取Client
	//std::shared_ptr<T> GetClientByConnection(uint32_t conn){}
	//通过玩家唯一ID获取Client
	//std::shared_ptr<T> GetClientByUserId(uint64_t userId){}
	//时间迭代更新
	void OnUpdate(){}
	//
	void SetEchoServere(uv::TCPServer* echo);
private:
	//所有玩家已接收消息队列:TODO由于现在数据有点问题，所以暂时不开启环形对列
	std::unordered_map<uint32_t, std::shared_ptr<MsgBlock>>	m_UserRecvMessage;
	//所有在线玩家列表:uint32为连接ID,T为Client类型
	//std::unordered_map<uint32_t, std::shared_ptr<T>> m_ClientConnection;
	//所有在线玩家列表:uint64_t为玩家唯一ID，T为Client类型
	//std::unordered_map<uint64_t, std::shared_ptr<T>> m_ClientUserId;
	//保存boost::shared_ptr<EchoServer>
	uv::TCPServer* m_EchoServer;
};

#define g_AppManager AppManager::GetInstance()

#endif