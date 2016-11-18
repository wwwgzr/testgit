/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Thu Apr 09 16:59:47 2009
 */
/* Compiler settings for D:\喷墨绘图仪\bak\喷墨机绘图中心\可读外用文件\NewPlotCenter\PlotCenter\PlotCenter.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID LIBID_PlotCenterLib = {0x2935D650,0xF72F,0x4C89,{0x95,0x41,0xE1,0x2D,0xED,0xDD,0x0F,0x8E}};


#ifdef __cplusplus
}
#endif

