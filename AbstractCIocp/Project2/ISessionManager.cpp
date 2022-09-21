#include"ISessionManager.h"

ISessionManager* ISessionManager::instance = nullptr;

ISessionManager::ISessionManager()
{
	LogManager::LogPrint(L"Session Manager Start");
	sessions.clear();
}

ISessionManager::~ISessionManager()
{
	int k = static_cast<int>(sessions.size());
	while (k > 0)
	{
		ISession* tmp = sessions.front();
		delete tmp;
		sessions.pop_front();
		k--;
	}
	sessions.clear();
}

ISession* ISessionManager::findbysocket(SOCKET s)
{
	for (int i = 0; i < sessions.size(); i++)
	{
		if (sessions[i]->is_me(s) == true)
			return sessions[i];
	}
	return nullptr;
}

ISession* ISessionManager::findbyuuid(int i)
{
	for (int k = 0; k < sessions.size(); k++)
	{
		if (sessions[k]->is_me_u(i) == true)
			return sessions[k];
	}
	return nullptr;
}

void ISessionManager::addSession(SOCKET s)
{
	ISession* tmp = new ISession(s);
	tmp->addState(static_cast<int>(STATUS::LOBBY), new LobbyState());
	tmp->addState(static_cast<int>(STATUS::LOGIN), new LoginState());
	tmp->setState(STATUS::LOGIN);
	sessions.push_back(tmp);
}

void ISessionManager::deleteSession(SOCKET s)
{
	ISession* tmp = findbysocket(s);
	std::deque<ISession*>::iterator iter;
	for (iter = sessions.begin(); iter != sessions.end(); iter++)
	{
		if (*iter == tmp)
		{
			sessions.erase(iter);
			if(tmp->getUUID()!=-1)LoginManager::GetInstance()->LogOut(tmp->getUUID());
			delete tmp;
			return;
		}
	}
}
