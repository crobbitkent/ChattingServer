
// MKChatServerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "MKChatServer.h"
#include "MKChatServerDlg.h"
#include "afxdialogex.h"

#include "MK_Socket.h"

// OBDC

#pragma comment(lib, "odbc32.lib")



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


struct UserData
{
	wchar_t ID[12];
	wchar_t PW[16];
};


CMKChatServerDlg::CMKChatServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MKCHATSERVER_DIALOG, pParent)
	, m_Server(this)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	// int size = HEAD_SIZE;
}

void CMKChatServerDlg::AddEventString(const wchar_t * pString)
{
	// ����Ʈ �ڽ� ���� �Ʒ��ʿ� �߰�
	int index = m_ListBox_EventList.InsertString(-1, pString);

	// ���ڿ��� �߰��� ��ġ�� ������ �����Ͽ� �ٷ� Ȯ�� �����ϵ���
	m_ListBox_EventList.SetCurSel(index);
}

// ��� ���� ��� ������Ʈ
void CMKChatServerDlg::UpdateListBoxAllUser()
{
	// listbox�� ����� �׸���� ��� ����
	m_list_AllUser.ResetContent();

	// all_user ���̺� ����� �����͸� ��� �д� SQL ��ɹ�
	wchar_t query_str[256] = L"SELECT * FROM all_user";

	// �о�� �������� ���¸� ����� ����
	unsigned short record_state[MAX_DATA];
	// �о�� �����͸� ������ ����
	UserData raw_data[MAX_DATA];

	CString str;

	// �����͸� ������ �迭�� �ʱ�ȭ
	memset(raw_data, 0, sizeof(raw_data));

	HSTMT hStatement;
	RETCODE retCode;

	// Query���� ���� �޸𸮸� �Ҵ�
	if (SQL_SUCCESS == SQLAllocHandle(SQL_HANDLE_STMT, m_hODBC, &hStatement))
	{
		// ���� �������� ������ ������ ����
		unsigned long recordCount = 0;

		// Query���� ������ �� Ÿ�Ӿƿ��� ����
		SQLSetStmtAttr(hStatement, SQL_ATTR_QUERY_TIMEOUT, (SQLPOINTER)15, SQL_IS_UINTEGER);

		// ������ �����͸� ������ �޸��� ũ�⸦ ����
		SQLSetStmtAttr(hStatement, SQL_ATTR_ROW_BIND_TYPE, (SQLPOINTER)sizeof(raw_data), 0);

		// �����͸� �����ö� ���ü��� ���� ��� ����
		SQLSetStmtAttr(hStatement, SQL_ATTR_CONCURRENCY, (SQLPOINTER)SQL_CONCUR_ROWVER, SQL_IS_UINTEGER);

		// �˻��� �������� ��ġ�� ����ϴ� ����� ����
		SQLSetStmtAttr(hStatement, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_KEYSET_DRIVEN, SQL_IS_UINTEGER);

		// �����͸� �������� �ִ� ���� ����
		SQLSetStmtAttr(hStatement, SQL_ATTR_ROW_NUMBER, (SQLPOINTER)MAX_DATA, SQL_IS_UINTEGER);

		// ���� �������� ���¸� ������ ������ �ּҸ� ����
		SQLSetStmtAttr(hStatement, SQL_ATTR_ROW_STATUS_PTR, record_state, 0);

		// ���� �������� ������ ������ ������ �ּҸ� ����
		SQLSetStmtAttr(hStatement, SQL_ATTR_ROWS_FETCHED_PTR, &recordCount, 0);

		// ���̺��� ������ �����͸� �Ӽ����� raw_data ������ �����ϱ� ���ؼ�
		// �Ӽ����� ������ �޸� ��ġ�� ����
		SQLBindCol(hStatement, 1, SQL_WCHAR, raw_data[0].ID, sizeof(wchar_t) * 12, NULL);
		SQLBindCol(hStatement, 2, SQL_WCHAR, raw_data[0].PW, sizeof(wchar_t) * 16, NULL);
		// SQLBindCol(hStatement, 1, SQL_WCHAR, raw_data[0].ID, sizeof(wchar_t) * 12, NULL);

		// SQL ��ɹ��� ����
		retCode = SQLExecDirect(hStatement, (SQLWCHAR*)query_str, SQL_NTS);

		// SQL ��ɹ��� �������� ���� �����͸� listbox�� �߰�
		while (retCode = SQLFetchScroll(hStatement, SQL_FETCH_NEXT, 0) != SQL_NO_DATA)
		{
			// ������ ������ŭ �ݺ��ϸ鼭 �۾�
			for (unsigned int i = 0; i < recordCount; ++i)
			{
				// ������ �����Ͱ� ������ ������ �ƴ϶�� �ش� �Ӽ�����
				// ���ļ� ���ڿ��� �����ϰ� listbox�� ������Ʈ
				if (SQL_ROW_DELETED != record_state[i] && SQL_ROW_ERROR != record_state[i])
				{
					// ���� Id�� �ִ��� ã�ƺ���
					wchar_t* tempID = raw_data[i].ID;

					// ���� id�� ���ٸ�
					if (nullptr == m_Server.FindUserDataID(tempID))
					{
						// ���� �� userlist�� �߰�
						m_Server.CreateUserData(tempID, raw_data[i].PW);
					}
					else
					{
						// ���� id�� �ִٸ� �ѱ��
						continue;
					}

					//str.Format(L"%s", raw_data[i].ID);
					//m_list_AllUser.InsertString(-1, str);				
					//str.Format(L"%s", raw_data[i].PW);
					//m_list_Password.InsertString(-1, str);
				}
			}
		}

		// List All User �� ������Ʈ
		UpdateListBoxString();

		// Query���� ���� �Ҵ��� �޸𸮸� ����
		SQLFreeHandle(SQL_HANDLE_STMT, hStatement);
	}
}

// ��°�� �߰�
void CMKChatServerDlg::UpdateListBoxString()
{
	// ����Ʈ ��� ���� ����
	m_list_AllUser.ResetContent();
	m_list_Password.ResetContent();

	auto iter = m_Server.GetUserList().begin();
	auto iterEnd = m_Server.GetUserList().end();

	for (; iter != iterEnd; ++iter)
	{
		CString str;

		str.Format(L"%s", (*iter)->GetID());
		m_list_AllUser.InsertString(-1, str);				
		str.Format(L"%s", (*iter)->GetPassword());
		m_list_Password.InsertString(-1, str);
	}
}

void CMKChatServerDlg::SetUserDisconnected(MyUserData * _user)
{
	auto iter = m_Server.GetConnectedUserList().begin();
	auto iterEnd = m_Server.GetConnectedUserList().end();

	for (; iter != iterEnd; ++iter)
	{
		if (_user == (*iter))
		{
			m_Server.GetConnectedUserList().erase(iter);
			return;
		}
	}
}

void CMKChatServerDlg::UpdateListBoxConnectedUser()
{
	// ����Ʈ ��� ���� ����
	m_list_ConnectedUsers.ResetContent();

	auto iter = m_Server.GetConnectedUserList().begin();
	auto iterEnd = m_Server.GetConnectedUserList().end();

	for (; iter != iterEnd; ++iter)
	{
		CString str;

		str.Format(L"%s", (*iter)->GetID());
		m_list_ConnectedUsers.InsertString(-1, str);
	}
}

void CMKChatServerDlg::InitODBC()
{
	// ODBC ����̹��� ������ ���� �⺻ ������ ����
	SQLSetEnvAttr(NULL, SQL_ATTR_CONNECTION_POOLING, (SQLPOINTER)SQL_CP_ONE_PER_DRIVER, SQL_IS_INTEGER);

	// ODBC ����� ����ϱ� ���� ȯ���� ����
	// ������ ȯ�� ������ ���� �ڵ鰪�� m_hEnvironment�� ����
	if (SQL_ERROR != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnvironment))
	{
		// ����� ODBC ���� ������ ����
		SQLSetEnvAttr(m_hEnvironment, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
		SQLSetEnvAttr(m_hEnvironment, SQL_ATTR_CP_MATCH, (SQLPOINTER)SQL_CP_RELAXED_MATCH, SQL_IS_INTEGER);

		// ODBC �Լ��� ����ϱ� ���� ������ ����
		// �������� ���� �ڵ鰪�� m_hODBC�� ����
		if (SQL_ERROR != SQLAllocHandle(SQL_HANDLE_DBC, m_hEnvironment, &m_hODBC))
		{
			RETCODE retcode = SQLConnect(m_hODBC,
				// ������ DSN ����
				(SQLWCHAR*)L"my_users", SQL_NTS,
				// ���ӿ� ����� ID
				(SQLWCHAR*)L"crobbitkent", SQL_NTS,
				// ���ӿ� ����� password
				(SQLWCHAR*)L"alsrb5779", SQL_NTS);

			if (SQL_SUCCESS == retcode || SQL_SUCCESS_WITH_INFO == retcode)
			{
				// ODBC�� ����Ͽ� ������ ���̽� ������ ���������� ������ ���
				m_ConnectFlag = 1;

				return;
			}
			else
			{
				// SQL ���� ����
				assert(false);

				CloseODBC();
			}
		}
	}
}

void CMKChatServerDlg::CloseODBC()
{
	// ���ӿ� ������ ���, �����ߴ� �޸𸮸� ����
	if (SQL_NULL_HDBC != m_hODBC)
	{
		SQLFreeHandle(SQL_HANDLE_DBC, m_hODBC);
	}

	if (SQL_NULL_HENV != m_hEnvironment)
	{
		SQLFreeHandle(SQL_HANDLE_ENV, m_hEnvironment);
	}
}

void CMKChatServerDlg::AddToSQL(const wchar_t * _sql)
{
	SQLHSTMT hStatement;

	// Query���� ���� �޸� �Ҵ�
	if (SQL_SUCCESS == SQLAllocHandle(SQL_HANDLE_STMT, m_hODBC, &hStatement) )
	{
		// query���� ������ �� Ÿ�Ӿƿ��� ����
		SQLSetStmtAttr(hStatement, SQL_ATTR_QUERY_TIMEOUT, (SQLPOINTER)15, SQL_IS_UINTEGER);

		// SQL ��ɹ� ����
		RETCODE ret = SQLExecDirect(hStatement, (SQLWCHAR*)_sql, SQL_NTS);

		// ���������� �Ϸ��ߴ��� üũ
		if (SQL_SUCCESS == ret || SQL_SUCCESS_WITH_INFO == ret)
		{
			// ����
		}
		else
		{
			assert(false);
		}

		// ��� ������ �Ϸ�Ǿ��ٴ� ���� ����(commit)
		SQLEndTran(SQL_HANDLE_ENV, m_hEnvironment, SQL_COMMIT);
		// Query���� ���� �Ҵ��� �޸� ����
		SQLFreeHandle(SQL_HANDLE_STMT, hStatement);
	}

	UpdateListBoxString();
}

void CMKChatServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EVENT_LIST, m_ListBox_EventList);
	DDX_Control(pDX, IDC_LISTBOX_ID, m_list_AllUser);
	DDX_Control(pDX, IDC_LISTBOX_PW, m_list_Password);
	DDX_Control(pDX, IDC_LISTBOX_ID2, m_list_ConnectedUsers);
}

BEGIN_MESSAGE_MAP(CMKChatServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(25001, &CMKChatServerDlg::OnAcceptUser)
	ON_MESSAGE(25002, &CMKChatServerDlg::OnReadAndClose)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK2, &CMKChatServerDlg::OnBnClickedOk2)
END_MESSAGE_MAP()


// CMKChatServerDlg message handlers

BOOL CMKChatServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//////////////////////////////////////////////////////////////////////////////////////
	// START
	//////////////////////////////////////////////////////////////////////////////////////

	// SQL �����ϱ�
	InitODBC();

	// SQL ���� �ҷ�����
	UpdateListBoxAllUser();

	SERVER_TYPE serverType = ST_IOCP;

	// ���� ����
	// 192.168.0.9
	// 27100 �̻��� ��ȣ�� ����϶�.
	m_Server.StartServer(L"192.168.0.10", 27444, m_hWnd, serverType);
	AddEventString(L"���� ���񽺸� �����մϴ�.");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMKChatServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMKChatServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////


void MyServer::AddWorkForAccept(MK_UserData * pUserData)
{
	CString str;
	str.Format(L"%s���� ���ο� ����ڰ� �����߽��ϴ�.", pUserData->GetIP());
	m_pParent->AddEventString(str);
}

void MyServer::AddWorkForCloseUser(MK_UserData * pUserData, int iErrorCode)
{
	CString str;
	str.Format(L"%s ����ڰ� ������ �����߽��ϴ�.", pUserData->GetIP());
	m_pParent->AddEventString(str);

	// ����� ���� ��, Connected User ����Ʈ���� ����
	MyUserData* tempUser = FindUserDataInConnectedUserList(pUserData);

	m_pParent->SetUserDisconnected(tempUser);

	m_pParent->UpdateListBoxConnectedUser();
}

// ���μ����� ���� ���...
int MyServer::ProcessRecvData(SOCKET hSocket, unsigned char cMessageID, char * pRecvData, BS bsBodySize)
{
	MK_UserData* pUser = (MK_UserData*)FindUserData(hSocket);

	if (MID_CHAT_DATA == cMessageID)
	{
		CString str;
		str.Format(L"%s : %s", pUser->GetIP(), (wchar_t*)pRecvData);

		m_pParent->AddEventString(str);

		// Broadcast
		for (size_t i = 0; i < m_iMaxUserCount; ++i)
		{
			if (-1 != m_UserList[i]->GetHandle())
			{
				SendFrameData(m_UserList[i]->GetHandle(), MID_CHAT_DATA, (const char*)(const wchar_t*)str, (str.GetLength() + 1) * 2);
			}
		}
	}

	return 1;
}

void MyServer::ShowLimitError(const wchar_t* cIPAddress)
{
}

void MyServer::BroadcastToAll(SOCKET hSocket, char * pRecvData, DWORD size)
{
	CString test = (wchar_t*)pRecvData;

	MK_UserData* pUser = (MK_UserData*)FindUserData(hSocket);

	// Message ID Ȯ��
	char messageID;
	memcpy(&messageID, pRecvData + 1, 1);

	// ��� �κ��� �� BODY �κ�
	char tempString[SENDDATA_SIZE];
	memcpy(tempString, pRecvData + 4, size - 4);


	CString str;
	CString strID;
	CString strPW;
	MyUserData* pUserData = nullptr;
	int result = 0;
	int length = 0;


	switch (messageID)
	{
	case MID_CHAT_DATA:
		pUserData = FindUserDataInUserList(pUser);

		str.Format(L"%s : %s", pUserData->GetID(), (wchar_t*)tempString);

		m_pParent->AddEventString(str);

		// Broadcast
		for (size_t i = 0; i < m_iMaxUserCount; ++i)
		{
			if (-1 != m_UserList[i]->GetHandle())
			{
				SendFrameData(m_UserList[i]->GetHandle(), MID_CHAT_DATA, (const char*)(const wchar_t*)str, (str.GetLength() + 1) * 2);
			}
		}

		break;
	case MID_ID:
		/*CString test = L"fdddfddd 1234";
	int result = test.Find(L' ');
	int length = test.GetLength();
	CString ID = test.Mid(0, result);
	CString PW = test.Mid(result, length);*/

		str.Format(L"%s", tempString);

		// str�� ID �� password�� ������.
		result = str.Find(L' ');
		length = str.GetLength();
		strID = str.Mid(0, result);
		// space ���� ����
		strPW = str.Mid(result + 1, length);

		// ���� ID�� �ִ��� Ȯ��
		pUserData = FindUserDataID(strID);

		// ���� ID�� ����
		// ���ο� MyUserData ����
		if (nullptr == pUserData)
		{
			pUserData = CreateUserData(strID, strPW, pUser);
		}
		// ���� ID�� �ִ�... ���̵� �������.
		else
		{
			str = L"SAME ID";

			// broadecast �� �ƴ� �������� ������
			SendFrameData(pUser->GetHandle(), MID_ERROR, (const char*)(const wchar_t*)str, (str.GetLength() + 1) * 2);

			// assert(false);

			return;
		}
	
		str.Format(L"INSERT INTO all_user VALUES('%s', '%s')", pUserData->GetID(), pUserData->GetPassword());

		m_pParent->AddToSQL(str);

		break;
	case MID_LOGIN:
		str.Format(L"%s", tempString);
		// str�� ID �� password�� ������.
		result = str.Find(L' ');
		length = str.GetLength();
		strID = str.Mid(0, result);
		// space ���� ����
		strPW = str.Mid(result + 1, length);

		// ���� ID�� �ִ��� Ȯ��
		pUserData = FindUserDataID(strID);

		// ���� ID�� ���� => �α��� �Ұ�
		if (nullptr == pUserData)
		{
			str = ERRORID_LOGIN_NOID;
			SendFrameData(pUser->GetHandle(), MID_ERROR, (const char*)(const wchar_t*)str, (str.GetLength() + 1) * 2);

			return;
		}
		// ���� ID�� �ִ�... ��й�ȣ Ȯ��
		else
		{
			// ��й�ȣ�� �ٸ�
			if (strPW != pUserData->GetPassword())
			{
				str = ERRORID_LOGIN_WRONGPW;
				SendFrameData(pUser->GetHandle(), MID_ERROR, (const char*)(const wchar_t*)str, (str.GetLength() + 1) * 2);
			}
			// ��й�ȣ ��ġ
			else
			{
				str = ERRORID_LOGIN_APPROVED;
				SendFrameData(pUser->GetHandle(), MID_LOGIN, (const char*)(const wchar_t*)str, (str.GetLength() + 1) * 2);

				pUserData->SetUserData(pUser);

				// �α��� �Ϸ�
				// ���� ���� �߰�
				AddConnectedUser(pUserData);
				// ���� ���� â ������Ʈ
				m_pParent->UpdateListBoxConnectedUser();
			}
		}
		break;
	default:
		break;
	}

	//CString str;
	//str.Format(L"%s : %s", pUser->GetIP(), (wchar_t*)pRecvData);	
}

MyUserData * MyServer::FindUserDataID(const wchar_t * _ID)
{
	auto iter = m_UserDataList.begin();
	auto iterEnd = m_UserDataList.end();

	for (; iter != iterEnd; ++iter)
	{
		int result = strcmp( (const char*)(*iter)->GetID(), (const char*)_ID);

		// ������
		if (0 == result)
		{
			return (*iter);
		}
	}

	return nullptr;
}

MyUserData * MyServer::FindUserDataInUserList(MK_UserData * _userData)
{
	auto iter = m_UserDataList.begin();
	auto iterEnd = m_UserDataList.end();

	for (; iter != iterEnd; ++iter)
	{
		if (_userData == (*iter)->GetUserData())
		{
			return (*iter);
		}
	}

	return nullptr;
}

MyUserData * MyServer::FindUserDataInConnectedUserList(MK_UserData * _userData)
{
	auto iter = m_ConnectedUserDataList.begin();
	auto iterEnd = m_ConnectedUserDataList.end();

	for (; iter != iterEnd; ++iter)
	{
		if (_userData == (*iter)->GetUserData())
		{
			return (*iter);
		}
	}

	return nullptr;
}

MyUserData * MyServer::CreateUserData(const wchar_t * _ID, const wchar_t * _PW, MK_UserData * _userData)
{
	MyUserData* pUserData = new MyUserData;

	pUserData->SetID(_ID);
	pUserData->SetPassword(_PW);
	pUserData->SetUserData(_userData);
	m_UserDataList.push_back(pUserData);

	return pUserData;
}

void MyServer::AddUserData(MyUserData * _userData)
{
	m_UserDataList.push_back(_userData);
}

void MyServer::AddConnectedUser(MyUserData * _user)
{
	m_ConnectedUserDataList.push_back(_user);
}


afx_msg LRESULT CMKChatServerDlg::OnAcceptUser(WPARAM wParam, LPARAM lParam)
{
	m_Server.ProcessToAccept(wParam, lParam); // FD_ACCEPT

	return 0;
}


afx_msg LRESULT CMKChatServerDlg::OnReadAndClose(WPARAM wParam, LPARAM lParam)
{
	m_Server.ProcessClientEvent(wParam, lParam); // FD_READ, FD_CLOSE

	return 0;
}


void CMKChatServerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// ������ ����Ǿ� �ִ� ��쿡�� ����. 
	// ������ ������ �����ߴٸ�
	// ������ �������� �Ʒ��� ������ �����ȴ�.
	if (1 == m_ConnectFlag)
	{
		CloseODBC();
	}

}


// ����Ʈ ������Ʈ ��ư
void CMKChatServerDlg::OnBnClickedOk2()
{
	UpdateListBoxAllUser();
}






////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

MyUserData::MyUserData()
{
}

MyUserData::~MyUserData()
{
}

void MyUserData::SetUserData(MK_UserData* _userData)
{
	m_pUserData = _userData;
}

wchar_t * MyUserData::GetID()
{
	return m_ID;
}

wchar_t * MyUserData::GetPassword()
{
	return m_PW;
}

void MyUserData::SetID(const wchar_t * cID)
{
	for (size_t i = 0; i < ID_LENGTH; ++i)
	{
		m_ID[i] = cID[i];
	}
}

void MyUserData::SetPassword(const wchar_t * cPassword)
{
	if (nullptr == cPassword)
	{
		return;
	}

	for (size_t i = 0; i < PW_LENGTH; ++i)
	{
		m_PW[i] = cPassword[i];
	}
}