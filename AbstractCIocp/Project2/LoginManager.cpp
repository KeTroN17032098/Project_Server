#include"LoginManager.h"

LoginManager* LoginManager::instance = nullptr;

LoginManager::LoginManager(): _BASICMANAGER(static_cast<int>(STATUS::LOGIN))
{
	join_member.clear();
	loggedin.clear();
	_BASICMANAGER::addMsg(static_cast<int>(LOGIN_PROTOCOL::LOGIN_MSG), L"로그인을 하시기위해 아이디와 비밀번호를 입력하세요.");
	_BASICMANAGER::addMsg(static_cast<int>(SIGNIN_PROTOCOL::SIGNIN_MSG), L"회원가입을 하시기위해 아이디와 비밀번호를 입력하세요.");
	_BASICMANAGER::addMsg(static_cast<int>(LOGINMANAGER_ERR::MULTIPLEID), L"아이디 중복입니다.");
	_BASICMANAGER::addMsg(static_cast<int>(LOGINMANAGER_ERR::LENGTHLIMIT), L"아이디와 비밀번호는 20자이내로 입력해주세요.");
	_BASICMANAGER::addMsg(static_cast<int>(LOGINMANAGER_ERR::SMALLCHAR), L"너무 짧습니다.");
	_BASICMANAGER::addMsg(static_cast<int>(LOGINMANAGER_ERR::ALREADYLGIN), L"이미 로그인되어 있습니다.");
	_BASICMANAGER::addMsg(static_cast<int>(LOGINMANAGER_ERR::NOMATCH), L"아이디 혹은 비밀번호 오류입니다.");
	_BASICMANAGER::addMsg(static_cast<int>(LOGINMANAGER_ERR::UNKNOWNERR), L"알 수 없는 오류입니다.");
	_BASICMANAGER::addMsg(static_cast<int>(LOGINMANAGER_ERR::NOERR), L"에러가 없습니다.");
	_BASICMANAGER::addMsg(static_cast<int>(LOGIN_PROTOCOL::LOGIN_SUCCESS), L"로그인에 성공하였습니다.");
	_BASICMANAGER::addMsg(static_cast<int>(SIGNIN_PROTOCOL::SIGNIN_SUCCESS), L"회원가입에 성공하였습니다.");
	_BASICMANAGER::addMsg(static_cast<int>(SELECTION::SELECTION_MSG), L"로그인하거나 회원가입을 하셔야 합니다.");
	LogManager::LogPrint("LoginManager Created");
	getJoinm();
	LogManager::LogPrint("JOINMEBER GET COMPLETED");

}

LoginManager::~LoginManager()
{
	join_member.clear();
	loggedin.clear();
	LogManager::LogPrint("LoginManager Deleted");
}

void LoginManager::CreateInstance()
{
	if(instance==nullptr)instance = new LoginManager();
}

LoginManager* LoginManager::GetInstance()
{
	return instance;
}

void LoginManager::ClearInstance()
{
	if (instance != nullptr)
	{
		delete instance;
		instance = nullptr;
	}
}

void LoginManager::getJoinm()
{
	int a = 0;
	LoginInfo** tmp = DBManager::getinstance()->getJoinedMember(&a);
	for (int i = 0; i < a; i++)
	{
		join_member.push_back(tmp[i]);
	}
}

int LoginManager::trySignin(sentence* id, sentence* pw)
{
	if (wcslen(id) == 0 || wcslen(pw) == 0)return static_cast<int>(LOGINMANAGER_ERR::SMALLCHAR);
	if (wcslen(id) > 20 ||wcslen(pw) > 20)return static_cast<int>(LOGINMANAGER_ERR::LENGTHLIMIT);
	for (int i = 0; i < join_member.size(); i++)
	{
		if (wcscmp(join_member[i]->ID, id) == 0)return static_cast<int>(LOGINMANAGER_ERR::MULTIPLEID);
	}

	int uuid = DBManager::getinstance()->getNewUUID(id, pw);
	if (uuid == -1)return static_cast<int>(LOGINMANAGER_ERR::UNKNOWNERR);

	LoginInfo* tmp = new LoginInfo();
	ZeroMemory(tmp, sizeof(LoginInfo));
	wcscpy(tmp->ID, id);
	wcscpy(tmp->PW, pw);
	tmp->uuid = uuid;

	join_member.push_back(tmp);

	return tmp->uuid;
}

int LoginManager::tryLogin(sentence* id,sentence* pw)
{
	LoginInfo* tmp = nullptr;
	std::deque<LoginInfo*>::iterator iter;
	for (iter = join_member.begin(); iter != join_member.end(); iter++)
	{
		if (wcscmp(id, (*iter)->ID) == 0)
		{
			if (wcscmp(pw, (*iter)->PW) == 0)
			{
				tmp = (*iter);
				if (std::find(loggedin.begin(), loggedin.end(), tmp->uuid) != loggedin.end())
				{
					return static_cast<int>(LOGINMANAGER_ERR::ALREADYLGIN);
				}
				else
				{
					loggedin.push_back(tmp->uuid);
					return tmp->uuid;
				}
			}
			break;
		}
	}

	return static_cast<int>(LOGINMANAGER_ERR::NOMATCH);
}

void LoginManager::LogOut(int uuid)
{
	std::deque<int>::iterator iter;
	for (iter = loggedin.begin(); iter != loggedin.end(); iter++)
	{
		if ((*iter) == uuid)
		{
			break;
		}
	}
	if (iter != loggedin.end())loggedin.erase(iter);
}



int LoginManager::packPackit(char* Dest, int l, int p, int num, sentence* id, sentence* pw, sentence* msg, int e, public_key_class* pub)
{
	int size = 0;
	PROTOCOL tmp = 0;
	ProtocolManager::GetInstance()->AddMainPart(&tmp, l);
	ProtocolManager::GetInstance()->AddSubPart(&tmp, p);
	PROTOCOL_DETAIL tmpd = 0;
	if (l == static_cast<int>(LORS::SELCTION))
	{
		if (p == static_cast<int>(SELECTION::REQ_SELECTIONMSG))
		{
			tmpd = (static_cast<int>(LOGINMANAGER_DETAIL::MSG)|static_cast<int>(LOGINMANAGER_DETAIL::PUBLICKEY));
		}
		else if (p == static_cast<int>(SELECTION::SELECTION_MSG))
		{
			tmpd = (static_cast<int>(LOGINMANAGER_DETAIL::MSG)|static_cast<int>(LOGINMANAGER_DETAIL::PUBLICKEY));
		}
		else if (p == static_cast<int>(SELECTION::SELECTION_SET))
		{
			tmpd = (static_cast<int>(LOGINMANAGER_DETAIL::NUMBER));
		}
	}
	else if (l == static_cast<int>(LORS::LOGIN))
	{
		if (p == static_cast<int>(LOGIN_PROTOCOL::LOGIN_MSG))
		{
			tmpd = (static_cast<int>(LOGINMANAGER_DETAIL::MSG));
		}
		else if (p == static_cast<int>(LOGIN_PROTOCOL::LOGIN_TRY))
		{
			tmpd = (static_cast<int>(LOGINMANAGER_DETAIL::IDPW));
		}
		else if (p == static_cast<int>(LOGIN_PROTOCOL::LOGIN_SUCCESS))
		{
			tmpd = (static_cast<int>(LOGINMANAGER_DETAIL::NUMBER)|static_cast<int>(LOGINMANAGER_DETAIL::MSG));
		}
		else if (p == static_cast<int>(LOGIN_PROTOCOL::LOGIN_ERROR))
		{
			tmpd = (static_cast<int>(LOGINMANAGER_DETAIL::ERRCODE) | static_cast<int>(LOGINMANAGER_DETAIL::MSG));
		}
	}
	else if (l == static_cast<int>(LORS::SIGNIN))
	{
		if (p == static_cast<int>(SIGNIN_PROTOCOL::SIGNIN_MSG))
		{
			tmpd = (static_cast<int>(LOGINMANAGER_DETAIL::MSG));
		}
		else if (p == static_cast<int>(SIGNIN_PROTOCOL::SIGNIN_TRY))
		{
			tmpd = (static_cast<int>(LOGINMANAGER_DETAIL::IDPW));
		}
		else if (p == static_cast<int>(SIGNIN_PROTOCOL::SIGNIN_SUCCESS))
		{
			tmpd = (static_cast<int>(LOGINMANAGER_DETAIL::NUMBER )| static_cast<int>(LOGINMANAGER_DETAIL::MSG));
		}
		else if (p == static_cast<int>(SIGNIN_PROTOCOL::SIGNIN_ERROR))
		{
			tmpd = (static_cast<int>(LOGINMANAGER_DETAIL::ERRCODE) | static_cast<int>(LOGINMANAGER_DETAIL::MSG));
		}
	}
	ProtocolManager::GetInstance()->AddDetailPart(&tmp, tmpd);
	LogManager::LogPrint("Protocol made : %ld", tmp);
	memcpy(Dest + size, &tmp, sizeof(PROTOCOL));
	size += sizeof(PROTOCOL);
	if (tmpd & static_cast<int>(LOGINMANAGER_DETAIL::NUMBER))
	{
		memcpy(Dest + size, &num, sizeof(int));
		size += sizeof(int);
	}
	if (tmpd & static_cast<int>(LOGINMANAGER_DETAIL::MSG))
	{
		int msgsize = wcslen(msg) * static_cast<int>(sizeof(sentence));
		memcpy(Dest + size, &msgsize, sizeof(int));
		size += sizeof(int);
		memcpy(Dest + size, msg, msgsize);
		size += msgsize;
	}
	if (tmpd & static_cast<int>(LOGINMANAGER_DETAIL::ERRCODE))
	{
		memcpy(Dest + size, &e, sizeof(int));
		size += sizeof(int);
	}
	if (tmpd & static_cast<int>(LOGINMANAGER_DETAIL::IDPW))
	{
		int msgsize = wcslen(id)* static_cast<int>(sizeof(sentence));
		memcpy(Dest + size, &msgsize, sizeof(int));
		size += sizeof(int);
		memcpy(Dest + size, id, msgsize);
		size += msgsize;
		msgsize = wcslen(pw) * static_cast<int>(sizeof(sentence));
		memcpy(Dest + size, &msgsize, sizeof(int));
		size += sizeof(int);
		memcpy(Dest + size, pw, msgsize);
		size += msgsize;
	}
	if (tmpd & static_cast<int>(LOGINMANAGER_DETAIL::PUBLICKEY))
	{
		int keysize = sizeof(public_key_class);
		memcpy(Dest + size, pub, keysize);
		size += keysize;
	}
	return size;
}

void LoginManager::unpackPackit(char* Data, int* l, int* p, int* num, sentence* id, sentence* pw, sentence* msg, int* e,public_key_class* pub)
{
	int size = 0;
	PROTOCOL tmp = 0;
	memcpy(&tmp, Data, sizeof(PROTOCOL));
	size += sizeof(PROTOCOL);
	*l = ProtocolManager::GetInstance()->GetMainPart(tmp);
	*p= ProtocolManager::GetInstance()->GetSubPart(tmp);
	PROTOCOL_DETAIL tmpd = ProtocolManager::GetInstance()->GetDetailPart(tmp);
	LogManager::LogPrint("Unpacked Data Protocol : [%d, %d, %d]", *l, *p, tmpd);
	if (tmpd & static_cast<int>(LOGINMANAGER_DETAIL::NUMBER))
	{
		memcpy(&num, Data+size,sizeof(int));
		size += sizeof(int);
	}
	if (tmpd & static_cast<int>(LOGINMANAGER_DETAIL::MSG))
	{
		int msgsize = 0;
		memcpy(&msgsize, Data + size, sizeof(int));
		size += sizeof(int);
		memcpy(msg,Data+size,msgsize);
		size += msgsize;
	}
	if (tmpd & static_cast<int>(LOGINMANAGER_DETAIL::ERRCODE))
	{
		memcpy(e,Data+size,sizeof(int));
		size += sizeof(int);
	}
	if (tmpd & static_cast<int>(LOGINMANAGER_DETAIL::IDPW))
	{
		int msgsize = 0;
		memcpy(&msgsize,Data + size,sizeof(int));
		size += sizeof(int);
		memcpy(id,Data+size,msgsize);
		size += msgsize;
		msgsize = 0;
		memcpy(&msgsize, Data + size, sizeof(int));
		size += sizeof(int);
		memcpy(pw, Data + size, msgsize);
		size += msgsize;
	}
	if (tmpd & static_cast<int>(LOGINMANAGER_DETAIL::PUBLICKEY))
	{
		int keysize = sizeof(public_key_class);
		memcpy(pub,Data+size, keysize);
		size += keysize;
	}
}

void LoginManager::Process(ISession* is, int* managerNo, char* data, int* datasize)
{
	int l = 0, p = 0, num = 0, e = 0;
	sentence id[20] = L"", pw[20] = L"", msg[500] = L"";
	public_key_class pub;
	LogManager::LogPrint("Unpacking Packet");
	unpackPackit(data, &l, &p, &num, id, pw, msg, &e,&pub);
	LogManager::LogPrint("Unpack Complete");
	
	if (l == static_cast<int>(LORS::SELCTION))
	{
		if (p == static_cast<int>(SELECTION::REQ_SELECTIONMSG))
		{
			is->setPublicKey(pub.modulus, pub.exponent);
			_BASICMANAGER::getMsg(msg, static_cast<int>(SELECTION::SELECTION_MSG));
			*datasize=packPackit(data, l, static_cast<int>(SELECTION::SELECTION_MSG), 0, nullptr, nullptr, msg, 0, Crypt::GetInstance()->getPublicKey());
			*managerNo = _BASICMANAGER::getNo();
		}
		else if (p == static_cast<int>(SELECTION::SELECTION_SET))
		{
			if (num == 1)
			{
				_BASICMANAGER::getMsg(msg, static_cast<int>(LOGIN_PROTOCOL::LOGIN_MSG));
				*datasize = packPackit(data, static_cast<int>(LORS::LOGIN), static_cast<int>(LOGIN_PROTOCOL::LOGIN_MSG), 0, nullptr, nullptr, msg, 0,nullptr);
				*managerNo = _BASICMANAGER::getNo();
			}
			else if (num == 2)
			{
				_BASICMANAGER::getMsg(msg, static_cast<int>(SIGNIN_PROTOCOL::SIGNIN_MSG));
				*datasize = packPackit(data, static_cast<int>(LORS::SIGNIN), static_cast<int>(LOGIN_PROTOCOL::LOGIN_MSG), 0, nullptr, nullptr, msg, 0, nullptr);
				*managerNo = _BASICMANAGER::getNo();
			}
		}
	}
	else if (l == static_cast<int>(LORS::LOGIN))
	{
		if (p == static_cast<int>(LOGIN_PROTOCOL::LOGIN_TRY))
		{
			e = tryLogin(id, pw);
			if (e > 0)//로그인 성공
			{
				is->setUUID(e);
				_BASICMANAGER::getMsg(msg, static_cast<int>(LOGIN_PROTOCOL::LOGIN_SUCCESS));
				int k = packPackit(data, static_cast<int>(LORS::LOGIN), static_cast<int>(LOGIN_PROTOCOL::LOGIN_SUCCESS), e, nullptr, nullptr, msg, 0, nullptr);
				*datasize = k;
				*managerNo =static_cast<int>( STATUS::LOBBY);
			}
			else//로그인 실패
			{
				_BASICMANAGER::getMsg(msg, e);
				int k = packPackit(data, static_cast<int>(LORS::LOGIN), static_cast<int>(LOGIN_PROTOCOL::LOGIN_ERROR), -1, nullptr, nullptr, msg, e, nullptr);
				*datasize = k;
				*managerNo = _BASICMANAGER::getNo();
			}
		}
		else
		{
			_BASICMANAGER::getMsg(msg, static_cast<int>(LOGINMANAGER_ERR::UNKNOWNERR));
			int k = packPackit(data, static_cast<int>(LORS::LOGIN), static_cast<int>(LOGIN_PROTOCOL::LOGIN_ERROR), -1, nullptr, nullptr, msg, static_cast<int>(LOGINMANAGER_ERR::UNKNOWNERR), nullptr);
			*datasize = k;
			*managerNo = _BASICMANAGER::getNo();
		}
	}
	else if (l == static_cast<int>(LORS::SIGNIN))
	{
		if (p == static_cast<int>(SIGNIN_PROTOCOL::SIGNIN_TRY))
		{
			e = trySignin(id, pw);
			if (e > 0)//회원가입 성공
			{
				is->setUUID(e);
				_BASICMANAGER::getMsg(msg, static_cast<int>(SIGNIN_PROTOCOL::SIGNIN_SUCCESS));
				int k = packPackit(data, static_cast<int>(LORS::SIGNIN), static_cast<int>(SIGNIN_PROTOCOL::SIGNIN_SUCCESS), e, nullptr, nullptr, msg, 0, nullptr);
				*datasize = k;
				*managerNo = static_cast<int>(STATUS::LOBBY);
			}
			else//로그인 실패
			{
				_BASICMANAGER::getMsg(msg, e);
				int k = packPackit(data, static_cast<int>(LORS::SIGNIN), static_cast<int>(SIGNIN_PROTOCOL::SIGNIN_ERROR), -1, nullptr, nullptr, msg, e, nullptr);
				*datasize = k;
				*managerNo = _BASICMANAGER::getNo();
			}
		}
		else
		{
			_BASICMANAGER::getMsg(msg, static_cast<int>(LOGINMANAGER_ERR::UNKNOWNERR));
			int k = packPackit(data, static_cast<int>(LORS::SIGNIN), static_cast<int>(SIGNIN_PROTOCOL::SIGNIN_ERROR), -1, nullptr, nullptr, msg, static_cast<int>(LOGINMANAGER_ERR::UNKNOWNERR), nullptr);
			*datasize = k;
			*managerNo = _BASICMANAGER::getNo();
		}
	}
}
