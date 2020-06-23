
#include "StdAfx.h"
#include "SpaceMonger.h"
#include "FolderView.h"
#include "AboutDialog.h"
#include "Lang.h"

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_WM_ERASEBKGND()
	ON_WM_ACTIVATEAPP()
	ON_WM_CLOSE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI_RANGE(100, 41000, OnIgnoreUpdate)
END_MESSAGE_MAP()

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	CFrameWnd::PreCreateWindow(cs);

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;

	return(1);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1) return(-1);

	HINSTANCE hinst = AfxFindResourceHandle(MAKEINTRESOURCE(ID_SPACEMONGER), RT_ICON);
	HICON hicon = ::LoadIcon(hinst, MAKEINTRESOURCE(ID_SPACEMONGER));
	SetIcon(hicon, 1);
	//SetIcon(hicon, 0);

	if (!m_toolbar.Create(this)) return(-1);

	CMenu *menu = GetSystemMenu(FALSE);
	menu->AppendMenu(MF_SEPARATOR);
	menu->AppendMenu(MF_STRING, ID_APP_ABOUT, CString("&")
		+ CurLang->about_spacemonger + "...");

	WINDOWPLACEMENT w;
	w.length = sizeof(WINDOWPLACEMENT);
	::GetWindowPlacement(m_hWnd, &w);
	theApp.m_settings.rect.left = w.rcNormalPosition.left;
	theApp.m_settings.rect.top = w.rcNormalPosition.top;
	theApp.m_settings.rect.right = w.rcNormalPosition.right;
	theApp.m_settings.rect.bottom = w.rcNormalPosition.bottom;
	theApp.m_settings.showcmd = w.showCmd;

	RecalcLayout(0);

	return(0);
}

void CMainFrame::OnDestroy(void)
{
	WINDOWPLACEMENT w;
	w.length = sizeof(WINDOWPLACEMENT);
	::GetWindowPlacement(m_hWnd, &w);
	theApp.m_settings.rect.left = w.rcNormalPosition.left;
	theApp.m_settings.rect.top = w.rcNormalPosition.top;
	theApp.m_settings.rect.right = w.rcNormalPosition.right;
	theApp.m_settings.rect.bottom = w.rcNormalPosition.bottom;
	theApp.m_settings.showcmd = w.showCmd;
}

void CMainFrame::OnActivateApp(BOOL bActive, HTASK hTask)
{
	if (bActive) {
		CSpaceMonger *monger = (CSpaceMonger *)AfxGetApp();
		if (monger != NULL && monger->m_view != NULL) {
			CFolderView *view = (CFolderView *)monger->m_view;
			view->OnUpdate(monger->m_document);
		}
	}
	else {
		CSpaceMonger *monger = (CSpaceMonger *)AfxGetApp();
		if (monger != NULL && monger->m_view != NULL) {
			CFolderView *view = (CFolderView *)monger->m_view;
			view->SelectFolder(NULL);
		}
	}
}

void CMainFrame::RecalcLayout(BOOL bNotify)
{
	CRect rectBorder;
	int numwnds, wndnum, width;
	CWnd *wnd, *first, *last;

	if (m_bInRecalcLayout || !IsWindow(m_hWnd)
		|| (wnd = GetWindow(GW_CHILD)) == NULL) return;

	m_bInRecalcLayout = TRUE;

	first = wnd->GetWindow(GW_HWNDFIRST);
	last = wnd->GetWindow(GW_HWNDLAST);

	GetClientRect(&rectBorder);
	rectBorder.OffsetRect(-rectBorder.left, -rectBorder.top);
	width = rectBorder.right - rectBorder.left;

	// Position our two toolbars
	m_toolbar.SetWindowPos(NULL, 0, -2, width, 30,
		SWP_NOZORDER|SWP_NOACTIVATE);
	rectBorder.top += 28;

	// First count how many windows need to be rearranged
	numwnds = 0;
	if (first != NULL) {
		wnd = first;
		while (1) {
			if (wnd->m_hWnd != m_toolbar.m_hWnd)
				numwnds++;
			if (wnd == last) break;
			wnd = wnd->GetWindow(GW_HWNDNEXT);
		}
	}

	// Now lay them all out in a horizontal band (cheap & easy)
	if (first != NULL && numwnds > 0) {
		wndnum = 0, wnd = first;
		while (1) {
			if (wnd->m_hWnd != m_toolbar.m_hWnd) {
				wnd->SetWindowPos(NULL, rectBorder.left + (width * wndnum) / numwnds, rectBorder.top,
					(width * (wndnum+1)) / numwnds - (width * wndnum) / numwnds,
					rectBorder.bottom - rectBorder.top,
					SWP_NOZORDER);
				if (++wndnum == numwnds) break;
			}
			wnd = wnd->GetWindow(GW_HWNDNEXT);
		}
	}

	m_bInRecalcLayout = FALSE;
}

void CMainFrame::OnSysCommand(UINT nid, LPARAM lparam)
{
	if (nid == ID_APP_ABOUT) {
		CAboutDialog about;
		about.DoModal();
	}
	else CFrameWnd::OnSysCommand(nid, lparam);
}

BOOL CMainFrame::OnEraseBkgnd(CDC* pDC)
{
#if 0
	CRect rect;
	GetClientRect(&rect);
	FillBox(pDC->m_hDC, ColorFlat, 0, 0,
		rect.right - rect.left, rect.bottom - rect.top);
#endif
	return(-1);
}

void CMainFrame::OnClose()
{
	CFrameWnd::OnClose();
}

void CMainFrame::OnIgnoreUpdate(CCmdUI *ui)
{
	GeneralIgnoreUpdate(ui);
}

void CMainFrame::OnWindowPosChanged(WINDOWPOS *wp)
{
	CFrameWnd::OnWindowPosChanged(wp);

	WINDOWPLACEMENT w;
	w.length = sizeof(WINDOWPLACEMENT);
	::GetWindowPlacement(m_hWnd, &w);
	theApp.m_settings.rect.left = w.rcNormalPosition.left;
	theApp.m_settings.rect.top = w.rcNormalPosition.top;
	theApp.m_settings.rect.right = w.rcNormalPosition.right;
	theApp.m_settings.rect.bottom = w.rcNormalPosition.bottom;
	theApp.m_settings.showcmd = w.showCmd;
}

void CMainFrame::OnShowWindow(BOOL bShow, UINT status)
{
	CFrameWnd::OnShowWindow(bShow, status);

	WINDOWPLACEMENT w;
	w.length = sizeof(WINDOWPLACEMENT);
	::GetWindowPlacement(m_hWnd, &w);
	theApp.m_settings.rect.left = w.rcNormalPosition.left;
	theApp.m_settings.rect.top = w.rcNormalPosition.top;
	theApp.m_settings.rect.right = w.rcNormalPosition.right;
	theApp.m_settings.rect.bottom = w.rcNormalPosition.bottom;
	theApp.m_settings.showcmd = w.showCmd;
}

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CMainToolBar, CToolBar)

BEGIN_MESSAGE_MAP(CMainToolBar, CToolBar)
	//{{AFX_MSG_MAP(CMainToolBar)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI_RANGE(100, 41000, OnIgnoreUpdate)
END_MESSAGE_MAP()

CMainToolBar::CMainToolBar()
{
}

CMainToolBar::~CMainToolBar()
{
}

int CMainToolBar::Create(CWnd *parent)
{
	if (!CToolBar::Create(parent, WS_CHILD|WS_VISIBLE|CBRS_TOP, IDR_TOOLBAR_EN)) return 0;
	if (!LoadToolBar(CurLang->toolbar_bitmap_id)) return 0;

	EnableState(ViewFree, 1);

	return 1;
}

void CMainToolBar::EnableState(int state, BOOL enabled)
{
	switch (state) {
	case CMainToolBar::ViewFree:
		SetButtonStyle(CommandToIndex(ID_VIEW_FREE), enabled ? TBBS_BUTTON|TBBS_PRESSED : TBBS_BUTTON);
		break;
	default:
		break;
	}
}

void CMainToolBar::UpdateButtonsForView(CFolderView *view)
{
	CSpaceMonger *app = (CSpaceMonger *)AfxGetApp();
	CToolBarCtrl &ctrl = this->GetToolBarCtrl();
	ctrl.EnableButton(ID_FILE_RUN, view->IsAnythingOpen() && view->IsAnythingSelected());
	ctrl.EnableButton(ID_FILE_DELETE, !app->m_settings.disable_delete
		&& view->IsAnythingOpen() && view->IsAnythingSelected());
	ctrl.EnableButton(ID_FILE_OPEN, 1);
	ctrl.EnableButton(ID_FILE_REFRESH, view->IsAnythingOpen());
	ctrl.EnableButton(ID_VIEW_ZOOM_FULL, view->IsAnythingOpen() && !view->IsZoomFull());
	ctrl.EnableButton(ID_VIEW_ZOOM_OUT, view->IsAnythingOpen() && !view->IsZoomFull());
	ctrl.EnableButton(ID_VIEW_ZOOM_IN, view->IsAnythingOpen() && view->IsAnythingSelected()
		&& view->IsSelectedAFolder());
	ctrl.EnableButton(ID_VIEW_FREE, view->IsAnythingOpen());
	ctrl.EnableButton(ID_SETTINGS, 1);
	ctrl.EnableButton(ID_APP_ABOUT, 1);
}

void CMainToolBar::OnIgnoreUpdate(CCmdUI *ui)
{
	GeneralIgnoreUpdate(ui);
}

void CMainToolBar::Reload(void)
{
	LoadToolBar(CurLang->toolbar_bitmap_id);
	RedrawWindow();
}
