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

//���flash�Ƿ�Ϊ�գ�1Ϊ�գ�0��Ϊ��
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

//�ȴ��������
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
//д
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
//��������
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

//���û��ͳ�����λmm
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


//��Flash�ж�ȡ���ò���,���Ч��Ͳ���ȷ�����в�������ֵ
void ReadParameter(void)
{
	int iBakrINTMSK;
	int iFalg;
	
	iBakrINTMSK = rINTMSK;
	rINTMSK = BIT_GLOBAL;		//���������ж�
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
		//��īȫ�ֱ�����ʼ��
		gl_cSprinklerCount = 2;				//��ͷ��
		gl_cSprinklerNo = 0;
		gl_cBidirectional = 1;				//˫���ӡ
		gl_cLineWidth = 2;
		gl_cPenNo = 1; // 20101206 �ʺŵ���
		gl_cCutNo = 4;
		gl_cPrintCutSelect = 0x03; // 20101206 ��ӡ�и�ѡ��
		gl_cMultiDotLine12 = 0;
		gl_cMultiDotLine23 = 0;
		gl_cMultiDotLine34 = 0;
		//���귵�����½�  1:ON ���� / 0:OFF ������
		gl_cOverReturnState = 1;

		// 1: ����PU0,0ʱ�����ƶ������½ǣ� 0:������
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
		//SetMacLengthWith(gl_sPlotType);//���û��������20100623
		//Printf("MacLen[%d] MacWith[%d]\n", gl_sMachineLenght, gl_sMachineWidth);
		//�и�ȫ�ֱ�����ʼ��20100622
		//gl_cCutPenNum = 4;		//�ʵ�����
		gl_cMovPaper = 0x0;			//�Զ���ֽ
		gl_cPumpSwitch = 0x0;		//������������
		gl_cEndReturnRight = 1;	//�����������½�
		gl_cOverCutOffPaper = 0; //��ӡ&�����ж�ֽ�� 20120906
		gl_sOverCutOffset = 10;	 // ƫ��10�����ж�ֽ�� 20120906
		gl_cAutoSendPaperSet = 1; //�Զ���ֽ����
		//gl_sMachineWidth = 1225;		//���Ϳ��
		//gl_sMachineLenght = 920;		//���ͳ���
		//gl_iOriginalPos_X = 0;			//ԭ������X
		//gl_iOriginalPos_Y = 0;			//ԭ������Y
		//Printf("Read def O Pos X: %d, Pox Y: %d\n", gl_iOriginalPos_X, gl_iOriginalPos_Y);
		gl_iCutDown[0] = 0;					//��ѹ#0 ���� 
		gl_iCutDown[1] = 600;					//��ѹ#1   20120822
		gl_iCutDown[2] = 600;					//��ѹ#2
		gl_iCutDown[3] = 600;					//��ѹ#3
		gl_iCutDown[4] = 600;					//��ѹ#4
		gl_iCutDown[5] = 600;					//��ѹ#5
		gl_iCutDown[6] = 600;					//��ѹ#6
		gl_iCutDown[7] = 600;					//��ѹ#7
		gl_iCutDown[8] = 600;					//��ѹ#8
		gl_iCutDown[9] = 600;					//��ѹ#9

		//������
		gl_cSetPenAttr[0] = 0xff;	//������  1�� 2ת���ᵶ 0�� 20131208
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
		gl_iCutSpeed = 2;					// �и��ٶ� 0 ���� 1�е� 2�� 3�и� 4����
		//gl_iPrintSpeed = 1;					// ��ӡ�ٶ� 0 ���� 1���� 2����
		gl_iMoveFramSpeed = 2;				// �ƿ��ٶ� 0 ���� 1�е� 2�� 3�и� 4����
		gl_iCutOffset = 40;					//ƫ��
		gl_iCutDistanceY = 140;				//���� ��λ ���ص� 640*37.8/300 = 80mm
		gl_iCutDistanceX = 537;				//�絶Xƫ�� 20120711 264*37.8/300 = 33mm
		gl_dCutExtensValue = 40;			//�ᵶ����ֵ
		gl_dCutRotaExtenValue = 8;		//ת���ᵶ����ֵ
		gl_dXLengthCalibration = 50.0;		//X����У��
		gl_dYLengthCalibration = 50.0;  	//Y����У��
		gl_dCutPress = 0.01;				//��ѹ����

		gl_dXProp = 1.0;
		gl_dYProp = 1.0;

		gl_dPosUnitX = HPGL_MM * X_PPMM * gl_dXProp;
		gl_dPosUnitY = HPGL_MM * Y_PPMM * gl_dYProp;

		gl_iCutUpDelay = 1;                //̧���ӳ�,��λ:us
		gl_iCutDownDelay = 500;              //����ӳ�,��λ:us

		gl_iCutSectionLen = 30;				// 50mmһ��
		gl_iCutSectionGap = 10;				// 0.25mm ��϶
		
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
		//дFLASH
		WriteToFlash(SetParameterAddr);
	}
	rSYSCFG = CACHECFG;
	rTCON = 0x0a;				//update T0
	rTCON = 0x09;				//T0=one shot,all_start 
	rINTMSK = iBakrINTMSK;	//�ָ��ж�
	gl_iMaxYMM = (gl_sPlotType + 3) * Y_PPMM;
	gl_iGraphWidth = (int)((double)(gl_iMaxYMM - gl_iOriginalPos) / (double)(8 * DotPulseNum));	
	//gl_iMaxYMM += (3 * (MaxMalposedDot + SprinklerSpace) + 2 * AdditionDot + OriginalSideDot + MotorSideDot + 320) * DotPulseNum;
	gl_iMaxYMM += ((MaxMalposedDot + SprinklerSpace) + 2 * AdditionDot + OriginalSideDot + MotorSideDot ) * DotPulseNum;
	gl_iStaggerDot = (int)(gl_dErrorY * Y_PPMM / DotPulseNum);
	//Printf("Read Parameter OK!\n");
	CalVariableValue();
}

//��������ȫ�ֱ�����ֵ
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
//��������ȫ�ֱ�����ֵ
void CalVariableValue(void)
{
	
	//����ɽ���λͼ XY���������
	gl_iMaxPictureWidth = gl_sMachineWidth* Y_PPMM;
	gl_iMaxPictureLength = gl_sMachineLenght* X_PPMM;
	
	//����дFIFO��ַ
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
	// д��ͷ��
	WriteFIFOFull(gl_cSprinklerNo+gl_cSprinklerCount-1);

	// һ���������ֽ���
	gl_iMaxPictureWidth /= (DotPulseNum * 8);
	//Printf("**BYTE*** gl_iMaxPictureWidth[%d]\n", gl_iMaxPictureWidth);

	// ���㵱ǰ�ļ�һ�����峤��
	//Printf("gl_iPulseNumY[%d] gl_iGraphWidth[%d]\n",gl_iPulseNumY ,gl_iGraphWidth);
	gl_iPulseNumY = (gl_iPulseNumY + gl_iGraphWidth * 8 + AdditionDot * 2 + OriginalSideDot + MotorSideDot) * DotPulseNum;//320
	//Printf("gl_iPulseNumY[%d]\n", gl_iPulseNumY);
	
	// ���㵱ǰ�ļ��о�
	//Printf("gl_iTHPulseNumX[%d] gl_dErrorX[%d]\n",gl_iTHPulseNumX ,gl_dErrorX);
	gl_iPulseNumX = gl_iTHPulseNumX + (int)(gl_dErrorX * X_PPMM);
	//Printf("gl_iPulseNumX[%d]\n", gl_iPulseNumX);
	

	// XYʵ��������峤��
	gl_lMaxX = IntRound((double)gl_sMachineLenght  * X_PPMM);  //��ͼ��XY���ֵ
	gl_lMaxY = IntRound((double)(gl_sMachineWidth+180) * Y_PPMM);  //������180mm
	gl_lMachineLenghtPluse = gl_lMaxX;
	gl_lMachineWidthPluse = gl_lMaxY;
	//Printf("gl_lMaxX[%d] gl_lMaxY[%d]\n", gl_lMaxX, gl_lMaxY);

	// ��ӡ��Ч��λ����ֵ
	gl_iYMaxEffectivePMM = gl_lMaxY - (175 * Y_PPMM);
	gl_iXMaxEffectivePMM = gl_lMaxX;
	//Printf("gl_iYMaxEffectivePMM[%d], gl_iXMaxEffectivePMM[%d]\n", gl_iYMaxEffectivePMM, gl_iXMaxEffectivePMM);

	//��������ɨ��֮����ĵ��� 
	gl_iStaggerDot = (int)(gl_dErrorY * Y_PPMM / DotPulseNum);
	//Printf("=====END CalVariableValue======\n");
}
*/

//�ж����ò����洢ֵ�Ƿ���ȷ
//0ʧ��,1�ɹ�
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
	//����ī����
	sTemp = (unsigned short *)&PrintPara;
	j = (sizeof(PrintPara) + 1) / 2;
	for (i=0;i<j;i++)
	{
		*sTemp = Readflash(address++);
		sTemp++;
	}
	//���и����
	sTemp = (unsigned short *)&CutPara;
	j = (sizeof(CutPara) + 1) / 2;
	for (i=0;i<j;i++)
	{
		*sTemp = Readflash(address++);
		sTemp++;
	}
	//��֤��ī����
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
	//20120809 ֻҪ��⵽���ų�����Χ�ͻָ�Ĭ��ֵ
	if ((PrintPara.cPenNo > 9)||(PrintPara.cPenNo < 1))
		PrintPara.cPenNo = 1;
	if ((PrintPara.cCutNo > 9)||(PrintPara.cCutNo < 1))
		PrintPara.cCutNo = 4;	
	
	
	gl_cSprinklerCount = PrintPara.cSprinklerCount;
	gl_cSprinklerNo = PrintPara.cSprinklerNo;
	gl_cBidirectional = PrintPara.cBidirectional;				//˫���ӡ
	gl_cLineWidth = PrintPara.cLineWidth;
	gl_cPenNo = PrintPara.cPenNo; // 20101206 �ʺŵ���
	gl_cCutNo = PrintPara.cCutNo;
	gl_cPrintCutSelect = PrintPara.cPrintCutSelect; // 20101206 ��ӡ�и�ѡ��
	
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
	//SetMacLengthWith(gl_sPlotType);//���û��������20100623
	
	
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

	//��֤�и����
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

	gl_cOverCutOffPaper =  CutPara.cOverCutOffPaper;	//��ӡ&�����ж�ֽ�� 20120906

	gl_cPHCDTime = CutPara.cPHCDTime;				//  20141021
	
	gl_sOverCutOffset = CutPara.sOverCutOffset;			// ƫ�ƺ����ж�ֽ�� 20120906

	gl_sMachineLenght = CutPara.sMachineLenght;
	//Printf("sMachineLenght:%d\n",CutPara.sMachineLenght);			

	gl_sMachineWidth = CutPara.sMachineWidth;
	Printf("sMachineWidth:%d\n",CutPara.sMachineWidth);


	//gl_iOriginalPos_X = CutPara.iOriginalPos_X;//ԭ������X 
	//gl_iOriginalPos_Y= CutPara.iOriginalPos_Y;//ԭ������Y
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

//20100622 WriteToFlash(unsigned int address)д���ݵ�FLASH
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
	//��ī��������
	//PrintPara.cChinaEn = 'C';
	PrintPara.cSprinklerCount = gl_cSprinklerCount;
	PrintPara.cSprinklerNo = gl_cSprinklerNo;
	PrintPara.cBidirectional = gl_cBidirectional;
	PrintPara.cLineWidth = gl_cLineWidth;
	PrintPara.cMultiDotLine12 = gl_cMultiDotLine12;
	PrintPara.cPrintCutSelect = gl_cPrintCutSelect; // 20101206 ��ӡ�и�ѡ��
	//PrintPara.cMultiDotLine23 = gl_cMultiDotLine23;
	//PrintPara.cMultiDotLine34 = gl_cMultiDotLine34;
	//PrintPara.cNull1 = 0;
	PrintPara.cPenNo = gl_cPenNo;// 20101206 �ʺŵ���
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
	//�и��������
	//CutPara.cCutPenNum = gl_cCutPenNum;
	CutPara.cLanguageSelect = gl_cLanguageSelect;		// 20131218
	//Printf("Language W %d\n", gl_cLanguageSelect);
	CutPara.cEndReturnRight = gl_cEndReturnRight;
	CutPara.cMovPaper = gl_cMovPaper;
	CutPara.cPumpSwitch = gl_cPumpSwitch;
	CutPara.cOverCutOffPaper = gl_cOverCutOffPaper; 	//��ӡ&�����ж�ֽ�� 20120906
	CutPara.cAutoSendPaperSet = gl_cAutoSendPaperSet;	//�Զ���ֽ����
	CutPara.cPHCDTime = gl_cPHCDTime;				//  20141021
	CutPara.sOverCutOffset = gl_sOverCutOffset;			// ƫ�ƺ����ж�ֽ�� 20120906
	
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
	CutPara.sMachineWidth = gl_sMachineWidth;	//��Ӧԭ���Ӧ����λ��
	
	Printf("gl_sMachineWidth:%d\n",gl_sMachineWidth);
	Printf("CutPara.sMachineWidth:%d\n",CutPara.sMachineWidth);
	CutPara.sMachineLenght = gl_sMachineLenght;

	//CutPara.iOriginalPos_X = gl_iOriginalPos_X;		//ԭ��x
	//CutPara.iOriginalPos_Y = gl_iOriginalPos_Y;		//ԭ��y
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
	//д�и����
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

	//���д���������С��һ��������С��ʣ�ಿ��ȫд0xffff	
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

//д���ò���
//0ʧ��,1�ɹ�
int WriteParameter(void)
{
	int iBakrINTMSK;
	int iTemp;
	
	iBakrINTMSK = rINTMSK;
	rINTMSK = BIT_GLOBAL;		//���������ж�
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
	rINTMSK = iBakrINTMSK;	//�ָ��ж�
	
	return iTemp;
}
