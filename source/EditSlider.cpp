// Copyleft 2017 Akshay.C.S. Based on works by Chris Korda and others.
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.

// EditSlider.cpp : implementation file

#include "stdafx.h"
#include "Resource.h"
#include "EditSlider.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditSlider

IMPLEMENT_DYNAMIC(CEditSlider, CClickSliderController);

CEditSlider::CEditSlider()
{
	m_Edit = NULL;
	m_Val = 0;
	m_Scale = 1;
	m_LogBase = 0;
}

CEditSlider::~CEditSlider()
{
}

double CEditSlider::Norm(double x) const
{
	if (m_LogBase)
		x = log(x) / log(m_LogBase);
	return(x * m_Scale);
}

double CEditSlider::Denorm(double x) const
{
	x /= m_Scale;
	if (m_LogBase)
		x = pow(m_LogBase, x);
	return(x);
}

void CEditSlider::SetVal(double Val)
{
	m_Val = Val;
	SetPos(round(Norm(m_Val)));
	if (m_Edit != NULL)
		m_Edit->SetVal(m_Val, CNumEdit::NTF_NONE);	// don't notify anyone
}

void CEditSlider::SetValNorm(double Val)
{
	int	nMin, nMax;
	GetRange(nMin, nMax);
	SetVal(Denorm(Val * (nMax - nMin) + nMin));
}

double CEditSlider::GetValNorm() const
{
	int	nMin, nMax;
	GetRange(nMin, nMax);
	return((Norm(m_Val) - nMin) / (nMax - nMin));
}

double CEditSlider::Norm(const INFO& Info, double Val)
{
	if (Info.LogBase)
		Val = log(Val) / log(double(Info.LogBase));
	Val *= Info.SliderScale;
	Val = (Val - Info.RangeMin) / (Info.RangeMax - Info.RangeMin);
	return(Val);
}

double CEditSlider::Denorm(const INFO& Info, double Val)
{
	Val = Val * (Info.RangeMax - Info.RangeMin) + Info.RangeMin;
	Val /= Info.SliderScale;
	if (Info.LogBase)
		Val = pow(double(Info.LogBase), Val);
	return(Val);
}

void CEditSlider::SetEditCtrl(CNumEdit *Edit)
{
	m_Edit = Edit;
	Edit->SetAuxNotify(this);
}

void CEditSlider::SetInfo(const INFO& Info, CNumEdit *Edit)
{
	SetEditCtrl(Edit);
	SetRange(Info.RangeMin, Info.RangeMax, TRUE);
	SetLogBase(Info.LogBase);
	SetScale(Info.SliderScale);
	SetDefaultPos(Info.DefaultPos);
	SetTicCount(Info.TicCount);
	if (m_Edit != NULL) {
		m_Edit->SetScale(Info.EditScale);
		m_Edit->SetPrecision(Info.EditPrecision);
	}
}

double CEditSlider::GetDefaultVal() const
{
	return(Denorm(GetDefaultPos()));
}

BEGIN_MESSAGE_MAP(CEditSlider, CClickSliderController)
	//{{AFX_MSG_MAP(CEditSlider)
	ON_WM_HSCROLL_REFLECT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditSlider message handlers

void CEditSlider::HScroll(UINT nSBCode, UINT nPos)
{
	int	pos = GetPos();
	double	val = Denorm(pos);
	if (val != m_Val) {	// if value changed
		m_Val = val;
		if (m_Edit != NULL)
			m_Edit->SetVal(m_Val, CNumEdit::NTF_PARENT);	// notify parent only
	}
}

void CEditSlider::OnDestroy() 
{
	CClickSliderController::OnDestroy();
	if (m_Edit != NULL) {
		m_Edit->SetAuxNotify(NULL);
		m_Edit = NULL;
	}
}

BOOL CEditSlider::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR	*nmh = (NMHDR *)lParam;
	if (m_Edit != NULL && nmh->hwndFrom == m_Edit->m_hWnd) {
		m_Val = m_Edit->GetVal();
		SetPos(round(Norm(m_Val)));
	}
	return CClickSliderController::OnNotify(wParam, lParam, pResult);
}
