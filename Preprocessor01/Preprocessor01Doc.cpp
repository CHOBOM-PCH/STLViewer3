
// Preprocessor01Doc.cpp : CPreprocessor01Doc 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Preprocessor01.h"
#endif

#include "MainFrm.h"
#include "Preprocessor01Doc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPreprocessor01Doc

IMPLEMENT_DYNCREATE(CPreprocessor01Doc, CDocument)

BEGIN_MESSAGE_MAP(CPreprocessor01Doc, CDocument)
	ON_COMMAND(ID_FILE_SAVE,      &CPreprocessor01Doc::OnFileSave)           // 여기에 있음       // pkc
	ON_COMMAND(ID_FILE_SAVE_AS,   &CPreprocessor01Doc::OnFileSaveAs)       // 여기에 있음
END_MESSAGE_MAP()


// CPreprocessor01Doc 생성/소멸

CPreprocessor01Doc::CPreprocessor01Doc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.
	int doc = 1;                  // (2)

	g_make = 0;
}

CPreprocessor01Doc::~CPreprocessor01Doc()
{
}

BOOL CPreprocessor01Doc::OnNewDocument()    // override
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.         // (5)
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}

void CPreprocessor01Doc::OnFileSave()        // pkc save
{
	FILE *fp;
	char szFilter[200];
	CString sFileName,sPathName,sExtName;

	CFileDialog    *pdlg;
	if (g_make) {
		strcpy(szFilter, "VCam Files (*.pmc)|*.pmc|All Files (*.*)|*.*||");
	} else {
		strcpy(szFilter, "VCam Files (*.stl)|*.stl|All Files (*.*)|*.*||");
	}
	pdlg  =  new CFileDialog(true,"stl|asc",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
							szFilter);
						//	(BOOL bOpenFileDialog,
						//LPCTSTR lpszDefExt = NULL, 
						//LPCTSTR lpszFileName = NULL, 
						// DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
						//LPCTSTR lpszFilter = NULL, CWnd* pParentWnd = NULL );

	if (pdlg->DoModal() != IDOK) { 
		delete pdlg;
		g_make = 0;
		return ;
	}
	
	sFileName = pdlg->GetFileTitle();
	sPathName = pdlg->GetPathName();    // sPathName.GetBuffer(_MAX_PATH)
	sExtName = pdlg->GetFileExt();
	delete pdlg;

	fp = fopen(sPathName.GetBuffer(_MAX_PATH), "w");  ///
	if (fp == NULL) {
		AfxMessageBox("File Save Failure.");
		g_make = 0;
		return;
	}

	if (g_make==1) {
		CMainFrame* wndFrame = (CMainFrame*)AfxGetMainWnd();
		(wndFrame->GetView())->MakeG(fp);
	}









	fclose(fp);

}
void CPreprocessor01Doc::OnFileSaveAs()        // pkc saveas
{
	int fsave = 1;
	OnFileSave();
}

// CPreprocessor01Doc serialization

void CPreprocessor01Doc::Serialize(CArchive& ar)       // override
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CPreprocessor01Doc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CPreprocessor01Doc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CPreprocessor01Doc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CPreprocessor01Doc 진단

#ifdef _DEBUG
void CPreprocessor01Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPreprocessor01Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CPreprocessor01Doc 명령
