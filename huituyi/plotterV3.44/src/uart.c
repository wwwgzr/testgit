#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "..\target\44b.h"
#include "..\target\option.h"
#include "..\Target\44blib.h"
#include "..\inc\extern.h"
#include "..\inc\defconst.h"
#include "..\inc\uart.h"

int _atoi_(char *str)
{
    int n=0;
    char c=1;

    while((c=*str++)&&(c>=0x30)&&(c<=0x39))
            n = n*10 + c-'0';
    return(n);
}

char *_itoa_(int num, char *str, int radix)
{
    char string[] = {"0123456789abcdef"};
    char* ptr = str;
    int i;
    int j;
    int temp;

    while (num)
    {
        *ptr++  = string[num % radix];
        num    /= radix;

        if (num < radix)
        {
            *ptr++  = string[num];
            *ptr    = '\0';
            break;
        }
    }

    j = ptr - str - 1;

    for (i = 0; i < (ptr - str) / 2; i++)
    {
        temp = str[i];
        str[i]  = str[j];
        str[j--] = temp;
    }

    return str;
}


//初始化串口0,baud为要设置的波特率
void Uart0_Init(int baud)
{
  int i;
  
  rUFCON0 = 0x0;     //FIFO disable
  rUMCON0 = 0x0;
  rULCON0 = 0x3;     //Normal,No parity,1 stop,8 bit
  rUCON0 = 0x245;     //rx=edge,tx=edge,disable timeout int.,disable rx error int.,
                     //normal,interrupt or polling
  rUBRDIV0 = ((int)(MCLK/16./baud + 0.5) -1);
  
  for (i=0;i<100;i++);
 
}

void SerialTxChar(char data)
{
	if(data=='\n') 
	{
		while(!(rUTRSTAT0 & 0x2));
		Delay(1);	//because the slow response of hyper_terminal 
		WrUTXH0('\r');
	}
	while(!(rUTRSTAT0 & 0x2));  //Wait until THR is empty.
	WrUTXH0(data);

}

void SerialTxString(char *s)
{
	 while(*s)
		SerialTxChar(*s++);
}

/*---------------------printf and support routines ---------------------*/
/* print c count times */
void PutRepChar(char c, int count)
{
	while (count--) Putch(c);
}

/* put string reverse */
void PutStringReverse(char *s, int index)
{
  while ((index--) > 0) Putch(s[index]);
}

/*-------------------------------------------------------------------------*/
/* 
   prints value in radix, in a field width width, with fill
   character fill
   if radix is negative, print as signed quantity
   if width is negative, left justify
   if width is 0, use whatever is needed
   if fill is 0, use ' '
 */

static void PutNumber(int value, int radix, int width, char fill)
{
  char buffer[40];
  int bi = 0;
  int unsigned uvalue;
  short int digit;
  short int left = FALSE;
  short int negative = FALSE;
  
  if (fill == 0) fill = ' ';

  if (width < 0) 
  {
    width = -width;
    left = TRUE;
  }
  if (width < 0 || width > 80) width = 0;
  
  if (radix < 0) 
  {
    radix = -radix;
    if (value < 0) 
    {
      negative = TRUE;
      value = -value;
    }
  }
  uvalue = value;
  do 
  {
    if (radix != 16) 
    {
      digit = uvalue % radix ;
      uvalue = uvalue / radix ;
    }
    else 
    {
      digit = uvalue & 0xf;
      uvalue = uvalue >> 4;
    }
    buffer[bi] = digit + ((digit <= 9) ? '0' : ('A' - 10));
    bi++;

    if (uvalue != 0) 
    {
      	if ((radix==10)&&((bi==3)||(bi==7)||(bi==11)|(bi==15))) 
      	{
			buffer[bi++]=',';
      	}
    }
  } while (uvalue != 0);

  if (negative) 
  {
    buffer[bi] = '-';
    bi += 1;
  }
  if (width <= bi) PutStringReverse(buffer, bi);
  else 
  {
    width -= bi;
    if (!left) PutRepChar(fill, width);
    PutStringReverse(buffer, bi);
    if (left) PutRepChar(fill, width);
  } 
}

/*-------------------------------------------------------------------------*/
static char *FormatItemd(char *f, double a)
{
 	char temp[10];//,temp2[30];
	int i;

  	sprintf(temp,  "%5.3f",  a);
	for(i=0;i<10;i++)
	{
		if(temp[i] == 0x14)
		{
			temp[i] = 0x2e;	
			break;
		}
	}
	//sprintf(temp2, "%x,%x,%x,%x", temp[0],temp[1],temp[2],temp[3]);
  	//LcdDsip(temp, temp2);
	Puts(temp);
	return(++f);
}

static char *FormatItem(char *f, int a)
{
  char c;
  int fieldwidth = 0;
  int leftjust = FALSE;
  int radix = 0;
  char fill = ' ';
  
  if (*f == '0') fill = '0';
  while ((c = *f++)!=0) 
  {
    if (c >= '0' && c <= '9') 
    {
      fieldwidth = (fieldwidth * 10) + (c - '0');
    }
    else switch (c) {
    case '\000': return(--f);
    case '%': Putch('%');
      return(f);
    case '-': leftjust = TRUE;
      break;
    case 'c': 
    {
      if (leftjust) Putch(a & 0x7f);
      if (fieldwidth > 0) PutRepChar(fill, fieldwidth - 1);
      if (!leftjust) Putch(a & 0x7f);
      return(f);
    }
    case 's': 
    {
      if (leftjust) Puts((char *) a);
      if (fieldwidth > strlen((char *) a))
	PutRepChar(fill, fieldwidth - strlen((char *)a));
      if (!leftjust) Puts((char *) a);
      return(f);
    }
    case 'd': 
    case 'i': radix = -10;break;
    case 'u': radix = 10;break;
    case 'x': radix = 16;break;
    case 'X': radix = 16;break;
    case 'o': radix = 8;break;
    default : radix = 3;break;/* unknown switch! */
    }
    if (radix) break;
  }
  if (leftjust) fieldwidth = -fieldwidth;
  PutNumber(a, radix, fieldwidth, fill);
  return(f);
}

#define vaStart(list, param) list = (char*)((int)&param + sizeof(param))
#define vaArg(list, type) ((type *)(list += sizeof(type)))[-1]
#define vaEnd(list)

void Printf(char *f, ...)       // variable arguments 
{
//	U32 mode ;
	char *argP;
	
	// disable IRQs and FIQs
//	mode = uHALir_ReadMode() ;
//	uHALir_WriteMode(mode | NoFIQ | NoIRQ) ;
	vaStart(argP,f);		// point at the end of the format string
	while (*f) {			// this works because args are all ints 
		if (*f == '%')
		{
			if(*(f+1) == 'f')
				f = FormatItemd(f + 1, vaArg(argP, double));
			else
				f = FormatItem(f + 1, vaArg(argP, int));
		}
		else
			Putch(*f++);

	}
	vaEnd(argP);

	// restore the previous mode
//	uHALir_WriteMode(mode) ;
}

extern void lcd_refresh(void);
void LcdDsip(char *s1, char *s2)
{
	if(s1==NULL) strcpy(s1, "NULL");
	if(s2==NULL) strcpy(s2, "NULL");
	
	strcpy(gl_cLcdDispBuf[0],s1);
	strcpy(gl_cLcdDispBuf[1],s2);
	lcd_refresh();
}

