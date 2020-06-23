#ifndef FOLDERVIEW_H
#define FOLDERVIEW_H

#include "FolderTree.h"
#include "TipWnd.h"

struct CDisplayFolder {
	char *name;
	si32 depth;
	ui32 flags;				// 1=folder
	CFolder *source;
	ui32 index;
	si16 x, y, w, h;
	CDisplayFolder *next;
};

class CFolderView : public CFreeView {
public:
	CFolderView();
	DECLARE_DYNCREATE(CFolderView)
	virtual ~CFolderView();

	virtual void SetDocument(CFreeDoc *doc = NULL);
	virtual void SetPalette(void);

	void BuildTitleReverse(CFolder *folder, CString &string);
	CDisplayFolder *GetDisplayFolderFromPoint(const CPoint &point);
	CDisplayFolder *GetContainerDisplayFolderFromPoint(const CPoint &point);
	void HighlightPathAtPoint(const CPoint &point);

	void ZoomIn(CDisplayFolder *folder);
	void ZoomOut(void);
	void ZoomFull(void);
	void ShowFreeSpace(BOOL show);

	inline bool IsAnythingOpen(void)
		{ return rootfolder != NULL; }
	inline bool IsAnythingSelected(void)
		{ return selected != NULL; }
	inline bool IsSelectedAFolder(void)
		{ return selected != NULL && (selected->flags & 1) != 0; }
	inline bool IsZoomFull(void)
		{ return zoomlevel == 0; }

	void UpdateTitleBar(void);

	void SelectFolder(CDisplayFolder *cur);

	//{{AFX_VIRTUAL(CFolderView)
protected:
	virtual void OnDraw(CDC *pDC);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CFolderView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT flags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT flags, CPoint point);
	afx_msg void OnRButtonUp(UINT flags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg void OnIgnoreUpdate(CCmdUI *ui);
	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pResult);
	afx_msg void OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized);
	DECLARE_MESSAGE_MAP()

public:
	virtual void OnUpdate(CFreeDoc *doc);

protected:
	void BuildFolderLayout(int x, int y, int w, int h, CFolder *folder, int depth);
	void SizeFolders(int x, int y, int w, int h, CFolder *folder, int *index, int numindices, int depth);
	CDisplayFolder *AddDisplayFolder(CFolder *source, ui32 index,
		si32 depth, si16 x, si16 y, si16 w, si16 h, ui32 flags);
	void ClearDisplayFolders(void);
	void DrawDisplayFolder(CDC *pDC, CDisplayFolder *cur, BOOL selected);
	void MinimalDrawDisplayFolder(CDC *pDC, CDisplayFolder *cur, BOOL selected);
	void AnimateBox(const CRect &start, const CRect &end);
	void SetupInfoTip(CDisplayFolder *cur);
	void SetupNameTip(CDisplayFolder *cur);

protected:
	CPalette m_palette;
	CBrush black, white;
	CFont minifont;
	CFolder *rootfolder;
	CDisplayFolder *displayfolders, *displayend;
	int vmin, hmin;
	CTipWnd m_infotipwnd;
	CTipWnd m_nametipwnd;
	CDisplayFolder *lastcur;

public:
	int zoomlevel;
	CDisplayFolder *selected;
	BOOL showfreespace;
};

#endif
