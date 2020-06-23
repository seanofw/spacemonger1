#include "stdafx.h"
#include "spacemonger.h"
#include "DriveDialog.h"
#include "Lang.h"

CDriveDialog::CDriveDialog(CWnd* pParent)
	: CDialog(IDD_OPEN_DIALOG, pParent)
{
}

BEGIN_MESSAGE_MAP(CDriveDialog, CDialog)
	//{{AFX_MSG_MAP(CDriveDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_DRIVE_LIST, OnDriveDblClk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDriveDialog::OnDriveDblClk(NMHDR *pNotifyStruct, LRESULT *result)
{
	CListCtrl *lc = (CListCtrl *)GetDlgItem(IDC_DRIVE_LIST);
	if (lc == NULL) return;
	if (lc->GetSelectedCount() != 1) return;

	int i, max = lc->GetItemCount();
	for (i = 0; i < max; i++) {
		if (lc->GetItemState(i, LVIS_SELECTED)) {
			EndDialog(lc->GetItemData(i));
		}
	}
}

void CDriveDialog::OnOK()
{
	CListCtrl *lc = (CListCtrl *)GetDlgItem(IDC_DRIVE_LIST);
	if (lc == NULL) EndDialog(-1);
	if (lc->GetSelectedCount() != 1) return;

	int i, max = lc->GetItemCount();
	for (i = 0; i < max; i++) {
		if (lc->GetItemState(i, LVIS_SELECTED)) {
			EndDialog(lc->GetItemData(i));
		}
	}
}

void CDriveDialog::OnCancel()
{
	EndDialog(-1);
}

BOOL CDriveDialog::OnInitDialog()
{
	CWaitCursor wait;

	CDialog::OnInitDialog();

	CDriveInfo driveinfo[26];
	HIMAGELIST imagelist;
	int i, index;
	LV_ITEM item;

	imagelist = ImageList_Create(32, 32, TRUE, 0, 26);

	CListCtrl *lc = (CListCtrl *)GetDlgItem(IDC_DRIVE_LIST);
	::SendMessage(lc->m_hWnd, LVM_SETIMAGELIST, (WPARAM)LVSIL_NORMAL, (LPARAM)(HIMAGELIST)imagelist);

	SetWindowText(CurLang->selectdrive);
	SetDlgItemText(IDOK, CurLang->ok);
	SetDlgItemText(IDCANCEL, CurLang->cancel);

	lc->InsertColumn(0, "Name");
	if (lc != NULL) {
		lc->SetItemCount(26);
		index = 0;
		for (i = 'A'; i <= 'Z'; i++) {
			driveinfo[i-'A'].LoadDriveInfo(i);
			if (driveinfo[i-'A'].avail) {
				ImageList_AddIcon(imagelist, driveinfo[i-'A'].icon);
				item.mask = LVIF_STATE|LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
				item.iItem = index;
				item.iSubItem = 0;
				item.state = 0;
				item.stateMask = LVIS_SELECTED;
				item.pszText = driveinfo[i-'A'].longname;
				item.cchTextMax = strlen(driveinfo[i-'A'].longname);
				item.iImage = index;
				item.lParam = i - 'A';
				::SendMessage(lc->m_hWnd, LVM_INSERTITEM, (WPARAM)0, (LPARAM)(&item));
				index++;
			}
			driveinfo[i-'A'].Free();
		}
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

void CDriveInfo::Free(void)
{
	if (longname != NULL) delete[] longname;
}

void CDriveInfo::LoadDriveInfo(char let)
{
	char name[8];
	UINT type;

	if (let >= 'a' && let <= 'z') let -= 32;
	name[0] = let;
	name[1] = ':';
	name[2] = '\\';
	name[3] = '\0';

	type = GetDriveType(name);

	if (type != 0 && type != 1) {
		SHFILEINFO fileinfo;
		memset(&fileinfo, sizeof(SHFILEINFO), 0);

		SHGetFileInfo(name, 0, &fileinfo, sizeof(SHFILEINFO),
			SHGFI_DISPLAYNAME|SHGFI_ICON);

		avail = 1;
		letter = let;
		icon = fileinfo.hIcon;
		longname = new char[strlen(fileinfo.szDisplayName)+1];
		strcpy(longname, fileinfo.szDisplayName);
	}
	else {
		avail = 0;
		letter = let;
		icon = NULL;
		longname = NULL;
	}
}
