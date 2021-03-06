#ifndef __MESSAGE_DISPATCH_H__
#define __MESSAGE_DISPATCH_H__

#include <functional>
#include <unordered_map>

//int:命令号,int:Client连接ID, std::string:接收字符数据
#define PARAMS int, int, std::string
#define BIND_FUNC(x, y) std::bind(&x, y, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
#define REGISTER_MH(x, y, z) MessageDispatch<PARAMS>::GetInstance()->RegisterMessageHandle(x, BIND_FUNC(y,z));
#define DISPATCH_MH(x, y, z) MessageDispatch<PARAMS>::GetInstance()->DispatchMessageHandle(x, x, y, z);

//消息分发器
template<typename ... Args>
class MessageDispatch
{
public:
	static MessageDispatch* GetInstance()
	{
		static MessageDispatch m_Instance;
		return &m_Instance;
	}

	void RegisterMessageHandle(int cmd, std::function<void(PARAMS)> func)
	{
		m_Message[cmd] = func;
	}
	
	bool DispatchMessageHandle(int cmd, Args ... args)
	{
		auto func = m_Message[cmd];
		if (nullptr == func)
		{
			return false;
		}

		func(args...);
		return true;
	}

protected:
	MessageDispatch(){}
	~MessageDispatch(){}

private:
	std::unordered_map<int, std::function<void(PARAMS)>> m_Message;
};

#endif