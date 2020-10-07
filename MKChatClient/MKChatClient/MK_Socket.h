#pragma once

#define _WINSOCKAPI_
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>

#include <winsock2.h>

#include <string.h>
#include <process.h>

#include <functional>
#include <list>
#include <unordered_map>

// SOCKET 
// #include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
// #pragma comment(lib, "mswsock.lib")

typedef unsigned  short int BS;

#define HEAD_SIZE 2 + sizeof(BS)
#define SENDDATA_SIZE 8192
#define IPADDRESS_SIZE 16

// #define BUF_SIZE 100

#define LM_SEND_COMPLETED 29001
#define LM_RECV_COMPLETED 29002

struct SOCKET_DATA
{
	SOCKET hClientSocket;
	SOCKADDR_IN clientAddress;
};

struct OVERLAPPED_DATA
{
	OVERLAPPED overlapped;
	char buffer[SENDDATA_SIZE];
	WSABUF wsaBuffer;
};

enum SERVER_TYPE
{
	ST_ASYNC,
	ST_THREAD,
	ST_IOCP
};



// ���۰� ����
class MK_ExchangeManager
{
public:
	////////////////////////////////////////////////
	// ��� �Լ�
	////////////////////////////////////////////////
	MK_ExchangeManager();
	~MK_ExchangeManager();


	// ���� �Ǵ� ���ſ� ����� �޸� �Ҵ�
	// idataSize�� �Ҵ��� ũ�⸦ ����ϸ� 
	// �Ҵ�� �޸��� �ּҸ� ��ȯ
	char* MemoryAllocation(int iDataSize);

	// �Ҵ�Ǿ� ���Ǵ� �޸𸮸� ����
	void DeleteData();

	inline int GetTotalSize()
	{
		return m_iTotalSize;
	}

	inline int GetCurrentSize()
	{
		return m_iCurrentSize;
	}

protected:


	////////////////////////////////////////////////
	// ��� ����
	////////////////////////////////////////////////

protected:
	int m_iTotalSize;
	int m_iCurrentSize;
	char* m_pData;
};



// ���������� ���۵Ǵ� �����͸� �ϳ��� �޸𸮿� ��ġ�� �۾�
//
class MK_RecvManager : public MK_ExchangeManager
{
public:
	// ���ŵ� �����͸� ���� ���ŵ� �����Ϳ� �߰�
	int AddData(char* pData, int iSize);

	// ���ŵ� �����͸� ��ģ �޸��� ���� �ּҸ� ����
	inline char* GetData()
	{
		return m_pData;
	}
};



class MK_SendManager : public MK_ExchangeManager
{
public:
	// ������ ��ġ�� ũ�� ���
	int GetPosition(char**pData, int iDataSize = 2048);

	// ������ �Ϸ�Ǿ����� üũ
	// ��ȯ���� 1�̸� ������ �Ϸ�
	inline char IsProcessing()
	{
		return m_iTotalSize != m_iCurrentSize;
	}

};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

class MK_UserData
{
	//////////////////////////////////////////
	// ��� �Լ�
	//////////////////////////////////////////
public:
	MK_UserData();
	~MK_UserData();

	void CloseSocket(char cLingerFlag);



	wchar_t* GetIP();
	void SetIP(const wchar_t* cIPAddress);

	inline SOCKET GetHandle() { return m_hSocket; }
	inline void SetHandle(SOCKET socket) { m_hSocket = socket; }
	inline class MK_SendManager* GetSendManager() { return m_pSendManager; }
	inline class MK_RecvManager* GetRecvManager() { return m_pRecvManager; }


	// ������ �����, ������ Ŭ������ Ȯ���� �� ���
	virtual MK_UserData* CreateObject() { return new MK_UserData; }


	//////////////////////////////////////////
	// ��� ����
	//////////////////////////////////////////
protected:
	// Ŭ���̾�Ʈ�� ����ϱ� ���� �����ڵ�
	SOCKET m_hSocket;

	// Ŭ���̾�Ʈ���� ū �����͸� �����ϱ� ���� ����� ��ü
	class MK_SendManager* m_pSendManager;
	class MK_RecvManager* m_pRecvManager;

	// Ŭ���̾�Ʈ IP �ּ�
	wchar_t m_cIPAddress[IPADDRESS_SIZE];
};



class MK_UserAccount : public MK_UserData
{
	//////////////////////////////////////////
	// ��� �Լ�
	//////////////////////////////////////////
public:
	wchar_t* GetID();
	wchar_t* GetPassword();
	void SetID(const wchar_t* cID);
	void SetPassword(const wchar_t* cPassword);


	virtual MK_UserData* CreateObject() { return new MK_UserAccount; }

	//////////////////////////////////////////
	// ��� ����
	//////////////////////////////////////////
protected:
	wchar_t m_cID[32];
	wchar_t m_cPassword[32];


};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


class MK_Socket
{
	//////////////////////////////////////////////////////////
	// ��� �Լ�
	//////////////////////////////////////////////////////////
public:
	MK_Socket();
	MK_Socket(unsigned char cValidKey, int iNotifyData);
	virtual ~MK_Socket();

	// int SendDataToPort(Socket so)
	int SendFrameData(SOCKET socket, unsigned char cMessageID, const char* pBodyData, BS bsBodySize);
	int ReceiveData(SOCKET socket, BS bsBodySize);

	void ProcessRecvEvent(SOCKET socket);

	virtual void DisconnectSocket(SOCKET socket, int iError);
	virtual int ProcessRecvData(SOCKET socket, unsigned char cMeesageID, char* pRecvData, BS bsBodySize);

	// IP string ����ȯ
	static void AsciiToUnicode(wchar_t* cDestIP, char* cSrcIP);
	static void UnicodeToAscii(char* cDestIP, wchar_t* cScrIP);

	//////////////////////////////////////////////////////////
	// ��� ����
	//////////////////////////////////////////////////////////
protected:

	// ���������� ��ȿ���� üũ�ϱ� ���� ���а��� ������ ����
	unsigned char m_cValidKey;
	char* m_pSendData;
	char* m_pRecvData;

	// �����Ͱ� ���ŵǰų� ������� ������ �������� ��, �޽����� ������ ������ �ڵ� ��
	HWND m_hNotifyWnd;

	int m_iNotifyDataID;
};






class MK_ServerSocket : public MK_Socket
{
	//////////////////////////////////////////
	// ��� �Լ�
	//////////////////////////////////////////
public:
	MK_ServerSocket(unsigned char cValidKey, unsigned short int iMaxUserCount, class MK_UserData* pUserData
		, int iAcceptMessageID = 25001, int iDataMessageID = 25002);
	virtual ~MK_ServerSocket();


	int StartServer(const wchar_t* cIPAddress, int iPort, HWND hNotifyWnd, SERVER_TYPE eServerType);

	int ProcessToAccept(WPARAM wParam, LPARAM lParam);
	void ProcessClientEvent(WPARAM wParam, LPARAM lParam);
	virtual void DisconnectSocket(SOCKET socket, int iError);
	virtual int ProcessRecvData(SOCKET socket, unsigned char cMeesageID, char* pRecvData, BS bsBodySize);

	int FindUserIndex(SOCKET hSocket);
	class MK_UserData* FindUserData(SOCKET hSocket);



	inline class MK_UserData** GetUserList()
	{
		return m_UserList;
	}

	inline unsigned short int GetMaxUserCount()
	{
		return m_iMaxUserCount;
	}

	// Accept �ÿ� �� �۾�. �������̵� �ʿ�
	virtual void AddWorkForAccept(class MK_UserData* pUserData) = 0;

	// �ִ� ����ڼ� �ʰ��� �� �۾�. �������̵� �ʿ�
	virtual void ShowLimitError(const wchar_t* cIPAddress0) = 0;

	// ������ �߻�. �������̵� �ʿ�
	virtual void AddWorkForCloseUser(class MK_UserData* pUserData, int iError) = 0;

	// IOCP ������ ������ ���
	virtual void BroadcastToAll(SOCKET hSocket, char* pRecvData, DWORD size) = 0;

	//////////////////////////////////////////
	// ��� ����
	//////////////////////////////////////////
protected:
	SOCKET m_hListenSocket;

	SERVER_TYPE m_eServerType;
	// ���ο� Ŭ���̾�Ʈ�� �������� �� �߻��� �޽��� ID
	int m_iAcceptMessageID;

	// ���ٰ� ������ �ִ� ����� ��
	unsigned short int m_iMaxUserCount;

	// �ִ� ����ڸ� �����ϱ� ���� ���� ��ü��
	class MK_UserData** m_UserList;

	// Input/Ouput Completion Port
	// HANDLE m_hCompletionPort;
	// SYSTEM_INFO m_SystemInfo;

	// PER_HANDLE_DATA
	SOCKET_DATA* m_pSocketData;
	// PER_IODATA
	OVERLAPPED_DATA* m_pOverlappedData;

	// Thread
	HANDLE* m_pThreadPool;

	class MK_IOCPThread* m_pIOCP;
};


class MK_ClientSocket : public MK_Socket
{
public:
	MK_ClientSocket(unsigned char cValidKey, int iConnectNotifyID = 26001, int iDataNotifyID = 26002);
	~MK_ClientSocket();

	int ConnectToServer(const wchar_t* pIPAddress, int iPortNumber, HWND hNotifyWnd);
	// �޽��� ó���⿡�� ����� �Լ�
	int ResultOfConnection(LPARAM lParam);
	// ������ ���� ó���� ���� ���� ���� ó��
	int ProcessServerEvent(WPARAM wParam, LPARAM lParam);
	// ������ ���� ������ �����ϱ�
	void DisconnectSocket(SOCKET hSocket, int iErrorCode);
	// ���ŵ� ������ ó��
	int ProcessRecvData(SOCKET hSocket, unsigned char cMessageID, char* pRecvData, BS bsBodySize);

	inline int IsConnected() { return m_cConnectFlag == 2; }
	inline SOCKET GetHandle() { return m_hSocket; }

protected:
	SOCKET m_hSocket;

	// 0 : ���� �ȵ�
	// 1 : ���� �õ� ��
	// 2 : ���� ��
	char m_cConnectFlag;
	int m_iConnectNotifyID;
	class MK_SendManager m_SendManager;
	class MK_RecvManager m_RecvManager;


};

class MK_IOCPThread
{
public:
	MK_IOCPThread(class MK_ServerSocket& core);
	~MK_IOCPThread();

	void Init();
	void Update();

	static HANDLE GetCompletionPort() { return m_hCompletionPort; }

	static unsigned int _stdcall CompletionThread(void* CompletionPortIO);

private:
	static HANDLE m_hCompletionPort;
	static int AcceptNumber;
	HANDLE m_hThread;
	SYSTEM_INFO m_SystemInfo;

	class MK_ServerSocket& Core;
};


