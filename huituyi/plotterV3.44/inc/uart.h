#ifndef __UART_H__
#define __UART_H__

#include "..\target\def.h"

#ifdef __cplusplus
extern "C" {
#endif

void Uart0_Init(int baud);

void SerialTxChar(char);
void SerialTxString(char *);

#define	Putch	SerialTxChar
#define	Putchar	putch
#define	Puts		SerialTxString

void LcdDsip(char *s1, char *s2);
void Printf(char *f, ...);

#ifdef __cplusplus
}
#endif

#endif 

