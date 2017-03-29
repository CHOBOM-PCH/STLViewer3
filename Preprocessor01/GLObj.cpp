// GLObj.cpp : implementation file
//

#include "stdafx.h"
#include <afxpriv.h> 

//#include "GL.h"
#include "Preprocessor01.h"

#include "GLObj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CGLObj

CGLObj::CGLObj()
{
	m_pDC = NULL;
	m_hRC = NULL;

	m_dAspectPrint = 1.0; 
	m_dNearPlane   = 1.0; 
	m_dFarPlane    = 15.0; 
	m_dMaxObjSize  = 14.0; 
	m_fRadius    = float(m_dNearPlane + m_dMaxObjSize / 2.0f); 

	m_xRotation = 0.0f;
	m_yRotation = 0.0f;
	m_zRotation = 0.0f;

	m_xTranslation = 0.0f;
	m_yTranslation = 0.0f;
	m_zTranslation = 0.0f;

	m_xScaling = 1.0f;
	m_yScaling = 1.0f;
	m_zScaling = 1.0f;

	m_ClearColorRed   = 0.90f;
	m_ClearColorGreen = 0.90f;
	m_ClearColorBlue  = 0.99f;

	// Initialize printing and clipboard info
	m_hDib = NULL;
	m_hOldDC = NULL;
	m_hMemDC = NULL;
	m_hOldRC = NULL;
	m_hMemRC = NULL;
	m_hOldDib = NULL;
	m_pBitmapBits = NULL;
}

CGLObj::~CGLObj()
{
	if (::wglGetCurrentContext())
		::wglMakeCurrent(NULL, NULL);
	
	if (m_hRC != NULL)
	{
		::wglDeleteContext(m_hRC);
		m_hRC = NULL;
	}
	
	if (m_hMemRC != NULL)
	{
		::wglDeleteContext(m_hMemRC);	
		m_hMemRC = NULL;
	}

	if (m_pDC != NULL)
	{
		delete m_pDC;	
		m_pDC = NULL;
	}

	if (m_hMemDC != NULL)
	{
		DeleteDC(m_hMemDC);
		m_hMemDC = NULL;
	}

	if (m_hDib != NULL)
	{
		DeleteObject(m_hDib);
		m_hDib = NULL;
	}

	m_hOldDC = NULL;
	m_hOldRC = NULL;
	m_hOldDib = NULL;
}

bool CGLObj::InitializeOpenGL(CView* pView)
{
	m_pDC = new CClientDC(pView);
    ASSERT(m_pDC);

	if (!SetDCPixelFormat(m_pDC->GetSafeHdc(), 
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_STEREO_DONTCARE))
		return false;

	m_hRC = ::wglCreateContext(m_pDC->GetSafeHdc());
	ASSERT(m_hRC);
	if(!::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC))
		return false;

	SetOpenGLState();

	CreateDisplayList(1);
	return true;
}

bool CGLObj::SetDCPixelFormat(HDC hDC, DWORD dwFlags)
{
	PIXELFORMATDESCRIPTOR pixelDesc;
	
	pixelDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixelDesc.nVersion = 1;
	
	pixelDesc.dwFlags = dwFlags;
	pixelDesc.iPixelType = PFD_TYPE_RGBA;
	pixelDesc.cColorBits = 24;
	pixelDesc.cRedBits = 8;
	pixelDesc.cRedShift = 16;
	pixelDesc.cGreenBits = 8;
	pixelDesc.cGreenShift = 8;
	pixelDesc.cBlueBits = 8;
	pixelDesc.cBlueShift = 0;
	pixelDesc.cAlphaBits = 0;
	pixelDesc.cAlphaShift = 0;
	pixelDesc.cAccumBits = 64;
	pixelDesc.cAccumRedBits = 16;
	pixelDesc.cAccumGreenBits = 16;
	pixelDesc.cAccumBlueBits = 16;
	pixelDesc.cAccumAlphaBits = 0;
	pixelDesc.cDepthBits = 32;
	pixelDesc.cStencilBits = 8;
	pixelDesc.cAuxBuffers = 0;
	pixelDesc.iLayerType = PFD_MAIN_PLANE;
	pixelDesc.bReserved = 0;
	pixelDesc.dwLayerMask = 0;
	pixelDesc.dwVisibleMask = 0;
	pixelDesc.dwDamageMask = 0;
	
	int nPixelIndex = ::ChoosePixelFormat(hDC, &pixelDesc);
	if (nPixelIndex == 0) // Choose default
	{
		nPixelIndex = 1;
		if (::DescribePixelFormat(hDC, nPixelIndex, 
			sizeof(PIXELFORMATDESCRIPTOR), &pixelDesc) == 0)
			return false;
	}

	if (!::SetPixelFormat(hDC, nPixelIndex, &pixelDesc))
		return false;

	return true;
}

void CGLObj::SetOpenGLState()
{
	// Default mode
	::glEnable(GL_NORMALIZE);
	::glEnable(GL_DEPTH_TEST);

	// Lights, material properties
	::glClearColor(m_ClearColorRed, m_ClearColorGreen, m_ClearColorBlue, 1.0f);
	::glClearDepth(1.0);
	::glShadeModel(GL_SMOOTH);

	// Default polygonmode
	::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void CGLObj::OnSize(int cx, int cy)
{
	::glViewport(0, 0, cx, cy);
	m_dAspect = (cy == 0) ? (double)cx : (double)cx/(double)cy;
	SetFrustum();
}

void CGLObj::SetFrustum()
{
	::glMatrixMode(GL_PROJECTION);
	::glLoadIdentity();
	::gluPerspective(45.0, m_dAspect, m_dNearPlane, m_dFarPlane); 
	::glMatrixMode(GL_MODELVIEW);
}

void CGLObj::CreateDisplayList(UINT nList)
{
	::glNewList(nList, GL_COMPILE);
	float fLen(3);
	// Color-Triangle
	::glBegin(GL_TRIANGLES);
		::glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		::glVertex3f(-fLen, fLen, 0);
		::glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
        ::glVertex3f(-fLen, -fLen, 0);
		::glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        ::glVertex3f(fLen, -fLen, 0);
	::glEnd();
	// Mono-Triangle
	::glBegin(GL_TRIANGLES);
		::glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		::glVertex3f(-fLen, fLen, 0);
		::glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        ::glVertex3f(fLen, fLen, 0);
		::glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        ::glVertex3f(fLen, -fLen, 0);
	::glEnd();
	::glEndList();
}

void CGLObj::OnPrint(CDC* pDC, CPrintInfo* pInfo, CView* pView) 
{
	OnPrint1(pDC, pInfo, pView);
	RenderScene();
	OnPrint2(pDC);
}

void CGLObj::OnPrint1(CDC* pDC, CPrintInfo* pInfo, CView* pView) 
{
	CRect rcClient;
	pView->GetClientRect(&rcClient); 
	float fClientRatio = float(rcClient.Height())/rcClient.Width();

	// Get page size
	m_szPage.cx  = pDC->GetDeviceCaps(HORZRES);
	m_szPage.cy = pDC->GetDeviceCaps(VERTRES);

	CSize szDIB;
	if (pInfo->m_bPreview)
	{
		// Use screen resolution for preview.
		szDIB.cx = rcClient.Width();
		szDIB.cy = rcClient.Height();
	}
	else  // Printing
	{
		// Use higher resolution for printing.
		// Adjust size according screen's ratio.
		if (m_szPage.cy > fClientRatio*m_szPage.cx)
		{
			// View area is wider than Printer area
			szDIB.cx = m_szPage.cx;
			szDIB.cy = long(fClientRatio*m_szPage.cx);
		}
		else
		{
			// View area is narrower than Printer area
			szDIB.cx = long(float(m_szPage.cy)/fClientRatio);
			szDIB.cy = m_szPage.cy;
		}
		// Reduce the Resolution if the Bitmap size is too big.
		// Ajdust the maximum value, which is depends on printer's memory.
		// I use 20 MB. 
		while (szDIB.cx*szDIB.cy > 20e6)   
		{
			szDIB.cx = szDIB.cx/2;
			szDIB.cy = szDIB.cy/2;
		}
	}

	TRACE("Buffer size: %d x %d = %6.2f MB\n", szDIB.cx, szDIB.cy, szDIB.cx*szDIB.cy*0.000001);
	
	// Initialize the bitmap header info.
	memset(&m_bmi, 0, sizeof(BITMAPINFO));
	m_bmi.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	m_bmi.bmiHeader.biWidth			= szDIB.cx;
	m_bmi.bmiHeader.biHeight		= szDIB.cy;
	m_bmi.bmiHeader.biPlanes		= 1;
	m_bmi.bmiHeader.biBitCount		= 24;
	m_bmi.bmiHeader.biCompression	= BI_RGB;
	m_bmi.bmiHeader.biSizeImage		= szDIB.cx * szDIB.cy * 3;

	// Create DIB
	HDC	hDC = ::GetDC(pView->m_hWnd);
	m_hDib = ::CreateDIBSection(hDC, &m_bmi, DIB_RGB_COLORS, &m_pBitmapBits, NULL, (DWORD)0);
	::ReleaseDC(pView->m_hWnd, hDC);

	// Create memory DC
	m_hMemDC = ::CreateCompatibleDC(NULL);
	if (!m_hMemDC)
	{
		DeleteObject(m_hDib);
		m_hDib = NULL;
		return;
	}

	m_hOldDib = SelectObject(m_hMemDC, m_hDib);

	// Setup memory DC's pixel format.
	if (!SetDCPixelFormat(m_hMemDC, PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_STEREO_DONTCARE))
	{
		SelectObject(m_hMemDC, m_hOldDib);
		DeleteObject(m_hDib);
		m_hDib = NULL;
		DeleteDC(m_hMemDC);
		m_hMemDC = NULL;
		return;
	}

	// Create memory RC
	m_hMemRC = ::wglCreateContext(m_hMemDC);
	if (!m_hMemRC)
	{
		SelectObject(m_hMemDC, m_hOldDib);
		DeleteObject(m_hDib);
		m_hDib = NULL;
		DeleteDC(m_hMemDC);
		m_hMemDC = NULL;
		return;
	}

	// Store old DC and RC
	m_hOldDC = ::wglGetCurrentDC();
	m_hOldRC = ::wglGetCurrentContext(); 

	// Make m_hMemRC the current RC.
	::wglMakeCurrent(m_hMemDC, m_hMemRC);

	SetOpenGLState();
	::glViewport(0, 0, szDIB.cx, szDIB.cy);
	SetFrustum();
	CreateDisplayList(1);
}

void CGLObj::OnPrint2(CDC* pDC) 
{
	::wglMakeCurrent(NULL, NULL);	
	::wglDeleteContext(m_hMemRC);

	// Restore last DC and RC
	::wglMakeCurrent(m_hOldDC, m_hOldRC);	

	// Size of printing image
	float fBmiRatio = float(m_bmi.bmiHeader.biHeight) / m_bmi.bmiHeader.biWidth;
	CSize szImageOnPage;  
	if (m_szPage.cx > m_szPage.cy)	 // Landscape page
	{
		if(fBmiRatio<1)	  // Landscape image
		{
			szImageOnPage.cx = m_szPage.cx;
			szImageOnPage.cy = long(fBmiRatio * m_szPage.cy);
		}
		else			  // Portrait image
		{
			szImageOnPage.cx = long(m_szPage.cy/fBmiRatio);
			szImageOnPage.cy = m_szPage.cy;
		}
	}
	else		    // Portrait page
	{
		if(fBmiRatio<1)	  // Landscape image
		{
			szImageOnPage.cx = m_szPage.cx;
			szImageOnPage.cy = long(fBmiRatio * m_szPage.cx);
		}
		else			  // Portrait image
		{
			szImageOnPage.cx = long(m_szPage.cx/fBmiRatio);
			szImageOnPage.cy = m_szPage.cy;
		}
	}

	CSize szOffset((m_szPage.cx - szImageOnPage.cx) / 2, (m_szPage.cy - szImageOnPage.cy) / 2);

	// Stretch the Dib to fit the image size.
	int nRet = ::StretchDIBits(pDC->GetSafeHdc(),
							  szOffset.cx, szOffset.cy,
							  szImageOnPage.cx, szImageOnPage.cy,
							  0, 0,
							  m_bmi.bmiHeader.biWidth, m_bmi.bmiHeader.biHeight,
							  (GLubyte*) m_pBitmapBits,
							  &m_bmi, DIB_RGB_COLORS, SRCCOPY);

	if(nRet == GDI_ERROR) {
		TRACE0("Failed in StretchDIBits()");
	}

	if (m_hOldDib) {
		SelectObject(m_hMemDC, m_hOldDib);
	}

	DeleteObject(m_hDib);
	m_hDib = NULL;
	DeleteDC(m_hMemDC);	
	m_hMemDC = NULL;
	m_hOldDC = NULL;
}

void CGLObj::RenderScene()
{	
	::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	::glPushMatrix();
	
	// Position
	::glTranslatef(m_xTranslation, m_yTranslation, m_zTranslation - m_fRadius);
	::glRotatef(m_xRotation, 1.0, 0.0, 0.0);
	::glRotatef(m_yRotation, 0.0, 1.0, 0.0);
	::glRotatef(m_zRotation, 0.0, 0.0, 1.0);

	::glCallList(1);

	::glPopMatrix();
	::glFinish();
}
