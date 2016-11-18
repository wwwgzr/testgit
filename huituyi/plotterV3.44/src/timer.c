#include "..\target\44b.h"
#include "..\inc\defconst.h"
#include "..\inc\extern.h"

void Timer0_Init(void);
void __irq Timer0_Int(void);
void ReadKeybd(void);
void MoveKeyToBuf(unsigned char KeyValue);

extern void DriveSprinkler(int iPulseNumX,int iPulseNumY);
extern void DealPrint(void);

//初始化定时器0
void Timer0_Init(void)
{
	pISR_TIMER0=(unsigned)Timer0_Int;
	rTCON=0x0;			  //all_stop
	rTCFG0=0x0f;		  //dead zone=0,pre0=0xf
	rTCFG1=0x04;		  //all interrupt,mux0=1/32
	rTCNTB0=1500;		  //(1/(64MHz/16/32))*1000=0.008s
	rTCMPB0=0;
	
	rTCON=0x02;//0x0a;				//update T0
	rTCON=0x01;//0x09;				//T0=one shot,all_start 
}

void __irq Timer0_Int(void)
{
	static unsigned int iTimerCount=0;			//定时中断次数计数器
	
	rTCON = 0;
	rI_ISPC = BIT_TIMER0;
	
	iTimerCount++;
	if (iTimerCount == 70)
		rPDATE |= 0x90;
	else if (iTimerCount == 100)
	{
		rPDATE &= 0x16f;
		iTimerCount = 0;
	}
	DealPrint();
	
	//20100220 静电问题
	if(gl_scankeyflag)
		ReadKeybd();
	
	//20101118
	if (gl_WatchDogState)
		gl_WatchDogCount++;
	/*
	if (gl_LockUSBState)
		gl_LockUSBTime++;
	*/
	rTCNTB0 = 1500;		  //(1/(64MHz/16/32))*1000=0.008s
	rTCMPB0 = 0;
	rTCON = 0x02;//0x0a;				//update T0
	rTCON = 0x01;
}
//20100220 静电问题
void ReadKeybd(void)
{
	static unsigned char BakKeyValue=0;		//键值
	static unsigned char cScanTimes=0;		//扫描次数
	static unsigned char cScanNo=0;				//当前扫描线编号
	static unsigned char cKeyScanNo=0;		//按键所在扫描线编号
	unsigned short sTemp;
	unsigned char keyvalue;
	unsigned short  scanvalue[3] ={0x60,0xa0,0xc0};
	
	//判断是否有键按下
	sTemp = KEYSCANADDR & 0x0ff;
	if( (sTemp != 0x0e) )
	{
		sTemp |= scanvalue[cScanNo];
		switch(sTemp)
		{
			case 0x66:
				keyvalue = '1';
				break;
			case 0x6a:
				keyvalue = '2';
				break;
			case 0x6c:
				keyvalue = '3';
				break;
			case 0xa6:
				keyvalue = '4';
				break;
			case 0xaa:
				keyvalue = '5';
				break;
			case 0xac:
				keyvalue = '6';
				break;
			case 0xc6:
				keyvalue = '7';
				break;
			case 0xca:
				keyvalue = '8';
				break;
			case 0xcc:
				keyvalue = '9';
				break;
			default:
				keyvalue = 0;
				break;
			
		}
	}
	else
		keyvalue = 0;
	
	if ((keyvalue == 0) && (cScanNo != cKeyScanNo))
	{
		cScanNo++;
		if (cScanNo > 2)
			cScanNo = 0;
		KEYSCANADDR = scanvalue[cScanNo];//送扫描线
		return;
	}
	
	if (keyvalue != BakKeyValue)
	{
		BakKeyValue = keyvalue;
		cScanTimes = 1;
		cKeyScanNo = cScanNo;
	}
	else
	{
		cScanTimes++;
		if (cScanTimes == SCANTIMES1)
		{
			MoveKeyToBuf(BakKeyValue);
		}
		else if (cScanTimes == SCANTIMES2)
		{
			if (keyvalue != 0)
				MoveKeyToBuf(BakKeyValue);
			cScanTimes = SCANTIMES1;
		}
	}
	
	cScanNo++;
	if (cScanNo >2)
		cScanNo = 0;
	KEYSCANADDR = scanvalue[cScanNo];//送扫描线
}


/*
const char Keyboard[4][4] = {{'7','8','9',0},
	                          	{'4','5','6',0},
	                          	{'1','2','3',0},
	                          	{0,0,0,0}};
//按键读取
void ReadKeybd(void)
{
	static unsigned char BakKeyValue=0;		//键值
	static unsigned char cScanTimes=0;		//扫描次数
	static unsigned char cScanNo=0;				//当前扫描线编号
	static unsigned char cKeyScanNo=0;		//按键所在扫描线编号
	unsigned short sTemp;
	unsigned char keyvalue;
	unsigned short  scanvalue[4] ={0x1bf,0x1df,0x1fd,0x1fe};
	
	//判断是否有键按下
	sTemp = rPDATF & 0x98;
	if (sTemp != 0x98)
	{
		if(sTemp == 0x90)
			keyvalue = Keyboard[1][cScanNo];
		else if(sTemp == 0x88)
			keyvalue = Keyboard[2][cScanNo];
		else if(sTemp == 0x18)
			keyvalue = Keyboard[3][cScanNo];
		else
			keyvalue = 0;
	}
	else if((rPDATG&0x01) != 0x01)
		keyvalue = Keyboard[0][cScanNo];
	else
		keyvalue = 0;
	
	if ((keyvalue == 0) && (cScanNo != cKeyScanNo))
	{
		cScanNo++;
		if (cScanNo > 3)
			cScanNo = 0;
		rPDATF = scanvalue[cScanNo];//送扫描线
		return;
	}
	if (keyvalue != BakKeyValue)
	{
		BakKeyValue = keyvalue;
		cScanTimes = 1;
		cKeyScanNo = cScanNo;
	}
	else
	{
		cScanTimes++;
		if (cScanTimes == SCANTIMES1)
				MoveKeyToBuf(BakKeyValue);
		else if (cScanTimes == SCANTIMES2)
		{
			if (keyvalue != 0)
				MoveKeyToBuf(BakKeyValue);
			cScanTimes = SCANTIMES1;
		}
	}
	cScanNo++;
	if (cScanNo > 3)
		cScanNo = 0;
	rPDATF = scanvalue[cScanNo];//送扫描线
}
*/
void MoveKeyToBuf(unsigned char KeyValue)
{
	unsigned char cTemp;
	
	gl_cKeyBuf[gl_cKeyPH] = KeyValue;
	cTemp = gl_cKeyPH + 1;
	if (cTemp == KeyBufLength)
		cTemp = 0;
	if (cTemp != gl_cKeyPE)
		gl_cKeyPH = cTemp;
}

