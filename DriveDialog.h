
#ifndef DRIVEDIALOG_H
#define DRIVEDIALOG_H

struct CDriveInfo {
	void Free(void);
	void LoadDriveInfo(char letter);

	char letter;
	BOOL avail;
	HICON icon;
	char *longname;
};

class CDriveDialog : public CDialog {
public:
	CDriveDialog(CWnd* pParent = NULL);

	//{{AFX_VIRTUAL(CDriveDialog)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CDriveDialog)
	afx_msg BOOL OnInitDialog();
	afx_msg void OnDriveDblClk(NMHDR *pNotifyStruct, LRESULT *result);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CDriveInfo *driveinfo;
};

#endif
