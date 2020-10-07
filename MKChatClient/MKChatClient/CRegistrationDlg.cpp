// CRegistrationDlg.cpp : implementation file
//

#include "pch.h"
#include "MKChatClient.h"
#include "CRegistrationDlg.h"
#include "afxdialogex.h"

#include "MKChatClientDlg.h"

// CRegistrationDlg dialog

IMPLEMENT_DYNAMIC(CRegistrationDlg, CDialogEx)

CRegistrationDlg::CRegistrationDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REGISTRATION, pParent)
{

}

CRegistrationDlg::~CRegistrationDlg()
{
}

void CRegistrationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRegistrationDlg, CDialogEx)
	ON_BN_CLICKED(ID_BUTTON_REG, &CRegistrationDlg::OnBnClickedButtonReg)
END_MESSAGE_MAP()


// CRegistrationDlg message handlers


void CRegistrationDlg::OnBnClickedButtonReg()
{
	CString strID;
	GetDlgItemText(IDC_ID, strID);

	CString strPW;
	GetDlgItemText(IDC_PW, strPW);

	CString strPWCheck;
	GetDlgItemText(IDC_PW_CHECK, strPWCheck);

	// L' ' 이 있는지 확인
	wchar_t space = L' ';
	// wcsstr()
	const wchar_t* result1 = wcsstr((const wchar_t*)strID, L" ");
	const wchar_t* result2 = wcsstr((const wchar_t*)strID, L" ");
	const wchar_t* result3 = wcsstr((const wchar_t*)strID, L" ");

	if (nullptr != result1 || nullptr != result2 || nullptr != result3)
	{
		SetDlgItemText(IDC_STATIC_ERROR, L"특수문자를 넣을 수 없습니다.");
		return;
	}

	if (strPW != strPWCheck)
	{
		SetDlgItemText(IDC_STATIC_ERROR, L"비밀번호가 맞지 않습니다.");
		return;
	}
	else
	{
		// CMKChatClientDlg client;

		wchar_t finalStr[27];
		wcscpy_s(finalStr, strID);
		wcscat_s(finalStr, L" ");
		wcscat_s(finalStr, strPW);
		CString strIDPW = finalStr;

		m_pParentDlg->SendRegistration((const wchar_t*)strIDPW, strIDPW.GetLength());

		SetDlgItemText(IDC_STATIC_ERROR, L"회원 가입이 완료되었습니다.");
	}


}

void CRegistrationDlg::ErrorSameID()
{
	SetDlgItemText(IDC_STATIC_ERROR, L"같은 ID가 이미 존재합니다.");
}
