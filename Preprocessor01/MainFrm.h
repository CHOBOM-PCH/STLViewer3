
// MainFrm.h : CMainFrame 클래스의 인터페이스
//
//#if !defined(AFX_MAINFRM_H__20527D4A_2603_11D2_9DCF_0080AD172BB5__INCLUDED_)
//#define AFX_MAINFRM_H__20527D4A_2603_11D2_9DCF_0080AD172BB5__INCLUDED_

#include "Preprocessor01View.h"
#include "Calc.h"

#pragma once

class CMainFrame : public CFrameWnd
{
protected: // serialization에서만 만들어집니다.
	CMainFrame();
	//~CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
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

// 작업입니다.
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

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CMainFrame();


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif



protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CToolBar          m_wndToolBar;
	//CStatusBar        m_wndStatusBar;

// 생성된 메시지 맵 함수
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