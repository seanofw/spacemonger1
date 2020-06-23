
#ifndef XAPP_H
#define XAPP_H

class CFreeDoc;
class CFreeView;

/******************************************************************************
**  Painting utility functions
*/

#define ColorWindowBack (GetSysColorBrush(COLOR_APPWORKSPACE))
#define ColorText (GetSysColorBrush(COLOR_WINDOWTEXT))
#define ColorTextBack (GetSysColorBrush(COLOR_WINDOW))
#define ColorBright (GetSysColorBrush(COLOR_3DHIGHLIGHT))
#define ColorFlat (GetSysColorBrush(COLOR_3DFACE))
#define ColorDark (GetSysColorBrush(COLOR_3DSHADOW))
#define ColorBorder (GetSysColorBrush(COLOR_3DDKSHADOW))
#define ColorActive (GetSysColorBrush(COLOR_ACTIVECAPTION))
#define ColorInactive (GetSysColorBrush(COLOR_INACTIVECAPTION))
	
/*
**  DrawHLine
**    Draw a horizontal line in a given brush.
*/

inline static void DrawHLine(HDC dc, HBRUSH brush, int x, int y, int length)
{
	RECT rect;
	rect.top = y, rect.bottom = y + 1;
	rect.left = x, rect.right = x + length;
	FillRect(dc, &rect, brush);
}

/*
**  DrawVLine
**    Draw a vertical line in a given brush.
*/

inline static void DrawVLine(HDC dc, HBRUSH brush, int x, int y, int length)
{
	RECT rect;
	rect.top = y, rect.bottom = y + length;
	rect.left = x, rect.right = x + 1;
	FillRect(dc, &rect, brush);
}

/*
**  FillBox
**    Fill a rectangle in a given brush.
*/

inline static void FillBox(HDC dc, HBRUSH brush, int x, int y, int width, int height)
{
	RECT rect;
	rect.top = y, rect.bottom = y + height;
	rect.left = x, rect.right = x + width;
	FillRect(dc, &rect, brush);
}

/*
**  DrawBox
**    Draw a rectangle in a given brush.
*/

inline static void DrawBox(HDC dc, HBRUSH brush, int x, int y, int width, int height)
{
	RECT rect;
	rect.top = y, rect.bottom = y + height;
	rect.left = x, rect.right = x + width;
	FrameRect(dc, &rect, brush);
}

/*
**  DrawDualBox
**    Draw a two-color rectangle in a given pair of brushes.
*/

inline static void DrawDualBox(HDC dc, HBRUSH topleft, HBRUSH bottomright,
	int x, int y, int width, int height)
{
	RECT rect;

	rect.top = y, rect.bottom = y + 1;
	rect.left = x, rect.right = x + width;
	FillRect(dc, &rect, topleft);
	rect.top = y + height - 1, rect.bottom = y + height;
	FillRect(dc, &rect, bottomright);
	rect.top = y, rect.bottom = y + height;
	rect.left = x, rect.right = x + 1;
	FillRect(dc, &rect, topleft);
	rect.left = x + width - 1, rect.right = x + width;
	FillRect(dc, &rect, bottomright);
}

/*
**  FillDualBox
**    Draw a two-color rectangle in a given pair of brushes, and fill
**    in the area it encloses with a third color.
*/

inline static void FillDualBox(HDC dc, HBRUSH topleft, HBRUSH bottomright,
	HBRUSH fill, int x, int y, int width, int height)
{
	RECT rect;

	rect.top = y, rect.bottom = y + 1;
	rect.left = x, rect.right = x + width;
	FillRect(dc, &rect, topleft);
	rect.top = y + height - 1, rect.bottom = y + height;
	FillRect(dc, &rect, bottomright);
	rect.top = y, rect.bottom = y + height;
	rect.left = x, rect.right = x + 1;
	FillRect(dc, &rect, topleft);
	rect.left = x + width - 1, rect.right = x + width;
	FillRect(dc, &rect, bottomright);
	rect.top = y + 1, rect.bottom = y + width - 2;
	rect.left = x + 1, rect.right = x + width - 2;
	FillRect(dc, &rect, fill);
}

/*
**  Macros to build some common UI objects
*/

#define DrawRaisedBox(dc, x, y, width, height) \
	(DrawDualBox(dc, ColorBright, ColorDark, x, y, width, height))

#define DrawLoweredBox(dc, x, y, width, height) \
	(DrawDualBox(dc, ColorDark, ColorBright, x, y, width, height))

#define DrawEditBox(dc, x, y, width, height) \
	(DrawDualBox(dc, ColorDark, ColorBright, x, y, width, height), \
	 DrawDualBox(dc, ColorBorder, ColorFlat, (x)+1, (y)+1, (width)-2, (height)-2))

#define DrawWindowBorder(dc, x, y, width, height) \
	(DrawDualBox(dc, ColorFlat, ColorBorder, x, y, width, height), \
	 DrawDualBox(dc, ColorBright, ColorDark, (x)+1, (y)+1, (width)-2, (height)-2))

#define FillWindowBorder(dc, x, y, width, height) \
	(DrawDualBox(dc, ColorFlat, ColorBorder, x, y, width, height), \
	 FillDualBox(dc, ColorBright, ColorDark, ColorFlat, (x)+1, (y)+1, (width)-2, (height)-2))

#define DrawButtonUp(dc, x, y, width, height) \
	(DrawDualBox(dc, ColorBright, ColorBorder, x, y, width, height), \
	 DrawDualBox(dc, ColorFlat, ColorDark, (x)+1, (y)+1, (width)-2, (height)-2))

#define DrawButtonDown(dc, x, y, width, height) \
	(DrawDualBox(dc, ColorDark, ColorBorder, x, y, width, height), \
	 DrawBox(dc, ColorFlat, (x)+1, (y)+1, (width)-2, (height)-2))

#define FillRaisedBox(dc, x, y, width, height) \
	(FillDualBox(dc, ColorBright, ColorDark, ColorFlat, x, y, width, height))

#define FillLoweredBox(dc, x, y, width, height) \
	(FillDualBox(dc, ColorDark, ColorBright, ColorFlat, x, y, width, height))

#define FillButtonUp(dc, x, y, width, height) \
	(DrawDualBox(dc, ColorBright, ColorBorder, x, y, width, height), \
	 FillDualBox(dc, ColorFlat, ColorDark, ColorFlat, (x)+1, (y)+1, (width)-2, (height)-2))

#define FillButtonDown(dc, x, y, width, height) \
	(DrawDualBox(dc, ColorDark, ColorBorder, x, y, width, height), \
	 FillBox(dc, ColorFlat, (x)+1, (y)+1, (width)-2, (height)-2))


/******************************************************************************
**  The "liberated document" class
*/

class CFreeDoc {
public:
	CFreeDoc();
	virtual ~CFreeDoc();

	virtual BOOL Create(void);

	virtual const CString &GetTitle(void) const
		{ return(m_strTitle); }
	virtual void SetTitle(const CString &string);
	virtual const CString &GetPathName(void) const
		{ return(m_strPathName); }
	virtual void SetPathName(const CString &string);

	virtual BOOL IsModified(void) const;
	virtual void SetModifiedFlag(const BOOL modified = TRUE);

	virtual UINT GetViewCount(void) const;
	virtual void AddView(CFreeView *view);
	virtual void RemoveView(CFreeView *view);
	virtual POSITION GetFirstViewPosition(void) const;
	virtual CFreeView *GetNextView(POSITION &position) const;
	virtual void UpdateAllViews(void);
	virtual void UpdateView(CFreeView *view);

protected:
	virtual void OnChangedViewList(CFreeView *view, BOOL added);
	virtual BOOL OnCreate(void);
	virtual void OnDestroy(void);

protected:
	CString m_strTitle, m_strPathName;
	BOOL m_bModified, m_bCreated;
	CPtrList m_viewList;
};


/******************************************************************************
**  The "liberated document view" class
*/

#define AFX_FREEVIEW_ID 0xE888

class CFreeView : public CWnd {
	friend void CFreeDoc::AddView(CFreeView *view);
	friend void CFreeDoc::RemoveView(CFreeView *view);

protected:
	DECLARE_DYNCREATE(CFreeView)
	CFreeView();
	virtual ~CFreeView();

public:
	virtual BOOL Create(CWnd *wnd, BOOL show = 1);
	virtual void SetDocument(CFreeDoc *doc = NULL);
	virtual CFreeDoc *GetDocument(void);

	virtual void OnUpdate(CFreeDoc *doc);

	//{{AFX_VIRTUAL(CFreeView)
	protected:
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CFreeView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	//}}AFX_MSG

protected:
	DECLARE_MESSAGE_MAP()

	int m_width, m_height;		// Width and height of view
	CFreeDoc *m_document;		// Document
};

#endif
