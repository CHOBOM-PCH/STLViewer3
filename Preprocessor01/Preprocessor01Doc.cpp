
// Preprocessor01Doc.cpp : CPreprocessor01Doc Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
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
	ON_COMMAND(ID_FILE_SAVE,      &CPreprocessor01Doc::OnFileSave)           // ���⿡ ����       // pkc
	ON_COMMAND(ID_FILE_SAVE_AS,   &CPreprocessor01Doc::OnFileSaveAs)       // ���⿡ ����
END_MESSAGE_MAP()


// CPreprocessor01Doc ����/�Ҹ�

CPreprocessor01Doc::CPreprocessor01Doc()
{
	// TODO: ���⿡ ��ȸ�� ���� �ڵ带 �߰��մϴ�.
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

	// TODO: ���⿡ ���ʱ�ȭ �ڵ带 �߰��մϴ�.         // (5)
	// SDI ������ �� ������ �ٽ� ����մϴ�.

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
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}

#ifdef SHARED_HANDLERS

// ����� �׸��� �����մϴ�.
void CPreprocessor01Doc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// ������ �����͸� �׸����� �� �ڵ带 �����Ͻʽÿ�.
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

// �˻� ó���⸦ �����մϴ�.
void CPreprocessor01Doc::InitializeSearchContent()
{
	CString strSearchContent;
	// ������ �����Ϳ��� �˻� �������� �����մϴ�.
	// ������ �κ��� ";"�� ���еǾ�� �մϴ�.

	// ��: strSearchContent = _T("point;rectangle;circle;ole object;");
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

// CPreprocessor01Doc ����

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


// CPreprocessor01Doc ���
