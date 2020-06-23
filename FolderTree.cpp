
#include "stdafx.h"
#include "spacemonger.h"
#include "FolderTree.h"
#include "Lang.h"

#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <winioctl.h>

//////////////////////////////////////////////////////////////////////////////
//  Reparse points are utterly undocumented in Win2K, but can wreak havoc
//  in SpaceMonger.  We try hard to deal with them anyway.

#if (_WIN32_WINNT < 0x500)

//  Maximum reparse buffer info size. The max user defined reparse
//  data is 16KB, plus there's a header.
//
#define MAX_REPARSE_SIZE	17000

//  Undocumented FSCTL_SET_REPARSE_POINT structure definition
#define REPARSE_MOUNTPOINT_HEADER_SIZE   8
typedef struct {
    DWORD          ReparseTag;
    DWORD          ReparseDataLength;
    WORD           Reserved;
    WORD           ReparseTargetLength;
    WORD           ReparseTargetMaximumLength;
    WORD           Reserved1;
    WCHAR          ReparseTarget[1];
} REPARSE_MOUNTPOINT_DATA_BUFFER, *PREPARSE_MOUNTPOINT_DATA_BUFFER;

#define FSCTL_GET_REPARSE_POINT CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 42, METHOD_BUFFERED, FILE_ANY_ACCESS) // , REPARSE_DATA_BUFFER

#endif

//////////////////////////////////////////////////////////////////////////////

CFolderTree::CFolderTree()
{
	root = cur = NULL;
	freespace = usedspace = totalspace = 0;
	m_path = "";
}

CFolderTree::~CFolderTree()
{
	if (root != NULL) delete root;
}

static void PseudoSleep(ui32 time)
{
	ui32 starttime = GetTickCount();
	MSG msg;
	while (GetTickCount() - starttime < time) {
		while (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
}

BOOL CFolderTree::LoadTree(const CString &path, BOOL includespace, CWnd *modalwin)
{
	CFolderDialog dialog;

	if (path == "") return 1;

	dialog.Create(IDD_SCAN_DIALOG, modalwin);
	dialog.Reset();

	m_path = path;
	GetSpace(path);
	if (root != NULL) delete root;
	root = new CFolder;
	cur = root;
	filespace = 0;

	if (!root->LoadFolderInitial(this, path, clustersize, &dialog)) {
		root->Finalize();
		if (root != NULL) delete root;
		root = cur = NULL;
		freespace = usedspace = totalspace = 0;
		m_path = "";
		return 0;
	}
	else {
		dialog.ForcedUpdate(this);
		numfiles = dialog.numfiles;
		numfolders = dialog.numfolders;
		if (includespace)
			root->AddFile(this, "<<<<<<<<<<<<<<<<<<<<", 1, freespace, freespace, 0);
		root->Finalize();
		::PseudoSleep(1000);
		modalwin->SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
		return 1;
	}
}

void CFolderTree::GetSpace(const CString &path)
{
	DWORD SecPerClus, BytesPerSec, ClusPerDisk, FreeClus;
	ui64 oddfree, total, totalfree;

	typedef BOOL (WINAPI *GetFreeDiskSpaceExFunc)(LPCTSTR pathname,
		ui64 *oddfree, ui64 *total, ui64 *totalfree);

	// First, compute the cluster size (which will be needed later)
	GetDiskFreeSpace(path, &SecPerClus, &BytesPerSec, &FreeClus, &ClusPerDisk);
	clustersize = (ui64)BytesPerSec * (ui64)SecPerClus;

	// Next, load in Kernel32 and use GetDiskFreeSpaceEx to find out
	// the size of the disk.  If GetDiskFreeSpaceEx doesn't exist, then
	// fall back on the values from GetDiskFreeSpace.
	HINSTANCE hLibrary = LoadLibrary("KERNEL32.DLL");
	GetFreeDiskSpaceExFunc getfreediskspaceex =
		(GetFreeDiskSpaceExFunc)GetProcAddress(hLibrary, "GetDiskFreeSpaceExA");
	if (hLibrary != NULL && getfreediskspaceex != NULL) {
		getfreediskspaceex(path, &oddfree, &total, &totalfree);
		freespace = totalfree;
		totalspace = total;
	}
	else {
		freespace = clustersize * (ui64)FreeClus;
		totalspace = clustersize * (ui64)ClusPerDisk;
	}
	FreeLibrary(hLibrary);

	usedspace = totalspace - freespace;
}

CFolder *CFolderTree::GetRoot(void)
{
	return(root);
}

CFolder *CFolderTree::SetCur(const CString &path)
{
	return(cur);
}

CFolder *CFolderTree::GetCur(void)
{
	return(cur);
}

CFolder *CFolderTree::Down(unsigned int index)
{
	if (index < cur->cur) {
		CFolder *newfolder = cur->children[index];
		if (newfolder != NULL) {
			cur = newfolder;
			return(cur);
		}
	}
	return(NULL);
}

CFolder *CFolderTree::Up(void)
{
	if (cur != root) {
		cur = cur->parent;
		return(cur);
	}
	return(NULL);
}

//////////////////////////////////////////////////////////////////////////////

static char *cpp_strdup(const char *string, int stringlen)
{
	char *newstr = (char *)malloc(stringlen+1 * sizeof(char));
	if (newstr == NULL) return(NULL);

	char *src;
	char c;
	BOOL allupper = 1;
	src = newstr;
	while (stringlen--) {
		c = *src++ = *string++;
		if (c >= 'a' && c <= 'z') allupper = 0;
	}
	*src = '\0';
	if (allupper && src != newstr) {
		src = newstr+1;
		while ((c = *src) != '\0') {
			if (c >= 'A' && c <= 'Z') *src += 32;
			src++;
		}
	}

	return(newstr);
}

CFolder::CFolder()
{
	cur = 0;
	max = 2;
	names = (char **)malloc(sizeof(char *) * max);
	sizes = (ui64 *)malloc(sizeof(ui64) * max);
	actualsizes = (ui64 *)malloc(sizeof(ui64) * max);
	children = (CFolder **)malloc(sizeof(CFolder *) * max);
	times = (ui64 *)malloc(sizeof(ui64) * max);
	size_self = size_children = 0;
	parent = NULL;
	parentindex = 0;
}

CFolder::~CFolder()
{
	unsigned int i;

	for (i = 0; i < cur; i++) {
		if (names[i] != NULL) free(names[i]);
		if (children[i] != NULL) delete children[i];
	}

	free(names);
	free(children);
	free(sizes);
	free(actualsizes);
	free(times);

	cur = max = 0;
	size_self = size_children = 0;
	parent = NULL;
	parentindex = 0;
}

void CFolder::MoreEntries(void)
{
	int newmax = max * 2;
	char **newnames = (char **)malloc(sizeof(char *) * newmax);
	ui64 *newsizes = (ui64 *)malloc(sizeof(ui64) * newmax);
	ui64 *newactualsizes = (ui64 *)malloc(sizeof(ui64) * newmax);
	ui64 *newtimes = (ui64 *)malloc(sizeof(ui64) * newmax);
	CFolder **newkids = (CFolder **)malloc(sizeof(CFolder *) * newmax);

	memcpy(newnames, names, max * sizeof(char *));
	memcpy(newsizes, sizes, max * sizeof(ui64));
	memcpy(newtimes, times, max * sizeof(ui64));
	memcpy(newactualsizes, actualsizes, max * sizeof(ui64));
	memcpy(newkids, children, max * sizeof(CFolder *));
	free(names);
	free(sizes);
	free(times);
	free(actualsizes);
	free(children);
	names = newnames;
	sizes = newsizes;
	times = newtimes;
	actualsizes = newactualsizes;
	children = newkids;

	max = newmax;
}

void CFolder::AddFile(CFolderTree *tree, const char *name, ui32 namelen, ui64 size,
	ui64 actual_size, ui64 time)
{
	if (cur >= max) MoreEntries();

	names[cur] = cpp_strdup(name, namelen);
	actualsizes[cur] = actual_size;
	times[cur] = time;
	size_self += (sizes[cur] = size);
	children[cur] = NULL;
	cur++;
	tree->filespace += size;
}

void CFolder::AddFolder(CFolderTree *tree, const char *name, ui32 namelen, CFolder *folder,
	ui64 time)
{
	if (cur >= max) MoreEntries();

	names[cur] = cpp_strdup(name, namelen);
	size_children += (sizes[cur] = folder->SizeTotal());
	actualsizes[cur] = sizes[cur];
	times[cur] = time;
	children[cur] = folder;
	folder->parent = this;
	folder->parentindex = cur;
	cur++;
}

// EightBitCountingSort
//   Performs an 8-bit counting sort on the given data.  O(n).

void CFolder::EightBitCountingSort(ui64 *dsize, ui64 *ssize, ui32 count, ui32 bitpos,
	char **dnames, char **snames, CFolder **dkids, CFolder **skids,
	ui64 *dasize, ui64 *sasize, ui64 *dtimes, ui64 *stimes)
{
	ui32 countarray[257];
	ui32 i, dest;

#define VALUE(size) (0xFF - (ui32)((size) >> bitpos) & 0xFF)

	// Initially, zero offsets for each
	for (i = 0; i < 256; i++)
		countarray[i] = 0;

	// Count how many of each value we have
	for (i = 0; i < count; i++)
		countarray[VALUE(ssize[i]) + 1]++;

	// Create array offsets for each
	for (i = 1; i < 256; i++)
		countarray[i] += countarray[i-1];

	// Now move them into place
	for (i = 0; i < count; i++) {
		dest = countarray[VALUE(ssize[i])]++;
		dsize[dest] = ssize[i];
		dnames[dest] = snames[i];
		dkids[dest] = skids[i];
		dasize[dest] = sasize[i];
		dtimes[dest] = stimes[i];
		if (dkids[dest] != NULL) dkids[dest]->parentindex = i;
	}
}

// Finalize
//   Sorts the directory by file size.  O(n)!

void CFolder::Finalize(void)
{
	if (cur <= 1) return;

	char **newnames = (char **)malloc(sizeof(char *) * cur);
	ui64 *newsizes = (ui64 *)malloc(sizeof(ui64) * cur);
	ui64 *newactualsizes = (ui64 *)malloc(sizeof(ui64) * cur);
	CFolder **newkids = (CFolder **)malloc(sizeof(CFolder *) * cur);
	ui64 *newtimes = (ui64 *)malloc(sizeof(ui64) * cur);

	// We do a radix sort with internal counting sort.  This means
	// that we pass over the source data exactly 16*cur times to sort it.
	EightBitCountingSort(newsizes, sizes, cur, 0,  newnames, names, newkids, children, newactualsizes, actualsizes, newtimes, times);
	EightBitCountingSort(sizes, newsizes, cur, 8,  names, newnames, children, newkids, actualsizes, newactualsizes, times, newtimes);
	EightBitCountingSort(newsizes, sizes, cur, 16, newnames, names, newkids, children, newactualsizes, actualsizes, newtimes, times);
	EightBitCountingSort(sizes, newsizes, cur, 24, names, newnames, children, newkids, actualsizes, newactualsizes, times, newtimes);
	EightBitCountingSort(newsizes, sizes, cur, 32, newnames, names, newkids, children, newactualsizes, actualsizes, newtimes, times);
	EightBitCountingSort(sizes, newsizes, cur, 40, names, newnames, children, newkids, actualsizes, newactualsizes, times, newtimes);
	EightBitCountingSort(newsizes, sizes, cur, 48, newnames, names, newkids, children, newactualsizes, actualsizes, newtimes, times);
	EightBitCountingSort(sizes, newsizes, cur, 56, names, newnames, children, newkids, actualsizes, newactualsizes, times, newtimes);

	free(newtimes);
	free(newnames);
	free(newsizes);
	free(newactualsizes);
	free(newkids);
}

static ui32 strxcpy(char *dest, const char *src)
{
	ui32 len = 0;
	while ((*dest++ = *src++) != '\0') len++;
	return len;
}

BOOL CFolder::LoadFolderInitial(CFolderTree *tree, const char *name, ui64 clustersize, CFolderDialog *dialog)
{
	// First, fix up the pathname so that it's canonical and complete.
	int len;
	char path[MAX_PATH+270];
	_fullpath(path, name, MAX_PATH);
	if (dialog != NULL)
		dialog->SetPath(tree, path, this);
	if ((len = strlen(path)) < 1 || path[len-1] != '\\')
		path[len++] = '\\';

	// Next, compute a bit-field to use when computing the cluster size.  This
	// saves us the work of having to do a real modulus later on.
	int clusterbits;
	ui64 clustermod;
	ui64 temp = clustersize;
	BOOL aligned;
	clusterbits = 0;
	while (temp != 0) {
		temp >>= 1;
		clusterbits++;
	}
	clustermod = 1;
	while (clusterbits--) clustermod <<= 1;
	aligned = (clustermod == clustersize);

	// Thus aligned will be set to TRUE if clustersize is a power of two,
	// which, if it is, it saves a lot of computation work by letting us use
	// bitwise-and instead of true modulus.  We also subtract 1 from the
	// cluster size here because it saves extra work in the recursive part.

	return LoadFolder(tree, path, len, clustersize-1, aligned, dialog);
}

BOOL CFolder::LoadFolder(CFolderTree *tree, char *name, ui32 namelen, ui64 clustersize, BOOL aligned, CFolderDialog *dialog)
{
	WIN32_FIND_DATA finddata;
	BOOL gotfile;
	HANDLE handle;
	ui64 size;
	static DWORD last_tick = 0;
	ui32 newlen;

	name[namelen] = '*';
	name[namelen+1] = '.';
	name[namelen+2] = '*';
	name[namelen+3] = '\0';

	// "name" now contains something like "C:\Windows\System\*.*"

	handle = FindFirstFile(name, &finddata);
	gotfile = (handle != INVALID_HANDLE_VALUE);
	while (gotfile && !dialog->cancelled) {
		newlen = namelen + strxcpy(name+namelen, finddata.cFileName);

		// Handle (i.e., IGNORE) any junction points, mount points, or symbolic links.
		// This is a lot messier than it would be with Unix soft-links, but a lot
		// simpler than Unix mount points.
		if (finddata.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {

			// Open a handle to the object.  This should never be able to fail.
			HANDLE fileHandle = CreateFile(name, 0, FILE_SHARE_READ, NULL,
						OPEN_EXISTING, FILE_FLAG_OPEN_REPARSE_POINT, 0);

			// Retrieve the reparse information for the object.
			BYTE reparseBuffer[MAX_REPARSE_SIZE];
			PREPARSE_GUID_DATA_BUFFER reparseInfo = (PREPARSE_GUID_DATA_BUFFER)reparseBuffer;
			DWORD returnedLength;
			DWORD result = DeviceIoControl(fileHandle, FSCTL_GET_REPARSE_POINT,
					NULL, 0, reparseInfo, sizeof(reparseBuffer),
					&returnedLength, NULL);

			if (result) {
				// If the reparse information indicated this was a mount point, junction
				// point, or symbolic link, skip the file.  Its size will be small enough
				// that failing to factor it into the overall size computations will not
				// yield invalid results.
				ULONG msresult = IsReparseTagMicrosoft(reparseInfo->ReparseTag);
				if (msresult) {
					switch (reparseInfo->ReparseTag) {
					case 0x80000000|IO_REPARSE_TAG_SYMBOLIC_LINK:
					case IO_REPARSE_TAG_MOUNT_POINT:
						CloseHandle(fileHandle);
						goto nextfile;
					}
				}
			}
			CloseHandle(fileHandle);
		}

		// Process directories, except for the ".." directory.
		if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (!(finddata.cFileName[0] == '.' && (finddata.cFileName[1] == '\0'
					|| (finddata.cFileName[1] == '.' && finddata.cFileName[2] == '\0')))) {
				if (dialog != NULL)
					dialog->IncFolders();
				name[newlen++] = '\\';
				name[newlen] = '\0';
				CFolder *newfolder = new CFolder;
				newfolder->LoadFolder(tree, name, newlen, clustersize, aligned, dialog);
				AddFolder(tree, finddata.cFileName, newlen-namelen-1, newfolder,
					*(ui64 *)&finddata.ftLastWriteTime);
			}
		}
		else {
			// Process files.
			if (dialog != NULL) dialog->IncFiles();
			// We round up to the cluster size because that's the
			// smallest the file can get
			ui64 actualsize = (ui64)finddata.nFileSizeLow + ((ui64)finddata.nFileSizeHigh << 32);
			if (aligned) size = (actualsize+clustersize) & clustersize;
			else size = actualsize - (actualsize % (clustersize+1));
			AddFile(tree, finddata.cFileName, newlen-namelen, size, actualsize,
				*(ui64 *)&finddata.ftLastWriteTime);
		}

	nextfile:
		gotfile = FindNextFile(handle, &finddata);

		// Only poll the system event queue every 1/5 of a second or so.
		// That's enough to ensure quick response, but it won't bog
		// down the scan.
		DWORD tick = ::GetTickCount();
		if (tick > last_tick + 200) {
			last_tick = tick;
			MSG msg;
			if (dialog != NULL) {
				name[namelen] = '\0';
				dialog->SetPath(tree, name, this);
			}
			while (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
	}
	FindClose(handle);
	Finalize();

	return !dialog->cancelled;
}

void CFolder::DelFile(unsigned int index)
{
}

void CFolder::RenameFile(unsigned int index, const CString &name)
{
}

unsigned int CFolder::FindFile(const CString &name)
{
	return((unsigned int)-1);
}

//////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CFolderDialog, CDialog)

BEGIN_MESSAGE_MAP(CFolderDialog, CDialog)
	//{{AFX_MSG_MAP(CFolderDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CFolderDialog::CFolderDialog()
{
	last_tick = last_redraw = 0;
	path = "";
	numfiles = 0;
	numfolders = 0;
	chg_path = chg_numfiles = chg_numfolders = 0;
	cur_frame = 0;

	cancelled = 0;
}

CFolderDialog::~CFolderDialog()
{
}

void CFolderDialog::OnCancel(void)
{
	cancelled = 1;
	CDialog::OnCancel();
}

void CFolderDialog::Reset(void)
{
	if (!IsWindow(m_hWnd)) return;

	last_tick = last_redraw = 0;
	path = "";
	numfiles = 0;
	numfolders = 0;
	chg_path = chg_numfiles = chg_numfolders = 0;
	cur_frame = 0;

	cancelled = 0;

	SetWindowText(CurLang->scanning);
	SetDlgItemText(IDC_STATIC_FILESFOUND, CurLang->files_found);
	SetDlgItemText(IDC_STATIC_FOLDERSFOUND, CurLang->folders_found);
	SetDlgItemText(IDCANCEL, CurLang->cancel);
	SetDlgItemText(IDC_PATH, path);
	SetDlgItemInt(IDC_FILES, numfiles);
	SetDlgItemInt(IDC_FOLDERS, numfolders);
	CProgressCtrl *progress = (CProgressCtrl *)GetDlgItem(IDC_LOAD_PROGRESS);
	if (progress != NULL) {
		progress->SetRange(0, 4096);
		progress->SetPos(0);
	}

	CClientDC dc(this);
	CDC srcdc;
	srcdc.CreateCompatibleDC(&dc);
	::SelectObject(srcdc.m_hDC, theApp.m_scan_animation[0]);
	CRect rect;
	GetClientRect(&rect);
	dc.BitBlt(8, rect.bottom - rect.top - (48 + 8) - 12, 128, 48, &srcdc, 0, 0, SRCCOPY);
}

void CFolderDialog::UpdateDisplay(CFolderTree *tree, CFolder *folder)
{
	// Guarantee that we don't update more often than 1/5 of a second
	DWORD tick = ::GetTickCount();
	if (tick > last_tick + 200) {
		last_tick = tick;

		if (!IsWindow(m_hWnd)) return;

		if (chg_path) SetDlgItemText(IDC_PATH, path);
		if (chg_numfiles) SetDlgItemInt(IDC_FILES, numfiles);
		if (chg_numfolders) SetDlgItemInt(IDC_FOLDERS, numfolders);
		CProgressCtrl *progress = (CProgressCtrl *)GetDlgItem(IDC_LOAD_PROGRESS);
		if (progress != NULL) {
			ui64 filespace = tree->filespace/16384;
			ui64 usedspace = tree->usedspace/16384;
			if (usedspace <= 0) usedspace = 1;
			progress->SetRange(0, 4096);
			progress->SetPos((int)(filespace*4096/usedspace));
		}
		chg_path = chg_numfiles = chg_numfolders = 0;

		cur_frame = (cur_frame + 1) & 7;
		if (!(cur_frame & 1)) {
			CClientDC dc(this);
			CDC srcdc;
			srcdc.CreateCompatibleDC(&dc);
			::SelectObject(srcdc.m_hDC, theApp.m_scan_animation[cur_frame >> 1]);
			CRect rect;
			GetClientRect(&rect);
			dc.BitBlt(8, rect.bottom - rect.top - (48 + 8) - 12, 128, 48, &srcdc, 0, 0, SRCCOPY);
		}
	}
#if 0
	// Redraws the display during the scan.  Very ugly-looking.
	if (tick > last_redraw + 1000) {
		last_redraw = tick;
		while (folder != NULL) {
			folder->Finalize();
			folder = folder->parent;
		}
		tree->UpdateAllViews();
	}
#endif
}

void CFolderDialog::ForcedUpdate(CFolderTree *tree)
{
	if (!IsWindow(m_hWnd)) return;

	SetDlgItemText(IDC_PATH, path);
	SetDlgItemInt(IDC_FILES, numfiles);
	SetDlgItemInt(IDC_FOLDERS, numfolders);
	CProgressCtrl *progress = (CProgressCtrl *)GetDlgItem(IDC_LOAD_PROGRESS);
	if (progress != NULL) {
		progress->SetRange(0, 4096);
		progress->SetPos(4096);
	}
}

