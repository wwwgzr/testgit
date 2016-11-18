#ifndef __UART_H__
#define __UART_H__

#include "..\target\def.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define DUG	1

#ifdef DUG 
char IntMskFlag=0;
#endif

void  FloatPrecision(char *buf, int decimalLen);

void Uart0_Init(int baud);

void SerialTxChar(char);
void SerialTxString(char *);

#define	Putch	SerialTxChar
#define	Putchar	putch
#define	Puts		SerialTxString
void DisableIntMsk(void);
void EnableIntMsk(void);
void Printf(char *f, ...);

#ifdef __cplusplus
}
#endif

#endif 

