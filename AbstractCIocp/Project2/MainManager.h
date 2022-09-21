#pragma once
#include"AbstractIOCP.h"
#include"ISessionManager.h"
#include"LobbyManager.h"
#include"LoginManager.h"

class MainManager : public _VIRTUALIOCP
{
private:
	static MainManager* instance;

	ISocket* ListenSock;
	

	MainManager(bool a, char* dbip, char* dbuser, char* dbpw, char* dbname, int dbport);
	~MainManager();
public:
	static void CreateInstance(bool a, char* dbip, char* dbuser, char* dbpw, char* dbname, int dbport);
	static void ClearInstance();
	static MainManager* GetInstance();

	void ListenSocketSet(int portno);
	void ListenSocketEnd();

	void run(int portno);

	// _VIRTUALIOCP��(��) ���� ��ӵ�
	virtual void accept(SOCKET clientsock, HANDLE hcp) override;

	virtual void recv(void* session, DWORD cbTransferred) override;

	virtual void send(void* session, DWORD cbTransferred) override;

	virtual void disconnect(int retval, void* session) override;

};