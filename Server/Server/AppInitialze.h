#ifndef __APP_INITIALZE_H__
#define __APP_INITIALZE_H__

#include "Observer.h"

#define DEFINE_DEMO_CLASS(T)	private:\
								static T* m_Instance;\
								T()\
								{\
									g_AppManager->AddDemos(this);\
								}\
								\
								~T(){}\
								\
								public:\
								static T* GetInstance()\
								{\
									return m_Instance;\
								}\
						
#define DEFINE_INSTANCE(T)		T* T::m_Instance = new T();

//各功能基类可实现自动挂载类似于Initialze()函数
class AppBase
{
public:
	AppBase(){}
	virtual ~AppBase(){}
	virtual bool Initialze() = 0;
	virtual void RegisterMessage(){};
};

class ServerAppSubject : public Subject
{
public:
	virtual void Attach(Observer* obj);
	virtual void Detach(Observer* obj);
private:
	std::list<Observer*> m_Initialze;	//初始化列表
};

#endif