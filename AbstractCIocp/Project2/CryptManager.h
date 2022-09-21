#pragma once

#include"rsa.h"
#include"LogManager.h"

class Crypt
{
private:
	static Crypt* instance;

	public_key_class pub;
	private_key_class priv;

	Crypt();
	~Crypt();
public:
	static void CreateInstance();
	static void ClearInstance();
	static Crypt* GetInstance();

	public_key_class* getPublicKey() { return &pub; }

	void Encrypt(char* data, char* encrypted, int* size);//�ڽ��� ����Ű�� ��ȣȭ
	void Encrypt(char* data, char* encrypted, int* size,public_key_class* key);// �Էµ� ����Ű�� ��ȣȭ
	void Decrypt(char* Encrypted, char* data, int* size);//�ڽ��� ����Ű�� ��ȣȭ
};