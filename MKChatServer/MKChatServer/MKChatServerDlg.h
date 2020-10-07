
// MKChatServerDlg.h : header file
//

#pragma once

#include <odbcinst.h>
#include <sqlext.h>

#include <list>

#define MAX_USER_COUNT 50
#define MAX_DATA 100 // �����͸� ODBC���� �������� �ִ� ����

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

	// UserList���� ���� ã��
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
	// ����Ʈ �ڽ��� ���ڿ��� �߰��ϴ� �Լ�
	void AddEventString(const wchar_t* pString);

	// SQL�� ���� �߰�
	void AddToSQL(const wchar_t* _sql);

	// ������ ���� ����Ʈ ������Ʈ
	void UpdateListBoxConnectedUser();

	// ������ ������ ���� ����
	void SetUserDisconnected(MyUserData* _user);
private:
	// SQL���� ���� ��������
	void UpdateListBoxAllUser();

	// ���� ����Ʈ â ������Ʈ
	void UpdateListBoxString();



	// ODBC ����
	void InitODBC();

	// ODBC ����
	void CloseODBC();



private:
	MyServer m_Server;

	SQLHANDLE m_hEnvironment; // ODBC�� ����ϱ� ���� ȯ�� ����
	SQLHDBC m_hODBC;		// ODBC �Լ��� ����ϱ� ���� ����
	char m_ConnectFlag = 0; // ������ ���� ���θ� ������ ���� (1 : ����)


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
