
#include "StdAfx.h"
#include "SpaceMonger.h"
#include "Lang.h"
#include "MainFrm.h"
#include "FolderTree.h"
#include "FolderView.h"
#include "DriveDialog.h"
#include "SetupDlg.h"
#include "TipWnd.h"
#include "AboutDialog.h"

/////////////////////////////////////////////////////////////////////////////
// The primary CSpaceMonger object

CSpaceMonger theApp;

/////////////////////////////////////////////////////////////////////////////
// CApp

BEGIN_MESSAGE_MAP(CSpaceMonger, CWinApp)
	//{{AFX_MSG_MAP(CSpaceMonger)
	ON_COMMAND(ID_APP_ABOUT, OnAbout)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_REFRESH, OnFileRefresh)
	ON_COMMAND(ID_FILE_RUN, OnFileRun)
	ON_COMMAND(ID_FILE_DELETE, OnFileDelete)
	ON_COMMAND(ID_FILE_PROPERTIES, OnFileProperties)
	ON_COMMAND(ID_VIEW_ZOOM_FULL, OnZoomFull)
	ON_COMMAND(ID_VIEW_ZOOM_IN, OnZoomIn)
	ON_COMMAND(ID_VIEW_ZOOM_OUT, OnZoomOut)
	ON_COMMAND(ID_VIEW_FREE, OnShowFree)
	ON_COMMAND(ID_SETTINGS, OnSettings)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI_RANGE(100, 41000, OnIgnoreUpdate)
END_MESSAGE_MAP()

CSpaceMonger::CSpaceMonger()
{
	m_document = NULL;
}

CSpaceMonger::~CSpaceMonger()
{
}

BOOL CSpaceMonger::InitInstance()
{
	int i;

	InitTipWnd(AfxGetInstanceHandle());

	// Load in the settings from the Registry.  If there were no settings,
	// we load defaults.  Once we've got them, save them back, which sounds
	// odd, but it will end up creating the necessary entries if they don't
	// exist.
	m_settings.Load();
	m_settings.Save();

	int showcmd = m_settings.showcmd;
	RECT rect = m_settings.rect;
	if (!m_settings.save_pos) {
		showcmd = SW_SHOWNORMAL;
		rect.top = rect.left = CW_USEDEFAULT;
		rect.right = rect.bottom = 0;	// This is actually CW_USEDEFAULT because of a bug
										// in CFrameWnd::Create()
	}

	// Create the main window
	m_mainframe = new CMainFrame;
	m_mainframe->CFrameWnd::Create(NULL, "SpaceMonger", WS_OVERLAPPEDWINDOW, rect);
	m_pMainWnd = m_mainframe;

	// Create the one and only document
	m_document = new CFolderTree;
	m_document->Create();

	// Create the main document view
	m_view = new CFolderView;
	m_view->Create(m_mainframe, 1);
	m_view->SetDocument(m_document);
	m_view->SetFocus();

	// Load in the animations
	static int anim_name[4] = {
		IDB_SCAN1, IDB_SCAN2, IDB_SCAN3, IDB_SCAN4,
	};
	for (i = 0; i < 4; i++)
		m_scan_animation[i] = (HBITMAP)LoadImage(m_hInstance,
			MAKEINTRESOURCE(anim_name[i]), IMAGE_BITMAP, 128, 48,
			LR_LOADTRANSPARENT|LR_LOADMAP3DCOLORS);

	m_mainframe->ShowWindow(showcmd);
	m_mainframe->UpdateWindow();
	m_mainframe->RecalcLayout();

	return(TRUE);
}

int CSpaceMonger::ExitInstance()
{
	m_settings.Save();

	// m_pMainWnd is auto-deleted by the framework
	delete m_view;
	delete m_document;

	return(0);
}

void CSpaceMonger::OnAppExit(void)
{
	CWinApp::OnAppExit();
}

void CSpaceMonger::OnFileOpen(void)
{
	CDriveDialog drvdialog;
	int drive = drvdialog.DoModal();
	if (drive == -1) return;

	CFolderTree *ft = (CFolderTree *)m_document;
	ft->m_path.Format("%c:\\", (char)(drive + 'A'));
	OnFileRefresh();
}

void CSpaceMonger::OnFileRefresh(void)
{
	CFolderTree *ft = (CFolderTree *)m_document;

	m_mainframe->EnableWindow(0);
	m_view->SetDocument(NULL);
	m_view->UpdateWindow();

	((CFolderView *)m_view)->zoomlevel = 0;
	ft->LoadTree(ft->m_path, 1, m_mainframe);
	m_view->SetDocument(m_document);
	ft->UpdateAllViews();
	m_mainframe->EnableWindow(1);
}

void CSpaceMonger::OnFileRun(void)
{
	CFolderView *fv = (CFolderView *)m_view;
	CFolderTree *ft = (CFolderTree *)m_document;

	if (fv == NULL || ft == NULL || fv->selected == NULL) return;

	CString title, path;
	if (ft != NULL) title += ft->m_path;
	fv->BuildTitleReverse(fv->selected->source, title);
	path = title;
	title += fv->selected->source->names[fv->selected->index];

	ShellExecute(NULL, NULL, title, NULL, path, SW_SHOWDEFAULT);
}

void CSpaceMonger::OnFileDelete(void)
{
	CFolderView *fv = (CFolderView *)m_view;
	CFolderTree *ft = (CFolderTree *)m_document;

	if (fv == NULL || ft == NULL || fv->selected == NULL) return;

	CString title;
	if (ft != NULL) title += ft->m_path;
	fv->BuildTitleReverse(fv->selected->source, title);
	title += fv->selected->source->names[fv->selected->index];
	BOOL isfolder;
	if (fv->selected->source->children[fv->selected->index] != NULL)
		isfolder = 1;
	else isfolder = 0;

	char source[MAX_PATH+8];
	int len;

	strcpy(source, title);
	len = strlen(source);
	source[len] = '\0';
	source[len+1] = '\0';

	SHFILEOPSTRUCT fop;
	fop.hwnd = m_mainframe->m_hWnd;
	fop.wFunc = FO_DELETE;
	fop.pFrom = source;
	fop.pTo = NULL;
	fop.fFlags = FOF_SIMPLEPROGRESS|FOF_ALLOWUNDO|FOF_NOCONFIRMATION;
	fop.fAnyOperationsAborted = 0;
	fop.hNameMappings = NULL;
	fop.lpszProgressTitle = CurLang->deleting;

	if (SHFileOperation(&fop) != 0 || fop.fAnyOperationsAborted) {
		AfxMessageBox("Windows failed to delete file.");
		return;
	}

	if (isfolder) {
		delete fv->selected->source->children[fv->selected->index];
		fv->selected->source->children[fv->selected->index] = NULL;
	}
	ui64 subsize = fv->selected->source->sizes[fv->selected->index];
	CFolder *folder = fv->selected->source;
	if (isfolder) folder->size_children -= subsize;
	else folder->size_self -= subsize;
	folder = folder->parent;
	while (folder != NULL) {
		folder->size_children -= subsize;
		folder = folder->parent;
	}
	fv->selected->source->sizes[fv->selected->index] = 0;

	if (m_settings.auto_rescan)
		OnFileRefresh();
	else
		fv->OnUpdate(m_document);
}

void CSpaceMonger::OnZoomIn(void)
{
	((CFolderView *)m_view)->ZoomIn(((CFolderView *)m_view)->selected);
}

void CSpaceMonger::OnZoomOut(void)
{
	((CFolderView *)m_view)->ZoomOut();
}

void CSpaceMonger::OnZoomFull(void)
{
	((CFolderView *)m_view)->ZoomFull();
}

void CSpaceMonger::OnShowFree(void)
{
	CFolderView *fv = (CFolderView *)m_view;
	CMainFrame *mf = (CMainFrame *)m_mainframe;
	if (fv == NULL || mf == NULL) return;

	BOOL showfreespace = !fv->showfreespace;
	mf->m_toolbar.EnableState(CMainToolBar::ViewFree, showfreespace);
	fv->ShowFreeSpace(showfreespace);
}

void CSpaceMonger::OnSettings(void)
{
	CSettingsDialog settingsdialog;
	if (settingsdialog.DoModal() == IDOK) {
		m_settings.Save();
		if (m_view != NULL && m_document != NULL) {
			m_view->OnUpdate(m_document);
			((CFolderView *)m_view)->UpdateTitleBar();
		}
		((CMainFrame *)m_mainframe)->m_toolbar.Reload();
	}
}

void CSpaceMonger::OnFileProperties(void)
{
	CFolderView *fv = (CFolderView *)m_view;
	CFolderTree *ft = (CFolderTree *)m_document;

	if (fv == NULL || ft == NULL || fv->selected == NULL) return;

	CString path = ft->m_path;
	fv->BuildTitleReverse(fv->selected->source, path);
	path += fv->selected->source->names[fv->selected->index];

	SHELLEXECUTEINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(si);
	si.hwnd = fv->GetSafeHwnd();
	si.nShow = SW_SHOW;
	si.fMask  = SEE_MASK_INVOKEIDLIST;
	si.lpVerb = _T("properties");
	si.lpFile = (LPCTSTR)path;
	ShellExecuteEx(&si);
}

void CSpaceMonger::OnAbout()
{
	CAboutDialog about;
	about.DoModal();
}

void CSpaceMonger::OnIgnoreUpdate(CCmdUI *ui)
{
	GeneralIgnoreUpdate(ui);
}

void GeneralIgnoreUpdate(CCmdUI *ui)
{
	CFolderView *fv = (CFolderView *)theApp.m_view;
	CDisplayFolder *cur = fv->selected;
	int zoom = fv->zoomlevel;

	if (ui->m_nID == ID_FILE_DELETE) {
		if (theApp.m_settings.disable_delete || cur == NULL)
			ui->Enable(0);
	}
	else if (ui->m_nID == ID_FILE_RUN) {
		if (cur == NULL) ui->Enable(0);
	}
	else if (ui->m_nID == ID_FILE_PROPERTIES) {
		if (cur == NULL) ui->Enable(0);
	}
	else if (ui->m_nID == ID_FILE_REFRESH) {
		CFolderTree *ft = (CFolderTree *)theApp.m_document;
		if (ft == NULL || ft->GetRoot() == NULL) ui->Enable(0);
	}
	else if (ui->m_nID == ID_VIEW_FREE) {
		CFolderTree *ft = (CFolderTree *)theApp.m_document;
		ui->SetCheck(fv->showfreespace);
		if (ft == NULL || ft->GetRoot() == NULL) ui->Enable(0);
	}
	else if (ui->m_nID == ID_VIEW_ZOOM_IN) {
		if (cur == NULL || (cur->flags & 1) == 0)
			ui->Enable(0);
	}
	else if (ui->m_nID == ID_VIEW_ZOOM_OUT || ui->m_nID == ID_VIEW_ZOOM_FULL) {
		if (zoom == 0) ui->Enable(0);
	}
}

/////////////////////////////////////////////////////////////////////////////
//  Application Settings
//

void CCurrentSettings::Reset(void)
{
	density = 0;
	file_color = 0;
	folder_color = 0;
	auto_rescan = 0;
	animated_zoom = 1;
	disable_delete = 0;
	bias = 0;
	show_name_tips = 1;
	nametip_delay = 125;
	show_info_tips = 1;
	rollover_box = 0;
	infotip_flags = TIP_DATE|TIP_SIZE|TIP_ICON;
	infotip_delay = 250;
	rect.left = rect.right = rect.top = rect.bottom = CW_USEDEFAULT;
	showcmd = SW_SHOWNORMAL;
	save_pos = 0;
	lang[0] = 'u';
	lang[1] = 's';
	lang[2] = '\0';
	lang[3] = '\0';
}

void CCurrentSettings::Load(void)
{
	HKEY key;
	Reset();
	if (RegOpenKey(HKEY_CURRENT_USER, "Software\\65\\SpaceMonger\\1.3", &key) != ERROR_SUCCESS)
		return;

	DWORD size = sizeof(density);
	DWORD type;
	RegQueryValueEx(key, "density",				NULL, &type, (unsigned char *)&density, &size);
	size = sizeof(file_color);
	RegQueryValueEx(key, "file_color",			NULL, &type, (unsigned char *)&file_color, &size);
	size = sizeof(folder_color);
	RegQueryValueEx(key, "folder_color",		NULL, &type, (unsigned char *)&folder_color, &size);
	size = sizeof(auto_rescan);
	RegQueryValueEx(key, "auto_rescan",			NULL, &type, (unsigned char *)&auto_rescan, &size);
	size = sizeof(animated_zoom);
	RegQueryValueEx(key, "animated_zoom",		NULL, &type, (unsigned char *)&animated_zoom, &size);
	size = sizeof(disable_delete);
	RegQueryValueEx(key, "disable_delete",		NULL, &type, (unsigned char *)&disable_delete, &size);
	size = sizeof(save_pos);
	RegQueryValueEx(key, "save_pos",			NULL, &type, (unsigned char *)&save_pos, &size);
	size = sizeof(bias);
	RegQueryValueEx(key, "bias",				NULL, &type, (unsigned char *)&bias, &size);
	size = sizeof(show_name_tips);
	RegQueryValueEx(key, "show_name_tips",		NULL, &type, (unsigned char *)&show_name_tips, &size);
	size = sizeof(rollover_box);
	RegQueryValueEx(key, "show_rollover_box",	NULL, &type, (unsigned char *)&rollover_box, &size);
	size = sizeof(show_info_tips);
	RegQueryValueEx(key, "show_info_tips",		NULL, &type, (unsigned char *)&show_info_tips, &size);
	size = sizeof(nametip_delay);
	RegQueryValueEx(key, "nametip_delay",		NULL, &type, (unsigned char *)&nametip_delay, &size);
	size = sizeof(infotip_flags);
	RegQueryValueEx(key, "infotip_flags",		NULL, &type, (unsigned char *)&infotip_flags, &size);
	size = sizeof(infotip_delay);
	RegQueryValueEx(key, "infotip_delay",		NULL, &type, (unsigned char *)&infotip_delay, &size);
	size = sizeof(rect.left);
	RegQueryValueEx(key, "window_rect.left",	NULL, &type, (unsigned char *)&rect.left, &size);
	size = sizeof(rect.top);
	RegQueryValueEx(key, "window_rect.top",		NULL, &type, (unsigned char *)&rect.top, &size);
	size = sizeof(rect.right);
	RegQueryValueEx(key, "window_rect.right",	NULL, &type, (unsigned char *)&rect.right, &size);
	size = sizeof(rect.bottom);
	RegQueryValueEx(key, "window_rect.bottom",	NULL, &type, (unsigned char *)&rect.bottom, &size);
	size = sizeof(showcmd);
	RegQueryValueEx(key, "window_showcmd",		NULL, &type, (unsigned char *)&showcmd, &size);
	size = sizeof(lang);
	RegQueryValueEx(key, "lang",				NULL, &type, (unsigned char *)&lang, &size);
	RegCloseKey(key);

	if (density < -3) density = -3;
	if (density > 3) density = 3;

	if (file_color < 0 || file_color > 12) file_color = 0;
	if (folder_color < 0 || folder_color > 12) folder_color = 0;

	auto_rescan = !!auto_rescan;
	animated_zoom = !!animated_zoom;
	disable_delete = !!disable_delete;
	save_pos = !!save_pos;

	rollover_box = !!rollover_box;

	show_name_tips = !!show_name_tips;
	if (nametip_delay < 0) nametip_delay = 0;
	if (nametip_delay > 99999) nametip_delay = 99999;

	show_info_tips = !!show_info_tips;
	infotip_flags &= TIP_PATH|TIP_NAME|TIP_ICON|TIP_DATE|TIP_SIZE|TIP_ATTRIB;
	if (infotip_delay < 0) infotip_delay = 0;
	if (infotip_delay > 99999) infotip_delay = 99999;

	if (bias < -20) bias = -20;
	if (bias > 20) bias = 20;

	// Ensure that the window's provided rectangle has at least a little of
	// its area visible to the end-user.
	int scrnwidth = GetSystemMetrics(SM_CXSCREEN), scrnheight = GetSystemMetrics(SM_CYSCREEN);
	if (rect.top > rect.bottom) {
		int temp = rect.top;
		rect.top = rect.bottom;
		rect.bottom = temp;
	}
	if (rect.left > rect.right) {
		int temp = rect.left;
		rect.left = rect.right;
		rect.right = temp;
	}
	if (rect.right - rect.left > scrnwidth) rect.right = rect.left + scrnwidth;
	if (rect.bottom - rect.top > scrnheight) rect.bottom = rect.top + scrnheight;
	if (rect.top > scrnheight-8)
		rect.bottom -= (rect.top - (scrnheight-8)), rect.top = scrnheight-8;
	if (rect.left > scrnwidth-8)
		rect.right -= (rect.left - (scrnwidth-8)), rect.left = scrnwidth-8;
	if (rect.right < 8)
		rect.left += (8-rect.right), rect.right = 8;
	if (rect.bottom < 8)
		rect.top += (8-rect.bottom), rect.bottom = 8;

	// Ensure that the window's placement is comprehensible.
	if (showcmd != SW_SHOWMINIMIZED && showcmd != SW_SHOWMAXIMIZED
		&& showcmd != SW_SHOWNORMAL)
		showcmd = SW_SHOWNORMAL;

	// Find the language in the list of languages.  If it's not found,
	// try US English.
	lang[0] = tolower(lang[0]);
	lang[1] = tolower(lang[1]);
	lang[2] = '\0';
	lang[3] = '\0';
	int i;
	CurLang = NULL;
	for (i = 0; Langs[i].name != NULL; i++) {
		if (lang[0] == Langs[i].abbrev[0] && lang[1] == Langs[i].abbrev[1]) {
			CurLang = Langs[i].text;
			break;
		}
	}
	if (CurLang == NULL) {
		CurLang = Langs[0].text;
		lang[0] = Langs[0].abbrev[0];
		lang[1] = Langs[0].abbrev[1];
		lang[2] = '\0';
		lang[3] = '\0';
	}
}

void CCurrentSettings::Save(void)
{
	HKEY key;
	if (RegCreateKey(HKEY_CURRENT_USER, "Software\\65\\SpaceMonger\\1.3", &key) != ERROR_SUCCESS)
		return;
	DWORD size;
	size = sizeof(density);
	RegSetValueEx(key, "density",			NULL, REG_DWORD, (unsigned char *)&density, size);
	size = sizeof(file_color);
	RegSetValueEx(key, "file_color",		NULL, REG_DWORD, (unsigned char *)&file_color, size);
	size = sizeof(folder_color);
	RegSetValueEx(key, "folder_color",		NULL, REG_DWORD, (unsigned char *)&folder_color, size);
	size = sizeof(auto_rescan);
	RegSetValueEx(key, "auto_rescan",		NULL, REG_DWORD, (unsigned char *)&auto_rescan, size);
	size = sizeof(animated_zoom);
	RegSetValueEx(key, "animated_zoom",		NULL, REG_DWORD, (unsigned char *)&animated_zoom, size);
	size = sizeof(disable_delete);
	RegSetValueEx(key, "disable_delete",	NULL, REG_DWORD, (unsigned char *)&disable_delete, size);
	size = sizeof(save_pos);
	RegSetValueEx(key, "save_pos",			NULL, REG_DWORD, (unsigned char *)&save_pos, size);
	size = sizeof(bias);
	RegSetValueEx(key, "bias",				NULL, REG_DWORD, (unsigned char *)&bias, size);
	size = sizeof(show_name_tips);
	RegSetValueEx(key, "show_name_tips",	NULL, REG_DWORD, (unsigned char *)&show_name_tips, size);
	size = sizeof(rollover_box);
	RegSetValueEx(key, "show_rollover_box",	NULL, REG_DWORD, (unsigned char *)&rollover_box, size);
	size = sizeof(show_info_tips);
	RegSetValueEx(key, "show_info_tips",	NULL, REG_DWORD, (unsigned char *)&show_info_tips, size);
	size = sizeof(nametip_delay);
	RegSetValueEx(key, "nametip_delay",		NULL, REG_DWORD, (unsigned char *)&nametip_delay, size);
	size = sizeof(infotip_flags);
	RegSetValueEx(key, "infotip_flags",		NULL, REG_DWORD, (unsigned char *)&infotip_flags, size);
	size = sizeof(infotip_delay);
	RegSetValueEx(key, "infotip_delay",		NULL, REG_DWORD, (unsigned char *)&infotip_delay, size);
	size = sizeof(rect.left);
	RegSetValueEx(key, "window_rect.left",	NULL, REG_DWORD, (unsigned char *)&rect.left, size);
	size = sizeof(rect.top);
	RegSetValueEx(key, "window_rect.top",	NULL, REG_DWORD, (unsigned char *)&rect.top, size);
	size = sizeof(rect.right);
	RegSetValueEx(key, "window_rect.right",	NULL, REG_DWORD, (unsigned char *)&rect.right, size);
	size = sizeof(rect.bottom);
	RegSetValueEx(key, "window_rect.bottom",NULL, REG_DWORD, (unsigned char *)&rect.bottom, size);
	size = sizeof(showcmd);
	RegSetValueEx(key, "window_showcmd",	NULL, REG_DWORD, (unsigned char *)&showcmd, size);
	size = sizeof(lang);
	RegSetValueEx(key, "lang",				NULL, REG_SZ, (unsigned char *)&lang, 3);
	RegCloseKey(key);
}
