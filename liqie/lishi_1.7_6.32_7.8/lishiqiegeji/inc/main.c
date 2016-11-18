#include <string.h>
#include <stdio.h>

#include "..\Target\44b.h"
#include "..\Target\44blib.h"
#include "..\src\extern.h"

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
void WorkStatus(void);



extern void Uart0_Init(int baud);
extern void Timer0_Init(void);
extern void Init8900(void);
extern void InitCH375(void);
extern void lcd_init(void);
extern void lcd_clr(void);
extern void lcd_refresh(void);
extern void lcd_display(unsigned char row,const char *string);
extern void LcdDisp(char * s1, char * s2);
extern void PollRecNetData(void);
extern void PollRecUsbData(void);
extern void KBProc(void);
extern void DrivePaper(void);
extern short RecLineData(void);
extern void DealUartData(void);
extern void ReadParameter(void);
extern void InvertPrintData(void);
extern void Init_MCX312(void);//20100622
extern void InitFlatPlotter(void);
extern void testMcx312(void);
extern int InitPaper(void);
extern void DriveSprinkler(int iPulseNumX,int iPulseNumY);

extern long gl_TestSendLengPluse;
/*
void testRWFifo(void)
{
	unsigned int address, ReadData, WriteData;

while(1)
{
	address = WriteFIFOAddr;
	WriteData = 0x3ffffff;
	WriteFIFO(address,WriteData);
	Delay(100);
	ReadData= ReadFIFO(address);
	if(WriteData != ReadData)
	{
		Printf("1,Write[0x%x],Read[0x%x]", WriteData, ReadData);
		break;
	}
	
	address = WriteFIFOAddr+28;
	WriteData = 0x3ffffff;
	WriteFIFO(address,WriteData);
	Delay(100);
	ReadData= ReadFIFO(address);
	if(WriteData != ReadData)
	{
		Printf("2, Write[0x%x],Read[0x%x]", WriteData, ReadData);
		break;
	}
}	
	
}
*/
/*
extern void WaitKeyExit(unsigned char cKeyValue);
#define SPD    id )100000
void testmovpps(vo
{
	int InductorStatus, mem=1;
	char cReadKey;
	int SendPulse=50000, toalPulseY = 0, toalPulseX = 0;
	char dispbuf[20];
	int abc;
	//��ʼ���˶�����оƬMCX312
	Init_MCX312();
	SetMovFrameSpeed(200);
	LcdDisp("Mov test", "updown");
	while(1)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		
		cReadKey = gl_cKeyBuf[gl_cKeyPE];	
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;

		switch (cReadKey)
		{
			case Key_F1:
				SendPulse += 1000;
				sprintf(dispbuf,"%d", SendPulse);
				LcdDisp("Set Send pulse", dispbuf);
				break;
				
			case Key_Up://Y�ƶ�
				toalPulseY += SendPulse;
				sprintf(dispbuf,"%d", toalPulseY);
				LcdDisp("Y UP ", dispbuf);
				pmove(AXIS_Y,SendPulse);
				mem=1;
				while(mem)
				{
					get_status(AXIS_Y, &InductorStatus);
					if(InductorStatus == 0)
					mem=0;;
				}
				Printf("Y Up end\n");
				break;
				
			case Key_Down:
				toalPulseY -= SendPulse;
				sprintf(dispbuf,"%d", toalPulseY);
				LcdDisp("Y Down ", dispbuf);
				abc = (-1 * SendPulse);
				Printf("Down[%d]\n", abc);
				pmove(AXIS_Y,abc);
				mem=1;
				while(mem)
				{
					get_status(AXIS_Y, &InductorStatus);
					if(InductorStatus == 0)
					mem=0;;
				}
				Printf("Y Down end\n");
				break;
				
			case Key_Left:
				toalPulseX += SendPulse;
				sprintf(dispbuf,"%d", SendPulse);
				LcdDisp("X  Left", dispbuf);
				abc = (-1 * SendPulse);
				Printf("Left[%d]\n", abc);
				pmove(AXIS_X,abc);
				mem=1;
				while(mem)
				{
					get_status(AXIS_X, &InductorStatus);
					if(InductorStatus == 0)
					mem=0;;
				}
				Printf("X Left end\n");
				break;
				
			case Key_Right:
				toalPulseX -= SendPulse;
				sprintf(dispbuf,"%d", SendPulse);
				LcdDisp("X Right", dispbuf);
				pmove(AXIS_X,SendPulse);
				mem=1;
				while(mem)
				{
					get_status(AXIS_X, &InductorStatus);
					if(InductorStatus == 0)
					mem=0;;
				}
				Printf("X Right end\n");
				break;
				
				
		}
		
	}
	
}
*/
extern void MC_CommonMenuDispInit(void);
extern void ADM_PopMenuInit(void);
extern void ADM_ViewErrMenuInit(void);		//20141202 rwt

void GlobalPara_Init()
{
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
	gl_MacRunState = 0;//20100623
	gl_cCutFileSendFlag = 0;
	gl_iGapPixel = 0;
//	gl_dLUnit = 0.025;                  //��ͼ���Ե��߼���λ(ע�ⲻ�ǻ�ͼ�ǵĵ�λ),��λ:����
	//gl_dXProp = 1.0;                    //X�����������
	//gl_dYProp = 1.0;                    //Y�����������
	gl_DeleteFileState = 0;
	gl_cPrintWorkStatus = 0;
	gl_cMachineWorkStatus = 0; 			//�豸����״̬  20120827
	gl_cKeyStart = 0;
	gl_iATime = 700;
	gl_iInitV0 = 5;
	gl_cManualPumpSwitch = 0xff;		// 0:���� 1:�ر�

		//Z���ٶȶ���
	gl_iZInitSpeed = 8000;
	gl_lZDriveSpeed = 50000;					// 20120808
	gl_lZAddSpeed  = 50000*2;

	gl_cKeyDirectPost = 0;

	gl_cStopWorkState = 0;

	 gl_cStartWorkFlag = 0;		// 2014141021 �ļ���ʼ������־
  	gl_lSWFCount = 0;		// 2014141021 �ļ�����������ʱ��
	 gl_cSWFStartClearFlag = 1;	// 2014141021 �´δ�ӡ�ļ�ʱ������ӡͷ��ϴ

	// TEST Debug Speed
	//gl_iSVSpeed = 40;
	//gl_iVSpeed = 200;
	//gl_iTimeAdd = 200;

	gl_cCutPenNum = 4;
	gl_lWorkOrig_YYPluse = PenCarSideInterval * DotPulseNum;	//20130831
	gl_lWorkOrig_XXPluse = 0;
	gl_cSendPaperStar = 1;
	
	InitSpeedCurve();// �ʵ������ļ�ʱ�ٶ�����

	cReviceFlag = 0;
	lXlimitNum = 0;

	MC_CommonMenuDispInit();
	ADM_PopMenuInit();
	ADM_ViewErrMenuInit();					//20141202 rwt
}

extern void SetIOBit(unsigned char cBit,unsigned char cEnable);
extern void WaitKeyExit(unsigned char cKeyValue);
extern void LineReverse(unsigned char line);

//������ֽ���
void ITestRollPaper(void)
{
	char state, cReadKey, cDir;
	if (gl_cLanguageSelect == 0)
	{
		LcdDisp("��ֽ�����ͣ����", "��ֽ����");
	}
	else
	{
		LcdDisp("Roll Motor Test", "Back Direction");
	}
	state = 1;
	cDir = 0;
	SetIOBit(O_ROLLMOTORDIR,R_BACKPAPER);
	while(state)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		switch (cReadKey)
		{	
			case Key_F1:
				//������ֽ���
				SetIOBit(O_STARTROLLPMOTOR,0);
				break;

			case Key_F2:
				//ֹͣ��ֽ���
				if (gl_cLanguageSelect == 0)
				{
					LcdDisp("��ֽ�����ͣ����", "ͣ��");
				}
				else
				{
					LcdDisp("Roll Motor Test", "Stop");
				}
				SetIOBit(O_STARTROLLPMOTOR,1);
				break;

			case Key_F3:
				//������ֽ����
				if(cDir == 1)
				{
					if (gl_cLanguageSelect == 0)
					{
						LcdDisp("��ֽ�����ͣ����", "��ֽ����");
					}
					else
					{
						LcdDisp("Roll Motor Test", "Roll Direction");
					}
					SetIOBit(O_ROLLMOTORDIR,R_RECEIVEPAPER);
					cDir = 0;
				}
				else
				{
					if (gl_cLanguageSelect == 0)
					{
						LcdDisp("��ֽ�����ͣ����", "��ֽ����");
					}
					else
					{
						LcdDisp("Roll Motor Test", "Back Direction");
					}
					SetIOBit(O_ROLLMOTORDIR,R_BACKPAPER);
					cDir = 1;
				}
				
				break;
			case Key_Ok:
				//ͣ��
				SetIOBit(O_STARTGIVEPMOTOR,1);
				state=0;
				break;
		}
	}
}

//���Ե����
void ITestElectromagnet(void)
{
	char state, cReadKey;
	if (gl_cLanguageSelect == 0) {
		LcdDisp("���������", "  ");
	}
	else {
		LcdDisp("ElectromagntTst", "  ");
	}
	//LcdDisp("���������", "  ");  //20141201 rwt
	state = 1;
	while(state)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		switch (cReadKey)
		{	
			case Key_F1:
				if (gl_cLanguageSelect == 0) {
					LcdDisp("���������", "����");
				}
				else {
					LcdDisp("ElectromagntTst", "Attract");
				}
				//LcdDisp("���������", "����");		//20141201 rwt
				SetIOBit(O_CUTDOWN,C_CUTDOWN);
				break;

			case Key_F2:
				if (gl_cLanguageSelect == 0) {
					LcdDisp("���������", "�ɿ�");
				}
				else {
					LcdDisp("ElectromagntTst", "Loosen");
				}
				//LcdDisp("���������", "�ɿ�");		//20141201 rwt
				SetIOBit(O_CUTDOWN,C_CUTUP);
				break;
			
			case Key_Ok:
				state=0;
				break;
		}
	}


}


//������ֽ���
void ITestFeedingPaper(void)
{
	char state, cReadKey, cDir;
	if (gl_cLanguageSelect == 0)
	{
		LcdDisp("��ֽ�����ͣ����", "��ֽ����");
	}
	else
	{
		LcdDisp("Send Motor test", "Roll Direction");
	}
	state = 1;
	cDir = 0;
	SetIOBit(O_GIVEMOTORDIR,G_ROLLPAPER);
	while(state)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		switch (cReadKey)
		{	
			case Key_F1:
				//������ֽ���
				if (gl_cLanguageSelect == 0)
				{
				LcdDisp("��ֽ�����ͣ����", "����");
				}
				else
				{
					LcdDisp("Send Motor Test", "Start");
				}
				SetIOBit(O_STARTGIVEPMOTOR,0);
				break;

			case Key_F2:
				//ֹͣ��ֽ���
				if (gl_cLanguageSelect == 0)
				{
					LcdDisp("��ֽ�����ͣ����", "ͣ��");
				}
				else
				{
					LcdDisp("Send Motor Test", "Stop");
				}
				SetIOBit(O_STARTGIVEPMOTOR,1);
				break;

			case Key_F3:
				//������ֽ����
				if(cDir == 1)
				{
					if (gl_cLanguageSelect == 0)
					{
						LcdDisp("��ֽ�����ͣ����", "��ֽ����");
					}
					else
					{
						LcdDisp("Send Motor Test", "Send Direction");
					}
					SetIOBit(O_GIVEMOTORDIR,G_SENDPAPER);
					cDir = 0;
				}
				else
				{
					if (gl_cLanguageSelect == 0)
					{
						LcdDisp("��ֽ�����ͣ����", "��ֽ����");
					}
					else
					{
						LcdDisp("Send Motor Test", "Roll Direction");
					}
					SetIOBit(O_GIVEMOTORDIR,G_ROLLPAPER);
					cDir = 1;
				}
				
				break;
			case Key_Ok:
				//ֹͣ��ֽ���
				SetIOBit(O_STARTGIVEPMOTOR,1);
				state=0;
				break;
		}
	}
}

//��������ֽ��Ӧ��
void ITestPaperSensor(void)
{
	char cReadKey;

	if (gl_cLanguageSelect == 0) {
		LcdDisp("��Ӧ������", "  ");
	}
	else {
		LcdDisp("Sensor Test", "  ");
	}
	while(1)
	{
		if (GetIOBit(I_ROLLPEND) == StatusOk) {
			if (gl_cLanguageSelect == 0) {
				LcdDisp("��ֽͣ��Ӧ��", "���Գɹ�");
			}
			else {
				LcdDisp("RollStopSensor", "Test Succeed");
			}
		}

		if (GetIOBit(I_GIVEBACK) == StatusOk) {
			if (gl_cLanguageSelect == 0) {
				LcdDisp("��ֽͣ��Ӧ��", "���Գɹ�");
			}
			else {
				LcdDisp("SendStopSensor", "Test Succeed");
			}
		}

		if (GetIOBit(I_GIVEPEND) == StatusOk) {
			if (gl_cLanguageSelect == 0) {
				LcdDisp("��ֽ������Ӧ��", "���Գɹ�");
			}
			else {
				LcdDisp("SendStartSensor", "Test Succeed");
			}
		}

		if (GetIOBit(I_YORIGIN) == StatusOk) {
			if (gl_cLanguageSelect == 0) {
				LcdDisp("ԭ���Ӧ��", "���Գɹ�");
			}
			else {
			 	LcdDisp("Origin Sensor", "Test Succeed");
			}
		}
		
		if(gl_cKeyPH == gl_cKeyPE)
			continue;

		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;

		if(cReadKey == Key_Pause)
			break;
		
	}

}

//  ����XY�ƶ�
//�ƶ���ͷ��ָ��λ��
void ITMoveHandpiece(char cAix, int iMovePluse)
{
	int iStatus;

	pmove(cAix, iMovePluse);
	//�ȴ��ƶ�����
	do
	{   ///   ??  7-14 714 7.14
		{
			//��ֽ���
	/*if ((gl_cReacterStatus & B_ROLLPAPER) != 0)
	{//�ѿ�ʼ��ֽ
		*/	
		if ((GetIOBit(I_ROLLPEND) == StatusOk))
		{//��ֽͣλ�õ��������ֽ5s
			SetIOBit(O_STARTROLLPMOTOR,1);		//�ر���ֽ���
			gl_cReacterStatus &= (~B_ROLLPAPER);
			Printf("Close Roo\n");
		}
		else
		{
		
		}
	/*else
	{
		if (GetIOBit(I_ROLLSTART) == StatusOk)//��ֽ����
		{
			if ((GetIOBit(I_ROLLPEND) == StatusErr) && (gl_cSelfCheckFlag == 0))
			{
				gl_cReacterStatus |= B_ROLLPAPER;
				SetIOBit(O_STARTROLLPMOTOR,0);		//������ֽ���
				gl_iRollPaper = 0;								//��ֽ����������
				Printf("Start Rool\n");
			}
			Printf("Start Rool 2\n");
		}
	}*/
		}
		get_status(cAix, &iStatus);
	} while (iStatus != 0);

}

extern void DebugSpeed(void);

void ITestMove(void)
{
	char state, cReadKey;
	char cDispbuf[15];
	int iMoveLen_X,iMoveLen_Y, iMoveSpeed;
	int iMovePluse_X, iMovePluse_Y;
	
	iMoveLen_X = 50;		// cm
	iMoveLen_Y = 50;
	iMovePluse_X = iMoveLen_X * X_PPMM;
	iMovePluse_Y = iMoveLen_Y * Y_PPMM;
	gl_iVSpeed = 1500;
	iMoveSpeed = gl_iVSpeed;	//
	
	sprintf(cDispbuf, "Sp %d LEN %d", iMoveSpeed, iMoveLen_X);
	lcd_display(0,cDispbuf);
	sprintf(cDispbuf, "X  UP %d", iMoveLen_X);
	lcd_display(1,cDispbuf);

	//SetMovFrameSpeed(gl_iVSpeed);
	state = 1;
	while(state)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		switch (cReadKey)
		{	
			case Key_F2:	//�� 	X
				sprintf(cDispbuf, "X  UP %d", iMoveLen_X);
				lcd_display(1,cDispbuf);
				FixedSpeeDrive(AXIS_X, gl_iVSpeed);
				ITMoveHandpiece(AXIS_X, -iMovePluse_X);
				Printf("X Up Pluse:%d\n", iMovePluse_X);
				break;

			case Key_F8:	//��	X
				sprintf(cDispbuf, "X  DOWN %d", iMoveLen_X);
				lcd_display(1,cDispbuf);
				FixedSpeeDrive(AXIS_X, gl_iVSpeed);
				ITMoveHandpiece(AXIS_X, iMovePluse_X);
				Printf("X Down Pluse:%d\n", iMovePluse_X);
				break;

			case Key_F4:	//��	Y
				sprintf(cDispbuf, "Y  LEFT %d", iMoveLen_Y);
				lcd_display(1,cDispbuf);
				FixedSpeeDrive(AXIS_Y, gl_iVSpeed);
				ITMoveHandpiece(AXIS_Y, -iMovePluse_Y);
				break;

			case Key_F6:	//��	Y
				sprintf(cDispbuf, "Y  RIGHT %d", iMoveLen_Y);
				lcd_display(1,cDispbuf);
				FixedSpeeDrive(AXIS_Y, gl_iVSpeed);
				ITMoveHandpiece(AXIS_Y, iMovePluse_Y);
				break;

			case Key_F1:	//�ƶ�����
				iMoveLen_X += 10;
				iMoveLen_Y += 10;
				iMovePluse_X = iMoveLen_X * X_PPMM;
				iMovePluse_Y = iMoveLen_Y * Y_PPMM;
				sprintf(cDispbuf, "Sp %d LEN %d", gl_iVSpeed, iMoveLen_X);
				lcd_display(0,cDispbuf);
				break;

			case Key_F3:	
				iMoveLen_X -= 10;
				iMoveLen_Y -= 10;
				iMovePluse_X = iMoveLen_X * X_PPMM;
				iMovePluse_Y = iMoveLen_Y * Y_PPMM;
				sprintf(cDispbuf, "Sp %d LEN %d", gl_iVSpeed, iMoveLen_X);
				lcd_display(0,cDispbuf);
				break;

			case Key_F9:	//�ƶ��ٶ� -
				DebugSpeed();
				sprintf(cDispbuf, "Sp %d Len %d", gl_iVSpeed, iMoveLen_X);
				lcd_display(0,cDispbuf);
				break;

			case Key_Pause:	//�ƶ��ٶ� +
				ITestElectromagnet();
				sprintf(cDispbuf, "Sp %d LEN %d", iMoveSpeed, iMoveLen_X);
				LcdDisp(cDispbuf, "          ");
				break;
						
			case Key_F5:
				state = 0;
				break;
		}
	}

	
}


void InitTest(void)
{
	char state, cReadKey;
	if (gl_cLanguageSelect == 0) {
		LcdDisp("���Գ���", "�ȴ�����");
	}
	else {
		LcdDisp("Test Progra", "Wait Opration");
	}
	state = 1;
	while(state)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (cReadKey)
		{	
			case Key_F1:
			Printf("Test Paper Sensor:\n");
			//��������ֽ��Ӧ�� ԭ���Ӧ��
			ITestPaperSensor();
			break;
			
			case Key_F2:
			//������ֽ���
			Printf("Test Feeding Paper:\n");
			ITestFeedingPaper();
			break;
			
			case Key_F3:
			//������ֽ���
			Printf("Test Roll Paper:\n");
			ITestRollPaper();
			break;
			
			case Key_F4:
			//���Ե����
			Printf("Test Electromagnet:\n");
			ITestElectromagnet();
			break;
						
			case Key_Ok:
			//���Աʳ��ƶ�
			ITestMove();
			break;

			case Key_F6:
			
			
			break;

			case Key_Pause:
			
			break;
		}
		if (gl_cLanguageSelect == 0) {
			LcdDisp("���Գ���", "�ȴ�����");
		}
		else {
			LcdDisp("Test Progra", "Wait Opration");
		}
		
	}

}


void SHOW_data(int type ) //CutDataAddr
{	
	int j, cun = 0 ,add = 0 , i = 0 ;
	if (type == 1)
	{
		add = CutDataAddr;
		while (add < 0xDFFFFFF)
		{ 	cun++;
			for(j = 0 ;j < 10 ;j++)
			{
				 i = Readsdram(add  , 8);
				 add++;
				 Printf("%c",&i);
			}
				 Printf(" ");
				 if ((cun%120) == 0 ){
				  Printf(" 0xDFFFFFF ::: %d \n",add);
				 }
		}
	}
	else
	{
		add = BEGINADDR;
		while (add < 0x0c700000)
		{	cun++;
			for(j = 0 ;j < 10 ;j++)
			{
				i = Readsdram(add  , 8);
				add++;
				Printf("%c",&i);
			}
			Printf(" ");
			if ((cun%120) == 0 ){
				Printf(" 0x0c700000 ::: %d \n",add);
			}
		}
	
	}
}

extern	void	FUN_KBProc(void);
extern int gl_intoruncutmod;
//extern void InitFilePaper(void);
/*******************main function*************************/
void Main(void)
{
	SystemClockInit();
	rSYSCFG = CACHECFG;
  	Port_Init();
 	Isr_Init();
  	Delay(0);
  	Uart0_Init(115200);	
	
  	Timer0_Init();

	//Delay(20000);				//20141202 rwt
  	//Printf("lcd init OK\n");
	//LcdDisp("ƽ����ī�и��", "��ʼ��������");
  	ReadParameter();
	//20141202 rwt
 	lcd_init();	
	Delay(20000);				//20141202 rwt
	//Printf("ReadParameter OK\n");
	GlobalPara_Init();
	Init_MCX312();
	
	ClrFIFO;
	gl_cIOWriteValue = 0xff;
	WriteIO(gl_cIOWriteValue);	
	
	//gzr20160831����
	InitFlatPlotter();//��ʽ�и����ʼ��
	Printf("InitFlatPlotter Ok\n");
	Init8900();
	InitCH375();
	
	rI_ISPC = 0x1ffffff;
	rINTMSK = ~(BIT_GLOBAL|BIT_URXD0|BIT_UERR01|BIT_TIMER0);//|BIT_UTXD0|BIT_EINT1
	if(gl_cLanguageSelect==0)
		LcdDisp("��ӭʹ��", "������ī�и�� ");
	else
		LcdDisp("Welcome to use ", "RICHPEACE CAD");
	
	//testmovpps();
	//Printf("gl_iCutSpeed[%d] gl_iMoveFramSpeed[%d]\n", gl_iCutSpeed, gl_iMoveFramSpeed);
	//InitTest();
	//Printf("Start Time!\n");
  	while(1)
  	{
		//gl_intoruncutmod = 1;
		FUN_KBProc();//��������								
		//gl_intoruncutmod = 0;
		ReceiveData();		
	
		InvertPrintData();	//ת����ӡ����
		gl_intoruncutmod = 1;
		WorkStatus();
		gl_intoruncutmod = 0;
	}
	
}
//Start
void ReceiveData(void)
{
	switch (gl_cCommunicationMode)
	{
		case 0:
			PollRecNetData();				//��������
			break;
		case 1:
			PollRecUsbData();				//����USB����
			break;
		default:
			PollRecNetData();
			PollRecUsbData();
			break;
	}
	//Printf("gl_cCommunicationMode ==%d!\n",gl_cCommunicationMode);
}


//ȫ�ֱ���gl_cWorkStatus���ڼ�¼��ǰ����״̬����ǰ����ʱ���Ƚ�ǰһ״̬
//�Ƿ������״̬��ͬ����ͬ���Ƚϵ�ǰ��ӡͷ��ԭ��λ���Ƿ���ͬһλ�ã�
//������ڣ��ص�ԭ��λ��
extern void  ReturnOriginal(long lCurPost_XPlus, long lCurPost_YPlus);
extern void  RunCutMod(void);

//��ֽ��ԭ��
void BackPaper(void)
{
	//��ֽ��⵽�ϸ�Ӧ�����ֽ���¸�Ӧ����Ȼ�������ֽ���ϸ�Ӧ����ֱ��gl_lAxisXPos����0��Ȼ���ֽ���¸�Ӧ��
	int iStep;
	char cEnd;

	if(gl_lAxisXPos <= 0)
	{
		gl_lAxisXPos = 0;
		return;
	}
	
	cEnd = 1;
	iStep = 2 * (int)X_PPMM;
	FixedSpeeDrive(AXIS_X,1000);
	Printf("***1***gl_lAxisXPos:%d*******\n", gl_lAxisXPos);
	while (cEnd)
	{
		/*if (GetIOBit(I_GIVEBACK) == StatusOk)
		{
			SetIOBit(O_GIVEMOTORDIR,G_ROLLPAPER);
			SetIOBit(O_STARTGIVEPMOTOR,0);//������
			
			while (1)
			{
				if (GetIOBit(I_GIVEPEND) == StatusOk)
					break;
			}
			SetIOBit(O_STARTGIVEPMOTOR,1);//ֹͣ��ֽ
			SetIOBit(O_GIVEMOTORDIR,G_SENDPAPER);
			SetIOBit(O_STARTGIVEPMOTOR,0);//������
			Delay(100);
			SetIOBit(O_STARTGIVEPMOTOR,1);//������
			Printf("**2****gl_lAxisXPos:%d*******\n", gl_lAxisXPos);
		}
		else*/
		{
			if((gl_lAxisXPos - iStep) > 0)
			{
				ITMoveHandpiece(AXIS_X, iStep);
				gl_lAxisXPos -= iStep;
			}
			else
			{
				ITMoveHandpiece(AXIS_X, gl_lAxisXPos);
				Printf("**3****gl_lAxisXPos:%d*******\n", gl_lAxisXPos);
				gl_lAxisXPos = 0;
				cEnd = 0;
			}
		}
	}
	Printf("**4****gl_lAxisXPos:%d*******\n", gl_lAxisXPos);

	//InitPaper();
}
//spe

void WorkStatus(void)
{
	static unsigned char oldWorkStatus=0;
	static char stcPHCDTime = 1;
	char PrintEnd=0;
	if(gl_cPrintWorkStatus != oldWorkStatus)
	{
		oldWorkStatus = gl_cPrintWorkStatus;
		switch(gl_cPrintWorkStatus)
		{
			case 0:
				Printf(" WorkStatus-->case 0:\n");
				while (ReadPrintEnd() != 1);
				
				Printf(" ���� Y��ԭ�� \n");
				Printf(" gl_iAxisYPos = %d \n", gl_iAxisYPos);
				Printf(" gl_iOriginalPos = %d \n", gl_iOriginalPos);
				if(gl_iAxisYPos > gl_iOriginalPos)
				{//���� Y��ԭ��
					Printf(" ���� Y��ԭ�� \n");
					Printf("gl_iAxisYPos > gl_iOriginalPos\n");
					FixedSpeeDrive(AXIS_Y,2000);
					ITMoveHandpiece(AXIS_Y, -(gl_iOriginalPos - gl_iAxisYPos));//
					gl_iAxisYPos = gl_iOriginalPos;
				}
				else if(gl_iAxisYPos < gl_iOriginalPos)
					{	
						Printf(" WorkStatus-->case 0:-->else gl_iAxisYPos < gl_iOriginalPos\n");
						Printf("gl_iAxisYPos(%d) < gl_iOriginalPos(%d)\n",gl_iAxisYPos, gl_iOriginalPos);
						FixedSpeeDrive(AXIS_Y,2000);
						ITMoveHandpiece(AXIS_Y, (gl_iOriginalPos - gl_iAxisYPos));
						gl_iAxisYPos = gl_iOriginalPos;
					}
				Printf(" ���� Y��ԭ�� over \n");
				if((gl_cPrintCutSelect&0x02) ==0x02)
				{
					PrintEnd = 1;
					//�����и�ģʽ
					Printf("****In to Cut Mod!*****\n");
				}
				else
				{
					gl_cMachineWorkStatus = 0;	// 20120827  ����
					if(gl_cLanguageSelect==0)
						LcdDisp("  ��ӭʹ��", "������ī�и�� ");
					else						
						LcdDisp("Welcome to use", "RICHPEACE CAD");					
					gl_cStartWorkFlag = 0; // 20141021
				}
				break;
			case 1:
				break;
		}
	}

	if((PrintEnd == 1) || ((gl_cPrintCutSelect == 0x02)&&(1 == gl_cCutWorkStatus)))
	{
		Printf("Run cut module\n");
		Printf("gl_TestSendLengPluse:%d\n", gl_TestSendLengPluse);
		while (ReadPrintEnd() != 1);
		//��ֽ�˻�
		///*****
		Printf("��ֽ�˻� \n");
		BackPaper();
		Printf("��ֽ�˻� \n");
		///*
		gl_cMachineWorkStatus = 1;	// 20120827 
		
		Printf("RunCutMod \n");
		RunCutMod();
		
		Printf("END------------------------------------------>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<< gl_cMachineWorkStatus =%d  \n" ,gl_cMachineWorkStatus );
		
		/*
		��Ҫ����ظ���λ����ͨ��  
		*/
		//�ٶ�
		gl_cMachineWorkStatus = 0;	// 20120827 
		gl_cStartWorkFlag = 0; // 20141021
		
		if(gl_cSWFStartClearFlag != 1)	//20141213 rwt
		{
			gl_cSWFStartClearFlag = 1;
		}
		//****************
		/*gl_cCutFileSendFlag = 0;
		gl_lCurrCutFileLength = 0;
		gl_cCutWorkStatus = 0; //�Ѿ�ִ�����и���Խ�����һ�����ļ�
		gl_MacRunState = 0;	//�ļ���ӡ����
		if(gl_cLanguageSelect==0)
			LcdDisp("  ��ӭʹ��", "������ī�и�� ");
		else
			LcdDisp("Welcome use RP", "Cutter Plotter");*/
	}

	// 20141021 
	if(gl_cStartWorkFlag == 0)
	{
		if(gl_lSWFCount++ > 12000000L)
		{
			gl_lSWFCount = 0;
		
			if (++stcPHCDTime > gl_cPHCDTime)
			{
				stcPHCDTime = 1;
				
				if(gl_cSWFStartClearFlag != 1)
					gl_cSWFStartClearFlag = 1;
				Printf(">1.2KW\n");
			}
		}
	}
}

//��������ʼ��
void Isr_Init(void)
{
	pISR_UNDEF = (unsigned)HaltUndef;
	pISR_SWI = (unsigned)HaltSwi;
	pISR_PABORT = (unsigned)HaltPabort;
	pISR_DABORT = (unsigned)HaltDabort;

	rINTCON = 0x5;						// Non-vectored,IRQ enable,FIQ disable    ��ʸ���ж�ģʽ����ֹFIQ�ж�
	rINTMOD = 0x0;						// All=IRQ mode
	rINTMSK = BIT_GLOBAL;			// All interrupt is masked. ���������ж�
}

void SystemClockInit(void)
{
	rPLLCON = (56 << 12) | (4 << 4) | 1;
}

void HaltUndef(void)
{
	lcd_clr();
	lcd_display(0,"����:001");
	while(1);
}

void HaltSwi(void)
{
	lcd_clr();
	lcd_display(0,"����:002");
	while(1);
}

void HaltPabort(void)
{
	lcd_clr();
	lcd_display(0,"����:003");
	while(1);
}

void HaltDabort(void)
{
	lcd_clr();
	lcd_display(0,"����:004");
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

	rPCONC = 0xaaaaaaaa;	//0x144
	rPUPC  = 0xfffe;	//PULL UP RESISTOR should be enabled to I/O
	rPDATC = 0xffe5;

	//PORT D GROUP
	/*  PORT D GROUP(I/O OR LCD)										*/
	/*  BIT	7		6					5				4					3			2			1			0	*/
	/*  ROLALM	ROLINPOS	CARALM	CARINPOS	NC		NC		NC		NC	*/
	/*	  01		00				01			00				00		00		00		00	*/
	rPCOND= ((0x1<<14) | (0x1<<12));	
	rPUPD = 0xff;
	rPDATD= 0x040;//0x00;	20120517
	//These pins must be set only after CPU's internal LCD controller is enable
	
	//PORT E GROUP 
	/*  Bit 8				7		6		5		4		3		2			1		 	0	*/
	/*      ENDIAN	LED	NC	NC	LED	NC	RXD0	TXD0	PE0	*/ 
	/*      01			01	00	00	01	00	10		10		10	*/
	rPDATE	= 0x1ff;
	rPCONE	= ((0x0<<16) | (0x1<<14) | (0x1<<12) | (0x2<<4) | (0x2<<2) | (0x3<<0));	
	rPUPE	= 0xffff;
	
	//PORT F GROUP
	/*  Bit8	7				6				5		 		4				3				2			1				0		*/   
	/*  NC		KEYIN4	KEYOUT1	KEYOUT2	KEYIN1	KEYIN2	nWAIT	KEYOUT3	KEYOUT4	*/
	/*	000		000			001			001			00			00			00		01			01		*/
	rPDATF = 0x1ff;
	rPCONF = 0x2405;
	rPUPF  = 0x00;	//������������

	//PORT G GROUP
	/*  BIT7	6				5		4		3		2		1		0	 */
	/*	NC		NETINT	NC	NC	NC	NC	USB	KEYIN3	*/
	/*  00    00      00  00  00  00  11  00       */
	//rPDATG = 0xff;
	//rPCONG = 0xffff;
	rPCONG = 0x0C; //EXINT1 enabled
	rPUPG  = 0x02;	//pull-up regiseter should be enabled  
	
	rSPUCR=0x07;  //D15-D0 pull-up disable

	/*�����Cache��*/
	rNCACHBE0=((Non_Cache_End>>12)<<16)|(Non_Cache_Start>>12); 
	/*��Ϊ�͵�ƽ����,Exint1����Ϊ�½��ش���20090903*/
	rEXTINT=0x0|(1<<5);
}


