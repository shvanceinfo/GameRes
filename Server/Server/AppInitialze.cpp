#include "AppManager.h"
#include "AppInitialze.h"
#include "..\Server\CLogin.h"

AppManager* AppManager::m_Instance = nullptr;

AppManager* AppManager::GetInstance()
{
	if (nullptr == m_Instance)
	{
		m_Instance = new AppManager();
	}

	return m_Instance;
}

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

bool AppManager::Run()
{
	for (auto itr = m_Demos.begin(); itr != m_Demos.end(); itr++)
	{
		if (false == (*itr)->Initialze())
		{
			return false;
		}
	}

	status = RUNING;

	return true;
}