#ifndef __MESSAGE_DISPATCH_H__
#define __MESSAGE_DISPATCH_H__

#include <functional>
#include <unordered_map>

//int:�����,int:Client����ID, std::string:�����ַ�����
#define PARAMS int, int, std::string
#define REGISTER_MH(x, y) MessageDispatch<PARAMS>::GetInstance()->RegisterMessageHandle(x, y);
#define DISPATCH_MH(x, y, z) MessageDispatch<PARAMS>::GetInstance()->DispatchMessageHandle(x, x, y, z);

//��Ϣ�ַ���
template<typename ... Args>
class MessageDispatch
{
public:
	static MessageDispatch* GetInstance()
	{
		static MessageDispatch m_Instance;
		return &m_Instance;
	}

	void RegisterMessageHandle(int cmd, std::function<int(PARAMS)> func)
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
	std::unordered_map<int, std::function<int(PARAMS)>> m_Message;
};

#endif