#ifndef __APP_INITIALZE_H__
#define __APP_INITIALZE_H__

#include "Observer.h"

//�����ܻ����ʵ���Զ�����������Initialze()����
class AppBase
{
public:
	AppBase(){}
	virtual ~AppBase(){}
	virtual bool Initialze() = 0;
};

class ServerAppSubject : public Subject
{
public:
	virtual void Attach(Observer* obj);
	virtual void Detach(Observer* obj);
private:
	std::list<Observer*> m_Initialze;	//��ʼ���б�
};

#endif