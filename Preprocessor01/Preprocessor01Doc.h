
// Preprocessor01Doc.h : CPreprocessor01Doc Ŭ������ �������̽�
//


#pragma once


class CPreprocessor01Doc : public CDocument
{
protected: // serialization������ ��������ϴ�.
	CPreprocessor01Doc();
	DECLARE_DYNCREATE(CPreprocessor01Doc)

// Ư���Դϴ�.
public:

// �۾��Դϴ�.
public:

	int g_make;


// �������Դϴ�.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// �����Դϴ�.
public:
	virtual ~CPreprocessor01Doc();


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif



protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// �˻� ó���⿡ ���� �˻� �������� �����ϴ� ����� �Լ�
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
