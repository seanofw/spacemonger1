
#ifndef CMAINFRAME_H
#define CMAINFRAME_H

class CMainToolBar : public CToolBar {
public:
	CMainToolBar();
	virtual ~CMainToolBar();
	DECLARE_DYNCREATE(CMainToolBar)

	virtual int Create(CWnd *parent);

	enum {
		NoState = 0,
		ViewFree,
	};

	virtual void EnableState(int state, BOOL enabled);
	void UpdateButtonsForView(class CFolderView *view);

	void Reload(void);

protected:
	//{{AFX_MSG(CMainToolBar)
	afx_msg void OnIgnoreUpdate(CCmdUI *ui);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

class CMainFrame : public CFrameWnd {
public:
	CMainFrame();
	virtual ~CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)
	BOOL PreCreateWindow(CREATESTRUCT& cs);

	virtual void RecalcLayout(BOOL bNotify);

protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy(void);
	afx_msg void OnSysCommand(UINT nid, LPARAM lparam);
	afx_msg void OnClose();
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnWindowPosChanged(WINDOWPOS *wp);
	afx_msg void OnShowWindow(BOOL bShow, UINT status);
	//}}AFX_MSG
	afx_msg void OnIgnoreUpdate(CCmdUI *ui);
	DECLARE_MESSAGE_MAP()

public:
	CMainToolBar m_toolbar;
};

#endif

