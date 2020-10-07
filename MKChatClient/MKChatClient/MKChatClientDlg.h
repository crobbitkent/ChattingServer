
// MKChatClientDlg.h : header file
//

#pragma once

#define MID_CHAT_DATA 1
#define MID_REG_ID 2
#define MID_REG_PW 3
#define MID_ERROR 4
#define MID_LOGIN 5


#define ERRORID_REG_SAMEID L"SAME ID"
#define ERRORID_LOGIN_NOID L"NO ID"
#define ERRORID_LOGIN_WRONGPW L"WRONG PW"
#define ERRORID_LOGIN_APPROVED L"APPROVED"


class CMKChatClientDlg;

class MyClient : public MK_ClientSocket
{
public:
	MyClient() : MK_ClientSocket(0x27) {}

	MyClient(CMKChatClientDlg* pParent)
		: MK_ClientSocket(0x27)
	{
		m_pParent = pParent;
	}

	virtual int ProcessRecvData(SOCKET hSocket, unsigned char cMessageID, char* pRecvData, BS bsBodySize);

	void SetParent(CMKChatClientDlg* dlg) { m_pParent = dlg; }

private:
	CMKChatClientDlg* m_pParent;
};







// CMKChatClientDlg dialog
class CMKChatClientDlg : public CDialogEx
{
// Construction
public:
	CMKChatClientDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MKCHATCLIENT_DIALOG };
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
	afx_msg void OnBnClickedButton1();
	CListBox m_ListBox_EventList;

	void AddEventString(const wchar_t* pString);

	// 서버에 회원가입
	void SendRegistration(const wchar_t* _ID, int _idLength);

	// 서버에 로그인
	void SendLogin(const wchar_t* _ID, int _idLength);

	void SetRegDlg(class CRegistrationDlg* dlg) { m_pRegDlg = dlg; }
	void SetLoginDlg(class CLoginDlg* dlg) { m_pLoginDlg = dlg; }
	class CLoginDlg* GetLoginDlg() const {		return m_pLoginDlg;	}
	class CRegistrationDlg* GetRegDlg() const { return m_pRegDlg; }
private:
	static MyClient m_Client;
	class CLoginDlg* m_pLoginDlg;
	class CRegistrationDlg* m_pRegDlg;

protected:
	afx_msg LRESULT OnConnect(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReadAndClose(WPARAM wParam, LPARAM lParam);
};
