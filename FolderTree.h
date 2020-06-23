
#ifndef FOLDERTREE_H
#define FOLDERTREE_H

class CFolderTree;
class CFolderDialog;
struct CFolder;

struct CFolder {
public:
	CFolder();
	~CFolder();

	void AddFile(CFolderTree *tree, const char *name, ui32 namelen, ui64 size, ui64 actual_size, ui64 time);
	void AddFolder(CFolderTree *tree, const char *name, ui32 namelen, CFolder *folder, ui64 time);
	void Finalize(void);
	void DelFile(unsigned int index);
	void RenameFile(unsigned int index, const CString &name);
	unsigned int FindFile(const CString &name);
	BOOL LoadFolderInitial(CFolderTree *tree, const char *name, ui64 clustersize, CFolderDialog *dialog);
	inline ui64 SizeFiles() { return(size_self); }
	inline ui64 SizeSub() { return(size_children); }
	inline ui64 SizeTotal() { return(size_self + size_children); }

private:
	void MoreEntries(void);
	void EightBitCountingSort(ui64 *dsize, ui64 *ssize, ui32 count, ui32 bitpos,
		char **dnames, char **snames, CFolder **dkids, CFolder **skids,
		ui64 *dasize, ui64 *sasize, ui64 *dtimes, ui64 *stimes);
	BOOL LoadFolder(CFolderTree *tree, char *name, ui32 namelen, ui64 clustersize, BOOL aligned, CFolderDialog *dialog);

public:
	CFolder *parent;
	unsigned int parentindex;

	char **names;
	CFolder **children;
	ui64 *sizes;
	ui64 *actualsizes;
	ui64 *times;
	ui64 size_self, size_children;

	unsigned int cur, max;
};

class CFolderTree : public CFreeDoc {
public:
	CFolderTree();
	virtual ~CFolderTree();

	BOOL LoadTree(const CString &path, BOOL includespace, CWnd *modalwin);
	void GetSpace(const CString &path);
	CFolder *GetRoot(void);

	CFolder *SetCur(const CString &path);
	CFolder *GetCur(void);
	CFolder *Down(unsigned int index);
	CFolder *Up(void);

protected:
	CFolder *root, *cur;

public:
	CString m_path;
	ui64 freespace, usedspace, totalspace, clustersize;
	ui64 numfiles, numfolders;
	ui64 filespace;
};

class CFolderDialog : public CDialog {
public:
	CFolderDialog();
	DECLARE_DYNCREATE(CFolderDialog)
	virtual ~CFolderDialog();

	virtual void Reset(void);
	virtual void UpdateDisplay(CFolderTree *tree, CFolder *folder);
	virtual void ForcedUpdate(CFolderTree *tree);

	void IncFiles(void);
	void IncFolders(void);
	void SetPath(CFolderTree *tree, const CString &string, CFolder *folder);

	virtual void OnCancel(void);

protected:
	//{{AFX_MSG(CFolderDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	ui32 numfiles, numfolders;
	CString path;

	BOOL cancelled;

protected:
	BOOL chg_numfiles, chg_numfolders, chg_path;
	DWORD last_tick, last_redraw;
	ui32 cur_frame;
};

inline void CFolderDialog::IncFiles(void)
{
	numfiles++;
	chg_numfiles = 1;
}

inline void CFolderDialog::IncFolders(void)
{
	numfolders++;
	chg_numfolders = 1;
}

inline void CFolderDialog::SetPath(CFolderTree *tree, const CString &string, CFolder *folder)
{
	path = string;
	chg_path = 1;
	UpdateDisplay(tree, folder);
}

#endif
