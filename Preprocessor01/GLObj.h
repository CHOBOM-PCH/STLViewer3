#if !defined(AFX_CGLOBJ_H__0EBE7241_BD6F_11D3_85C9_00E02930B0CE__INCLUDED_)
#define AFX_CGLOBJ_H__0EBE7241_BD6F_11D3_85C9_00E02930B0CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GLObj.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGLObj command target

class CGLObj
{
public:
	CGLObj();           

// Attributes
public:
	CDC*  m_pDC;
 	HGLRC m_hRC;
protected:
	// Clear colors
	float m_ClearColorRed;
	float m_ClearColorGreen;
	float m_ClearColorBlue;

	// Rotation, translation, and scaling
	float m_xRotation;
	float m_yRotation;
	float m_zRotation;
	float m_xTranslation;
	float m_yTranslation;
	float m_zTranslation;
	float m_xScaling;
	float m_yScaling;
	float m_zScaling;

	// Size
    float		m_fRadius;
	double		m_dAspectPrint; 
	double		m_dNearPlane; 
	double		m_dFarPlane; 
	double		m_dMaxObjSize; 
	double		m_dAspect;

	// Printing
	HDC			m_hOldDC;
	HDC			m_hMemDC;
	HGLRC		m_hOldRC;
	HGLRC		m_hMemRC;
	BITMAPINFO	m_bmi;
	LPVOID		m_pBitmapBits;
	HBITMAP		m_hDib;
	HGDIOBJ		m_hOldDib;
	CSize		m_szPage;	

// Operations
protected:
	virtual bool InitializeOpenGL(CView* pView);
	virtual void SetFrustum();
	virtual bool SetDCPixelFormat(HDC hDC, DWORD dwFlags);
	virtual void SetOpenGLState();
	virtual void CreateDisplayList(UINT nList);
	virtual void RenderScene();
	virtual void OnSize(int cx, int cy);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo, CView* pView);
	virtual void OnPrint1(CDC* pDC, CPrintInfo* pInfo, CView* pView);
	virtual void OnPrint2(CDC* pDC);

// Implementation
protected:
	virtual ~CGLObj();
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_CGLOBJ_H__0EBE7241_BD6F_11D3_85C9_00E02930B0CE__INCLUDED_)
