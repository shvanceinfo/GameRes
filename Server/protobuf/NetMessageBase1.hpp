#ifndef __NET_MESSAGE_BASE_H__
#define __NET_MESSAGE_BASE_H__

#include "iostream"
#include "string.h"

namespace NetMessage{
	template <typename T, typename ... Args>
	T Func(Args ... args)
	{
		T t;
		return t;
	};

	class NetMessageBasic
	{
	public:
		virtual bool Read(std::string&& out) = 0;
		virtual void Write(std::string& in) = 0;
	};
}


#endif