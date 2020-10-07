
// MKChatServerDlg.h : header file
//

#pragma once

#include <odbcinst.h>
#include <sqlext.h>

#include <list>

#define MAX_USER_COUNT 50
#define MAX_DATA 100 // 데이터를 ODBC에서 가져오는 최대 단위

#define ID_LENGTH 12
#define PW_LENGTH 16



#define MID_CHAT_DATA 1
#define MID_ID 2
#define MID_PW 3
#define MID_ERROR 4
#define MID_LOGIN 5

#define ERRORID_REG_SAMEID L"SAME ID"
#define ERRORID_LOGIN_NOID L"NO ID"
#define ERRORID_LOGIN_WRONGPW L"WRONG PW"
#define ERRORID_LOGIN_APPROVED L"APPROVED"


class MyUserData : MK_UserData
{
public:
	MyUserData();
	~MyUserData();

	void SetUserData(MK_UserData* _userData);
	MK_UserData* GetUserData() const { return m_pUserData; }

	wchar_t* GetID();
	wchar_t* GetPassword();
	void SetID(const wchar_t* cID);
	void SetPassword(const wchar_t* cPassword);

	virtual MK_UserData* CreateObject() { return new MyUserData; }

private:
	MK_UserData* m_pUserData = nullptr;
	wchar_t m_ID[ID_LENGTH];
	wchar_t m_PW[PW_LENGTH];
};


class MyServer : public MK_ServerSocket
{
public:
	MyServer(class CMKChatServerDlg* pParent)
		: MK_ServerSocket(0x27, MAX_USER_COUNT, new MK_UserData)
	{
		m_pParent = pParent;
	}

	virtual void AddWorkForAccept(MK_UserData* pUserData);
	virtual void AddWorkForCloseUser(MK_UserData* pUserData, int iErrorCode);
	virtual int ProcessRecvData(SOCKET hSocket, unsigned char cMessageID, char* pRecvData, BS bsBodySize);
	virtual void ShowLimitError(const wchar_t* cIPAddress);
	virtual void BroadcastToAll(SOCKET hSocket, char* pRecvData, DWORD size);

	std::list<MyUserData*>& GetUserList() { return m_UserDataList; }
	std::list<MyUserData*>& GetConnectedUserList() { return m_ConnectedUserDataList; }

	// UserList에서 유저 찾기
	MyUserData* FindUserDataID(const wchar_t* _ID);
	MyUserData* FindUserDataInUserList(MK_UserData* _userData);
	MyUserData* FindUserDataInConnectedUserList(MK_UserData* _userData);


	MyUserData* CreateUserData(const wchar_t* _ID, const wchar_t* _PW, MK_UserData* _userData = nullptr);
	void AddUserData(MyUserData* _userData);
	
	void AddConnectedUser(MyUserData* _user);


private:
	class CMKChatServerDlg* m_pParent;

	std::list<MyUserData*> m_UserDataList;
	std::list<MyUserData*> m_ConnectedUserDataList;
	
};




// CMKChatServerDlg dialog
class CMKChatServerDlg : public CDialogEx
{
// Construction
public:
	CMKChatServerDlg(CWnd* pParent = nullptr);	// standard constructor

	

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MKCHATSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// 리스트 박스에 문자열을 추가하는 함수
	void AddEventString(const wchar_t* pString);

	// SQL에 정보 추가
	void AddToSQL(const wchar_t* _sql);

	// 접속한 유저 리스트 업데이트
	void UpdateListBoxConnectedUser();

	// 접속한 유저가 접속 해제
	void SetUserDisconnected(MyUserData* _user);
private:
	// SQL에서 정보 가져오기
	void UpdateListBoxAllUser();

	// 유저 리스트 창 업데이트
	void UpdateListBoxString();



	// ODBC 시작
	void InitODBC();

	// ODBC 종료
	void CloseODBC();



private:
	MyServer m_Server;

	SQLHANDLE m_hEnvironment; // ODBC를 사용하기 위한 환경 정보
	SQLHDBC m_hODBC;		// ODBC 함수를 사용하기 위한 정보
	char m_ConnectFlag = 0; // 서버와 연결 여부를 저장할 변수 (1 : 연결)


public:
protected:
	afx_msg LRESULT OnAcceptUser(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReadAndClose(WPARAM wParam, LPARAM lParam);



public:
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedOk2();

	CListBox m_ListBox_EventList;
	CListBox m_list_AllUser;
	CListBox m_list_Password;
	CListBox m_list_ConnectedUsers;
};
