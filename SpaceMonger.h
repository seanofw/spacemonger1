
#ifndef CSPACEMONGER_H
#define CSPACEMONGER_H

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"   // main symbols
#include "MainFrm.h"    // Main window

enum {
	TIP_PATH = 1,			// Display full path in tooltip
	TIP_NAME = 2,			// Display filename in tooltip
	TIP_ICON = 4,			// Display suitable icon in tooltip
	TIP_DATE = 8,			// Display file date in tooltip
	TIP_SIZE = 16,			// Display file size in tooltip
	TIP_ATTRIB = 32,		// Display file attributes in tooltip
};

struct CCurrentSettings {
	int density;			// Density of filenames
	int file_color;			// Displayed color of files
	int folder_color;		// Displayed color of folders
	BOOL auto_rescan;		// Auto-rescan when a file is deleted
	BOOL animated_zoom;		// Use animated zoom in/out
	BOOL disable_delete;	// Disallow the deletion of files
	BOOL rollover_box;		// Show rollover box
	int bias;				// Display bias, from -20 (vert) to +20 (horz)

	BOOL save_pos;			// Save this window's position?
	RECT rect;				// Pluralized window rectangle
	int showcmd;			// Current show-command

	BOOL show_name_tips;	// Show tooltips?
	int nametip_delay;		// Tip delay, in milliseconds

	BOOL show_info_tips;	// Show tooltips?
	int infotip_flags;		// Tooltip flags
	int infotip_delay;		// Tip delay, in milliseconds

	char lang[4];			// Language, by two-letter code

	void Reset(void);		// Reset to default values
	void Load(void);		// Retrieve current settings from Registry
	void Save(void);		// Save current settings to Registry
};

class CSpaceMonger : public CWinApp {
public:
	CSpaceMonger();
	~CSpaceMonger();

	//{{AFX_VIRTUAL(CSpaceMonger)
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSpaceMonger)
	afx_msg void OnAbout();
	afx_msg void OnAppExit(void);
	afx_msg void OnFileOpen(void);
	afx_msg void OnFileRefresh(void);
	afx_msg void OnFileRun(void);
	afx_msg void OnFileDelete(void);
	afx_msg void OnZoomFull(void);
	afx_msg void OnZoomIn(void);
	afx_msg void OnZoomOut(void);
	afx_msg void OnShowFree(void);
	afx_msg void OnSettings(void);
	afx_msg void OnFileProperties(void);
	afx_msg void OnIgnoreUpdate(CCmdUI *ui);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CFrameWnd *m_mainframe;
	CFreeDoc *m_document;
	CFreeView *m_view;

	HBITMAP m_scan_animation[4];

	CCurrentSettings m_settings;
};

void GeneralIgnoreUpdate(CCmdUI *ui);

extern CSpaceMonger theApp;

#endif
