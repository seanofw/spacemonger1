
#include "stdafx.h"

IMPLEMENT_DYNCREATE(CFreeView, CWnd)

CFreeView::CFreeView()
{
	m_document = NULL;
}

CFreeView::~CFreeView()
{
	if (m_document != NULL)
		m_document->RemoveView(this);
}

BEGIN_MESSAGE_MAP(CFreeView, CWnd)
	//{{AFX_MSG_MAP(CFreeView)
	ON_WM_SIZE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CFreeView::Create(CWnd *parent, BOOL show)
{
	CRect rect;

	if (parent != NULL)
		parent->GetClientRect(&rect);
	else {
		rect.left = 0, rect.right = 16;
		rect.top = 0, rect.bottom = 16;
	}

	if (!CWnd::Create(NULL, NULL, WS_CHILD,
		rect, parent, 0)) return(FALSE);
	if (show) {
		ShowWindow(SW_SHOWNORMAL);
		UpdateWindow();
	}

	return(TRUE);
}

void CFreeView::SetDocument(CFreeDoc *document)
{
	if (m_document != NULL) m_document->RemoveView(this);
	if (document != NULL) document->AddView(this);
}

CFreeDoc *CFreeView::GetDocument(void)
{
	return(m_document);
}

void CFreeView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	m_width = cx, m_height = cy;
}

void CFreeView::OnDraw(CDC* pDC)
{
}

void CFreeView::OnUpdate(CFreeDoc *doc)
{
}

void CFreeView::OnPaint()
{
	CPaintDC dc(this);
	OnDraw(&dc);
}
