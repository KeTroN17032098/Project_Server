#pragma once
#include"AbstractState.h"
#include"LoginManager.h"
#include"LobbyManager.h"

class LoginState : public _STATE
{
private:
	bool a;
public:
	// _STATE��(��) ���� ��ӵ�
	virtual void Recv(void* session) override;
	virtual void Send(void* session) override;
};

class LobbyState : public _STATE 
{
public:
	// _STATE��(��) ���� ��ӵ�
	virtual void Recv(void* session) override;

	virtual void Send(void* session) override;

};