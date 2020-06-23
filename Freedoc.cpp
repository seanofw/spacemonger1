
#include "stdafx.h"

CFreeDoc::CFreeDoc()
{
	m_strTitle = "";
	m_strPathName = "";
	m_bModified = 0;
	m_bCreated = 0;
}

CFreeDoc::~CFreeDoc()
{
	if (m_bCreated) OnDestroy();

	m_bCreated = 0;
	m_bModified = 0;
	m_strPathName = "";
	m_strTitle = "";
}

BOOL CFreeDoc::Create(void)
{
	BOOL createresult;

	m_bModified = 0;
	m_bCreated = 0;
	m_strTitle = "";
	m_strPathName = "";

	createresult = OnCreate();

	m_bCreated = 1;

	return(createresult);
}

BOOL CFreeDoc::IsModified(void) const
{
	return(m_bModified);
}

void CFreeDoc::SetModifiedFlag(const BOOL modified)
{
	m_bModified = modified;
}

void CFreeDoc::SetTitle(const CString &string)
{
	m_strTitle = string;
}

void CFreeDoc::SetPathName(const CString &string)
{
	m_strPathName = string;
}

UINT CFreeDoc::GetViewCount(void) const
{
	return(m_viewList.GetCount());
}

void CFreeDoc::AddView(CFreeView *view)
{
	if (view != NULL) {
		m_viewList.AddTail((void *)view);
		OnChangedViewList(view, 1);
		view->m_document = this;
	}
}

void CFreeDoc::RemoveView(CFreeView *view)
{
	if (view != NULL) {
		POSITION pos = m_viewList.Find(view);
		if (pos != NULL) {
			m_viewList.RemoveAt(pos);
			OnChangedViewList(view, 0);
		}
		view->m_document = NULL;
	}
}

POSITION CFreeDoc::GetFirstViewPosition(void) const
{
	return(m_viewList.GetHeadPosition());
}

CFreeView *CFreeDoc::GetNextView(POSITION &position) const
{
	CFreeView *view = (CFreeView *)m_viewList.GetNext(position);
	return(view);
}

void CFreeDoc::UpdateAllViews(void)
{
	POSITION pos = m_viewList.GetHeadPosition();
	CFreeView *view;

	while (pos != NULL) {
		view = (CFreeView *)m_viewList.GetNext(pos);
		UpdateView(view);
	}
}

void CFreeDoc::UpdateView(CFreeView *view)
{
	view->OnUpdate(this);
}

void CFreeDoc::OnChangedViewList(CFreeView *view, BOOL added)
{
}

BOOL CFreeDoc::OnCreate(void)
{
	return(TRUE);
}

void CFreeDoc::OnDestroy(void)
{
}
