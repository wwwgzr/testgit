// SplineLibrary.h : SplineLibrary DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSplineLibraryApp
// �йش���ʵ�ֵ���Ϣ������� SplineLibrary.cpp
//

class CSplineLibraryApp : public CWinApp
{
public:
	CSplineLibraryApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
