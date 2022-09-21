#include"LobbyManager.h"

LobbyManager* LobbyManager::instance = nullptr;

LobbyManager::LobbyManager() : _BASICMANAGER(static_cast<int>(STATUS::LOBBY))
{
}

LobbyManager::~LobbyManager()
{
	_BASICMANAGER::ClearMap();
}

void LobbyManager::CreateInstance()
{
	if (instance == nullptr)instance = new LobbyManager();
}

void LobbyManager::ClearInstance()
{
	if (instance != nullptr)
	{
		delete instance;
		instance = nullptr;
	}
}

LobbyManager* LobbyManager::GetInstance()
{
	return instance;
}

int LobbyManager::PackData(char* data, int m, int s, char* msg)
{
	int size = 0;
	PROTOCOL tmp = 0;
	ProtocolManager::GetInstance()->AddMainPart(&tmp, m);
	ProtocolManager::GetInstance()->AddSubPart(&tmp, s);
	PROTOCOL_DETAIL tmpd = 0;
	if (m == static_cast<int>(LOBBY_PROTOCOL::REQ_LOBBYDATA))
	{
		if (s == static_cast<int>(LOBBYDATA_PROTOCOL::PROFILE))
		{
			tmpd = 0;
		}
	}
	else if (m == static_cast<int>(LOBBY_PROTOCOL::LOBBYDATA))
	{
		if (s == static_cast<int>(LOBBYDATA_PROTOCOL::PROFILE))
		{
			tmpd |= static_cast<int>(LOBBYMANAGER_DETAIL::MSG);
		}
	}
	ProtocolManager::GetInstance()->AddDetailPart(&tmp, tmpd);
	memcpy(data + size, &tmp, sizeof(PROTOCOL));
	size += sizeof(PROTOCOL);
	if (tmpd & static_cast<int>(LOBBYMANAGER_DETAIL::MSG))
	{
		int msgsize = static_cast<int>(strlen(msg));
		memcpy(data + size, &msgsize, sizeof(int));
		size += sizeof(int);
		memcpy(data + size, msg, msgsize);
		size += msgsize;
	}
	return size;
}

void LobbyManager::UnPackData(char* From, int* m, int* s, char* msg)
{
	int size = 0;
	PROTOCOL tmp = 0;
	PROTOCOL_DETAIL tmpd = 0;
	memcpy(&tmp, From + size, sizeof(PROTOCOL));
	size += sizeof(PROTOCOL);
	*m = ProtocolManager::GetInstance()->GetMainPart(tmp);
	*s = ProtocolManager::GetInstance()->GetSubPart(tmp);
	tmpd = ProtocolManager::GetInstance()->GetDetailPart(tmp);

	if (tmpd & static_cast<int>(LOBBYMANAGER_DETAIL::MSG))
	{
		int msgsize = 0;
		memcpy(&msgsize, From + size, sizeof(int));
		size += sizeof(int);
		memcpy(msg, From + size, msgsize);
		size += msgsize;
	}
}



void LobbyManager::Process(ISession* is, int* managerNo, char* data, int* datasize)
{
	int a, b;
	UnPackData(data, &a, &b, nullptr);
	LogManager::LogPrint("LOBBYManager Data\nLBProtocol  - %d", a);
	if (a == static_cast<int>(LOBBY_PROTOCOL::REQ_LOBBYDATA))
	{
		if (b == static_cast<int>(LOBBYDATA_PROTOCOL::PROFILE))
		{
			int k = PackData(data, static_cast<int>(LOBBY_PROTOCOL::LOBBYDATA), static_cast<int>(LOBBYDATA_PROTOCOL::PROFILE), "현재 준비중입니다.");
			*datasize = k;
			*managerNo = _BASICMANAGER::getNo();
		}
	}
}
