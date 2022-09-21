#include"AbstractIOCP.h"

DWORD __stdcall _VIRTUALIOCP::WorkerThread(LPVOID arg)
{
	int retval;
	_VIRTUALIOCP* vi = (_VIRTUALIOCP*)arg;
	DWORD myH = GetCurrentThreadId();

	while (1)
	{
		DWORD cbTransferred;
		SOCKET client_sock;
		ExOverlapped* ptrk;
		LPOVERLAPPED ptra;
		retval = GetQueuedCompletionStatus(vi->hcp, &cbTransferred,
			(PULONG_PTR)&client_sock, &ptra, INFINITE);
		ptrk = (ExOverlapped*)ptra;
		IO_TYPE iotype = ptrk->type;
		LogManager::LogPrint("%d - GetQueuedCompletionStatus / IO_TYPE : %d", myH,iotype);

		if (cbTransferred == -27 && iotype == IO_TYPE::acceptIO)//accept�� ���
		{
			vi->accept(client_sock, vi->hcp);
			LogManager::LogPrint("%d - Accept", myH);
			continue;
		}
		void* ptr = ptrk->clientLP;
		if (retval == 0 || cbTransferred == 0)
		{
			vi->disconnect(retval, ptr);
			LogManager::LogPrint("%d - Disconnect", myH);
			continue;
		}

		//�Ϸ�� ��Ŷ ó��
		if (iotype == IO_TYPE::recvIO)
		{
			LogManager::LogPrint("%d - Recv Process", myH);
			vi->recv(ptr, cbTransferred);
			LogManager::LogPrint("%d - Recv Process Completed", myH);

		}
		else if (iotype == IO_TYPE::sendIO)
		{
			LogManager::LogPrint("%d - Send Process", myH);
			vi->send(ptr, cbTransferred);
			LogManager::LogPrint("%d - Send Process Completed", myH);

		}
	}
}

_VIRTUALIOCP::_VIRTUALIOCP()
{
	count = 0;
	threads = nullptr;
	hcp = NULL;
	ZeroMemory(&accept_overlapped, sizeof(ExOverlapped));
	accept_overlapped.clientLP = this;
	accept_overlapped.type = IO_TYPE::acceptIO;
}

_VIRTUALIOCP::~_VIRTUALIOCP()
{
	CloseHandle(hcp);
	ZeroMemory(&accept_overlapped, sizeof(ExOverlapped));
}

void _VIRTUALIOCP::init()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)//���� ���� �� ������ ���ϵ��� ���� �� ����
		return;
	int retval;
	hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (hcp == NULL)
	{
		LogManager::LogPrint("IOCP FAILED");
		return;
	}
	LogManager::LogPrint("IOCP STARTED");
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	threads = new HANDLE[static_cast<int>(si.dwNumberOfProcessors) * 2];
	count = static_cast<int>(si.dwNumberOfProcessors) * 2;
	for (int i = 0; i < count; i++)
	{
		threads[i] = CreateThread(NULL, 0, WorkerThread, this, 0, NULL);
		if (threads[i] == NULL)
		{
			LogManager::LogPrint("IOCP Thread Failed");
			return;
		}
		LogManager::LogPrint("Thread No.%d Created", i + 1);
	}
}

void _VIRTUALIOCP::end()
{
	WaitForMultipleObjects(count,threads,TRUE,INFINITE);
	for (int i = 0; i < count; i++)
	{
		CloseHandle(threads[i]);
	}
	delete[] threads;
}

void _VIRTUALIOCP::PostAccept(SOCKET s)
{
	PostQueuedCompletionStatus(hcp, -27, s, &accept_overlapped.overlapped);
	LogManager::LogPrint("%d", this->accept_overlapped.type);
}
