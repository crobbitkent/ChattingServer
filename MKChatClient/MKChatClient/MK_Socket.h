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



// 전송과 수신
class MK_ExchangeManager
{
public:
	////////////////////////////////////////////////
	// 멤버 함수
	////////////////////////////////////////////////
	MK_ExchangeManager();
	~MK_ExchangeManager();


	// 전송 또는 수신에 사용할 메모리 할당
	// idataSize에 할당할 크기를 명시하면 
	// 할당된 메모리의 주소를 반환
	char* MemoryAllocation(int iDataSize);

	// 할당되어 사용되던 메모리를 제거
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
	// 멤버 변수
	////////////////////////////////////////////////

protected:
	int m_iTotalSize;
	int m_iCurrentSize;
	char* m_pData;
};



// 나누어져서 전송되는 데이터를 하나의 메모리에 합치는 작업
//
class MK_RecvManager : public MK_ExchangeManager
{
public:
	// 수신된 데이터를 기존 수신된 데이터에 추가
	int AddData(char* pData, int iSize);

	// 수신된 데이터를 합친 메모리의 시작 주소를 얻음
	inline char* GetData()
	{
		return m_pData;
	}
};



class MK_SendManager : public MK_ExchangeManager
{
public:
	// 전송할 위치와 크기 계산
	int GetPosition(char**pData, int iDataSize = 2048);

	// 전송이 완료되었는지 체크
	// 반환값이 1이면 전송이 완료
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
	// 멤버 함수
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


	// 다형성 적용시, 동일한 클래스를 확장할 때 사용
	virtual MK_UserData* CreateObject() { return new MK_UserData; }


	//////////////////////////////////////////
	// 멤버 변수
	//////////////////////////////////////////
protected:
	// 클라이언트와 통신하기 위한 소켓핸들
	SOCKET m_hSocket;

	// 클라이언트에서 큰 데이터를 전송하기 위해 사용할 객체
	class MK_SendManager* m_pSendManager;
	class MK_RecvManager* m_pRecvManager;

	// 클라이언트 IP 주소
	wchar_t m_cIPAddress[IPADDRESS_SIZE];
};



class MK_UserAccount : public MK_UserData
{
	//////////////////////////////////////////
	// 멤버 함수
	//////////////////////////////////////////
public:
	wchar_t* GetID();
	wchar_t* GetPassword();
	void SetID(const wchar_t* cID);
	void SetPassword(const wchar_t* cPassword);


	virtual MK_UserData* CreateObject() { return new MK_UserAccount; }

	//////////////////////////////////////////
	// 멤버 변수
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
	// 멤버 함수
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

	// IP string 형변환
	static void AsciiToUnicode(wchar_t* cDestIP, char* cSrcIP);
	static void UnicodeToAscii(char* cDestIP, wchar_t* cScrIP);

	//////////////////////////////////////////////////////////
	// 멤버 변수
	//////////////////////////////////////////////////////////
protected:

	// 프로토콜의 유효성을 체크하기 위한 구분값을 저장할 변수
	unsigned char m_cValidKey;
	char* m_pSendData;
	char* m_pRecvData;

	// 데이터가 수신되거나 상대편이 접속을 해제했을 때, 메시지를 수신할 윈도우 핸들 값
	HWND m_hNotifyWnd;

	int m_iNotifyDataID;
};






class MK_ServerSocket : public MK_Socket
{
	//////////////////////////////////////////
	// 멤버 함수
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

	// Accept 시에 할 작업. 오버라이딩 필요
	virtual void AddWorkForAccept(class MK_UserData* pUserData) = 0;

	// 최대 사용자수 초과시 할 작업. 오버라이딩 필요
	virtual void ShowLimitError(const wchar_t* cIPAddress0) = 0;

	// 닫을때 발생. 오버라이딩 필요
	virtual void AddWorkForCloseUser(class MK_UserData* pUserData, int iError) = 0;

	// IOCP 정보를 받으면 방송
	virtual void BroadcastToAll(SOCKET hSocket, char* pRecvData, DWORD size) = 0;

	//////////////////////////////////////////
	// 멤버 변수
	//////////////////////////////////////////
protected:
	SOCKET m_hListenSocket;

	SERVER_TYPE m_eServerType;
	// 새로운 클라이언트가 접속했을 때 발생할 메시지 ID
	int m_iAcceptMessageID;

	// 서바가 관리할 최대 사용자 수
	unsigned short int m_iMaxUserCount;

	// 최대 사용자를 저장하기 위해 사용될 객체들
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
	// 메시지 처리기에서 사용할 함수
	int ResultOfConnection(LPARAM lParam);
	// 데이터 수신 처리와 서버 연결 해제 처리
	int ProcessServerEvent(WPARAM wParam, LPARAM lParam);
	// 서버와 접속 강제로 해제하기
	void DisconnectSocket(SOCKET hSocket, int iErrorCode);
	// 수신된 데이터 처리
	int ProcessRecvData(SOCKET hSocket, unsigned char cMessageID, char* pRecvData, BS bsBodySize);

	inline int IsConnected() { return m_cConnectFlag == 2; }
	inline SOCKET GetHandle() { return m_hSocket; }

protected:
	SOCKET m_hSocket;

	// 0 : 접속 안됨
	// 1 : 접속 시도 중
	// 2 : 접속 중
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


