
// MKChatClientDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "MKChatClient.h"
#include "MKChatClientDlg.h"
#include "afxdialogex.h"

#include "CLoginDlg.h"
#include "CRegistrationDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMKChatClientDlg dialog

MyClient CMKChatClientDlg::m_Client;

CMKChatClientDlg::CMKChatClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MKCHATCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// TEST
	/*CString test = L"fdddfddd 1234";
	int result = test.Find(L' ');
	int length = test.GetLength();
	CString ID = test.Mid(0, result);
	CString PW = test.Mid(result, length);*/

	int a = 0;
}

void CMKChatClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EVENT_LIST, m_ListBox_EventList);
}

BEGIN_MESSAGE_MAP(CMKChatClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMKChatClientDlg::OnBnClickedButton1)
	ON_MESSAGE(26001, &CMKChatClientDlg::OnConnect)
	ON_MESSAGE(26002, &CMKChatClientDlg::OnReadAndClose)
END_MESSAGE_MAP()


// CMKChatClientDlg message handlers

BOOL CMKChatClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Client.SetParent(this);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	
	
	// 서버 접속
	m_Client.ConnectToServer(L"192.168.0.10", 27444, m_hWnd);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMKChatClientDlg::OnPaint()
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
HCURSOR CMKChatClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// 전송 버튼
void CMKChatClientDlg::OnBnClickedButton1()
{
	CString str;
	GetDlgItemText(IDC_CHAT_EDIT, str);
	SetDlgItemText(IDC_CHAT_EDIT, L"");

	if (m_Client.IsConnected())
	{
		m_Client.SendFrameData(m_Client.GetHandle(), MID_CHAT_DATA, (char*)(const wchar_t*)str, (str.GetLength() + 1) * 2 );
	}
}

void CMKChatClientDlg::AddEventString(const wchar_t * pString)
{
	int index = m_ListBox_EventList.InsertString(-1, pString);
	m_ListBox_EventList.SetCurSel(index);
}

void CMKChatClientDlg::SendRegistration(const wchar_t * _ID, int _idLength)
{
	if (m_Client.IsConnected())
	{
		CString test = _ID;

		m_Client.SendFrameData(m_Client.GetHandle(), MID_REG_ID, (char*)_ID, (_idLength + 1) * 2);
	}
}

void CMKChatClientDlg::SendLogin(const wchar_t * _ID, int _idLength)
{
	if (m_Client.IsConnected())
	{
		CString test = _ID;

		m_Client.SendFrameData(m_Client.GetHandle(), MID_LOGIN, (char*)_ID, (_idLength + 1) * 2);
	}
}









int MyClient::ProcessRecvData(SOCKET hSocket, unsigned char cMessageID, char * pRecvData, BS bsBodySize)
{
	wchar_t* tempString = (wchar_t*)pRecvData;

	if (MID_CHAT_DATA == cMessageID)
	{
		// tempString += 4;

		m_pParent->AddEventString(tempString);
	}
	else if (MID_ERROR == cMessageID)
	{	
		CString message = tempString;

		if (ERRORID_REG_SAMEID == message)
		{
			m_pParent->GetRegDlg()->ErrorSameID();
		}
		else if (ERRORID_LOGIN_NOID == message)
		{
			m_pParent->GetLoginDlg()->ErrorNoID();
		}
		else if (ERRORID_LOGIN_WRONGPW == message)
		{
			m_pParent->GetLoginDlg()->ErrorWrongPW();
		}
		else
		{
			// 로그인 승인
			// m_pParent->GetLoginDlg()->FinishLogin();
		}



	}
	else if (MID_LOGIN == cMessageID)
	{
		// 로그인 승인
		m_pParent->GetLoginDlg()->FinishLogin();
	}

	return 1;
}


afx_msg LRESULT CMKChatClientDlg::OnConnect(WPARAM wParam, LPARAM lParam)
{
	int result = m_Client.ResultOfConnection(lParam);

	if (1 == result)
	{
		AddEventString(L"MK채팅 서버에 접속했습니다.");

		// 로그인 창 띄우기
		CLoginDlg* login = new CLoginDlg;

		m_pLoginDlg = login;
		m_pLoginDlg->SetParentDlg(this);
		login->DoModal();
	}
	else
	{
		AddEventString(L"MK채팅 서버에 접속할 수 없습니다.");
	}

	return 0;
} 


afx_msg LRESULT CMKChatClientDlg::OnReadAndClose(WPARAM wParam, LPARAM lParam)
{
	m_Client.ProcessServerEvent(wParam, lParam);

	return 0;
}
