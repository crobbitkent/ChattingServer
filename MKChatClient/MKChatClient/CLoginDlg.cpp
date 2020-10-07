// CLoginDlg.cpp : implementation file
//

#include "pch.h"
#include "MKChatClient.h"
#include "CLoginDlg.h"
#include "afxdialogex.h"

#include "MKChatClientDlg.h"

#include "CRegistrationDlg.h"

// CLoginDlg dialog

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

CLoginDlg::CLoginDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGIN_DIALOG, pParent)
{

}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_REGISTRATION, &CLoginDlg::OnBnClickedButtonRegistration)
	ON_BN_CLICKED(ID_BUTTON_LOGIN, &CLoginDlg::OnBnClickedButtonLogin)
END_MESSAGE_MAP()


// CLoginDlg message handlers


void CLoginDlg::OnBnClickedButtonRegistration()
{
	// 회원 가입 창
	CRegistrationDlg* reg = new CRegistrationDlg;
	m_pParentDlg->SetRegDlg(reg);
	reg->SetParentDlg(m_pParentDlg);

	reg->DoModal();
}

void CLoginDlg::ErrorNoID()
{
	SetDlgItemText(IDC_STATIC_ERROR, L"ID를 찾을 수 없습니다.");
}

void CLoginDlg::ErrorWrongPW()
{
	SetDlgItemText(IDC_STATIC_ERROR, L"비밀번호가 틀립니다.");
}

void CLoginDlg::FinishLogin()
{
	EndDialog(0);
}



void CLoginDlg::OnBnClickedButtonLogin()
{
	CString strID;
	GetDlgItemText(IDC_ID, strID);

	CString strPW;
	GetDlgItemText(IDC_PW, strPW);

	// L' ' 이 있는지 확인
	wchar_t space = L' ';
	// wcsstr()

	const wchar_t* result1 = wcsstr((const wchar_t*)strID, L" ");
	const wchar_t* result2 = wcsstr((const wchar_t*)strID, L" ");

	//if (nullptr != result1 || nullptr != result2)
	//{
	//	SetDlgItemText(IDC_STATIC_ERROR, L"특수문자를 넣을 수 없습니다.");
	//	return;
	//}
		

	wchar_t finalStr[27];
	wcscpy_s(finalStr, strID);
	wcscat_s(finalStr, L" ");
	wcscat_s(finalStr, strPW);
	CString strIDPW = finalStr;

	m_pParentDlg->SendLogin((const wchar_t*)strIDPW, strIDPW.GetLength());

	// SetDlgItemText(IDC_STATIC_ERROR, L"회원 가입이 완료되었습니다.");
	

}
