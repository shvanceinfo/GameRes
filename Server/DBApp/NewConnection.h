#ifndef __NEW_CONNCTION_H__
#define __NEW_CONNCTION_H__

/************************************************************************/
/*                         ���ӹ�����                                   */
/************************************************************************/

#include "MySqlConnection.h"
#include "tbb44_20160413oss/include/tbb/concurrent_queue.h"

namespace DBConnection
{
	//ÿ�����Ӵ�����һ����
	class Connection
	{
	public:
		Connection(std::string ip, uint16_t port, std::string account, std::string pass, std::string name) :
			ip_(ip), port_(port), account_(account), pass_(pass), name_(name){}

		//��ȡ����
		MySqlConnection* GetSqlConnection()
		{
			MySqlConnection* conn = nullptr;
			if (false == que.try_pop(conn))
			{
				conn = new MySqlConnection();
				if (true == conn->init(ip_, port_, account_, pass_, name_))
				{
					return conn;
				}

				return nullptr;
			}

			return conn;
		}

		//�ͷ�����
		void PushConnection(MySqlConnection* conn)
		{
			que.push(conn);
		}

	private:
		//ÿ������Ӧ��sql����
		tbb::concurrent_queue<MySqlConnection*> que;
		std::string ip_;
		uint16_t port_;
		std::string account_;
		std::string pass_;
		std::string name_;
	};

	template<class T>
	class ConnectionMgr
	{
	public:
		void PushConnction(uint16_t index, T* conn)
		{
			m_Connections[index] = conn;
		}

		T* GetConnction(uint16_t index)
		{
			auto itr = m_Connections.find(index);
			if (itr == m_Connections.end())
			{
				return nullptr;
			}

			return itr->second;
		}

	private:
		std::unordered_map<uint16_t, T*> m_Connections;
	};
}
#endif