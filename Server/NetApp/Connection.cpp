#include "Connection.hpp"


void ServerAppSubject::Attach(Observer* obj)
{
	m_Initialze.push_back(obj);
}

void ServerAppSubject::Detach(Observer* obj)
{
	auto ptr = std::find(m_Initialze.begin(), m_Initialze.end(), obj);
	if (ptr != m_Initialze.end())
	{
		m_Initialze.erase(ptr);
	}
}