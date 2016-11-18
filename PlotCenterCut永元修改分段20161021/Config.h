// Config.h: interface for the CConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIG_H__00B4BD6D_AD00_46D1_AF8B_E9F207F5E9BA__INCLUDED_)
#define AFX_CONFIG_H__00B4BD6D_AD00_46D1_AF8B_E9F207F5E9BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define		MAX_PEN		8

class	CConfig
{
public:
	CConfig();
	virtual ~CConfig();
public:
	void	LoadConfig();
	void	SaveConfig();
public:
	CDWordArray		m_penary;
	BYTE			m_curPen;
	BYTE			m_curKnife;
public:
public:
	double			m_outUnit;
	UINT			m_curDevSel;
public:
	CString			m_recentSaveFile;
public:
	UINT			m_curModSel;
public:
	UINT			m_curPortSel;
public:
	UINT			m_curBaudSel;
public:
	double	m_inUnit;
	//gzr20160906 ×ÖÌå±äÁ¿ start
	LOGFONT m_lfDrawText;
};

#endif // !defined(AFX_CONFIG_H__00B4BD6D_AD00_46D1_AF8B_E9F207F5E9BA__INCLUDED_)
