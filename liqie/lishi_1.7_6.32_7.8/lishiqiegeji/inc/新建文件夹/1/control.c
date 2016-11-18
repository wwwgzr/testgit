#include "..\src\defconst.h"
#include "..\Target\44blib.h"
#include "..\src\extern.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

//#define 	AXIS_X		2							//X��
//#define 	AXIS_Y		1							//Y��

void DriveSprinkler(int iPulseNumX,int iPulseNumY);
void InvertPrintData(void);
//void DrawTestGraph(int dotPix);
void InitPaper(void);
void SetIOBit(unsigned char cBit,unsigned char cEnable);
void DealPrint(void);
void WriteDataToFIFO(void);
void InvertDataBackward(unsigned int iWriteAddr);
void InvertDataForward(unsigned int iWriteAddr);
void ClearPrintHead(unsigned int iFlag);

unsigned short DrawMalposedGraph(unsigned int iGraphWidth);
unsigned short SparyError(unsigned char cPtintCount);
unsigned int CalculateRecDatNum(void);
unsigned int FlashPointerAdd(unsigned int pointer,unsigned int length);
unsigned char GetIOBit(unsigned char cBit);

extern void WaitPrintEnd(void);
extern void ViewError(int iErrorCode);
extern void LineReverse(unsigned char line);
extern void lcd_refresh(void);
extern void lcd_display(unsigned char row,const char *string);
extern void DealUartData(void);
extern void ReceiveData(void);
extern void WaitKeyExit(unsigned char cKeyValue);
extern void Init_MCX312(void);
extern void get_status(int axis, int * value);
//extern void CtrlDownCut();

//**********************************************************
//���ܣ�	��ȡIOλ
//�������:	cBitҪ��ȡ��IOλ(0~7��0xff)
//����ֵ: 	1��IOλΪ1��0��IOλΪ0
//**********************************************************
unsigned char GetIOBit(unsigned char cBit)
{
	unsigned char cTemp;

	cTemp = ReadIO();
	//Printf("Get Io:%x\n",cTemp);
	cTemp >>= cBit;
	cTemp &= 0x01;
	return cTemp;
}

//********************************************************
//���ܣ�	����IOλ
//�������:	cBit  Ҫ���õ�IOλ(0~7)
//					cEnable  1��IOλ����λ1,0��IOλ����Ϊ0
//����ֵ����
//********************************************************
void SetIOBit(unsigned char cBit,unsigned char cEnable)
{
	if (cEnable == 0)
		gl_cIOWriteValue &= ~cBit;
	else
		gl_cIOWriteValue |= cBit;
	WriteIO(gl_cIOWriteValue);
}
	
//���¼��ٱ�
void UpdataAccTable(void)
{
	unsigned int iMaxSpeed;
	double dAccTime,dLength,dSpeedH;
	double dAcc,dAccA;
	double dTemp,dTemp1;
	unsigned char cCount,cTemp;
	int iPenSpeed;
	
	dLength = 2.0;// 1.5;//
	dTemp1 = Y_PPMM / (double)SPEED_M;
	//���Ʊ�����800mm/s����
	Printf("gl_iCutSpeed:%d\n", gl_iCutSpeed);
	if (gl_iCutSpeed > 750)
	{
		if (gl_iCutSpeed < 800)
			iPenSpeed = 750;
		else
			iPenSpeed = gl_iCutSpeed - 50;
	}
	else
		iPenSpeed = gl_iCutSpeed;
	//
	dAccTime = (double)(gl_iATime + 50) * 0.001;	//T�ٶȼӵ���������ʱ��

	dSpeedH = (double)iPenSpeed * dTemp1;

	dAcc = (double)(iPenSpeed - gl_iInitV0) / (dAccTime + 0.05);//A���ٶ�
	
	dAccA = (double)(iPenSpeed - gl_iInitV0) - dAcc * dAccTime;
	dAccA = dAccA / (dAccTime * dAccTime); //���ٶȱ仯��
	
	dTemp = (double)gl_iInitV0;
	if (gl_iInitV0 > 30)
	{
		gl_iStepSpeed[1] = (unsigned int)(dTemp * dTemp1);
		dTemp = dTemp * dTemp;
		dTemp -=  2.0 * dAcc * dLength;
		if (dTemp < 625.0)
		{//�����һ���ٶ�
			gl_iStepSpeed[0] = (unsigned int)(25.0 * dTemp1);
			if (gl_iStepSpeed[0] > gl_iStepSpeed[1])
				gl_iStepSpeed[0] = gl_iStepSpeed[1];
		}
		else
		{//�����һ���ٶ�
			dTemp = sqrt(dTemp) * dTemp1;
			gl_iStepSpeed[0] = (unsigned int)dTemp;
		}
		cCount = 2;
	}
	else
	{
		gl_iStepSpeed[0] = (unsigned int)(dTemp * dTemp1);
		cCount = 1;
	}
	
	while(cCount < 150)
	{
		dTemp = (double)gl_iStepSpeed[cCount-1] / dTemp1;
		dTemp = dTemp * dTemp;
		dTemp +=  2.0 * dAcc * dLength;
		dTemp = sqrt(dTemp) * dTemp1;//Vt�ٶ�
		if (dTemp >= dSpeedH)
		{
			dTemp =  dSpeedH;
			gl_cStep = cCount;
			gl_iStepSpeed[cCount] = (unsigned int)dTemp;
			break;
		}
		gl_iStepSpeed[cCount] = (unsigned int)dTemp;
		dTemp = (double)(gl_iStepSpeed[cCount] - gl_iStepSpeed[cCount-1]) / dTemp1 / dAcc;
		dAcc = dAcc + dAccA * dTemp;
		cCount++;
	}
	if (cCount >= 150)
		gl_cStep = 149;
	
	cCount = 0;
	iMaxSpeed = 216;//26mm/s
	for(cTemp=0;cTemp < gl_cStep;)
	{
		if (iMaxSpeed < gl_iStepSpeed[cTemp])
		{
			if (cTemp == 0)
				gl_cMaxStep[cCount] = 0;
			else
				gl_cMaxStep[cCount] = cTemp - 1;

			cCount++;
			if (cCount < 21)
				iMaxSpeed += 125;//15mm/s
			else if (cCount < 41)
				iMaxSpeed += 108;//13mm/s
			else if (cCount < 61)
				iMaxSpeed += 83;//10mm/s
			else
				return;
		}
		else
			cTemp++;
	}
	for (;cCount < 61;cCount++)
		gl_cMaxStep[cCount] = cTemp;
}

void SetZAxisDevice(int speed)
{
	//����Z��ʼ�ٶ�
	WriteZAxisInitSpeed(speed);
	 //����Z�����ٶ�
	WriteZAxisDriveSpeed(speed);
	 //����Z���ٶ�
	WriteZAxisAddSpeed(0);	
}

//Z���豸��ʼ��
// �ٶ�2000  1S 1Ȧ1Ȧ2000������
void Init_ZAxisDevice()
{
	//char  FindOrgFailState=0;
	char zAxisState;
	int plusZ;
	Printf("Init Z Axis Device: [%x]\n",GetIOBit(I_ZINDUCTOR));

	SetZAxisDevice(2000);
	//���Z�Ƿ���ԭ��
	if (GetIOBit(I_ZINDUCTOR) == StatusOk)
	{	//  Z���Ӧ�����ڵ�
		Printf("Z Axis No Org!\n");
		WriteZAxisPlus(-4000);
		plusZ = ReadZAxisPlus();
		Printf("Read Z Plus:[%d]\n", plusZ);
		
		//����Z��������
		WriteZAxisDriveCmd(DriveZAxisStartCmd);
		do{
			 zAxisState = ZAxisDriveEndState;
			 if (zAxisState == 1)
			 {//Z����������δ�ҵ�Z��ԭ��
			 	ViewError(ER_ZINDERR);
			 	/*
			 	if (FindOrgFailState)
			 	{//��ʱ��180��˳ʱ��360��δ�ҵ�ԭ�㣬��ԭ���Ӧ����
			 		Printf("Init Z Axis Fail\n");
					ViewError(ER_ZINDERR);
			 	}
				Delay(10);
				//˳ʱ����ת360��Ѱ��ԭ��
			 	WriteZAxisPlus(2000);
				WriteZAxisDriveCmd(DriveZAxisStartCmd);
				FindOrgFailState = 1;
				*/
			 }
		}while(GetIOBit(I_ZINDUCTOR) == StatusOk);//Z��Ӧ��δ���ڵ�
		WriteZAxisPlus(0);
		WriteZAxisDriveCmd(DriveZAxisStopCmd);
		

	}
/*	else
	{// ��ǰZ����ԭ��
		Printf("Z Axis at Org!\n");
		WriteZAxisPlus(100);
		WriteZAxisDriveCmd(DriveZAxisStartCmd);	
		while(!ZAxisDriveEndState);//�ȴ�Z����������

		while(GetIOBit(I_ZINDUCTOR)== StatusErr )
		{//Z���Ӧ������ס
			ViewError(ER_ZINDERR);
		}
		
	}

	//ԭ��λ��У��
	SetZAxisDevice(1000);
	WriteZAxisPlus( -100);
	WriteZAxisDriveCmd(DriveZAxisStartCmd);	
	do{
		 if (ZAxisDriveEndState)
		 {
			ViewError(ER_ZINDERR);
		 }
	}while(GetIOBit(I_ZINDUCTOR) == StatusOk);

	WriteZAxisPlus(0);
	WriteZAxisDriveCmd(DriveZAxisStopCmd);
*/
	//�����趨Z�������ٶ�
	SetZAxisDevice(2000);

	gl_cCutUpDownState = 0; //������ԭ��λ��
	Printf("Init Z Axis Device OK\n");
	//CtrlDownCut();
	//while(1);
}

//extern void testMcx312(void);
//extern void lp(int axis, long wdata);
//extern void ep(int axis, long wdata);
//��ʼ��
//����Y�ص���ͼ�����ֵ
//����X�ص���ͼ�����ֵ
// 1, �����ж��Ƿ���ԭ���Ӧ��λ�ã�������Y���������Ҷˣ�
//�����Ȼδ��Ӧ����Ӧ������X���������¶�,ֱ����Ӧ��ԭ���Ӧ��
// 2,�ж�Z���ԭ�㣬�����ڣ�����Z���ԭ�㣻
void InitFlatPlotter()
{
	//int iMaxPulseNum;
	//int iPlusNumX, iPlusNumY;
	//int iTemp;
	int iToalPulse_X, iToalPulse_Y;
	int InductorStatus=0, cStatus=0;;

		
	ClrFIFO;
	gl_cIOWriteValue = 0xff;
	WriteIO(gl_cIOWriteValue);	

	//Z���豸��ʼ��
	Init_ZAxisDevice();
	
	//���¼��ٶȱ�
	UpdataAccTable();
	
	//��ʼ���˶�����оƬMCX312
	Init_MCX312();

	//DEBUG
	#if DEBUG == 1
	{
		Printf("Y up:%x\n", GetIOBit(I_YUPINDUCTOR));
		Printf("Y down:%x\n", GetIOBit(I_YDOWNINDUCTOR));
		Printf("X up:%x\n", GetIOBit(I_XLEFTINDUCTOR));
		Printf("X down:%x\n", GetIOBit(I_XRIGHINDUCTOR));
		Printf("Z up:%x\n", GetIOBit(I_ZINDUCTOR));
		Printf("MoveFramSpeed : %d\n", gl_iMoveFramSpeed);
	}
	#endif

	// ��������Ѱ��XY��еԭ��
	iToalPulse_X = (gl_sMachineLenght+10)* (int)X_PPMM;
	iToalPulse_Y = (gl_sMachineWidth+10)* (int)Y_PPMM;;
	if (GetIOBit(I_YUPINDUCTOR) == StatusErr)
		InductorStatus |= 0x01;
	
	if (GetIOBit(I_XRIGHINDUCTOR) == StatusErr)
		InductorStatus |= 0x02;
	
	if (InductorStatus != 0)
	{
		set_startv(AXIS_X+AXIS_Y,50);
		set_speed(AXIS_X+AXIS_Y,gl_iMoveFramSpeed);
		switch(InductorStatus)
		{
			case 1:
				pmove(AXIS_Y,iToalPulse_Y);
				while(1)
				{
					if (GetIOBit(I_YUPINDUCTOR) == StatusOk)
					{// ��ǰ��Ӧ������ס
						pmove(AXIS_Y,0);
						break;
					}
					//Y���巢�����¿϶����ʹ�Y�ϸ�Ӧ����
					get_status(AXIS_Y, &InductorStatus);
					if(InductorStatus == 0 && GetIOBit(I_YUPINDUCTOR) == StatusErr)
						ViewError(ER_UINDERR);
				}
				break;
				
			case 2:
				//Printf("R Inductor != OK\n");
				pmove(AXIS_X,iToalPulse_X);
				//Printf("R Send Pulse : %d\n", iToalPulse_X);
				while(1)
				{
					if (GetIOBit(I_XRIGHINDUCTOR) == StatusOk)
					{// ��ǰ��Ӧ������ס
						pmove(AXIS_X,0);
						break;
					}
					
					get_status(AXIS_X, &InductorStatus);
					if(InductorStatus == 0 && GetIOBit(I_XRIGHINDUCTOR) == StatusErr)
						ViewError(ER_RINDERR);
				}
				break;
				
			case 3:
				//���ö̾��г̱���
				InductorStatus = 0;
				//iTemp = gl_iCutSpeed;
				//gl_iCutSpeed = 10;
				//SetSDPenSpeed();
				//gl_iCutSpeed = iTemp;
				//����ֱ�߲岹�˶�
				//inp_move2(iToalPulse_X,iToalPulse_Y);
				pmove(AXIS_Y,iToalPulse_Y);
				pmove(AXIS_X,iToalPulse_X);
				do
				{
					if ( (GetIOBit(I_XRIGHINDUCTOR) == StatusOk) && (InductorStatus &0x01) != 0x01 )
					{//	Printf("Check I_XRIGHINDUCTOR\n");
						pmove(AXIS_X,0);
						//WriteCommand(AXIS_X, 0x27);
						InductorStatus |= 0x01;
					}

					if ( (GetIOBit(I_YUPINDUCTOR) == StatusOk) && (InductorStatus & 0x02) != 0x02 )
					{	//Printf("Check I_YUPINDUCTOR\n");
						pmove(AXIS_Y,0);
						//WriteCommand(AXIS_Y, 0x27);
						InductorStatus |= 0x02;
					}

					get_status(AXIS_X, &cStatus);
					if(cStatus == 0 && GetIOBit(I_XRIGHINDUCTOR) == StatusErr)
						ViewError(ER_RINDERR);
					
					get_status(AXIS_Y, &cStatus);
					if(cStatus == 0 && GetIOBit(I_YUPINDUCTOR) == StatusErr)
						ViewError(ER_UINDERR);
				} while (InductorStatus != 0x03);
				break;
		}
	}

	gl_iCurrentPos_X = gl_sMachineLenght;											
	gl_iCurrentPos_Y= gl_sMachineWidth;
	Printf("Init plotter:gl_sMachineLenght:%d,  gl_sMachineWidth:%d\n",gl_sMachineLenght, gl_sMachineWidth);
	
}

/*
//��ʼ����ͼ��
void InitPlotter()
{
	int iPos,iStep;
	int iMaxPulseNum;
	
	ClrFIFO;
	gl_cIOWriteValue = 0xff;
	WriteIO(gl_cIOWriteValue);
	
	//��ֽ
	iPos = 0;
	iStep = -3 * (int)X_PPMM;
	while (1)
	{
		if (GetIOBit(I_GIVEPEND) == StatusOk)
		{
			if (iPos != 0)
			{
				SoftDelay(1000);
				DriveSprinkler(-iPos,0);
				while (ReadPrintEnd() == 0);//�ȴ���ӡ����
			}
			SetIOBit(O_STARTGIVEPMOTOR,0);//������ֽ
			iStep = 0;
			while ((GetIOBit(I_GIVEBACK) == StatusErr) && (iStep < 5000000))
				iStep++;
			SetIOBit(O_STARTGIVEPMOTOR,1);//ֹͣ��ֽ
			break;
		}
		else if ((GetIOBit(I_GIVEBACK) == StatusOk) || (iPos < 226*iStep))
		{//������ֽ
			if (iPos != 0)
			{
				DriveSprinkler(-iPos,0);
				while (ReadPrintEnd() == 0);//�ȴ���ӡ����
			}
			break;
		}
		else
		{
			DriveSprinkler(iStep,0);
			while (ReadPrintEnd() == 0);//�ȴ���ӡ����
			iPos += iStep;
		}
	}
	//��Y��ԭ��
	iPos = 0;
	iStep = 2 * (int)Y_PPMM;
	iMaxPulseNum = gl_iMaxYMM + 50 * Y_PPMM;
	while ((GetIOBit(I_YORIGIN) == StatusOk) && (iPos < 20*iStep))
	{  //����ʳ��Ѿ�ͣ��Y��ԭ��λ��,��Ҫ��������(Y��������)�ƶ�������ԭ���λ��
		DriveSprinkler(0,iStep);
		if (gl_cPrintStatus == 0x10)
		{
			strcpy(gl_cLcdDispBuf[0],"��������");
			strcpy(gl_cLcdDispBuf[1],"��ػ�����");
			lcd_refresh();
			while (1);
		}
		iPos += iStep;
		while (ReadPrintEnd() == 0);//�ȴ���ӡ����
	}
	if (iPos >= 20*iStep)
	{  //�ʳ������ƶ���20mm�Ե�סԭ�㴫����,ԭ�㴫������
		ViewError(ER_YOBAD);
	}
	Delay(1000);
	iPos = 0;
	while (GetIOBit(I_YORIGIN) == StatusErr)
	{  //����Y��ԭ��λ��
		if (iPos > iMaxPulseNum)
			ViewError(ER_YOBAD);
		DriveSprinkler(0,-iStep);
		if (gl_cPrintStatus == 0x10)
		{
			strcpy(gl_cLcdDispBuf[0],"��������");
			strcpy(gl_cLcdDispBuf[1],"��ػ�����");
			lcd_refresh();
			while (1);
		}
		iPos += iStep;
		while (ReadPrintEnd() == 0);//�ȴ���ӡ����
	}
	Delay(1000);
	
	iMaxPulseNum = (int)(gl_sPlotType * Y_PPMM);
	if (gl_iOriginalPos <= 0)
	{//��ʼ��ӡλ����ԭ���Ӧ��1cm
		gl_iOriginalPos = 0;
		iStep = 10 * Y_PPMM;
		DriveSprinkler(0,iStep);
	}
	else
	{
		if (gl_iOriginalPos > iMaxPulseNum)
			gl_iOriginalPos = (long)iMaxPulseNum;
		iStep = gl_iOriginalPos + 10 * Y_PPMM;
		DriveSprinkler(0,iStep);
		while (ReadPrintEnd() == 0);//�ȴ���ӡ����
	}
	gl_iAxisYPos = gl_iOriginalPos;
}
*/
void DriveSprinkler(int iPulseNumX,int iPulseNumY)
{
	//int axis, plus, speed;
	int speed;
	if ((rPDATG& 0x08) == 0)
	{//�ж�Y���Ƿ��Ѿ�����
		gl_cPrintStatus = 0x10;
		return;
	}
	//Printf("AA:YPlus[%d]\n", iPulseNumY);
	//Write312YAxisPlus(gl_iGapPixel);
	//plus = Read312YAxisPlus();
	//speed = ReadF312();
	//Printf("AB:Read312:[%d],[%d]\n", plus,speed);
	if(iPulseNumX != 0)
	{
		speed = (((double)(gl_iPrintSpeed/2) * X_PPMM) / 20.0);;
		set_startv(AXIS_X,speed);
		set_speed(AXIS_X,speed);
		pmove(AXIS_X, iPulseNumX);
		//while (ReadPrintEnd() != 0);
	}

	if(iPulseNumY != 0)
	{
		if(gl_cBidirectional == 1)
		{
			if(iPulseNumY < 0)
			{
				WriteDirct(0xaaaa);//��ӡ����
				//Printf("-\n");
			}
			else
			{
				WriteDirct(0x5555);//��ӡ����
				//Printf("+\n");
			}
		}
		else
		{
			if(iPulseNumY > 0)
			{
				WriteDirct(0x5555);//��ӡ����
				//Printf("+\n");
			}
		}
		Write312YAxisPlus(iPulseNumY);// 408
		speed = (((double)(gl_iPrintSpeed) * Y_PPMM) / 20.0);
		set_startv(AXIS_Y,speed);
		set_speed(AXIS_Y,speed);
		if(iPulseNumY < 0)
			iPulseNumY += -200;
		else
			iPulseNumY += 200;
		//Printf("PY[%d]\n", iPulseNumY);
		pmove(AXIS_Y, iPulseNumY);
		//while (ReadPrintEnd() != 0);
	}
	/*
	if(iPulseNumY != 0)
	{
		axis = AXIS_Y;
		plus = iPulseNumY;
		speed = (((double)gl_iPrintSpeed * Y_PPMM) / 20.0);
		Write312YAxisPlus(gl_iGapPixel);
	}
	else{
			axis = AXIS_X;
			plus = iPulseNumX;
			speed = (((double)(gl_iPrintSpeed/2) * X_PPMM) / 20.0);;
		}
	set_startv(axis,speed);
	set_speed(axis,speed);
	pmove(axis, plus);		
	*/
	while (ReadPrintEnd() != 0);
	gl_iMovePaperLength += iPulseNumX;
}

//��������ת�������ָ����ַ
void InvertDataBackward(unsigned int iWriteAddr)
{
	unsigned char i,j,k,m,n;
	unsigned char cMultiDotLine[4];
	unsigned short sdat1,sdat2;
	unsigned int idat1,idat2;
	unsigned int iCountBit;
	unsigned int iCount,iCount1,iCount2,iCount3,iCount4;
	unsigned int iCountBak;
	unsigned int iCountByte;
	unsigned int iEthnetRecBufPE;
	unsigned int bakWriteAddr,bakWriteAddr1,ReadAddr,ReadAddr1,ReadAddr2;
	unsigned int iYDotSpace[3];
	
	cMultiDotLine[0] = 0;
	switch (gl_cSprinklerCount)
	{
		case 1:
			iCountByte = 30;
			break;
		case 2:
			iCountByte = 58;
			if (gl_cSprinklerNo == 0)
			{
				cMultiDotLine[1] = gl_cMultiDotLine12;
				iYDotSpace[0] = gl_iYDotSpace12;
			}
			else if (gl_cSprinklerNo == 1)
			{
				cMultiDotLine[1] = gl_cMultiDotLine23;
				iYDotSpace[0] = gl_iYDotSpace23;
			}
			else
			{
				cMultiDotLine[1] = gl_cMultiDotLine34;
				iYDotSpace[0] = gl_iYDotSpace34;
			}
			break;
		case 3:
			iCountByte = 86;
			if (gl_cSprinklerNo == 0)
			{
				cMultiDotLine[1] = gl_cMultiDotLine12;
				cMultiDotLine[2] = gl_cMultiDotLine23;
				iYDotSpace[0] = gl_iYDotSpace12;
				iYDotSpace[1] = gl_iYDotSpace23;
			}
			else
			{
				cMultiDotLine[1] = gl_cMultiDotLine23;
				cMultiDotLine[2] = gl_cMultiDotLine34;
				iYDotSpace[0] = gl_iYDotSpace23;
				iYDotSpace[1] = gl_iYDotSpace34;
			}
			break;
		default:
			cMultiDotLine[1] = gl_cMultiDotLine12;
			cMultiDotLine[2] = gl_cMultiDotLine23;
			cMultiDotLine[3] = gl_cMultiDotLine34;
			iYDotSpace[0] = gl_iYDotSpace12;
			iYDotSpace[1] = gl_iYDotSpace23;
			iYDotSpace[2] = gl_iYDotSpace34;
			iCountByte = 114;
			break;
	}
	//Printf("IB[%d],SD[%d], IW[0x%x]\n", iCountByte, gl_iStaggerDot, iWriteAddr);
	bakWriteAddr = iWriteAddr + (iCountByte - 2) * (gl_iGraphWidth * 8 + AdditionDot - gl_iStaggerDot + MotorSideDot);
	for (n=0;n<(gl_cSprinklerCount-1);n++)
		bakWriteAddr = bakWriteAddr + (iCountByte - 2) * iYDotSpace[n];
	if (iWriteAddr == InvertDataAddr0)
		gl_iDataEndAddr0 = bakWriteAddr;
	else
		gl_iDataEndAddr1 = bakWriteAddr;
	
	iWriteAddr = bakWriteAddr + (AdditionDot + gl_iStaggerDot + OriginalSideDot - 1) * (iCountByte - 2);
	
	idat1 = AdditionDot + gl_iStaggerDot + OriginalSideDot;
	iCount1 = 2 * (iCountByte- 2);
	//Printf("backAdd:[0x%x], starAdd[0x%x],ID[%d],iC[%d]\n", bakWriteAddr, iWriteAddr, idat1, iCount1);
	for (iCount=0;iCount<idat1;iCount++)
	{//��������ɨ�����
		for (k=0;k<gl_cSprinklerCount;k++)
		{
			for (j=0;j<14;j++)
			{
				Writesdram(iWriteAddr,0);
				iWriteAddr += 2;
			}
		}
		iWriteAddr -= iCount1;
	}
	//Printf("IW0[0x%x],IC1[%d]\n", iWriteAddr, iCount1);
	bakWriteAddr = iWriteAddr;
	iCount1 = gl_iGraphWidth * 8;
	for (n=0;n<gl_cSprinklerCount;n++)
	{
		ReceiveData();
		iWriteAddr = bakWriteAddr + n * 28;
		j = gl_cSprinklerCount - 1;
		i = 0;
		for (k=j-n;k<j;k++)
		{
			for (iCount=0;iCount<iYDotSpace[i];iCount++)
			{
				for (m=0;m<14;m++)
				{
					Writesdram(iWriteAddr,0x0);
					iWriteAddr += 2;
				}
				iWriteAddr -= (iCountByte + 26);
			}
			i++;
		}
	//	Printf("IW1[0x%x]\n", iWriteAddr);
		bakWriteAddr1 = iWriteAddr;
		for (k=0;k<7;k++)
		{
			if ((k == 0) || (k == 6))
			{
				m = 20;
				iCountBit = 0x0f;
			}
			else
			{
				m = 22;
				iCountBit = 0x3f;
			}
			if (m <= cMultiDotLine[n])
			{
				iWriteAddr = bakWriteAddr1 + (unsigned int)(k * 4);
				for (iCount=0;iCount<iCount1;iCount++)
				{
					Writesdram(iWriteAddr,0);
					iWriteAddr += 2;
					Writesdram(iWriteAddr,0);
					iWriteAddr -= iCountByte;
				}
				cMultiDotLine[n] -= m;	
				continue;
			}
			iWriteAddr = bakWriteAddr1 + (unsigned int)(k * 4);
			//Printf("IW3[0x%x]\n", iWriteAddr);
			j = cMultiDotLine[n] + gl_cRemainderBit;
			gl_cRemainderBit = 16 - gl_cRemainderBit;
			iCount = 0;
			if (j >= m)
			{
				iCount2 = EthnetBufLength - gl_iEthnetRecBufPE;
				iCount2 >>= 1;
				ReadAddr = gl_iEthnetRecBufPE + BEGINADDR;
				if (iCount2 > iCount1)
					iCount2 = iCount1;
				while (iCount < iCount1)
				{
					for (;iCount<iCount2;iCount++)
					{
						sdat1 = Readsdram(ReadAddr,16);
						sdat2 = sdat1 >> gl_cRemainderBit;
						idat1 = (unsigned int)sdat2;
						idat1 = idat1 << cMultiDotLine[n];
						//idat1 &= iCountBit;
						sdat1 = (unsigned short)(idat1 & 0xffff);
						sdat2 = (unsigned short)((idat1 >> 16) & iCountBit);
						Writesdram(iWriteAddr,sdat1);
						iWriteAddr += 2;
						Writesdram(iWriteAddr,sdat2);
						iWriteAddr -= iCountByte;
						ReadAddr += 2;
					}
					ReadAddr = BEGINADDR;
					iCount2 = iCount1;
				}
				if (j == m)
				{
					gl_iEthnetRecBufPE = FlashPointerAdd(gl_iEthnetRecBufPE,(iCount1 * 2));
					gl_cRemainderBit = 16;
				}
				else
					gl_cRemainderBit = j - m;
			}
			else if ((j + 16) >= m)
			{
				iCount2 = EthnetBufLength - gl_iEthnetRecBufPE;
				iCount2 >>= 1;
				ReadAddr = gl_iEthnetRecBufPE + BEGINADDR;
				iEthnetRecBufPE = FlashPointerAdd(gl_iEthnetRecBufPE,(iCount1 * 2));
				iCount3 = EthnetBufLength - iEthnetRecBufPE;
				iCount3 >>= 1;
				ReadAddr1 = iEthnetRecBufPE + BEGINADDR;
				
				if (iCount3 < iCount2)
					iCountBak = iCount3;
				else
					iCountBak = iCount2;
				if (iCountBak > iCount1)
					iCountBak = iCount1;
				while (iCount < iCount1)
				{
					for (;iCount<iCountBak;iCount++)
					{
						sdat1 = Readsdram(ReadAddr, 16);
						sdat2 = sdat1 >> gl_cRemainderBit;
						idat1 = (unsigned int)sdat2;
						idat1 = idat1 << cMultiDotLine[n];
						sdat2 = Readsdram(ReadAddr1, 16);
						idat2 = (unsigned int)sdat2;
						idat2 <<= j;
						idat1 |= idat2;
						//idat1 &= iCountBit;
						sdat1 = (unsigned short)(idat1 & 0xffff);
						sdat2 = (unsigned short)((idat1 >> 16) & iCountBit);
						Writesdram(iWriteAddr,sdat1);
						iWriteAddr += 2;
						Writesdram(iWriteAddr,sdat2);
						iWriteAddr -= iCountByte;
						ReadAddr += 2;
						ReadAddr1 += 2;
					}
					if (iCountBak == iCount1)
						break;
					else if (iCountBak == iCount3)
					{
						ReadAddr1 = BEGINADDR;
						if ((iCount2 > iCountBak) && (iCount2 < iCount1))
							iCountBak = iCount2;
						else
							iCountBak = iCount1;
					}
					else
					{
						ReadAddr = BEGINADDR;
						if ((iCount3 > iCountBak) && (iCount3 < iCount1))
							iCountBak = iCount3;
						else
							iCountBak = iCount1;
					}
				}
				if ((j + 16) == m)
				{
					gl_iEthnetRecBufPE = FlashPointerAdd(gl_iEthnetRecBufPE,(iCount1 * 4));
					gl_cRemainderBit = 16;
				}
				else
				{
					gl_iEthnetRecBufPE = FlashPointerAdd(gl_iEthnetRecBufPE,(iCount1 * 2));
					gl_cRemainderBit = j + 16 - m;
				}
			}
			else
			{
				iCount2 = EthnetBufLength - gl_iEthnetRecBufPE;
				iCount2 >>= 1;
				ReadAddr = gl_iEthnetRecBufPE + BEGINADDR;
				iEthnetRecBufPE = FlashPointerAdd(gl_iEthnetRecBufPE,(iCount1 * 2));
				iCount3 = EthnetBufLength - iEthnetRecBufPE;
				iCount3 >>= 1;
				ReadAddr1 = iEthnetRecBufPE + BEGINADDR;
				iEthnetRecBufPE = FlashPointerAdd(gl_iEthnetRecBufPE,(iCount1 * 4));
				iCount4 = EthnetBufLength - iEthnetRecBufPE;
				iCount4 >>= 1;
				ReadAddr2 = iEthnetRecBufPE + BEGINADDR;
				if ((iCount2 <= iCount3) && (iCount2 <= iCount4))
					iCountBak = iCount2;
				else if ((iCount3 <= iCount2) && (iCount3 <= iCount4))
					iCountBak = iCount3;
				else
					iCountBak = iCount4;
				if (iCountBak > iCount1)
					iCountBak = iCount1;
				while (iCount < iCount1)
				{	
					for (;iCount<iCountBak;iCount++)
					{
						sdat1 = Readsdram(ReadAddr,16);
						sdat2 = sdat1 >> gl_cRemainderBit;
						idat1 = (unsigned int)sdat2;
						idat1 = idat1 << cMultiDotLine[n];
						sdat2 = Readsdram(ReadAddr1,16);
						idat2 = (unsigned int)sdat2;
						sdat2 = Readsdram(ReadAddr2, 16);
						idat2 |= ((unsigned int)sdat2 << 16);
						idat2 <<= j;
						idat1 |= idat2;
						//sdat2 = Readsdram(ReadAddr2);
						//idat2 = (unsigned int)sdat2;
						//idat2 <<= (j + 16);
						//idat1 |= idat2;
						//idat1 &= iCountBit;
						sdat1 = (unsigned short)(idat1 & 0xffff);
						sdat2 = (unsigned short)((idat1 >> 16) & iCountBit);
						Writesdram(iWriteAddr,sdat1);
						iWriteAddr += 2;
						Writesdram(iWriteAddr,sdat2);
						iWriteAddr -= iCountByte;
						ReadAddr += 2;
						ReadAddr1 += 2;
						ReadAddr2 += 2;
					}
					if (iCountBak == iCount1)
						break;
					else if (iCountBak == iCount4)
					{
						ReadAddr2 = BEGINADDR;
						if (iCount3 > iCountBak)
						{
							if (iCount2 > iCountBak)
							{
								if (iCount3 <= iCount2)
									iCountBak = iCount3;
								else
									iCountBak = iCount2;
							}
							else
								iCountBak = iCount3;
						}
						else if (iCount2 > iCountBak)
							iCountBak = iCount2;
						else
							iCountBak = iCount1;
					}
					else if (iCountBak == iCount3)
					{
						ReadAddr1 = BEGINADDR;
						if (iCount4 > iCountBak)
						{
							if (iCount2 > iCountBak)
							{
								if (iCount4 <= iCount2)
									iCountBak = iCount4;
								else
									iCountBak = iCount2;
							}
							else
								iCountBak = iCount4;
						}
						else if (iCount2 > iCountBak)
							iCountBak = iCount2;
						else
							iCountBak = iCount1;
					}
					else
					{
						ReadAddr = BEGINADDR;
						if (iCount4 > iCountBak)
						{
							if (iCount3 > iCountBak)
							{
								if (iCount4 <= iCount3)
									iCountBak = iCount4;
								else
									iCountBak = iCount3;
							}
							else
								iCountBak = iCount4;
						}
						else if (iCount3 > iCountBak)
							iCountBak = iCount3;
						else
							iCountBak = iCount1;
					}
					if (iCountBak > iCount1)
						iCountBak = iCount1;
				}
				gl_iEthnetRecBufPE = FlashPointerAdd(gl_iEthnetRecBufPE,(iCount1 * 4));
				gl_cRemainderBit = j + 32 - m;
			}
			cMultiDotLine[n] = 0;
		}
		iWriteAddr -= 24;
		
		idat1 = AdditionDot - gl_iStaggerDot + MotorSideDot;
	//	Printf("IW4[0x%x], ID[%d]\n", iWriteAddr,idat1);
		for (iCount=0;iCount<idat1;iCount++)
		{
			for (j=0;j<14;j++)
			{
				Writesdram(iWriteAddr,0);
				iWriteAddr += 2;
			}
			iWriteAddr -= (iCountByte + 26);
		}
//		Printf("IW5[0x%x]\n", iWriteAddr);
		for (i=n;i<(gl_cSprinklerCount-1);i++)
		{
			for (iCount=0;iCount<iYDotSpace[i];iCount++)
			{
				for (j=0;j<14;j++)
				{
					Writesdram(iWriteAddr,0);
					iWriteAddr += 2;
				}
				iWriteAddr -= (iCountByte + 26);
			}
		}
	}
//	Printf("endAdd:[0x%x]\n",iWriteAddr);
}

//��������ת�������ָ����ַ
void InvertDataForward(unsigned int iWriteAddr)
{
	unsigned char i,j,k,m,n;
	unsigned char cMultiDotLine[4];
	unsigned short sdat1,sdat2;
	unsigned int idat1,idat2;
	unsigned int iCountBit;
	unsigned int iCount,iCount1,iCount2,iCount3,iCount4;
	unsigned int iCountBak;
	unsigned int iCountByte;
	unsigned int iEthnetRecBufPE;
	unsigned int bakWriteAddr,bakWriteAddr1,ReadAddr,ReadAddr1,ReadAddr2;
	unsigned int iYDotSpace[3];
	
	cMultiDotLine[0] = 0;
	switch (gl_cSprinklerCount)
	{
		case 1:
			iCountByte = 26;
			break;
		case 2:
			iCountByte = 54;
			if (gl_cSprinklerNo == 0)
			{
				cMultiDotLine[1] = gl_cMultiDotLine12;
				iYDotSpace[0] = gl_iYDotSpace12;
			}
			else if (gl_cSprinklerNo == 1)
			{
				cMultiDotLine[1] = gl_cMultiDotLine23;
				iYDotSpace[0] = gl_iYDotSpace23;
			}
			else
			{
				cMultiDotLine[1] = gl_cMultiDotLine34;
				iYDotSpace[0] = gl_iYDotSpace34;
			}
			break;
		case 3:
			iCountByte = 82;
			if (gl_cSprinklerNo == 0)
			{
				cMultiDotLine[1] = gl_cMultiDotLine12;
				cMultiDotLine[2] = gl_cMultiDotLine23;
				iYDotSpace[0] = gl_iYDotSpace12;
				iYDotSpace[1] = gl_iYDotSpace23;
			}
			else
			{
				cMultiDotLine[1] = gl_cMultiDotLine23;
				cMultiDotLine[2] = gl_cMultiDotLine34;
				iYDotSpace[0] = gl_iYDotSpace23;
				iYDotSpace[1] = gl_iYDotSpace34;
			}
			break;
		default:
			cMultiDotLine[1] = gl_cMultiDotLine12;
			cMultiDotLine[2] = gl_cMultiDotLine23;
			cMultiDotLine[3] = gl_cMultiDotLine34;
			iYDotSpace[0] = gl_iYDotSpace12;
			iYDotSpace[1] = gl_iYDotSpace23;
			iYDotSpace[2] = gl_iYDotSpace34;
			iCountByte = 110;
			break;
	}
	//Printf("DataForward....\n");
	//Printf("SC[%d],SN[%d],ML[%d],YS[%d]\n", gl_cSprinklerCount, gl_cSprinklerNo,gl_cMultiDotLine12, gl_iYDotSpace12 );
	bakWriteAddr = iWriteAddr + (iCountByte + 2) * (gl_iGraphWidth * 8 + AdditionDot + OriginalSideDot);
	//Printf("Base:[0x%x],Bak:[0x%x]\n",iWriteAddr, bakWriteAddr);
	for (n=0;n<(gl_cSprinklerCount-1);n++)
		bakWriteAddr = bakWriteAddr + (iCountByte + 2) * iYDotSpace[n];
	//Printf("BakEnd:[0x%x]\n", bakWriteAddr);
	if (iWriteAddr == InvertDataAddr0)
		gl_iDataEndAddr0 = bakWriteAddr;
	else
		gl_iDataEndAddr1 = bakWriteAddr;
	
	//���߸��ӵ�
	idat1 = AdditionDot + OriginalSideDot;
	for (iCount=0;iCount<idat1;iCount++)
	{
		for (k=0;k<gl_cSprinklerCount;k++)
		{
			for (j=0;j<14;j++)
			{
				Writesdram(iWriteAddr,0);
				iWriteAddr += 2;
			}
		}
	}
//	Printf("CurAdr:[0x%x]\n", iWriteAddr);
	
	bakWriteAddr = iWriteAddr;
	iCount1 = gl_iGraphWidth * 8;
	for (n=0;n<gl_cSprinklerCount;n++)
	{
		ReceiveData();
		iWriteAddr = bakWriteAddr + n * 28;
		j = gl_cSprinklerCount - 1;
		i = 0;
		for (k=j-n;k<j;k++)
		{	
			for (iCount=0;iCount<iYDotSpace[i];iCount++)
			{
				for (m=0;m<14;m++)
				{
					Writesdram(iWriteAddr,0);
					iWriteAddr += 2;
				}
				iWriteAddr += (iCountByte - 26);
			}
			i++;
		}
		bakWriteAddr1 = iWriteAddr;
		for (k=0;k<7;k++)
		{
			if ((k == 0) || (k == 6))
			{
				m = 20;
				iCountBit = 0x0f;
			}
			else
			{
				m = 22;
				iCountBit = 0x3f;
			}
			if (m <= cMultiDotLine[n])
			{
				iWriteAddr = bakWriteAddr1 + (unsigned int)(k * 4);
				for (iCount=0;iCount<iCount1;iCount++)
				{
					Writesdram(iWriteAddr,0);
					iWriteAddr += 2;
					Writesdram(iWriteAddr,0);
					iWriteAddr += iCountByte;
				}
				cMultiDotLine[n] -= m;	
				continue;
			}
			iWriteAddr = bakWriteAddr1 + (unsigned int)(k * 4);
			j = cMultiDotLine[n] + gl_cRemainderBit;
			gl_cRemainderBit = 16 - gl_cRemainderBit;
			iCount = 0;
			if (j >= m)
			{
				iCount2 = EthnetBufLength - gl_iEthnetRecBufPE;
				iCount2 >>= 1;
				ReadAddr = gl_iEthnetRecBufPE + BEGINADDR;
				if (iCount2 > iCount1)
					iCount2 = iCount1;
				while (iCount < iCount1)
				{
					for (;iCount<iCount2;iCount++)
					{
						sdat1 = Readsdram(ReadAddr, 16);
						sdat2 = sdat1 >> gl_cRemainderBit;
						idat1 = (unsigned int)sdat2;
						idat1 = idat1 << cMultiDotLine[n];
						//idat1 &= iCountBit;
						sdat1 = (unsigned short)(idat1 & 0xffff);
						sdat2 = (unsigned short)((idat1 >> 16) & iCountBit);
						Writesdram(iWriteAddr,sdat1);
						iWriteAddr += 2;
						Writesdram(iWriteAddr,sdat2);
						iWriteAddr += iCountByte;
						ReadAddr += 2;
					}
					ReadAddr = BEGINADDR;
					iCount2 = iCount1;
				}
				if (j == m)
				{
					gl_iEthnetRecBufPE = FlashPointerAdd(gl_iEthnetRecBufPE,(iCount1 * 2));
					gl_cRemainderBit = 16;
				}
				else
					gl_cRemainderBit = j - m;
			}
			else if ((j + 16) >= m)
			{
				iCount2 = EthnetBufLength - gl_iEthnetRecBufPE;
				iCount2 >>= 1;
				ReadAddr = gl_iEthnetRecBufPE + BEGINADDR;
				iEthnetRecBufPE = FlashPointerAdd(gl_iEthnetRecBufPE,(iCount1 * 2));
				iCount3 = EthnetBufLength - iEthnetRecBufPE;
				iCount3 >>= 1;
				ReadAddr1 = iEthnetRecBufPE + BEGINADDR;
				
				if (iCount3 < iCount2)
					iCountBak = iCount3;
				else
					iCountBak = iCount2;
				if (iCountBak > iCount1)
					iCountBak = iCount1;
				while (iCount < iCount1)
				{
					for (;iCount<iCountBak;iCount++)
					{
						sdat1 = Readsdram(ReadAddr, 16);
						sdat2 = sdat1 >> gl_cRemainderBit;
						idat1 = (unsigned int)sdat2;
						idat1 = idat1 << cMultiDotLine[n];
						sdat2 = Readsdram(ReadAddr1, 16);
						idat2 = (unsigned int)sdat2;
						idat2 <<= j;
						idat1 |= idat2;
						//idat1 &= iCountBit;
						sdat1 = (unsigned short)(idat1 & 0xffff);
						sdat2 = (unsigned short)((idat1 >> 16) & iCountBit);
						Writesdram(iWriteAddr,sdat1);
						iWriteAddr += 2;
						Writesdram(iWriteAddr,sdat2);
						iWriteAddr += iCountByte;
						ReadAddr += 2;
						ReadAddr1 += 2;
					}
					if (iCountBak == iCount3)
					{
						ReadAddr1 = BEGINADDR;
						if ((iCount2 > iCountBak) && (iCount2 < iCount1))
							iCountBak = iCount2;
						else
							iCountBak = iCount1;
					}
					else if (iCountBak == iCount2)
					{
						ReadAddr = BEGINADDR;
						if ((iCount3 > iCountBak) && (iCount3 < iCount1))
							iCountBak = iCount3;
						else
							iCountBak = iCount1;
					}
					else
						break;
				}
				if ((j + 16) == m)
				{
					gl_iEthnetRecBufPE = FlashPointerAdd(gl_iEthnetRecBufPE,(iCount1 * 4));
					gl_cRemainderBit = 16;
				}
				else
				{
					gl_iEthnetRecBufPE = FlashPointerAdd(gl_iEthnetRecBufPE,(iCount1 * 2));
					gl_cRemainderBit = j + 16 - m;
				}
			}
			else
			{
				iCount2 = EthnetBufLength - gl_iEthnetRecBufPE;
				iCount2 >>= 1;
				ReadAddr = gl_iEthnetRecBufPE + BEGINADDR;
				iEthnetRecBufPE = FlashPointerAdd(gl_iEthnetRecBufPE,(iCount1 * 2));
				iCount3 = EthnetBufLength - iEthnetRecBufPE;
				iCount3 >>= 1;
				ReadAddr1 = iEthnetRecBufPE + BEGINADDR;
				iEthnetRecBufPE = FlashPointerAdd(gl_iEthnetRecBufPE,(iCount1 * 4));
				iCount4 = EthnetBufLength - iEthnetRecBufPE;
				iCount4 >>= 1;
				ReadAddr2 = iEthnetRecBufPE + BEGINADDR;
				if ((iCount2 <= iCount3) && (iCount2 <= iCount4))
					iCountBak = iCount2;
				else if ((iCount3 <= iCount2) && (iCount3 <= iCount4))
					iCountBak = iCount3;
				else
					iCountBak = iCount4;
				if (iCountBak > iCount1)
					iCountBak = iCount1;
				while (iCount < iCount1)
				{
					for (;iCount<iCountBak;iCount++)
					{
						sdat1 = Readsdram(ReadAddr, 16);
						sdat2 = sdat1 >> gl_cRemainderBit;
						idat1 = (unsigned int)sdat2;
						idat1 = idat1 << cMultiDotLine[n];
						sdat2 = Readsdram(ReadAddr1, 16);
						idat2 = (unsigned int)sdat2;
						sdat2 = Readsdram(ReadAddr2, 16);
						idat2 |= ((unsigned int)sdat2 << 16);
						idat2 <<= j;
						idat1 |= idat2;
						//sdat2 = Readsdram(ReadAddr2);
						//idat2 = (unsigned int)sdat2;
						//idat2 <<= (j + 16);
						//idat1 |= idat2;
						//idat1 &= iCountBit;
						sdat1 = (unsigned short)(idat1 & 0xffff);
						sdat2 = (unsigned short)((idat1 >> 16) & iCountBit);
						Writesdram(iWriteAddr,sdat1);
						iWriteAddr += 2;
						Writesdram(iWriteAddr,sdat2);
						iWriteAddr += iCountByte;
						ReadAddr += 2;
						ReadAddr1 += 2;
						ReadAddr2 += 2;
					}
					if (iCountBak == iCount4)
					{
						ReadAddr2 = BEGINADDR;
						if (iCount3 > iCountBak)
						{
							if (iCount2 > iCountBak)
							{
								if (iCount3 <= iCount2)
									iCountBak = iCount3;
								else
									iCountBak = iCount2;
							}
							else
								iCountBak = iCount3;
						}
						else if (iCount2 > iCountBak)
							iCountBak = iCount2;
						else
							iCountBak = iCount1;
					}
					else if (iCountBak == iCount3)
					{
						ReadAddr1 = BEGINADDR;
						if (iCount4 > iCountBak)
						{
							if (iCount2 > iCountBak)
							{
								if (iCount4 <= iCount2)
									iCountBak = iCount4;
								else
									iCountBak = iCount2;
							}
							else
								iCountBak = iCount4;
						}
						else if (iCount2 > iCountBak)
							iCountBak = iCount2;
						else
							iCountBak = iCount1;
					}
					else if (iCountBak == iCount2)
					{
						ReadAddr = BEGINADDR;
						if (iCount4 > iCountBak)
						{
							if (iCount3 > iCountBak)
							{
								if (iCount4 <= iCount3)
									iCountBak = iCount4;
								else
									iCountBak = iCount3;
							}
							else
								iCountBak = iCount4;
						}
						else if (iCount3 > iCountBak)
							iCountBak = iCount3;
						else
							iCountBak = iCount1;
					}
					else
						break;
					if (iCountBak > iCount1)
						iCountBak = iCount1;
				}
				gl_iEthnetRecBufPE = FlashPointerAdd(gl_iEthnetRecBufPE,(iCount1 * 4));
				gl_cRemainderBit = j + 32 - m;
			}
			cMultiDotLine[n] = 0;
		}
		iWriteAddr -= 24;
//		idat1 = AdditionDot + MotorSideDot;
//		for (iCount=0;iCount<idat1;iCount++)
//		{//���߸��ӵ�
//			for (j=0;j<14;j++)
//			{
//				Writesdram(iWriteAddr,0xaaaa);
//				iWriteAddr += 2;
//			}
//			iWriteAddr += (iCountByte - 26);
//		}
		for (i=n;i<(gl_cSprinklerCount-1);i++)
		{
			for (iCount=0;iCount<iYDotSpace[i];iCount++)
			{
				for (j=0;j<14;j++)
				{
					Writesdram(iWriteAddr,0);
					iWriteAddr += 2;
				}
				iWriteAddr += (iCountByte - 26);
			}
		}
	}
	//Printf("PrendAdd:[0x%x]\n",iWriteAddr);
}

extern void RunCutMod(void);

//�����յ�������ת���ɴ�ӡ��ʽ
void InvertPrintData(void)
{
	unsigned int iWriteAddr;

	//���ڽ����и�����
	if( gl_cCutFileSendFlag == 0x01)
	{
		Printf("InvertPrintData is cut data....\n");
		Printf(" gl_cPrintStatus:%d, gl_cCurPrintBuf:%d\n", gl_cCurPrintBuf, gl_cCurPrintBuf);
		return;
	}
	if (gl_cPrintStatus != 2)
	{// ��ӡû�н���
		if (gl_cCurPrintBuf == 0)
		{// ������0
			if((gl_cInvertBufStatus&PrintBit) != 0)
			{// ׼��������0
				gl_cInvertBufStatus &= (~Buf1Presetbit);
				gl_cInvertBufStatus &= (~PrintBit);
			}
		}
		else
		{// ������1
			if ((gl_cInvertBufStatus&PrintBit) == 0)
			{// ׼��������1
				gl_cInvertBufStatus &= (~Buf0Presetbit);
				gl_cInvertBufStatus |= PrintBit;
			}
		}
	}
	if (gl_cPrintStatus == 0)
	{//��ǰ�д�ӡ����
	
		if (gl_cCurPrintBuf == 0)
		{ //������0
			if ((gl_cInvertBufStatus&Buf0Presetbit) != 0)
			{
				gl_iReadDataAddr = InvertDataAddr0; //���ݿ鿪ʼ��ַ
				gl_iDataEndAddr = gl_iDataEndAddr0;	//���ݿ������ַ 
				gl_cPrintStatus = 1;		//������ӡ
				//Printf("DataA-1-S\n");
			}
			else if ((gl_cFileReceive & 0x20) == 0x20)
			{	// ��ӡ����
				gl_cFileReceive = 0;
				gl_cInvertBufStatus = 0;
				gl_cCommunicationMode = 0xff;
				return;
			}
		}
		else
		{ //������1
			if ((gl_cInvertBufStatus&Buf1Presetbit) != 0)
			{
				gl_iReadDataAddr = InvertDataAddr1;
				gl_iDataEndAddr = gl_iDataEndAddr1;
				gl_cPrintStatus = 1;
				//Printf("DataA-2-S\n");
			}
			else if ((gl_cFileReceive & 0x20) == 0x20)
			{//Printf("Print End 0\n");
				if (ReadPrintEnd() == 0)
					return;
				else
				{
					gl_cFileReceive = 0;
					gl_cCurPrintBuf = 0;
					gl_cInvertBufStatus = 0;
					gl_cCommunicationMode = 0xff;
					if (gl_iPulseNumY < 0)
					{//Printf("Print End 1\n");
						ClrFIFO;
						DriveSprinkler(0,gl_iPulseNumY);//����Y��
						gl_iAxisYPos += gl_iPulseNumY;
						gl_iPulseNumY = -gl_iPulseNumY;
					}
					//Printf("Print End 1\n");
					return;
				}
			}
		}
	}
	
	if ((gl_cFileReceive&0x20) == 0x20)
	{
		//Printf("Print End 3\n");
		if( gl_cCutFileSendFlag == 0x02)
		{// 20100909  �����и����
			Printf("Print end run CUT.....\n");
			RunCutMod();
		}
		return;
	}
	if ((gl_cFileReceive&0x40) == 0)
	{
		if (CalculateRecDatNum() == 0)
			return;		//δ������һ�������ֽ�
	}
		
	if ((gl_cInvertBufStatus&PrepData) != 0)
	{//��ǰ׼����ӡ�����ڴ�ӡ���ǻ�����1�е�����
		if ((gl_cInvertBufStatus&Buf1Presetbit) == 0)
			iWriteAddr = InvertDataAddr1;//������1������δ׼����
		else
		{
			if (gl_cPrintStatus == 0x10)
			{
				if (gl_cFileReceive != 0)
				{
					gl_iEthnetRecBufPH = 0;
					gl_iEthnetRecBufPE = 0;
					gl_cFileReceive = 0;
					gl_cCommunicationMode = 0xff;
					gl_cInvertBufStatus = 0;
					gl_cCurPrintBuf = 0;
					gl_cRemainderBit = 16;
					gl_cCountFlag = 0;
					lcd_display(0,"��������");
					lcd_display(1,"7:�˳�");
					WaitKeyExit(Key_Pause);
					lcd_refresh();
					Printf("driver error!gl_cFileReceive1 != 0\n");
				}
			}
			return;
		}
	}
	else
	{//��ǰ׼����ӡ�����ڴ�ӡ���ǻ�����0�е�����
		if ((gl_cInvertBufStatus&Buf0Presetbit) == 0)
			iWriteAddr = InvertDataAddr0;//������0������δ׼����
		else
		{
			if (gl_cPrintStatus == 0x10)
			{
				if (gl_cFileReceive != 0)
				{
					gl_iEthnetRecBufPH = 0;
					gl_iEthnetRecBufPE = 0;
					gl_cFileReceive = 0;
					gl_cCommunicationMode = 0xff;
					gl_cInvertBufStatus = 0;
					gl_cCurPrintBuf = 0;
					gl_cRemainderBit = 16;
					gl_cCountFlag = 0;
					lcd_display(0,"��������");
					lcd_display(1,"7:�˳�");
					WaitKeyExit(Key_Pause);
					lcd_refresh();
					Printf("driver error!gl_cFileReceive2 != 0\n");
				}
			}
			return;
		}
	}
	if (iWriteAddr == InvertDataAddr0)
	{
		InvertDataForward(iWriteAddr);
		gl_cInvertBufStatus |= Buf0Presetbit;
		gl_cInvertBufStatus |= PrepData;
	}
	else
	{
		if (gl_cBidirectional == 1)
			InvertDataBackward(iWriteAddr);//����
		else
			InvertDataForward(iWriteAddr);
		gl_cInvertBufStatus |= Buf1Presetbit;
		gl_cInvertBufStatus &= (~PrepData);
	}
	if ((gl_cFileReceive&0x40) == 0x40)
	{
		gl_cFileReceive |= 0x20;
		gl_iEthnetRecBufPE = gl_iEthnetRecBufPH;
		gl_cRemainderBit = 16;
	}
}
//ָ��ƫ�Ƽ���
unsigned int FlashPointerAdd(unsigned int pointer,unsigned int length)
{
	pointer += length;
	if (pointer >= EthnetBufLength)
		pointer = pointer - EthnetBufLength;
	return pointer;
}
//�����ѽ������ݳ���
//
unsigned int CalculateRecDatNum(void)
{
	unsigned int iNullSize,iNullSize1;
	unsigned long address;
	unsigned int i;
	
	if (gl_iEthnetRecBufPH == gl_iEthnetRecBufPE)
	{
		if ((gl_cFileReceive & 0x80) != 0)	//�ļ�������
		{
			gl_cFileReceive |= 0x60;
			gl_cRemainderBit = 16;
		}
		return 0;
	}
	else if (gl_iEthnetRecBufPH > gl_iEthnetRecBufPE)
		iNullSize = gl_iEthnetRecBufPH - gl_iEthnetRecBufPE;
	else
		iNullSize = EthnetBufLength - (gl_iEthnetRecBufPE - gl_iEthnetRecBufPH);
	
	iNullSize1 = (iNullSize / (gl_iGraphWidth * 16)) * 16 + gl_cRemainderBit - 16;
	
	if (iNullSize1 < gl_iLineDataNum)
	{
		if ((gl_cFileReceive & 0x80) != 0)	//�ļ�������
		{
			if (gl_cRemainderBit < 16)
				iNullSize1 = ((gl_iLineDataNum - gl_cRemainderBit + 15) / 16  + 1) * gl_iGraphWidth * 16 - iNullSize ;
			else
				iNullSize1 = (gl_iLineDataNum + 15) / 16 * gl_iGraphWidth * 16 - iNullSize;
			if ((gl_iEthnetRecBufPH & 0x01) == 1)
			{
				if ((gl_iEthnetRecBufPH+1) == EthnetBufLength)
					gl_iEthnetRecBufPH = 0;
				else
					gl_iEthnetRecBufPH++;
				iNullSize1--;
			}
			address = gl_iEthnetRecBufPH + BEGINADDR;
			for (i=0;i<iNullSize1;i++)
			{
				Writesdram(address,0x0);
				if ((gl_iEthnetRecBufPH+2) == EthnetBufLength)
				{
					gl_iEthnetRecBufPH = 0;
					address = BEGINADDR;
				}
				else
				{
					gl_iEthnetRecBufPH += 2;
					address += 2;
				}
			}
			gl_cFileReceive |= 0x40;
			return 1;
		}
		else
			return 0;
	}
	else
		return 1;
}
//��ͷ��X��Y����������ֽ���
//cPtintCount=2ʱ��ӡ��ֽ���
//cPtintCount=1ʱ��ӡ��ͷ��X��Y�������
unsigned short SparyError(unsigned char cPtintCount)
{
	unsigned char m,n,j,k;
	unsigned char cSendPosFlag;
	unsigned int address;
	unsigned int iYDotSpace[3];
	unsigned char cMultiDotLine[4];
	int i;
	int iPosNumY;
	int iTotalPixel;
	int sdat[4];
	int sdat1[4];
	int iGapPixel;
	unsigned int iSendDat[23] = {0x3fffff,0x3ffffe,0x3ffffc,0x3ffff8,0x3ffff0,0x3fffe0,0x3fffc0,0x3fff80,0x3fff00,
																				0x3ffe00,0x3ffc00,0x3ff800,0x3ff000,0x3fe000,0x3fc000,0x3f8000,0x3f0000,
																				0x3e0000,0x3c0000,0x380000,0x300000,0x200000,0x000000};
	
	cMultiDotLine[0] = 0;
	switch (gl_cSprinklerCount)
	{
		case 2:
			if (gl_cSprinklerNo == 0)
			{
				cMultiDotLine[1] = gl_cMultiDotLine12;
				iYDotSpace[0] = gl_iYDotSpace12;
			}
			else if (gl_cSprinklerNo == 1)
			{
				cMultiDotLine[1] = gl_cMultiDotLine23;
				iYDotSpace[0] = gl_iYDotSpace23;
			}
			else
			{
				cMultiDotLine[1] = gl_cMultiDotLine34;
				iYDotSpace[0] = gl_iYDotSpace34;
			}
			break;
		case 3:
			if (gl_cSprinklerNo == 0)
			{
				cMultiDotLine[1] = gl_cMultiDotLine12;
				cMultiDotLine[2] = gl_cMultiDotLine23;
				iYDotSpace[0] = gl_iYDotSpace12;
				iYDotSpace[1] = gl_iYDotSpace23;
			}
			else
			{
				cMultiDotLine[1] = gl_cMultiDotLine23;
				cMultiDotLine[2] = gl_cMultiDotLine34;
				iYDotSpace[0] = gl_iYDotSpace23;
				iYDotSpace[1] = gl_iYDotSpace34;
			}
			break;
		case 4:
			cMultiDotLine[1] = gl_cMultiDotLine12;
			cMultiDotLine[2] = gl_cMultiDotLine23;
			cMultiDotLine[3] = gl_cMultiDotLine34;
			iYDotSpace[0] = gl_iYDotSpace12;
			iYDotSpace[1] = gl_iYDotSpace23;
			iYDotSpace[2] = gl_iYDotSpace34;
			break;
		default:
			gl_cSprinklerCount = 1;
			return 0;
	}
	//����Y�����峤��
	iGapPixel = (int)(1000.0 * Y_PPMM / DotPulseNum);
	iTotalPixel = iGapPixel + 2 * AdditionDot + OriginalSideDot + MotorSideDot;//1��
	for (m=0;m<(gl_cSprinklerCount-1);m++)
		iTotalPixel = iTotalPixel + iYDotSpace[m];
	iPosNumY = (iTotalPixel + 320) * DotPulseNum;
	//�ж�Y���Ƿ�ᳬ��
	LineReverse(0x80);		//�巴��
	if (((gl_iAxisYPos + iPosNumY) < 0) || ((gl_iAxisYPos + iPosNumY) > gl_iMaxYMM))
		return 1;
	//ˢ����ʾ
	lcd_display(0,"���ڴ�ӡ");
	lcd_display(1,"���Ժ�...");
	while (ReadPrintEnd() == 0);
	WriteFilePrint(0xaa);
	ClrFIFO;		//��FIFO������
	while (cPtintCount != 0)
	{
		sdat[0] = AdditionDot + OriginalSideDot;
		sdat1[0] = AdditionDot + iGapPixel + OriginalSideDot;
		for (m=1;m<gl_cSprinklerCount;m++)
		{
			sdat[m] = sdat[m-1] + iYDotSpace[m-1];
			sdat1[m] = sdat1[m-1] + iYDotSpace[m-1];
		}
		i = 0;
		cSendPosFlag = 0;
		while (1)
		{
			if (i >= iTotalPixel)
				break;
			while (ReadFIFOFull() != 0)	//������
			{
				if (cSendPosFlag == 0)
				{
					DriveSprinkler(0,iPosNumY);
					gl_iAxisYPos += iPosNumY;
					iPosNumY = -iPosNumY;
					cSendPosFlag = 1;
				}
				else if (ReadPrintEnd() != 0)
					break;
			}
			if ((cSendPosFlag == 1) && (ReadPrintEnd() != 0))
				break;
			address = gl_iWriteFIFOAddr;
			for (m=0;m<gl_cSprinklerCount;m++)
			{
				if ((i >= sdat1[m]) || (i < sdat[m]))
				{
					for (j=0;j<7;j++)
					{
						WriteDL16;
						WriteFIFO(address,0);
						WriteDH16;
						WriteFIFO(address,0);
						WriteDL16;
						address += 4;
					}
				}
				else if (i < (sdat[m] + gl_cLineWidth))
				{
					k = cMultiDotLine[m];
					for (j=0;j<7;j++)
					{
						if ((j == 0) && (j == 6))
							n = 20;
						else
							n = 22;
						if (k >= n)
						{
							WriteDL16;
							WriteFIFO(address,0);
							WriteDH16;
							WriteFIFO(address,0);
							WriteDL16;
							address += 4;
							k -= n;
						}
						else
						{
							WriteDL16;
							WriteFIFO(address,(unsigned short)iSendDat[k]);
							WriteDH16;
							WriteFIFO(address,(unsigned short)(iSendDat[k] >> 16));
							WriteDL16;
							address += 4;
							k = 0;
						}
					}
				}
				else
				{
					sdat[m] += 119;
					if (sdat[m] > sdat1[m])
						sdat[m] -= 119;
					for (j=0;j<7;j++)
					{
						WriteDL16;
						WriteFIFO(address,0);
						WriteDH16;
						WriteFIFO(address,0);
						WriteDL16;
						address += 4;
					}
				}
			}
			i++;
		}
		while (ReadPrintEnd() == 0);
		ClrFIFO;		//��FIFO������
		Delay(1000);
		cPtintCount--;
		if (cPtintCount == 0)
			i = gl_iPulseNumX + (int)(-10 * X_PPMM);
		else
			i = gl_iPulseNumX;
		DriveSprinkler(i,iPosNumY);
		gl_iAxisYPos += iPosNumY;
		iPosNumY = -iPosNumY;
		while (ReadPrintEnd() == 0);
	}
	return 0;
}

//��ӡ˫���ӡʱ���Ҵ�λͼ��
//iGraphWidth:ͼ�ο��
unsigned short DrawMalposedGraph(unsigned int iGraphWidth)
{
	unsigned char m,j;
	unsigned char cSendPosFlag;
	unsigned int address;
	unsigned int iYDotSpace[3];
	int i;
	int iPosNumY;
	int iTotalPixel;
	int sdat[4];
	int sdat1[4];
	
	switch (gl_cSprinklerCount)
	{
		case 2:
			if (gl_cSprinklerNo == 0)
				iYDotSpace[0] = gl_iYDotSpace12;
			else if (gl_cSprinklerNo == 1)
				iYDotSpace[0] = gl_iYDotSpace23;
			else
				iYDotSpace[0] = gl_iYDotSpace34;
			break;
		case 3:
			if (gl_cSprinklerNo == 0)
			{
				iYDotSpace[0] = gl_iYDotSpace12;
				iYDotSpace[1] = gl_iYDotSpace23;
			}
			else
			{
				iYDotSpace[0] = gl_iYDotSpace23;
				iYDotSpace[1] = gl_iYDotSpace34;
			}
			break;
		case 4:
			iYDotSpace[0] = gl_iYDotSpace12;
			iYDotSpace[1] = gl_iYDotSpace23;
			iYDotSpace[2] = gl_iYDotSpace34;
			break;
		default:
			gl_cSprinklerCount = 1;
			break;
	}
	//����Y�����峤��
	iTotalPixel = iGraphWidth * 8 + 2 * AdditionDot + OriginalSideDot + MotorSideDot;
	for (m=0;m<(gl_cSprinklerCount-1);m++)
		iTotalPixel = iTotalPixel + iYDotSpace[m];
	iPosNumY = (iTotalPixel + 320) * DotPulseNum;
	//�ж�Y���Ƿ�ᳬ��
	LineReverse(0x80);		//�巴��
	if (((gl_iAxisYPos + iPosNumY) < 0) || ((gl_iAxisYPos + iPosNumY) > gl_iMaxYMM))
		return 1;
	//ˢ����ʾ
	lcd_display(0,"��λͼ�δ�ӡ");
	lcd_display(1,"���Ժ�...");
	
	while (ReadPrintEnd() == 0);
	WriteFilePrint(0xaa);
	ClrFIFO;		//��FIFO������
	sdat[0] = AdditionDot + OriginalSideDot;
	sdat1[0] = AdditionDot + OriginalSideDot + iGraphWidth * 8;
	for (m=1;m<gl_cSprinklerCount;m++)
	{
		sdat[m] = sdat[m-1] + iYDotSpace[m-1];
		sdat1[m] = sdat1[m-1] + iYDotSpace[m-1];
	}
	i = 0;
	cSendPosFlag = 0;
	while (1)
	{
		if (i >= iTotalPixel)
			break;
		while (ReadFIFOFull() != 0)	//������
		{
			if (cSendPosFlag == 0)
			{
				DriveSprinkler(0,iPosNumY);
				gl_iAxisYPos += iPosNumY;
				iPosNumY = -iPosNumY;
				cSendPosFlag = 1;
			}
			else if(ReadPrintEnd() != 0)
				break;
		}
		if ((cSendPosFlag == 1) && (ReadPrintEnd() != 0))
			break;
		address = gl_iWriteFIFOAddr;
		for (m=0;m<gl_cSprinklerCount;m++)
		{
			if ((i >= sdat1[m]) || (i < sdat[m]))
			{
				for (j=0;j<7;j++)
				{
					WriteDL16;
					WriteFIFO(address,0);
					WriteDH16;
					WriteFIFO(address,0);
					WriteDL16;
					address += 4;
				}
			}
			else if (i < (sdat[m] + gl_cLineWidth))
			{
				for (j=0;j<7;j++)
				{
					WriteDL16;
					WriteFIFO(address,0xffff);
					WriteDH16;
					WriteFIFO(address,0xffff);
					WriteDL16;
					address += 4;
				}
			}
			else
			{
				sdat[m] += 119;
				if (sdat[m] >= sdat1[m])
					sdat[m] -= 119;
				for (j=0;j<7;j++)
				{
					WriteDL16;
					WriteFIFO(address,0);
					WriteDH16;
					WriteFIFO(address,0);
					WriteDL16;
					address += 4;
				}
			}
		}
		i++;
	}
	while (ReadPrintEnd() == 0);
	ClrFIFO;		//��FIFO������
	iTotalPixel = -(unsigned int)(((float)SprinklerWidth * DotLineWidth) * X_PPMM / 2.0);
	DriveSprinkler(iTotalPixel,0);
	while (ReadPrintEnd() == 0);
	cSendPosFlag = 0;
	sdat[0] += gl_iStaggerDot;
	sdat1[0] = AdditionDot + gl_iStaggerDot + OriginalSideDot;
	for (m=1;m<gl_cSprinklerCount;m++)
	{
		sdat[m] += gl_iStaggerDot;
		sdat1[m] = sdat1[m-1] + iYDotSpace[m-1];
	}
	while (1)
	{
		if (i <= 0)
			break;
		
		while (ReadFIFOFull() != 0)	//������
		{
			if (cSendPosFlag == 0)
			{
				DriveSprinkler(0,iPosNumY);
				gl_iAxisYPos += iPosNumY;
				iPosNumY = -iPosNumY;
				cSendPosFlag = 1;
			}
			else if(ReadPrintEnd() != 0)
				break;
		}
		if ((cSendPosFlag == 1) && (ReadPrintEnd() != 0))
			break;
		address = gl_iWriteFIFOAddr;
		for (m=0;m<gl_cSprinklerCount;m++)
		{
			if ((i <= sdat1[m]) || (i > (sdat[m] + gl_cLineWidth)) || (i > (sdat1[m] + iGraphWidth * 8)))
			{
				for (j=0;j<7;j++)
				{
					WriteDL16;
					WriteFIFO(address,0);
					WriteDH16;
					WriteFIFO(address,0);
					WriteDL16;
					address += 4;
				}
			}
			else if (i <= sdat[m])
			{
				sdat[m] -= 119;
				for (j=0;j<7;j++)
				{
					WriteDL16;
					WriteFIFO(address,0);
					WriteDH16;
					WriteFIFO(address,0);
					WriteDL16;
					address += 4;
				}
			}
			else
			{
				for (j=0;j<7;j++)
				{
					WriteDL16;
					WriteFIFO(address,0xffff);
					WriteDH16;
					WriteFIFO(address,0xffff);
					WriteDL16;
					address += 4;
				}
			}
		}
		i--;
	}
	while (ReadPrintEnd() == 0);
	iTotalPixel = gl_iPulseNumX + (int)(-10 * X_PPMM);
	DriveSprinkler(iTotalPixel,0);
	while (ReadPrintEnd() == 0);
	return 0;
}

void ClearPrintHead(unsigned int iFlag)
{
	return ;
	/*
	int iTemp;
	unsigned int address;
	unsigned char cTemp1,cTemp2,cTemp3,cTemp4;
	unsigned int sdat[5]={0xffff,0,0,0,0}; 
	Printf("ClearPrintHead....\n");
	if (iFlag == 1)
		iTemp = gl_iAxisYPos - (OriginalSideDot + 160) * DotPulseNum;
	else
		iTemp = gl_iAxisYPos;
		
	if (gl_iAxisYPos != 0)
	{
		DriveSprinkler(0,-gl_iAxisYPos);
		while (ReadPrintEnd() == 0);//�ȴ���ӡ����
	}
	WriteFilePrint(0xaa);
	ClrFIFO;
	WriteDL16;
	for (cTemp4=0;cTemp4<gl_cSprinklerCount;cTemp4++)
	{
		for (cTemp1=0;cTemp1<40;cTemp1++)
		{
			address = gl_iWriteFIFOAddr;
			for (cTemp2=0;cTemp2<gl_cSprinklerCount;cTemp2++)
			{
				for (cTemp3=0;cTemp3<7;cTemp3++)
				{
					WriteFIFO(address,sdat[cTemp2]);
					WriteDH16;
					WriteFIFO(address,sdat[cTemp2]);
					WriteDL16;
					address += 4;
				}
			}
		}
		sdat[4] = sdat[gl_cSprinklerCount-1];
		for (cTemp2=gl_cSprinklerCount-1;cTemp2>0;cTemp2--)
			sdat[cTemp2] = sdat[cTemp2-1];
		sdat[cTemp2] = sdat[4];
	}
	gl_iAxisYPos = iTemp;
	if (gl_iAxisYPos >= (40*gl_cSprinklerCount+2*160)*14)
	{
		DriveSprinkler(0,gl_iAxisYPos);
		while (ReadPrintEnd() == 0);//�ȴ���ӡ����
	}
	else
	{
		iTemp = (40*gl_cSprinklerCount + 2 * 160) * 14;
		DriveSprinkler(0,iTemp);
		while (ReadPrintEnd() == 0);//�ȴ���ӡ����
		Delay(1000);//��ʱ100ms
		iTemp -= gl_iAxisYPos;
		DriveSprinkler(0,-iTemp);
		while (ReadPrintEnd() == 0);//�ȴ���ӡ����
	}
	*/
}

	
//д���ݵ�FIFO��д��Ϊֹ
void WriteDataToFIFO(void)
{
	unsigned int dat;
	unsigned char i,j;
	unsigned int address;
	//int k=0;
	//int y=0;
	dat=0;
	i = gl_cSprinklerCount * 7;
	
	while (1)
	{
		if (ReadFIFOFull() != 0)//��������
			break;
		else if (gl_iReadDataAddr >= gl_iDataEndAddr)
		{//�д�ӡ�����Ѿ��������,���д0
			address = gl_iWriteFIFOAddr;
			for (j=0;j<i;j++)
			{
				WriteFIFO(address,0);
				address += 4;
				
			}
			break;
		}
		else
		{//����1�Ŵ�ӡ����
			address = gl_iWriteFIFOAddr;
			for (j=0;j<i;j++)
			{
				dat = Readsdram(gl_iReadDataAddr, 32);
				WriteFIFO(address,dat);
				gl_iReadDataAddr += 4;
				address += 4;
				//y++;
				//if(dat!=0)
				//	Printf("[0x%x]",dat);
			}
		}
		//k++;
	}
	//if(k>0)
	//	Printf("KR[0x%x]\n",gl_iReadDataAddr);
}

//20100223
//static char SendPaperClose = 0;
//static char SendDelayTime = 0;
//static char SendPaperStar = 1;

//��ӡ����
void DealPrint(void)
{
	switch (gl_cPrintStatus)
	{
		case	0:
			if ((gl_cDelayFlag == 1) && (ReadPrintEnd() != 0))
				gl_iWaitCount++;
			break;
		case	1:		//׼����ʼ��ӡ
			//SelecAxis(AXIS_Y);
			//if (ReadAxisStatus(AXIS_Y) == 0)
			if (ReadPrintEnd() != 0)
			{
				if (gl_cDelayFlag == 1)
				{
					if (gl_iWaitCount < gl_iDelayCount)
					{
						if ((gl_iDelayCount - gl_iWaitCount - gl_iWaitCount1) > 1)
						{
							gl_iWaitCount1++;
							break;
						}
						else
							gl_iDelayCount = gl_iDelayCount - (gl_iDelayCount - gl_iWaitCount) / 4;
					}
					else
						gl_iDelayCount = gl_iDelayCount + (gl_iWaitCount - gl_iDelayCount) * 3 / 4;
					gl_cDelayFlag = 0;
				}
				//Printf("Start Print:Y[%d]\n",gl_iPulseNumY);
				WriteDataToFIFO();	//дFIFO����������,д��Ϊֹ
				DriveSprinkler(0,gl_iPulseNumY);//����Y��
				
				gl_iAxisYPos += gl_iPulseNumY;
				//gl_iCurrentPos_Y = gl_iAxisYPos; //��ǰY���˶�����
				gl_cPrintStatus = 2;
			}
			break;
		case	2:		//Y���ӡ��
			//SelecAxis(AXIS_Y);
			//if (ReadAxisStatus(AXIS_Y) == 0)
			if (ReadPrintEnd() != 0)
			{//Y���ӡ����
				ClrFIFO;//��FIFO������
				if (gl_cBidirectional == 1)
				{//˫���ӡ
					gl_cPrintStatus = 0;//4;
					if ((gl_cDelayFlag & 0x40) == 0)
					{
						gl_cDelayFlag = 1;
						gl_iWaitCount = 0;
						gl_iWaitCount1 = 0;
					}
					DriveSprinkler(gl_iPulseNumX,0);//����X����ֽ
					//Delay(1000);
					//Printf("Drive X Start\n");
					//WaitDriveEnd(AXIS_X);
					//Printf("X Drive end\n");
					gl_iAxisXPos += gl_iPulseNumX;//��ǰX���˶�����
					gl_iPulseNumY = -gl_iPulseNumY;//����
					//Printf("Move X\n");
				}
				else
				{
					DriveSprinkler(gl_iPulseNumX,0);//����Y���ԭ��
					WaitDriveEnd(AXIS_X);
					DriveSprinkler(0, -gl_iPulseNumY);//
					//WaitDriveEnd(AXIS_Y);
					gl_iAxisYPos -= gl_iPulseNumY;
					gl_iAxisXPos += gl_iPulseNumX;//��ǰX���˶�����
					//gl_iCurrentPos_Y = gl_iAxisYPos; //��ǰY���˶�����
					gl_cPrintStatus = 0;
				}
				if (gl_cCurPrintBuf == 0)
					gl_cCurPrintBuf = 1;//�´�Ӧ��ӡ�Ļ�����1�е�����
				else
					gl_cCurPrintBuf = 0;//�´�Ӧ��ӡ�Ļ�����0�е�����
			}
			else
				WriteDataToFIFO();	//дFIFO����������,д��Ϊֹ
			break;
		case	3:		//���߻�ԭ��
			//SelecAxis(AXIS_Y);
			//if (ReadAxisStatus(AXIS_Y) == 0)
			if (ReadPrintEnd() != 0)
			{//Y�᷵�ش�ӡ����
				Printf("Print end return org!\n");
				/*
				gl_cPrintStatus = 0;
				DriveSprinkler(gl_iPulseNumX,0);//����X����ֽ
				gl_iCurrentPos_X += gl_iPulseNumX;//��ǰX���˶�����
				*/
			}
			break;
		default:
			break;
	}

	if(gl_cPrintStatus==1)
	{
		if ((GetIOBit(I_YUPINDUCTOR) == StatusOk))
		{
			pmove(AXIS_Y, 0);
			while(1);
		}
	
		if ((GetIOBit(I_XRIGHINDUCTOR) == StatusOk))
		{
			pmove(AXIS_X, 0);
			while(1);
		}	
	}
	/*
	//��ֽ���
	if ((gl_cReacterStatus & B_SENDPAPER) != 0)
	{//��������ֽ
		if ((GetIOBit(I_GIVEBACK) == StatusOk) || (gl_iSendPaper >= 625))
		{ //��ֽλ�õ��������ֽ5s

				SendPaperClose=1;
				//201002023 ����
				//SetIOBit(O_STARTGIVEPMOTOR,1);		//�ر���ֽ���
				//gl_cReacterStatus &= (~B_SENDPAPER);
				//gl_iMovePaperLength = 0;

		}
		else if (GetIOBit(I_GIVEPEND) == StatusOk)//ûֽ
		{
			SetIOBit(O_STARTGIVEPMOTOR,1);		//�ر���ֽ���
			gl_cReacterStatus &= (~B_SENDPAPER);
			gl_cReacterStatus |= B_NOPAPER;
		}
		else
			gl_iSendPaper++;
	}
	else if (((gl_cReacterStatus & B_NOPAPER) == 0) && (gl_cSelfCheckFlag == 0))
	{
		//��ֽ����20100223
		if(SendPaperStar == 0)
		if(GetIOBit(I_GIVEBACK) == StatusOk)
		{
			SendPaperStar = 1;
			gl_iMovePaperLength = 0;
		}
		//
		if (GetIOBit(I_GIVEPEND) == StatusOk)//ûֽ
			gl_cReacterStatus |= B_NOPAPER;
		else if (gl_iMovePaperLength < -44659)			//��ֽ����660MM,��ֽ
		{
			if(SendPaperStar)
			if (GetIOBit(I_GIVEBACK) == StatusErr)
			{
				gl_cReacterStatus |= B_SENDPAPER;
				SetIOBit(O_STARTGIVEPMOTOR,0);		//������ֽ���
				gl_iSendPaper = 0;								//��ֽ����������
			}
		}
	}
	//20100223 ��ֽ����,��ʱ�رյ��
	if(SendPaperClose)
	{
		if(SendDelayTime>4)
		{
			SendPaperClose = 0;
			SendDelayTime = 0;
			SetIOBit(O_STARTGIVEPMOTOR,1);		//�ر���ֽ���
			gl_cReacterStatus &= (~B_SENDPAPER);
			gl_iMovePaperLength = 0;
			if(GetIOBit(I_GIVEBACK) != StatusOk)
				SendPaperStar = 0;
		}
		else
			SendDelayTime += 1;
	}
	//
	
	//��ֽ���
	if ((gl_cReacterStatus & B_ROLLPAPER) != 0)
	{//�ѿ�ʼ��ֽ
		if ((GetIOBit(I_ROLLPEND) == StatusOk) || (gl_iRollPaper >= 625))
		{//��ֽͣλ�õ��������ֽ5s
			SetIOBit(O_STARTROLLPMOTOR,1);		//�ر���ֽ���
			gl_cReacterStatus &= (~B_ROLLPAPER);
		}
		else
			gl_iRollPaper++;
	}
	else
	{
		if (GetIOBit(I_ROLLSTART) == StatusOk)//��ֽ����
		{
			if ((GetIOBit(I_ROLLPEND) == StatusErr) && (gl_cSelfCheckFlag == 0))
			{
				gl_cReacterStatus |= B_ROLLPAPER;
				SetIOBit(O_STARTROLLPMOTOR,0);		//������ֽ���
				gl_iRollPaper = 0;								//��ֽ����������
			}
		}
	}
	*/
}
