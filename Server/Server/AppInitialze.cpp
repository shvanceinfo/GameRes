#include "AppManager.h"
#include "AppInitialze.h"
#include "..\Server\CLogin.h"

AppBase* appObject[] = {
	g_Login,
	g_Login
};

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

bool AppManager::StartDemo()
{
	for (size_t index = 0; index < sizeof(appObject) / sizeof(AppBase*); index++)
	{
		if (false == appObject[index]->Initialze())
		{
			std::cout << "App[" << index << "] Initialze error" << std::endl;
			return false;
		}
	}

	status = RUNING;

	return true;
}