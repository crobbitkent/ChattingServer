
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
	// 리스트 박스 가장 아래쪽에 추가
	int index = m_ListBox_EventList.InsertString(-1, pString);

	// 문자열이 추가된 위치에 선택을 설정하여 바로 확인 가능하도록
	m_ListBox_EventList.SetCurSel(index);
}

// 모든 유저 목록 업데이트
void CMKChatServerDlg::UpdateListBoxAllUser()
{
	// listbox에 저장된 항목들을 모두 제거
	m_list_AllUser.ResetContent();

	// all_user 테이블에 저장된 데이터를 모두 읽는 SQL 명령문
	wchar_t query_str[256] = L"SELECT * FROM all_user";

	// 읽어온 데이터의 상태를 기록할 변수
	unsigned short record_state[MAX_DATA];
	// 읽어온 데이터를 저장할 변수
	UserData raw_data[MAX_DATA];

	CString str;

	// 데이터를 저장할 배열을 초기화
	memset(raw_data, 0, sizeof(raw_data));

	HSTMT hStatement;
	RETCODE retCode;

	// Query문을 위한 메모리를 할당
	if (SQL_SUCCESS == SQLAllocHandle(SQL_HANDLE_STMT, m_hODBC, &hStatement))
	{
		// 읽은 데이터의 개수를 저장할 변수
		unsigned long recordCount = 0;

		// Query문을 실행할 때 타임아웃을 설정
		SQLSetStmtAttr(hStatement, SQL_ATTR_QUERY_TIMEOUT, (SQLPOINTER)15, SQL_IS_UINTEGER);

		// 가져온 데이터를 저장할 메모리의 크기를 설정
		SQLSetStmtAttr(hStatement, SQL_ATTR_ROW_BIND_TYPE, (SQLPOINTER)sizeof(raw_data), 0);

		// 데이터를 가져올때 동시성에 대한 방식 설정
		SQLSetStmtAttr(hStatement, SQL_ATTR_CONCURRENCY, (SQLPOINTER)SQL_CONCUR_ROWVER, SQL_IS_UINTEGER);

		// 검색된 데이터의 위치를 기억하는 방식을 설정
		SQLSetStmtAttr(hStatement, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_KEYSET_DRIVEN, SQL_IS_UINTEGER);

		// 데이터를 가져오는 최대 단위 설정
		SQLSetStmtAttr(hStatement, SQL_ATTR_ROW_NUMBER, (SQLPOINTER)MAX_DATA, SQL_IS_UINTEGER);

		// 읽은 데이터의 상태를 저장할 변수의 주소를 전달
		SQLSetStmtAttr(hStatement, SQL_ATTR_ROW_STATUS_PTR, record_state, 0);

		// 읽은 데이터의 개수를 저장할 변수의 주소를 전달
		SQLSetStmtAttr(hStatement, SQL_ATTR_ROWS_FETCHED_PTR, &recordCount, 0);

		// 테이블에서 가져온 데이터를 속성별로 raw_data 변수에 저장하기 위해서
		// 속성별로 저장할 메모리 위치를 설정
		SQLBindCol(hStatement, 1, SQL_WCHAR, raw_data[0].ID, sizeof(wchar_t) * 12, NULL);
		SQLBindCol(hStatement, 2, SQL_WCHAR, raw_data[0].PW, sizeof(wchar_t) * 16, NULL);
		// SQLBindCol(hStatement, 1, SQL_WCHAR, raw_data[0].ID, sizeof(wchar_t) * 12, NULL);

		// SQL 명령문을 실행
		retCode = SQLExecDirect(hStatement, (SQLWCHAR*)query_str, SQL_NTS);

		// SQL 명령문의 실행결과로 받은 데이터를 listbox에 추가
		while (retCode = SQLFetchScroll(hStatement, SQL_FETCH_NEXT, 0) != SQL_NO_DATA)
		{
			// 데이터 개수만큼 반복하면서 작업
			for (unsigned int i = 0; i < recordCount; ++i)
			{
				// 가져운 데이터가 삭제된 정보가 아니라면 해당 속성으로
				// 합쳐서 문자열로 구성하고 listbox에 업데이트
				if (SQL_ROW_DELETED != record_state[i] && SQL_ROW_ERROR != record_state[i])
				{
					// 같은 Id가 있는지 찾아보기
					wchar_t* tempID = raw_data[i].ID;

					// 같은 id가 없다면
					if (nullptr == m_Server.FindUserDataID(tempID))
					{
						// 생성 및 userlist에 추가
						m_Server.CreateUserData(tempID, raw_data[i].PW);
					}
					else
					{
						// 같은 id가 있다면 넘기기
						continue;
					}

					//str.Format(L"%s", raw_data[i].ID);
					//m_list_AllUser.InsertString(-1, str);				
					//str.Format(L"%s", raw_data[i].PW);
					//m_list_Password.InsertString(-1, str);
				}
			}
		}

		// List All User 에 업데이트
		UpdateListBoxString();

		// Query문을 위해 할당한 메모리를 해제
		SQLFreeHandle(SQL_HANDLE_STMT, hStatement);
	}
}

// 통째로 추가
void CMKChatServerDlg::UpdateListBoxString()
{
	// 리스트 모든 내용 삭제
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
	// 리스트 모든 내용 삭제
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
	// ODBC 드라이버에 연결을 위한 기본 정보를 설정
	SQLSetEnvAttr(NULL, SQL_ATTR_CONNECTION_POOLING, (SQLPOINTER)SQL_CP_ONE_PER_DRIVER, SQL_IS_INTEGER);

	// ODBC 기술을 사용하기 위한 환경을 구성
	// 구성된 환경 정보에 대한 핸들값은 m_hEnvironment에 저장
	if (SQL_ERROR != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnvironment))
	{
		// 사용할 ODBC 버전 정보를 설정
		SQLSetEnvAttr(m_hEnvironment, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
		SQLSetEnvAttr(m_hEnvironment, SQL_ATTR_CP_MATCH, (SQLPOINTER)SQL_CP_RELAXED_MATCH, SQL_IS_INTEGER);

		// ODBC 함수를 사용하기 위한 정보를 구성
		// 이정보에 대한 핸들값은 m_hODBC에 저장
		if (SQL_ERROR != SQLAllocHandle(SQL_HANDLE_DBC, m_hEnvironment, &m_hODBC))
		{
			RETCODE retcode = SQLConnect(m_hODBC,
				// 접속할 DSN 설정
				(SQLWCHAR*)L"my_users", SQL_NTS,
				// 접속에 사용할 ID
				(SQLWCHAR*)L"crobbitkent", SQL_NTS,
				// 접속에 사용할 password
				(SQLWCHAR*)L"alsrb5779", SQL_NTS);

			if (SQL_SUCCESS == retcode || SQL_SUCCESS_WITH_INFO == retcode)
			{
				// ODBC를 사용하여 데이터 베이스 서버에 성공적으로 접속한 경우
				m_ConnectFlag = 1;

				return;
			}
			else
			{
				// SQL 접속 실패
				assert(false);

				CloseODBC();
			}
		}
	}
}

void CMKChatServerDlg::CloseODBC()
{
	// 접속에 실패한 경우, 구성했던 메모리를 제거
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

	// Query문을 위한 메모리 할당
	if (SQL_SUCCESS == SQLAllocHandle(SQL_HANDLE_STMT, m_hODBC, &hStatement) )
	{
		// query문을 실행할 때 타임아웃을 설정
		SQLSetStmtAttr(hStatement, SQL_ATTR_QUERY_TIMEOUT, (SQLPOINTER)15, SQL_IS_UINTEGER);

		// SQL 명령문 실행
		RETCODE ret = SQLExecDirect(hStatement, (SQLWCHAR*)_sql, SQL_NTS);

		// 성공적으로 완료했는지 체크
		if (SQL_SUCCESS == ret || SQL_SUCCESS_WITH_INFO == ret)
		{
			// 성공
		}
		else
		{
			assert(false);
		}

		// 명령 수행이 완료되었다는 것을 설정(commit)
		SQLEndTran(SQL_HANDLE_ENV, m_hEnvironment, SQL_COMMIT);
		// Query문을 위해 할당한 메모리 해제
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

	// SQL 연결하기
	InitODBC();

	// SQL 정보 불러오기
	UpdateListBoxAllUser();

	SERVER_TYPE serverType = ST_IOCP;

	// 서버 시작
	// 192.168.0.9
	// 27100 이상의 번호를 사용하라.
	m_Server.StartServer(L"192.168.0.10", 27444, m_hWnd, serverType);
	AddEventString(L"서버 서비스를 시작합니다.");

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
	str.Format(L"%s에서 새로운 사용자가 접속했습니다.", pUserData->GetIP());
	m_pParent->AddEventString(str);
}

void MyServer::AddWorkForCloseUser(MK_UserData * pUserData, int iErrorCode)
{
	CString str;
	str.Format(L"%s 사용자가 접속을 해제했습니다.", pUserData->GetIP());
	m_pParent->AddEventString(str);

	// 사용자 해제 시, Connected User 리스트에서 제외
	MyUserData* tempUser = FindUserDataInConnectedUserList(pUserData);

	m_pParent->SetUserDisconnected(tempUser);

	m_pParent->UpdateListBoxConnectedUser();
}

// 프로세서를 통한 방법...
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

	// Message ID 확인
	char messageID;
	memcpy(&messageID, pRecvData + 1, 1);

	// 헤드 부분을 뺀 BODY 부분
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

		// str을 ID 와 password로 가른다.
		result = str.Find(L' ');
		length = str.GetLength();
		strID = str.Mid(0, result);
		// space 공간 띄우고
		strPW = str.Mid(result + 1, length);

		// 같은 ID가 있는지 확인
		pUserData = FindUserDataID(strID);

		// 같은 ID가 없음
		// 새로운 MyUserData 생성
		if (nullptr == pUserData)
		{
			pUserData = CreateUserData(strID, strPW, pUser);
		}
		// 같은 ID가 있다... 아이디 못만든다.
		else
		{
			str = L"SAME ID";

			// broadecast 가 아닌 개인으로 보내기
			SendFrameData(pUser->GetHandle(), MID_ERROR, (const char*)(const wchar_t*)str, (str.GetLength() + 1) * 2);

			// assert(false);

			return;
		}
	
		str.Format(L"INSERT INTO all_user VALUES('%s', '%s')", pUserData->GetID(), pUserData->GetPassword());

		m_pParent->AddToSQL(str);

		break;
	case MID_LOGIN:
		str.Format(L"%s", tempString);
		// str을 ID 와 password로 가른다.
		result = str.Find(L' ');
		length = str.GetLength();
		strID = str.Mid(0, result);
		// space 공간 띄우고
		strPW = str.Mid(result + 1, length);

		// 같은 ID가 있는지 확인
		pUserData = FindUserDataID(strID);

		// 같은 ID가 없음 => 로그인 불가
		if (nullptr == pUserData)
		{
			str = ERRORID_LOGIN_NOID;
			SendFrameData(pUser->GetHandle(), MID_ERROR, (const char*)(const wchar_t*)str, (str.GetLength() + 1) * 2);

			return;
		}
		// 같은 ID가 있다... 비밀번호 확인
		else
		{
			// 비밀번호가 다름
			if (strPW != pUserData->GetPassword())
			{
				str = ERRORID_LOGIN_WRONGPW;
				SendFrameData(pUser->GetHandle(), MID_ERROR, (const char*)(const wchar_t*)str, (str.GetLength() + 1) * 2);
			}
			// 비밀번호 일치
			else
			{
				str = ERRORID_LOGIN_APPROVED;
				SendFrameData(pUser->GetHandle(), MID_LOGIN, (const char*)(const wchar_t*)str, (str.GetLength() + 1) * 2);

				pUserData->SetUserData(pUser);

				// 로그인 완료
				// 접속 유저 추가
				AddConnectedUser(pUserData);
				// 접속 유저 창 업데이트
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

		// 같으면
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

	// 서버에 연결되어 있는 경우에만 제거. 
	// 서버에 연결을 실패했다면
	// 실패한 시점에서 아래의 정보가 정리된다.
	if (1 == m_ConnectFlag)
	{
		CloseODBC();
	}

}


// 리스트 업데이트 버튼
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