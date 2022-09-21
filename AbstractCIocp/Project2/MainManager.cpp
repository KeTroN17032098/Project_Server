#include"MainManager.h"

MainManager* MainManager::instance = nullptr;

MainManager::MainManager(bool a, char* dbip, char* dbuser, char* dbpw, char* dbname, int dbport) : _VIRTUALIOCP()
{
	int retval;
	LogManager::CreateInstance(a);
	ProtocolManager::CreateInstance();
	ISessionManager::createinstance();
	ISessionManager::getinstance()->printSessionMemoryMap();

	DBManager::CreateInstance(dbip, dbuser, dbpw, dbname, dbport);
	LoginManager::CreateInstance();
	LobbyManager::CreateInstance();
	Crypt::CreateInstance();
	ListenSock = nullptr;

}

MainManager::~MainManager()
{

	ISessionManager::clearinstance();
	ProtocolManager::ClearInstance();
	DBManager::clearInstance();
	LoginManager::ClearInstance();
	LobbyManager::ClearInstance();
	LogManager::ClearInstance();
	Crypt::ClearInstance();
}

void MainManager::CreateInstance(bool a, char* dbip, char* dbuser, char* dbpw, char* dbname, int dbport)
{
	if (instance == nullptr)
	{
		instance = new MainManager(a,dbip,dbuser,dbpw,dbname,dbport);
	}
}

void MainManager::ClearInstance()
{
	if (instance != nullptr)
	{
		delete instance;
		instance = nullptr;
	}
}

MainManager* MainManager::GetInstance()
{
	return instance;
}


void MainManager::accept(SOCKET clientsock, HANDLE hcp)
{
	int retval;
	CreateIoCompletionPort((HANDLE)clientsock, hcp, clientsock, 0);
	LogManager::LogPrint("Connected Socket with IOCP");
	ISessionManager::getinstance()->addSession(clientsock);
	LogManager::LogPrint("Made Session by Socket");
	ISession* tmp = ISessionManager::getinstance()->findbysocket(clientsock);
	if (tmp->tryrecv() == false)
	{
		ISessionManager::getinstance()->deleteSession(clientsock);
	}
}

void MainManager::recv(void* session, DWORD cbTransferred)
{
	ISession* ptr = (ISession*)session;
	if (ptr->recvprogress(cbTransferred) == true)//���ú� ���� -> ��Ŷ ó�� ->����
	{
		ptr->getState()->Recv(ptr);
	}
	else//���ú� �� �� -> ���� ���
	{
		ptr->tryrecv();
	}
}

void MainManager::send(void* session, DWORD cbTransferred)
{
	ISession* ptr = (ISession*)session;
	ptr->sendprogress(cbTransferred);
	if (ptr->is_sendleft() == true)ptr->trysend();
	else
	{
		//state->send()
		ptr->getState()->Send(ptr);
	}
}

void MainManager::disconnect(int retval, void* session)
{
	ISession* ptr = (ISession*)session;
	if (retval == 0)
	{
		DWORD temp1, temp2;
		ptr->getoverlapped(&temp1, &temp2);
		LogManager::LogPrint("WSAGetOverlappedResult()");
	}
	ISessionManager::getinstance()->deleteSession(ptr);
}

void MainManager::ListenSocketSet(int portno)
{//������ ���� �ʱ�ȭ
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);//TCP,IPV4�� ���� ����
	if (sock == INVALID_SOCKET)
	{
		LogManager::LogPrint("Socket Error - MainManger");
		exit(-1);
	}
	ListenSock = new ISocket(sock);
	ListenSock->Bind(portno);
}

void MainManager::ListenSocketEnd()
{//������ ���� ����
	delete ListenSock;
	ListenSock = nullptr;
}

void MainManager::run(int portno)
{	

	init();
	ListenSocketSet(portno);
	ListenSock->Listen();
	LogManager::LogPrint("Listen Started");
	while (1)
	{
		SOCKET cs = ListenSock->Accept();
		if (cs == INVALID_SOCKET)break;
		SOCKADDR_IN das;
		int sda = sizeof(das);
		getpeername(cs, (SOCKADDR*)&das, &sda);
		LogManager::LogPrint( "Socket Accepted : %s:%d", inet_ntoa(das.sin_addr), ntohs(das.sin_port));
		PostAccept(cs);
		ISessionManager::getinstance()->printSessionMemoryMap();
	}
	end();
	ListenSocketEnd();
}
