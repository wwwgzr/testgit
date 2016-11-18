// stdafx.cpp : source file that includes just the standard includes
//	PlotCenter.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"



	#ifdef _ATL_STATIC_REGISTRY
#include <statreg.h>
#endif
#include <atlimpl.cpp>
CPlotContext    m_plotContext;//20160122切割指令识别
int nLarge1;//20160122切割指令识别
unsigned char g_cPrintCutSelect; //20101206 切割打印选择
CString strFileName1;//20160122切割指令识别
