#ifndef __OBSERVER_H__
#define __OBSERVER_H__

#include "iostream"
#include "list"
class Subject;

class Observer
{
public:
	virtual ~Observer();
	virtual void Update(Subject *) = 0;
protected:
	Observer();
};

class Subject
{
public:
	virtual ~Subject(){}
	virtual void Notify();
	virtual void Attach(Observer*);
	virtual void Detch(Observer*);
protected:
	Subject(){}
};

#endif