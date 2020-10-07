#pragma once


// CLoginDlg dialog

class CLoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CLoginDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGIN_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonRegistration();

	void SetParentDlg(class CMKChatClientDlg* dlg) { m_pParentDlg = dlg; }

	void ErrorNoID();
	void ErrorWrongPW();

	void FinishLogin();
	
private:
	class CMKChatClientDlg* m_pParentDlg;
public:
	afx_msg void OnBnClickedButtonLogin();
};
