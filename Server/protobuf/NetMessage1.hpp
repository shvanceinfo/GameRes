#ifndef __NET_MESSAGE_H__
#define __NET_MESSAGE_H__
#include "iostream"
#include "NetMessageBase.hpp"

class NetMessageHead : public NetMessage::NetMessageBasic
{
public:
	uint32_t length = 0;
	uint32_t cmd = 0;
public:
	virtual bool Read(std::string && out)
	{
		int a = 10;
		std::string temp = NetMessage::Func<std::string, std::string, int&, int, int>(std::move(std::string("abc")), a, 20, 30);
		return true;
	}

	virtual void Write(std::string& in)
	{
		//TODO
	}
};

class RequestLogin : public NetMessage::NetMessageBasic
{
public:
	NetMessageHead header;
	std::string _userName = "";
	std::string _userPassword = "";
	std::string _userGUID = "";
	std::string _deviceToken = "";
	bool _reConnect = "";

	virtual bool Read(std::string && out){ return true; }
	virtual void Write(std::string& in){}
};


#endif