#ifndef ___DATA_BASE_THREAD_POOL___
#define ___DATA_BASE_THREAD_POOL___

/************************************************************************/
/* tbb使用教程						                                    */
/* http://onestraw.net/cprogram/tbb/                                    */
/* tbb下载							                                    */
/* https://www.threadingbuildingblocks.org/download                     */
/************************************************************************/

#include <list>
#include <thread>
#include <memory>
#include "MySqlConnection.h"
#include "ProducerConsumerQueue.h"

#define MysqlPtr(x)	std::shared_ptr<MySqlConnection<x>>

template<typename T>
class DatabaseThreadPool
{
private:
	ProducerConsumerQueue<T> MessageQueue;
	std::list<std::shared_ptr<std::thread>> ThreadPool;
	//std::concurrent_queue<int> MysqlPool;

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

	void CreateThreadPool(std::string ip, uint16_t port, std::string account, std::string pass, std::string name)
	{
		//for (int i = 0; i < threadsNum; i++)
		//{
		//	//shared_ptr<thread> ptrThread(new thread(std::bind(&DatabaseThreadPool::run, this, std::_Placeholder::_1, std::_Placeholder::_2, )))
		//	//ThreadPool.add_thread(new boost::thread(boost::bind(&DatabaseThreadPool::run, this, ip, port, account, pass, name)));
		//}
	}

	void post(const T &value){ MessageQueue.Push(value); }
	void wait()
	{
		for (auto itr = ThreadPool.begin(); itr != ThreadPool.end(); itr++)
		{
			(*itr)->join();
		}
	}
};

#endif