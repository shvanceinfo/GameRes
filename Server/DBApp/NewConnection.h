#ifndef __NEW_CONNCTION_H__
#define __NEW_CONNCTION_H__

/************************************************************************/
/*                         ���ӹ�����                                   */
/************************************************************************/

#include <iostream>
#include <unordered_map>
#include "MySqlConnection.h"
#include "tbb44_20160413oss/include/tbb/concurrent_queue.h"

namespace DBConnection
{
	//ÿ�����Ӵ�����һ����
	template<typename T>
	class Connction
	{
	public:
		Connction(std::string ip, uint16_t port, std::string account, std::string pass, std::string name) :
			ip_(ip), port_(port), account_(account), pass_(pass), name_(name){}

		//��ȡ����
		ConnectionBase* GetSqlConnection()
		{
			ConnectionBase* conn = nullptr;
			if (false == que.try_pop(conn))
			{
				conn = new T();
				if (true == conn->init(ip_, port_, account_, pass_, name_))
				{
					return conn;
				}

				return nullptr;
			}

			return conn;
		}

		//�ͷ�����
		void PushConnection(ConnectionBase* conn)
		{
			que.push(conn);
		}

	private:
		//ÿ������Ӧ��sql����
		tbb::concurrent_queue<ConnectionBase*> que;
		std::string ip_;
		uint16_t port_;
		std::string account_;
		std::string pass_;
		std::string name_;
	};

	template<typename T>
	class ConnectionMgr
	{
	public:
		void PushConnction(uint16_t index, std::shared_ptr<Connction<T>*> conn)
		{
			m_Connections[index] = conn;
		}

		std::shared_ptr<Connction<T>*> GetConnction(uint16_t index)
		{
			auto itr = m_Connections.find(index);
			if (itr == m_Connections.end())
			{
				return nullptr;
			}

			return itr->second;
		}

	private:
		std::unordered_map<uint16_t, std::shared_ptr<Connction<T>*>> m_Connections;
	};
}
#endif