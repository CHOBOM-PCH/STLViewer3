
// Preprocessor01.h : Preprocessor01 ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CPreprocessor01App:
// �� Ŭ������ ������ ���ؼ��� Preprocessor01.cpp�� �����Ͻʽÿ�.
//

class CPreprocessor01App : public CWinApp
{
public:
	CPreprocessor01App();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CPreprocessor01App theApp;
