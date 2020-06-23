
#include "stdafx.h"
#include "spacemonger.h"
#include "AboutDialog.h"
#include "Lang.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CAboutDialog::CAboutDialog(CWnd* pParent)
	: CDialog(IDD_ABOUT, pParent)
{
}

BEGIN_MESSAGE_MAP(CAboutDialog, CDialog)
	//{{AFX_MSG_MAP(CAboutDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(CurLang->about_spacemonger);
	SetDlgItemText(IDC_STATIC_FREEWARE, CurLang->freeware);
	SetDlgItemText(IDC_STATIC_WARRANTY, CurLang->warranty);
	SetDlgItemText(IDC_STATIC_EMAIL, CurLang->email);
	SetDlgItemText(IDOK, CurLang->ok);

	return TRUE;
}
