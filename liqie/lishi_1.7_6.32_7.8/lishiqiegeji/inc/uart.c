//****************UART.C******************************
//功能简介
// 1,串口初始化
// 2,printf向PC输出调试信息

//****************************************************
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "..\target\44b.h"
#include "..\target\option.h"
#include "..\Target\44blib.h"
#include "..\src\extern.h"
#include "..\src\defconst.h"
#include "..\src\uart.h"

#define vaStart(list, param) list = (char*)((int)&param + sizeof(param))
#define vaArg(list, type) ((type *)(list += sizeof(type)))[-1]
#define vaEnd(list)

#define ReleaseVersion

void Uart0_Init(int baud)
{
  int i;
  
  rUFCON0 = 0x0;     //FIFO disable
  rUMCON0 = 0x0;	//	0X10 自动模式
  rULCON0 = 0x3;     //Normal,No parity,1 stop,8 bit
  rUCON0 = 0x245;     //rx=edge,tx=edge,disable timeout int.,disable rx error int.,
                     //normal,interr upt or polling

  rUMCON0 = 0X10;
  rUBRDIV0 = ((int)(MCLK/16./baud + 0.5) -1);
  
  for (i=0;i<100;i++);


}

#ifndef ReleaseVersion
void Memcpy(void *s1, const void *s2, int n)
{
	int i;

	for (i = 0; i < n; i++)
		((char *)(s1))[i] = ((const char *)(s2))[i];
}

void  FloatPrecision(char *buf, int decimalLen)
{
	int iBufLen, iDecLen=0;
	char cDecFlag=0;
	iBufLen = strlen(buf);
//	Printf("buf: %x,%x,%x,%x   iBufLen:%d, decimalLen:%d\n",  buf[0],buf[1],buf[2],buf[3],  iBufLen, decimalLen);
	while(*buf != '\0')
	{
		if(*buf < 0x30 || *buf > 0x39)
		{
			cDecFlag = 1;
			*buf  = 0x2e;
			buf++;
			continue;
		}
		if(cDecFlag)
		{
			iDecLen += 1;
			if(iDecLen > decimalLen)
			{
				*buf = '\0';
				return;
			}
		}
		buf++;
	}
	
}


//读串口准备
int SerialRxReady(void)
{
	return (rUTRSTAT0 & 0x1);	//Receive data ready
}

//读
char SerialRxKey(void)
{

	if((rUTRSTAT0 & 0x1)) //Receive data ready
		return RdURXH0();

	return 0;
}
//读一个字符
char SerialRxChar(void)
{

	while(!(rUTRSTAT0 & 0x1)); //Receive data ready
	return RdURXH0();

}
//读数据到缓冲区
int SerialRxToBuf(char *b)
{
	if(rUTRSTAT0 & 0x1)    //Receive data ready
	    *b = RdURXH0();
	else
	    return 0;

    
    return 1;
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
 	char temp[50];//, temp2[80];
	int i;
	int iLen;
	
  	sprintf(temp,  "%f",  a);
	iLen = strlen(temp);
	
	for(i=0;i<iLen;i++)
	{
		if((temp[i] < 0x30 || temp[i] > 0x39)&&(temp[i]!='\0'))
		{
			temp[i] = 0x2e;	
			break;
		}
	}
	
	//sprintf(temp2, "%x,%x,%x,%x", temp[0],temp[1],temp[2],temp[3]);
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


//********************************
//读入数据
//关中断
void DisableIntMsk(void)
{
	rI_ISPC=BIT_EINT0;
	rINTMSK = BIT_GLOBAL;		//屏蔽所有中断
	//IntMskFlag = 1;
}

//开中断
void EnableIntMsk(void)
{
	rI_ISPC = 0x1ffffff;
	rINTMSK = ~(BIT_GLOBAL|BIT_URXD0|BIT_UERR01|BIT_TIMER0);
	//IntMskFlag = 0;
}

void ReadSerialKeyNum(char *command)
{
	int i, j, k, key;
	//char cEnd;

	for(i=0;i<80;i++)
		command[i] = '\0';
	
	i = 0;
	j = 0;
	//cEnd = 1;
	
	while(1)
	{				
		if(SerialRxReady())
		{
			
			key = SerialRxKey();
			if(key == ENTER_KEY)
			{
				Puts("\n");
				return;
			}
			else if(key == BACK_KEY && j>0)
			{
				if(i==j)
				{
					i--;
					j--;
					Puts("\b \b");
				}
				else
				{
					i--;
					j--;
					for(k=j;k<i;k++)
						command[k]=command[k+1];
					command[k] = ' ';
					Puts("\b \b");
					for(k=j;k<i+1;k++)
						Putch(command[k]);
					for(k=0;k<i+1-j;k++)
					{
						Putch(0x1b);
						Putch(0x5b);
						Putch(LEFT_KEY);
					}
				}
				command[i] = '\0';
			}
			else if(key>=0x20 && key<=0x7e && i<(80-1))
			{
				if(i==j)
				{
					command[i++] = key;
					Putch(key);
					j++;
				}
				else
				{
					for(k=i;k>j;k--)
						command[k] = command[k-1];
					command[j] = key;
					i++;
					for(k=j;k<i;k++)
						Putch(command[k]);
					j++;
					for(k=0;k<i-j;k++)
					{
						Putch(0x1b);
						Putch(0x5b);
						Putch(LEFT_KEY);
					}
				}
				command[i] = '\0';
			}
		}
	}	
	
}

int pow_my(int base,int n)
{    
    int result=1;
    while(n-->0)
    {
        result *= base;
    }
    return result;
}

double atol_my(char *str)
{
    int i=0;
    int j=0;
    int sum=0;
    int flag=0, bMinus=0;
    double decimal=0.0;
    
    int len=strlen(str);

    for(i=0;i<len;i++,str++)
    {
		if(*str=='.') {flag=1;continue;}
		if(*str=='-') {bMinus=1;continue;}
		if(*str!='.'&&flag==0)
		{
			sum = sum * 10+(*str-'0');
		}
    
		if(flag==1)
		{
			j++;
    
			decimal += (double)(*str-'0')/pow_my(10,j);
    
		}
    }/*for end*/

    if (bMinus)
    {
        return -(sum+decimal);
    }else
    {
        return sum+decimal;
    }
}

unsigned int Atox(char buf[])
{
	unsigned int result=0;
	char oxch[6]={0xa, 0xb, 0xc, 0xd, 0xe, 0xf};
	//int iBufLen,i;
	//iBufLen = strlen(buf);
	//Printf("buf:%s, iBufLen:%d\n", buf, iBufLen);
	
	//for(i=0; i<iBufLen; i++)
	while( *buf!= '\0' )
	{
		if(*buf >= 'A' && *buf <='Z') *buf+=0x20;

		if((*buf >= 0x30 && *buf <= 0x39))
		{
			result <<= 4;
			result += *buf - '0';
		}
		else if((*buf >= 'a' && *buf <= 'f'))
		{
			result <<= 4;
			result += oxch[*buf-'a'];
		}
		buf++;
	}
	//Printf("result:%x\n", result);
	return (result);
}

#define   TYPECHANG(data, type)  ((type *)(data))

int Atoi(char *str)
{
    int n=0;
    char c=1;

    while((c=*str++)&&(c>=0x30)&&(c<=0x39))
            n = n*10 + c-'0';
    return(n);
}

void Printf(char *f, ...)       // variable arguments 
{
	char *argP;
	
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
	
	Delay(100);
}


void  Scanf(char *f, void  *argP)
{
	char ReadBuff[80];//, cBuff[20];
	//double dTemp;
	#ifdef DUG
	IntMskFlag =1;
	#endif
	DisableIntMsk( );
	while (*f) {			// this works because args are all ints 
		if (*f == '%')
		{
			switch(*(f+1) )
			{
				case 'd':
					ReadSerialKeyNum(ReadBuff);
					*TYPECHANG(argP, int) =  Atoi(ReadBuff);
					f++;
					break;
				case 'c':
					ReadSerialKeyNum(ReadBuff);
					*TYPECHANG(argP, char) =  ReadBuff[0];
					f++;
					break;
				case 'x':
					ReadSerialKeyNum(ReadBuff);
					*TYPECHANG(argP, unsigned int ) =  Atox(ReadBuff);
					f++;
					break;		
				case 'f':
					ReadSerialKeyNum(ReadBuff);
					*TYPECHANG(argP, double) =  atol_my(ReadBuff);
					f++;
					break;						
			}
		}
		else 
			f++;
	}
		
	EnableIntMsk( );
	Delay(100);
	#ifdef DUG
	IntMskFlag =0;
	#endif
}

#else

void Printf(char * f,...)
{
	return;
}

void  Scanf(char *f, void  *argP)
{
	return;
}

#endif


