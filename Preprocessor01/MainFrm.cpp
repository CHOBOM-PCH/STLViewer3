
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "Preprocessor01.h"

#include "MainFrm.h"
#include "resource.h"
#include "Preprocessor01View.h"
#include "Calc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define 	ID_INDICATOR_PROGRESS 10

double VSCALE = 10.0;
double line_width = 0.0015;       // 0.001 = 1u -> 0.0015      0.0001 = 100n  0.00001 = 10n
double line_gap = 0.015;         // X 10

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(ID_COMBO,OnSelchangeCombo)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	int mainf = 1;                 // (3)
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)  // message WM_CREATE
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)	return -1;         // (3-2)

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 도구 모음을 도킹할 수 없게 하려면 이 세 줄을 삭제하십시오.
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);



	CPoint p;
	p.x = 10;
	p.y = 100;

	CRect rect;
	//DWORD dw;
	UINT n=0;
	RecalcLayout();
	//m_wndDlgToolBar.GetWindowRect(&rect);
	rect.OffsetRect(0, 1);
	//dw=m_wndDlgToolBar.GetBarStyle();
	//DockControlBar(&m_wndViewSliceDlgBar,n,&rect);


	DWORD ComboBoxStyle2 = CBS_DROPDOWN | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL;
	Add_ComboBox(&m_wndToolBar, &m_ComboLine, 20, 250, 200, ComboBoxStyle2, ID_COMBO);
	m_ComboLine.AddString(_T("Line All"));
	m_ComboLine.SetCurSel(0);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)    // override
{
	//if (!CFrameWnd::PreCreateWindow(cs))
	//	return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	cs.cx=1200;                                      // (3-1)        1200 X 900
	cs.cy=900;
	return CFrameWnd::PreCreateWindow(cs);/*TRUE;*/
}

void CMainFrame::stepProgress()
{
	char s[10];
	RECT R = Rect;
	R.top = 4;
	R.left -=50;
	CDC* pDC = NULL;
	_progressBar.StepIt();
	sprintf(s,"  %d%%",(int)(100*_progressBar.GetPos()/progressMax));
	pDC = _progressInfoWindow->GetDC();
	pDC->SelectObject(pStatusBarFont);
	pDC->SetBkColor(bkColor);
	pDC->DrawText(s, -1, &R, DT_LEFT);
	
    //m_wndStatusBar.SetPaneText(0, _T(""), TRUE);
//    m_wndStatusBar.Invalidate();
  //  m_wndStatusBar.UpdateWindow();
}

void CMainFrame::setProgressCtrl(UINT uMin, UINT uMax, LPCTSTR strText)
{
    CSize textSize;
    szText = strText;
	szText += "   ";
	
    pOldFont = NULL;
	pStatusBarFont = NULL;
    CDC* pDC = NULL;
    UINT uStyle = 0;
	progressMax = uMax;

    //empty first pane of the status bar
    m_wndStatusBar.SetPaneText(0, _T(""), TRUE);
    m_wndStatusBar.Invalidate();
    m_wndStatusBar.UpdateWindow();

    uStyle = m_wndStatusBar.GetPaneStyle(0);
    m_wndStatusBar.SetPaneStyle(0, uStyle | SBPS_DISABLED);

    //  set the rectangle of the control + text as  2/3 of the first pane
    m_wndStatusBar.GetItemRect(0, &Rect);
        //Rect.left = Rect.right-250;
	Rect.left = 10;


    // create a window which will act as parent for the control
    _progressInfoWindow = new CWnd;
	_progressInfoWindow->Create(NULL, NULL,WS_VISIBLE | WS_CHILD, Rect, 
    &m_wndStatusBar, 0);
	
	// set the font of the status bar
    bkColor = ::GetSysColor(COLOR_ACTIVEBORDER);
    pStatusBarFont = m_wndStatusBar.GetFont();
    pDC = _progressInfoWindow->GetDC();
	
	pOldFont = pDC->SelectObject(pStatusBarFont);
	textSize = pDC->GetTextExtent(szText);
	
	_progressInfoWindow->GetClientRect(&Rect);
	pDC->SetBkColor(bkColor);

	// Draw the text on left
	Rect.top = 4;
		
    pDC->DrawText(szText, -1, &Rect, DT_LEFT);

    pDC->SelectObject(pOldFont);
    _progressInfoWindow->ReleaseDC(pDC);

	// set the rectangle of the control, after drawing the text
	Rect.top = 1;
	Rect.left += textSize.cx+50;
	//Rect.right = Rect.left+750;
	//Create the progress control
	
	_progressBar.Create(/*PBS_SMOOTH |*/ WS_VISIBLE|WS_CHILD, Rect, _progressInfoWindow, 
		ID_INDICATOR_CAPS);
	_progressBar.SetRange(uMin, uMax); //Set the range to between 0 and 100
	_progressBar.SetStep(1); // Set the step amount
}

void CMainFrame::removeProgressCtrl()
{
    UINT uStyle;

	_progressBar.DestroyWindow();
	_progressInfoWindow->DestroyWindow();
	delete _progressInfoWindow;
	
	uStyle = m_wndStatusBar.GetPaneStyle(0);
	m_wndStatusBar.SetPaneStyle(0, uStyle & ~SBPS_DISABLED);
	m_wndStatusBar.Invalidate();
	m_wndStatusBar.UpdateWindow();
}

void CMainFrame::StatusBarMessage(TCHAR* fmt, ...)

{

    TCHAR  buffer[256] = _T("");

 

    //직접적인 차일드만이 아닌 전체 차일드 트리를 검색

    CStatusBar *pStatus = (CStatusBar *)GetDescendantWindow(AFX_IDW_STATUS_BAR);

 

    //가변길이 인수를 사용한다.

    va_list   argptr;

    va_start(argptr, fmt);

    _vstprintf(buffer, fmt, argptr);

    va_end(argptr);

 

    if (pStatus != NULL) {

        pStatus->SetPaneText(0, buffer);

        pStatus->UpdateWindow();

    }

}



// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const    // override
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const    // override
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기


void CMainFrame::OnDestroy()    // message WM_DESTROY
{
	CFrameWnd::OnDestroy();

	// Clear
	int i = 0;

}

void CMainFrame::Add_ComboBox(CToolBar *pToolBar, CComboBox *pComboBox, int nIndex, int cx, int cy, DWORD dwStyle, UINT nID)
{
	CRect rect;
	pToolBar->GetItemRect(nIndex, rect);  // 20 번째
	rect.right = rect.left+cx;            // cx 는 폭
	rect.bottom = rect.top+cy;            // cy 는 길이
	rect.top += 3;

	if (!pComboBox->Create(dwStyle, rect, pToolBar, nID)) {
		TRACE(_T("Fail to create ComboBox\n"));
		return;
	}
	pComboBox->ShowWindow(SW_SHOW);
	pToolBar->SetButtonInfo(nIndex, nID, TBBS_SEPARATOR, cx);
	return;
}
CPreprocessor01View* CMainFrame::GetView()
{
	CPreprocessor01View* pView = (CPreprocessor01View*)GetActiveView();
	return pView;
}
void CMainFrame::OnSelchangeCombo()
{
    CString str;
	int nSel = m_ComboLine.GetCurSel();
	//m_ComboLine.GetLBText(nSel,str);
	//AfxMessageBox(str);
	CMainFrame* wndFrame = (CMainFrame*)AfxGetMainWnd();
	if (nSel == 0) (wndFrame->GetView())->Calc.Solid->currentline = -1;
	else (wndFrame->GetView())->Calc.Solid->currentline = nSel;
	(wndFrame->GetView())->Calc.seg_counter = 1;

	(wndFrame->GetView())->Invalidate();
}

//  CWinApp * pApp = (CWinApp*)AfxGetApp();
//  CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();
//  CDocument *pDoc = (CDocument *)pFrame->GetActiveDocument();
//  CView *pView = (CView *)pFrame->GetActiveView();
