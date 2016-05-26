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

template<typename T>
class DatabaseThreadPool
{
private:
	ProducerConsumerQueue<T> MessageQueue;
	std::list<std::thread*> ThreadPool;
	void run()
	{
		while (true)
		{
			T value;
			MessageQueue.WaitAndPop(value);
			
			if (value.conn == nullptr)
			{
				break;
			}

			auto conn = value.conn->GetSqlConnection();
			if (conn == nullptr)
			{
				continue;
			}

			if (false == conn->Query(std::move(value.cmd)))
			{
				
			}

			value.conn->PushConnection(conn);
		}
	}
public:
	void CreateThreadPool(uint16_t threadNum)
	{
		for (int i = 0; i < threadNum; i++)
		{
			std::thread *ptrThread(new std::thread(&DatabaseThreadPool::run, this));
			ThreadPool.push_back(ptrThread);
		}
	}

	void post(const T &value){ MessageQueue.Push(value); }
	void wait()
	{
		for (auto ptr = ThreadPool.begin(); ptr != ThreadPool.end(); ptr++)
		{
			(*ptr)->join();
		}
	}
	void ClosePool()
	{
		//利用空指针结束线程
		T value;
		for (uint32_t i = 0; i < ThreadPool.size(); i++)
		{
			MessageQueue.Push(value);
		}
	}
};

#endif