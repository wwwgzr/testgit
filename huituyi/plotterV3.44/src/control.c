#include "..\inc\defconst.h"
#include "..\Target\44blib.h"
#include "..\inc\extern.h"
#include <string.h>


void DriveSprinkler(int iPulseNumX,int iPulseNumY);
void InvertPrintData(void);
void DrawTestGraph(void);
void InitPaper(void);
int InitPlotter(void);
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
//**********************************************************
//功能：	读取IO位
//输入参数:	cBit要读取的IO位(0~7或0xff)
//返回值: 	1该IO位为1，0该IO位为0
//**********************************************************
unsigned char GetIOBit(unsigned char cBit)
{
	unsigned char cTemp;

	cTemp = ReadIO();
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

void InitPaper(void)
{
	int iPos,iStep;
	
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
				WaitPrintEnd();//等待打印结束
			}
			SetIOBit(O_STARTGIVEPMOTOR,0);//启动送纸
			iStep = 0;
			while (1)
			{
				ReceiveData();				//接收数据
				if ((GetIOBit(I_GIVEBACK) == StatusErr) && (iStep < 5000000))
				{
					iStep++;
					if (GetIOBit(I_GIVEPEND) == StatusOk)//没纸
					{
						gl_cReacterStatus |= B_NOPAPER;
						break;
					}
				}
				else
					break;
			}
			SetIOBit(O_STARTGIVEPMOTOR,1);//停止送纸
			break;
		}
		else if ((GetIOBit(I_GIVEBACK) == StatusOk) || (iPos < 226*iStep))
		{//不用送纸
			if (iPos != 0)
			{
				DriveSprinkler(-iPos,0);
				WaitPrintEnd();//等待打印结束
			}
			break;
		}
		else
		{
			DriveSprinkler(iStep,0);
			WaitPrintEnd();//等待打印结束
			iPos += iStep;
		}
	}
	gl_iMovePaperLength = 0;
}

extern void SetDeviceType(void);
extern void FlashChipSelect(void);

//初始化绘图仪
int InitPlotter()
{
	int iPos,iStep;
	int iMaxPulseNum;
	char  cErrCode;
	
	ClrFIFO;
	gl_cIOWriteValue = 0xff;
	WriteIO(gl_cIOWriteValue);

	//送纸
	iPos = 0;
	iStep = -3 * (int)X_PPMM;

	// 20111103 检测当前电机类型是无刷或是有刷，如果当前驱动器报警，应跳出循环，重新设置电机型号和Flash 型号
	cErrCode = gl_DeviceType == 0 ? 0x0 : 0x10; 
	if ((rPDATD & 0x10) == cErrCode)
	{//判断Y轴是否已经报警
		return 1;
	}

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
		else if ((GetIOBit(I_GIVEBACK) == StatusOk) || (iPos <  350*iStep)) // 20111027 
		{//不用送纸
			if (iPos != 0)
			{
				DriveSprinkler(-iPos,0);
				while (ReadPrintEnd() == 0);//等待打印结束
				
			}
			//20110308 改进送纸初始化过程，如果送纸杆在下感应器下方
			//此时应当启动收纸，将送纸杆停在下感应器上。
			//Printf("iPos=%d\n", iPos);
			if ((GetIOBit(I_GIVEBACK) == StatusErr) && (iPos<-gl_sSendPaperLength))
			{
				SetIOBit(O_GIVEMOTORDIR,G_ROLLPAPER);
				SetIOBit(O_STARTGIVEPMOTOR,0);//启动收纸
				iStep = 0;
				while((GetIOBit(I_GIVEBACK) == StatusErr) && (iStep < 9000000))
					iStep++;
				SetIOBit(O_STARTGIVEPMOTOR,1);//停止收纸
				SetIOBit(O_GIVEMOTORDIR,G_SENDPAPER);
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
			#if( LANGUAGE == 0 )
			strcpy(gl_cLcdDispBuf[0],"驱动报警");
			strcpy(gl_cLcdDispBuf[1],"请关机重试");
			#elif(LANGUAGE==1)
			strcpy(gl_cLcdDispBuf[0],"Drive Error!");
			strcpy(gl_cLcdDispBuf[1]," ");
			#elif(LANGUAGE==2)
			strcpy(gl_cLcdDispBuf[0],"Error de Driver");
			strcpy(gl_cLcdDispBuf[1]," ");
			#elif(LANGUAGE==3)
			strcpy(gl_cLcdDispBuf[0],"Surucu Hatasi!");
			strcpy(gl_cLcdDispBuf[1]," ");
			#else
			strcpy(gl_cLcdDispBuf[0],"Drive Error!");
			strcpy(gl_cLcdDispBuf[1]," ");
			#endif
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
			#if( LANGUAGE == 0 )
			strcpy(gl_cLcdDispBuf[0],"驱动报警");
			strcpy(gl_cLcdDispBuf[1],"请关机重试");
			#elif(LANGUAGE==1)
			strcpy(gl_cLcdDispBuf[0],"Drive Error!");
			strcpy(gl_cLcdDispBuf[1]," ");
			#elif(LANGUAGE==2)
			strcpy(gl_cLcdDispBuf[0],"Error de Driver");
			strcpy(gl_cLcdDispBuf[1]," ");
			#elif(LANGUAGE==3)
			strcpy(gl_cLcdDispBuf[0],"Surucu Hatasi!");
			strcpy(gl_cLcdDispBuf[1]," ");
			#else
			strcpy(gl_cLcdDispBuf[0],"Drive Error!");
			strcpy(gl_cLcdDispBuf[1]," ");
			#endif
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

	return 0;
}

void DriveSprinkler(int iPulseNumX,int iPulseNumY)
{
	// 有刷报警代码0x0，无刷报警代码0x10，20111101
	char  cErrCode;
	cErrCode = gl_DeviceType == 0 ? 0x0 : 0x10; 
	if ((rPDATD & 0x10) == cErrCode)
	{//判断Y轴是否已经报警
		gl_cPrintStatus = 0x10;
		return;
	}
	//iPulseNumY = -iPulseNumY;
	WriteDL16;
	WritePulseX((short)(iPulseNumX&0xffff));
	WriteDH16;
	WritePulseX((short)((iPulseNumX>>16)&0xffff));
	WriteDL16;
	WritePulseY((short)(iPulseNumY&0xffff));
	WriteDH16;
	WritePulseY((short)((iPulseNumY>>16)&0xffff));
	WriteDL16;
	DrivePrint;
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
				iWriteAddr -= (iCountByte + 26);
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
					iWriteAddr -= iCountByte;
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
						sdat1 = Readsdram(ReadAddr);
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
						sdat1 = Readsdram(ReadAddr);
						sdat2 = sdat1 >> gl_cRemainderBit;
						idat1 = (unsigned int)sdat2;
						idat1 = idat1 << cMultiDotLine[n];
						sdat2 = Readsdram(ReadAddr1);
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
						sdat1 = Readsdram(ReadAddr);
						sdat2 = sdat1 >> gl_cRemainderBit;
						idat1 = (unsigned int)sdat2;
						idat1 = idat1 << cMultiDotLine[n];
						sdat2 = Readsdram(ReadAddr1);
						idat2 = (unsigned int)sdat2;
						sdat2 = Readsdram(ReadAddr2);
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
		for (iCount=0;iCount<idat1;iCount++)
		{
			for (j=0;j<14;j++)
			{
				Writesdram(iWriteAddr,0);
				iWriteAddr += 2;
			}
			iWriteAddr -= (iCountByte + 26);
		}
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
	
	bakWriteAddr = iWriteAddr + (iCountByte + 2) * (gl_iGraphWidth * 8 + AdditionDot + OriginalSideDot);
	for (n=0;n<(gl_cSprinklerCount-1);n++)
		bakWriteAddr = bakWriteAddr + (iCountByte + 2) * iYDotSpace[n];
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
						sdat1 = Readsdram(ReadAddr);
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
						sdat1 = Readsdram(ReadAddr);
						sdat2 = sdat1 >> gl_cRemainderBit;
						idat1 = (unsigned int)sdat2;
						idat1 = idat1 << cMultiDotLine[n];
						sdat2 = Readsdram(ReadAddr1);
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
						sdat1 = Readsdram(ReadAddr);
						sdat2 = sdat1 >> gl_cRemainderBit;
						idat1 = (unsigned int)sdat2;
						idat1 = idat1 << cMultiDotLine[n];
						sdat2 = Readsdram(ReadAddr1);
						idat2 = (unsigned int)sdat2;
						sdat2 = Readsdram(ReadAddr2);
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
//				Writesdram(iWriteAddr,0);
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
}

//将接收到的数据转换成打印格式
void InvertPrintData(void)
{
	unsigned int iWriteAddr;

	if (gl_cPrintStatus != 2)
	{
		if (gl_cCurPrintBuf == 0)
		{
			if((gl_cInvertBufStatus&PrintBit) != 0)
			{
				gl_cInvertBufStatus &= (~Buf1Presetbit);
				gl_cInvertBufStatus &= (~PrintBit);
			}
		}
		else
		{
			if ((gl_cInvertBufStatus&PrintBit) == 0)
			{
				gl_cInvertBufStatus &= (~Buf0Presetbit);
				gl_cInvertBufStatus |= PrintBit;
			}
		}
	}
	if (gl_cPrintStatus == 0)
	{//当前行打印结束
		if (gl_cCurPrintBuf == 0)
		{
			if ((gl_cInvertBufStatus&Buf0Presetbit) != 0)
			{
				gl_iReadDataAddr = InvertDataAddr0;
				gl_iDataEndAddr = gl_iDataEndAddr0;
				gl_cPrintStatus = 1;		//启动打印
			}
			else if ((gl_cFileReceive & 0x20) == 0x20)
			{
				gl_cFileReceive = 0;
				gl_cInvertBufStatus = 0;
				gl_cCommunicationMode = 0xff;
				return;
			}
		}
		else
		{
			if ((gl_cInvertBufStatus&Buf1Presetbit) != 0)
			{
				gl_iReadDataAddr = InvertDataAddr1;
				gl_iDataEndAddr = gl_iDataEndAddr1;
				gl_cPrintStatus = 1;
			}
			else if ((gl_cFileReceive & 0x20) == 0x20)
			{
				if (ReadPrintEnd() == 0)
					return;
				else
				{
					gl_cFileReceive = 0;
					gl_cCurPrintBuf = 0;
					gl_cInvertBufStatus = 0;
					gl_cCommunicationMode = 0xff;
					if (gl_iPulseNumY < 0)
					{
						ClrFIFO;
						DriveSprinkler(0,gl_iPulseNumY);//驱动Y轴
						gl_iAxisYPos += gl_iPulseNumY;
						gl_iPulseNumY = -gl_iPulseNumY;
					}
					return;
				}
			}
		}
	}
	if ((gl_cFileReceive&0x20) == 0x20)
		return;
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
					#if(LANGUAGE==0)
					lcd_display(0,"驱动报警");
					lcd_display(1,"7:退出");
					#elif(LANGUAGE==1)
					lcd_display(0,"Drive Error!");
					lcd_display(1,"7:EXIT");
					#elif(LANGUAGE==2)
					strcpy(gl_cLcdDispBuf[0],"Error de Driver");
					strcpy(gl_cLcdDispBuf[1]," ");
					#elif(LANGUAGE==3)
					strcpy(gl_cLcdDispBuf[0],"Surucu Hatasi!");
					strcpy(gl_cLcdDispBuf[1]," ");
					#else
					strcpy(gl_cLcdDispBuf[0],"Drive Error!");
					strcpy(gl_cLcdDispBuf[1]," ");
					#endif
					WaitKeyExit(Key_Pause);
					lcd_refresh();
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
					#if(LANGUAGE==0)
					lcd_display(0,"驱动报警");
					lcd_display(1,"7:退出");
					#elif(LANGUAGE==1)
					lcd_display(0,"Drive Error!");
					lcd_display(1,"7:EXIT");
					#elif(LANGUAGE==2)
					strcpy(gl_cLcdDispBuf[0],"Error de Driver");
					strcpy(gl_cLcdDispBuf[1]," ");
					#elif(LANGUAGE==3)
					strcpy(gl_cLcdDispBuf[0],"Surucu Hatasi!");
					strcpy(gl_cLcdDispBuf[1]," ");
					#else
					strcpy(gl_cLcdDispBuf[0],"Drive Error!");
					strcpy(gl_cLcdDispBuf[1]," ");;
					#endif
					WaitKeyExit(Key_Pause);
					lcd_refresh();
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
	#if(LANGUAGE==0)
	lcd_display(0,"正在打印");
	lcd_display(1,"请稍后...");
	#elif(LANGUAGE==1)
	lcd_display(0,"Printing!");
	lcd_display(1," ");
	#elif(LANGUAGE==2)
	lcd_display(0,"Imprmiendo...");
	lcd_display(1," ");
	#elif(LANGUAGE==3)
	lcd_display(0,"Ciziliyor!...");
	lcd_display(1," ");
	#else
	lcd_display(0,"Imprimindo!...");
	lcd_display(1," ");
	#endif
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
	#if(LANGUAGE==0)
	lcd_display(0,"错位图形打印");
	lcd_display(1,"请稍后...");
	#elif(LANGUAGE==1)
	lcd_display(0,"Printing!");
	lcd_display(1," ");
	#elif(LANGUAGE==2)
	lcd_display(0,"Imprmiendo...");
	lcd_display(1," ");
	#elif(LANGUAGE==3)
	lcd_display(0,"Ciziliyor!...");
	lcd_display(1," ");
	#else
	lcd_display(0,"Imprimindo!...");
	lcd_display(1," ");
	#endif
	
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
// 清洗打印头
void ClearPrintHead(unsigned int iFlag)
{
	int iTemp;
	//unsigned int address, i;
	//unsigned char cTemp1,cTemp2,cTemp3,cTemp4;
	//unsigned int sdat[5]={0xffff,0,0,0,0}; 
	
	if (iFlag == 1)
		iTemp = gl_iAxisYPos - (OriginalSideDot + 160) * DotPulseNum;
	else
		iTemp = gl_iAxisYPos;
		
	if (gl_iAxisYPos != 0)
	{
		DriveSprinkler(0,-gl_iAxisYPos);
		while (ReadPrintEnd() == 0);//等待打印结束
	}

	PrintHeadClean(gl_sPrintHeadCleanCount);
	
	/*
	WriteFilePrint(0xaa);
	ClrFIFO;
	WriteDL16;
	//20110221 增加打印头清洗次数
	for(i=0; i<gl_sPrintHeadCleanCount; i++)
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
	*/
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
}

//打印测试图形(测试喷嘴好坏)
void DrawTestGraph(void)
{
	unsigned int address;
	unsigned int i,j,k;
	unsigned int icount;
	int iPosNumY;
	int iGapPixel;
	unsigned char cSprinklerNum;
	unsigned int sdat[7];
	int iPosNumX;
	#if( LANGUAGE == 0)
	strcpy(gl_cLcdDispBuf[0],"测试图形打印");
	strcpy(gl_cLcdDispBuf[1],"请稍后...");
	#elif(LANGUAGE==1)
	strcpy(gl_cLcdDispBuf[0],"Printing!");
	strcpy(gl_cLcdDispBuf[1]," ");
	#elif(LANGUAGE==2)
	strcpy(gl_cLcdDispBuf[0],"Imprmiendo...");
	strcpy(gl_cLcdDispBuf[1]," ");
	#elif(LANGUAGE==3)
	strcpy(gl_cLcdDispBuf[0],"Ciziliyor!...");
	strcpy(gl_cLcdDispBuf[1]," ");
	#else
	strcpy(gl_cLcdDispBuf[0],"Imprimindo!...");
	strcpy(gl_cLcdDispBuf[1]," ");
	#endif
	lcd_refresh();

	ClearPrintHead(0);
	
	iGapPixel = (int)(50.0 * Y_PPMM / DotPulseNum);
	iPosNumY = (15 * iGapPixel + 3 + 320) * DotPulseNum;
	if (((gl_iAxisYPos + iPosNumY) < 0) || ((gl_iAxisYPos + iPosNumY) > gl_iMaxYMM))
	{
		#if( LANGUAGE == 0)
		strcpy(gl_cLcdDispBuf[1]," ");
		strcpy(gl_cLcdDispBuf[0]," Y轴移动超界!");
		#elif(LANGUAGE==1)
		strcpy(gl_cLcdDispBuf[1]," ");
		strcpy(gl_cLcdDispBuf[0],"Y:out-of-range");
		#elif(LANGUAGE==2)
		strcpy(gl_cLcdDispBuf[1]," ");
		strcpy(gl_cLcdDispBuf[0],"Y Fuera d rango");
		#elif(LANGUAGE==3)
		strcpy(gl_cLcdDispBuf[1]," ");
		strcpy(gl_cLcdDispBuf[0],"Y:Kontur Disi!");
		#else
		strcpy(gl_cLcdDispBuf[1]," ");
		strcpy(gl_cLcdDispBuf[0],"Y:ForaDoLimite");
		#endif
		lcd_refresh();
		WaitKeyExit(Key_Pause);
		return;
	}
	WriteFilePrint(0xaa);
	ClrFIFO;
	iPosNumX = -(unsigned int)(50.0 * DotLineWidth * X_PPMM);
	for (cSprinklerNum=0;cSprinklerNum<gl_cSprinklerCount;cSprinklerNum++)
	{              
		WriteFIFOFull(gl_cSprinklerNo+cSprinklerNum);
		sdat[0] = 0x8001;
		sdat[1] = 0x400;
		sdat[2] = 0x40008;
		sdat[3] = 0x800;
		sdat[4] = 0x80010;
		sdat[5] = 0x1000;
		sdat[6] = 0x20;
		ClrFIFO;		//清FIFO缓冲区
		for (i=0;i<3;i++)
		{
			address = gl_iWriteFIFOAddr + 28 * cSprinklerNum;
			for (j=0;j<7;j++)
			{//画边框
				WriteDL16;
				WriteFIFO(address,0xffff);
				WriteDH16;
				WriteFIFO(address,0x3f);
				WriteDL16;
				address += 4;
			}
		}
		while (ReadFIFOFull() == 0)//缓冲区不满
		{
			address = gl_iWriteFIFOAddr + 28 * cSprinklerNum;
			for (j=0;j<7;j++)
			{
				WriteDL16;
				WriteFIFO(address,(unsigned short)sdat[j]);
				WriteDH16;
				WriteFIFO(address,(unsigned short)(sdat[j]>>16));
				WriteDL16;
				address += 4;
			}
			i++;
		}
		DriveSprinkler(0,iPosNumY);
		gl_iAxisYPos += iPosNumY;
		iPosNumY = -iPosNumY;
		for (icount=0;icount<15;icount++)
		{
			if (ReadPrintEnd() != 0)
				break;
			for (;i<iGapPixel;i++)
			{
				while (ReadFIFOFull() == 1)
				{//缓冲区满
					if (ReadPrintEnd() != 0)
						break;
				}
				if (ReadPrintEnd() != 0)
					break;
				address = gl_iWriteFIFOAddr + 28 * cSprinklerNum;
				for (j=0;j<7;j++)
				{
					WriteDL16;
					WriteFIFO(address,(unsigned short)sdat[j]);
					WriteDH16;
					WriteFIFO(address,(unsigned short)(sdat[j]>>16));
					WriteDL16;
					address += 4;
				}
			}
			for (i=0;i<3;i++)
			{
				while (ReadFIFOFull() == 1)
				{//缓冲区满
					if (ReadPrintEnd() != 0)
						break;
				}
				if (ReadPrintEnd() != 0)
					break;
				address = gl_iWriteFIFOAddr + 28 * cSprinklerNum;
				for (j=0;j<7;j++)
				{
					WriteDL16;
					WriteFIFO(address,0xffff);
					WriteDH16;
					WriteFIFO(address,0x3f);
					WriteDL16;
					address += 4;
				}
			}
			sdat[0] <<= 1;
			if ((sdat[0] & 0x100000) != 0)
				sdat[1] = (sdat[1] << 1) | 0x01;
			else
				sdat[1] = sdat[1] << 1;
			sdat[0] &= 0xfffff;
			for (k=1;k<6;k++)
			{
				if ((sdat[k] & 0x400000) != 0)
					sdat[k+1] = (sdat[k+1] << 1) | 0x01;
				else
					sdat[k+1] = sdat[k+1] << 1;
				sdat[k] &= 0x3fffff; 
			}
			sdat[6] &= 0xfffff;
		}
		WaitPrintEnd();
		DriveSprinkler(iPosNumX,0);
		WaitPrintEnd();
		DriveSprinkler(0,iPosNumY);
		gl_iAxisYPos += iPosNumY;
		iPosNumY = -iPosNumY;
		WaitPrintEnd();
	}
	iPosNumX = -(unsigned int)((float)SprinklerWidth * DotLineWidth * X_PPMM);
	iPosNumX += gl_iPulseNumX;
	DriveSprinkler(iPosNumX,0);
	WaitPrintEnd();
}

//写数据到FIFO，写满为止
void WriteDataToFIFO(void)
{
	unsigned short dat;
	unsigned char i,j;
	unsigned int address;
	
	i = gl_cSprinklerCount * 7;
	WriteDL16;
	
	while (1)
	{
		if (ReadFIFOFull() == 1)//缓冲区满
			break;
		else if (gl_iReadDataAddr >= gl_iDataEndAddr)
		{//行打印数据已经发送完毕,最后写0
			address = gl_iWriteFIFOAddr;
			for (j=0;j<i;j++)
			{
				WriteFIFO(address,0);
				WriteDH16;
				WriteFIFO(address,0);
				WriteDL16;
				address += 4;
			}
			break;
		}
		else
		{//发送1排打印数据
			address = gl_iWriteFIFOAddr;
			for (j=0;j<i;j++)
			{
				dat = Readsdram(gl_iReadDataAddr);
				gl_iReadDataAddr += 2;
				WriteFIFO(address,dat);
				WriteDH16;
				dat = Readsdram(gl_iReadDataAddr);
				gl_iReadDataAddr += 2;
				WriteFIFO(address,dat);
				WriteDL16;
				address += 4;
			}
		}
	}
}

//20100223
static char SendPaperClose = 0;
static char SendDelayTime = 0;
static char SendPaperStar = 1;
//static char startBool=0;

//打印处理
void DealPrint(void)
{
	int SendPaperCount;
	
	switch (gl_cPrintStatus)
	{
		case	0:
			if ((gl_cDelayFlag == 1) && (ReadPrintEnd() != 0))
				gl_iWaitCount++;
			break;
		case	1:		//准备开始打印
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
				WriteDataToFIFO();	//写FIFO缓冲区数据,写满为止
				DriveSprinkler(0,gl_iPulseNumY);//驱动Y轴
				gl_iAxisYPos += gl_iPulseNumY;
				gl_cPrintStatus = 2;
			}
			break;
		case	2:		//Y轴打印中
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
					gl_iPulseNumY = -gl_iPulseNumY;//反向
				}
				else
				{
					DriveSprinkler(gl_iPulseNumX,-gl_iPulseNumY);//驱动Y轴回原点
					gl_iAxisYPos -= gl_iPulseNumY;
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
				gl_cPrintStatus = 0;
				DriveSprinkler(gl_iPulseNumX,0);//驱动X轴走纸
			}
			break;
		default:
			break;
	}
	//送纸检测
	if ((gl_cReacterStatus & B_SENDPAPER) != 0)
	{//已启动送纸
		// 20110311 根据走纸长度设定送纸到达计数器，高机架送纸
		// 5s若未检测到送纸停感应器则停止送纸，矮机架若送纸
		// 3s未检测到送纸停感应器则停止送纸。
		if(gl_sSendPaperLength < 44659)
			SendPaperCount = 375;// 3s
		else
			SendPaperCount = 625;//5s
			
		if ( (gl_iSendPaper >= SendPaperCount))
		{
			SetIOBit(O_STARTGIVEPMOTOR,1);		//关闭送纸电机
			gl_cReacterStatus &= (~B_SENDPAPER);
			gl_iMovePaperLength = 0;		
			//SendPaperStar = 1;
			//Printf("Send paper > 3S , At once close Send paper!\n");
		}
		else if (GetIOBit(I_GIVEBACK) == StatusOk)
		{ 

				//if(gl_sSendPaperDelay == 1)
				//{
				//	SetIOBit(O_STARTGIVEPMOTOR,1);		//关闭送纸电机
				//	gl_cReacterStatus &= (~B_SENDPAPER);
				//	gl_iMovePaperLength = 0;	
					//Printf("At once close Send paper!SendPaperClose:%d\n",SendPaperClose);
				//}
				//else
				//{
					SendPaperClose=1;
					//Printf("Delay [%d]ms close Send paper!\n", gl_sSendPaperDelay*8);
				//}
		}
		else if (GetIOBit(I_GIVEPEND) == StatusOk)//没纸
		{
			SetIOBit(O_STARTGIVEPMOTOR,1);		//关闭送纸电机
			gl_cReacterStatus &= (~B_SENDPAPER);
			gl_cReacterStatus |= B_NOPAPER;
			SendPaperStar = 1;
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
			//Printf("Inductor ward off!\n");
		}
		//***************
		if (GetIOBit(I_GIVEPEND) == StatusOk)//没纸
		{
			gl_cReacterStatus |= B_NOPAPER;
			SendPaperStar = 1;
		}
		else if (gl_iMovePaperLength < -gl_sSendPaperLength)			//  20100919走纸超过160MM*67.66552=10826,  // 走纸超过560MM*67.66552=37892, 送纸//走纸超过660MM,送纸44659pps
		{
			if(SendPaperStar)
			if (GetIOBit(I_GIVEBACK) == StatusErr)
			{
				gl_cReacterStatus |= B_SENDPAPER;
				SetIOBit(O_STARTGIVEPMOTOR,0);		//启动送纸电机
				gl_iSendPaper = 0;								//送纸计数器清零
				//Printf("Start Send paper! 0\n");
			}
		}
	}
	//20100223 送纸到达,延时关闭电机
	if(SendPaperClose)
	{
		if(SendDelayTime>gl_sSendPaperDelay)
		{
			SendPaperClose = 0;
			SendDelayTime = 0;
			SetIOBit(O_STARTGIVEPMOTOR,1);		//关闭送纸电机
			gl_cReacterStatus &= (~B_SENDPAPER);
			gl_iMovePaperLength = 0;
			if(GetIOBit(I_GIVEBACK) == StatusErr)
				SendPaperStar = 0;
			//Printf("Stop send paper!SendPaperStar = [%d]\n",SendPaperStar);
		}
		else
			SendDelayTime += 1;
	}
	//******************
	
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
}
