// Copyleft 2017 Akshay.C.S. Based on works by Chris Korda and others.
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.

#if !defined(AFX_OptEditDlg_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_)
#define AFX_OptEditDlg_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptEditDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptEditDlg dialog

#include "CodeOptionsInfo.h"

class COptEditDlg : public CPropertyPage
{
// Construction
public:
	COptEditDlg(CCodeOptionsInfo& Info);

// Attributes

// Operations
	static	void	ValidateFolder(CDataExchange *pDX, int CtrlID, const CString& Path);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptEditDlg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(COptEditDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnUndoUnlimited();
	afx_msg void OnTempFolderBrowse();
	afx_msg void OnTempFolderType();
	afx_msg void OnKillFocusEditorTempFolderPath();
	//}}AFX_MSG
	afx_msg LRESULT	OnKickIdle(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUpdateUndoLevels(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTempFolderPath(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

// Dialog data
	//{{AFX_DATA(COptEditDlg)
	enum { IDD = IDD_OPT_EDIT };
	int		m_TempFolderType;
	//}}AFX_DATA

// Constants
	enum {
		DEFAULT_UNDO_LEVELS = 10
	};

// Member data
	CCodeOptionsInfo&	m_oi;		// reference to parent's options info 
	bool	m_UndoUnlimited;	// true if undo is unlimited

// Helpers
	void	UpdateTempFolderFreeSpace();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OptEditDlg_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_)
