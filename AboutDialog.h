#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

class CAboutDialog : public CDialog {
public:
	CAboutDialog(CWnd* pParent = NULL);

protected:
	//{{AFX_MSG(CAboutDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
