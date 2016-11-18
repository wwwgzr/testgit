
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "..\target\44b.h"
#include "..\target\44blib.h"
#include "..\target\def.h"
#include "..\target\option.h"

#define STACKSIZE    0xa00 //SVC satck size(do not use user stack)//
#define HEAPEND     (_ISR_STARTADDRESS-STACKSIZE-0x500) // = 0xc7ff000//
//SVC Stack Area:0xc(e)7ff000-0xc(e)7ffaff//

extern char Image$$RW$$Limit[];
volatile unsigned char *downPt;
unsigned int fileSize;
void *mallocPt=Image$$RW$$Limit;

//void (*restart)(void)=(void (*)(void))0x0;
//void (*run)(void)=(void (*)(void))DOWNLOAD_ADDRESS;
//--------------------------------SYSTEM---------------------------------//
static int delayLoopCount=400;

void Delay(int time)
// time=0: adjust the Delay function by WatchDog timer.//
// time>0: the number of loop time//
// 100us resolution.//
{
	int i,adjust=0;
	if(time==0)
	{
		time=200;
		adjust=1;
		delayLoopCount=400;
		rWTCON=((MCLK/1000000-1)<<8)|(2<<3);	// 1M/64,Watch-dog,nRESET,interrupt disable//
		rWTDAT=0xffff;
		rWTCNT=0xffff;	 
		rWTCON=((MCLK/1000000-1)<<8)|(2<<3)|(1<<5); // 1M/64,Watch-dog enable,nRESET,interrupt disable //
	}
	for(;time>0;time--)
		for(i=0;i<delayLoopCount;i++);
	if(adjust==1)
	{
		rWTCON=((MCLK/1000000-1)<<8)|(2<<3);
		i=0xffff-rWTCNT;   //  1count/16us?????????//
		delayLoopCount=8000000/(i*64);	//400*100/(i*64/200)   //
	}
}

void SoftDelay(int us)//64M, delay about 1us
{
	int i,j;
	for(i=0;i<us;i++)
		for(j=0;j<64;j++);
}
//------------------------PORTS------------------------------//
void Port_Init(void)
{
	//CAUTION:Follow the configuration order for setting the ports. 
	// 1) setting value 
	 // 2) setting control register 
	 // 3) configure pull-up resistor.  

	//16bit data bus configuration  

	// PORT A GROUP
	/*  BIT    9	8	  7		6		5		4		3		2		1		0	*/
	/*        A24	A23	A22	A21	A20	A19	A18	A17	A16	A0	*/	      
	/*        1		1		1		1		1		1		1		1		1		1	*/
	rPCONA = 0x3ff;	

	// PORT B GROUP
	/*  BIT   10  	9			8			7			6		 	5				4		  	3		 	2			1		 	0		*/
	/*	    /CS5	/CS4	/CS3	/CS2	/CS1	/nWBE3	/nWBE2	/SRAS	/SCAS	/SCLK	/SCKE	*/
	/*	   CPLD CS/FPGACS/NC	/NETAEN/NC	/NC			/NC			/nSRAS/nSCAS/nSCLK/nSCKE*/
	/*      1,     1,   	1,   1,    1,    1,       1,     	1,    1,    1,   1          */
	rPDATB = 0x7ff;
//	rPCONB = 0x68f;
    
	//PORT C GROUP
	//BUSWIDTH=16													*/
	/*  PC15	14		13				12				11					10		9			8	*/
	/*	I			I			I					I					I						I			I			I	*/
	/* NIC		NIC		CNCONFIG	CNSTATUS	CNCONFDONE	CDATA	CDCLK	NC*/
	/*   00		00		00				00				00					00		00		00	*/

	/*  PC7		6		5		4				3				2			1				0	*/
	/*   I		I		I		O				O				I			I				I	*/
	/*   NC		NC	NC	DH16L16	DINCLK 	WRREG ClrFIFO FIFOFULL*/
	/*   00		00	00	01			01			00		01			00	*/

	rPCONC = 0x144;	
	rPUPC  = 0xfffe;	//PULL UP RESISTOR should be enabled to I/O
	rPDATC = 0xffe5;

	//PORT D GROUP
	/*  PORT D GROUP(I/O OR LCD)										*/
	/*  BIT	7		6					5				4					3			2			1			0	*/
	/*  ROLALM	ROLINPOS	CARALM	CARINPOS	NC		NC		NC		NC	*/
	/*	  00		00				00			00				00		00		00		00	*/
//	rPDATD= 0xff;
	rPCOND= 0x00;	
	rPUPD = 0xff;
	//These pins must be set only after CPU's internal LCD controller is enable
	
	//PORT E GROUP 
	/*  Bit 8				7		6		5		4		3		2			1		 	0	*/
	/*      ENDIAN	LED	NC	NC	LED	NC	RXD0	TXD0	PE0	*/ 
	/*      01			01	00	00	01	00	10		10		10	*/
	rPDATE	= 0x1ff;
	rPCONE	= 0x412a;	
	rPUPE	= 0x6;
	
	//PORT F GROUP
	/*  Bit8	7				6				5		 		4				3				2			1				0		*/   
	/*  NC		KEYIN4	KEYOUT1	KEYOUT2	KEYIN1	KEYIN2	nWAIT	KEYOUT3	KEYOUT4	*/
	/*	000		000			001			001			00			00			00		01			01		*/
	rPDATF = 0x1ff;
	rPCONF = 0x2405;
	rPUPF  = 0x00;	//势能上拉电阻

	//PORT G GROUP
	/*  BIT7	6				5		4		3		2		1		0	 */
	/*	NC		NETINT	NC	NC	NC	NC	NC	KEYIN3	*/
	/*  00    11      00  00  00  00  00  00       */
	//rPDATG = 0xff;
	//rPCONG = 0xffff;
	rPCONG = 0x3000; //EXINT0~3 enabled
	rPUPG  = 0x0;	//pull-up regiseter should be enabled  
	
	rSPUCR=0x07;  //D15-D0 pull-up disable

	/*定义非Cache区*/
	rNCACHBE0=((Non_Cache_End>>12)<<16)|(Non_Cache_Start>>12); 
	/*均为低电平触发,Exint3设置为上升沿触发----modify at 04-12-11*/
	rEXTINT=0x0|(1<<14);

//Misc
//USBN9603's A0 pin	->	CPU's A1
//IDE's CS0			->	CPU's A2
//IDE's CS1			->	CPU's A1
//IDE's A2			->	CPU's A3
//IDE's A1			->	CPU's A5
//IDE's A0			->	CPU's A4
}

/************************* General Library **********************/

void * _malloc_(unsigned nbyte) 
/*Very simple; Use _malloc_() & _free_() like Stack*/
//void *mallocPt=Image$$RW$$Limit;
{
    void *returnPt=mallocPt;

    mallocPt= (int *)mallocPt+nbyte/4+((nbyte%4)>0); //to align 4byte

    if( (int)mallocPt > HEAPEND )
    {
		mallocPt=returnPt;
		return NULL;
    }
    return returnPt;
}


void _free_(void *pt)
{
    mallocPt=pt;
}


void Cache_Flush(void)
{
    int i,saveSyscfg;
    
    saveSyscfg=rSYSCFG;

    rSYSCFG=SYSCFG_0KB; 		      
    for(i=0x10004000;i<0x10004800;i+=16)    
    {					   
		*((int *)i)=0x0;		   
    }
    rSYSCFG=saveSyscfg; 			    
}

void _disable_(void)
{
    rI_ISPC=BIT_EINT0;
    rINTMSK|=(BIT_EINT0|BIT_GLOBAL);//0x01<<26;
		
	//rINTCON=0x2|rINTCON;
	
}

void _enable_(void)
{
    
    rI_ISPC=BIT_EINT0;
	rINTMSK&=(~(BIT_EINT0|BIT_GLOBAL));
	//rINTCON=0xfffffff9&rINTCON;
}

int _atoi_(char *str)
{
    int n=0;
    char c=1;

    while((c=*str++)&&(c>=0x30)&&(c<=0x39))
            n = n*10 + c-'0';
    return(n);
}
		
