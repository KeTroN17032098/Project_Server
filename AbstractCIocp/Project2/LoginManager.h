#pragma once
#include"BasicManager.h"
#include"DBManager.h"
#include"CryptManager.h"
#include<map>
#include<vector>
#include<string>

class LoginManager : public _BASICMANAGER
{
private:
	enum class LORS
	{
		SELCTION=76,
		LOGIN,
		SIGNIN
	};

	enum class SELECTION
	{
		REQ_SELECTIONMSG=32,
		SELECTION_MSG,
		SELECTION_SET
	};

	enum class LOGIN_PROTOCOL
	{
		LOGIN_MSG = 25,
		LOGIN_TRY,
		LOGIN_SUCCESS,
		LOGIN_ERROR
	};

	enum class SIGNIN_PROTOCOL
	{
		SIGNIN_MSG = 42,
		SIGNIN_TRY,
		SIGNIN_SUCCESS,
		SIGNIN_ERROR
	};

	enum class LOGINMANAGER_ERR
	{
		MULTIPLEID = -472,// ȸ������ ���̵��ߺ�
		LENGTHLIMIT,// �ִ� ���� �ʰ�
		SMALLCHAR,//����� ����
		ALREADYLGIN,//�̹� �α��� ��
		NOMATCH,//���̵� Ȥ�� ��й�ȣ ����
		UNKNOWNERR,//�˼����� ����
		NOERR//���� ����
	};

	enum class LOGINMANAGER_DETAIL
	{
		NUMBER=1,
		MSG=1<<1,
		ERRCODE=1<<2,
		IDPW=1<<3,
		PUBLICKEY=1<<4
	};

	static LoginManager* instance;

	std::deque<LoginInfo*> join_member;
	std::deque<int> loggedin;


	LoginManager();
	virtual ~LoginManager();

public:
	static void CreateInstance();
	static LoginManager* GetInstance();
	static void ClearInstance();

	void getJoinm();

	int trySignin(sentence* id, sentence* pw);//ȸ������ �õ�
	int tryLogin(sentence* id, sentence* pw);//�α��� �õ�
	
	void LogOut(int uuid);

	int packPackit(char* Dest,int l, int p, int num, sentence* id, sentence* pw, sentence* msg, int e,public_key_class* pub);
	void unpackPackit(char* Data, int* l, int* p, int* num, sentence* id, sentence* pw, sentence* msg, int* e, public_key_class* pub);

	// _BASICMANAGER��(��) ���� ��ӵ�
	virtual void Process(ISession* is, int* managerNo, char* data, int* datasize) override;
};