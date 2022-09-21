#include"IPacket.h"

IPacket::IPacket(SOCKET s) : ISocket(s)
{
	sendbuf.clear();
	recvbuf = new _BUFFER(IO_TYPE::recvIO, 8192, 0);
	recvpno = 1;
	sendpno = 1;
	ccs = new CCS();
}

IPacket::~IPacket()
{
	delete recvbuf;
	std::deque<_BUFFER*>::iterator iter;
	for (iter = sendbuf.begin(); iter != sendbuf.end(); iter++)
	{
		_BUFFER* tmp = *iter;
		delete tmp;
	}
	sendbuf.clear();
	delete ccs;
}

bool IPacket::recvprogress(DWORD cbTransferred)
{
	_BUFFER* b = recvbuf;
	if (b->getcompleted() < sizeof(int))//size �� ���޾ƿ���
	{
		if (b->getcompleted() + cbTransferred >= sizeof(int))//�̹��� �� �޾Ҵٸ�
		{
			int size = 0;
			memcpy(&size, b->getresult(), sizeof(int));
			b->setleft(size);
			b->fcompleted(cbTransferred);
		}
		else
		{
			b->fcompleted(cbTransferred);
			b->setleft(0);
		}
	}
	else//���� �޾ƿ���
	{
		b->fcompleted(cbTransferred);
	}
	if (b->is_completed() == true)//�̹��� ��Ŷ�� �� �޾ƿԴٸ�
	{
		return true;
	}
	return false;
}

bool IPacket::sendprogress(DWORD cbTransferred)
{
	if (sendbuf.size() < 1)
	{
		LogManager::LogPrint("Sendbuf Alreay Done");
		return false;
	}
	LockForSession lfs(this->ccs);
	LogManager::LogPrint("Accessing Send Buf");
	_BUFFER* b = sendbuf.front();
	LogManager::LogPrint("Get Front Buf");
	b->fcompleted(cbTransferred);
	if (b->is_completed() == true)//�̹��� ��Ŷ�� �� ���´ٸ�
	{
		LogManager::LogPrint("Buf Send Completed");
		sendbuf.pop_front();
		LogManager::LogPrint("Front Buf Pop");
		delete b;
		LogManager::LogPrint("Buf Deleted");
		return true;
	}
	return false;
}

bool IPacket::tryrecv()
{
	if (recvbuf->getcompleted() > 0)return true;
	return this->recv(recvbuf->getbuf(), recvbuf->getleft());
}

bool IPacket::trysend()
{
	if (sendbuf.size() != 1)
	{
		return true;
	}
	else
	{
		LockForSession lfs(this->ccs);
		return this->send(sendbuf.front()->getbuf(), sendbuf.front()->getleft());
	}
}

int IPacket::Pack(int status, int datasize, char* Data)
{
	_BUFFER* tmp = new _BUFFER(IO_TYPE::sendIO, datasize + 8+sizeof(PROTOCOL), datasize + 8 + sizeof(PROTOCOL));
	int size = sizeof(int);
	PROTOCOL ardf = 0;
	ProtocolManager::GetInstance()->AddMainPart(&ardf, status);
	ProtocolManager::GetInstance()->AddDetailPart(&ardf, static_cast<int>(ENCRYPTED::Y));
	memcpy(tmp->getresult() + size, &ardf, sizeof(int));
	size += sizeof(PROTOCOL);
	memcpy(tmp->getresult() + size, &sendpno, sizeof(int));
	size += sizeof(int);
	memcpy(tmp->getresult() + size, Data, datasize);
	size += datasize;
	memcpy(tmp->getresult(), &size, sizeof(int));
	sendpno++;
	sendbuf.push_back(tmp);
	return size;
}

void IPacket::UnPack(int* status, int* recvpno, int* datasize, char* Data)
{
	int packitsize = recvbuf->getcompleted();
	int fds = 0;
	memcpy(&fds, recvbuf->getresult(), sizeof(int));
	int size = sizeof(int);
	PROTOCOL ardf = 0;
	memcpy(&ardf, recvbuf->getresult() + size, sizeof(PROTOCOL));
	size += sizeof(PROTOCOL);
	int k=static_cast<int>(ProtocolManager::GetInstance()->GetMainPart(ardf));
	*status = k;
	memcpy(recvpno, recvbuf->getresult() + size, sizeof(int));
	size += sizeof(int);
	memcpy(Data, recvbuf->getresult() + size, packitsize - size);
	*datasize = packitsize - size;
	recvbuf->recvclear();
}
