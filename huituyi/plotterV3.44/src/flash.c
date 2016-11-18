//使用的如果是老主板得屏蔽掉 20110817
//SST39VF3201		老板子
//SST39VF3201B		新板子2011,7开始使用
//#define USING3201B

#include "..\inc\defconst.h"
#include "..\Target\44blib.h"
#include "..\inc\extern.h"

void	Waitfor_endofprg(void);
void	SST39VF320_WordProg(unsigned int iAddress,unsigned short sData);
void	ReadParameter(void);
void	CalVariableValue(void);
int	SST39VF320_CheckBlank(unsigned int iAddress,unsigned int iWordSize);
int SST39VF320_SectorErase(unsigned int iSAaddr);
int WriteToFlash(unsigned int address);
int WriteParameter(void);
int VerifyCodeAdd(unsigned int address);
int ReadFromFlash(unsigned int address);

//检测flash是否为空，1为空；0不为空
int SST39VF320_CheckBlank(unsigned int iAddress,unsigned int iWordSize) 
{
	unsigned int i;
	unsigned short sTemp;
	for (i=iAddress;i<(iAddress+iWordSize);i++)
	{
		sTemp = Readflash(i);
		if (sTemp != 0xffff)
			return 0;
	}
	return 1;	
}

//等待操作完毕
void Waitfor_endofprg(void) //Check if the bit6 toggle ends.
{
	unsigned short sFlashStatus,sOld;
	
	sOld = Readflash(0);
	while(1)
	{
		sFlashStatus = Readflash(0);//*((volatile INT16U *)0xff800);//
		if ((sOld&0x40) == (sFlashStatus&0x40))
		{
			sFlashStatus = Readflash(0);//*((volatile INT16U *)0xff800);//
			sOld = Readflash(0);
			if((sOld&0x40) == (sFlashStatus&0x40))
				break;
		}
		else
		  sOld = sFlashStatus;
	}
	return;
}
//写
void SST39VF320_WordProg (unsigned int iAddress,unsigned short sData)
{
//#ifdef USING3201B
	if(gl_FlashChipSelect==0)
	{
		Writeflash(0x555,0xAA);
		Writeflash(0x2AA,0x55);
		Writeflash(0x555,0xA0);
		Writeflash(iAddress,sData);
	}
	else
	{
//#else
		Writeflash(0x5555,0xAA);
		Writeflash(0x2AAA,0x55);
		Writeflash(0x5555,0xA0);
		Writeflash(iAddress,sData);
	}
//#endif
	Waitfor_endofprg();
}
//扇区擦除
int  SST39VF320_SectorErase(unsigned int iSAaddr)
{
//#ifdef USING3201B
	if(gl_FlashChipSelect==0)
	{
		Writeflash(0x555,0xAA);
		Writeflash(0x2AA,0x55);
		Writeflash(0x555,0x80);
		Writeflash(0x555,0xAA);
		Writeflash(0x2AA,0x55);
		Writeflash(iSAaddr,0x50);
	}
	else
	{
//#else
		Writeflash(0x5555,0xAA);
		Writeflash(0x2AAA,0x55);
		Writeflash(0x5555,0x80);
		Writeflash(0x5555,0xAA);
		Writeflash(0x2AAA,0x55);
		Writeflash(iSAaddr,0x30);
	}
//#endif
	Waitfor_endofprg();
	return(SST39VF320_CheckBlank(iSAaddr,SectorSize));
}

//从Flash中读取设置参数,如果效验和不正确则将所有参数付初值
void ReadParameter(void)
{
	int iBakrINTMSK;
	int iFlag;
	
	iBakrINTMSK = rINTMSK;
	rINTMSK = BIT_GLOBAL;		//屏蔽所有中断
	rTCON = 0x0;			  //all_stop
	rSYSCFG = SYSCFG_0KB;
	
	if (VerifyCodeAdd(SetParameterAddr) == 1)
	{
		iFlag = ReadFromFlash(SetParameterAddr);
	}
	else if (VerifyCodeAdd(SetParameterAddr+SectorSize) == 1)
	{
		iFlag = ReadFromFlash(SetParameterAddr+SectorSize);
	}
	else
	{
		iFlag = 0;
	}

	if (iFlag == 0)
	{
		gl_cSprinklerCount = 1;				//喷头数
		gl_cSprinklerNo = 0;
		gl_cBidirectional = 1;				//双向打印
		gl_cLineWidth = 2;
		gl_cMultiDotLine12 = 0;
		gl_cMultiDotLine23 = 0;
		gl_cMultiDotLine34 = 0;
		MyIP[0] = 0xC0A8;
		MyIP[1] = 0x0174;
		SubnetMask[0] = 0xffff;
		SubnetMask[1] = 0xff00;
		GatewayIP[0] = 0xC0A8;
		GatewayIP[1] = 0x0101;
		gl_iOriginalPos = (int)(200 * Y_PPMM);
		gl_iYDotSpace12 = SprinklerSpace;
		gl_iYDotSpace23 = SprinklerSpace;
		gl_iYDotSpace34 = SprinklerSpace;
		gl_dErrorX = 0;
		gl_dErrorY = 0;
		gl_dPictureErrorX = 1000.0;
		gl_dPictureErrorY = 1000.0;
		gl_sPlotType = 1350;
		SprinklerNum = 2;   // 20101214   喷头数可选主板设置的喷头数
		gl_sSendPaperLength = 20299; //20110221 送纸长度默认10826个脉冲,合160MM
		gl_sSendPaperDelay = 6;   // 20110221 默认延迟6*8mS
		gl_sPrintHeadCleanCount = 1; // 20110221 设置打印头清洗次数
		gl_DeviceType = 0; // 20111101 电机设置 1无刷电机 默认有刷电机
		gl_FlashChipSelect = 0 ; // 20111101 Flash 芯片选择 0=3201B
		//Printf("Check 1 Write sprinklerNum = %d\n", SprinklerNum);
		WriteToFlash(SetParameterAddr);
	}
	rSYSCFG = CACHECFG;
	rTCON = 0x0a;				//update T0
	rTCON = 0x09;				//T0=one shot,all_start 
	rINTMSK = iBakrINTMSK;	//恢复中断
	gl_iMaxYMM = (gl_sPlotType + 3) * Y_PPMM;
	gl_iGraphWidth = (int)((double)(gl_iMaxYMM - gl_iOriginalPos) / (double)(8 * DotPulseNum));	
	gl_iMaxYMM += (3 * (MaxMalposedDot + SprinklerSpace) + 2 * AdditionDot + OriginalSideDot + MotorSideDot + 320) * DotPulseNum;
	gl_iStaggerDot = (int)(gl_dErrorY * Y_PPMM / DotPulseNum);
	CalVariableValue();
}

//计算其它全局变量的值
void CalVariableValue(void)
{
	gl_iMaxPictureWidth = (gl_sPlotType + 3) * Y_PPMM - gl_iOriginalPos + 3 * (MaxMalposedDot + SprinklerSpace) * DotPulseNum;;
	gl_iWriteFIFOAddr = WriteFIFOAddr + gl_cSprinklerNo * 28;
	switch (gl_cSprinklerCount)
	{
		case 1:
			gl_iLineDataNum = SprinklerWidth;
			gl_iTHPulseNumX = (unsigned int)(((float)SprinklerWidth * DotLineWidth) * X_PPMM);
			gl_iPulseNumY = 0;
			break;
		case 2:
			if (gl_cSprinklerNo == 0)
			{
				gl_iLineDataNum = SprinklerWidth * 2 - gl_cMultiDotLine12;
				gl_iTHPulseNumX = (unsigned int)(((float)(SprinklerWidth * 2 - gl_cMultiDotLine12) * DotLineWidth) * X_PPMM);
				gl_iMaxPictureWidth -= gl_iYDotSpace12 * DotPulseNum;
				gl_iPulseNumY = gl_iYDotSpace12;
			}
			else if (gl_cSprinklerNo == 1)
			{
				gl_iLineDataNum = SprinklerWidth * 2 - gl_cMultiDotLine23;
				gl_iTHPulseNumX = (unsigned int)(((float)(SprinklerWidth * 2 - gl_cMultiDotLine23) * DotLineWidth) * X_PPMM);
				gl_iMaxPictureWidth -= gl_iYDotSpace23 * DotPulseNum;
				gl_iPulseNumY = gl_iYDotSpace23;
			}
			else
			{
				gl_iLineDataNum = SprinklerWidth * 2 - gl_cMultiDotLine34;
				gl_iTHPulseNumX = (unsigned int)(((float)(SprinklerWidth * 2 - gl_cMultiDotLine34) * DotLineWidth) * X_PPMM);
				gl_iMaxPictureWidth -= gl_iYDotSpace34 * DotPulseNum;
				gl_iPulseNumY = gl_iYDotSpace34;
			}
			break;
		case 3:
			if (gl_cSprinklerNo == 0)
			{
				gl_iLineDataNum = SprinklerWidth * 3 - gl_cMultiDotLine12 - gl_cMultiDotLine23;
				gl_iTHPulseNumX = (unsigned int)(((float)(SprinklerWidth * 3 - gl_cMultiDotLine12 - gl_cMultiDotLine23) * DotLineWidth) * X_PPMM);
				gl_iMaxPictureWidth -= (gl_iYDotSpace12 + gl_iYDotSpace23) * DotPulseNum;
				gl_iPulseNumY = gl_iYDotSpace12 + gl_iYDotSpace23;
			}
			else
			{
				gl_iLineDataNum = SprinklerWidth * 3 - gl_cMultiDotLine23 - gl_cMultiDotLine34;
				gl_iTHPulseNumX = (unsigned int)(((float)(SprinklerWidth * 3 - gl_cMultiDotLine23 - gl_cMultiDotLine34) * DotLineWidth) * X_PPMM);
				gl_iMaxPictureWidth -= (gl_iYDotSpace23 + gl_iYDotSpace34) * DotPulseNum;
				gl_iPulseNumY = gl_iYDotSpace23 + gl_iYDotSpace34;
			}
			break;
		case 4:
			gl_iLineDataNum = SprinklerWidth * 4 - gl_cMultiDotLine12 - gl_cMultiDotLine23 - gl_cMultiDotLine34;
			gl_iTHPulseNumX = (unsigned int)(((float)(SprinklerWidth * 4 - gl_cMultiDotLine12 - gl_cMultiDotLine23 - gl_cMultiDotLine34) * DotLineWidth) * X_PPMM);
			gl_iMaxPictureWidth -= (gl_iYDotSpace12 + gl_iYDotSpace23 + gl_iYDotSpace34) * DotPulseNum;
			gl_iPulseNumY = gl_iYDotSpace12 + gl_iYDotSpace23 + gl_iYDotSpace34;
			break;
		default:
			gl_cSprinklerCount = 1;
			gl_iLineDataNum = SprinklerWidth;
			gl_iTHPulseNumX = (unsigned int)(((float)SprinklerWidth * DotLineWidth) * X_PPMM);
			gl_iPulseNumY = 0;
			break;
	}
	WriteFIFOFull(gl_cSprinklerNo+gl_cSprinklerCount-1);
	gl_iMaxPictureWidth /= (double)(DotPulseNum * 8);
	gl_iPulseNumY = (gl_iPulseNumY + gl_iGraphWidth * 8 + AdditionDot * 2 + OriginalSideDot + MotorSideDot + 320) * DotPulseNum;
	gl_iPulseNumX = -gl_iTHPulseNumX + (int)(gl_dErrorX * X_PPMM);
}

//判断设置参数存储值是否正确
//0失败,1成功
int VerifyCodeAdd(unsigned int address)
{
	unsigned int iCodeAdd1,iCodeAdd2;
	unsigned int i;
	
	iCodeAdd1 = 0;
	iCodeAdd2 = SectorSize-2;

	for(i=0;i<iCodeAdd2;i++)
		iCodeAdd1 += Readflash(address++);

	iCodeAdd2 = (unsigned int)Readflash(address++) << 16;
	iCodeAdd2 += Readflash(address);

	if (iCodeAdd1 == iCodeAdd2)
		return 1;
	else
		return 0;
}

int ReadFromFlash(unsigned int address)
{
	unsigned int i,j;
	unsigned short *sTemp;
	PrintSetPara PrintPara;

	sTemp = (unsigned short *)&PrintPara;
	j = (sizeof(PrintPara) + 1) / 2;
	for (i=0;i<j;i++)
	{
		*sTemp = Readflash(address++);
		sTemp++;
	}
	// 20101214 机型喷头数只与读写参数相关
	if((PrintPara.cSprnklerNum == 0) ||(PrintPara.cSprnklerNum>4))
		return 0;
	SprinklerNum = PrintPara.cSprnklerNum;

	if ((PrintPara.cSprinklerCount > SprinklerNum) || (PrintPara.cSprinklerCount == 0)
		|| (PrintPara.cSprinklerNo > (SprinklerNum - PrintPara.cSprinklerCount)))
		return 0;
	if ((PrintPara.cBidirectional != 0) && (PrintPara.cBidirectional != 1))
		return 0;
	if ((PrintPara.cLineWidth < 1) || (PrintPara.cLineWidth > 9))
		return 0;
	if (PrintPara.cMultiDotLine12 >= SprinklerWidth)
		return 0;
	if (PrintPara.cMultiDotLine23 >= SprinklerWidth)
		return 0;
	if (PrintPara.cMultiDotLine34 >= SprinklerWidth)
		return 0;
	// 最大可设置为3030mm   20100926
	if (
			(PrintPara.sPlotType != 1350) 
		&& (PrintPara.sPlotType != 1630) 
		&& (PrintPara.sPlotType != 1830) 
		&& (PrintPara.sPlotType != 2030) 
		&& (PrintPara.sPlotType != 2230) 
		&& (PrintPara.sPlotType != 2430)
		&& (PrintPara.sPlotType != 2630) 
		&& (PrintPara.sPlotType != 2830)
		&& (PrintPara.sPlotType != 3030)
		)
		return 0;

	if ((PrintPara.iOriginalPos < 0) || (PrintPara.iOriginalPos > PrintPara.sPlotType * Y_PPMM))
		return 0;
	if ((PrintPara.iYDotSpace12 > (SprinklerSpace + MaxMalposedDot)) || (PrintPara.iYDotSpace12 < (SprinklerSpace - MaxMalposedDot)))
		return 0;
	if ((PrintPara.iYDotSpace23 > (SprinklerSpace + MaxMalposedDot)) || (PrintPara.iYDotSpace23 < (SprinklerSpace - MaxMalposedDot)))
		return 0;
	if ((PrintPara.iYDotSpace34 > (SprinklerSpace + MaxMalposedDot)) || (PrintPara.iYDotSpace34 < (SprinklerSpace - MaxMalposedDot)))
		return 0;
	if (PrintPara.dErrorX > PrintPara.cSprinklerCount * SprinklerWidth * DotLineWidth)
		return 0;
	else if (PrintPara.dErrorX < -PrintPara.cSprinklerCount * SprinklerWidth * DotLineWidth)
		return 0;
	if ((PrintPara.dErrorY > 6.0) || (PrintPara.dErrorY < -6.0))
		return 0;
	if ((PrintPara.dPictureErrorX > 1100.0) || (PrintPara.dPictureErrorX < 900.0))
		return 0;
	if ((PrintPara.dPictureErrorY > 1100.0) || (PrintPara.dPictureErrorY < 900.0))
		return 0;
	
	if (PrintPara.sSendPaperLength > 47365 || PrintPara.sSendPaperLength < 6766) // 20110221 X脉冲长度小于100mm,或大于700mm
		return 0;
	
	gl_cSprinklerCount = PrintPara.cSprinklerCount;
	gl_cSprinklerNo = PrintPara.cSprinklerNo;
	gl_cBidirectional = PrintPara.cBidirectional;				//双向打印
	gl_cLineWidth = PrintPara.cLineWidth;
	gl_cMultiDotLine12 = PrintPara.cMultiDotLine12;
	gl_cMultiDotLine23 = PrintPara.cMultiDotLine23;
	gl_cMultiDotLine34 = PrintPara.cMultiDotLine34;
	MyIP[0] = PrintPara.MyIP[0];
	MyIP[1] = PrintPara.MyIP[1];
	SubnetMask[0] = PrintPara.SubnetMask[0];
	SubnetMask[1] = PrintPara.SubnetMask[1];
	GatewayIP[0] = PrintPara.GatewayIP[0];
	GatewayIP[1] = PrintPara.GatewayIP[1];
	gl_sPlotType = PrintPara.sPlotType;
	gl_iOriginalPos = PrintPara.iOriginalPos;
	gl_iYDotSpace12 = (unsigned int)PrintPara.iYDotSpace12;
	gl_iYDotSpace23 = (unsigned int)PrintPara.iYDotSpace23;
	gl_iYDotSpace34 = (unsigned int)PrintPara.iYDotSpace34;
	gl_dErrorX = PrintPara.dErrorX;
	gl_dErrorY = PrintPara.dErrorY;
	gl_dPictureErrorX = PrintPara.dPictureErrorX;
	gl_dPictureErrorY = PrintPara.dPictureErrorY;
	gl_sSendPaperLength = PrintPara.sSendPaperLength; // 20110221 Read 
	gl_sSendPaperDelay = PrintPara.sSendPaperDelay;		// 20110221 Read
	gl_sPrintHeadCleanCount = PrintPara.sPrintHeadCleanCount;// 20110221 设置打印头清洗次数
	gl_DeviceType = PrintPara.cDeviceType; // 20111101 电机设置 1:无刷
	gl_FlashChipSelect = PrintPara.cFlashChipSelect; // 20111101 Flash芯片选择 0=3201B
	//Printf("Check 1: Read gl_sSendPaperLength = %d, gl_sSendPaperDelay = %d, ch= %d\n", gl_sSendPaperLength, gl_sSendPaperDelay, gl_sPrintHeadCleanCount);
	return 1;
}

int WriteToFlash(unsigned int address)
{
	unsigned int iCodeAdd1,iCodeAdd2;
	unsigned int bakaddr;
	unsigned int i,j;
	unsigned short *sTemp;
	PrintSetPara PrintPara;
	
	if (SST39VF320_SectorErase(address) == 0)
		return 0;
	
	PrintPara.cChinaEn = 'C';
	PrintPara.cSprinklerCount = gl_cSprinklerCount;
	PrintPara.cSprinklerNo = gl_cSprinklerNo;
	PrintPara.cBidirectional = gl_cBidirectional;
	PrintPara.cLineWidth = gl_cLineWidth;
	PrintPara.cMultiDotLine12 = gl_cMultiDotLine12;
	PrintPara.cMultiDotLine23 = gl_cMultiDotLine23;
	PrintPara.cMultiDotLine34 = gl_cMultiDotLine34;
	PrintPara.MyIP[0] = MyIP[0];
	PrintPara.MyIP[1] = MyIP[1];
	PrintPara.SubnetMask[0] = SubnetMask[0];
	PrintPara.SubnetMask[1] = SubnetMask[1];
	PrintPara.GatewayIP[0] = GatewayIP[0];
	PrintPara.GatewayIP[1] = GatewayIP[1];
	PrintPara.sPlotType = gl_sPlotType;
	PrintPara.sNull1 = 0;
	PrintPara.iOriginalPos = gl_iOriginalPos;
	PrintPara.iYDotSpace12 = (int)gl_iYDotSpace12;
	PrintPara.iYDotSpace23 = (int)gl_iYDotSpace23;
	PrintPara.iYDotSpace34 = (int)gl_iYDotSpace34;
	PrintPara.dErrorX = gl_dErrorX;
	PrintPara.dErrorY = gl_dErrorY;
	PrintPara.dPictureErrorX = gl_dPictureErrorX;
	PrintPara.dPictureErrorY = gl_dPictureErrorY;
	PrintPara.cSprnklerNum  = SprinklerNum;   // 20101214   喷头数可选主板设置的喷头数	
	PrintPara.sSendPaperLength = gl_sSendPaperLength; // 20110221 Write
	PrintPara.sSendPaperDelay = gl_sSendPaperDelay;	    // 20110221 Write	
	 PrintPara.sPrintHeadCleanCount = gl_sPrintHeadCleanCount;// 20110221 设置打印头清洗次数
	 PrintPara.cDeviceType = gl_DeviceType; // 20111101 电机设置  1无刷
	 PrintPara.cFlashChipSelect = gl_FlashChipSelect; // 20111101 Flash芯片选择 0 = 3201B
	//Printf("Check 2 Write gl_sSendPaperLength = %d, gl_sSendPaperDelay = %d, ch=%d\n", gl_sSendPaperLength, gl_sSendPaperDelay, gl_sPrintHeadCleanCount );
	sTemp = (unsigned short *)&PrintPara;
	bakaddr = address + SectorSize - 2;
	j = (sizeof(PrintPara) + 1) / 2;
	iCodeAdd1 = 0;
	for (i=0;i<j;i++)
	{
		iCodeAdd1 += *sTemp;
		SST39VF320_WordProg(address++,*sTemp);
		sTemp++;
	}
	for (;address < bakaddr;address++)
		iCodeAdd1 += 0xffff;
	SST39VF320_WordProg(address++,(unsigned short)(iCodeAdd1 >> 16));
	SST39VF320_WordProg(address,(unsigned short)iCodeAdd1);
	iCodeAdd2 = 0;
	address = bakaddr - SectorSize + 2;
	for(i=0;i<SectorSize;i++)
		iCodeAdd2 += Readflash(address++);
	i = iCodeAdd1 + ((iCodeAdd1 >> 16) & 0xffff) + (iCodeAdd1 & 0xffff);
	if (i == iCodeAdd2)
		return 1;
	else
		return 0;
}

//写设置参数
//0失败,1成功
int WriteParameter(void)
{
	int iBakrINTMSK;
	int iTemp;
	
	iBakrINTMSK = rINTMSK;
	rINTMSK = BIT_GLOBAL;		//屏蔽所有中断
	rTCON = 0x0;			  //all_stop
	rSYSCFG = SYSCFG_0KB;
	if (VerifyCodeAdd(SetParameterAddr) == 0)
	{
		if (WriteToFlash(SetParameterAddr) == 0)
			iTemp = 0;
		else if (SST39VF320_SectorErase(SetParameterAddr+SectorSize) == 0)
			iTemp = 0;
		else
			iTemp = 1;
	}
	else
	{
		if (WriteToFlash(SetParameterAddr+SectorSize) == 0)
			iTemp = 0;
		else if (SST39VF320_SectorErase(SetParameterAddr) == 0)
			iTemp = 0;
		else
			iTemp = 1;
	}
	rSYSCFG = CACHECFG;
	rTCON = 0x0a;				//update T0
	rTCON = 0x09;				//T0=one shot,all_start 
	rINTMSK = iBakrINTMSK;	//恢复中断
	
	return iTemp;
}

