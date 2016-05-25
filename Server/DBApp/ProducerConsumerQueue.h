#ifndef ___PRODUCER_QUEUE_H___
#define ___PRODUCER_QUEUE_H___

#include <condition_variable>
#include <mutex>
#include <queue>
#include <atomic>
#include <type_traits>
#include "NewConnection.h"

struct TaskInfo
{
public:
	TaskInfo(){};
	TaskInfo(DBConnection::Connection* conn_, std::string && cmd_) : conn(conn_), cmd(cmd_){}
	DBConnection::Connection* conn = nullptr;
	std::string cmd = "";
};

template<typename T>
class ProducerConsumerQueue
{
public:
	void Push(const T &value)
	{
		std::lock_guard<std::mutex> lock(_queueLock);
		_queue.push(value);
		_condition.notify_one();
	}

	int Empty()
	{
		std::lock_guard<std::mutex> lock(_queueLock);
		return _queue.empty();
	}

	int Pop(T &value)
	{
		std::lock_guard<std::mutex> lock(_queueLock);
		if (_queue.empty() || _shutDown)
		{
			return false;
		}

		value = _queue.front();
		_queue.pop();

		return true;
	}

	void WaitAndPop(T &value)
	{
		std::unique_lock<std::mutex> lock(_queueLock);
		_condition.wait(lock);
		if (_queue.empty() || _shutDown)
		{
			return ;
		}

		value = _queue.front();
		_queue.pop();
	}

private:
	std::mutex _queueLock;
	std::queue<T> _queue;
	std::condition_variable _condition;
	std::atomic<int> _shutDown;
};

#endif