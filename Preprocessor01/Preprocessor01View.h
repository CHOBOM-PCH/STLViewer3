
// Preprocessor01View.h : CPreprocessor01View 클래스의 인터페이스
//

#include "Calc.h"

class CPreprocessor01Doc;

//#include "GLObj.h"

#pragma once

//#define M_MAX(a,b) ((a>=b) ? a:b  );
//#define M_MIN(a,b) ((a<=b) ? a:b  );
//#define ISOMETRICX	-60.0
//#define ISOMETRICY	0.0
//#define ISOMETRICZ	-135.0
//#define XVIEWX		-90.0
//#define XVIEWY		0.0
//#define XVIEWZ		-90.0
//#define YVIEWX		-90.0
//#define YVIEWY		0.0
//#define YVIEWZ		180.0
//#define ZVIEWX		0.0
//#define ZVIEWY		0.0
//#define ZVIEWZ		0.0
//#define XREVERSEVIEWX		-90.0
//#define XREVERSEVIEWY		0.0
//#define XREVERSEVIEWZ		90.0
//#define YREVERSEVIEWX		-90.0
//#define YREVERSEVIEWY		0.0
//#define YREVERSEVIEWZ		0.0
//#define ZREVERSEVIEWX		0.0
//#define ZREVERSEVIEWY		180.0
//#define ZREVERSEVIEWZ		90.0
//#define CONST_REG_NUMBER_DATE 0417

enum enum_mouse_mode
{
	MOUSE_NONE,	MOUSE_ROTATION1, MOUSE_ROTATION2, MOUSE_TRANSLATION, MOUSE_ZOOMING
};

enum enum_mode {
	NO_MODE,
	STL_MODE,
	CLI_MODE,
	WHOLE_SLICE_MODE,
	CHECK_MODE,
	REPAIR_MODE,
	SIMULATION_MODE,
	SLICE_LAYER_MODE,
	DEPART_MODE
};

enum enum_model_style {
	MODEL_SHADE, MODEL_TRANSPARENCY,	MODEL_WIRE_FRAME,	MODEL_HIDDEN_LINE
};

class CPreprocessor01View : public CView
//class CPreprocessor01View : public CView, virtual public CGLObj
//class CPreprocessor01View : public CPreviewView
{
protected: // serialization에서만 만들어집니다.
	CPreprocessor01View();
	DECLARE_DYNCREATE(CPreprocessor01View)

	HDC hDC;
	HGLRC hRC;


// 특성입니다.
public:
	CPreprocessor01Doc* GetDocument() const;
	CString path;
	CClientDC *m_pDC;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CPreprocessor01View();



#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif



public:
	double width, height;
	double CenterX,CenterY,WinSize;
	double Y_Rotate;
	double X_Rotate;
	double Z_Rotate;
	double ZoomScale;
	double StartX,StartY,EndX,EndY;
	double fTransX,fTransY,fTransZ;
	double AxisValueX,AxisValueY,AxisValueZ;
	double axis_size;


	int ModelStyle, ViewPoint, DrawMode ,MouseMode, SectionMode;

	bool IS_LAYOUT;
	int platform_x_size,platform_y_size,platform_z_size;

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	CCalc Calc;                                     // pkc
	int fnew;
	bool IS_DRAW_AXIS,IS_DRAW_NET,IS_ZOOMING;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void SetDCPixelFormat(HDC hDC);
	void InitOpenGL(void);
	void InitProjection(void);
	void InitLight(void);
	void InitShadingModel(void);

	void DrawScene(int dir = 1);
	void DrawPlatform(void);
	void OnViewStl();

	afx_msg void OnFileOpen();
	afx_msg void OnFileNew();
	afx_msg void OnViewGrid();

	
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	afx_msg void OnRotationMode();
	afx_msg void OnUpdateRotationMode(CCmdUI *pCmdUI);
	afx_msg void OnTransMode();
	afx_msg void OnUpdateTransMode(CCmdUI *pCmdUI);


	
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnFillPolygon();
	afx_msg void OnTransparency();
	afx_msg void OnWireFrame();
	afx_msg void OnUpdateFillPolygon(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTransparency(CCmdUI *pCmdUI);
	afx_msg void OnUpdateWireFrame(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewGrid(CCmdUI *pCmdUI);
	void DrawAxis(void);
	void DrawMainaxis(void);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnFilePrintPreview();



	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint1(CDC* pDC, CPrintInfo* pInfo, CView* pView);
	virtual void OnPrint2(CDC* pDC);
	virtual BOOL SetDCPixelFormat(HDC hDC, DWORD dwFlags);

	CSize		m_szPage;
	BITMAPINFO	m_bmi;
	HBITMAP		m_hDib;
	LPVOID		m_pBitmapBits;
	HDC			m_hOldDC;
	HDC			m_hMemDC;
	HGLRC		m_hOldRC;
	HGLRC		m_hMemRC;
	HGDIOBJ		m_hOldDib;

	afx_msg void OnDecLine();
	afx_msg void OnIncLine();
	afx_msg void OnChangeOrder();
	afx_msg void OnMakeG();
	afx_msg void OnViewG();
	void MakeG(FILE *fpp);
	void ReadINI();
};

#ifndef _DEBUG  // Preprocessor01View.cpp의 디버그 버전
inline CPreprocessor01Doc* CPreprocessor01View::GetDocument() const
   { return reinterpret_cast<CPreprocessor01Doc*>(m_pDocument); }
#endif

