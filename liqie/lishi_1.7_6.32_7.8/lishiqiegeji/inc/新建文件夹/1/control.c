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

void SetZAxisDevice(int speed)
{
	//设置Z初始速度
	WriteZAxisInitSpeed(speed);
	 //设置Z驱动速度
	WriteZAxisDriveSpeed(speed);
	 //设置Z加速度
	WriteZAxisAddSpeed(0);	
}

//Z轴设备初始化
// 速度2000  1S 1圈1圈2000个脉冲
void Init_ZAxisDevice()
{
	//char  FindOrgFailState=0;
	char zAxisState;
	int plusZ;
	Printf("Init Z Axis Device: [%x]\n",GetIOBit(I_ZINDUCTOR));

	SetZAxisDevice(2000);
	//检测Z是否在原点
	if (GetIOBit(I_ZINDUCTOR) == StatusOk)
	{	//  Z轴感应器被遮挡
		Printf("Z Axis No Org!\n");
		WriteZAxisPlus(-4000);
		plusZ = ReadZAxisPlus();
		Printf("Read Z Plus:[%d]\n", plusZ);
		
		//驱动Z轴电机命令
		WriteZAxisDriveCmd(DriveZAxisStartCmd);
		do{
			 zAxisState = ZAxisDriveEndState;
			 if (zAxisState == 1)
			 {//Z驱动结束仍未找到Z轴原点
			 	ViewError(ER_ZINDERR);
			 	/*
			 	if (FindOrgFailState)
			 	{//逆时针180，顺时针360仍未找到原点，报原点感应器错
			 		Printf("Init Z Axis Fail\n");
					ViewError(ER_ZINDERR);
			 	}
				Delay(10);
				//顺时针旋转360°寻找原点
			 	WriteZAxisPlus(2000);
				WriteZAxisDriveCmd(DriveZAxisStartCmd);
				FindOrgFailState = 1;
				*/
			 }
		}while(GetIOBit(I_ZINDUCTOR) == StatusOk);//Z感应器未被遮挡
		WriteZAxisPlus(0);
		WriteZAxisDriveCmd(DriveZAxisStopCmd);
		

	}
/*	else
	{// 当前Z轴在原点
		Printf("Z Axis at Org!\n");
		WriteZAxisPlus(100);
		WriteZAxisDriveCmd(DriveZAxisStartCmd);	
		while(!ZAxisDriveEndState);//等待Z轴驱动结束

		while(GetIOBit(I_ZINDUCTOR)== StatusErr )
		{//Z轴感应器被挡住
			ViewError(ER_ZINDERR);
		}
		
	}

	//原点位置校正
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
	//重新设定Z轴驱动速度
	SetZAxisDevice(2000);

	gl_cCutUpDownState = 0; //刀具在原点位置
	Printf("Init Z Axis Device OK\n");
	//CtrlDownCut();
	//while(1);
}

//extern void testMcx312(void);
//extern void lp(int axis, long wdata);
//extern void ep(int axis, long wdata);
//初始化
//驱动Y回到绘图仪最大值
//驱动X回到绘图仪最大值
// 1, 首先判断是否在原点感应器位置，不在则将Y驱动到最右端，
//如果仍然未感应到感应器，则将X驱动到左下端,直到感应到原点感应器
// 2,判断Z轴回原点，若不在，驱动Z轴回原点；
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

	//Z轴设备初始化
	Init_ZAxisDevice();
	
	//更新加速度表
	UpdataAccTable();
	
	//初始化运动控制芯片MCX312
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

	// 两轴联动寻找XY机械原点
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
					{// 当前感应器被挡住
						pmove(AXIS_Y,0);
						break;
					}
					//Y脉冲发送完事肯定能送达Y上感应器的
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
					{// 当前感应器被挡住
						pmove(AXIS_X,0);
						break;
					}
					
					get_status(AXIS_X, &InductorStatus);
					if(InductorStatus == 0 && GetIOBit(I_XRIGHINDUCTOR) == StatusErr)
						ViewError(ER_RINDERR);
				}
				break;
				
			case 3:
				//设置短剧行程笔速
				InductorStatus = 0;
				//iTemp = gl_iCutSpeed;
				//gl_iCutSpeed = 10;
				//SetSDPenSpeed();
				//gl_iCutSpeed = iTemp;
				//两轴直线插补运动
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
//初始化绘图仪
void InitPlotter()
{
	int iPos,iStep;
	int iMaxPulseNum;
	
	ClrFIFO;
	gl_cIOWriteValue = 0xff;
	WriteIO(gl_cIOWriteValue);
	
	//送纸
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
				while (ReadPrintEnd() == 0);//等待打印结束
			}
			SetIOBit(O_STARTGIVEPMOTOR,0);//启动送纸
			iStep = 0;
			while ((GetIOBit(I_GIVEBACK) == StatusErr) && (iStep < 5000000))
				iStep++;
			SetIOBit(O_STARTGIVEPMOTOR,1);//停止送纸
			break;
		}
		else if ((GetIOBit(I_GIVEBACK) == StatusOk) || (iPos < 226*iStep))
		{//不用送纸
			if (iPos != 0)
			{
				DriveSprinkler(-iPos,0);
				while (ReadPrintEnd() == 0);//等待打印结束
			}
			break;
		}
		else
		{
			DriveSprinkler(iStep,0);
			while (ReadPrintEnd() == 0);//等待打印结束
			iPos += iStep;
		}
	}
	//找Y轴原点
	iPos = 0;
	iStep = 2 * (int)Y_PPMM;
	iMaxPulseNum = gl_iMaxYMM + 50 * Y_PPMM;
	while ((GetIOBit(I_YORIGIN) == StatusOk) && (iPos < 20*iStep))
	{  //如果笔车已经停在Y轴原点位置,需要首先向左(Y轴正方向)移动到不在原点的位置
		DriveSprinkler(0,iStep);
		if (gl_cPrintStatus == 0x10)
		{
			strcpy(gl_cLcdDispBuf[0],"驱动报警");
			strcpy(gl_cLcdDispBuf[1],"请关机重试");
			lcd_refresh();
			while (1);
		}
		iPos += iStep;
		while (ReadPrintEnd() == 0);//等待打印结束
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
		DriveSprinkler(0,-iStep);
		if (gl_cPrintStatus == 0x10)
		{
			strcpy(gl_cLcdDispBuf[0],"驱动报警");
			strcpy(gl_cLcdDispBuf[1],"请关机重试");
			lcd_refresh();
			while (1);
		}
		iPos += iStep;
		while (ReadPrintEnd() == 0);//等待打印结束
	}
	Delay(1000);
	
	iMaxPulseNum = (int)(gl_sPlotType * Y_PPMM);
	if (gl_iOriginalPos <= 0)
	{//起始打印位置离原点感应器1cm
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
		while (ReadPrintEnd() == 0);//等待打印结束
	}
	gl_iAxisYPos = gl_iOriginalPos;
}
*/
void DriveSprinkler(int iPulseNumX,int iPulseNumY)
{
	//int axis, plus, speed;
	int speed;
	if ((rPDATG& 0x08) == 0)
	{//判断Y轴是否已经报警
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
				WriteDirct(0xaaaa);//打印反向
				//Printf("-\n");
			}
			else
			{
				WriteDirct(0x5555);//打印正向
				//Printf("+\n");
			}
		}
		else
		{
			if(iPulseNumY > 0)
			{
				WriteDirct(0x5555);//打印正向
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
	
	//空走附加点
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
	//Printf("PrendAdd:[0x%x]\n",iWriteAddr);
}

extern void RunCutMod(void);

//将接收到的数据转换成打印格式
void InvertPrintData(void)
{
	unsigned int iWriteAddr;

	//正在接收切割数据
	if( gl_cCutFileSendFlag == 0x01)
	{
		Printf("InvertPrintData is cut data....\n");
		Printf(" gl_cPrintStatus:%d, gl_cCurPrintBuf:%d\n", gl_cCurPrintBuf, gl_cCurPrintBuf);
		return;
	}
	if (gl_cPrintStatus != 2)
	{// 打印没有结束
		if (gl_cCurPrintBuf == 0)
		{// 缓冲区0
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
				//Printf("DataA-1-S\n");
			}
			else if ((gl_cFileReceive & 0x20) == 0x20)
			{	// 打印结束
				gl_cFileReceive = 0;
				gl_cInvertBufStatus = 0;
				gl_cCommunicationMode = 0xff;
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
						DriveSprinkler(0,gl_iPulseNumY);//驱动Y轴
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
		{// 20100909  运行切割操作
			Printf("Print end run CUT.....\n");
			RunCutMod();
		}
		return;
	}
	if ((gl_cFileReceive&0x40) == 0)
	{
		if (CalculateRecDatNum() == 0)
			return;		//未接收满一行数据字节
	}
		
	if ((gl_cInvertBufStatus&PrepData) != 0)
	{//当前准备打印或正在打印的是缓冲区1中的数据
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
					Printf("driver error!gl_cFileReceive1 != 0\n");
				}
			}
			return;
		}
	}
	else
	{//当前准备打印或正在打印的是缓冲区0中的数据
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
	iGapPixel = (int)(1000.0 * Y_PPMM / DotPulseNum);
	iTotalPixel = iGapPixel + 2 * AdditionDot + OriginalSideDot + MotorSideDot;//1米
	for (m=0;m<(gl_cSprinklerCount-1);m++)
		iTotalPixel = iTotalPixel + iYDotSpace[m];
	iPosNumY = (iTotalPixel + 320) * DotPulseNum;
	//判断Y轴是否会超界
	LineReverse(0x80);		//清反白
	if (((gl_iAxisYPos + iPosNumY) < 0) || ((gl_iAxisYPos + iPosNumY) > gl_iMaxYMM))
		return 1;
	//刷新显示
	lcd_display(0,"正在打印");
	lcd_display(1,"请稍后...");
	while (ReadPrintEnd() == 0);
	WriteFilePrint(0xaa);
	ClrFIFO;		//清FIFO缓冲区
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
		ClrFIFO;		//清FIFO缓冲区
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
	iTotalPixel = iGraphWidth * 8 + 2 * AdditionDot + OriginalSideDot + MotorSideDot;
	for (m=0;m<(gl_cSprinklerCount-1);m++)
		iTotalPixel = iTotalPixel + iYDotSpace[m];
	iPosNumY = (iTotalPixel + 320) * DotPulseNum;
	//判断Y轴是否会超界
	LineReverse(0x80);		//清反白
	if (((gl_iAxisYPos + iPosNumY) < 0) || ((gl_iAxisYPos + iPosNumY) > gl_iMaxYMM))
		return 1;
	//刷新显示
	lcd_display(0,"错位图形打印");
	lcd_display(1,"请稍后...");
	
	while (ReadPrintEnd() == 0);
	WriteFilePrint(0xaa);
	ClrFIFO;		//清FIFO缓冲区
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
		while (ReadFIFOFull() != 0)	//缓冲满
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
	ClrFIFO;		//清FIFO缓冲区
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
		
		while (ReadFIFOFull() != 0)	//缓冲满
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
		while (ReadPrintEnd() == 0);//等待打印结束
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
		while (ReadPrintEnd() == 0);//等待打印结束
	}
	else
	{
		iTemp = (40*gl_cSprinklerCount + 2 * 160) * 14;
		DriveSprinkler(0,iTemp);
		while (ReadPrintEnd() == 0);//等待打印结束
		Delay(1000);//延时100ms
		iTemp -= gl_iAxisYPos;
		DriveSprinkler(0,-iTemp);
		while (ReadPrintEnd() == 0);//等待打印结束
	}
	*/
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
			break;
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

//打印处理
void DealPrint(void)
{
	switch (gl_cPrintStatus)
	{
		case	0:
			if ((gl_cDelayFlag == 1) && (ReadPrintEnd() != 0))
				gl_iWaitCount++;
			break;
		case	1:		//准备开始打印
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
				WriteDataToFIFO();	//写FIFO缓冲区数据,写满为止
				DriveSprinkler(0,gl_iPulseNumY);//驱动Y轴
				
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
					gl_cPrintStatus = 0;//4;
					if ((gl_cDelayFlag & 0x40) == 0)
					{
						gl_cDelayFlag = 1;
						gl_iWaitCount = 0;
						gl_iWaitCount1 = 0;
					}
					DriveSprinkler(gl_iPulseNumX,0);//驱动X轴走纸
					//Delay(1000);
					//Printf("Drive X Start\n");
					//WaitDriveEnd(AXIS_X);
					//Printf("X Drive end\n");
					gl_iAxisXPos += gl_iPulseNumX;//当前X向运动坐标
					gl_iPulseNumY = -gl_iPulseNumY;//反向
					//Printf("Move X\n");
				}
				else
				{
					DriveSprinkler(gl_iPulseNumX,0);//驱动Y轴回原点
					WaitDriveEnd(AXIS_X);
					DriveSprinkler(0, -gl_iPulseNumY);//
					//WaitDriveEnd(AXIS_Y);
					gl_iAxisYPos -= gl_iPulseNumY;
					gl_iAxisXPos += gl_iPulseNumX;//当前X向运动坐标
					//gl_iCurrentPos_Y = gl_iAxisYPos; //当前Y向运动坐标
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
			//SelecAxis(AXIS_Y);
			//if (ReadAxisStatus(AXIS_Y) == 0)
			if (ReadPrintEnd() != 0)
			{//Y轴返回打印结束
				Printf("Print end return org!\n");
				/*
				gl_cPrintStatus = 0;
				DriveSprinkler(gl_iPulseNumX,0);//驱动X轴走纸
				gl_iCurrentPos_X += gl_iPulseNumX;//当前X向运动坐标
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
	//送纸检测
	if ((gl_cReacterStatus & B_SENDPAPER) != 0)
	{//已启动送纸
		if ((GetIOBit(I_GIVEBACK) == StatusOk) || (gl_iSendPaper >= 625))
		{ //送纸位置到达或已送纸5s

				SendPaperClose=1;
				//201002023 屏蔽
				//SetIOBit(O_STARTGIVEPMOTOR,1);		//关闭送纸电机
				//gl_cReacterStatus &= (~B_SENDPAPER);
				//gl_iMovePaperLength = 0;

		}
		else if (GetIOBit(I_GIVEPEND) == StatusOk)//没纸
		{
			SetIOBit(O_STARTGIVEPMOTOR,1);		//关闭送纸电机
			gl_cReacterStatus &= (~B_SENDPAPER);
			gl_cReacterStatus |= B_NOPAPER;
		}
		else
			gl_iSendPaper++;
	}
	else if (((gl_cReacterStatus & B_NOPAPER) == 0) && (gl_cSelfCheckFlag == 0))
	{
		//送纸过冲20100223
		if(SendPaperStar == 0)
		if(GetIOBit(I_GIVEBACK) == StatusOk)
		{
			SendPaperStar = 1;
			gl_iMovePaperLength = 0;
		}
		//
		if (GetIOBit(I_GIVEPEND) == StatusOk)//没纸
			gl_cReacterStatus |= B_NOPAPER;
		else if (gl_iMovePaperLength < -44659)			//走纸超过660MM,送纸
		{
			if(SendPaperStar)
			if (GetIOBit(I_GIVEBACK) == StatusErr)
			{
				gl_cReacterStatus |= B_SENDPAPER;
				SetIOBit(O_STARTGIVEPMOTOR,0);		//启动送纸电机
				gl_iSendPaper = 0;								//送纸计数器清零
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
				SendPaperStar = 0;
		}
		else
			SendDelayTime += 1;
	}
	//
	
	//收纸检测
	if ((gl_cReacterStatus & B_ROLLPAPER) != 0)
	{//已开始收纸
		if ((GetIOBit(I_ROLLPEND) == StatusOk) || (gl_iRollPaper >= 625))
		{//收纸停位置到达或已收纸5s
			SetIOBit(O_STARTROLLPMOTOR,1);		//关闭收纸电机
			gl_cReacterStatus &= (~B_ROLLPAPER);
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
			}
		}
	}
	*/
}
