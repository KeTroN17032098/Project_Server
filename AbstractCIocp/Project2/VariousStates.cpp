#include"VariousStates.h"

void LoginState::Recv(void* session)
{
	LogManager::LogPrint("LoginState");
	int mno = 0,pn=0;
	ISession* ptr = (ISession*)session;
	char data1[8192]="",data[8192]="";
	int datasize = 0;
	LogManager::LogPrint("UnPacking...");
	ptr->UnPack(&mno, &pn, &datasize, data);
	LogManager::LogPrint("UnPacking Completed");
	LogManager::LogPrint("Checking PNO ....");
	if (ptr->checkpno(pn) == false) 
	{
		LogManager::LogPrint("PNO Check Fail");
		return;
	}
	if (ptr->getPublicKey()->exponent == 0)
	{
		LogManager::LogPrint("Get Encrypt Key");
		//Ŭ�󿡼� Encrypt Key�� ���� �� �޾ƿ� = ���� ������ ����Ű�� �������� ���� = ���� ��ȣȭ �ȵ�
		LoginManager::GetInstance()->Process(ptr, &mno,data,&datasize);
		memcpy(data1, data, datasize);
		Crypt::GetInstance()->Encrypt(data1,data,&datasize,ptr->getPublicKey());
		ptr->Pack(mno, datasize, data);
	}
	else
	{
		LogManager::LogPrint("Get Encrypted");
		Crypt::GetInstance()->Decrypt(data, data1, &datasize);
		LoginManager::GetInstance()->Process(ptr, &mno, data1, &datasize);
		Crypt::GetInstance()->Encrypt(data1, data, &datasize, ptr->getPublicKey());
		ptr->Pack(mno, datasize, data);
		if (mno != static_cast<int>(STATUS::LOGIN))a = true;
	}
	ptr->trysend();
}

void LoginState::Send(void* session)
{
	ISession* ptr = (ISession*)session;
	if (a == true)
	{
		ptr->setState(STATUS::LOBBY);
	}
	ptr->tryrecv();
}

void LobbyState::Recv(void* session)
{
	int mno = 0, pn = 0;
	ISession* ptr = (ISession*)session;
	char data1[8192] = "", data[8192] = "";
	int datasize = 0;
	ptr->UnPack(&mno, &pn, &datasize, data);
	if (ptr->checkpno(pn) == false)return;
	Crypt::GetInstance()->Decrypt(data, data1, &datasize);
	LobbyManager::GetInstance()->Process(ptr, &mno, data1, &datasize);
	Crypt::GetInstance()->Encrypt(data1, data, &datasize, ptr->getPublicKey());
	ptr->Pack(mno, datasize, data);
	ptr->trysend();
}

void LobbyState::Send(void* session)
{
	ISession* ptr = (ISession*)session;
	ptr->tryrecv();
}
