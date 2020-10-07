#pragma once


// CRegistrationDlg dialog

class CRegistrationDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRegistrationDlg)

public:
	CRegistrationDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CRegistrationDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REGISTRATION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonReg();

	void ErrorSameID();

	void SetParentDlg(class CMKChatClientDlg* dlg) { m_pParentDlg = dlg; }

private:
	class CMKChatClientDlg* m_pParentDlg;
};
