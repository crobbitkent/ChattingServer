
// MKChatClient.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "MKChatClient.h"
#include "MKChatClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMKChatClientApp

BEGIN_MESSAGE_MAP(CMKChatClientApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMKChatClientApp construction

CMKChatClientApp::CMKChatClientApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CMKChatClientApp object

CMKChatClientApp theApp;


// CMKChatClientApp initialization

BOOL CMKChatClientApp::InitInstance()
{
	CWinApp::InitInstance();

	CMKChatClientDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	return FALSE;
}

