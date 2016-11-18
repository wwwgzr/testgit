#include "..\src\defconst.h"
#include "..\Target\44blib.h"
#include "..\src\extern.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

//#define 	AXIS_X		2							//X轴
//#define 	AXIS_Y		1							//Y轴

void DriveSprinkler(int iPulseNumX,int iPulseNumY);
void InvertPrintData(void);
//void DrawTestGraph(int dotPix);
void InitPaper(void);
void SetIOBit(unsigned char cBit,unsigned char cEnable);
void DealPrint(void);
void WriteDataToFIFO(void);
void InvertDataBackward(unsigned int iWriteAddr);
void InvertDataForward(unsigned int iWriteAddr);
void ClearPrintHead(char cOrgState);


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
//**********20130807 extern define
extern void ContinuousDrive(int axis,long pulse);
extern void DecSpeedState(int axis);
//***********
extern void ITMoveHandpiece(char cAix,int iMovePluse);
//extern void CtrlDownCut();

long gl_TestSendLengPluse=0;

//20100223
static char SendPaperClose = 0;
static char SendDelayTime = 0;

//**********************************************************
//功能：	读取IO位
//输入参数:	cBit要读取的IO位(0~7或0xff)
//返回值: 	1该IO位为1，0该IO位为0
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
//功能：	设置IO位
//输入参数:	cBit  要设置的IO位(0~7)
//					cEnable  1将IO位设置位1,0将IO位设置为0
//返回值：无
//********************************************************
void SetIOBit(unsigned char cBit,unsigned char cEnable)
{
	if (cEnable == 0)
		gl_cIOWriteValue &= ~cBit;
	else
		gl_cIOWriteValue |= cBit;
	WriteIO(gl_cIOWriteValue);
}

// 更新加速表
// 要调试，加大初始速度
// 更新加速表
// X轴速度设定 SV:15 V:150 T:600  最大加速步 26
// Y轴速度设定 SV:10 V:500 T:200
int UpdataAccTable(int iInitV, int iPenSpeed, int iAccTime, unsigned int *iStepSpeed, unsigned char *cMaxStep)
{
	unsigned int  iAddStep;//iMaxSpeed
	double dAccTime,dLength,dSpeedH;
	double dAcc,dAccA;
	double dTemp,dTemp1;
	unsigned char cCount,cTemp;
	int i=0;
	//int iPenSpeed;
	//int iInitV, iAccTime;

	dLength = 2.0;//1.5;//
	dTemp1 = Y_PPMM / (double)SPEED_M;
	//控制笔速在800mm/s以下
	//Printf("V[%d],S[%d],T[%d]\n", iInitV, iPenSpeed, iAccTime);
	//
	dAccTime = (double)(iAccTime) * 0.001;	//T速度加到最大所需的时间 300

	dSpeedH = (double)iPenSpeed * dTemp1;

	dAcc = (double)(iPenSpeed - iInitV) / (dAccTime);//A加速度  20
	
	dAccA = (double)(iPenSpeed - iInitV) - dAcc * dAccTime;
	dAccA = dAccA / (dAccTime * dAccTime);
	dTemp = (double)iInitV;
	
	iStepSpeed[0] = (unsigned int)(dTemp * dTemp1);
	
	cCount = 1;

	// 两毫米一段加速，共150步加速步，
	//Printf("Add Speed Step List:\n[%d],",iStepSpeed[0]);
	while(cCount < 150)
	{
		dTemp = (double)iStepSpeed[cCount-1] / dTemp1;
		dTemp = dTemp * dTemp;
		dTemp +=  2.0 * dAcc * dLength;
		dTemp = sqrt(dTemp) * dTemp1;//Vt速度
		if (dTemp >= dSpeedH)
		{
			dTemp =  dSpeedH;
			iAddStep = cCount;
			iStepSpeed[cCount] = (unsigned int)dTemp;
			break;
		}
		iStepSpeed[cCount] = (unsigned int)dTemp;
		//Printf("[%d],", iStepSpeed[cCount]);
		dTemp = (double)(iStepSpeed[cCount] - iStepSpeed[cCount-1]) / dTemp1 / dAcc;
		dAcc = dAcc + dAccA * dTemp;
		cCount++;
	}
	//Printf("[%d],", iStepSpeed[cCount]);
	//Printf("\nCT[%d] GStep[%d]\n", cCount, iAddStep);
	if (cCount >= 150)
		iAddStep = 149;
	
	//////
	if(iAddStep<60)
	{
		cCount=iAddStep/60+1;
		cMaxStep[0] = cCount;
		i=1;
		for(cTemp=cCount*2;cTemp<iAddStep;)
		{
			cMaxStep[i++] = cTemp;
			cTemp+=cCount;
		}
	}
	else
	{
		cCount=iAddStep/60+1;
		cMaxStep[0] = cCount;
		i=1;
		for(cTemp=cCount*2;cTemp<iAddStep;)
		{
			cMaxStep[i++] = cTemp;
			cTemp+=cCount;
		}
	}
	
	//Printf("i=%d\n", i);
	while(i<61)
	{
		cMaxStep[i++]=iAddStep;
	}
	////Printf("Max Setp:\n");
/*	for (cCount=0;cCount < 61;cCount++)
	{
		Printf("[%d],", cMaxStep[cCount]);
	}
	Printf("\n");	
*/
	//////
	/*
	cCount = 0;
	iMaxSpeed = ;//116;//26mm/s216
	//Printf("Max Step List:\n");
	for(cTemp=0;cTemp < iAddStep;)
	{
		if (iMaxSpeed < iStepSpeed[cTemp])
		{
			if (cTemp == 0)
				cMaxStep[cCount] = 0;
			else
				cMaxStep[cCount] = cTemp - 1;
			//Printf("[%d],", gl_cMaxStep[cCount]);
			cCount++;
			if (cCount < 11)//21
				iMaxSpeed += 85;//15mm/s125
			else if (cCount < 31)//41
				iMaxSpeed += 100;//13mm/s108
			else if (cCount < 51)//61
				iMaxSpeed += 115;//10mm/s83
			else
				return iAddStep;
		}
		else
			cTemp++;
	}
	
	for (;cCount < 61;cCount++)
	{
		cMaxStep[cCount] = cTemp;
		//Printf("[%d],", cMaxStep[cCount]);
	}
	
	for (cCount=0;cCount < 61;cCount++)
	{
		Printf("[%d],", cMaxStep[cCount]);
	}
	*/
	return iAddStep;
	//Printf("\n");
/*
	Printf("Max Speed:\n");
	for(cCount=0; cCount<gl_cStep; cCount++)
	{
		Printf("[%d],", gl_iStepSpeed[gl_cMaxStep[cCount]]*20);
	}
	Printf("\n");
	*/
}
/*
int NewUpdataAccTable(int iInitV, int iPenSpeed, int iAccTime, unsigned int *iStepSpeed, unsigned char *cMaxStep)
{
	unsigned int iMaxSpeed, iAddStep;
	double dAccTime,dLength,dSpeedH;
	double dAcc,dAccA;
	double dTemp,dTemp1;
	unsigned char cCount,cTemp;
	//int iPenSpeed;
	//int iInitV, iAccTime;
	int iStepPluse;
	
	dLength = 2.0;//1.5;//
	dTemp1 = Y_PPMM / (double)SPEED_M;
	//控制笔速在800mm/s以下
	Printf("V[%d],S[%d],T[%d]\n", iInitV, iPenSpeed, iAccTime);
	//
	dAccTime = (double)(iAccTime) * 0.001;	//T速度加到最大所需的时间 300

	dSpeedH = (double)iPenSpeed * dTemp1;

	dAcc = (double)(iPenSpeed - iInitV) / (dAccTime);//A加速度  20
	
	dAccA = (double)(iPenSpeed - iInitV) - dAcc * dAccTime;
	dAccA = dAccA / (dAccTime * dAccTime);
	dTemp = (double)iInitV;
	
	iStepSpeed[0] = (unsigned int)(dTemp * dTemp1);
	
	cCount = 1;

	iStepPluse = (int)((dSpeedH+(double)iStepSpeed[0])/150.0);
		
	// 两毫米一段加速，共150步加速步，
	Printf("Add Speed Step List:\n[%d],",iStepSpeed[0]);
	while(cCount < 150)
	{
		iStepSpeed[cCount] = iStepPluse + iStepSpeed[cCount-1];
		dTemp = iStepSpeed[cCount];
		if(dTemp>dSpeedH)
		{
			iAddStep = cCount;
			break;
		}
		Printf("[%d],", iStepSpeed[cCount]);
		cCount++;
		
	}
	Printf("[%d],", iStepSpeed[cCount-1]);
	Printf("\nCT[%d] GStep[%d]\n", cCount, iAddStep);
	if (cCount >= 150)
		iAddStep = 149;
	
	cCount = 0;
	iMaxSpeed = 100;
	for(cTemp=0;cTemp < iAddStep;)
	{
		if (iMaxSpeed < iStepSpeed[cTemp])
		{
			iMaxSpeed = iStepSpeed[cTemp];
			if (cTemp == 0)
				cMaxStep[cCount] = 0;
			else
				cMaxStep[cCount] = cTemp - 1;
			
			cCount++;
			iMaxSpeed += ((int)((149.0/60.0)*iStepPluse));
		}
		else
			cTemp++;
	}

	for (;cCount < 61;cCount++)
	{
		cMaxStep[cCount] = iAddStep;
		//Printf("[%d],", cMaxStep[cCount]);
	}

	for (cCount=0;cCount < 61;cCount++)
	{
		Printf("[%d],", cMaxStep[cCount]);
	}
	return iAddStep;
}
*/
/*	
//更新加速表
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
	//控制笔速在800mm/s以下
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
	dAccTime = (double)(gl_iATime + 50) * 0.001;	//T速度加到最大所需的时间

	dSpeedH = (double)iPenSpeed * dTemp1;

	dAcc = (double)(iPenSpeed - gl_iInitV0) / (dAccTime + 0.05);//A加速度
	
	dAccA = (double)(iPenSpeed - gl_iInitV0) - dAcc * dAccTime;
	dAccA = dAccA / (dAccTime * dAccTime); //加速度变化率
	
	dTemp = (double)gl_iInitV0;
	if (gl_iInitV0 > 30)
	{
		gl_iStepSpeed[1] = (unsigned int)(dTemp * dTemp1);
		dTemp = dTemp * dTemp;
		dTemp -=  2.0 * dAcc * dLength;
		if (dTemp < 625.0)
		{//计算第一步速度
			gl_iStepSpeed[0] = (unsigned int)(25.0 * dTemp1);
			if (gl_iStepSpeed[0] > gl_iStepSpeed[1])
				gl_iStepSpeed[0] = gl_iStepSpeed[1];
		}
		else
		{//计算第一步速度
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
		dTemp = sqrt(dTemp) * dTemp1;//Vt速度
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
*/
void SetZAxisDevice(int Ispeed, int Dspeed, int Aspeed)
{
	//设置Z初始速度
	WriteZAxisInitSpeed(Ispeed);
	 //设置Z驱动速度
	WriteZAxisDriveSpeed(Dspeed);
	 //设置Z加速度
	WriteZAxisAddSpeed(Aspeed);	
}

//extern void testMcx312(void);
//extern void lp(int axis, long wdata);
//extern void ep(int axis, long wdata);
extern void SetLp(int axis, long wdata);
extern long ReadLp(int axis);


//初始化
//驱动Y回到绘图仪最大值
//驱动X回到绘图仪最大值
// 1, 首先判断是否在原点感应器位置，不在则将Y驱动到最右端，
//如果仍然未感应到感应器，则将X驱动到左下端,直到感应到原点感应器
// 2,判断Z轴回原点，若不在，驱动Z轴回原点；
//*********************20130807 Control.c InitFlatPlotter
void RW_MoveStartPrintPost(void)
{
	long lPluseXX, lPluseYY;
	
	//判断当前位置是否与用户设定位置是否一致，不一致移动到工作位置
	if((gl_lWorkOrig_XXPluse != gl_lCurPost_XPluse) || (gl_lWorkOrig_YYPluse != gl_lCurPost_YPluse))
	{
		lPluseXX = gl_lWorkOrig_XXPluse - gl_lCurPost_XPluse;
		lPluseYY = gl_lWorkOrig_YYPluse - gl_lCurPost_YPluse;
		
		//Printf("W!=C >> M[%d] M[%d] WOX[%d] WOY[%d] CPX[%d] CPY[%d]\n", lPluseXX, lPluseYY, gl_lWorkOrig_XXPluse, gl_lWorkOrig_YYPluse, gl_lCurPost_XPluse, gl_lCurPost_YPluse);
		Printf("MSPP W!=C >> M[%d] M[%d] WOX[%d] WOY[%d] CPX[%d] CPY[%d]\n", lPluseXX, lPluseYY, gl_lWorkOrig_XXPluse, gl_lWorkOrig_YYPluse, gl_lCurPost_XPluse, gl_lCurPost_YPluse);
		SetReturnOrgSpeed();
		//SetPrintSpeed(AXIS_Y+AXIS_X);
		inp_move2(lPluseXX, lPluseYY);
		WaitInpEnd();
		gl_lCurPost_XPluse += lPluseXX;
		gl_lCurPost_YPluse += lPluseYY;
		gl_lWorkOrig_XXPluse = 0;
		gl_lWorkOrig_YYPluse = PenCarSideInterval * DotPulseNum;
		Printf("MSPP W!=C >> WOX[%d] WOY[%d] CPX[%d] CPY[%d]\n",  gl_lWorkOrig_XXPluse, gl_lWorkOrig_YYPluse, gl_lCurPost_XPluse, gl_lCurPost_YPluse);
	
	}
	else{
			gl_lWorkOrig_XXPluse = 0;
			gl_lWorkOrig_YYPluse = PenCarSideInterval * DotPulseNum;
		}
		
}

void SetUAxisInit(int Ispeed, long Dspeed, long Aspeed)
{
	//设置U初始速度
	WriteUAxisInitSpeed(Ispeed);
	 //设置U驱动速度
	WriteUAxisDriveSpeed(Dspeed);
	 //设置U加速度
	WriteUAxisAddSpeed(Aspeed);	
}

void IDT_ZAxisTestInductor(int iPlusLen)
{
	long count;
	
	//Printf("Test Z Axis Inductor Start[%d]\n", iPlusLen);
	WriteZAxisPlus(iPlusLen);
	WriteZAxisDriveCmd(DriveZAxisStartCmd);
	Delay(10);
	while(ZAxisDriveEndState == 0);//等待Z轴驱动结束
	//	count++;
	WriteZAxisPlus(0);
	WriteZAxisDriveCmd(DriveZAxisStopCmd);
	//Printf("Z Test Step 1 end [%d]\n", count);
	/*********************************/
	iPlusLen = iPlusLen*-1;
	//Printf("Z Test Step 2 start[%d] \n", iPlusLen);
	WriteZAxisPlus(iPlusLen);
	WriteZAxisDriveCmd(DriveZAxisStartCmd);
	Delay(10);
	count=0;
	do
	{		 
	 	count++;
	 	if(count>1000000L)
	 		ViewError(ER_ZINDERR);
	 
	}while(GetIOBit(INDUCTOR_ZORG) != Z_INDUCTOR_ORIGIN);//Z感应器未被遮挡
	WriteZAxisPlus(0);
	WriteZAxisDriveCmd(DriveZAxisStopCmd);
	//Printf("Z Test Inductor end \n");
}

// 测试各轴感应器好坏
void ID_TestAxisInductor(int iAxis)
{
	char cEnd=0;
	int iPlusLen, iInductorPosition, iInductorStatus;
	
	if(iAxis != AXIS_Z)
	{
		switch(iAxis)
		{
			case AXIS_X:
				iPlusLen = -(X_PPMM * 10);
				iInductorPosition = INDUCTOR_XRIGHT;
				//RP_SetSingleAxisADSpeed(AXIS_X, 400);
				SingleAxisInpDriveSpeed(AXIS_X,50, 100, 200);
				break;
			
			case AXIS_Y:
				iPlusLen = -(Y_PPMM * 10);
				iInductorPosition = INDUCTOR_YUP;
				//RP_SetSingleAxisADSpeed(AXIS_Y, 400);
				SingleAxisInpDriveSpeed(AXIS_Y,50, 100, 200);
				break;
		}
		
		// 移开感应器原点
		Delay(100);
		pmove(iAxis,iPlusLen);
		while(1)
		{
			do{
				get_status(iAxis, &iInductorStatus);
			}while(iInductorStatus != 0);
			
			if (GetIOBit(iInductorPosition) == XY_INDUCTOR_ORIGIN)
				pmove(iAxis,iPlusLen);
			else
				break;
		}
		Delay(100);
		iPlusLen = (int)(Y_PPMM * 0.1);
		pmove(iAxis,iPlusLen);
		while(1)
		{
			do{
				get_status(iAxis, &iInductorStatus);
				if (GetIOBit(iInductorPosition) == XY_INDUCTOR_ORIGIN)
				{
					cEnd = 1;
					break;
				}
			}while(iInductorStatus != 0);

			if(cEnd == 1)
				break;
			
			pmove(iAxis,iPlusLen);
		}
		//20130830 当离开Y原点感应器后，移动到笔车工作范围,机器类型
		if(iAxis == AXIS_Y)
		{
			//Printf("ID_TestAxisInductor Y\n");
			do{
				get_status(iAxis, &iInductorStatus);
			}while(iInductorStatus != 0);
			Delay(100);
			iPlusLen = gl_lMaxY - ((gl_sMachineWidth * Y_PPMM) + (PenCarSideInterval  * DotPulseNum));
			//Printf("iPlusLen[%d] gl_lMaxY[%d] gl_sMacWidth[%d]\n", -iPlusLen, gl_lMaxY, gl_sMachineWidth);
			SingleAxisInpDriveSpeed(AXIS_Y,50, 200, 200);
			pmove(iAxis,-iPlusLen);
			do{
				get_status(iAxis, &iInductorStatus);
			}while(iInductorStatus != 0);
			gl_lCurPost_YPluse = (gl_lMaxY - iPlusLen);
			gl_lWorkOrig_YYPluse = PenCarSideInterval * DotPulseNum;
			Printf("gl_lWorkOrig_YYPluse[%d], gl_lCurPost_YPluse[%d]\n",gl_lWorkOrig_YYPluse, gl_lCurPost_YPluse);
		}
		
	}
	else{
			iPlusLen = -(Z_PPMM * 50);
			IDT_ZAxisTestInductor(iPlusLen);
		}
	
	return;
}

void Init_ReturnOrigin(int iAxis)
{
	int iPlusLen, iInductorPosition;
	int iInductorStatus;
	long count=0;
	//long LpCount;
	long TestRealPluse;
	if(iAxis != AXIS_Z)
	{
		switch(iAxis)
		{
			case AXIS_X:	//881206 / 600 = 1468mm 146.8cm
				//iPlusLen = (gl_iMacLength_MM + 10) * X_PPMM;
				iPlusLen = (100) * X_PPMM;
				iInductorPosition = INDUCTOR_XRIGHT;
				//Printf("RO X start[%d]Pluse\n", iPlusLen);
				//SetInitAxisSpeed(AXIS_X, 500);
				//SingleAxisInpDriveSpeed(AXIS_X,100, 600, 100);
				//SingleAxisInpDriveSpeed(AXIS_X,10, 40, 200);
				FixedSpeeDrive(AXIS_X, 2200);
				break;
			
			case AXIS_Y:	//833361  818471 / 600 = 1364mm 136.4cm
				//iPlusLen = (gl_iMacWide_MM + 10) * X_PPMM;
				iPlusLen = (100) * Y_PPMM;
				iInductorPosition = INDUCTOR_YUP;
				//Printf("RO Y start [%d]Pluse\n", iPlusLen);
				//SetInitAxisSpeed(AXIS_Y, 500);
				//SingleAxisInpDriveSpeed(AXIS_Y,100, 600, 100);
				//SingleAxisInpDriveSpeed(AXIS_Y,10, 40, 200);
				FixedSpeeDrive(AXIS_Y, 2200);
				break;
		}
		if (GetIOBit(iInductorPosition) == XY_INDUCTOR_ORIGIN)
			return ;
		SetLp(iAxis, 0);
		ContinuousDrive(iAxis,iPlusLen);
		while(1)
		{
			if (GetIOBit(iInductorPosition) == XY_INDUCTOR_ORIGIN)
			{// 当前感应器被挡住
				DecSpeedState(iAxis);
				break;
			}
		}
		
		do{
			get_status(iAxis,&iInductorStatus);
		}while(iInductorStatus != 0);
		TestRealPluse = ReadLp(iAxis);
		Printf("Real pluse[%d]\n", TestRealPluse);
		Delay(100);
	}
	else{			
			SetZAxisDevice(gl_iZInitSpeed, gl_iZInitSpeed, 0);
			if(GetIOBit(INDUCTOR_ZORG) != Z_INDUCTOR_ORIGIN)
			{	Printf("Check org!\n");
				WriteZAxisPlus(-RP_ZAXISLAPPLUS);
				WriteZAxisDriveCmd(DriveZAxisStartCmd);
				Delay(10);
				do
				{		 
	 				count++;
			 		if(count>30000000L)
	 					ViewError(ER_ZINDERR);
				}while(GetIOBit(INDUCTOR_ZORG) != Z_INDUCTOR_ORIGIN);//Z感应器未被遮挡
				WriteZAxisPlus(0);
				WriteZAxisDriveCmd(DriveZAxisStopCmd);
				Delay(100);
				
				Printf("Go 16mm\n");
				WriteZAxisPlus((int)(53*Z_PPMM));
				WriteZAxisDriveCmd(DriveZAxisStartCmd);
				Delay(100);
				count=0;
				while(ZAxisDriveEndState == 0)
					count++;
				WriteZAxisPlus(0);
				WriteZAxisDriveCmd(DriveZAxisStopCmd);
				Printf("CT[%d]\n", count);
			}
			else{	//Printf("Go work!\n");
					WriteZAxisPlus((int)(53*Z_PPMM));
					WriteZAxisDriveCmd(DriveZAxisStartCmd);
					Delay(100);
					while(ZAxisDriveEndState == 0);
					WriteZAxisPlus(0);
					WriteZAxisDriveCmd(DriveZAxisStopCmd);	
				}
		}
		
}
/*
void InitFlatPlotter()
{
	ClrFIFO;
	gl_cIOWriteValue = 0xff;
	WriteIO(gl_cIOWriteValue);	
	
	//测试Z轴是否在感应器原点位置
	Init_ReturnOrigin(AXIS_Z);
	Delay(100);
	gl_cCutUpDownState = 0; 	//刀具收起	 1刀具落下	


	Init_MCX312();
	//测试X轴是否在感应器原点位置
	//Printf("Test X 1 Axis\n");
	if (GetIOBit(INDUCTOR_XRIGHT) != XY_INDUCTOR_ORIGIN)
	{	Printf("Test X 2 Axis\n");
		Init_ReturnOrigin(AXIS_X);
	}
	//Printf("Text X Ok1\n");
	//测试X轴感应器
	ID_TestAxisInductor(AXIS_X);
	gl_lCurPost_XPluse = gl_lMaxX;
	gl_lWorkOrig_XXPluse = 0;	
	//Printf("Text X Ok2\n");

	
	//Printf("Test Y 1 Axis\n");
	//测试Y轴是否在感应器原点位置
	if (GetIOBit(INDUCTOR_YUP) != XY_INDUCTOR_ORIGIN)
	{	//Printf("Test Y 2 Axis\n");
		Init_ReturnOrigin(AXIS_Y);
	}
	//Printf("Text Y Ok1\n");
	//测试Y轴感应器
	ID_TestAxisInductor(AXIS_Y);
	RestMcx312();
	Printf("Text Y Ok2 gl_lCurPost_YPluse[%d]\n", gl_lCurPost_YPluse);

}
*/
void InitPaper(void)
{
	int iPos,iStep;

	Printf("init paper:\n");
	iPos = 0;
	iStep = -2 * (int)X_PPMM;
	FixedSpeeDrive(AXIS_X,1500);
	while (1)
	{
		if (GetIOBit(I_GIVEPEND) == StatusOk)
		{
			if (iPos != 0)
			{
				SoftDelay(1000);
				ITMoveHandpiece(AXIS_X, -iPos);
				//DriveSprinkler(-iPos,0);
				//WaitPrintEnd();//等待打印结束
			}
			SetIOBit(O_GIVEMOTORDIR,G_SENDPAPER);
			SetIOBit(O_STARTGIVEPMOTOR,0);//启动送纸
			//Delay(2000);
			iStep = 0;
			while (1)
			{
				//ReceiveData();				//接收数据
				if ((GetIOBit(I_GIVEBACK) == StatusErr) && (iStep < 5000000))
				{
					iStep++;
					if (GetIOBit(I_GIVEPEND) == StatusOk)//没纸
					{
						SoftDelay(50000);						//20141212 rwt
						if (GetIOBit(I_GIVEPEND) == StatusOk) 
						{
							Printf("I_GIVEPEND\n");
							//gl_cReacterStatus |= B_NOPAPER;
							break;
						}
					}
				}
				else
					break;
			}
			SetIOBit(O_STARTGIVEPMOTOR,1);//停止送纸
			Printf("I_GIVEPEND\n");
			break;
		}
		else if (GetIOBit(I_GIVEBACK) == StatusOk)
		{//不用送纸
			if (iPos != 0)
			{
				ITMoveHandpiece(AXIS_X, -iPos);
				//DriveSprinkler(-iPos,0);
				//WaitPrintEnd();//等待打印结束
			}
			Printf("I_GIVEBACK\n");
			break;
		}
		else
		{
			ITMoveHandpiece(AXIS_X, iStep);
			//DriveSprinkler(iStep,0);
			//WaitPrintEnd();//等待打印结束
			iPos += iStep;
		}
	}
	gl_iMovePaperLength = 0;
}

// 文件开始打印时纸张初始化
void InitFilePaper(void)
{
	int iStep;

	iStep = 0;

	if (GetIOBit(I_GIVEPEND) == StatusOk)
	{
		//Printf("I_GIVEPEND == StatusOk\n");
		SetIOBit(O_GIVEMOTORDIR,G_SENDPAPER);
		SetIOBit(O_STARTGIVEPMOTOR,0);//启动送纸
		while (1)
		{
			if ((GetIOBit(I_GIVEBACK) == StatusErr))// && (iStep < 8000000))
			{
				iStep++;
				if ((GetIOBit(I_GIVEPEND) == StatusOk))//&&(iStep>300000))//没纸
				{
					break;
				}
			}
			else
				break;
		}
		SetIOBit(O_STARTGIVEPMOTOR,1);//停止送纸
	}
	else if (GetIOBit(I_GIVEBACK) != StatusOk)	
	{
		//Printf("I_GIVEPEND != StatusOk\n");
		SetIOBit(O_GIVEMOTORDIR,G_ROLLPAPER);
		SetIOBit(O_STARTGIVEPMOTOR,0);//启动送纸电机收纸
		while (1)
		{
			if ((GetIOBit(I_GIVEPEND) == StatusErr))// && (iStep < 5000000))
			{
				iStep++;
			}
			else 
			{
				break;
			}
			if((GetIOBit(I_GIVEBACK) == StatusOk))
			{
				break;
			}
		}
		SetIOBit(O_STARTGIVEPMOTOR,1);//停止收纸
		
		Delay(1000);
		
		//Printf("ROLLPAPER END [%d]\n", iStep);
		if((GetIOBit(I_GIVEBACK) != StatusOk))
		{
			iStep = 0;
			Printf("Start Send\n");
			SetIOBit(O_GIVEMOTORDIR,G_SENDPAPER);
			SetIOBit(O_STARTGIVEPMOTOR,0);//启动送纸
			while (1)
			{
				if ((GetIOBit(I_GIVEBACK) == StatusErr))// && (iStep < 5000000))
				{
					iStep++;
				}
				else
					break;
			}
			SetIOBit(O_STARTGIVEPMOTOR,1);//停止送纸
			//Printf("Send Stop [%d]\n", iStep);
		}
	}
	
	gl_iMovePaperLength = 0;
	//Printf("Send paper end[%d]\n", iStep);
}

//
void CtrDownCut(void)
{
	SetIOBit(O_CUTDOWN,C_CUTDOWN);
	Delay(gl_iCutDownDelay);
	gl_cCutUpDownState = 1;
}

void CtrUpCut(void)
{
	//将笔抬起
	SetIOBit(O_CUTDOWN,C_CUTUP);
	Delay(gl_iCutUpDelay);
	gl_cCutUpDownState = 0;
}


extern void ITMoveHandpiece(char cAix, int iMovePluse);

//初始化绘图仪
int InitFlatPlotter()
{
	int iPos,iStep;
	int iMaxPulseNum;
	//int iPlusLen;
	Printf("gl_cAutoSendPaperSet:%d\n", gl_cAutoSendPaperSet);
	// 启用XY搜索回原点
	// X搜索回原点
	if(gl_cAutoSendPaperSet != 0)
	{
		iPos = 0;
		iStep = -2 * (int)X_PPMM;
		FixedSpeeDrive(AXIS_X, 1500);
		while(1)
		{
			if ((GetIOBit(I_GIVEPEND) == StatusOk) || (iPos < -100000))	//20141201 rwt
			{// 送纸上感应器  启动
				if (iPos != 0)
				{
					SoftDelay(1000);
					ITMoveHandpiece(AXIS_X, -iPos);
					
				}
				SetIOBit(O_GIVEMOTORDIR,G_SENDPAPER);
				SetIOBit(O_STARTGIVEPMOTOR,0);//启动送纸
				iStep = 0;
				while ((GetIOBit(I_GIVEBACK) == StatusErr) && (iStep < 3000000))
					iStep++;
				SetIOBit(O_STARTGIVEPMOTOR,1);//停止送纸
				//Printf("GIVEP END!\n");
				break;
			}
			else if (GetIOBit(I_GIVEBACK) == StatusOk)
				{// 送纸下感应器 停
					if (iPos != 0)
					{
						ITMoveHandpiece(AXIS_X, -iPos);
					}
					
					if (GetIOBit(I_GIVEBACK) == StatusErr)
					{
						SetIOBit(O_GIVEMOTORDIR,G_ROLLPAPER);
						SetIOBit(O_STARTGIVEPMOTOR,0);//启动收纸
						iStep = 0;
						while((GetIOBit(I_GIVEBACK) == StatusErr) && (iStep < 5000000))
							iStep++;
						SetIOBit(O_STARTGIVEPMOTOR,1);//停止收纸
						SetIOBit(O_GIVEMOTORDIR,G_SENDPAPER);
					}
					Printf("GIVEP BACK!\n");
					break;
				}
			/*
			else if ((iStep < -3000000) && (GetIOBit(I_GIVEPEND) == StatusErr)	//20141120 rwt
					&& (GetIOBit(I_GIVEBACK) == StatusErr))
			{
				break;
			}
			*/
			else
			{//　
				ITMoveHandpiece(AXIS_X, iStep);
				iPos += iStep;
				//Printf("iPos: %d\n",iPos);		//20141201 rwttestaaaa
			}
		}
	}
	
	//找Y轴原点
	iPos = 0;
	iStep = 2 * (int)Y_PPMM;
	iMaxPulseNum = gl_iMaxYMM + 50 * Y_PPMM;
	FixedSpeeDrive(AXIS_Y, 1500);
	while ((GetIOBit(I_YORIGIN) == StatusOk) && (iPos < 20*iStep))
	{  //如果笔车已经停在Y轴原点位置,需要首先向左(Y轴正方向)移动到不在原点的位置
		
		if (gl_cPrintStatus == 0x10)
		{
			if( gl_cLanguageSelect == 0 )
			{
				strcpy(gl_cLcdDispBuf[0],"驱动报警");
				strcpy(gl_cLcdDispBuf[1],"请关机重试");
			}
			else
			{
				strcpy(gl_cLcdDispBuf[0],"Drive Error!");
				strcpy(gl_cLcdDispBuf[1],"PowerOff Retry!");
			}
			lcd_refresh();
			while (1);
		}
		
		ITMoveHandpiece(AXIS_Y, iStep);
		iPos += iStep;
	}
	
	if (iPos >= 20*iStep)
	{  //笔车向左移动了20mm仍挡住原点传感器,原点传感器损坏
		ViewError(ER_YOBAD);
	}
	Delay(1000);
	
	iPos = 0;
	while (GetIOBit(I_YORIGIN) == StatusErr)
	{  //搜索Y轴原点位置
		if (iPos > iMaxPulseNum)
			ViewError(ER_YOBAD);
	
		if (gl_cPrintStatus == 0x10)
		{
			if( gl_cLanguageSelect == 0 )
			{
				strcpy(gl_cLcdDispBuf[0],"驱动报警");
				strcpy(gl_cLcdDispBuf[1],"请关机重试");
			}
			else
			{
				strcpy(gl_cLcdDispBuf[0],"Drive Error!");
				strcpy(gl_cLcdDispBuf[1],"PowerOff Retry!");
			}
			lcd_refresh();
			while (1);
		}
		ITMoveHandpiece(AXIS_Y, -iStep);
		iPos += iStep;
	}
	Delay(1000);

	// 找原点
	iStep = LeftMargin + gl_iOriginalPos;
	ITMoveHandpiece(AXIS_Y, iStep);
	/*
	iMaxPulseNum = (int)(gl_sPlotType * Y_PPMM);
	if (gl_iOriginalPos <= 0)
	{//起始打印位置离原点感应器1cm
		gl_iOriginalPos = 0;
		iStep = 10 * Y_PPMM;
		ITMoveHandpiece(AXIS_Y, iStep);
	}
	else
	{
		if (gl_iOriginalPos > iMaxPulseNum)
			gl_iOriginalPos = (long)iMaxPulseNum;
		iStep = gl_iOriginalPos + 10 * Y_PPMM;
		ITMoveHandpiece(AXIS_Y, iStep);
	}
	*/
	gl_iAxisYPos = gl_iOriginalPos;

	return 0;
}


// 终止工作 搜索回原点
void StopWorkRetu(void)
{
	InitFlatPlotter();	
}
//************************************************************
	
void DriveSprinkler(int iPulseNumX,int iPulseNumY)
{
	//int axis, plus, speed;
	//int speed;
	//int rA, rB;
	
	int iStatus;
	if ((rPDATG& 0x08) == 0)
	{//判断Y轴是否已经报警
		gl_cPrintStatus = 0x10;
		return;
	}
	
	if(iPulseNumX != 0)
	{
		//StraightLineADSpeed(AXIS_X, gl_iSVSpeed, gl_iVSpeed, gl_iTimeAdd);
		//SetPrintSpeed(AXIS_X);
		//rA = ReadEncoderA();
		//rB = ReadEncoderB();
		//Printf("RA[0x%x, 0x%x]\n", rA, rB);
		//StraightLineADSpeed(AXIS_X, 5, Print_V-50, Print_T); //边沿喷墨不齐 6.22
		StraightLineADSpeed(AXIS_X, 5, Print_V-50, Print_T);
		pmove(AXIS_X, iPulseNumX);
		do
		{
			//ReceiveData();
			get_status(AXIS_X,&iStatus);
		}while(iStatus!=0);
		gl_iMovePaperLength += iPulseNumX;
		gl_TestSendLengPluse += fabs(iPulseNumX);
	}

	if(iPulseNumY != 0)
	{
		if(gl_cBidirectional == 1)
		{
			if(iPulseNumY < 0)
			{
				WriteDirct(0xaaaa);//打印反向
				//Printf("-");
			}
			else
			{
				WriteDirct(0x5555);//打印正向
				//Printf("+");
			}
		}
		else
		{
			if(iPulseNumY > 0)
			{
				WriteDirct(0x5555);//打印正向
				//Delay(50);
				//Printf("+");
			}
		}
		
		Write312YAxisPlus(iPulseNumY);// 408
		
		//StraightLineADSpeed(AXIS_Y, gl_iSVSpeed, gl_iVSpeed, gl_iTimeAdd);
		//StraightLineADSpeed(AXIS_Y,50, Print_V-50, Print_T); gzr20160830
		StraightLineADSpeed(AXIS_Y,5, Print_V-50, Print_T);
		//SetPrintSpeed(AXIS_Y);
		if(iPulseNumY < 0)
			iPulseNumY += -3000;
		else
			iPulseNumY += 3000;
		//Printf("AA:YPlus[%d]\n", iPulseNumY);
		pmove(AXIS_Y, iPulseNumY);
				
		while (ReadPrintEnd() != 0);
	}
	
	//Printf("Print end 1 line\n");
	
}

//反向将数据转换后存入指定地址
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
	//Printf("DataBackward....\n");
	//Printf("IB[%d],SD[%d], IW[0x%x]\n", iCountByte, gl_iStaggerDot, iWriteAddr);
	bakWriteAddr = iWriteAddr + (iCountByte - 2) * (gl_iGraphWidth * 8 + AdditionDot - gl_iStaggerDot + MotorSideDot);
	for (n=0;n<(gl_cSprinklerCount-1);n++)
		bakWriteAddr = bakWriteAddr + (iCountByte - 2) * iYDotSpace[n];
	if (iWriteAddr == InvertDataAddr0)
		gl_iDataEndAddr0 = bakWriteAddr;
	else
		gl_iDataEndAddr1 = bakWriteAddr;
	
	//Printf("\n=3==gl_cPrintStatus[%d] PE[%d] PH[%d]===\n", gl_cPrintStatus, gl_iEthnetRecBufPE, gl_iEthnetRecBufPH);
	iWriteAddr = bakWriteAddr + (AdditionDot + gl_iStaggerDot + OriginalSideDot - 1) * (iCountByte - 2);
	
	idat1 = AdditionDot + gl_iStaggerDot + OriginalSideDot;
	iCount1 = 2 * (iCountByte- 2);
	Printf("backAdd:[0x%x], starAdd[0x%x],ID[%d],iC[%d]\n", bakWriteAddr, iWriteAddr, idat1, iCount1);
	for (iCount=0;iCount<idat1;iCount++)
	{//消除反向扫描误差
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
		//ReceiveData();
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
				Writesdram(iWriteAddr,0x0);
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
	//Printf("endAdd:[0x%x]\n",iWriteAddr);
}

//正向将数据转换后存入指定地址
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
	Printf("DataForward....\n");
	Printf("SC[%d],SN[%d],ML[%d],YS[%d]\n", gl_cSprinklerCount, gl_cSprinklerNo,gl_cMultiDotLine12, gl_iYDotSpace12 );
	bakWriteAddr = iWriteAddr + (iCountByte + 2) * (gl_iGraphWidth * 8 + AdditionDot + OriginalSideDot);
	Printf("Base:[0x%x],Bak:[0x%x]\n",iWriteAddr, bakWriteAddr);
	for (n=0;n<(gl_cSprinklerCount-1);n++)
		bakWriteAddr = bakWriteAddr + (iCountByte + 2) * iYDotSpace[n];
//	Printf("BakEnd:[0x%x] ------->>>><<<<<\n", bakWriteAddr);
	if (iWriteAddr == InvertDataAddr0)
		gl_iDataEndAddr0 = bakWriteAddr;
	else
		gl_iDataEndAddr1 = bakWriteAddr;
	
	//空走附加点
	idat1 = AdditionDot + OriginalSideDot;
	for (iCount=0;iCount<idat1;iCount++)
	{
		for (k=0;k<gl_cSprinklerCount;k++)
		{
			for (j=0;j<14;j++)
			{
				Writesdram(iWriteAddr,0);
				//Printf(" Writesdram------->>>><<<<<\n");
				iWriteAddr += 2;
			}
		}
	}
	
	Printf("CurAdr:[0x%x]\n", iWriteAddr);
	
	bakWriteAddr = iWriteAddr;
	iCount1 = gl_iGraphWidth * 8;
	for (n=0;n<gl_cSprinklerCount;n++)
	{
		//ReceiveData();
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
					//Printf(" 1650 Writesdram------->>>><<<<<\n");
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
				//Printf(" 1679 Writesdram------->>>><<<<<\n");
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
						//Printf(" 1711 Writesdram------->>>><<<<<\n");
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
						//Printf(" 1761 Writesdram------->>>><<<<<\n");
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
//		{//空走附加点
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
	Printf("PrendAdd:[0x%x]\n",iWriteAddr);
}

extern void RunCutMod(void);

//将接收到的数据转换成打印格式
void InvertPrintData(void)
{
	unsigned int iWriteAddr;
	//unsigned int iWriteAddr;

	//正在接收切割数据
	if( gl_cCutFileSendFlag == 0x01)
	{
		//Printf("InvertPrintData is cut data....\n");
		//Printf(" gl_cPrintStatus:%d, gl_cCurPrintBuf:%d\n", gl_cCurPrintBuf, gl_cCurPrintBuf);
		return;
	}
	
	if(gl_cPrintCutSelect != 0x01)
	if(gl_cFileReceive == 0x01)
		return;

	if (gl_cPrintStatus != 2)
	{// 打印没有结束
		if (gl_cCurPrintBuf == 0)
		{// 缓冲区0
			//Printf("zhun bei buf0\n");
			if((gl_cInvertBufStatus&PrintBit) != 0)
			{// 准备缓冲区0
				gl_cInvertBufStatus &= (~Buf1Presetbit);
				gl_cInvertBufStatus &= (~PrintBit);
			}
		}
		else
		{// 缓冲区1
			if ((gl_cInvertBufStatus&PrintBit) == 0)
			{// 准备缓冲区1
				//Printf("zhun bei buf1\n");
				gl_cInvertBufStatus &= (~Buf0Presetbit);
				gl_cInvertBufStatus |= PrintBit;
			}
		}
	}
	if (gl_cPrintStatus == 0)
	{//当前行打印结束
	
		if (gl_cCurPrintBuf == 0)
		{ //缓冲区0
			if ((gl_cInvertBufStatus&Buf0Presetbit) != 0)
			{
				gl_iReadDataAddr = InvertDataAddr0; //数据块开始地址
				gl_iDataEndAddr = gl_iDataEndAddr0;	//数据块结束地址 				
				gl_cPrintStatus = 1;		//启动打印
				Printf("====>Start Print Data buff 0\n");
			}
			else if ((gl_cFileReceive & 0x20) == 0x20)
			{	
				// 打印结束
				//单向是在此处打印结束
				gl_cFileReceive = 0;
				gl_cInvertBufStatus = 0;
				gl_cCommunicationMode = 0xff;
				gl_cPrintWorkStatus = 0;						//20120719
				//Printf("==>Print end WX[%d] WY[%d] AX[%d] AY[%d]\n", gl_lWorkOrig_XXPluse, gl_lWorkOrig_YYPluse, gl_lAxisXPos, gl_iAxisYPos);
				//gl_lCurPost_XPluse = gl_lAxisXPos + gl_lWorkOrig_XXPluse;	// 当前脉冲数 X 20130809 Control invertPrintData
				//gl_lCurPost_YPluse = gl_iAxisYPos + gl_lWorkOrig_YYPluse;  // 
				//Printf("==>Print end CX[%d] CY[%d]\n", gl_lCurPost_XPluse, gl_lCurPost_YPluse);
				gl_MacRunState = 0;
				cReviceFlag = 0;
				return;
			}
		}
		else
		{ //缓冲区1
			if ((gl_cInvertBufStatus&Buf1Presetbit) != 0)
			{
				gl_iReadDataAddr = InvertDataAddr1;
				gl_iDataEndAddr = gl_iDataEndAddr1;
				gl_cPrintStatus = 1;
				Printf("====>Start Print Data buff 1\n");
			}
			else if ((gl_cFileReceive & 0x20) == 0x20)
			{
				if (ReadPrintEnd() == 0)
				{
					Printf("Buff 1 double-way Print end\n");
					return;
				}
				else
				{
					gl_cFileReceive = 0;
					gl_cCurPrintBuf = 0;
					gl_cInvertBufStatus = 0;
					gl_cCommunicationMode = 0xff;
					if (gl_iPulseNumY < 0)
					{//Printf("Print End 1\n");
						ClrFIFO;
						DriveSprinkler(0,gl_iPulseNumY);//驱动Y轴
						gl_iAxisYPos += gl_iPulseNumY;
						gl_iPulseNumY = -gl_iPulseNumY;
					}

					gl_cPrintWorkStatus = 0;
					//Printf("-->Print end WX[%d] WY[%d] AX[%d] AY[%d]\n", gl_lWorkOrig_XXPluse, gl_lWorkOrig_YYPluse, gl_lAxisXPos, gl_iAxisYPos);
					//gl_lCurPost_XPluse = gl_lAxisXPos + gl_lWorkOrig_XXPluse;	// 当前脉冲数 X 20130809 Control invertPrintData
					//gl_lCurPost_YPluse = gl_iAxisYPos + gl_lWorkOrig_YYPluse;  // 
					//Printf("-->Print end CX[%d] CY[%d]\n", gl_lCurPost_XPluse, gl_lCurPost_YPluse);
					gl_MacRunState = 0; //打印结束，恢复按键操作
					cReviceFlag = 0;
					return;
				}
			}
		}
	}
	
	if ((gl_cFileReceive&0x20) == 0x20)
	{
		//Printf("File Receive over\n");		//文件接收完毕跳到此处，
		//if( gl_cCutFileSendFlag == 0x02)
		//{// 20100909  运行切割操作
		//	Printf("Print end run CUT.....\n");
		//	RunCutMod();
		//}
		return;
	}
	if ((gl_cFileReceive&0x40) == 0)//没结束
	{
		if (CalculateRecDatNum() == 0)
			return;		//未接收满一行数据字节
	}
		
	if ((gl_cInvertBufStatus&PrepData) != 0)
	{//当前准备打印或正在打印的是缓冲区1中的数据
		//Printf("zhunbei dyin 1\n");
		if ((gl_cInvertBufStatus&Buf1Presetbit) == 0)
			iWriteAddr = InvertDataAddr1;//缓冲区1中数据未准备好
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
					lcd_display(0,"驱动报警");
					lcd_display(1,"7:退出");
					WaitKeyExit(Key_Pause);
					lcd_refresh();
					//Printf("driver error!gl_cFileReceive1 != 0\n");
				}
			}
			return;
		}
	}
	else
	{//当前准备打印或正在打印的是缓冲区0中的数据
		//Printf("zhunbei dyin 2\n");
		if ((gl_cInvertBufStatus&Buf0Presetbit) == 0)
			iWriteAddr = InvertDataAddr0;//缓冲区0中数据未准备好
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
					lcd_display(0,"驱动报警");
					lcd_display(1,"7:退出");
					WaitKeyExit(Key_Pause);
					lcd_refresh();
					//Printf("driver error!gl_cFileReceive2 != 0\n");
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
			InvertDataBackward(iWriteAddr);//反向
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
//指针偏移计算
unsigned int FlashPointerAdd(unsigned int pointer,unsigned int length)
{
	pointer += length;
	if (pointer >= EthnetBufLength)
		pointer = pointer - EthnetBufLength;
	return pointer;
}
//计算已接收数据长度
//
unsigned int CalculateRecDatNum(void)
{
	unsigned int iNullSize,iNullSize1;
	unsigned long address;
	unsigned int i;
	//Printf("CalculateRecDatNum\n");
	if (gl_iEthnetRecBufPH == gl_iEthnetRecBufPE)
	{
		if ((gl_cFileReceive & 0x80) != 0)	//文件传送完
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
		if ((gl_cFileReceive & 0x80) != 0)	//文件传送完
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
			Printf("Write byte[%d]\n", iNullSize1);
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
//喷头间X、Y方向误差和走纸误差
//cPtintCount=2时打印走纸误差
//cPtintCount=1时打印喷头间X、Y方向误差
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
	
	//计算Y轴脉冲长度
	iGapPixel = (int)(300.0 * Y_PPMM / DotPulseNum);//1000
	iTotalPixel = iGapPixel + 2 * AdditionDot + OriginalSideDot + MotorSideDot;//1米
	for (m=0;m<(gl_cSprinklerCount-1);m++)
		iTotalPixel = iTotalPixel + iYDotSpace[m];
	iPosNumY = (iTotalPixel + 320) * DotPulseNum;
	//判断Y轴是否会超界
	Printf("iTotalPixel:%d, gl_iYMaxEffectivePMM:%d", (gl_lCurPost_YPluse + iPosNumY), gl_iYMaxEffectivePMM);
	
	if (((gl_lCurPost_YPluse + iPosNumY) > gl_iYMaxEffectivePMM))// || ((gl_lCurPost_XPluse + gl_iPulseNumX) > gl_iXMaxEffectivePMM))
	{
		lcd_display(0,"越界请重新定位");
		lcd_display(1,"任意键继续");
		WaitKeyExit(0xff);		//等待有键按下
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		return 1;
	}	
	
	LineReverse(0x80);		//清反白
	//刷新显示
	lcd_display(0,"正在打印");
	lcd_display(1,"请稍后...");
	
	while (ReadPrintEnd() == 0);
	
	PrintHeadClean(1);
	WriteFilePrint(0xaaaa);
	//DrivePrint;
	ClrFIFO;
	gl_cLineWidth = 3;
	//WriteFIFOFull(gl_cSprinklerCount-1);
	
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
			
			while (ReadFIFOFull() != 0)	//缓冲满
			{
				if (cSendPosFlag == 0)
				{
					DriveSprinkler(0,iPosNumY);
					//gl_iAxisYPos += iPosNumY;
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
						WriteFIFO(address,0);
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
							WriteFIFO(address,0);
							address += 4;
							k -= n;
						}
						else
						{
					
							WriteFIFO(address,iSendDat[k]);
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
						WriteFIFO(address,0);
						address += 4;
					}
				}
			}
			i++;
			
			if ((cSendPosFlag == 1) && (ReadPrintEnd() != 0))
					break;
		}
		WaitDriveEnd(AXIS_Y);
		ClrFIFO;		//清FIFO缓冲区
		Delay(1000);
		cPtintCount--;
		/*
		if (cPtintCount == 0)
			i = gl_iPulseNumX + (int)(-10 * X_PPMM);
		else
			i = gl_iPulseNumX;
		*/
		DriveSprinkler(0,iPosNumY);
		//gl_iAxisYPos += iPosNumY;
		//iPosNumY = -iPosNumY;
		WaitDriveEnd(AXIS_Y);
	}
	return 0;
}

//打印双向打印时左右错位图形
//iGraphWidth:图形宽度
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
	
	//计算Y轴脉冲长度
	iTotalPixel = iGraphWidth * 8 + 2 * AdditionDot + OriginalSideDot + MotorSideDot;////
	for (m=0;m<(gl_cSprinklerCount-1);m++)
		iTotalPixel = iTotalPixel + iYDotSpace[m];
	iPosNumY = (iTotalPixel ) * DotPulseNum;//+ 320
	
	//判断Y轴是否会超界
	if ((gl_iOriginalPos + iPosNumY) > gl_iYMaxEffectivePMM)
	{
		lcd_display(0,"越界请重新定位");
		lcd_display(1,"任意键继续");
		WaitKeyExit(0xff);		//等待有键按下
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		return 1;
	}	
	
	LineReverse(0x80);		//清反白
	//刷新显示
	lcd_display(0,"错位图形打印");
	lcd_display(1,"请稍后...");
	
	while (ReadPrintEnd() == 0);

	PrintHeadClean(1);
	WriteFilePrint(0xaaaa);
	ClrFIFO;
	WriteFIFOFull(gl_cSprinklerCount-1);

	sdat[0] = AdditionDot + OriginalSideDot;
	sdat1[0] = AdditionDot + OriginalSideDot + iGraphWidth * 8;////////
	for (m=1;m<gl_cSprinklerCount;m++)
	{
		sdat[m] = sdat[m-1] + iYDotSpace[m-1];
		sdat1[m] = sdat1[m-1] + iYDotSpace[m-1];
		Printf(" 2509 sdat[%d]:%d,  sdat1[%d]:%d  \n" ,m,sdat[m],m,sdat1[m]); // (一次)

	}
	Printf("iGraphWidth:%d, iYDotSpace[0]:%d\n",iGraphWidth,iYDotSpace[0]);
	Printf("[%d]sdat[0]:%d, sdat[1]:%d, sdat1[0]:%d, sdat1[1]:%d\n",iTotalPixel ,sdat[0],sdat[1],sdat1[0],sdat1[1]);
	i = 0;
	cSendPosFlag = 0;
	while (1)
	{
		Printf(" i = %d  \n ", i);
		if (i >= iTotalPixel)
			break;
		Printf("\n\n (rPDATD & 0x0010) = %d  \n\n\n ", (rPDATD & 0x0010));
		while (ReadFIFOFull() != 0)	//缓冲满
		{			
			
			if (cSendPosFlag == 0)
			{
				Printf(" i  = %d  \n ", i);
				DriveSprinkler(0,iPosNumY);//
				//gl_iAxisYPos += iPosNumY;
				iPosNumY = -iPosNumY;
				cSendPosFlag = 1;
			}
			else if(ReadPrintEnd() != 0)
				break;
		}
		Printf(" cSendPosFlag = %d  \n ",cSendPosFlag );
		if ((cSendPosFlag == 1) && (ReadPrintEnd() != 0))
			break;//跳出循环
	
		
		address = gl_iWriteFIFOAddr;
		for (m=0;m<gl_cSprinklerCount;m++)
		{
			if ((i >= sdat1[m]) || (i < sdat[m]))
			{
				for (j=0;j<7;j++)
				{
					WriteFIFO(address,0);
					address += 4;
				}
			}
			else if (i < (sdat[m] + gl_cLineWidth))
			{
				for (j=0;j<7;j++)
				{
					WriteFIFO(address,0x3ffffff);
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
					WriteFIFO(address,0);
					address += 4;
				}
			}
			Printf(" gl_cSprinklerCount = %d  , m = %d ",gl_cSprinklerCount , m);
			Printf(" 2566 sdat[%d]:%d,  sdat1[%d]:%d  \n" ,m,sdat[m],m,sdat1[m]);

		}

		i++;

		if ((cSendPosFlag == 1) && (ReadPrintEnd() != 0))
			break;
		
	}
	WaitDriveEnd(AXIS_Y);
	ClrFIFO;		//清FIFO缓冲区
	//驱动X前驱
	//iTotalPixel = (unsigned int)(((float)SprinklerWidth * DotLineWidth) * X_PPMM / 2.0);
	
	DriveSprinkler(-gl_iPulseNumX,0);
	WaitDriveEnd(AXIS_X);

	//驱动Y反向打印
	cSendPosFlag = 0;
	sdat[0] += gl_iStaggerDot;
	sdat1[0] = AdditionDot + gl_iStaggerDot + OriginalSideDot;
	for (m=1;m<gl_cSprinklerCount;m++)
	{
		sdat[m] += gl_iStaggerDot;
		sdat1[m] = sdat1[m-1] + iYDotSpace[m-1];
		Printf(" 2589 sdat[%d]:%d,  sdat1[%d]:%d  \n" ,m,sdat[m],m,sdat1[m]);

	}
	Printf("[%d] sdat[0]:%d, sdat[1]:%d, sdat1[0]:%d, sdat1[1]:%d, StaggerDot[%d]\n",i, sdat[0],sdat[1],sdat1[0],sdat1[1], gl_iStaggerDot);
	while (1)
	{
		Printf(" i = %d  \n ", i);
		if (i <= 0)
			break;

		Printf("\n\n (rPDATD & 0x0010) = %d  \n\n\n ", (rPDATD & 0x0010));

		while (ReadFIFOFull() != 0)	//缓冲满   //bingo
		{
			if (cSendPosFlag == 0)
			{
				Printf(" i  = %d  \n ", i);
				DriveSprinkler(0,iPosNumY);
				//gl_iAxisYPos += iPosNumY;
				iPosNumY = -iPosNumY;
				cSendPosFlag = 1;
			}
			else if(ReadPrintEnd() != 0)
				break;
		}
		Printf(" cSendPosFlag = %d  \n ",cSendPosFlag );
		if ((cSendPosFlag == 1) && (ReadPrintEnd() != 0))
			break;
		address = gl_iWriteFIFOAddr;
		for (m=0;m<gl_cSprinklerCount;m++)
		{
			Printf(" 2615 sdat[%d]:%d,  sdat1[%d]:%d  \n" ,m,sdat[m],m,sdat1[m]);

			if ((i <= sdat1[m]) || (i > (sdat[m] + gl_cLineWidth)) || (i > (sdat1[m] + iGraphWidth * 8)))////
			{
				for (j=0;j<7;j++)
				{
					WriteFIFO(address,0);
					address += 4;
				}
			}
			else if (i <= sdat[m])
			{
				sdat[m] -= 119;
				for (j=0;j<7;j++)
				{
					WriteFIFO(address,0);
					address += 4;
				}
			}
			else
			{
				for (j=0;j<7;j++)
				{
					WriteFIFO(address,0x3ffffff);
					address += 4;
				}
			}
		}
		i--;
	}
	WaitDriveEnd(AXIS_Y);
	
	//iTotalPixel = gl_iPulseNumX + (int)(-10 * X_PPMM);
	//驱动X 回退
	DriveSprinkler(-gl_iPulseNumX,0);
	WaitDriveEnd(AXIS_X);
	
	return 0;
}
extern int gl_time;
void ClearPrintHead(char cOrgState)
{
	long lTemp;
	
	lTemp = gl_iAxisYPos + 90*Y_PPMM;
		
	FixedSpeeDrive(AXIS_Y,2000);
	ITMoveHandpiece(AXIS_Y, -lTemp);//负号是往右走(左右是相对于人来看) 

	Printf("\nAXIS_Y = %d  lTemp = - %d \n", AXIS_Y,lTemp);	
	Printf("gl_cSWFStartClearFlag = %d\n\n", gl_cSWFStartClearFlag);	//20141213 rwt
	
	//if(gl_cSWFStartClearFlag == 1)
	//{// 20141021
		Printf("Right Clean Head!!\n");
		PrintHeadClean(gl_time);
		Printf("gl_time == %d \n" , gl_time);
	
		gl_cSWFStartClearFlag = 1;
	//	gl_cSWFStartClearFlag = 0;
	//}
	
	Delay(2000);
	
	if(cOrgState == 1)
	{
		ITMoveHandpiece(AXIS_Y, lTemp - ((AdditionDot + OriginalSideDot) * DotPulseNum) );
		gl_iAxisYPos -= ((AdditionDot + OriginalSideDot) * DotPulseNum);
		Printf("Clear PH gl_iAxisYPos:%d, gl_iOriginalPos:%d\n", gl_iAxisYPos, gl_iOriginalPos);
	}
	else
		ITMoveHandpiece(AXIS_Y, lTemp );
}

	
//写数据到FIFO，写满为止
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
		if (ReadFIFOFull() != 0)//缓冲区满
		{
			//Printf("FIFO FULL! FIFO FULL\n");
			break;
		}
		else if (gl_iReadDataAddr >= gl_iDataEndAddr)
		{//行打印数据已经发送完毕,最后写0
			address = gl_iWriteFIFOAddr;
			for (j=0;j<i;j++)
			{
				WriteFIFO(address,0);
				address += 4;
				
			}
			break;
		}
		else
		{//发送1排打印数据
			address = gl_iWriteFIFOAddr;
			for (j=0;j<i;j++)
			{
				dat = Readsdram(gl_iReadDataAddr, 32);
				
				//if((j>6) && (dat != 0))
				//	Printf("0x%x,", dat);
				//	WriteFIFO(address,0xffffffff);
				//else
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

extern int gl_intoruncutmod;

//打印处理//定时器中断每8ms来一次
void DealPrint(void)
{	
//	Printf("__ReceiveData__\n");
if( gl_intoruncutmod == 1) //进入函数runcutmod的标志位
	{
		ReceiveData();	
	}
//	//long lTemp=0;
//	Printf("__DealPrint__\n");
	switch (gl_cPrintStatus)
	{
		case	0:
			if ((gl_cDelayFlag == 1) && (ReadPrintEnd() != 0))
				gl_iWaitCount++;
			break;
		case	1:		
			//准备开始打印(打印一行，不是整个文件)
		    //ClearPrintHead(0);//打印前清洗喷头20151104
			//SelecAxis(AXIS_Y);
			//if (ReadAxisStatus(AXIS_Y) == 0)
			Printf("*****gl_cPrintStatus == 1 ReadPrintEnd() == %d\n", ReadPrintEnd());
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
				WriteDataToFIFO();	//写FIFO缓冲区数据,写满为止  a
				DriveSprinkler(0,gl_iPulseNumY);//驱动Y轴 a
				
				gl_iAxisYPos += gl_iPulseNumY;
				//gl_iCurrentPos_Y = gl_iAxisYPos; //当前Y向运动坐标
				gl_cPrintStatus = 2;
			}
			break;
		case	2:		//Y轴打印中
			//SelecAxis(AXIS_Y);
			//if (ReadAxisStatus(AXIS_Y) == 0)
			if (ReadPrintEnd() != 0)
			{//Y轴打印结束
				ClrFIFO;//清FIFO缓冲区	
				if (gl_cBidirectional == 1)
				{//双向打印
					//WriteDataToFIFO();
					gl_cPrintStatus = 0;//4;
					if ((gl_cDelayFlag & 0x40) == 0)
					{
						gl_cDelayFlag = 1;
						gl_iWaitCount = 0;
						gl_iWaitCount1 = 0;
					}
					//Printf("Drive X Start\n");
					DriveSprinkler(-gl_iPulseNumX,0);//驱动X轴走纸 q
					WaitDriveEnd(AXIS_X);	
					gl_lAxisXPos += gl_iPulseNumX;
					gl_iPulseNumY = -gl_iPulseNumY;//反向
				}
				else
				{
					DriveSprinkler(0, -gl_iPulseNumY);//a
					WaitDriveEnd(AXIS_Y);				//a
					gl_iAxisYPos -= gl_iPulseNumY;

					DriveSprinkler(-gl_iPulseNumX,0);//驱动Y轴回原点 a
					WaitDriveEnd(AXIS_X);			//a
					gl_lAxisXPos += gl_iPulseNumX;//当前X向运动坐标
						
					gl_cPrintStatus = 0;
				}
				if (gl_cCurPrintBuf == 0)
					gl_cCurPrintBuf = 1;//下次应打印的缓冲区1中的数据
				else
					gl_cCurPrintBuf = 0;//下次应打印的缓冲区0中的数据
			}
			else
				WriteDataToFIFO();	//写FIFO缓冲区数据,写满为止
			break;
		case	3:		//空走回原点
			if (ReadPrintEnd() != 0)
			{//Y轴返回打印结束
				Printf("Print end return org!\n");
			}
			break;
		default:
			break;
	}

	if(gl_cPrintStatus==1)
	{
		if ((GetIOBit(I_YUPINDUCTOR) == StatusOk))//// Y轴左感应器
		{
			//pmove(AXIS_Y, 0);
			SuddenStop(AXIS_Y);
			while(1);
		}
	
		if ((GetIOBit(I_XRIGHINDUCTOR) == StatusOk))
		{
			//pmove(AXIS_X, 0);
			SuddenStop(AXIS_X);
			while(1);
		}	
	}

	//20140512 增加自动送纸开启关闭
	
	if(gl_cAutoSendPaperSet != 0)/////
	{
		//送纸检测
		if ((gl_cReacterStatus & B_SENDPAPER) != 0)//B_SENDPAPER 0x01//收送纸状态.bit0为1:已启动送纸,bit1为1已启动收纸
		{//已启动送纸
			if ((GetIOBit(I_GIVEBACK) == StatusOk) || (gl_iSendPaper >= 400))//625
			{//送纸位置到达或已送纸5s
				/*	StatusOk					0x00				//感应器位置被挡住
				StatusErr					0x01				//感应器位置没有被挡住*/
			 
					SendPaperClose=1;
					//201002023 屏蔽
					//2016-1-13
					SetIOBit(O_STARTGIVEPMOTOR,1);			//关闭送纸电
				///////////	SetIOBit(O_STARTGIVEPMOTOR,0);			//启动送纸电机					
				//	gl_cReacterStatus &= (~B_SENDPAPER);
				//	gl_iMovePaperLength = 0;

			}
			else if (GetIOBit(I_GIVEPEND) == StatusOk)		//没纸
			{
				SetIOBit(O_STARTGIVEPMOTOR,1);				//关闭送纸电机

				gl_cReacterStatus &= (~B_SENDPAPER);
				gl_cReacterStatus |= B_NOPAPER;
			}
			else
				gl_iSendPaper++;
		}
		else if (((gl_cReacterStatus & B_NOPAPER) == 0))
		{
			//送纸过冲20100223
			if(gl_cSendPaperStar == 0)
			if(GetIOBit(I_GIVEBACK) == StatusOk)
			{
				gl_cSendPaperStar = 1;
				gl_iMovePaperLength = 0;
			}
			//
			if (GetIOBit(I_GIVEPEND) == StatusOk)//没纸
				gl_cReacterStatus |= B_NOPAPER;
			else if (gl_iMovePaperLength < -33200)				//走纸超过660MM,送纸
			{
				if(gl_cSendPaperStar)
				{
					if (GetIOBit(I_GIVEBACK) == StatusErr)
					{
						gl_cReacterStatus |= B_SENDPAPER;
						SetIOBit(O_GIVEMOTORDIR,G_SENDPAPER);	//20141213 rwt
						SetIOBit(O_STARTGIVEPMOTOR,0);			//启动送纸电机
						gl_iSendPaper = 0;						//送纸计数器清零
						Printf("BBBB\n");
					}
					Printf("AAAA\n");
				}
			}
		}
		
		//20100223 送纸到达,延时关闭电机
		if(SendPaperClose)
		{
			if(SendDelayTime>4)
			{
				SendPaperClose = 0;
				SendDelayTime = 0;
				SetIOBit(O_STARTGIVEPMOTOR,1);		//关闭送纸电机
				gl_cReacterStatus &= (~B_SENDPAPER);
				gl_iMovePaperLength = 0;
				if(GetIOBit(I_GIVEBACK) != StatusOk)
				gl_cSendPaperStar = 0;
			}
			else
				SendDelayTime += 1;
		}
		//
	}
	else
		gl_iMovePaperLength = 0;
	
	//收纸检测
	if ((gl_cReacterStatus & B_ROLLPAPER) != 0)
	{//已开始收纸
		if ((GetIOBit(I_ROLLPEND) == StatusOk) || (gl_iRollPaper >= 625))
		{//收纸停位置到达或已收纸5s
			SetIOBit(O_STARTROLLPMOTOR,1);		//关闭收纸电机
			gl_cReacterStatus &= (~B_ROLLPAPER);
			Printf("Close Roo\n");
		}
		else
			gl_iRollPaper++;
	}
	else
	{
		if (GetIOBit(I_ROLLSTART) == StatusOk)//收纸启动
		{
			if ((GetIOBit(I_ROLLPEND) == StatusErr) && (gl_cSelfCheckFlag == 0))
			{
				gl_cReacterStatus |= B_ROLLPAPER;
				SetIOBit(O_STARTROLLPMOTOR,0);		//启动收纸电机
				gl_iRollPaper = 0;								//收纸计数器清零
				Printf("Start Rool\n");
			}
			Printf("Start Rool 2\n");
		}
	}	
}

void PrintReturnZero(void)
{
	long lPluseXX, lPluseYY;
	int iStatus;
	
	//判断当前位置是否与用户设定位置是否一致，不一致移动到工作位置
	if((gl_lWorkOrig_XXPluse != gl_lCurPost_XPluse) || (gl_lWorkOrig_YYPluse != gl_lCurPost_YPluse))
	{//不在同一点 打印原点和当前点
		lPluseXX = gl_lWorkOrig_XXPluse - gl_lCurPost_XPluse;
		lPluseYY = (gl_lWorkOrig_YYPluse - gl_lCurPost_YPluse) - (PenCarSideInterval * DotPulseNum);
		
		Printf("W!=C >> M[%d] M[%d] WOX[%d] WOY[%d] CPX[%d] CPY[%d]\n", lPluseXX, lPluseYY, gl_lWorkOrig_XXPluse, gl_lWorkOrig_YYPluse, gl_lCurPost_XPluse, gl_lCurPost_YPluse);
		//Printf("W!=C >> M[%d] M[%d] WOX[%d] WOY[%d] CPX[%d] CPY[%d]\n", lPluseXX, lPluseYY, gl_lWorkOrig_XXPluse, gl_lWorkOrig_YYPluse, gl_lCurPost_XPluse, gl_lCurPost_YPluse);
		//StraightLineADSpeed(AXIS_Y, gl_iSVSpeed, gl_iVSpeed, gl_iTimeAdd);
		SetReturnOrgSpeed();
		inp_move2(lPluseXX, lPluseYY);
		WaitInpEnd();
		gl_lCurPost_XPluse += lPluseXX;
		gl_lCurPost_YPluse += lPluseYY;
		Printf("W!=C >> glCPX[%d] glCPY[%d]\n", gl_lCurPost_XPluse, gl_lCurPost_YPluse);
		
	}
	else{//在同一点 原点和当前点
			lPluseYY = -PenCarSideInterval * DotPulseNum;
			//Printf("W==C lPYY[%d]\n", lPluseYY);
			SetMoveFrameSpeed(AXIS_Y);
			pmove(AXIS_Y, lPluseYY);
			do
			{
				get_status(AXIS_Y, &iStatus);
			}while(iStatus!=0);
			gl_lCurPost_YPluse += lPluseYY;
			if(gl_lCurPost_YPluse < 0)
				gl_lCurPost_YPluse = 0;
			Printf("W==C >> glWX[%d] glWY[%d]\n", gl_lWorkOrig_XXPluse, gl_lWorkOrig_YYPluse);
			Printf("W==C >> glCPX[%d] glCPY[%d]\n", gl_lCurPost_XPluse, gl_lCurPost_YPluse);
		}
}


