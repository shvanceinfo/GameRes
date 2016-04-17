#ifndef ___DATA_BASE_THREAD_POOL___
#define ___DATA_BASE_THREAD_POOL___

#include "ProducerConsumerQueue.h"
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <mysql.h>
#include <concurrent_queue.h>
#include "MySqlConnection.h"

template<typename T>
class DatabaseThreadPool : boost::noncopyable
{
private:
	ProducerConsumerQueue<T> MessageQueue;
	boost::thread_group ThreadPool;

	void run(std::string ip, uint16_t port, std::string account, std::string pass, std::string name)
	{
		MySqlConnection<T>* ptrConnect = new MySqlConnection<T>(ip, port, account, pass, name);

		ptrConnect->init();

		while (true)
		{
			T value;
			MessageQueue.WaitAndPop(value);
			ptrConnect->Query(value);
		}
	}

public:

	void CreateMysqlConnection();

	void CreateThreadPool(int threadsNum, std::string ip, uint16_t port, std::string account, std::string pass, std::string name)
	{
		for (int i = 0; i < threadsNum; i++)
		{
			ThreadPool.add_thread(new boost::thread(boost::bind(&DatabaseThreadPool::run, this, ip, port, account, pass, name)));
		}
	}

	void post(const T &value){ MessageQueue.Push(value); }
	void wait()
	{
		ThreadPool.join_all();
	}
};

#endif