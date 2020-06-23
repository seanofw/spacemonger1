
#include "stdafx.h"
#include "spacemonger.h"
#include "Lang.h"
#include "SetupDlg.h"

IMPLEMENT_DYNCREATE(CSettingsDialog, CDialog)

BEGIN_MESSAGE_MAP(CSettingsDialog, CDialog)
	//{{AFX_MSG_MAP(CSettingsDialog)
	ON_BN_CLICKED(IDC_SHOW_NAME_TIPS, OnShowNameTips)
	ON_BN_CLICKED(IDC_SHOW_INFO_TIPS, OnShowInfoTips)
	ON_CBN_SELENDOK(IDC_LANG, OnLang)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CSettingsDialog::CSettingsDialog(CWnd *parent)
	: CDialog(IDD_SETTINGS, parent)
{
}

CSettingsDialog::~CSettingsDialog()
{
}

void CSettingsDialog::OnCancel(void)
{
	theApp.m_settings.lang[0] = oldlang[0];
	theApp.m_settings.lang[1] = oldlang[1];
	theApp.m_settings.lang[2] = oldlang[2];
	theApp.m_settings.lang[3] = oldlang[3];
	CurLang = oldlangptr;

	CDialog::OnCancel();
}

void CSettingsDialog::OnOK(void)
{
	CComboBox *density = (CComboBox *)GetDlgItem(IDC_DENSITY);
	if (density != NULL)
		theApp.m_settings.density = density->GetCurSel()-3;
	CComboBox *file_color = (CComboBox *)GetDlgItem(IDC_FILE_COLOR);
	if (file_color != NULL)
		theApp.m_settings.file_color = file_color->GetCurSel();
	CComboBox *folder_color = (CComboBox *)GetDlgItem(IDC_FOLDER_COLOR);
	if (folder_color != NULL)
		theApp.m_settings.folder_color = folder_color->GetCurSel();
	CButton *auto_rescan = (CButton *)GetDlgItem(IDC_AUTO_RESCAN);
	if (auto_rescan != NULL)
		theApp.m_settings.auto_rescan = auto_rescan->GetCheck();
	CButton *animated = (CButton *)GetDlgItem(IDC_ANIMATED);
	if (animated != NULL)
		theApp.m_settings.animated_zoom = animated->GetCheck();
	CButton *disable_delete = (CButton *)GetDlgItem(IDC_DISABLE_DELETE);
	if (disable_delete != NULL)
		theApp.m_settings.disable_delete = disable_delete->GetCheck();
	CSliderCtrl *bias = (CSliderCtrl *)GetDlgItem(IDC_BIAS);
	if (bias != NULL)
		theApp.m_settings.bias = bias->GetPos()-20;
	CButton *save_pos = (CButton *)GetDlgItem(IDC_SAVE_POS);
	if (save_pos != NULL)
		theApp.m_settings.save_pos = save_pos->GetCheck();

	CButton *show_tips = (CButton *)GetDlgItem(IDC_SHOW_NAME_TIPS);
	if (show_tips != NULL)
		theApp.m_settings.show_name_tips = show_tips->GetCheck();
	show_tips = (CButton *)GetDlgItem(IDC_SHOW_ROLLOVER_BOX);
	if (show_tips != NULL)
		theApp.m_settings.rollover_box = show_tips->GetCheck();
	show_tips = (CButton *)GetDlgItem(IDC_SHOW_INFO_TIPS);
	if (show_tips != NULL)
		theApp.m_settings.show_info_tips = show_tips->GetCheck();

	CButton *tip;
	theApp.m_settings.infotip_flags = 0;
	CEdit *nametipdelay = (CEdit *)GetDlgItem(IDC_NAMETIP_DELAY);
	if (nametipdelay != NULL) {
		CString string;
		nametipdelay->GetWindowText(string);
		theApp.m_settings.nametip_delay = atoi(string);
	}

	if ((tip = (CButton *)GetDlgItem(IDC_INFOTIP_PATH)) != NULL && tip->GetCheck())
		theApp.m_settings.infotip_flags |= TIP_PATH;
	if ((tip = (CButton *)GetDlgItem(IDC_INFOTIP_NAME)) != NULL && tip->GetCheck())
		theApp.m_settings.infotip_flags |= TIP_NAME;
	if ((tip = (CButton *)GetDlgItem(IDC_INFOTIP_SIZE)) != NULL && tip->GetCheck())
		theApp.m_settings.infotip_flags |= TIP_SIZE;
	if ((tip = (CButton *)GetDlgItem(IDC_INFOTIP_DATE)) != NULL && tip->GetCheck())
		theApp.m_settings.infotip_flags |= TIP_DATE;
	if ((tip = (CButton *)GetDlgItem(IDC_INFOTIP_ATTRIB)) != NULL && tip->GetCheck())
		theApp.m_settings.infotip_flags |= TIP_ATTRIB;
	if ((tip = (CButton *)GetDlgItem(IDC_INFOTIP_ICON)) != NULL && tip->GetCheck())
		theApp.m_settings.infotip_flags |= TIP_ICON;
	CEdit *infotipdelay = (CEdit *)GetDlgItem(IDC_INFOTIP_DELAY);
	if (infotipdelay != NULL) {
		CString string;
		infotipdelay->GetWindowText(string);
		theApp.m_settings.infotip_delay = atoi(string);
	}

	CDialog::OnOK();
}

void CSettingsDialog::OnShowNameTips(void)
{
	CButton *show_tips = (CButton *)GetDlgItem(IDC_SHOW_NAME_TIPS);
	if (show_tips != NULL)
		EnableNameTipButtons(show_tips->GetCheck());
}

void CSettingsDialog::OnShowInfoTips(void)
{
	CButton *show_tips = (CButton *)GetDlgItem(IDC_SHOW_INFO_TIPS);
	if (show_tips != NULL)
		EnableInfoTipButtons(show_tips->GetCheck());
}

void CSettingsDialog::EnableNameTipButtons(BOOL bEnable)
{
	CWnd *wnd;
	if ((wnd = GetDlgItem(IDC_STATIC_NAMETIP1)) != NULL)
		wnd->EnableWindow(bEnable);
	if ((wnd = GetDlgItem(IDC_NAMETIP_DELAY)) != NULL)
		wnd->EnableWindow(bEnable);
	if ((wnd = GetDlgItem(IDC_STATIC_NAMETIP2)) != NULL)
		wnd->EnableWindow(bEnable);
}

void CSettingsDialog::EnableInfoTipButtons(BOOL bEnable)
{
	CWnd *wnd;
	if ((wnd = GetDlgItem(IDC_INFOTIP_PATH)) != NULL)
		wnd->EnableWindow(bEnable);
	if ((wnd = GetDlgItem(IDC_INFOTIP_NAME)) != NULL)
		wnd->EnableWindow(bEnable);
	if ((wnd = GetDlgItem(IDC_INFOTIP_DATE)) != NULL)
		wnd->EnableWindow(bEnable);
	if ((wnd = GetDlgItem(IDC_INFOTIP_SIZE)) != NULL)
		wnd->EnableWindow(bEnable);
	if ((wnd = GetDlgItem(IDC_INFOTIP_ATTRIB)) != NULL)
		wnd->EnableWindow(bEnable);
	if ((wnd = GetDlgItem(IDC_INFOTIP_ICON)) != NULL)
		wnd->EnableWindow(bEnable);
	if ((wnd = GetDlgItem(IDC_STATIC_INFOTIP1)) != NULL)
		wnd->EnableWindow(bEnable);
	if ((wnd = GetDlgItem(IDC_INFOTIP_DELAY)) != NULL)
		wnd->EnableWindow(bEnable);
	if ((wnd = GetDlgItem(IDC_STATIC_INFOTIP2)) != NULL)
		wnd->EnableWindow(bEnable);
}

void CSettingsDialog::OnLang(void)
{
	CComboBox *lang = (CComboBox *)GetDlgItem(IDC_LANG);
	if (lang == NULL) return;
	int index = lang->GetCurSel();
	theApp.m_settings.lang[0] = Langs[index].abbrev[0];
	theApp.m_settings.lang[1] = Langs[index].abbrev[1];
	theApp.m_settings.lang[2] = '\0';
	theApp.m_settings.lang[3] = '\0';
	CurLang = Langs[index].text;
	UpdateDialogForNewLanguage();
}

void CSettingsDialog::UpdateDialogForNewLanguage(void)
{
	CComboBox *lang = (CComboBox *)GetDlgItem(IDC_LANG);
	if (lang != NULL) lang->ResetContent();
	CComboBox *density = (CComboBox *)GetDlgItem(IDC_DENSITY);
	if (density != NULL) density->ResetContent();
	CComboBox *file_color = (CComboBox *)GetDlgItem(IDC_FILE_COLOR);
	if (file_color != NULL) file_color->ResetContent();
	CComboBox *folder_color = (CComboBox *)GetDlgItem(IDC_FOLDER_COLOR);
	if (folder_color != NULL) folder_color->ResetContent();

	int i;

	SetWindowText(CurLang->settings);
	SetDlgItemText(IDOK, CurLang->ok);
	SetDlgItemText(IDCANCEL, CurLang->cancel);

	SetDlgItemText(IDC_STATIC_LAYOUT, CurLang->layout);
	SetDlgItemText(IDC_STATIC_DENSITY, CurLang->density);
	SetDlgItemText(IDC_STATIC_BIAS, CurLang->bias);
	SetDlgItemText(IDC_STATIC_HORZ, CurLang->horz);
	SetDlgItemText(IDC_STATIC_EQUAL, CurLang->equal);
	SetDlgItemText(IDC_STATIC_VERT, CurLang->vert);
	SetDlgItemText(IDC_STATIC_DISPLAYCOLORS, CurLang->displaycolors);
	SetDlgItemText(IDC_STATIC_FILES, CurLang->files);
	SetDlgItemText(IDC_STATIC_FOLDERS, CurLang->folders);
	SetDlgItemText(IDC_STATIC_TOOLTIPS, CurLang->tooltips);
	SetDlgItemText(IDC_STATIC_MISCOPTIONS, CurLang->miscoptions);
	SetDlgItemText(IDC_SHOW_NAME_TIPS, CurLang->shownametips);
	SetDlgItemText(IDC_SHOW_ROLLOVER_BOX, CurLang->showrolloverbox);
	SetDlgItemText(IDC_SHOW_INFO_TIPS, CurLang->showinfotips);
	SetDlgItemText(IDC_INFOTIP_PATH, CurLang->fullpath);
	SetDlgItemText(IDC_INFOTIP_NAME, CurLang->filename);
	SetDlgItemText(IDC_INFOTIP_ICON, CurLang->icon);
	SetDlgItemText(IDC_INFOTIP_DATE, CurLang->datetime);
	SetDlgItemText(IDC_INFOTIP_SIZE, CurLang->filesize);
	SetDlgItemText(IDC_INFOTIP_ATTRIB, CurLang->attrib);
	SetDlgItemText(IDC_STATIC_NAMETIP1, CurLang->delay);
	SetDlgItemText(IDC_STATIC_NAMETIP2, CurLang->msec);
	SetDlgItemText(IDC_STATIC_INFOTIP1, CurLang->delay);
	SetDlgItemText(IDC_STATIC_INFOTIP2, CurLang->msec);
	SetDlgItemText(IDC_AUTO_RESCAN, CurLang->autorescan);
	SetDlgItemText(IDC_DISABLE_DELETE, CurLang->disabledelete);
	SetDlgItemText(IDC_ANIMATED, CurLang->animatedzoom);
	SetDlgItemText(IDC_SAVE_POS, CurLang->savepos);

	if (lang != NULL) {
		for (i = 0; Langs[i].name != NULL; i++) {
			lang->AddString(Langs[i].name);
			if (Langs[i].abbrev[0] == theApp.m_settings.lang[0]
				&& Langs[i].abbrev[1] == theApp.m_settings.lang[1])
				lang->SetCurSel(i);
		}
	}

	if (density != NULL) {
		for (i = 0; i < 6; i++)
			density->AddString(CurLang->densitynames[i]);
		density->SetCurSel(theApp.m_settings.density+3);
	}
	if (file_color != NULL) {
		for (i = 0; i < 12; i++)
			file_color->AddString(CurLang->colornames[i]);
		file_color->SetCurSel(theApp.m_settings.file_color);
	}
	if (folder_color != NULL) {
		for (i = 0; i < 12; i++)
			folder_color->AddString(CurLang->colornames[i]);
		folder_color->SetCurSel(theApp.m_settings.folder_color);
	}
	CButton *auto_rescan = (CButton *)GetDlgItem(IDC_AUTO_RESCAN);
	if (auto_rescan != NULL)
		auto_rescan->SetCheck(theApp.m_settings.auto_rescan);
	CButton *animated = (CButton *)GetDlgItem(IDC_ANIMATED);
	if (animated != NULL)
		animated->SetCheck(theApp.m_settings.animated_zoom);
	CButton *disable_delete = (CButton *)GetDlgItem(IDC_DISABLE_DELETE);
	if (disable_delete != NULL)
		disable_delete->SetCheck(theApp.m_settings.disable_delete);
	CButton *save_pos = (CButton *)GetDlgItem(IDC_SAVE_POS);
	if (save_pos != NULL)
		save_pos->SetCheck(theApp.m_settings.save_pos);

	CButton *show_name_tips = (CButton *)GetDlgItem(IDC_SHOW_NAME_TIPS);
	if (show_name_tips != NULL)
		show_name_tips->SetCheck(theApp.m_settings.show_name_tips);
	show_name_tips = (CButton *)GetDlgItem(IDC_SHOW_ROLLOVER_BOX);
	if (show_name_tips != NULL)
		show_name_tips->SetCheck(theApp.m_settings.rollover_box);

	CButton *tip;
	CButton *show_info_tips = (CButton *)GetDlgItem(IDC_SHOW_INFO_TIPS);
	if (show_info_tips != NULL)
		show_info_tips->SetCheck(theApp.m_settings.show_info_tips);
	if ((tip = (CButton *)GetDlgItem(IDC_INFOTIP_PATH)) != NULL)
		tip->SetCheck((theApp.m_settings.infotip_flags & TIP_PATH) != 0);
	if ((tip = (CButton *)GetDlgItem(IDC_INFOTIP_NAME)) != NULL)
		tip->SetCheck((theApp.m_settings.infotip_flags & TIP_NAME) != 0);
	if ((tip = (CButton *)GetDlgItem(IDC_INFOTIP_DATE)) != NULL)
		tip->SetCheck((theApp.m_settings.infotip_flags & TIP_DATE) != 0);
	if ((tip = (CButton *)GetDlgItem(IDC_INFOTIP_SIZE)) != NULL)
		tip->SetCheck((theApp.m_settings.infotip_flags & TIP_SIZE) != 0);
	if ((tip = (CButton *)GetDlgItem(IDC_INFOTIP_ATTRIB)) != NULL)
		tip->SetCheck((theApp.m_settings.infotip_flags & TIP_ATTRIB) != 0);
	if ((tip = (CButton *)GetDlgItem(IDC_INFOTIP_ICON)) != NULL)
		tip->SetCheck((theApp.m_settings.infotip_flags & TIP_ICON) != 0);

	CEdit *tipdelay = (CEdit *)GetDlgItem(IDC_NAMETIP_DELAY);
	if (tipdelay != NULL) {
		CString string;
		string.Format("%u", theApp.m_settings.nametip_delay);
		tipdelay->SetWindowText(string);
	}
	tipdelay = (CEdit *)GetDlgItem(IDC_INFOTIP_DELAY);
	if (tipdelay != NULL) {
		CString string;
		string.Format("%u", theApp.m_settings.infotip_delay);
		tipdelay->SetWindowText(string);
	}
	EnableNameTipButtons(theApp.m_settings.show_name_tips);
	EnableInfoTipButtons(theApp.m_settings.show_info_tips);

	CSliderCtrl *bias = (CSliderCtrl *)GetDlgItem(IDC_BIAS);
	if (bias != NULL) {
		bias->SetRange(0, 40, 0);
		bias->SetPos(theApp.m_settings.bias+20);
		bias->SetTicFreq(5);
	}
}

BOOL CSettingsDialog::OnInitDialog()
{
	oldlang[0] = theApp.m_settings.lang[0];
	oldlang[1] = theApp.m_settings.lang[1];
	oldlang[2] = theApp.m_settings.lang[2];
	oldlang[3] = theApp.m_settings.lang[3];
	oldlangptr = CurLang;

	UpdateDialogForNewLanguage();

	CDialog::OnInitDialog();

	return TRUE;
}

