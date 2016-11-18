// Config.cpp: implementation of the CConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Config.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfig::CConfig()
{
	m_inUnit = 0.025;
}

CConfig::~CConfig()
{

}


void	CConfig::LoadConfig()
{
	char *	temp = new char[MAX_PATH + 1];
	if (temp == NULL)
		return;

	ZeroMemory(temp, MAX_PATH);
	GetModuleFileName(AfxGetInstanceHandle(), temp, MAX_PATH);

	int	i;
	for(i = MAX_PATH; i >= 0;  i--)
	{
		if (temp[i]=='\\')
		{
			temp[i] = '\0';
			break;
		}
	}

	CString	name;
	name.Format("%s\\plotcfg.ini", temp);

	LPTSTR	pfilename = name.GetBuffer(name.GetLength());

	bool	fileExist = false;
	CFile	file;

	TRY
	{
		if (file.Open(pfilename, CFile::modeRead))
		{
			fileExist = true;
			file.Close();
		}

	}
	CATCH( CFileException, e )
	{
	   #ifdef _DEBUG
		  afxDump << "File could not be opened " << e->m_cause << "\n";
	   #endif
	}
	END_CATCH


	int		pennum;
	long	pen;
	CString	sec;
	LPTSTR	pSec;
	bool	configok = true;
	bool	fnmok = true;

	if (fileExist)
	{
		ZeroMemory(temp, MAX_PATH);
		GetPrivateProfileString("PENS", "PENNUM", "", temp, MAX_PATH, pfilename);

		if (temp[0] != '\0')
		{
			pennum = atoi(temp);
			if (pennum > 0)
			{
				for (i = 0; i < pennum; i++)
				{
					sec.Format("PEN%d", i);
					pSec = sec.GetBuffer(sec.GetLength());

					ZeroMemory(temp, MAX_PATH);
					GetPrivateProfileString("PENS", pSec, "", temp, MAX_PATH, pfilename);
					if (temp[0] != '\0')
					{
						pen = atol(temp);
						m_penary.Add(pen);
					}
					else
					{
						configok = false;
						break;
					}
				}
			}
			else
			{
				configok = false;
			}
		}
		else
		{
			configok = false;
		}

		ZeroMemory(temp, MAX_PATH);
		GetPrivateProfileString("FILES", "RECENTSAVEFILE", "", temp, MAX_PATH, pfilename);
		if (temp[0] != '\0')
		{
			m_recentSaveFile.Format("%s", temp);
		}
		else
		{
			fnmok = false;
		}

	}
	else
	{
		configok = false;
		fnmok = false;
	}

	if (!configok)
	{
		m_penary.RemoveAll();
		DWORD	tmp = 0;
		pennum = MAX_PEN;
		for (UINT j = 0; j < MAX_PEN; j++)
		{
			if (j == 4)
			{
				m_penary.Add(tmp + 0x0000ff00);
			}
			else
			{
				m_penary.Add(tmp);
			}

			tmp += 0x01000000;
		}
	}

	if (!fnmok)
	{
		ZeroMemory(temp, MAX_PATH);

		GetModuleFileName(AfxGetInstanceHandle(), temp, MAX_PATH);
		int	i;
		for(i = MAX_PATH; i >= 0;  i--)
		{
			if (temp[i]=='\\')
			{
				temp[i] = '\0';
				break;
			}
		}
		m_recentSaveFile.Format("%s\\noname.plt", temp);
	}

	if (fileExist && configok)
	{
		ZeroMemory(temp, MAX_PATH);
		GetPrivateProfileString("PENS", "CURPEN", "", temp, MAX_PATH, pfilename);
		if (temp[0] != '\0')
		{
			pen = atoi(temp);
			if (pen >= 0)
			{
				m_curPen = LOBYTE(LOWORD(pen));
			}
			else
			{
				if (pennum >= 1)
					m_curPen = 1;
				else
					m_curPen = 0;
			}
		}
		else
		{
			if (pennum >= 1)
				m_curPen = 1;
			else
				m_curPen = 0;
		}

		ZeroMemory(temp, MAX_PATH);
		GetPrivateProfileString("PENS", "CURKNIFE", "", temp, MAX_PATH, pfilename);
		if (temp[0] != '\0')
		{
			pen = atoi(temp);
			if (pen >= 0)
			{
				m_curKnife = LOBYTE(LOWORD(pen));
			}
			else
			{
				if (pennum >= 4)
					m_curKnife = 4;
				else
					m_curKnife = 0;
			}
		}
		else
		{
			if (pennum >= 4)
				m_curKnife = 4;
			else
				m_curKnife = 0;
		}
	}
	else
	{
		m_curPen = 1;
		m_curKnife = 4;
	}

	if (fileExist)
	{
		ZeroMemory(temp, MAX_PATH);
		GetPrivateProfileString("OUTPUT", "UNIT", "", temp, MAX_PATH, pfilename);
		double	unit;
		if (temp[0] != '\0')
		{
			unit = atof(temp);
		}
		else
		{
			unit = 0.025;
		}
		m_outUnit = unit;
	}
	else
	{
		m_outUnit = 0.025;
	}

	if (fileExist)
	{
		ZeroMemory(temp, MAX_PATH);
		GetPrivateProfileString("OUTPUT", "CURDESEL", "", temp, MAX_PATH, pfilename);
		UINT	cursel;
		if (temp[0] != '\0')
		{
			cursel = atoi(temp);
		}
		else
		{
			cursel = 0;
		}
		m_curDevSel = cursel;
	}
	else
	{
		m_curDevSel = 0;
	}


	if (fileExist)
	{
		ZeroMemory(temp, MAX_PATH);
		GetPrivateProfileString("PORT", "POTSEL", "", temp, MAX_PATH, pfilename);
		UINT	cursel;
		if (temp[0] != '\0')
		{
			cursel = atoi(temp);
		}
		else
		{
			cursel = 0;
		}
		m_curPortSel = cursel;
	}
	else
	{
		m_curPortSel = 0;
	}

	if (fileExist)
	{
		ZeroMemory(temp, MAX_PATH);
		GetPrivateProfileString("SERPORT", "BAUDSEL", "", temp, MAX_PATH, pfilename);
		UINT	cursel;
		if (temp[0] != '\0')
		{
			cursel = atoi(temp);
		}
		else
		{
			cursel = 0;
		}
		m_curBaudSel = cursel;
	}
	else
	{
		m_curBaudSel = 0;
	}

	delete	[]temp;
}

void	CConfig::SaveConfig()
{
	char *	temp = new char[MAX_PATH + 1];
	if (temp == NULL)
		return;

	ZeroMemory(temp, MAX_PATH);
	GetModuleFileName(AfxGetInstanceHandle(), temp, MAX_PATH);
	int	i;
	for(i = MAX_PATH; i >= 0;  i--)
	{
		if (temp[i]=='\\')
		{
			temp[i] = '\0';
			break;
		}
	}
	CString	name;
	name.Format("%s\\plotcfg.ini", temp);

	delete	[]temp;
	temp = NULL;

	LPTSTR	pfilename = name.GetBuffer(name.GetLength());

	BOOL	ret = DeleteFile(pfilename);

	CString		value;

	int	n = m_penary.GetSize();
	value.Format("%d", n);
	LPTSTR	pValue = value.GetBuffer(value.GetLength());
	WritePrivateProfileString("PENS", "PENNUM", pValue, pfilename);

	CString		sec;
	LPTSTR		pSec;

	for (i = 0; i < n; i++)
	{
		sec.Format("PEN%d", i);
		pSec = sec.GetBuffer(sec.GetLength());
		value.Format("%d", m_penary.GetAt(i));
		pValue = value.GetBuffer(value.GetLength());
		WritePrivateProfileString("PENS", pSec, pValue, pfilename);
	}

	value.Format("%d", m_curPen);
	pValue = value.GetBuffer(value.GetLength());
	WritePrivateProfileString("PENS", "CURPEN", pValue, pfilename);

	value.Format("%d", m_curKnife);
	pValue = value.GetBuffer(value.GetLength());
	WritePrivateProfileString("PENS", "CURKNIFE", pValue, pfilename);

	//////////

	pValue = m_recentSaveFile.GetBuffer(m_recentSaveFile.GetLength());
	WritePrivateProfileString("FILES", "RECENTSAVEFILE", pValue, pfilename);


	//////////
	value.Format("%d", m_curDevSel);
	pValue = value.GetBuffer(value.GetLength());
	WritePrivateProfileString("OUTPUT", "CURDESEL", pValue, pfilename);

	value.Format("%f", m_outUnit);
	pValue = value.GetBuffer(value.GetLength());
	WritePrivateProfileString("OUTPUT", "UNIT", pValue, pfilename);

	//////////
	value.Format("%d", m_curPortSel);
	pValue = value.GetBuffer(value.GetLength());
	WritePrivateProfileString("PORT", "POTSEL", pValue, pfilename);

	//////////
	value.Format("%d", m_curBaudSel);
	pValue = value.GetBuffer(value.GetLength());
	WritePrivateProfileString("SERPORT", "BAUDSEL", pValue, pfilename);

}
