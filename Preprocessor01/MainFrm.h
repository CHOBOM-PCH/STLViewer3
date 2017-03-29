
// MainFrm.h : CMainFrame Ŭ������ �������̽�
//
//#if !defined(AFX_MAINFRM_H__20527D4A_2603_11D2_9DCF_0080AD172BB5__INCLUDED_)
//#define AFX_MAINFRM_H__20527D4A_2603_11D2_9DCF_0080AD172BB5__INCLUDED_

#include "Preprocessor01View.h"
#include "Calc.h"

#pragma once

class CMainFrame : public CFrameWnd
{
protected: // serialization������ ��������ϴ�.
	CMainFrame();
	//~CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Ư���Դϴ�.
public:
	UINT progressMax;
	CFont *pOldFont, *pStatusBarFont;
	COLORREF bkColor;

	RECT Rect;
	CString szText;
	//Progress control displayed in the status bar
    CProgressCtrl           _progressBar;
	//Pointer to the progress info window (which owns the progress bar)
	 CWnd*                   _progressInfoWindow;

// �۾��Դϴ�.
public:
	// steps the progress control, when needed
    void CMainFrame::stepProgress();
	
	// creates the progress when needed
    void setProgressCtrl(UINT uMin, UINT uMax, LPCTSTR strText);
	
	// removes the progress control when no longer needed
	void CMainFrame::removeProgressCtrl();
	
	//The status bar window
	CStatusBar        m_wndStatusBar;
	LPCTSTR AppPath;

	int progress_percent;

// �������Դϴ�.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// �����Դϴ�.
public:
	virtual ~CMainFrame();


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif



protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	CToolBar          m_wndToolBar;
	//CStatusBar        m_wndStatusBar;

// ������ �޽��� �� �Լ�
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnDestroy();
	void StatusBarMessage(TCHAR* fmt, ...);
	CPreprocessor01View* CMainFrame::GetView();

	void Add_ComboBox(CToolBar *pToolBar, CComboBox *pComboBox, int nIndex, int cx, int cy, DWORD dwStyle, UINT nID);
	afx_msg void CMainFrame::OnSelchangeCombo();

	CComboBox m_ComboLine, m_ComboEtc;
};


//#endif // !defined(AFX_MAINFRM_H__20527D4A_2603_11D2_9DCF_0080AD172BB5__INCLUDED_)