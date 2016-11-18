/***********************************************
 * NAME    : 44BLIB.H                          *
 * Version : 17.Apr.00                         *
 ***********************************************/
 
#ifndef __44BLIB_H__
#define __44BLIB_H__

#include "..\target\def.h"
 
//#define DebugOut Uart_Printf
//
//#define _mini_(x1,x2) ((x1<x2)? x1:x2)
//#define _maxi_(x1,x2) ((x1>x2)? x1:x2)
//
//#define ONESEC0 (62500)	//16us resolution, max 1.04 sec
//#define ONESEC1 (31250)	//32us resolution, max 2.09 sec
//#define ONESEC2 (15625)	//64us resolution, max 4.19 sec
//#define ONESEC3 (7812)	//128us resolution, max 8.38 sec
//#define ONESEC4 (MCLK/128/(0xff+1))  //@60Mhz, 128*4us resolution, max 32.53 sec
//
////#define NULL 0
//
//#define EnterPWDN(clkcon) ((void (*)(int))0xe0)(clkcon)
//#define DOWNLOAD_ADDRESS _RAM_STARTADDRESS

/*44blib.c*/
void Delay(int time); //Watchdog Timer is used.
void *_malloc_(unsigned nbyte); 
void _free_(void *pt);
int _atoi_(char *str);
char *_itoa_(int num, char *str, int radix);
void Port_Init(void);
void Cache_Flush(void);
void SoftDelay(int us);
void _disable_(void);
void _enable_(void);

#endif /*__44BLIB_H___*/