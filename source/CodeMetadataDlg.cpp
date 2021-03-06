// Copyleft 2017 Akshay.C.S. Based on works by Chris Korda and others.
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.

// CodeMetadataDlg.cpp : implementation file

#include "stdafx.h"
#include "SignalStudio.h"
#include "CodeMetadataDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCodeMetadataDlg dialog

const CControlResizer::CTRL_LIST CCodeMetadataDlg::m_CtrlList[] = {
//	CtrlID						BindType
	{IDC_METADATA_LIST,			BIND_ALL},
	{IDC_METADATA_CLEAR,		BIND_LEFT | BIND_BOTTOM},
	{IDOK,						BIND_RIGHT | BIND_BOTTOM},
	{IDCANCEL,					BIND_RIGHT | BIND_BOTTOM},
	{0}	// list terminator; don't delete
};

const CCodeMetadataDlg::COL_INFO CCodeMetadataDlg::m_ColInfo[COLUMNS] = {
//	NameID						Width
	{IDS_METADATA_COL_NAME,		80},
	{IDS_METADATA_COL_VALUE,	250},
};

#define METADATASTR(sndfile_str, id3v2_tag, wav_tag) IDS_METADATA_##sndfile_str,
const int CCodeMetadataDlg::m_StrNameID[STRINGS] = {
	#include "MetadataStrhead.h"	// generate metadata string info
};

#define RK_METADATA_COL_WIDTH _T("MetadataColWidth")

CCodeMetadataDlg::CCodeMetadataDlg(CStringArray& Metadata, CWnd* pParent /*=NULL*/)
	: CCodePersist(IDD, 0, _T("CodeMetadataDlg"), pParent), m_Metadata(Metadata)
{
	//{{AFX_DATA_INIT(CCodeMetadataDlg)
	//}}AFX_DATA_INIT
	m_InitSize = CSize(0, 0);
}

bool CCodeMetadataDlg::StoreColumnWidths(CListCtrl& List, LPCTSTR Key)
{
	int	cols = List.GetHeaderCtrl()->GetItemCount();
	CDWordArray	width;
	width.SetSize(cols);
	for (int iCol = 0; iCol < cols; iCol++)	// for each column
		width[iCol] = List.GetColumnWidth(iCol);
	DWORD	sz = cols * sizeof(int);
	return(CPersist::WriteBinary(REG_SETTINGS, Key, width.GetData(), sz) != 0);
}

bool CCodeMetadataDlg::LoadColumnWidths(CListCtrl& List, LPCTSTR Key)
{
	int	cols = List.GetHeaderCtrl()->GetItemCount();
	CDWordArray	width;
	width.SetSize(cols);
	DWORD	sz = cols * sizeof(int);
	if (!CPersist::GetBinary(REG_SETTINGS, Key, width.GetData(), &sz))
		return(FALSE);
	for (int iCol = 0; iCol < cols; iCol++)	// for each column
		List.SetColumnWidth(iCol, width[iCol]);
	return(TRUE);
}

void CCodeMetadataDlg::DoDataExchange(CDataExchange* pDX)
{
	CCodePersist::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCodeMetadataDlg)
	DDX_Control(pDX, IDC_METADATA_LIST, m_List);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CCodeMetadataDlg message map

BEGIN_MESSAGE_MAP(CCodeMetadataDlg, CCodePersist)
	//{{AFX_MSG_MAP(CCodeMetadataDlg)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_MEASUREITEM()
	ON_BN_CLICKED(IDC_METADATA_CLEAR, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCodeMetadataDlg message handlers

BOOL CCodeMetadataDlg::OnInitDialog() 
{
	CCodePersist::OnInitDialog();
	
	// save initial size
	CRect	rc;
	GetWindowRect(rc);
	m_InitSize = rc.Size();
	// set app icon and add document title to caption
	CSignalStudioApp::InitDialogCaptionView(*this, theApp.GetMain()->GetView());
	// init resizing
	m_Resize.AddControlList(this, m_CtrlList);
	// init list control
	m_List.SetExtendedStyle(LVS_EX_GRIDLINES);
	// report view only; owner draw required so OnMeasureItem sets item height
	ASSERT(m_List.GetStyle() & (LVS_REPORT | LVS_OWNERDRAWFIXED));
	// item height already set, so disable owner draw to allow default drawing
	m_List.ModifyStyle(LVS_OWNERDRAWFIXED, 0);
	for (int iCol = 0; iCol < COLUMNS; iCol++) {	// for each column
		const COL_INFO	info = m_ColInfo[iCol];
		m_List.InsertColumn(iCol, LDS(info.NameID), LVCFMT_LEFT, info.Width);
	}
	LoadColumnWidths(m_List, RK_METADATA_COL_WIDTH);
	for (int iItem = 0; iItem < STRINGS; iItem++) {	// for each string
		m_List.InsertItem(iItem, LDS(m_StrNameID[iItem]));	// add list item
		if (iItem < m_Metadata.GetSize())
			m_List.SetItemText(iItem, COL_VALUE, m_Metadata[iItem]);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCodeMetadataDlg::OnOK() 
{
	m_Metadata.SetSize(STRINGS);
	int	nValidStrs = 0;
	for (int iItem = 0; iItem < STRINGS; iItem++) {	// for each string
		m_Metadata[iItem] = m_List.GetItemText(iItem, COL_VALUE);
		if (!m_Metadata[iItem].IsEmpty())
			nValidStrs++;
	}
	if (!nValidStrs)	// if all strings empty
		m_Metadata.RemoveAll();	// delete metadata array
	StoreColumnWidths(m_List, RK_METADATA_COL_WIDTH);
	CCodePersist::OnOK();
}

void CCodeMetadataDlg::OnSize(UINT nType, int cx, int cy) 
{
	CCodePersist::OnSize(nType, cx, cy);
	m_Resize.OnSize();	
}

void CCodeMetadataDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	lpMMI->ptMinTrackSize = CPoint(m_InitSize);
	CCodePersist::OnGetMinMaxInfo(lpMMI);
}

void CCodeMetadataDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// increase item height, else gridlines style can cause drawing artifacts
	lpMeasureItemStruct->itemHeight += 3;
}

void CCodeMetadataDlg::OnClear() 
{
	for (int iItem = 0; iItem < STRINGS; iItem++)	// for each string
		m_List.SetItemText(iItem, COL_VALUE, NULL);	// clear corresponding list item
}
