#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "..\src\defconst.h"
#include "..\Target\44blib.h"
#include "..\src\extern.h"

#define USING3201B 
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
#ifdef USING3201B
		Writeflash(0x555,0xAA);
		Writeflash(0x2AA,0x55);
		Writeflash(0x555,0xA0);
		Writeflash(iAddress,sData);
#else
		Writeflash(0x5555,0xAA);
		Writeflash(0x2AAA,0x55);
		Writeflash(0x5555,0xA0);
		Writeflash(iAddress,sData);

#endif
	Waitfor_endofprg();
}
//扇区擦除
int  SST39VF320_SectorErase(unsigned int iSAaddr)
{
#ifdef USING3201B
		Writeflash(0x555,0xAA);
		Writeflash(0x2AA,0x55);
		Writeflash(0x555,0x80);
		Writeflash(0x555,0xAA);
		Writeflash(0x2AA,0x55);
		Writeflash(iSAaddr,0x50);
#else
		Writeflash(0x5555,0xAA);
		Writeflash(0x2AAA,0x55);
		Writeflash(0x5555,0x80);
		Writeflash(0x5555,0xAA);
		Writeflash(0x2AAA,0x55);
		Writeflash(iSAaddr,0x30);
#endif
	Waitfor_endofprg();
	return(SST39VF320_CheckBlank(iSAaddr,SectorSize));
}

//设置机型长宽，单位mm
void SetMacLengthWith(int MacType)
{
	switch(MacType)
	{
		case 906:
			gl_sMachineLenght = 1225;//923
			gl_sMachineWidth = 820;//618
			break;
		case 1209:
			gl_sMachineLenght = 1225;
			gl_sMachineWidth = 920;
			break;
		case 1509:
			gl_sMachineLenght = 1500;
			gl_sMachineWidth = 920;
			break;
		case 1412:
			gl_sMachineLenght = 1450;
			gl_sMachineWidth = 1200;	//1380
			break;
		case 1812:
			gl_sMachineLenght = 1800;
			gl_sMachineWidth = 1200;
			break;
	}
	//gl_iMaxPictureWidth = gl_sMachineWidth;
	//Printf("gl_iMaxPictureWidth:%d\n", gl_iMaxPictureWidth);
}


//从Flash中读取设置参数,如果效验和不正确则将所有参数付初值
void ReadParameter(void)
{
	int iBakrINTMSK;
	int iFalg;
	
	iBakrINTMSK = rINTMSK;
	rINTMSK = BIT_GLOBAL;		//屏蔽所有中断
	rTCON = 0x0;			  //all_stop
	rSYSCFG = SYSCFG_0KB;
	
	if (VerifyCodeAdd(SetParameterAddr) == 1)
		iFalg = ReadFromFlash(SetParameterAddr);
	else if (VerifyCodeAdd(SetParameterAddr+SectorSize) == 1)
		iFalg = ReadFromFlash(SetParameterAddr+SectorSize);
	else
		iFalg = 0;
	if (iFalg == 0)
	{
		//喷墨全局变量初始化
		gl_cSprinklerCount = 2;				//喷头数
		gl_cSprinklerNo = 0;
		gl_cBidirectional = 1;				//双向打印
		gl_cLineWidth = 2;
		gl_cPenNo = 1; // 20101206 笔号刀号
		gl_cCutNo = 4;
		gl_cPrintCutSelect = 0x03; // 20101206 打印切割选择
		gl_cMultiDotLine12 = 0;
		gl_cMultiDotLine23 = 0;
		gl_cMultiDotLine34 = 0;
		//割完返回右下角  1:ON 返回 / 0:OFF 不返回
		gl_cOverReturnState = 1;

		// 1: 读到PU0,0时允许移动到左下角； 0:不允许
		gl_cGoOOPointState = 0;
		MyIP[0] = 0xC0A8;
		MyIP[1] = 0x0174;
		SubnetMask[0] = 0xffff;
		SubnetMask[1] = 0xff00;
		GatewayIP[0] = 0xC0A8;
		GatewayIP[1] = 0x0101;
		gl_iOriginalPos = 0;//(int)(200 * Y_PPMM);
		gl_iYDotSpace12 = SprinklerSpace;
		gl_iYDotSpace23 = SprinklerSpace;
		gl_iYDotSpace34 = SprinklerSpace;
		gl_dErrorX = 0;
		gl_dErrorY = 0;
		gl_dPictureErrorX = 1000.0;
		gl_dPictureErrorY = 1000.0;
		gl_sPlotType = 1620;
		//SetMacLengthWith(gl_sPlotType);//设置机器长宽比20100623
		//Printf("MacLen[%d] MacWith[%d]\n", gl_sMachineLenght, gl_sMachineWidth);
		//切割全局变量初始化20100622
		//gl_cCutPenNum = 4;		//笔刀属性
		gl_cMovPaper = 0x0;			//自动送纸
		gl_cPumpSwitch = 0x0;		//气泵吸附设置
		gl_cEndReturnRight = 1;	//结束返回右下角
		gl_cOverCutOffPaper = 0; //打印&结束切断纸张 20120906
		gl_sOverCutOffset = 10;	 // 偏移10毫米切断纸张 20120906
		gl_cAutoSendPaperSet = 1; //自动送纸开启
		//gl_sMachineWidth = 1225;		//机型宽度
		//gl_sMachineLenght = 920;		//机型长度
		//gl_iOriginalPos_X = 0;			//原点坐标X
		//gl_iOriginalPos_Y = 0;			//原点坐标Y
		//Printf("Read def O Pos X: %d, Pox Y: %d\n", gl_iOriginalPos_X, gl_iOriginalPos_Y);
		gl_iCutDown[0] = 0;					//刀压#0 无用 
		gl_iCutDown[1] = 600;					//刀压#1   20120822
		gl_iCutDown[2] = 600;					//刀压#2
		gl_iCutDown[3] = 600;					//刀压#3
		gl_iCutDown[4] = 600;					//刀压#4
		gl_iCutDown[5] = 600;					//刀压#5
		gl_iCutDown[6] = 600;					//刀压#6
		gl_iCutDown[7] = 600;					//刀压#7
		gl_iCutDown[8] = 600;					//刀压#8
		gl_iCutDown[9] = 600;					//刀压#9

		//笔属性
		gl_cSetPenAttr[0] = 0xff;	//笔属性  1刀 2转角提刀 0笔 20131208
		gl_cSetPenAttr[1] = 0;		
		gl_cSetPenAttr[2] = 1;	
		gl_cSetPenAttr[3] = 1;	
		gl_cSetPenAttr[4] = 1;	
		gl_cSetPenAttr[5] = 1;	
		gl_cSetPenAttr[6] = 1;	
		gl_cSetPenAttr[7] = 1;	
		gl_cSetPenAttr[8] = 1;	
		gl_cSetPenAttr[9] = 1;	
		gl_iZInitSpeed = 8000;
		gl_lZDriveSpeed = 50000;					// 20120808
		gl_lZAddSpeed  = 50000*2;
		
		Printf("Set para: %d, %d, %d, %d \n", gl_iCutDown[0],gl_iCutDown[1],gl_iCutDown[2],gl_iCutDown[3]);
		gl_iCutSpeed = 2;					// 切割速度 0 低速 1中低 2中 3中高 4高速
		//gl_iPrintSpeed = 1;					// 打印速度 0 低速 1中速 2高速
		gl_iMoveFramSpeed = 2;				// 移框速度 0 低速 1中低 2中 3中高 4高速
		gl_iCutOffset = 40;					//偏距
		gl_iCutDistanceY = 140;				//刀距 单位 像素点 640*37.8/300 = 80mm
		gl_iCutDistanceX = 537;				//喷刀X偏移 20120711 264*37.8/300 = 33mm
		gl_dCutExtensValue = 40;			//提刀延伸值
		gl_dCutRotaExtenValue = 8;		//转角提刀延伸值
		gl_dXLengthCalibration = 50.0;		//X长度校正
		gl_dYLengthCalibration = 50.0;  	//Y长度校正
		gl_dCutPress = 0.01;				//刀压补偿

		gl_dXProp = 1.0;
		gl_dYProp = 1.0;

		gl_dPosUnitX = HPGL_MM * X_PPMM * gl_dXProp;
		gl_dPosUnitY = HPGL_MM * Y_PPMM * gl_dYProp;

		gl_iCutUpDelay = 1;                //抬笔延迟,单位:us
		gl_iCutDownDelay = 500;              //落笔延迟,单位:us

		gl_iCutSectionLen = 30;				// 50mm一段
		gl_iCutSectionGap = 10;				// 0.25mm 间隙
		
		RPGL_iAutoCutOff  = gl_sPlotType-100;
		RPGL_iPaperSpeedValue = 2;
		RPGL_iPauseTimeValue = 0;
		RPGL_lPaperOffsetValue = 10;

		gl_iUInitSpeed = 6000;
		gl_lUDriveSpeed = 30000;
		gl_lUAddSpeed =  60000;

		gl_cLanguageSelect = 0;		// 20131218

		gl_dMovePaperError_X = 0.0;
		gl_dMovePaperError_Y = 0.0;

		gl_dPaperAdjust = 0.5;

		gl_cPHCDTime = 1;
		//gl_lWorkOrig_YYPluse = PenCarSideInterval * DotPulseNum;	//20130831
		//gl_lWorkOrig_XXPluse = 0;
		//写FLASH
		WriteToFlash(SetParameterAddr);
	}
	rSYSCFG = CACHECFG;
	rTCON = 0x0a;				//update T0
	rTCON = 0x09;				//T0=one shot,all_start 
	rINTMSK = iBakrINTMSK;	//恢复中断
	gl_iMaxYMM = (gl_sPlotType + 3) * Y_PPMM;
	gl_iGraphWidth = (int)((double)(gl_iMaxYMM - gl_iOriginalPos) / (double)(8 * DotPulseNum));	
	//gl_iMaxYMM += (3 * (MaxMalposedDot + SprinklerSpace) + 2 * AdditionDot + OriginalSideDot + MotorSideDot + 320) * DotPulseNum;
	gl_iMaxYMM += ((MaxMalposedDot + SprinklerSpace) + 2 * AdditionDot + OriginalSideDot + MotorSideDot ) * DotPulseNum;
	gl_iStaggerDot = (int)(gl_dErrorY * Y_PPMM / DotPulseNum);
	//Printf("Read Parameter OK!\n");
	CalVariableValue();
}

//计算其它全局变量的值
void CalVariableValue(void)
{
	gl_iMaxPictureWidth = (gl_sPlotType + 3) * Y_PPMM - gl_iOriginalPos + 3 * (MaxMalposedDot + SprinklerSpace) * DotPulseNum;;
	gl_iYMaxEffectivePMM = gl_iMaxPictureWidth; 
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
	// 20120207 gl_iGraphWidth
	gl_iPulseNumY = (gl_iPulseNumY + gl_iGraphWidth * 8 + AdditionDot * 2 + OriginalSideDot + MotorSideDot) * DotPulseNum;
	Printf("gl_iGraphWidth: %d\n", gl_iGraphWidth);
	Printf("gl_iPulseNumY:%d\n", gl_iPulseNumY);
	
	gl_iPulseNumX = gl_iTHPulseNumX + (int)(gl_dErrorX * X_PPMM);
	Printf("--> gl_iPulseNumX :%d\n", gl_iPulseNumX);
	Printf("--> gl_dErrorX    :%d\n",    gl_dErrorX);	
}

/*
//计算其它全局变量的值
void CalVariableValue(void)
{
	
	//计算可接收位图 XY最大脉冲宽度
	gl_iMaxPictureWidth = gl_sMachineWidth* Y_PPMM;
	gl_iMaxPictureLength = gl_sMachineLenght* X_PPMM;
	
	//计算写FIFO地址
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
	
	//Printf("gl_iLineDataNum[%d] gl_iTHPulseNumX[%d] gl_iPulseNumY[%d]\n", gl_iLineDataNum, gl_iTHPulseNumX, gl_iPulseNumY);
	// 写喷头数
	WriteFIFOFull(gl_cSprinklerNo+gl_cSprinklerCount-1);

	// 一行最大接收字节数
	gl_iMaxPictureWidth /= (DotPulseNum * 8);
	//Printf("**BYTE*** gl_iMaxPictureWidth[%d]\n", gl_iMaxPictureWidth);

	// 计算当前文件一行脉冲长度
	//Printf("gl_iPulseNumY[%d] gl_iGraphWidth[%d]\n",gl_iPulseNumY ,gl_iGraphWidth);
	gl_iPulseNumY = (gl_iPulseNumY + gl_iGraphWidth * 8 + AdditionDot * 2 + OriginalSideDot + MotorSideDot) * DotPulseNum;//320
	//Printf("gl_iPulseNumY[%d]\n", gl_iPulseNumY);
	
	// 计算当前文件行距
	//Printf("gl_iTHPulseNumX[%d] gl_dErrorX[%d]\n",gl_iTHPulseNumX ,gl_dErrorX);
	gl_iPulseNumX = gl_iTHPulseNumX + (int)(gl_dErrorX * X_PPMM);
	//Printf("gl_iPulseNumX[%d]\n", gl_iPulseNumX);
	

	// XY实际最大脉冲长度
	gl_lMaxX = IntRound((double)gl_sMachineLenght  * X_PPMM);  //绘图的XY最大值
	gl_lMaxY = IntRound((double)(gl_sMachineWidth+180) * Y_PPMM);  //两侧间距180mm
	gl_lMachineLenghtPluse = gl_lMaxX;
	gl_lMachineWidthPluse = gl_lMaxY;
	//Printf("gl_lMaxX[%d] gl_lMaxY[%d]\n", gl_lMaxX, gl_lMaxY);

	// 打印有效方位脉冲值
	gl_iYMaxEffectivePMM = gl_lMaxY - (175 * Y_PPMM);
	gl_iXMaxEffectivePMM = gl_lMaxX;
	//Printf("gl_iYMaxEffectivePMM[%d], gl_iXMaxEffectivePMM[%d]\n", gl_iYMaxEffectivePMM, gl_iXMaxEffectivePMM);

	//相邻两次扫描之间错开的点数 
	gl_iStaggerDot = (int)(gl_dErrorY * Y_PPMM / DotPulseNum);
	//Printf("=====END CalVariableValue======\n");
}
*/

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

	//Printf("iCodeAdd1:%x\n", iCodeAdd1);
	iCodeAdd2 = (unsigned int)Readflash(address++) << 16;
	//Printf("iCodeAdd2:%x\n", iCodeAdd2);
	iCodeAdd2 += Readflash(address);
	//Printf("iCodeAdd3:%x\n", iCodeAdd2);
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
	CutSetPara   CutPara;
	//读喷墨参数
	sTemp = (unsigned short *)&PrintPara;
	j = (sizeof(PrintPara) + 1) / 2;
	for (i=0;i<j;i++)
	{
		*sTemp = Readflash(address++);
		sTemp++;
	}
	//读切割参数
	sTemp = (unsigned short *)&CutPara;
	j = (sizeof(CutPara) + 1) / 2;
	for (i=0;i<j;i++)
	{
		*sTemp = Readflash(address++);
		sTemp++;
	}
	//验证喷墨参数
	if ((PrintPara.cSprinklerCount > SprinklerNum) || (PrintPara.cSprinklerCount == 0)
		|| (PrintPara.cSprinklerNo > (SprinklerNum - PrintPara.cSprinklerCount)))
		return 0;
	if ((PrintPara.cBidirectional != 0) && (PrintPara.cBidirectional != 1))
		return 0;
	if ((PrintPara.cLineWidth < 1) || (PrintPara.cLineWidth > 9))
		return 0;
	if (PrintPara.cMultiDotLine12 >= SprinklerWidth)
		return 0;
//	if (PrintPara.cMultiDotLine23 >= SprinklerWidth)
//		return 0;
//	if (PrintPara.cMultiDotLine34 >= SprinklerWidth)
//		return 0;
	//if ((PrintPara.sPlotType != 1350) && (PrintPara.sPlotType != 1630) && (PrintPara.sPlotType != 1830) && (PrintPara.sPlotType != 2030) && (PrintPara.sPlotType != 2230) && (PrintPara.sPlotType != 2430))
	//	return 0;
	//if ((PrintPara.iOriginalPos < 0) || (PrintPara.iOriginalPos > PrintPara.sPlotType * Y_PPMM))
		//return 0;
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
	//20120809 只要检测到刀号超出范围就恢复默认值
	if ((PrintPara.cPenNo > 9)||(PrintPara.cPenNo < 1))
		PrintPara.cPenNo = 1;
	if ((PrintPara.cCutNo > 9)||(PrintPara.cCutNo < 1))
		PrintPara.cCutNo = 4;	
	
	
	gl_cSprinklerCount = PrintPara.cSprinklerCount;
	gl_cSprinklerNo = PrintPara.cSprinklerNo;
	gl_cBidirectional = PrintPara.cBidirectional;				//双向打印
	gl_cLineWidth = PrintPara.cLineWidth;
	gl_cPenNo = PrintPara.cPenNo; // 20101206 笔号刀号
	gl_cCutNo = PrintPara.cCutNo;
	gl_cPrintCutSelect = PrintPara.cPrintCutSelect; // 20101206 打印切割选择
	
	gl_cMultiDotLine12 = PrintPara.cMultiDotLine12;
	gl_cMultiDotLine23 = 0;//PrintPara.cMultiDotLine23;
	gl_cMultiDotLine34 = 0;//PrintPara.cMultiDotLine34;

	MyIP[0] = PrintPara.MyIP[0];
	MyIP[1] = PrintPara.MyIP[1];
	SubnetMask[0] = PrintPara.SubnetMask[0];
	SubnetMask[1] = PrintPara.SubnetMask[1];
	GatewayIP[0] = PrintPara.GatewayIP[0];
	GatewayIP[1] = PrintPara.GatewayIP[1];
	gl_sPlotType = PrintPara.sPlotType;
	//SetMacLengthWith(gl_sPlotType);//设置机器长宽比20100623
	
	
	gl_iYDotSpace12 = (unsigned int)PrintPara.iYDotSpace12;
	gl_iYDotSpace23 = (unsigned int)PrintPara.iYDotSpace23;
	gl_iYDotSpace34 = (unsigned int)PrintPara.iYDotSpace34;

	//*************20130807
	RPGL_iAutoCutOff  = CutPara.iAutoCutOff;
	RPGL_iPaperSpeedValue  = CutPara.iUAxisSpeedInit;
	RPGL_iPauseTimeValue = CutPara.iPauseTimeValue;
	RPGL_lPaperOffsetValue = CutPara.lPaperOffsetValue;
	gl_cGoOOPointState = CutPara.cGoOOPointState;
	gl_cOverReturnState = CutPara.cOverReturnState;
	gl_cAutoSendPaperSet = CutPara.cAutoSendPaperSet;
	gl_time = CutPara.cTime;
	Printf("gl_cAutoSendPaperSet S:%d\n", gl_cAutoSendPaperSet);
	
	//*****************

	gl_cLanguageSelect = CutPara.cLanguageSelect;		// 20131218
	//Printf("Language R %d\n", gl_cLanguageSelect);
	gl_dErrorX = PrintPara.dErrorX;
	gl_dErrorY = PrintPara.dErrorY;
	gl_dPictureErrorX = PrintPara.dPictureErrorX;
	gl_dPictureErrorY = PrintPara.dPictureErrorY;

	//验证切割参数
	//Printf("Verify Cut para!\n");

	//gl_cCutPenNum = CutPara.cCutPenNum;
	 //Printf("cCutPenNum:%d\n",CutPara.cCutPenNum);
	gl_iOriginalPos = CutPara.iOriginalPos;
	Printf("Read OrigPos:%d\n", gl_iOriginalPos);
	gl_cEndReturnRight = CutPara.cEndReturnRight;
	//Printf("cEndReturnRight:%d\n",CutPara.cEndReturnRight);	

	gl_cMovPaper = CutPara.cMovPaper;
	//Printf("cMovPaper:%d\n",CutPara.cMovPaper);		

	gl_cPumpSwitch = CutPara.cPumpSwitch;
	//Printf("cPumpSwitch:%d\n",CutPara.cPumpSwitch);	

	gl_cOverCutOffPaper =  CutPara.cOverCutOffPaper;	//打印&结束切断纸张 20120906

	gl_cPHCDTime = CutPara.cPHCDTime;				//  20141021
	
	gl_sOverCutOffset = CutPara.sOverCutOffset;			// 偏移毫米切断纸张 20120906

	gl_sMachineLenght = CutPara.sMachineLenght;
	//Printf("sMachineLenght:%d\n",CutPara.sMachineLenght);			

	gl_sMachineWidth = CutPara.sMachineWidth;
	Printf("sMachineWidth:%d\n",CutPara.sMachineWidth);


	//gl_iOriginalPos_X = CutPara.iOriginalPos_X;//原点坐标X 
	//gl_iOriginalPos_Y= CutPara.iOriginalPos_Y;//原点坐标Y
	//Printf("Read O Pos X: %d, Pox Y: %d\n", gl_iOriginalPos_X, gl_iOriginalPos_Y);
/*	
	gl_iCutDown[0]=CutPara.iCutDown[0];
	gl_iCutDown[1]=CutPara.iCutDown[1];
	gl_iCutDown[2]=CutPara.iCutDown[2];
	gl_iCutDown[3]=CutPara.iCutDown[3];
	gl_iCutDown[4]=CutPara.iCutDown[4];
	gl_iCutDown[5]=CutPara.iCutDown[5];
	gl_iCutDown[6]=CutPara.iCutDown[6];
	gl_iCutDown[7]=CutPara.iCutDown[7];
	gl_iCutDown[8]=CutPara.iCutDown[8];
	gl_iCutDown[9]=CutPara.iCutDown[9];
*/
	ArrCpy(gl_iCutDown, CutPara.iCutDown, 10);
	
/*	
	gl_cSetPenAttr[0]=CutPara.cSetPenAttr[0];
	gl_cSetPenAttr[1]=CutPara.cSetPenAttr[1];
	gl_cSetPenAttr[2]=CutPara.cSetPenAttr[2];
	gl_cSetPenAttr[3]=CutPara.cSetPenAttr[3];
	gl_cSetPenAttr[4]=CutPara.cSetPenAttr[4];
	gl_cSetPenAttr[5]=CutPara.cSetPenAttr[5];
	gl_cSetPenAttr[6]=CutPara.cSetPenAttr[6];
	gl_cSetPenAttr[7]=CutPara.cSetPenAttr[7];
	gl_cSetPenAttr[8]=CutPara.cSetPenAttr[8];
	gl_cSetPenAttr[9]=CutPara.cSetPenAttr[9];
*/	
	ArrCpy(gl_cSetPenAttr, CutPara.cSetPenAttr, 10);
	/*
	Printf("gl_iCutDown:");
	for(i=0; i<10; i++)
		Printf("[%d],", gl_iCutDown[i]);
	Printf("\n");
	Printf("gl_cSetPenAttr:");
	for(i=0; i<10; i++)
		Printf("[%d],", gl_cSetPenAttr[i]);
	Printf("\n");
	*/
	gl_iCutOffset = CutPara.iCutOffset;
	//Printf("iCutOffset:%d\n",CutPara.iCutOffset);				

	gl_iCutSpeed = CutPara.iCutSpeed;
	//Printf("iCutSpeed:%d\n",CutPara.iCutSpeed);	
	//gl_iPrintSpeed = CutPara.iPrintSpeed;
	gl_iMoveFramSpeed = CutPara.iMoveFramSpeed;

	gl_iZInitSpeed = CutPara.iZInitSpeed;
	gl_lZDriveSpeed = CutPara.lZDriveSpeed; //20120808
	gl_lZAddSpeed = CutPara.lZAddSpeed;
	
	//gl_lWorkOrig_YYPluse = CutPara.lWorkOrig_YYPluse;	//20130831
	//gl_lWorkOrig_XXPluse = CutPara.lWorkOrig_XXPluse;
	//Printf("RFLASH: WX[%d] WY[%d]\n", gl_lWorkOrig_XXPluse, gl_lWorkOrig_YYPluse);
	gl_iUInitSpeed = CutPara.iUInitSpeed;
	gl_lUDriveSpeed = CutPara.lUDriveSpeed;
	gl_lUAddSpeed = CutPara.lUAddSpeed;
	
	gl_iCutDownDelay = CutPara.iCutDownDelay;
	gl_iCutUpDelay = CutPara.iCutUpDelay;

	gl_iCutSectionLen = CutPara.iCutSectionLen;
	gl_iCutSectionGap = CutPara.iCutSectionGap;
		
	gl_iCutDistanceY = CutPara.iCutDistanceY;
	//Printf("dCutDistance:%f\n",CutPara.dCutDistance);				
	gl_iCutDistanceX = CutPara.iCutDistanceX;	//20120711 
	
	gl_dCutExtensValue = CutPara.dCutExtensValue;
	//Printf("dCutExtensValue:%f\n",CutPara.dCutExtensValue);				
	
	gl_dCutPress = CutPara.dCutPress;
	//Printf("dCutPress:%f\n",CutPara.dCutPress);				

	gl_dCutRotaExtenValue = CutPara.dCutRotaExtenValue;
	//Printf("dCutRotaExtenValue:%f\n",CutPara.dCutRotaExtenValue);				

	gl_dXLengthCalibration = CutPara.dXLengthCalibration;
	//Printf("dXLengthCalibration:%f\n",CutPara.dXLengthCalibration);				

	gl_dYLengthCalibration = CutPara.dYLengthCalibration;
	//Printf("dYLengthCalibration:%f\n",CutPara.dYLengthCalibration);	

	gl_dMovePaperError_X = CutPara.dMovePaperError_X;
	gl_dMovePaperError_Y = CutPara.dMovePaperError_Y;
	
	gl_dXProp = CutPara.dXProp;
	gl_dYProp = CutPara.dYProp;
	
	gl_dPosUnitX = HPGL_MM * X_PPMM * gl_dXProp;
	gl_dPosUnitY = HPGL_MM * Y_PPMM * gl_dYProp;
	Printf("RFLASH:gl_dPosUnitX:%f, gl_dPosUnitY:%f\n", gl_dPosUnitX, gl_dPosUnitY);

	gl_dPaperAdjust = CutPara.dPaperAdjust;
	Printf("dPaperAdjust:%f\n", gl_dPaperAdjust);
	return 1;
}

//20100622 WriteToFlash(unsigned int address)写数据到FLASH
int WriteToFlash(unsigned int address)
{
	unsigned int iCodeAdd1,iCodeAdd2;
	unsigned int bakaddr;
	unsigned int i,j;
	unsigned short *sTemp;
	PrintSetPara PrintPara;
	CutSetPara   CutPara;
	if (SST39VF320_SectorErase(address) == 0)
		return 0;
	//喷墨参数设置
	//PrintPara.cChinaEn = 'C';
	PrintPara.cSprinklerCount = gl_cSprinklerCount;
	PrintPara.cSprinklerNo = gl_cSprinklerNo;
	PrintPara.cBidirectional = gl_cBidirectional;
	PrintPara.cLineWidth = gl_cLineWidth;
	PrintPara.cMultiDotLine12 = gl_cMultiDotLine12;
	PrintPara.cPrintCutSelect = gl_cPrintCutSelect; // 20101206 打印切割选择
	//PrintPara.cMultiDotLine23 = gl_cMultiDotLine23;
	//PrintPara.cMultiDotLine34 = gl_cMultiDotLine34;
	//PrintPara.cNull1 = 0;
	PrintPara.cPenNo = gl_cPenNo;// 20101206 笔号刀号
	PrintPara.cCutNo = gl_cCutNo;
	PrintPara.MyIP[0] = MyIP[0];
	PrintPara.MyIP[1] = MyIP[1];
	PrintPara.SubnetMask[0] = SubnetMask[0];
	PrintPara.SubnetMask[1] = SubnetMask[1];
	PrintPara.GatewayIP[0] = GatewayIP[0];
	PrintPara.GatewayIP[1] = GatewayIP[1];
	PrintPara.sPlotType = gl_sPlotType;
	//PrintPara.sNull1= 0;
	//PrintPara.sNull1[0]=0;PrintPara.sNull1[1]=0;PrintPara.sNull1[2]=0;
	
	PrintPara.iYDotSpace12 = (int)gl_iYDotSpace12;
	PrintPara.iYDotSpace23 = (int)gl_iYDotSpace23;
	PrintPara.iYDotSpace34 = (int)gl_iYDotSpace34;
	
	
	PrintPara.dErrorX = gl_dErrorX;
	PrintPara.dErrorY = gl_dErrorY;
	PrintPara.dPictureErrorX = gl_dPictureErrorX;
	PrintPara.dPictureErrorY = gl_dPictureErrorY;
	//切割参数设置
	//CutPara.cCutPenNum = gl_cCutPenNum;
	CutPara.cLanguageSelect = gl_cLanguageSelect;		// 20131218
	//Printf("Language W %d\n", gl_cLanguageSelect);
	CutPara.cEndReturnRight = gl_cEndReturnRight;
	CutPara.cMovPaper = gl_cMovPaper;
	CutPara.cPumpSwitch = gl_cPumpSwitch;
	CutPara.cOverCutOffPaper = gl_cOverCutOffPaper; 	//打印&结束切断纸张 20120906
	CutPara.cAutoSendPaperSet = gl_cAutoSendPaperSet;	//自动送纸设置
	CutPara.cPHCDTime = gl_cPHCDTime;				//  20141021
	CutPara.sOverCutOffset = gl_sOverCutOffset;			// 偏移毫米切断纸张 20120906
	
	//************20130807
	CutPara.iOriginalPos = gl_iOriginalPos;
	Printf("Write OrigPos:%d\n", gl_iOriginalPos);
	CutPara.iPauseTimeValue = RPGL_iPauseTimeValue;
	CutPara.iAutoCutOff = RPGL_iAutoCutOff;
	CutPara.iUAxisSpeedInit = RPGL_iPaperSpeedValue;
	CutPara.lPaperOffsetValue = RPGL_lPaperOffsetValue;
	CutPara.cGoOOPointState = gl_cGoOOPointState;
	CutPara.cOverReturnState = gl_cOverReturnState;
	//*******************	
	CutPara.sMachineWidth = gl_sMachineWidth;	//对应原点感应器的位置
	
	Printf("gl_sMachineWidth:%d\n",gl_sMachineWidth);
	Printf("CutPara.sMachineWidth:%d\n",CutPara.sMachineWidth);
	CutPara.sMachineLenght = gl_sMachineLenght;

	//CutPara.iOriginalPos_X = gl_iOriginalPos_X;		//原点x
	//CutPara.iOriginalPos_Y = gl_iOriginalPos_Y;		//原点y
	//Printf("Write O Pos X: %d, Pox Y: %d\n", gl_iOriginalPos_X, gl_iOriginalPos_Y);
	/*
	CutPara.iCutDown[0]=gl_iCutDown[0];
	CutPara.iCutDown[1]=gl_iCutDown[1];
	CutPara.iCutDown[2]=gl_iCutDown[2];
	CutPara.iCutDown[3]=gl_iCutDown[3];
	CutPara.iCutDown[4]=gl_iCutDown[4];
	CutPara.iCutDown[5]=gl_iCutDown[5];
	CutPara.iCutDown[6]=gl_iCutDown[6];
	CutPara.iCutDown[7]=gl_iCutDown[7];
	CutPara.iCutDown[8]=gl_iCutDown[8];
	CutPara.iCutDown[9]=gl_iCutDown[9];
	*/

	//memcpy(CutPara.iCutDown, gl_iCutDown, sizeof(int)*10);
	//memcpy(CutPara.cSetPenAttr, gl_cSetPenAttr, sizeof(int)*10);
	ArrCpy(CutPara.iCutDown, gl_iCutDown, 10);
	ArrCpy(CutPara.cSetPenAttr, gl_cSetPenAttr, 10);
	Printf("Write para: %d, %d, %d, %d \n", gl_iCutDown[0],gl_iCutDown[1],gl_iCutDown[2],gl_iCutDown[3]);

	CutPara.cTime = gl_time ;
	
	CutPara.iCutOffset = gl_iCutOffset;
	CutPara.iCutSpeed = gl_iCutSpeed;
	//CutPara.iPrintSpeed = gl_iPrintSpeed;
	CutPara.iMoveFramSpeed = gl_iMoveFramSpeed;
	CutPara.iCutDownDelay = gl_iCutDownDelay;
	CutPara.iCutUpDelay = gl_iCutUpDelay;
	CutPara.iCutSectionLen = gl_iCutSectionLen;
	CutPara.iCutSectionGap = gl_iCutSectionGap;
	
	CutPara.iZInitSpeed = gl_iZInitSpeed;
	CutPara.lZDriveSpeed = gl_lZDriveSpeed; //20120808
	CutPara.lZAddSpeed = gl_lZAddSpeed;
	
	//CutPara.lWorkOrig_YYPluse = gl_lWorkOrig_YYPluse;	//20130831
	//CutPara.lWorkOrig_XXPluse = gl_lWorkOrig_XXPluse;
	
	CutPara.iUInitSpeed =  gl_iUInitSpeed;
	CutPara.lUDriveSpeed =  gl_lUDriveSpeed;
	CutPara.lUAddSpeed  = gl_lUAddSpeed;
	
	CutPara.iCutDistanceY = gl_iCutDistanceY;
	CutPara.iCutDistanceX = gl_iCutDistanceX;  //20120711 
	CutPara.dCutExtensValue = gl_dCutExtensValue;
	CutPara.dCutPress = gl_dCutPress;
	CutPara.dCutRotaExtenValue = gl_dCutRotaExtenValue;
	CutPara.dXLengthCalibration = gl_dXLengthCalibration;
	CutPara.dYLengthCalibration = gl_dYLengthCalibration;
	CutPara.dXProp = gl_dXProp;
	CutPara.dYProp = gl_dYProp;
	CutPara.dPosUnitX = gl_dPosUnitX;
	CutPara.dPosUnitY = gl_dPosUnitY;
	CutPara.dMovePaperError_X = gl_dMovePaperError_X;
	CutPara.dMovePaperError_Y = gl_dMovePaperError_Y;
	CutPara.dPaperAdjust = gl_dPaperAdjust;
	
	//**********
	sTemp = (unsigned short *)&PrintPara;
	bakaddr = address + SectorSize - 2;
	j = (sizeof(PrintPara) + 1) / 2;
	//Printf("Print para sizeof:%d\n", j);
	iCodeAdd1 = 0;
	for (i=0;i<j;i++)
	{
		iCodeAdd1 += *sTemp;
		SST39VF320_WordProg(address++,*sTemp);
		sTemp++;
	}
	//Printf("Print iCodeAdd1:%x\n", iCodeAdd1);
	//写切割参数
	sTemp = (unsigned short *)&CutPara;
	j = (sizeof(CutPara) + 1) / 2;
	//Printf("Cut para sizeof:%d\n", j);
	for (i=0;i<j;i++)
	{
		iCodeAdd1 += *sTemp;
		SST39VF320_WordProg(address++,*sTemp);
		sTemp++;
	}	
	//Printf("Cut iCodeAdd1:%x\n", iCodeAdd1);

	//如果写入参数长度小于一个扇区大小，剩余部分全写0xffff	
	for (;address < bakaddr;address++)
		iCodeAdd1 += 0xffff;
	//Printf("End iCodeAdd1:%x\n", iCodeAdd1);
	
	SST39VF320_WordProg(address++,(unsigned short)(iCodeAdd1 >> 16));
	SST39VF320_WordProg(address,(unsigned short)iCodeAdd1);
	iCodeAdd2 = 0;
	address = bakaddr - SectorSize + 2;
	for(i=0;i<SectorSize;i++)
		iCodeAdd2 += Readflash(address++);
	//Printf("CRC iCodeAdd2:%x\n", iCodeAdd2);
	i = iCodeAdd1 + ((iCodeAdd1 >> 16) & 0xffff) + (iCodeAdd1 & 0xffff);
	//Printf("CRC iCodeAdd1:%x\n", 1);
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
