#include <string.h>
#include <stdio.h>

#include "..\Target\44b.h"
#include "..\Target\44blib.h"
#include "..\inc\extern.h"

void Isr_Init(void);
void SystemClockInit(void);
void HaltUndef(void);
void HaltSwi(void);
void HaltPabort(void);
void HaltDabort(void);
void Delay(int time);
void SoftDelay(int us);
void Port_Init(void);
void ReceiveData(void);

extern void Uart0_Init(int baud);
extern void Timer0_Init(void);
extern void Init8900(void);
extern void	InitCH375(void);
extern void lcd_init(void);
extern void lcd_clr(void);
extern void lcd_refresh(void);
extern void lcd_display(unsigned char row,const char *string);
extern void PollRecNetData(void);
extern void	PollRecUsbData(void);
extern void KBProc(void);
extern void DrivePaper(void);
extern short RecLineData(void);
extern void DealUartData(void);
extern void ReadParameter(void);
extern void InvertPrintData(void);
extern int InitPlotter(void);
extern void RestCH375(void);
extern void WaitKeyExit(unsigned char cKeyValue);
extern void FlashChipSelect(void); // 20111103
extern void SetDeviceType(void);	// 20111103
/*******************main function*************************/
void Main(void)
{
  int iFlag=0;
	SystemClockInit();
	
	rSYSCFG = CACHECFG;
  Port_Init();
  Isr_Init();
  Delay(0);
  Delay(20000);
  Uart0_Init(115200);	
  Timer0_Init();
  ReadParameter();
  lcd_init();

CleanCommand(); //打印清洗结束
  iFlag = InitPlotter();
CleanCommand(); //打印清洗结束
  
  Init8900();
  InitCH375();
  Printf("Initializtion succeed!\n");
   gl_scankeyflag =1;//20100220
   gl_iEthnetRecBufPH = 0;
	gl_iEthnetRecBufPE = 0;
	gl_cKeyPH = 0;
	gl_cKeyPE = 0;
	gl_cUartRecBufPH = 0;
	gl_cUartSendBufPH = 0;
	gl_cUartSendBufPE = 0;
	gl_cCanRevChar = 1;
	gl_cPrintStatus = 0;
	gl_cRecFrameOver = 0;
	gl_cInvertBufStatus = 0;
	gl_cCurPrintBuf = 0;
	gl_cFileReceive = 0;
	gl_cReacterStatus = 0;
	gl_cSelfCheckFlag = 0;
	gl_cRemainderBit = 16;
	gl_cCountFlag = 0;
	gl_iMovePaperLength = 0;
	gl_cDelayFlag = 0x80;
	gl_iDelayCount = 0;
	gl_iWaitCount = 0;
	gl_iWaitCount1 = 0;
	gl_cCommunicationMode = 0xff;
	gl_WatchDogState = 0;//20101118
	gl_WatchDogCount = 0;

	rI_ISPC = 0x1ffffff;
	rINTMSK = ~(BIT_GLOBAL|BIT_URXD0|BIT_UERR01|BIT_TIMER0);//|BIT_UTXD0|BIT_EINT1

	if(iFlag==1)
	{
		FlashChipSelect();
		SetDeviceType();

		#if(LANGUAGE==0)
		strcpy(gl_cLcdDispBuf[0],"设置成功");
		strcpy(gl_cLcdDispBuf[1],"请重启绘图仪");
		#elif(LANGUAGE==1)
		strcpy(gl_cLcdDispBuf[0],"Set success");
		strcpy(gl_cLcdDispBuf[1],"Restart machine");
		#elif(LANGUAGE==2)
		strcpy(gl_cLcdDispBuf[0],"Configurar OK!");
		strcpy(gl_cLcdDispBuf[1],"Reiniciar");
		#elif(LANGUAGE==3)
		strcpy(gl_cLcdDispBuf[0],"Ayar Basarili");
		strcpy(gl_cLcdDispBuf[1],"Bastan Baslatin");
		#else
		strcpy(gl_cLcdDispBuf[0],"Sucesso!");
		strcpy(gl_cLcdDispBuf[1],"ReinicarPlotter");
		#endif
		lcd_refresh();
		while(1);
	}
	CleanCommand(); //打印清洗结束
	#if(LANGUAGE==0)
	strcpy(gl_cLcdDispBuf[0],"  欢迎使用");
	strcpy(gl_cLcdDispBuf[1],"富怡喷墨绘图仪");
	#elif(LANGUAGE==1)
	strcpy(gl_cLcdDispBuf[0],"Welcome to use");
	strcpy(gl_cLcdDispBuf[1],"PloterRichpeace");
	#elif(LANGUAGE==2)
	strcpy(gl_cLcdDispBuf[0],"Bienvenidos al");
	//strcpy(gl_cLcdDispBuf[1],"Ploter Stamp&go");
	strcpy(gl_cLcdDispBuf[1],"Ploter Richpeace");
	#elif(LANGUAGE==3)
	strcpy(gl_cLcdDispBuf[0],"Hosgeldiniz");
	strcpy(gl_cLcdDispBuf[1],"Plotter RYOU");
	#else// 葡萄牙
	strcpy(gl_cLcdDispBuf[0],"Bem vindo ao");
	strcpy(gl_cLcdDispBuf[1],"PlotterPlotag");
	#endif
	lcd_refresh();
	

  while(1)
  {
  	KBProc();								//按键处理
	ReceiveData();
	InvertPrintData();			//转换打印数据
  }
}

void ReceiveData(void)
{
	switch (gl_cCommunicationMode)
	{
		case 0:
			PollRecNetData();				//接收数据
			break;
		case 1:
			PollRecUsbData();				//接收USB数据
			break;
		default:
			PollRecNetData();
			PollRecUsbData();
			break;
	}
}

void Isr_Init(void)
{
	pISR_UNDEF = (unsigned)HaltUndef;
	pISR_SWI = (unsigned)HaltSwi;
	pISR_PABORT = (unsigned)HaltPabort;
	pISR_DABORT = (unsigned)HaltDabort;

	rINTCON = 0x5;						// Non-vectored,IRQ enable,FIQ disable    
	rINTMOD = 0x0;						// All=IRQ mode
	rINTMSK = BIT_GLOBAL;			// All interrupt is masked.
}

void SystemClockInit(void)
{
	rPLLCON = (56 << 12) | (4 << 4) | 1;
}

void HaltUndef(void)
{
	lcd_clr();
	#if(LANGUAGE==0)
	lcd_display(0,"错误:001");
	#elif(LANGUAGE==1)
	lcd_display(0,"Error:001");
	#else
	lcd_display(0,"Error:001");
	#endif
	while(1);
}

void HaltSwi(void)
{
	lcd_clr();
	#if(LANGUAGE==0)
	lcd_display(0,"错误:002");
	#elif(LANGUAGE==1)
	lcd_display(0,"Error:002");
	#else
	lcd_display(0,"Error:002");
	#endif
	while(1);
}

void HaltPabort(void)
{
	lcd_clr();
	#if(LANGUAGE==0)
	lcd_display(0,"错误:003");
	#elif(LANGUAGE==1)
	lcd_display(0,"Error:003");
	#else
	lcd_display(0,"Error:003");
	#endif
	while(1);
}

void HaltDabort(void)
{
	lcd_clr();
	#if(LANGUAGE==0)
	lcd_display(0,"错误:004");
	#elif(LANGUAGE==1)
	lcd_display(0,"Error:004");
	#else
	lcd_display(0,"Error:004");
	#endif
	while(1);
}

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
	/*	BIT	9		8	  7		6		5		4		3		2		1		0	*/
	/*			A24	A23	A22	A21	A20	A19	A18	A17	A16	A0	*/	      
	/*			1		1		1		1		1		1		1		1		1		1	*/
	rPCONA = 0x3ff;	

	// PORT B GROUP
	/*	BIT		10  	9			8			7			6		 	5				4		  	3		 	2			1		 	0		*/
	/*			/CS5	/CS4	/CS3	/CS2	/CS1	/nWBE3	/nWBE2	/SRAS	/SCAS	/SCLK	/SCKE	*/
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
	/*	  01		00				01			00				00		00		00		00	*/
	rPCOND= 0x4400;	
	rPUPD = 0xaf;
	rPDATD= 0xff;
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
	/*	NC		NETINT	NC	NC	NC	NC	USB	KEYIN3	*/
	/*  00    00      00  00  00  00  11  00       */
	//rPDATG = 0xff;
	//rPCONG = 0xffff;
	rPCONG = 0x0C; //EXINT1 enabled
	rPUPG  = 0x02;	//pull-up regiseter should be enabled  
	
	rSPUCR=0x07;  //D15-D0 pull-up disable

	/*定义非Cache区*/
	rNCACHBE0=((Non_Cache_End>>12)<<16)|(Non_Cache_Start>>12); 
	/*均为低电平触发,Exint1设置为下降沿触发20090903*/
	rEXTINT=0x0|(1<<5);
}


