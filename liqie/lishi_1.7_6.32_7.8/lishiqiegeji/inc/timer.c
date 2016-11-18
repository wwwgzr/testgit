//***************TIME.C**************************
//��ʱ����ʼ��
//����ɨ��
//***********************************************

#include "..\target\44b.h"
#include "..\src\defconst.h"
#include "..\src\extern.h"

void Timer0_Init(void);
void __irq Timer0_Int(void);
void ReadKeybd(void);
void MoveKeyToBuf(unsigned char KeyValue);

extern void DriveSprinkler(int iPulseNumX,int iPulseNumY);
extern void DealPrint(void);
extern void Printf(char * f,...);

//��ʼ����ʱ��0
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
/*
void deal_cutpress()
{
	//��ȡ��ǰ��312�ľ���λ��
	
	//�Աȱ��е�Ԫ�ؿ���ǰ���ĸ����� 
	
	//�ĸ�����Ϊ�����±�
	
	//��λ �ȽϹ��ı�־λ
	
	//�ж�����������ǽ����� ������ѹ
	
	//
}
*/

void __irq Timer0_Int(void)
{
	
	static unsigned int iTimerCount=0;			//��ʱ�жϴ���������
	
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
	//
	//deal_cutpress();
	//20100220 ��������
	if(gl_scankeyflag)
		ReadKeybd();
	
	rTCNTB0 = 1500;		  //(1/(64MHz/16/32))*1000=0.008s
	rTCMPB0 = 0;
	rTCON = 0x02;//0x0a;				//update T0
	rTCON = 0x01;
	Printf("Print end icount iTotalX %d\n",iTimerCount);
}
//20100220 ��������
void ReadKeybd(void)
{
	static unsigned char BakKeyValue=0;		//��ֵ
	static unsigned char cScanTimes=0;		//ɨ�����
	static unsigned char cScanNo=0;				//��ǰɨ���߱��
	static unsigned char cKeyScanNo=0;		//��������ɨ���߱��
	//static unsigned char cKeyPressState=0;	// �м����±�־
	unsigned short sTemp;
	unsigned char keyvalue;
	unsigned char  scanvalue[5] = {0x70,0xb0,0xd0,0xe0,0x70};//{0x60,0xa0,0xc0};
	
	//�ж��Ƿ��м�����
	sTemp = KEYSCANADDR & 0x0ff;
	if( (sTemp != 0x0f) )
	{
		sTemp |= scanvalue[cScanNo];
		
		switch(sTemp)
		{
			case 0xbb:				//		  Key_pupm 
				keyvalue = '1';		//Key_Left 0xbb:
				break;
				
			case 0xbd: 				//0xde:   Key_F2 
				keyvalue = '2';		//Key_Up  0xbd:
				break;	
				
			case 0xbe: 				//0xdd:   Key_F1
				keyvalue = '3';		//Key_Down  0xbe: 
				break;
				
			case 0xdb: 				//0xee:	Key_Pause
				keyvalue = '4';		//0  0xdb:
				break;

			case 0xdd: 				//0xd7:	Key_OK  
				keyvalue = '5';		//Key_Pause	 0xdd:	
				break;
				
			case 0xde: 				//0xbb:	Key_F3 
				keyvalue = '6';		//Key_F6  
				break;

			case 0xeb: 				//0xed:    Key_TEST
				keyvalue = '7';		//Key_Menu  0xeb:
				break;
				
			case 0xed: 				//0xbe:	Key_F4 
				keyvalue = '8';		//Key_F4  0xed:
				break;	
			
			case 0xee: 				//0xb7: Key_Drop 
				keyvalue = '9';		//Key_F5  0xee:
				break;
	
			default:
				keyvalue = 0;
				break;
		}
		//Printf("keyvalue[%c]\n ", keyvalue);
		//Printf("KeyValue[0x%x],%d,0x%x", sTemp, cScanNo, scanvalue[cScanNo]);
		//cKeyPressState = 1;
	}
	else
		keyvalue = 0;

	if ((keyvalue == 0) && (cScanNo != cKeyScanNo))
	{
		cScanNo++;
		if (cScanNo > 3)
			cScanNo = 0;
		KEYSCANADDR = scanvalue[cScanNo];//��ɨ����
		//Printf("Up %d\n", cScanNo);
		//�����ɿ�
		//if(cKeyPressState==1)
		//{
		//	cKeyPressState = 0;
		//	MoveKeyToBuf(0);
		//	Printf("Up\n");
		//}
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
	if (cScanNo > 3)
		cScanNo = 0;
	KEYSCANADDR = scanvalue[cScanNo];//��ɨ����
}

void MoveKeyToBuf(unsigned char KeyValue)
{
	unsigned char cTemp;
	//Printf("MoveKeyToBuf:%c\n",KeyValue);
	gl_cKeyBuf[gl_cKeyPH] = KeyValue;
	cTemp = gl_cKeyPH + 1;
	if (cTemp == KeyBufLength)
		cTemp = 0;
	if (cTemp != gl_cKeyPE)
		gl_cKeyPH = cTemp;
}

