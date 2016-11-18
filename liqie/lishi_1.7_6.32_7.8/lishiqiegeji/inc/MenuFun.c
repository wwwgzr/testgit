
// ��ʽ�и���˵�����
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "..\src\defconst.h"
#include "..\Target\44blib.h"
#include "..\src\extern.h"


extern int GetInpDriveState(void);
extern int WaitKeyBound(unsigned char cKeyValue);
extern void SetLp(int axis, long wdata);
extern long ReadLp(int axis);
extern void get_status(int axis, int * value);
extern long IntRound(double x);
extern void DriveSprinkler(int iPulseNumX, int iPulseNumY);
extern int  ReceiveData(void);
extern void CtrlDownCut(void);
extern void CtrlUpCut(void);
extern void LcdDisp(char * s1, char * s2);
extern void get_inp_status(int * value);
extern void lcd_refresh(void);
extern void ClearPrintHead(char cOrgState);
extern void SetZAxisDevice(int Ispeed, int Dspeed, int Aspeed);
extern void CM_XYLengthCheck(void);
extern void MC_CommonMenuDispInit(void);
extern void ADM_PopMenuInit(void);
extern void ADM_ViewErrMenuInit(void);		//20141202 rwt


//��������F2
extern int ReceiveData(void);
extern int lcd_display(unsigned char row,const char *string);
extern int WaitKeyBound(unsigned char cKeyValue);
extern int WaitKeyExit(unsigned char cKeyValue);
extern int LineReverse(unsigned char line);
extern int InitPaper(void);
extern int WriteParameter(void);
extern int CalVariableValue(void);
extern int PauseRecover(void);

extern void ITMoveHandpiece(char cAix,int iMovePluse);
extern void CtrDownCut(void);
extern void CtrUpCut(void);

void CM_PenCutAttribute(void);
void CM_CutSectionLength(void);
void CM_CutGap(void);

//��ͨ�˵�

//��������

void Delete_All_File(void){
//	Printf("join delete file");
	/*ȷ�ϰ������� ֹͣ��ӡ ɾ���ڴ��е��ļ� ���� �����ָ���λ��*/
	
}

int SE_DriveErrorCheck(void)
{
	if (gl_cPrintStatus == 0x10)
	{
		if(gl_cLanguageSelect == 0)
		{
			lcd_display(0,"��������");
			lcd_display(1,"7:�˳�");
		}
		else
		{
			lcd_display(0,"Drive Error!");
			lcd_display(1,"7:EXIT");
		}
	
		WaitKeyExit(Key_Pause);
		lcd_refresh();
		return 1;
	}
	return 0;

}

//�ȴ�����岹��������
void FUN_WaitInpEnd(void)
{
	int iStatus;
	
	do
	{
		get_inp_status(&iStatus);
	} while (iStatus != 0);

}

//�ٶ�ѡ��
void FUNS_SpeedTerm(int iSpeedLevel)
{
	if(gl_cLanguageSelect == 0)
	{
		switch(iSpeedLevel)
		{
			case 0:
				lcd_display(1,"����");
				break;
			case 1:
				lcd_display(1,"�е���");
				break;
			case 2:
				lcd_display(1,"����");
				break;
			case 3:
				lcd_display(1,"�и���");
				break;
			case 4:
				lcd_display(1,"����");
				break;
		}
	}
	else
	{
		switch(iSpeedLevel)
		{
			case 0:
				lcd_display(1,"Low");
				break;
			case 1:
				lcd_display(1,"Low-middle");
				break;
			case 2:
				lcd_display(1,"Middle");
				break;
			case 3:
				lcd_display(1,"Middle-high");
				break;
			case 4:
				lcd_display(1,"High");
				break;
		}
	}

}


//����ѡ��
void FUNS_TimesTerm(int itimes)
{		
	itimes--;
		switch(itimes)
		{
			case 0:
				lcd_display(1,"1");
				break;
			case 1:
				lcd_display(1,"2");
				break;
			case 2:
				lcd_display(1,"3");
				break;
			case 3:
				lcd_display(1,"4");
				break;
			case 4:
				lcd_display(1,"5");
				break;
			case 5:
				lcd_display(1,"6");
				break;
			case 6:
				lcd_display(1,"7");
				break;
			case 7:
				lcd_display(1,"8");
				break;
			case 8:
				lcd_display(1,"9");
				break;
			case 9:
				lcd_display(1,"10");
				break;
		}

}

//**//
extern int gl_time;

void FUNS_ClearTime(void)
{
	unsigned char cReadKey;
	unsigned char cEnd ;
	int time = gl_time  , i = 0 ;
	
	if(gl_cLanguageSelect == 0)
	{
		lcd_display(0,"��ͷ��ϴʱ��:");
		FUNS_TimesTerm( time );
	}
	else
	{
		lcd_display(0,"nozzle cleaning time :");
		FUNS_TimesTerm( time);

	}
	cEnd = 1;
	while (cEnd)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		switch (cReadKey)
		{
			case Key_F2:	
				time++;
				if(time > 10)
					time = 0;
				FUNS_TimesTerm( time);
				break;
			
			case Key_F4:
				time = time + 2;
				if(time > 10)
					time = 0;
				FUNS_TimesTerm( time);
				break;
			case Key_F6:
				time = time - 2;
				if(time < 0)
					time = 10;
				FUNS_TimesTerm( time);
				break;

			case Key_F8:
				time--;
				if(time < 0)
					time =10;
				FUNS_TimesTerm( time);
				break;

			case Key_Ok:
				cEnd = 0;
				gl_time = time;
				WriteParameter();
				ClearPrintHead(0);
				Printf("gl_time = %d \n" , gl_time);
				break;

			case Key_Pause:
				cEnd = 0;
				break;
		}
	}
	

}


//**//

//������ī��ϴ����
void FUN_SetPrintClearTimes(void)
{
	unsigned char cReadKey;
	unsigned char cEnd ;
	int times  , i = 0 ;
	
	times = 1;
	if(gl_cLanguageSelect == 0)
	{
		lcd_display(0,"��ͷ��ϴ����:");
		FUNS_TimesTerm( times);
	}
	else
	{
		lcd_display(0,"nozzle cleaning times:");
		FUNS_TimesTerm( times);

	}
	cEnd = 1;
	while (cEnd)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		switch (cReadKey)
		{
			case Key_F2:	
				times++;
				if(times > 10)
					times = 0;
				FUNS_TimesTerm( times);
				break;

			case Key_F4:
				times = times + 2;
				if(times > 10)
					times = 0;
				FUNS_TimesTerm( times);
				break;
			case Key_F6:
				times = times - 2;
				if(times < 0)
					times = 10;
				FUNS_TimesTerm( times);
				break;

			case Key_F8:
				times--;
				if(times < 0)
					times =10;
				FUNS_TimesTerm( times);
				break;

			case Key_Ok:
				cEnd = 0;
				if(gl_cLanguageSelect == 0)
					{
					strcpy(gl_cLcdDispBuf[0],"��ϴ��ͷ");
					strcpy(gl_cLcdDispBuf[1],"���Ժ�...");
					}
				else
				{
					strcpy(gl_cLcdDispBuf[0],"clean shower");
					strcpy(gl_cLcdDispBuf[1],"please wait...");
				}
					lcd_refresh();
				if (gl_iAxisYPos <= gl_iOriginalPos)
				{
					if(gl_cSWFStartClearFlag != 1)	
					{
						gl_cSWFStartClearFlag = 1;
					}
					for(i = 0 ; i < times;i++)	
					{
						ClearPrintHead(times);
					}
				}
				break;

			case Key_Pause:
				cEnd = 0;
				break;
		}
	}

}





//�����ƶ��ٶ�
void FUN_SetMovSpeed(void)
{
	unsigned char cReadKey;
	unsigned char cEnd;
	int iMoveSpeed;

	iMoveSpeed = gl_iMoveFramSpeed;
	if(gl_cLanguageSelect == 0)
	{
		lcd_display(0,"�ƶ��ٶ�:");
		FUNS_SpeedTerm(iMoveSpeed);
	}
	else
	{
		lcd_display(0,"MOV SPEED:");
		FUNS_SpeedTerm(iMoveSpeed);
	}
	
	cEnd = 1;
	while (cEnd)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		switch (cReadKey)
		{
			case Key_F2:	
				iMoveSpeed++;
				if(iMoveSpeed > 4)
					iMoveSpeed = 0;
				FUNS_SpeedTerm(iMoveSpeed);
				break;

			case Key_F8:
				iMoveSpeed--;
				if(iMoveSpeed < 0)
					iMoveSpeed = 4;
				FUNS_SpeedTerm(iMoveSpeed);
				break;

			case Key_Ok:
				gl_iMoveFramSpeed = iMoveSpeed;
				WriteParameter();
				cEnd = 0;
				break;

			case Key_Pause:
				cEnd = 0;
				break;
		}
	}

}

//��ԭ��˵���ʾ
void CMS_OriginDispMenu(void)
{
	
  	LineReverse(0);
	if( gl_cLanguageSelect == 0 )
	{
		strcpy(gl_cLcdDispBuf[0],"2:��ֽ  8:��ֽ");
		strcpy(gl_cLcdDispBuf[1],"4:����  6:����");
	}
	else
	{
		strcpy(gl_cLcdDispBuf[0],"2:back 8:front");
		strcpy(gl_cLcdDispBuf[1],"4:left 6:right");
	}
	lcd_refresh();
}

//��ԭ��
void CM_SetOriginManage(void)
{
	int iMaxPosY,iPulseNum,iTotalPulseNum,iTotalPulseNumX;
	unsigned char cTemp,cEnd, cWend;
	int iRepeatPress;
	float dStep;
	
	Printf("Start AYPos:%d\n", gl_iAxisYPos);
	/*
  	//���ֽ���Ƿ����꣬��ֽ������Ӧ�����ڵ�
	while (1)
	{
	  	if (((gl_cReacterStatus & B_NOPAPER) != 0) && (gl_cPrintStatus != 0x10))
		{//ûֽ
			//Printf("Set orig\n");
			LineReverse(0);
			if( gl_cLanguageSelect == 0 )
			{
				strcpy(gl_cLcdDispBuf[0],"  ֽ��������");
				strcpy(gl_cLcdDispBuf[1],"  ����ֽ!");
			}
			else
			{
				strcpy(gl_cLcdDispBuf[0],"   NO PAPER!");
				strcpy(gl_cLcdDispBuf[1]," ");
			}
			lcd_refresh();
			WaitKeyExit(Key_Pause);
			Printf("Key down pause:\n");
			gl_cReacterStatus &= (~B_NOPAPER);
			InitPaper();
		}
		else
			break;
	}
*/
	CMS_OriginDispMenu();

	iTotalPulseNum = 0;
	iTotalPulseNumX = 0;
	iMaxPosY = (int)(gl_sPlotType * Y_PPMM) + (OriginalSideDot + 160) * DotPulseNum;
	Printf("MaxPosY:%d, gl_iAxisYPos{%d}\n", iMaxPosY, gl_iAxisYPos);

	
	cWend = 1;
	while (cWend == 1)
	{
		
		WaitKeyExit(0xff);		//�ȴ��м�����
		iRepeatPress = 0;

		cTemp = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;

		switch (cTemp)
		{
			case Key_F1:
				//�����ƶ��ٶ�
				FUN_SetMovSpeed();
				CMS_OriginDispMenu();
				break;
			
			case Key_F4:
			case Key_F6:	 
			{
				if(SE_DriveErrorCheck() == 1)
					break;
				
				SetMoveFrameSpeed(AXIS_Y);
				cEnd = 1;
				while (cEnd)
				{
					//ReceiveData();				//��������
					if (iRepeatPress > 50)
					{
						iRepeatPress += 2;
						if (iRepeatPress > 300)
							iRepeatPress = 300;
						dStep = (double)iRepeatPress * (double)iRepeatPress / 50.0 * 0.2 * Y_PPMM;
					}
					else
					{
						iRepeatPress++;
						dStep = (double)iRepeatPress * (double)iRepeatPress / 50.0 * 0.1;
						if (dStep < 0.3)
							dStep = 0.3;
						dStep = dStep * Y_PPMM;
					}
					if (cTemp == Key_F4)
						iPulseNum =  (int)dStep;
					else
						iPulseNum = -(int)dStep;
					//Printf("ipN;%d, ", iPulseNum);
					
					if ((gl_iAxisYPos + iPulseNum) >= iMaxPosY)
					{		
						if(gl_iAxisYPos == iMaxPosY)
							iPulseNum = 0;
						else
							iPulseNum = iMaxPosY - gl_iAxisYPos;

					}
					else if ((gl_iAxisYPos + iPulseNum) <= 0)
						{
							if(gl_iAxisYPos == 0)
								iPulseNum = 0;
							else
								iPulseNum = -gl_iAxisYPos;
						}
					
					//Printf("iPulseNum;%d, ", iPulseNum);
					gl_iAxisYPos += iPulseNum;
					iTotalPulseNum += iPulseNum;
					if (iPulseNum != 0)
					{
						ITMoveHandpiece(AXIS_Y, iPulseNum);
					}
					
					while (gl_cKeyPE != gl_cKeyPH)
					{
						if (gl_cKeyBuf[gl_cKeyPE] != cTemp)
						{
							cEnd = 0;
							break;
						}
						else if ((gl_cKeyPE +1) == KeyBufLength)
							gl_cKeyPE = 0;
						else
							gl_cKeyPE++;
					}
					
					if (gl_cPrintStatus == 0x10)
		      			cEnd = 0;
				}
			}
			break;

			case Key_F2:
			case Key_F8:
			{
				if(SE_DriveErrorCheck() == 1)
					break;

				SetMoveFrameSpeed(AXIS_X);

				cEnd = 1;
				while (cEnd)
				{
					//ReceiveData();				//��������
					if (iRepeatPress > 50)
					{
						iRepeatPress += 2;
						if (iRepeatPress > 90)
							iRepeatPress = 90;
						dStep = (double)iRepeatPress * (double)iRepeatPress / 50.0 * 0.2 * X_PPMM;
					}
					else
					{
						iRepeatPress++;
						dStep = (double)iRepeatPress * (double)iRepeatPress / 50.0 * 0.1;
						if (dStep < 0.5)
							dStep = 0.5;
						dStep = dStep * X_PPMM;
					}
					if (cTemp == Key_F2)
						iPulseNum = (int)dStep;
					else
						iPulseNum = -(int)dStep;
					if (iPulseNum != 0)
					{
						//if (cTemp == Key_Up)
						//{
						if ((GetIOBit(I_ROLLPEND) == StatusOk) && (cTemp == Key_F2)) 	//20141209 rwt
						{
							Printf("rollpend:\n");
							cEnd = 0;
							break;
						}
						//}
						else
						{
							if ((GetIOBit(I_GIVEPEND) == StatusOk) && (cTemp == Key_F8))
							{
								Printf("Givep end : %d \n", iPulseNum);
								cEnd = 0;
								break;
							}
						}
						iTotalPulseNumX += iPulseNum;
						ITMoveHandpiece(AXIS_X, iPulseNum);
					}

					while (gl_cKeyPE != gl_cKeyPH)
					{
						if (gl_cKeyBuf[gl_cKeyPE] != cTemp)
						{
							cEnd = 0;							
							break;
						}
						else if ((gl_cKeyPE +1) == KeyBufLength)
							gl_cKeyPE = 0;
						else
							gl_cKeyPE++;
					}
					if (gl_cPrintStatus == 0x10)
						cEnd = 0;
				}
			}
			break;
			
			case Key_Pause: 		//�˳��Ӳ˵��������˵�
				WaitKeyBound(Key_Pause);
				if (((iTotalPulseNum != 0) || (iTotalPulseNumX != 0)) && (gl_cPrintStatus != 0x10))
				{
					iTotalPulseNum = -iTotalPulseNum;
					iTotalPulseNumX = -iTotalPulseNumX;
					
					Printf("iTotalPulseNum = %d, iTotalPluseNumx = %d\n",iTotalPulseNum, iTotalPulseNumX); 	//20141209 rwt
					SetMovFrameSpeed(500);
					inp_move2(iTotalPulseNumX,iTotalPulseNum);
					FUN_WaitInpEnd();
					
					gl_iAxisYPos += iTotalPulseNum;
				}
				cWend = 0;
			break;
			
			case Key_Ok:
				//if (gl_cPrintStatus == 0x10)
				//	break;
				//if (gl_iAxisYPos > iMaxPosY)
				//	iTotalPulseNum = (int)(gl_sPlotType * Y_PPMM);
				//else if (gl_iAxisYPos < (OriginalSideDot + 160) * DotPulseNum)
				//	iTotalPulseNum = 0;
				//else
				//	iTotalPulseNum = gl_iAxisYPos - (OriginalSideDot + 160) * DotPulseNum;
				Printf("***************AYPos:%d\n", gl_iAxisYPos);
				if (gl_iAxisYPos != gl_iOriginalPos)
				{
					gl_iOriginalPos = gl_iAxisYPos;
					Printf("Start ORG gl_iAxisYPos:%d, gl_iOriginalPos:%d\n", gl_iAxisYPos, gl_iOriginalPos);
					WriteParameter();  //д���������ļ�
					CalVariableValue();
				}
				
				//if (GetIOBit(I_GIVEPEND) == StatusOk)
				if(gl_cAutoSendPaperSet != 0)
				{
					Printf("iTotalPluseNumx = %d\n",iTotalPulseNumX); 	//20141209 rwt
					if ((iTotalPulseNumX < 0) || (GetIOBit(I_GIVEPEND) == StatusOk))	//20141209 rwt
						InitPaper();
				}
				
				if (((gl_cFileReceive & 0x01) == 0x01) && (gl_iGraphWidth > gl_iMaxPictureWidth))
				{
					if(gl_cLanguageSelect == 0)
					{
						strcpy(gl_cLcdDispBuf[0],"��ӡͼ�γ���");
						strcpy(gl_cLcdDispBuf[1]," ");
					}
					else
					{
						strcpy(gl_cLcdDispBuf[0],"Y:out-of-range");
						strcpy(gl_cLcdDispBuf[1]," ");
					}
					lcd_refresh();
					while (1);
				}
				cWend = 0;
			break;

			default:
			iRepeatPress = 0;
			cTemp = 0xff;
			break;
		}
	}	
}

void CM_PrintTestGrap(void)
{//��ӡ����ͼ
	unsigned int address;
	unsigned int i,j,k;
	unsigned int icount;
	int iPosNumY;
	int iGapPixel;
	unsigned char cSprinklerNum;
	unsigned int sdat[7];
	int iPosNumX;
	if( gl_cLanguageSelect == 0 )
	{
		strcpy(gl_cLcdDispBuf[0],"����ͼ�δ�ӡ");
		strcpy(gl_cLcdDispBuf[1],"���Ժ�...");
	}
	else
	{
		strcpy(gl_cLcdDispBuf[0],"TestChart Print");
		strcpy(gl_cLcdDispBuf[1],"Please Wait...");
	}
	lcd_refresh();
	
	iGapPixel = (int)(50.0 * Y_PPMM / DotPulseNum);
	iPosNumY = (15 * iGapPixel + 3 + 320) * DotPulseNum;
	if (((gl_iAxisYPos + iPosNumY) < 0) || ((gl_iAxisYPos + iPosNumY) > gl_iMaxYMM))
	{
		if( gl_cLanguageSelect == 0 )
		{
			strcpy(gl_cLcdDispBuf[1]," ");
			strcpy(gl_cLcdDispBuf[0]," Y���ƶ�����!");
		}
		else
		{
			strcpy(gl_cLcdDispBuf[1]," ");
			strcpy(gl_cLcdDispBuf[0],"Y:out-of-range");
		}
		lcd_refresh();
		WaitKeyExit(Key_Pause);
		return;
	}
	
	ClearPrintHead(0);
	WriteFilePrint(0xaaaa);
	ClrFIFO;
	iPosNumX = (unsigned int)(50.0 * DotLineWidth * X_PPMM);
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
		ClrFIFO;		//��FIFO������
		for (i=0;i<3;i++)
		{
			address = gl_iWriteFIFOAddr + 28 * cSprinklerNum;
			for (j=0;j<7;j++)
			{//���߿�
				WriteFIFO(address,0x3fffff);
				address += 4;
			}
		}
		while (ReadFIFOFull() == 0)//����������
		{
			address = gl_iWriteFIFOAddr + 28 * cSprinklerNum;
			for (j=0;j<7;j++)
			{
				WriteFIFO(address,sdat[j]);
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
				{//��������
					if (ReadPrintEnd() != 0)
						break;
				}
				if (ReadPrintEnd() != 0)
					break;
				address = gl_iWriteFIFOAddr + 28 * cSprinklerNum;
				for (j=0;j<7;j++)
				{
					WriteFIFO(address,sdat[j]);
					address += 4;
				}
			}
			for (i=0;i<3;i++)
			{
				while (ReadFIFOFull() == 1)
				{//��������
					if (ReadPrintEnd() != 0)
						break;
				}
				if (ReadPrintEnd() != 0)
					break;
				address = gl_iWriteFIFOAddr + 28 * cSprinklerNum;
				for (j=0;j<7;j++)
				{
					WriteFIFO(address,0x3fffff);
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
		DriveSprinkler(-iPosNumX,0);
		WaitPrintEnd();
		DriveSprinkler(0,iPosNumY);
		gl_iAxisYPos += iPosNumY;
		iPosNumY = -iPosNumY;
		WaitPrintEnd();
	}
	iPosNumX = (unsigned int)((float)SprinklerWidth * DotLineWidth * X_PPMM);
	iPosNumX += gl_iPulseNumX;
	DriveSprinkler(-iPosNumX,0);
	WaitPrintEnd();

}
//****************************************************************************

//****************************************************************************
//�ٶ�����
void CM_SetSpeed(void)
{
	unsigned char cReadKey;
	unsigned char cEnd;
	char cSpeedSelect;
	int iMoveSpeed;

	cSpeedSelect = 0;
	iMoveSpeed = gl_iCutSpeed;
	if(gl_cLanguageSelect == 0)
	{
		lcd_display(0,"�и��ٶ�:");
		FUNS_SpeedTerm(iMoveSpeed);
	}
	else
	{
		lcd_display(0,"CUT SPEED:");
		FUNS_SpeedTerm(iMoveSpeed);
	}
	
	cEnd = 1;
	while (cEnd)
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
				cSpeedSelect = cSpeedSelect == 0 ? 1 : 0;
				
				if(cSpeedSelect == 0)
				{
					gl_iMoveFramSpeed = iMoveSpeed;
					iMoveSpeed = gl_iCutSpeed;
					if(gl_cLanguageSelect == 0)
					{
						lcd_display(0,"�и��ٶ�:");
						FUNS_SpeedTerm(iMoveSpeed);
					}
					else
					{
						lcd_display(0,"CUT SPEED:");
						FUNS_SpeedTerm(iMoveSpeed);
					}
				}
				else
				{
					gl_iCutSpeed = iMoveSpeed;
					iMoveSpeed = gl_iMoveFramSpeed;
					if(gl_cLanguageSelect == 0)
					{
						lcd_display(0,"�ƶ��ٶ�:");
						FUNS_SpeedTerm(iMoveSpeed);
					}
					else
					{
						lcd_display(0,"MOV SPEED:");
						FUNS_SpeedTerm(iMoveSpeed);
					}
				}
				break;
				
			case Key_F2:	
				iMoveSpeed++;
				if(iMoveSpeed > 4)
					iMoveSpeed = 0;
				FUNS_SpeedTerm(iMoveSpeed);
				break;

			case Key_F8:
				iMoveSpeed--;
				if(iMoveSpeed < 0)
					iMoveSpeed = 4;
				FUNS_SpeedTerm(iMoveSpeed);
				break;

			case Key_Ok:
				if(cSpeedSelect == 0)
				{
					gl_iCutSpeed = iMoveSpeed;
					InitSpeedCurve();// ���ٶ�����
				}
				else
					gl_iMoveFramSpeed = iMoveSpeed;
				WriteParameter();
				cEnd = 0;
				break;

			case Key_Pause:
				cEnd = 0;
				break;
		}
	}	
}
//********************************************************************

//&&&&&&&&�˵���&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*******************̧�䵶��ʱ****************************************

void CMSU_Test(void)
{
	
	int iDrivePluseLenX, iDrivePluseLenY;	
	int iToalPluseX, iToalPluseY;
	int iSectionLenX, iSectionLenY, iGapX, iGapY;
	int iToalSectionX, iToalSectionY;
	
	iDrivePluseLenX = 100 * X_PPMM;
	iDrivePluseLenY = 100 * Y_PPMM;

	iSectionLenX = gl_iCutSectionLen * X_PPMM;//�䵶��
	iSectionLenY = gl_iCutSectionLen * Y_PPMM;
	
	iGapX = gl_iCutSectionGap * gl_dPosUnitX;//̧����
	iGapY = gl_iCutSectionGap * gl_dPosUnitY;

	iToalSectionX = iSectionLenX + iGapX;	//���ܳ�
	iToalSectionY = iSectionLenY + iGapY;
	Printf("SLX:%d, SLY:%d, GX:%d, GY:%d\n", iSectionLenX, iSectionLenY, iGapX, iGapY);
	Printf("ToalSecX:%d, ToalSecY:%d\n", iToalSectionX, iToalSectionY);

	// ��	+
	SetMoveFrameSpeed(AXIS_Y);
	iToalPluseY = 0;
	while(1)
	{
		if((iToalPluseY+iToalSectionY) < iDrivePluseLenY)
		{
			if(gl_cCutUpDownState != 1)
				CtrDownCut();
			ITMoveHandpiece(AXIS_Y, iSectionLenY);
			CtrUpCut();
			ITMoveHandpiece(AXIS_Y, iGapY);
			iToalPluseY += iToalSectionY;
		}
		else
		{
			if(gl_cCutUpDownState != 1)
				CtrDownCut();
			ITMoveHandpiece(AXIS_Y, (iDrivePluseLenY-iToalPluseY));
			break;
		}
	}
	
	// ��	-
	SetMoveFrameSpeed(AXIS_X);
	iToalPluseX = 0;
	while(1)
	{
		if((iToalPluseX+iToalSectionX) < iDrivePluseLenX)
		{
			if(gl_cCutUpDownState != 1)
				CtrDownCut();
			ITMoveHandpiece(AXIS_X, -iSectionLenX);
			CtrUpCut();
			ITMoveHandpiece(AXIS_X, -iGapX);
			iToalPluseX += iToalSectionX;
		}
		else
		{
			if(gl_cCutUpDownState != 1)
				CtrDownCut();
			ITMoveHandpiece(AXIS_X, -(iDrivePluseLenX-iToalPluseX));
			break;
		}
	}
	
	// ��	-
	SetMoveFrameSpeed(AXIS_Y);
	iToalPluseY = 0;
	while(1)
	{
		if((iToalPluseY+iToalSectionY) < iDrivePluseLenY)
		{
			if(gl_cCutUpDownState != 1)
				CtrDownCut();
			ITMoveHandpiece(AXIS_Y, -iSectionLenY);
			CtrUpCut();
			ITMoveHandpiece(AXIS_Y, -iGapY);
			iToalPluseY += iToalSectionY;
		}
		else
		{
			if(gl_cCutUpDownState != 1)
				CtrDownCut();
			ITMoveHandpiece(AXIS_Y, -(iDrivePluseLenY-iToalPluseY));
			break;
		}
	}
	

	// ��	+
	SetMoveFrameSpeed(AXIS_X);
	iToalPluseX = 0;
	while(1)
	{
		if((iToalPluseX+iToalSectionX) < iDrivePluseLenX)
		{
			if(gl_cCutUpDownState != 1)
				CtrDownCut();
			ITMoveHandpiece(AXIS_X, iSectionLenX);
			CtrUpCut();
			ITMoveHandpiece(AXIS_X, iGapX);
			iToalPluseX += iToalSectionX;
		}
		else
		{
			if(gl_cCutUpDownState != 1)
				CtrDownCut();
			ITMoveHandpiece(AXIS_X, (iDrivePluseLenX-iToalPluseX));
			break;
		}
	}
	
	if(gl_cCutUpDownState != 0)
		CtrUpCut();
}

void CMSU_Menu(void)
{
	if(gl_cLanguageSelect == 0)
	{
		strcpy(gl_cLcdDispBuf[0], "̧�䵶����");
		strcpy(gl_cLcdDispBuf[1], "10*10cm");
	}
	else
	{
		strcpy(gl_cLcdDispBuf[0], "Up Down Test");
		strcpy(gl_cLcdDispBuf[1], "10*10cm");
	}
	lcd_refresh();

}

void CMS_UPDTest(void)
{
	char cWstate, cReadKey;
	
	CMSU_Menu();

	cWstate = 1;
	while(cWstate)
	{
		//CtrDownCut();
		//CtrUpCut();

		if(gl_cKeyPH != gl_cKeyPE)
		{
			cReadKey = gl_cKeyBuf[gl_cKeyPE];
			if ((gl_cKeyPE+1) == KeyBufLength)
				gl_cKeyPE = 0;
			else
				gl_cKeyPE++;
			
			switch(cReadKey)
			{
				case Key_F1://�и��
					CM_CutSectionLength();
					CMSU_Menu();
					break;
					
				case Key_F2://�и��϶
					CM_CutGap();
					CMSU_Menu();
					break;

				case Key_F3://�ƶ��ٶ�
					FUN_SetMovSpeed();
					CMSU_Menu();
					break;

				case Key_F4://����ԭ��
					CM_SetOriginManage();
					CMSU_Menu();
					break;

				case Key_Ok://�и����
					CMSU_Test();
					break;

				case Key_Pause://ȷ���˳�
					cWstate = 0;
					break;
			}
		}
	}
}


void CMS_LiftDropDelayMenu(int iUpDelay, int iDownDelay, char cADstate)
{
	if(gl_cLanguageSelect == 0)
	{
		sprintf(gl_cLcdDispBuf[0], "�䵶��ʱ:%d/%c", iDownDelay,cADstate);
		sprintf(gl_cLcdDispBuf[1], "̧����ʱ:%d/%c", iUpDelay,cADstate);
	}
	else
	{
		sprintf(gl_cLcdDispBuf[0], "Down Delay:%d/%c", iDownDelay,cADstate);
		sprintf(gl_cLcdDispBuf[0], "Up Delay:%d/%c", iUpDelay,cADstate);
	}
	lcd_refresh();

}

//**********̧�����ʱ����
void CM_SetLiftDropCutDelay(void)
{
	char cWstate, cReadKey;
	char cADState;
	//char cDispbuf[2][8];
	static int iUpDelay, iDownDelay, iADValue;
	static int ioldUpDelay, ioldDownDelay;
	
	iUpDelay = gl_iCutUpDelay;
	iDownDelay = gl_iCutDownDelay/10;
	ioldUpDelay = iUpDelay;
	ioldDownDelay = iDownDelay;
	Printf("UP Delay[%d] Down Delay[%d]\n", iUpDelay, iDownDelay);

	cADState = 'B';

	CMS_LiftDropDelayMenu(iUpDelay, iDownDelay, cADState);
	iADValue = 10;
	cWstate = 1;
	while(cWstate)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;

		switch(cReadKey)
		{
			case Key_F2:
				iUpDelay += iADValue;
				if(iUpDelay > 2000)
					iUpDelay = 1;
				CMS_LiftDropDelayMenu(iUpDelay, iDownDelay, cADState);			
				break;

			case Key_F8:
				iUpDelay -= iADValue;
				if(iUpDelay < 1)
					iUpDelay = 2000;
				CMS_LiftDropDelayMenu(iUpDelay, iDownDelay, cADState);
				break;

			case Key_F4:
				iDownDelay -= iADValue;
				if(iDownDelay < 1)
					iDownDelay = 2000;
				CMS_LiftDropDelayMenu(iUpDelay, iDownDelay, cADState);
				break;

			case Key_F6:
				iDownDelay += iADValue;
				if(iDownDelay > 2000)
					iDownDelay = 1;
				CMS_LiftDropDelayMenu(iUpDelay, iDownDelay, cADState);
				break;

			case Key_F3:
				if(iADValue == 1)
				{
					iADValue = 10;
					cADState= 'B';
				}
				else if(iADValue == 10)
				{	
					iADValue = 100;
					cADState= 'C';
				}
				else
				{
					iADValue = 1;
					cADState= 'A';
				}
				CMS_LiftDropDelayMenu(iUpDelay, iDownDelay, cADState);
				break;
			
			case Key_F9:
				gl_iCutUpDelay = iUpDelay;//100
				gl_iCutDownDelay = iDownDelay*10;
				Printf("Up:%d, Down:%d\n", gl_iCutUpDelay, gl_iCutDownDelay);
				CMS_UPDTest();
				gl_iCutUpDelay = ioldUpDelay;
				gl_iCutDownDelay = ioldDownDelay*10;
				break;

			case Key_F1:
				CM_PenCutAttribute();
				cWstate = 0;
				break;
				
			case Key_Ok:
				gl_iCutUpDelay = iUpDelay;
				gl_iCutDownDelay = iDownDelay*10;
				WriteParameter();
				cWstate = 0;
				break;

			case Key_Pause:
				cWstate = 0;
				break;
			
		}
	}

}
//*********************************************************************

//*******************�ʵ��������**************************************
void CMP_PrintDrive(int iPulseNumY)
{
	if(iPulseNumY != 0)
	{
		if(gl_cBidirectional == 1)
		{
			if(iPulseNumY < 0)
			{
				WriteDirct(0xaaaa);//��ӡ����
				Printf("-");
			}
			else
			{
				WriteDirct(0x5555);//��ӡ����
				Printf("+");
			}
		}
		else
		{
			if(iPulseNumY > 0)
			{
				WriteDirct(0x5555);//��ӡ����
				//Delay(50);
				Printf("+");
			}
		}
			
		Write312YAxisPlus(iPulseNumY);// 408
		StraightLineADSpeed(AXIS_Y, Print_SV, Print_V, Print_T+300);
		if(iPulseNumY < 0)
			iPulseNumY += -3000;
		else
			iPulseNumY += 3000;
		//Printf("AA:YPlus[%d]\n", iPulseNumY);
		pmove(AXIS_Y, iPulseNumY);
					
		while (ReadPrintEnd() != 0);		
		//ITMoveHandpiece(AXIS_Y, iPulseNumY);
	}

}

void CMP_PrintTestGrap(void)
{
	unsigned char m,j;
	unsigned char cSendPosFlag;
	unsigned int address;
	int i;// iStatus;
	int iPosNumY; // Y��������
	int iTotalPixel; // ��ӡ������
	int spurtHeadStartPrtPix[2], spurtHeadEndtPrtPix[2]; //��ͷ1,2��ʼ/�������ص㣻0: ��ͷ1  , 1: ��ͷ2
	int lineWidth; //��ӡ�߿�
	int iGapPixel; // ͼ�����ص���
	int iOffsetPluse; 
	
	char tBidirec;
	//int iStatus;
	long lTempYPluse;
	
	// 2*2cm ���������� 
	unsigned int ptdataA[7] = {0xffffff, 0xffffff, 0xffffff, 0xffffff, 0x03ff, 0x0, 0x0};
	unsigned int ptdataB[7] = {0x0, 0x0, 0x0, 0x0, 0x300, 0x0, 0x0};

	tBidirec = gl_cBidirectional;
	
	lineWidth = 3;
	
	//����Y�����峤��
	iGapPixel = (int)((20* Y_PPMM) / DotPulseNum);
	iTotalPixel = iGapPixel+2 * AdditionDot + OriginalSideDot + MotorSideDot;
	iPosNumY = (iTotalPixel) * DotPulseNum;

	//�ж�Y�Ƿ񳬽� 20120905
	if(iPosNumY > gl_iYMaxEffectivePMM)
	{
		DriveSprinkler(-(gl_iPulseNumX+25*X_PPMM),0);//����X����ֽ
	}

	lTempYPluse = gl_iAxisYPos + LeftMargin;
		
	FixedSpeeDrive(AXIS_Y,3000);
	ITMoveHandpiece(AXIS_Y, -lTempYPluse);
	
	PrintHeadClean(2);
	Delay(2000);

	iOffsetPluse = (AdditionDot + OriginalSideDot) * DotPulseNum;

	lTempYPluse -= iOffsetPluse;
	ITMoveHandpiece(AXIS_Y, lTempYPluse);

	Delay(5000);
	
	WriteFilePrint(0xaaaa);
	WriteDirct(0x5555);//��ӡ����
	ClrFIFO;
	
	WriteFIFOFull(gl_cSprinklerCount-1);
	//Printf("FIFOADDR:0x%x, %d\n",gl_iWriteFIFOAddr,gl_cSprinklerCount);
	//Printf("iGapPixel:%d\n",iGapPixel);

	spurtHeadStartPrtPix[0] = AdditionDot + OriginalSideDot; // + iOffsetPixel;//��ͷ1��ʼ���ص�
	spurtHeadStartPrtPix[1] = spurtHeadStartPrtPix[0]  + SprinklerSpace;

	spurtHeadEndtPrtPix[0] = spurtHeadStartPrtPix[0] + iGapPixel;//��ͷ1�������ص�
	spurtHeadEndtPrtPix[1] = spurtHeadStartPrtPix[1] + iGapPixel;

	//Printf("ss0: %d, se0: %d\n", spurtHeadStartPrtPix[0], spurtHeadEndtPrtPix[0] );
	//Printf("ss1: %d, se1: %d\n", spurtHeadStartPrtPix[1], spurtHeadEndtPrtPix[1]);

	i = 0;
	cSendPosFlag = 0;
	while (1)
	{
		if (i > iTotalPixel)
			break;

		address = gl_iWriteFIFOAddr;
		for (m=0;m<2;m++)
		{//  m:0 ��ͷ1����  m:1 ��ͷ2����
			while (ReadFIFOFull() != 0)
			{
				if (cSendPosFlag == 0)
				{
					CMP_PrintDrive(iPosNumY);
					cSendPosFlag = 1;
					//Printf("Start Printf! i:%d\n", i);
				}
				
				if (ReadPrintEnd() != 0)
					break;
			}

			if((1==cSendPosFlag) && (ReadPrintEnd() != 0))
				break;

			if ((i > spurtHeadEndtPrtPix[m]+lineWidth) || (i < spurtHeadStartPrtPix[m]))
			{//����
				for (j=0;j<7;j++)
				{
					WriteFIFO(address,0);
					address += 4;
				}
			}
			else if (i < (spurtHeadStartPrtPix[m] + lineWidth) || (i > spurtHeadEndtPrtPix[m]) )
			{
				for (j=0;j<7;j++)
				{
					if(m==0)
					{// ����
						WriteFIFO(address,0x3ffffff);
					}
					else
					{// ����
						WriteFIFO(address, ptdataA[j]);
					}
					address += 4;
				}
			}
			else
			{
				for (j=0;j<7;j++)
				{
					if(m==0)
					{//  ����
						if(j == 0)
							WriteFIFO(address,0x03);
						else
							WriteFIFO(address, 0);
					}
					else
					{// ����
						WriteFIFO(address, ptdataB[j]);
					}
					address += 4;
				}
			}
		}
		i++;
		
		if((1==cSendPosFlag) && (ReadPrintEnd() != 0))
			break;
	}
	//Printf("*************\n\n");
	WaitDriveEnd(AXIS_Y);
	Delay(1000);

	iPosNumY -= iOffsetPluse;
	CMP_PrintDrive(-iPosNumY);
	WaitDriveEnd(AXIS_Y);
	
	gl_cBidirectional = tBidirec ;
	 
	return;

}

void CMP_CutExt(int iAxis, int iToalSection, int iDrivePluseLen, int iSectionLen, int iGap)
{

	if(gl_cCutUpDownState != 1)
		CtrDownCut();
	
	ITMoveHandpiece(iAxis, iSectionLen);
	CtrUpCut();

	/*
	int iToalPluse;
	int iCompPluse;
	
	SetMoveFrameSpeed(iAxis);

	if(iAxis == AXIS_X)
		iCompPluse = 0;
	else
		iCompPluse = 0.5*Y_PPMM;
	
	iToalPluse = 0;
	while(1)
	{
		if((iToalPluse+iToalSection) <=iDrivePluseLen)
		{
			if(gl_cCutUpDownState != 1)
				CtrDownCut();
			ITMoveHandpiece(iAxis, iSectionLen);
			CtrUpCut();
			ITMoveHandpiece(iAxis, iGap);
			iToalPluse += iToalSection;
		}
		else
		{
			if(gl_cCutUpDownState != 1)
				CtrDownCut();
			if(iSectionLen > 0)
				ITMoveHandpiece(iAxis, (iDrivePluseLen-iToalPluse));
			else
				ITMoveHandpiece(iAxis, -(iDrivePluseLen-iToalPluse));
			
			break;
		}
	}
	CtrUpCut();
	*/
}


void CMP_CutTest(int iCutOffsetX, int iCutOffsetY)
{
	int iOffsetPluseX, iOffsetPluseY;
	int iStatus;

	int iDrivePluseLenX, iDrivePluseLenY;	
	int iSectionLenX, iSectionLenY, iGapX, iGapY;
	int iToalSectionX, iToalSectionY;
	
	
	iOffsetPluseX = iCutOffsetX * DotPulseNum;
	iOffsetPluseY = iCutOffsetY * DotPulseNum;

	iDrivePluseLenX = 20 * X_PPMM;
	iDrivePluseLenY = 20 * Y_PPMM;

	iSectionLenX = 20 * X_PPMM;//�䵶��
	iSectionLenY = 20 * Y_PPMM;
	
	iGapX = 15 * gl_dPosUnitX;//̧����
	iGapY = 15 * gl_dPosUnitY;

	iToalSectionX = iSectionLenX + iGapX;	//���ܳ�
	iToalSectionY = iSectionLenY + iGapY;
	Printf("iOffseY:%d, iOffsetX:%d\n", iOffsetPluseY, iOffsetPluseX);
	Printf("SLX:%d, SLY:%d, GX:%d, GY:%d\n", iSectionLenX, iSectionLenY, iGapX, iGapY);
	Printf("ToalSecX:%d, ToalSecY:%d\n", iToalSectionX, iToalSectionY);
	//��λ���и��
	SetSDPenSpeed();
	inp_move2(iOffsetPluseX,iOffsetPluseY);
	do
	{
		get_inp_status(&iStatus);
	} while (iStatus != 0);	
 
	//�䵶
	CMP_CutExt(AXIS_Y, iToalSectionY, iDrivePluseLenY, iSectionLenY, iGapY);

	CMP_CutExt(AXIS_X, iToalSectionX, iDrivePluseLenX, -iSectionLenX, -iGapX);

	CMP_CutExt(AXIS_Y, iToalSectionY, iDrivePluseLenY, -iSectionLenY, -iGapY);

	CMP_CutExt(AXIS_X, iToalSectionX, iDrivePluseLenX, iSectionLenX, iGapX);
 
	SetSDPenSpeed();
	inp_move2(-iOffsetPluseX,-iOffsetPluseY);
	do
	{
		get_inp_status(&iStatus);
	} while (iStatus != 0);	
	
}

void CM_PenCutSpacing(void)
{
	unsigned char cReadKey;//��ֵ
	char  state ;
	char  cDispCutDistanceBuf[20];
	int  iWorkNum;
	long iTempXPluse, iTempYPluse;
	//int tdCutDistanceY, tdCutDistanceX;
	int iCutDistanceY, iCutDistanceX;
	int iOffsetNumLen;
	char WorkMode;

	
	iCutDistanceY = gl_iCutDistanceY;
	iCutDistanceX = gl_iCutDistanceX;
	//��ʾY��ƫ��
	if(gl_cLanguageSelect == 0)
		strcpy(gl_cLcdDispBuf[0], "�絶�������:");
		//strcpy(gl_cLcdDispBuf[1], " ");
	else
		strcpy(gl_cLcdDispBuf[0], "Cut Spacing:");
		//strcpy(gl_cLcdDispBuf[1], " ");	
	sprintf(cDispCutDistanceBuf, "Y:%9d", iCutDistanceY);
	strcpy(gl_cLcdDispBuf[1], cDispCutDistanceBuf);
	lcd_refresh();

	iOffsetNumLen = 87;//mm
	WorkMode = 0;
	iWorkNum = 2;
	iTempXPluse = 0;
	iTempYPluse = 0;
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
			case Key_F1:	//����̧�䵶��ʱ����
				CM_SetLiftDropCutDelay();
				state = 0;
				break;
				
			case Key_F3:	//�л�XY����
				if(WorkMode != 1)
				{
					WorkMode = 1;
					sprintf(cDispCutDistanceBuf, "X:%9d", iCutDistanceX);
					lcd_display(1, cDispCutDistanceBuf);
				}
				else{
						WorkMode = 0;
						sprintf(cDispCutDistanceBuf, "Y:%9d", iCutDistanceY);
						lcd_display(1, cDispCutDistanceBuf);
				}
				break;
				
			case Key_F9:	//���Աʵ����
				//���Աʵ��غ�
				FixedSpeeDrive(AXIS_Y,1000);
				ITMoveHandpiece(AXIS_Y, (iOffsetNumLen*Y_PPMM));
				gl_iAxisYPos += (iOffsetNumLen*Y_PPMM);
				CMP_PrintTestGrap();
				
				//FixedSpeeDrive(AXIS_Y,2000);
				//ITMoveHandpiece(AXIS_Y, (iOffsetNumLen*Y_PPMM));
				
				CMP_CutTest(iCutDistanceX, iCutDistanceY);
				
				FixedSpeeDrive(AXIS_Y,1000);
				ITMoveHandpiece(AXIS_Y, -(iOffsetNumLen*Y_PPMM));
				gl_iAxisYPos -= (iOffsetNumLen*Y_PPMM);
				iOffsetNumLen += 35;
				break;
	
			case Key_F2:	//��ֵ +1
				if(WorkMode == 0)
				{
					iCutDistanceY += 1;
					if (iCutDistanceY > 2000)
						iCutDistanceY = 0;
					sprintf(cDispCutDistanceBuf, "Y:%9d", iCutDistanceY);
					lcd_display(1, cDispCutDistanceBuf);
				}
				else{
						iCutDistanceX += 1;
						if (iCutDistanceX > 2000)
							iCutDistanceX = 0;	
						sprintf(cDispCutDistanceBuf, "X:%9d", iCutDistanceX);
						lcd_display(1, cDispCutDistanceBuf);
				}
					
				break;

			case Key_F8:	//��ֵ -1
				if(WorkMode == 0)
				{
					iCutDistanceY -= 1;
					if (iCutDistanceY < -2000)
						iCutDistanceY = 0;	
					sprintf(cDispCutDistanceBuf, "Y:%9d", iCutDistanceY);
					lcd_display(1, cDispCutDistanceBuf);
				}
				else{
						iCutDistanceX-= 1;
						if (iCutDistanceX < -2000)
							iCutDistanceX = 0;	
						sprintf(cDispCutDistanceBuf, "X:%9d", iCutDistanceX);
						lcd_display(1, cDispCutDistanceBuf);
				}
				break;

			case Key_F4:	//��ֵ -10
				if(WorkMode == 0)
				{
					iCutDistanceY -= 10;
					if (iCutDistanceY < -2000)
						iCutDistanceY = 0;	
					sprintf(cDispCutDistanceBuf, "Y:%9d", iCutDistanceY);
					lcd_display(1, cDispCutDistanceBuf);
				}
				else{
						iCutDistanceX-= 10;
						if (iCutDistanceX < -2000)
							iCutDistanceX = 0;	
						sprintf(cDispCutDistanceBuf, "X:%9d", iCutDistanceX);
						lcd_display(1, cDispCutDistanceBuf);
				}
				break;

			case Key_F6:	//��ֵ +10
				if(WorkMode == 0)
				{
					iCutDistanceY += 10;
					if (iCutDistanceY > 2000)
						iCutDistanceY = 0;	
					sprintf(cDispCutDistanceBuf, "Y:%9d", iCutDistanceY);
					lcd_display(1, cDispCutDistanceBuf);
				}
				else{
						iCutDistanceX += 10;
						if (iCutDistanceX > 2000)
							iCutDistanceX = 0;	
						sprintf(cDispCutDistanceBuf, "X:%9d", iCutDistanceX);
						lcd_display(1, cDispCutDistanceBuf);
				}
				break;

			case Key_Ok:	//�����˳�
				gl_iCutDistanceY  = iCutDistanceY;		
				gl_iCutDistanceX = iCutDistanceX;	
				Printf("Save Y: %d X: %d\n",gl_iCutDistanceY, gl_iCutDistanceX);
				if(!WriteParameter()) {
					if (gl_cLanguageSelect == 0) {
						LcdDisp("д�����ʧ��", "������");//д�����ʧ��, ������
					}
					else {
						LcdDisp("Data Write Fail", "Retry");
					}
				}
				state = 0;
				break;

			case Key_Pause:
				state = 0;
				break;
		}
	}
	//if (gl_lAxisXPos != 0)
	//	DriveSprinkler(-gl_lAxisXPos,0);
	//gl_iPulseNumX = iTempXPlus;

}
//*********************************************************************

//*******************�ʵ�����*******************************************
//���ñʺ����л��ǻ���
void CM_PenCutAttribute(void)
{
	unsigned char cReadKey;
	unsigned char cEnd;
	char cParaBuf[][5] = {{"PEN "},{"CUT "}, {"CUT*"}};// 20131208 ��Ҫ������
	int cPenAttrBuf[16];
	char cDispBuf[16];
	int iCurrPenNo;
	
	iCurrPenNo = gl_cCutPenNum; 
	ArrCpy(cPenAttrBuf, gl_cSetPenAttr, 10);
	Printf("iCurNo[%d], cPA[%d][%s]", iCurrPenNo, cPenAttrBuf[iCurrPenNo], cParaBuf[cPenAttrBuf[iCurrPenNo]]);
	if(gl_cLanguageSelect == 0)
		strcpy(gl_cLcdDispBuf[0], "�ʺ���������:");
	else
		strcpy(gl_cLcdDispBuf[0], "Cut Attribute:");
	sprintf(cDispBuf, "%2d#    %s", iCurrPenNo, cParaBuf[cPenAttrBuf[iCurrPenNo]]);
	strcpy(gl_cLcdDispBuf[1], cDispBuf);
	lcd_refresh();

	cEnd = 1;
	while (cEnd)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		switch (cReadKey)
		{
			case Key_F1:	//����ʵ��������
				CM_PenCutSpacing();
				cEnd = 0;
				break;

			case Key_F2:	//�޸ıʺ� +
				iCurrPenNo++;
				if(iCurrPenNo > 8)
					iCurrPenNo = 1;
				sprintf(cDispBuf, "%2d#    %s", iCurrPenNo, cParaBuf[cPenAttrBuf[iCurrPenNo]]);
				lcd_display(1, cDispBuf);
				break;

			case Key_F8:	//�޸ıʺ� -
				iCurrPenNo--;
				if(iCurrPenNo < 1)
					iCurrPenNo = 8;
				sprintf(cDispBuf, "%2d#    %s", iCurrPenNo, cParaBuf[cPenAttrBuf[iCurrPenNo]]);
				lcd_display(1, cDispBuf);
				break;

				
		/*
		ʱ��2015-12-24
		���ΰ�������ת���ᵽ
		*/
			
		case Key_F4:	//�޸ı����� ��� / �и� / ת���ᵶ
				cPenAttrBuf[iCurrPenNo]--;
				if(cPenAttrBuf[iCurrPenNo] < 0)
					cPenAttrBuf[iCurrPenNo] = 1;//cPenAttrBuf[iCurrPenNo] = 2;//ѭ������2-1-0-2-1-0
				sprintf(cDispBuf, "%2d#    %s", iCurrPenNo, cParaBuf[cPenAttrBuf[iCurrPenNo]]);
				lcd_display(1, cDispBuf);
				break;

			case Key_F6:	//�޸ı����� ��� / �и� / ת���ᵶ
				cPenAttrBuf[iCurrPenNo]++;
				if(cPenAttrBuf[iCurrPenNo] > 1)//if(cPenAttrBuf[iCurrPenNo] > 1)////ѭ������1-2-0-1-2-0
					cPenAttrBuf[iCurrPenNo] = 0;
				sprintf(cDispBuf, "%2d#    %s", iCurrPenNo, cParaBuf[cPenAttrBuf[iCurrPenNo]]);
				lcd_display(1, cDispBuf);
				break;
			
			case Key_Ok:
				ArrCpy(gl_cSetPenAttr, cPenAttrBuf, 10);
				WriteParameter();
				cEnd = 0;
				break;

			case Key_Pause:
				cEnd = 0;
				break;
		}

	}

}
//**********************************************************************

//&&&&&&&&&&&&&&&&&&&&&�˵�����&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*********�ᵶ����ֵ ת���ᵶ����ֵ
void CM_LiftCutExtendSet(void)
{
	unsigned char cReadKey;//��ֵ
	char  state;
	int dLiftCutValueTemp, iRota, iExten;
	char  aLiftCutDispBuff[17];
	char SetMenuState;
	SetMenuState = 0;
	dLiftCutValueTemp = gl_dCutExtensValue;
	iRota = gl_dCutRotaExtenValue;
	iExten = gl_dCutExtensValue;
	sprintf(aLiftCutDispBuff, "%d", dLiftCutValueTemp);
	
	if(gl_cLanguageSelect == 0)
		LcdDisp( "�ᵶ����ֵ",  aLiftCutDispBuff);	
	else
		LcdDisp( "Cut Extension:",  aLiftCutDispBuff);	
	
	Printf("ReadValue Rota[%d], Exter[%d]\n", gl_dCutRotaExtenValue, gl_dCutExtensValue);
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
				SetMenuState = (SetMenuState == 0) ? 1 : 0;
				if (SetMenuState == 0)
				{//�ᵶ����ֵ
					iRota = dLiftCutValueTemp;
					dLiftCutValueTemp = iExten;
					sprintf(aLiftCutDispBuff, "%d", dLiftCutValueTemp);
					if(gl_cLanguageSelect == 0)
						LcdDisp( "�ᵶ����ֵ",  aLiftCutDispBuff);	
					else
						LcdDisp( "Cut Extension:",  aLiftCutDispBuff);	
				}
				else{//ת���ᵶ����ֵ
						iExten = dLiftCutValueTemp;
						dLiftCutValueTemp = iRota;
						sprintf(aLiftCutDispBuff, "%d", dLiftCutValueTemp);
						if(gl_cLanguageSelect == 0)
							LcdDisp( "ת���ᵶ����ֵ",  aLiftCutDispBuff);
						else
							LcdDisp( "Cut *Extension:",  aLiftCutDispBuff);
					}
				break;
				
			case Key_F2:
				dLiftCutValueTemp += 1;
				if (dLiftCutValueTemp > 120.0)
					dLiftCutValueTemp = 0;
				sprintf(aLiftCutDispBuff, "%d", dLiftCutValueTemp);
				lcd_display(1, aLiftCutDispBuff);
				break;
				
			case Key_F8:
				dLiftCutValueTemp -= 1;
				if (dLiftCutValueTemp < 0)
					dLiftCutValueTemp = 120;
				sprintf(aLiftCutDispBuff, "%d", dLiftCutValueTemp);
				lcd_display(1, aLiftCutDispBuff);
				break;

			case Key_F6:
				dLiftCutValueTemp += 10;
				if (dLiftCutValueTemp > 120.0)
					dLiftCutValueTemp = 0;
				sprintf(aLiftCutDispBuff, "%d", dLiftCutValueTemp);
				lcd_display(1, aLiftCutDispBuff);
				break;
				
			case Key_F4:
				dLiftCutValueTemp -= 10;
				if (dLiftCutValueTemp < 0)
					dLiftCutValueTemp = 120;
				sprintf(aLiftCutDispBuff, "%d", dLiftCutValueTemp);
				lcd_display(1, aLiftCutDispBuff);
				break;
	
			case Key_Ok:
				if (SetMenuState == 0)
					iExten= dLiftCutValueTemp;
				else
					iRota = dLiftCutValueTemp;
				
				gl_dCutRotaExtenValue = iRota;
				gl_dCutExtensValue = iExten;

				Printf("SetValue Rota[%d], Exter[%d]\n", gl_dCutRotaExtenValue, gl_dCutExtensValue);
				WriteParameter();
				state = 0;	
				break;
			case Key_Pause:
				state = 0;
				break;
		}
	}

}

//ƫ������
void CM_OffsetDistSeting(void)
{
	char cDispTemp[10];
	unsigned char cReadKey;
	char  state;
	int  iCutOffset;

	sprintf(cDispTemp, "%d", gl_iCutOffset);
	if(gl_cLanguageSelect == 0)
		LcdDisp("ƫ������:", cDispTemp);
	else
		LcdDisp("Offset:", cDispTemp);
	iCutOffset = gl_iCutOffset;
		
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
			case Key_F2:
				iCutOffset += 1;
				if(iCutOffset > 120)
					iCutOffset = 10;
				sprintf(cDispTemp, "%d", iCutOffset);
				lcd_display(1, cDispTemp);
				break;
				
			case Key_F8:
				iCutOffset -= 1;
				if(iCutOffset < 10)
					iCutOffset = 120;
				sprintf(cDispTemp, "%d", iCutOffset);
				lcd_display(1, cDispTemp);		
				break;
				
			case Key_F6:
				iCutOffset += 10;
				if(iCutOffset > 120)
					iCutOffset = 10;
				sprintf(cDispTemp, "%d", iCutOffset);
				lcd_display(1, cDispTemp);
				break;

			case Key_F4:
				iCutOffset -= 10;
				if(iCutOffset < 10)
					iCutOffset = 120;
				sprintf(cDispTemp, "%d", iCutOffset);
				lcd_display(1, cDispTemp);	
				break;
/*2015-12-24*/
			case Key_F1://ת���ᵶ
				CM_LiftCutExtendSet();
				state = 0;
				break;
		
			case Key_Ok:
				gl_iCutOffset = iCutOffset;
				Printf("Save gl_iCutOffset: %d\n", gl_iCutOffset);
				if(!WriteParameter()) {
					if (gl_cLanguageSelect == 0) {
						LcdDisp("д�����ʧ��", "������");//д�����ʧ��, ������
					}
					else {
						LcdDisp("Data Write Fail", "Retry");
					}

				}
					//LcdDisp(language[11], language[12]);//д�����ʧ��, ������
				state = 0;
				break;
			case Key_Pause:
				state = 0;
				break;
		}
	}

}
//*******************************************************************************

//&&&&&&&&&&&&&&&&&&&&&&&&&&&�˵�����&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

//******************��ҳƴ��У��
void CM_PaperAdjust(void)
{
	unsigned char cReadKey;//��ֵ
	char  state;
	char cBuff[17];
	char cMenu;
	int iValue;
	int  iXProp;
		
	iXProp = (int)(gl_dPaperAdjust * 100);
	cMenu = 0;
	iValue = iXProp;
	if(gl_cLanguageSelect==0)
		strcpy(gl_cLcdDispBuf[0], "��ҳ��϶У��");
	else
		strcpy(gl_cLcdDispBuf[0], "Page gap:");
	sprintf(gl_cLcdDispBuf[1], "%1.2fmm", (double)iXProp/100.0);
	gl_cLcdDispBuf[1][1]='.';
	lcd_refresh();
		
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
				// XY����У��
				CM_XYLengthCheck();
				state = 0;
				break;

					
			case Key_F2:
				iValue++;
				if(iValue > 200)
					iValue = 0;
				sprintf(cBuff, "%1.2fmm", (double)iValue/100.0);
				//sprintf(cBuff, "%d", iValue);
				cBuff[1]='.';
				lcd_display(1, cBuff);
				break;
				
			case Key_F8:
				iValue--;
				if(iValue < 200)
					iValue = 0;
				sprintf(cBuff, "%1.2fmm", (double)iValue/100.0);
				//sprintf(cBuff, "%d", iValue);
				cBuff[1]='.';
				lcd_display(1, cBuff);
				break;
				
			case Key_F4:
				iValue-=10;
				if(iValue < 200)
					iValue = 0;
				sprintf(cBuff, "%1.2fmm", (double)iValue/100.0);
				//sprintf(cBuff, "%d", iValue);
				cBuff[1]='.';
				lcd_display(1, cBuff);
				break;

			case Key_F6:	
				iValue+=10;
				if(iValue > 200)
					iValue = 0;
				sprintf(cBuff, "%1.2fmm", (double)iValue/100.0);
				//sprintf(cBuff, "%d", iValue);
				cBuff[1]='.';
				lcd_display(1, cBuff);
				break;
			
			case Key_Ok:
				iXProp = iValue;
				gl_dPaperAdjust = (double)iXProp/100.0;
				Printf("gl_dPaperAdjust [%f], IPropX[%d]\n", gl_dPaperAdjust, iXProp);
				WriteParameter();
				state = 0;
				break;
			case Key_Pause:
				state = 0;
				break;
			
		}
	}

}

//******************��ֽ���У��
//��λ:����
void CM_MovePaperAdjust(void)
{
	unsigned char cReadKey;//��ֵ
	char  state;
	char cBuff[17];
	char cMenu;
	int iValue;
	int  iXProp, iYProp;
		
	iXProp = (int)(gl_dMovePaperError_X * 100);
	iYProp = (int)(gl_dMovePaperError_Y * 100);
	cMenu = 0;
	iValue = iXProp;
	if(gl_cLanguageSelect==0)
		strcpy(gl_cLcdDispBuf[0], "��ֽ���У�� X");
	else
		strcpy(gl_cLcdDispBuf[0], "Move Error X:");
	sprintf(gl_cLcdDispBuf[1], "%1.2fmm", (double)iXProp/100.0);
	gl_cLcdDispBuf[1][1]='.';
	lcd_refresh();
		
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
				// XY����У��
				CM_PaperAdjust();
				state = 0;
				break;

			case Key_F3://�л�XY����
				cMenu = (cMenu == 0) ? 1 : 0;
				if(cMenu == 0)
				{
					iYProp = iValue;
					iValue = iXProp;
					if(gl_cLanguageSelect==0)
						strcpy(gl_cLcdDispBuf[0], "��ֽ���У�� X");
					else
						strcpy(gl_cLcdDispBuf[0], "Move Error X:");
					sprintf(gl_cLcdDispBuf[1], "%1.2fmm", (double)iXProp/100.0);
					gl_cLcdDispBuf[1][1]='.';
					lcd_refresh();
				}
				else
				{
					iXProp = iValue;
					iValue = iYProp;
					if(gl_cLanguageSelect==0)
						strcpy(gl_cLcdDispBuf[0], "��ֽ���У�� Y");
					else
						strcpy(gl_cLcdDispBuf[0], "Move Error Y:");
					sprintf(gl_cLcdDispBuf[1], "%1.2fmm", (double)iYProp/100.0);
					gl_cLcdDispBuf[1][1]='.';
					lcd_refresh();
				}
				break;
			
			case Key_F2:
				iValue++;
				if(iValue > 200)
					iValue = 0;
				sprintf(cBuff, "%1.2fmm", (double)iValue/100.0);
				//sprintf(cBuff, "%d", iValue);
				cBuff[1]='.';
				lcd_display(1, cBuff);
				break;
				
			case Key_F8:
				iValue--;
				if(iValue < 0)
					iValue = 200;
				sprintf(cBuff, "%1.2fmm", (double)iValue/100.0);
				//sprintf(cBuff, "%d", iValue);
				cBuff[1]='.';
				lcd_display(1, cBuff);
				break;
				
			case Key_F4:
				iValue-=10;
				if(iValue < 0)
					iValue = 200;
				sprintf(cBuff, "%1.2fmm", (double)iValue/100.0);
				//sprintf(cBuff, "%d", iValue);
				cBuff[1]='.';
				lcd_display(1, cBuff);
				break;

			case Key_F6:	
				iValue+=10;
				if(iValue > 200)
					iValue = 0;
				sprintf(cBuff, "%1.2fmm", (double)iValue/100.0);
				//sprintf(cBuff, "%d", iValue);
				cBuff[1]='.';
				lcd_display(1, cBuff);
				break;
			
			case Key_Ok:
				if(cMenu == 0)
					iXProp = iValue;
				else
					iYProp = iValue;
				gl_dMovePaperError_X = (double)iXProp/100.0;
				gl_dMovePaperError_Y = (double)iYProp/100.0;
				Printf("gl_dMovePaperError_X[%f], IPropX[%d]\n", gl_dMovePaperError_X, iXProp);
				Printf("gl_dMovePaperError_Y[%f], IPropY[%d]\n", gl_dMovePaperError_Y, iYProp);
				WriteParameter();
				state = 0;
				break;
			case Key_Pause:
				state = 0;
				break;
			
		}
	}
}

//******************XY����У��
void CM_XYLengthCheck(void)
{
	unsigned char cReadKey;//��ֵ
	char  state;
	char cBuff[17];
	char cMenu;
	int iValue;
	int  iXProp, iYProp;
	
	iXProp = (int)(gl_dXProp * 10000);
	iYProp = (int)(gl_dYProp * 10000);
	cMenu = 0;
	iValue = iXProp;
	if(gl_cLanguageSelect==0)
		strcpy(gl_cLcdDispBuf[0], "X �᳤��У��");
	else
		strcpy(gl_cLcdDispBuf[0], "Calibrate: X");
	sprintf(gl_cLcdDispBuf[1], "%d", iXProp);
	lcd_refresh();
		
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
			case Key_F3://�л�XY����
				cMenu = (cMenu == 0) ? 1 : 0;
				if(cMenu == 0)
				{
					iYProp = iValue;
					iValue = iXProp;
					if(gl_cLanguageSelect==0)
						strcpy(gl_cLcdDispBuf[0], "X �᳤��У��");
					else
						strcpy(gl_cLcdDispBuf[0], "Calibrate: X");
					sprintf(gl_cLcdDispBuf[1], "%d", iXProp);
					lcd_refresh();
				}
				else
				{
					iXProp = iValue;
					iValue = iYProp;
					if(gl_cLanguageSelect==0)
						strcpy(gl_cLcdDispBuf[0], "Y �᳤��У��");
					else
						strcpy(gl_cLcdDispBuf[0], "Calibrate: Y");
					sprintf(gl_cLcdDispBuf[1], "%d", iYProp);
					lcd_refresh();
				}
				break;
			
			case Key_F2:
				iValue++;
				if(iValue > 20000)
					iValue = 5000;
				sprintf(cBuff, "%d", iValue);
				lcd_display(1, cBuff);
				break;
				
			case Key_F8:
				iValue--;
				if(iValue < 5000)
					iValue = 20000;
				sprintf(cBuff, "%d", iValue);
				lcd_display(1, cBuff);
				break;
				
			case Key_F4:
				iValue-=10;
				if(iValue < 5000)
					iValue = 20000;
				sprintf(cBuff, "%d", iValue);
				lcd_display(1, cBuff);
				break;

			case Key_F6:	
				iValue+=10;
				if(iValue > 20000)
					iValue = 5000;
				sprintf(cBuff, "%d", iValue);
				lcd_display(1, cBuff);
				break;
			
			case Key_Ok:
				if(cMenu == 0)
					iXProp = iValue;
				else
					iYProp = iValue;
				gl_dXProp = (double)iXProp/10000;
				gl_dYProp = (double)iYProp/10000;
				gl_dPosUnitX = HPGL_MM * X_PPMM * gl_dXProp;
				gl_dPosUnitY = HPGL_MM * Y_PPMM * gl_dYProp;
				Printf("gl_dXProp[%f], gl_dYProp[%f]\n", gl_dXProp, gl_dYProp);
				WriteParameter();
				state = 0;
				break;
			case Key_Pause:
				state = 0;
				break;
		}
	}

}
//*******************************************************************************

/**/


//******************XY�ٶȱ��ʵ���
void CM_XY_SpeedRatio(void)
{
	unsigned char cReadKey;//��ֵ
	char  state;
	char cBuff[17];
	char cMenu;
	int iValue;
	int  i_gl_iSVSpeed_Y,i_gl_iVSpeed_Y,i_gl_iTimeAdd_Y,i_gl_iSVSpeed_X,i_gl_iVSpeed_X,i_gl_iTimeAdd_X;
	
	i_gl_iSVSpeed_Y = (int)(gl_iSVSpeed);
	i_gl_iVSpeed_Y 	= (int)(gl_iVSpeed);
	i_gl_iTimeAdd_Y	= (int)(gl_iTimeAdd);
	
	i_gl_iSVSpeed_X	= (int)(gl_iSVSpeedX);
	i_gl_iVSpeed_X	= (int)(gl_iVSpeedX);
	i_gl_iTimeAdd_X	= (int)(gl_iTimeAddX);
	
	cMenu = 0;
	iValue = i_gl_iVSpeed_X;
	if(gl_cLanguageSelect==0)
		strcpy(gl_cLcdDispBuf[0], "X ���ٶ�����");
	else
		strcpy(gl_cLcdDispBuf[0], "Speed curve: X");
	sprintf(gl_cLcdDispBuf[1], "%d", gl_iVSpeedX);
	lcd_refresh();
		
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
			case Key_F3://�л�XY����
				cMenu = (cMenu == 0) ? 1 : 0;
				if(cMenu == 0)
				{
					i_gl_iVSpeed_Y = iValue;
					iValue = i_gl_iVSpeed_X;
					if(gl_cLanguageSelect==0)
						strcpy(gl_cLcdDispBuf[0], "X �᳤��У��");
					else
						strcpy(gl_cLcdDispBuf[0], "Calibrate: X");
					sprintf(gl_cLcdDispBuf[1], "%d", i_gl_iVSpeed_X);
					lcd_refresh();
				}
				else
				{
					i_gl_iVSpeed_X = iValue;
					iValue = i_gl_iVSpeed_Y;
					if(gl_cLanguageSelect==0)
						strcpy(gl_cLcdDispBuf[0], "Y �᳤��У��");
					else
						strcpy(gl_cLcdDispBuf[0], "Calibrate: Y");
					sprintf(gl_cLcdDispBuf[1], "%d", i_gl_iVSpeed_Y);
					lcd_refresh();
				}
				break;
			
			case Key_F2:
				iValue++;

				sprintf(cBuff, "%d", iValue);
				lcd_display(1, cBuff);
				break;
				
			case Key_F8:
				iValue--;
			
				sprintf(cBuff, "%d", iValue);
				lcd_display(1, cBuff);
				break;
				
			case Key_F4:
				iValue-=10;
			
				sprintf(cBuff, "%d", iValue);
				lcd_display(1, cBuff);
				break;

			case Key_F6:	
				iValue+=10;
			
				sprintf(cBuff, "%d", iValue);
				lcd_display(1, cBuff);
				break;
			
			case Key_Ok:
				if(cMenu == 0)
					i_gl_iVSpeed_X = iValue;
				else
					i_gl_iVSpeed_Y = iValue;
					
				gl_iVSpeed 	= i_gl_iVSpeed_Y;
				gl_iVSpeedX = i_gl_iVSpeed_X;				

				WriteParameter();
				state = 0;
				break;
			case Key_Pause:
				state = 0;
				break;
		}
	}

}
/**/
//&&&&&&&&&&&&&&&&&&&&&&&&&&�˵���&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//******************����IP ���� ��������
void  CMN_MacIpSetManage(unsigned short *sNetWorkValue)
{
	unsigned char i,cTemp;
	unsigned char IPAddr[4];
	unsigned char ch;
	unsigned char cPin, cState;
	unsigned short sBakValue[2];
	
	IPAddr[0] = (unsigned char)(*sNetWorkValue >> 8);
	IPAddr[1] = (unsigned char)(*sNetWorkValue);
	IPAddr[2] = (unsigned char)(*(sNetWorkValue+1) >> 8);
	IPAddr[3] = (unsigned char)(*(sNetWorkValue+1));
	strcpy(gl_cLcdDispBuf[0]," *");
	sprintf(gl_cLcdDispBuf[1],"%3d.%3d.%3d.%3d",IPAddr[0],IPAddr[1],IPAddr[2],IPAddr[3]);
	lcd_refresh();
	cPin = 0;
	cState = 1;
	
	while (cState)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		ch = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (ch)
		{
			case Key_F1:
				if (cPin > 0)
					cPin--;
				cTemp = cPin * 4 + 1;
				for (i=0;i<cTemp;i++)
					gl_cLcdDispBuf[0][i] = ' ';
				gl_cLcdDispBuf[0][i++] = '*';
				gl_cLcdDispBuf[0][i] = '\0';
				lcd_refresh();
				break;
				
			case Key_F3:
				if (cPin < 3)
					cPin++;
				cTemp = cPin * 4 + 1;
				for (i=0;i<cTemp;i++)
					gl_cLcdDispBuf[0][i] = ' ';
				gl_cLcdDispBuf[0][i++] = '*';
				gl_cLcdDispBuf[0][i] = '\0';
				lcd_refresh();
				break;
				
			case Key_F6:
				if (IPAddr[cPin] < 0xff)
						IPAddr[cPin]+=10;
					else
						IPAddr[cPin] = 0;
					sprintf(gl_cLcdDispBuf[1],"%3d.%3d.%3d.%3d",IPAddr[0],IPAddr[1],IPAddr[2],IPAddr[3]);
					
					lcd_refresh();
				break;

			case Key_F4:
				if (IPAddr[cPin] > 0)
						IPAddr[cPin]-=10;
					else
						IPAddr[cPin] = 0xff;
					sprintf(gl_cLcdDispBuf[1],"%3d.%3d.%3d.%3d",IPAddr[0],IPAddr[1],IPAddr[2],IPAddr[3]);
					
					lcd_refresh();
				break;
				
			case Key_F2:
				if (IPAddr[cPin] < 0xff)
					IPAddr[cPin]++;
				else
					IPAddr[cPin] = 0;
				sprintf(gl_cLcdDispBuf[1],"%3d.%3d.%3d.%3d",IPAddr[0],IPAddr[1],IPAddr[2],IPAddr[3]);
					
				lcd_refresh();
				break;
			case Key_F8:
				if (IPAddr[cPin] > 0)
					IPAddr[cPin]--;
				else
					IPAddr[cPin] = 0xff;
				sprintf(gl_cLcdDispBuf[1],"%3d.%3d.%3d.%3d",IPAddr[0],IPAddr[1],IPAddr[2],IPAddr[3]);
					
				lcd_refresh();
				break;
				
			case Key_Ok:
				if (gl_cFileReceive == 0)
				{
					sBakValue[0] = *sNetWorkValue;
					sBakValue[1] = *(sNetWorkValue+1);
					*sNetWorkValue = ((unsigned short)IPAddr[0] << 8) + (unsigned short)IPAddr[1];
					*(sNetWorkValue+1) = ((unsigned short)IPAddr[2] << 8) + (unsigned short)IPAddr[3];
					if (WriteParameter() == 1)
						return;
					else
					{
						lcd_display(0,"��������ʧ��!");
						lcd_display(1," ");
						WaitKeyBound(Key_Ok);
						*sNetWorkValue = sBakValue[0];
						*(sNetWorkValue+1) = sBakValue[1];
					}
				}
				cState = 0;
				break;
				
			case Key_Pause:
				cState = 0;
				break;
				
			default:
				break;
		}
	}
	
}

//�������
void CM_NetManage(void)
{
	unsigned char ch;
	unsigned char cState;

	if(gl_cLanguageSelect == 0)
		strcpy(gl_cLcdDispBuf[0],"�������");
	else
		strcpy(gl_cLcdDispBuf[0],"Network manage");
	strcpy(gl_cLcdDispBuf[1],"            ");
	lcd_refresh();

	cState = 1;
	while (cState)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		ch = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (ch)
		{
			case Key_F1:
				CMN_MacIpSetManage(MyIP);		// IP
				
				break;

			case Key_F2:
				CMN_MacIpSetManage(SubnetMask);	//��������
				
				break;

			case Key_F3:
				CMN_MacIpSetManage(GatewayIP);	//����
				
				break;
			
			case Key_Ok:
			case Key_Pause:
				cState = 0;
				break;
		}
		
		if(gl_cLanguageSelect == 0)
			strcpy(gl_cLcdDispBuf[0],"�������");
		else
			strcpy(gl_cLcdDispBuf[0],"Network manage");
		strcpy(gl_cLcdDispBuf[1],"            ");
		lcd_refresh();
	}
	

}
//******************************************************************************

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&�˵���&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//�и��϶
void CM_CutGap(void)					
{
	unsigned char ch;
	unsigned char cState;
	char cDisp[17];
	int iCutSectionGap;
	
	iCutSectionGap = gl_iCutSectionGap;
	if(gl_cLanguageSelect == 0)
		strcpy(gl_cLcdDispBuf[0],"�и�ֶμ�϶:");
	else
		strcpy(gl_cLcdDispBuf[0],"Cut section gap:");
	sprintf(gl_cLcdDispBuf[1], "%d Hpgl", iCutSectionGap);
	lcd_refresh();

	cState = 1;
	while (cState)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		ch = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (ch)
		{
			case Key_F2:
				iCutSectionGap++;
				if(iCutSectionGap > 100)
					iCutSectionGap = 0;
				sprintf(cDisp,"%d Hpgl",iCutSectionGap);
				lcd_display(1,cDisp);
				break;

			case Key_F8:
				iCutSectionGap--;
				if(iCutSectionGap < 0)
					iCutSectionGap = 100;
				sprintf(cDisp,"%d Hpgl",iCutSectionGap);
				lcd_display(1,cDisp);
				break;

			case Key_F4:
				iCutSectionGap-=10;
				if(iCutSectionGap > 100)
					iCutSectionGap = 0;
				sprintf(cDisp,"%d Hpgl",iCutSectionGap);
				lcd_display(1,cDisp);
				break;

			case Key_F6:
				iCutSectionGap+=10;
				if(iCutSectionGap > 100)
					iCutSectionGap = 0;
				sprintf(cDisp,"%d Hpgl",iCutSectionGap);
				lcd_display(1,cDisp);
				break;

			case Key_Ok:
				gl_iCutSectionGap = iCutSectionGap;
				WriteParameter();
				cState = 0;
				break;

			case Key_Pause:

				cState = 0;
				break;
				
		}
	}
}

//�и�ֶγ���
void CM_CutSectionLength(void)
{
	unsigned char ch;
	unsigned char cState;
	char cDisp[17];
	int iCutSectionLen;

	iCutSectionLen = gl_iCutSectionLen;
	if(gl_cLanguageSelect == 0)
		strcpy(gl_cLcdDispBuf[0],"�и�ֶγ���");
	else
		strcpy(gl_cLcdDispBuf[0],"Cut Section LEN");
	sprintf(gl_cLcdDispBuf[1], "%dmm", iCutSectionLen);
	lcd_refresh();

	cState = 1;
	while (cState)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		ch = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (ch)
		{
			case Key_F1:
				CM_CutGap();
				cState = 0;
				break;

			case Key_F2:
				iCutSectionLen++;
				if(iCutSectionLen > 100)
					iCutSectionLen = 0;
				sprintf(cDisp,"%dmm",iCutSectionLen);
				lcd_display(1, cDisp);
				break;

			case Key_F8:
				iCutSectionLen--;
				if(iCutSectionLen < 0)
					iCutSectionLen = 100;
				sprintf(cDisp,"%dmm",iCutSectionLen);
				lcd_display(1, cDisp);
				break;

			case Key_F4:
				iCutSectionLen-=10;
				if(iCutSectionLen < 0)
					iCutSectionLen = 100;
				sprintf(cDisp,"%dmm",iCutSectionLen);
				lcd_display(1, cDisp);
				break;

			case Key_F6:
				iCutSectionLen+=10;
				if(iCutSectionLen > 100)
					iCutSectionLen = 0;
				sprintf(cDisp,"%dmm",iCutSectionLen);
				lcd_display(1, cDisp);
				break;

			case Key_Ok:
				gl_iCutSectionLen = iCutSectionLen;
				WriteParameter();
				cState = 0;
				break;

			case Key_Pause:
				cState = 0;
				break;
				
		}
	}
	
}
//******************************************************************************

//************************�˵� �Զ���ֽ����*************************************
void CM_AutoSendPaper(void)
{
	char cAutoSendPaperSet, ch, cState;

	cAutoSendPaperSet = gl_cAutoSendPaperSet;

	if (gl_cLanguageSelect == 0) {
		lcd_display(0, "�Զ���ֽ����");
	}
	else {
		lcd_display(0, "AutoSendPaperSet");
	}
	//lcd_display(0, "�Զ���ֽ����");		//20141201 rwt
	if(cAutoSendPaperSet == 1) {
		if (gl_cLanguageSelect == 0) {
			lcd_display(1, "����");
		}
		else {
			lcd_display(1, "Open");
		}
	}
	else {
		if (gl_cLanguageSelect == 0) {
			lcd_display(1, "�ر�");
		}
		else {
			lcd_display(1,"Close");
		}
	}

	cState = 1;
	while (cState)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		ch = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (ch)
		{
			case Key_F1:
				cAutoSendPaperSet = cAutoSendPaperSet == 0 ? 1 : 0;
				if(cAutoSendPaperSet == 1) {
					if (gl_cLanguageSelect == 0) {
						lcd_display(1, "����");
					}
					else {
						lcd_display(1, "Open");
					}
				}
					//lcd_display(1, "����");
				else {
					if (gl_cLanguageSelect == 0) {
						lcd_display(1, "�ر�");
					}
					else {
						lcd_display(1, "Close");
					}

				}
					//lcd_display(1, "�ر�");
				break;
			case Key_Ok:
				gl_cAutoSendPaperSet = cAutoSendPaperSet;
				cState = 0;
				WriteParameter();
				break;
			case Key_Pause:
				cState = 0;
				break;
		}
	}
	//Printf("Auto Send paper end!\n");
}
//*****************************************************************************

//&&&&&&&&&&&&&&&&&&&&&&&&�˵���&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*****�汾��ѯ
void CM_VerionsQuery(void)				
{
	unsigned char cReadKey;//��ֵ
	char  state;

	if(gl_cLanguageSelect == 0)
	{
		strcpy(gl_cLcdDispBuf[0],"�汾��:V1.04");
		strcpy(gl_cLcdDispBuf[1],"����:15/08/03");
	}
	else
	{
		strcpy(gl_cLcdDispBuf[0],"Ver:V1.04");
		strcpy(gl_cLcdDispBuf[1],"Date:15/08/03");
	}
	lcd_refresh();

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
			case Key_Ok:
			case Key_Pause:
				state = 0;
				break;
		}
	}

}
//*****************************************************************************


/*****************************************************************************/
/******************************�߼��˵�**********************************/
/****************************************************************************/
void AM_MacType(void)					//��������
{
	unsigned int i;
	unsigned char ch;
		
	LineReverse(0x02);
	if (gl_sPlotType == 1620)
		i = 1;
	else if (gl_sPlotType == 2030)
		i = 2;
	else if (gl_sPlotType == 2230)
		i = 3;
	else if (gl_sPlotType == 2430)
		i = 4;
	else if (gl_sPlotType == 1320)
		i = 5;
	else if (gl_sPlotType == 2830)// 20100813 ����2.83�׻���
		i = 6;
	else if (gl_sPlotType == 1830)// 20150527 ����1.83�׻���
		i = 7;
	else
		i = 0;
	
	if(gl_cLanguageSelect == 0)
	{
		if (i == 1)
			strcpy(gl_cLcdDispBuf[0],"��ǰ:RP_MJ160 ");
		else if (i == 2)
			strcpy(gl_cLcdDispBuf[0],"��ǰ:RP_MJ200 ");
		else if (i == 3)
			strcpy(gl_cLcdDispBuf[0],"��ǰ:RP_MJ220 ");
		else if (i == 4)
			strcpy(gl_cLcdDispBuf[0],"��ǰ:RP_MJ240 ");
		else if (i == 5)
			strcpy(gl_cLcdDispBuf[0],"��ǰ:RP_MJ130 ");
		else if (i == 6)
			strcpy(gl_cLcdDispBuf[0],"��ǰ:RP_MJ283");	// 20100813 ����2.83�׻���
		else if (i == 7)
			strcpy(gl_cLcdDispBuf[0],"��ǰ:RP_MJ180");	// 20150527����1.83�׻���
		else
			strcpy(gl_cLcdDispBuf[0],"��ǰ:RP_MJ260 ");
	}
	else
	{
		if (i == 1)
			strcpy(gl_cLcdDispBuf[0],"Type:SP160");
		else if (i == 2)
			strcpy(gl_cLcdDispBuf[0],"Type:SP200 ");
		else if (i == 3)
			strcpy(gl_cLcdDispBuf[0],"Type:SP220 ");
		else if (i == 4)
			strcpy(gl_cLcdDispBuf[0],"Type:SP240 ");
		else if (i == 5)
			strcpy(gl_cLcdDispBuf[0],"Type:SP130 ");
		else if (i == 6)
			strcpy(gl_cLcdDispBuf[0],"Type:SP283"); // 20100813 ����2.83�׻���
		else if (i == 7)
			strcpy(gl_cLcdDispBuf[0],"Type:SP180"); // 20100926 ����3.03�׻���
		else
			strcpy(gl_cLcdDispBuf[0],"Type:SP260 ");
	}
		
	while (1)
	{
		if(gl_cLanguageSelect == 0)
		{
			if (i == 7)
				strcpy(gl_cLcdDispBuf[1],"ѡ��:RP_MJ180 "); 	// 20150527 ����1.83�׻���
			else if (i == 6)
				strcpy(gl_cLcdDispBuf[1],"ѡ��:RP_MJ283 "); // 20100813 ����2.83�׻���
			else if (i == 5)
				strcpy(gl_cLcdDispBuf[1],"ѡ��:RP_MJ130 ");
			else if (i == 4)
				strcpy(gl_cLcdDispBuf[1],"ѡ��:RP_MJ240 ");
			else if (i == 3)
				strcpy(gl_cLcdDispBuf[1],"ѡ��:RP_MJ220 ");
			else if (i == 2)
				strcpy(gl_cLcdDispBuf[1],"ѡ��:RP_MJ200 ");
			else if (i == 1)
				strcpy(gl_cLcdDispBuf[1],"ѡ��:RP_MJ160 ");
			else
				strcpy(gl_cLcdDispBuf[1],"ѡ��:RP_MJ260 ");
		}
		else
		{
			if (i == 7)
				strcpy(gl_cLcdDispBuf[1],"Select:SP180 ");	   // 20150527 ����1.83�׻���
			else if (i == 6)
				strcpy(gl_cLcdDispBuf[1],"Select:SP283 ");	// 20100813 ����2.83�׻���
			else if (i == 5)
				strcpy(gl_cLcdDispBuf[1],"Select:SP130 ");
			else if (i == 4)
				strcpy(gl_cLcdDispBuf[1],"Select:SP240 ");
			else if (i == 3)
				strcpy(gl_cLcdDispBuf[1],"Select:SP220 ");
			else if (i == 2)
				strcpy(gl_cLcdDispBuf[1],"Select:SP200 ");
			else if (i == 1)
				strcpy(gl_cLcdDispBuf[1],"Select:SP160 ");
			else
				strcpy(gl_cLcdDispBuf[1],"Select:SP260 ");
		}
			
		lcd_refresh();
		WaitKeyExit(0xff);		//�ȴ��м�����
		ch = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
			
		if (ch == Key_F2)
		{
			if (i == 0)
				i = 7;
			else
				i--;
		}
		else if (ch == Key_F8)
		{
			if (i == 7)
				i = 0;
			else
				i++;
		}
		else if (ch == Key_Ok)
		{
			if (gl_cFileReceive == 0)
			{
				if ( i == 7 )
					gl_sPlotType = 1830;	 // 20150527 ����1.83�׻���
				else if (i == 6)
					gl_sPlotType = 2830;	// 20100813 ����2.83�׻���			
				else if (i == 5)
					gl_sPlotType = 1320;
				else if (i == 4)
					gl_sPlotType = 2430;
				else if (i == 3)
					gl_sPlotType = 2230;
				else if (i == 2)
					gl_sPlotType = 2030;
				else if (i == 1)
					gl_sPlotType = 1620;
				else
					gl_sPlotType = 2630;		// 1630 �� 2630
				//hzx--

				WaitPrintEnd();//�ȴ���ӡ����
				gl_iMaxYMM = (gl_sPlotType + 3) * Y_PPMM;
				//gl_iGraphWidth = (int)((double)(gl_iMaxYMM - gl_iOriginalPos) / (double)(8 * DotPulseNum));	
				//gl_iMaxYMM += (3 * (MaxMalposedDot + SprinklerSpace) + 2 * AdditionDot + OriginalSideDot + MotorSideDot + 320) * DotPulseNum;
				gl_iMaxYMM += ( (MaxMalposedDot + SprinklerSpace) + 2 * AdditionDot + OriginalSideDot + MotorSideDot) * DotPulseNum;
				WriteParameter();
				break;
			}
		}
		else if (ch == Key_Pause)
		{
			WaitKeyBound(Key_Pause);
			break;
		}
		else
			continue;
	}

}
//************************************************************************************

//************************************************************************************
//��ֽ�������
extern void SetIOBit(unsigned char cBit, unsigned char cEnable);

void AM_PaperCollectTest(void)			
{
	char cEnd, cReadKey, cTemp;
	
	if(gl_cLanguageSelect == 0)
	{
		lcd_display(0,"2 ��ֽ�����ֽ");
		lcd_display(1,"8 ��ֽ�����ֽ");
	}
	else
	{
		lcd_display(0,"2 RollMotorRoll");
		lcd_display(0,"8 RollMotorSend");

	}
	
	cEnd = 1;
	while (cEnd)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		switch (cReadKey)
		{
			case Key_F2:
				SetIOBit(O_ROLLMOTORDIR,R_RECEIVEPAPER);	//��ֽ�������Ϊ��ֽ
				Delay(50);
				SetIOBit(O_STARTROLLPMOTOR,0);			//������ֽ���
				while (1)
				{
					if (GetIOBit(I_ROLLPEND) == StatusOk)
						break;
					if (gl_cKeyPH != gl_cKeyPE)
					{
						cTemp = gl_cKeyBuf[gl_cKeyPE];
						if (cTemp == 0)
						{
							if ((gl_cKeyPE +1) == KeyBufLength)
								gl_cKeyPE = 0;
							else
								gl_cKeyPE++;
						}
						else
							break;
					}
				}
				SetIOBit(O_STARTROLLPMOTOR,1);			//�ر���ֽ���
				break;
				
			case Key_F8:
				SetIOBit(O_ROLLMOTORDIR,R_BACKPAPER);			//��ֽ�������Ϊ��ֽ
				Delay(50);
				SetIOBit(O_STARTROLLPMOTOR,0);			//������ֽ���
				while (1)
				{
					if (GetIOBit(I_ROLLSTART) == StatusOk)
						break;
					if (gl_cKeyPH != gl_cKeyPE)
					{
						cTemp = gl_cKeyBuf[gl_cKeyPE];
						if (cTemp == 0)
						{
							if ((gl_cKeyPE +1) == KeyBufLength)
								gl_cKeyPE = 0;
							else
								gl_cKeyPE++;
						}
						else
							break;
					}
				}
				SetIOBit(O_STARTROLLPMOTOR,1);			//�ر���ֽ���
				
				break;

			case Key_Ok:
				cEnd = 0;
				break;

			case Key_Pause:
				cEnd = 0;
				break;
		}
	}	
	SetIOBit(O_ROLLMOTORDIR,R_RECEIVEPAPER);	//��ֽ�������Ϊ��ֽ
}
//***********************************************************************************

//***********************************************************************************
//��ֽ�������
void AM_PaperFeederTest(void)			
{
	char cEnd, cReadKey, cTemp;
	
	if(gl_cLanguageSelect == 0)
	{
		lcd_display(0,"2 ��ֽ�����ֽ");
		lcd_display(1,"8 ��ֽ�����ֽ");
	}
	else
	{
		lcd_display(0,"2 SendMotorRoll");
		lcd_display(0,"8 SendMotorSend");

	}
	
	cEnd = 1;
	while (cEnd)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		switch (cReadKey)
		{
			case Key_F2:
				SetIOBit(O_GIVEMOTORDIR,G_ROLLPAPER);			//��ֽ�������Ϊ��ֽ
				Delay(50);
				SetIOBit(O_STARTGIVEPMOTOR,0);			//������ֽ���
				while (1)
				{	
					if (GetIOBit(I_GIVEPEND) == StatusOk)
						break;
					if (gl_cKeyPH != gl_cKeyPE)
					{
						cTemp = gl_cKeyBuf[gl_cKeyPE];
						if (cTemp == 0)
						{
							if ((gl_cKeyPE +1) == KeyBufLength)
								gl_cKeyPE = 0;
							else
								gl_cKeyPE++;
						}
						else
							break;
					}
					
				}
				SetIOBit(O_STARTGIVEPMOTOR,1);			//�ر���ֽ���
				break;
				
			case Key_F8:
				SetIOBit(O_GIVEMOTORDIR,G_SENDPAPER);			//��ֽ�������Ϊ��ֽ
				Delay(50);
				SetIOBit(O_STARTGIVEPMOTOR,0);			//������ֽ���
				while (1)
				{
					if (GetIOBit(I_GIVEBACK) == StatusOk)
						break;
					if (gl_cKeyPH != gl_cKeyPE)
					{
						cTemp = gl_cKeyBuf[gl_cKeyPE];
						if (cTemp == 0)
						{
							if ((gl_cKeyPE +1) == KeyBufLength)
								gl_cKeyPE = 0;
							else
								gl_cKeyPE++;
						}
						else
							break;
					}
				}
				SetIOBit(O_STARTGIVEPMOTOR,1);			//�ر���ֽ���
				
				break;

			case Key_Ok:
				cEnd = 0;
				break;

			case Key_Pause:
				cEnd = 0;
				break;
		}
	}	
	
	Delay(50);
	SetIOBit(O_ROLLMOTORDIR,R_RECEIVEPAPER);	//��ֽ�������Ϊ��ֽ
	if (gl_cKeyPE != gl_cKeyPH)
	{
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
	}
}
//***********************************************************************************

//***********************************************************************************
//��ӡͷ��ϴ��ʱ����20141021
void AM_PrintHeadClearDelaySet()
{
	char cEnd, cReadKey;
	char cPHCDTime;

	cPHCDTime = gl_cPHCDTime;
	
	if(gl_cLanguageSelect == 0)
	{
		strcpy(gl_cLcdDispBuf[0],"��ӡͷ��ϴ��ʱ");
		sprintf(gl_cLcdDispBuf[1]," %d    ����",cPHCDTime);
	}
	else
	{
		strcpy(gl_cLcdDispBuf[0],"PrintHead Delay");
		sprintf(gl_cLcdDispBuf[1]," %d min", cPHCDTime);
	}
	lcd_refresh();
	
	cEnd = 1;
	while (cEnd)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (cReadKey)
		{
			case Key_F2:
				cPHCDTime++;
				if(cPHCDTime > 10)
					cPHCDTime = 1;
				break;
				
			case Key_F8:
				cPHCDTime--;
				if(cPHCDTime < 1)
					cPHCDTime = 10;
				break;
				
				break;

			case Key_Ok:
				gl_cPHCDTime = cPHCDTime;
				WriteParameter();
				cEnd = 0;
				break;

			case Key_Pause:
				cEnd = 0;
				break;
		}
		
		if(gl_cLanguageSelect == 0)
		{
			strcpy(gl_cLcdDispBuf[0],"��ӡͷ��ϴ��ʱ");
			sprintf(gl_cLcdDispBuf[1]," %d    ����",cPHCDTime);
		}
		else
		{
			strcpy(gl_cLcdDispBuf[0],"PrintHead Delay");
			sprintf(gl_cLcdDispBuf[1]," %d min", cPHCDTime);
		}
		lcd_refresh();
	}	
}

//***********************************************************************************
//���̲���
void AM_KeyTest(void)					
{
	char cKey;
	char ch;
	
	cKey = '0';
	do
	{
		cKey++;
		if(gl_cLanguageSelect == 0)
		{
			strcpy(gl_cLcdDispBuf[0],"  ��������");
			sprintf(gl_cLcdDispBuf[1],"  �밴��'%c'!",cKey);
		}
		else
		{
			strcpy(gl_cLcdDispBuf[0],"keyboard check");
			sprintf(gl_cLcdDispBuf[1],"Press key '%c'!",cKey);
		}

		lcd_refresh();
		gl_cKeyPE = gl_cKeyPH;
		while (1)
		{
			WaitKeyExit(0xff);		//�ȴ��м�����
			ch = gl_cKeyBuf[gl_cKeyPE];
			if (ch != cKey)
			{
				if(gl_cLanguageSelect == 0)
				{
					strcpy(gl_cLcdDispBuf[0],"    ����");
					sprintf(gl_cLcdDispBuf[1],"�����µ��Ǽ�%c!",ch);
				}
				else
				{
					strcpy(gl_cLcdDispBuf[0],"    WARING!");
					sprintf(gl_cLcdDispBuf[1],"The key is '%c'!",ch);
				}
				lcd_refresh();
				Delay(20000);
			}
			if ((gl_cKeyPE + 1) == KeyBufLength)
				gl_cKeyPE = 0;
			else
				gl_cKeyPE++;
			break;
		}
	}while (cKey != '9');
}
//***********************************************************************************

//***********************************************************************************
//��ֽ��������
void AM_MovePaperLenght(void)			
{

}
//**********************************************************************************

//***********************************************************************************
//����ѡ��
void AM_LangueSelct(void)				
{
	char cKey=1;
	char ch;
	

	if(gl_cLanguageSelect == 0)
	{
		strcpy(gl_cLcdDispBuf[0],"����ѡ��");
		strcpy(gl_cLcdDispBuf[1],"1:����2:Ӣ��");
	}
	else
	{
		strcpy(gl_cLcdDispBuf[0],"Langue Select");
		strcpy(gl_cLcdDispBuf[1],"1:Chinese 2:English");
	}

	lcd_refresh();
	gl_cKeyPE = gl_cKeyPH;
	while (cKey)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		ch = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		switch(ch)
		{
			case Key_F1:
				gl_cLanguageSelect = 0;
				WriteParameter();				
				MC_CommonMenuDispInit();
				ADM_PopMenuInit();
				ADM_ViewErrMenuInit();					//20141202 rwt
				cKey = 0;
				break;

			case Key_F2:
				gl_cLanguageSelect = 1;
				WriteParameter();			
				MC_CommonMenuDispInit();
				ADM_PopMenuInit();
				ADM_ViewErrMenuInit();					//20141202 rwt
				cKey = 0;
				break;

			case Key_Ok:
			case Key_Pause:
				cKey = 0;
				break;
		}
	}
	
}
//***********************************************************************************

//****************�߼��˵�����*********************
//�߼��˵����Գ�ʼ��
void ADM_PopMenuInit(void)
{
	if(gl_cLanguageSelect==0)
		{
			strcpy(gl_cPopMenu[0],"����ѡ��");
			strcpy(gl_cPopMenu[1],"��ֽ�������");
			strcpy(gl_cPopMenu[2],"��ֽ�������");
			strcpy(gl_cPopMenu[3],"���̲���");
	//		strcpy(gl_cPopMenu[4],"��ӡͷ��ϴ��ʱ");
			strcpy(gl_cPopMenu[4],"����ѡ��");
			strcpy(gl_cPopMenu[5],"");
		}
		else
		{
			strcpy(gl_cPopMenu[0],"Type Choose");
			strcpy(gl_cPopMenu[1],"Roll Motor Test");
			strcpy(gl_cPopMenu[2],"Send Motor Test");
			strcpy(gl_cPopMenu[3],"Keyborad Test");
	//		strcpy(gl_cPopMenu[4],"CleanHead Delay");
			strcpy(gl_cPopMenu[4],"Langue Select");
			strcpy(gl_cPopMenu[5]," ");
		}

}

//�߼��˵���ʾ
void ADM_DispPopMenu(unsigned char cFirstMenuNo,unsigned char cInverseMenuNo)
{
	if (cFirstMenuNo < cInverseMenuNo)
		{
			strcpy(gl_cLcdDispBuf[0],gl_cPopMenu[cInverseMenuNo-1]);
			strcpy(gl_cLcdDispBuf[1],gl_cPopMenu[cInverseMenuNo]);
			lcd_refresh();
			LineReverse(0x02);
		}
		else
		{
			strcpy(gl_cLcdDispBuf[0],gl_cPopMenu[cInverseMenuNo]);
			strcpy(gl_cLcdDispBuf[1],gl_cPopMenu[cInverseMenuNo+1]);
			lcd_refresh();
			LineReverse(0x01);
		}

}

//�߼��˵�����
void ADM_PopMenuManage(void)
{
	unsigned char cTemp;
	unsigned char cChooseMenu,cDispFirstMenu;
  
	cChooseMenu = 0;
	cDispFirstMenu = 0;
	ADM_DispPopMenu(cDispFirstMenu,cChooseMenu);
	while (1)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		cTemp = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		switch (cTemp)
		{
			case Key_Pause:
				WaitKeyBound(Key_Pause);
				return;
				
			case Key_F8:
				if (cChooseMenu < 6)
				{
					if (cDispFirstMenu != cChooseMenu)
						cDispFirstMenu++;
					cChooseMenu++;
					ADM_DispPopMenu(cDispFirstMenu,cChooseMenu);
				}
				break;
				
			case Key_F2:
				if (cChooseMenu > 0)
				{
					if (cDispFirstMenu == cChooseMenu)
						cDispFirstMenu--;
					cChooseMenu--;
					ADM_DispPopMenu(cDispFirstMenu,cChooseMenu);
				}
				break;	
				
			case Key_Ok:
				LineReverse(0);
				switch (cChooseMenu)
				{
					case 0:
						AM_MacType();
						break;
						
					case 1:
						AM_PaperCollectTest();
						break;
						
					case 2:
						AM_PaperFeederTest();
						break;
						
					case 3:
						AM_KeyTest();
						break;
						
					//case 4:
						//AM_MovePaperLenght();
						//break;

					// ��ӡͷ��ϴ��ʱʱ�䣬�豸����״̬����ʱ�೤ʱ����ٴδ�ӡ�Ƿ�ִ����ϴ��ӡͷ
					//case 4:
					//	AM_PrintHeadClearDelaySet();
					//	break;
						
					case 5:
						AM_LangueSelct();
						break;
						
					default:
						break;
				}
				ADM_DispPopMenu(cDispFirstMenu,cChooseMenu);
				break;
			default:
				break;
		}
	}
}
//**************************************************

#define COMMON_DELIMITER

//**************ͨ�ò˵�����************************
const unsigned char Secrity[6] = {'3','5','2','4','8','6'};
//������֤ 0��֤����,1��ȷ
int MC_VerifySecrity(void)
{
	unsigned int i;
	unsigned char ch;
	
	for (i=0;i<6;)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		ch = gl_cKeyBuf[gl_cKeyPE];
		if (ch == Secrity[i])
		{
			if ((gl_cKeyPE + 1) == KeyBufLength)
				gl_cKeyPE = 0;
			else
				gl_cKeyPE++;
			i++;
		}
		else
			return 0;
	}
	return 1;
}

//ͨ�ò˵���ʾ��ʼ��
void MC_CommonMenuDispInit(void)
{
	if(gl_cLanguageSelect==0)
	{
		strcpy(gl_cMenu[0],"��ӡ����ͼ");
		strcpy(gl_cMenu[1],"�ٶ�����");
		strcpy(gl_cMenu[2],"�ʵ�����");
		strcpy(gl_cMenu[3],"ƫ������");
		strcpy(gl_cMenu[4],"XY����У��");
		strcpy(gl_cMenu[5],"�и�ֶγ���");
		strcpy(gl_cMenu[6],"�Զ���ֽ����");
		strcpy(gl_cMenu[7],"�Զ���ֽ����");
		strcpy(gl_cMenu[8],"��������");
		strcpy(gl_cMenu[9],"����汾");
		strcpy(gl_cMenu[10],"��ϴ��ͷʱ��");
	}
	else										//20141201 rwt
	{
		strcpy(gl_cMenu[0],"PrintTest Chart");
		strcpy(gl_cMenu[1],"Speed Setting");
		strcpy(gl_cMenu[2],"PenKnife Attrib");
		strcpy(gl_cMenu[3],"Setover Set");
		strcpy(gl_cMenu[4],"xy LengthAdjust");
		strcpy(gl_cMenu[5],"Cut SectiLength");
		strcpy(gl_cMenu[6],"AutoSendPaprSet");
		strcpy(gl_cMenu[7],"AutoCutDownPapr");
		strcpy(gl_cMenu[8],"Network Setting");
		strcpy(gl_cMenu[9],"SoftwareVersion");
		strcpy(gl_cMenu[10],"CleanTime");
	}
}

//ͨ�ò˵���ʾ
void MC_DispMenu(unsigned char cFirstMenuNo,unsigned char cInverseMenuNo)
{
	if (cFirstMenuNo < cInverseMenuNo)
	{
		strcpy(gl_cLcdDispBuf[0],gl_cMenu[cInverseMenuNo-1]);
		strcpy(gl_cLcdDispBuf[1],gl_cMenu[cInverseMenuNo]);
		lcd_refresh();
		LineReverse(0x02);
	}
	else
	{
		strcpy(gl_cLcdDispBuf[0],gl_cMenu[cInverseMenuNo]);
		strcpy(gl_cLcdDispBuf[1],gl_cMenu[cInverseMenuNo+1]);
		lcd_refresh();
		LineReverse(0x01);
	}
}
extern void  DrawTestGraph(void);

// ����ֽ�ſ��
void CM_SetPaperWidth()
{
	char  ch, cState;
	char dispBuf[18];
	int iPaperWidth;
	
	iPaperWidth = RPGL_iAutoCutOff;

	if(gl_cLanguageSelect == 0)
	{
		lcd_display(0, "ֽ�ſ������");
		sprintf(dispBuf, "%dmm", iPaperWidth);
		lcd_display(1, dispBuf);
	}
	else
	{
		lcd_display(0, "Paper Width");
		sprintf(dispBuf, "%dmm", iPaperWidth);
		lcd_display(1, dispBuf);
	}
	
	cState = 1;
	while (cState)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		ch = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (ch)
		{
			case Key_F2:
				iPaperWidth += 10;
				if(iPaperWidth > gl_sPlotType)
					iPaperWidth = gl_sPlotType;
				break;
		
			case Key_F8:
				iPaperWidth -= 10;
				if(iPaperWidth < 100)
					iPaperWidth = gl_sPlotType;
				break;

			case Key_F4:
				iPaperWidth -= 100;
				if(iPaperWidth < 100)
					iPaperWidth = gl_sPlotType;
				break;

			case Key_F6:
				iPaperWidth += 100;
				if(iPaperWidth > gl_sPlotType)
					iPaperWidth = gl_sPlotType;
				break;
			
			case Key_Ok:
				RPGL_iAutoCutOff = iPaperWidth;
				cState = 0;
				WriteParameter();
				break;
			case Key_Pause:
				cState = 0;
				break;
		}
		
		sprintf(dispBuf, "%dmm", iPaperWidth);
		lcd_display(1, dispBuf);
	}
}

// �Զ���ֽ����
void CM_AutoCutPaper()
{
	char cOverCutOffPaper, ch, cState;

	cOverCutOffPaper = gl_cOverCutOffPaper;

	if(gl_cLanguageSelect == 0)
	{
		lcd_display(0, "�����Զ���ֽ");
		if(cOverCutOffPaper == 1)
			lcd_display(1, "����");
		else
			lcd_display(1, "�ر�");
	}
	else
	{
		lcd_display(0, "Over Auto Cut Paper");
		if(cOverCutOffPaper == 1)
			lcd_display(1, "Open");
		else
			lcd_display(1, "Close");
	}
	
	cState = 1;
	while (cState)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		ch = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (ch)
		{
			//case Key_F1:
			//	// ����ֽ�ſ��
			//	CM_SetPaperWidth();
			//	cState = 0;
			//	break;
		
			case Key_F2:
				cOverCutOffPaper = cOverCutOffPaper == 0 ? 1 : 0;
				if(gl_cLanguageSelect == 0)
				{
					if(cOverCutOffPaper == 1)
						lcd_display(1, "����");
					else
						lcd_display(1, "�ر�");
				}
				else
				{
					if(cOverCutOffPaper == 1)
						lcd_display(1, "Open");
					else
						lcd_display(1, "Close");	
				}
				break;
			case Key_Ok:
				gl_cOverCutOffPaper = cOverCutOffPaper;
				Printf("gl_cOverCutOffPaper  ��ӡ&�����ж�ֽ�� = %d " ,gl_cOverCutOffPaper );
				cState = 0;
				WriteParameter();
				break;
			case Key_Pause:
				cState = 0;
				break;
		}
	}
}

//ͨ�ò˵�����
void M_CommonMenuMange(void)
{
	unsigned char cReadKey;
	unsigned char cEnd;
	unsigned char cInverseMenuNo1;
	unsigned char cFirstMenuNo1;	
		
	cInverseMenuNo1 = 0;
	cFirstMenuNo1 = 0;
	MC_DispMenu(cFirstMenuNo1,cInverseMenuNo1);
	cEnd = 1;
	while (cEnd)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		switch (cReadKey)
		{
			case Key_Pause:
				WaitKeyBound(Key_Pause);
				cEnd = 0;
				break;
				
			case Key_Ok:
				LineReverse(0);
				switch (cInverseMenuNo1)
				{
					case 0:		//����ͼ�λ���
						CM_PrintTestGrap();
						break;
						
					case 1:		//�ٶ�����
						CM_SetSpeed();
						break;
						
					case 2:		//�ʵ�����
						CM_PenCutAttribute();
					//	CM_XY_SpeedRatio();/*2016-1-7*/
						break;
						
					case 3:		//ƫ������
						CM_OffsetDistSeting();
						break;
						
					case 4:		//XY����У��
						CM_XYLengthCheck();
						break;
						
					case 5:		//�и�ֶγ���
						CM_CutSectionLength();
						break;

					case 6:		//�Զ���ֽ
						CM_AutoSendPaper();
						break;
						
					case 7:		// �Զ���ֽ
						CM_AutoCutPaper();
						break;
						
					case 8:		//�������
						CM_NetManage();
						break;
						
					case 9:		//����汾
						CM_VerionsQuery();
						break;

					case 10: 	//��ϴ��īͷ
						FUNS_ClearTime();
				//		FUN_SetPrintClearTimes();
					 	
						break;

											
					default:
						break;
				}
				MC_DispMenu(cFirstMenuNo1,cInverseMenuNo1);
				break;
				
			case Key_F8:
				if (cInverseMenuNo1 < 11)
				{   
					if (cFirstMenuNo1 != cInverseMenuNo1)
						cFirstMenuNo1++;
					cInverseMenuNo1++;

					if(cInverseMenuNo1 == 11)
					{
						cFirstMenuNo1 = 0;
						cInverseMenuNo1 = 0;
					}
					
					MC_DispMenu(cFirstMenuNo1,cInverseMenuNo1);
				}
				break;
				
			case Key_F2:
				if (cInverseMenuNo1 > 0)
				{   
					if (cFirstMenuNo1 == cInverseMenuNo1)
						cFirstMenuNo1--;
					cInverseMenuNo1--;					
					MC_DispMenu(cFirstMenuNo1,cInverseMenuNo1);
				}
				else
				{
					cFirstMenuNo1 = 8;
					cInverseMenuNo1 = 9;
					MC_DispMenu(cFirstMenuNo1,cInverseMenuNo1);
				}
				break;

			case Key_F4:
			case Key_F6:
				CM_SetOriginManage();
				MC_DispMenu(cFirstMenuNo1,cInverseMenuNo1);
				break;
				
			case Key_F1:
				if (MC_VerifySecrity() == 1)
				{
					ADM_PopMenuManage();
					MC_DispMenu(cFirstMenuNo1,cInverseMenuNo1);
				}
				break;
				
			default:
				break;
		}
	}	
}

// 20141027�ֶ���λ�ж�ֽ��
void CM_ManualCutPaper()
{
	char cState;
	char ch, cend;
	int  iOriginalPos;
	int iRepeatPress;
	int iPulseNum;
	int iTotalPulseNum;
	double dStep;
	
	if(gl_cLanguageSelect == 0)
	{
		strcpy(gl_cLcdDispBuf[0],"�ֶ��ж�ֽ��");
		strcpy(gl_cLcdDispBuf[1],"2 ��λ5 �ж� ");
	}
	else
	{
		strcpy(gl_cLcdDispBuf[0],"Manual Cut Paper");
		strcpy(gl_cLcdDispBuf[1],"2FixedPoint/5 Cut");
	}
	lcd_refresh();	

	if (gl_cCutUpDownState == 1)
		CtrlUpCut();

	iRepeatPress = 0;
	iTotalPulseNum = 0;
	cend = 1;
	cState = 1;
	while (cState)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		ch = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;

		switch (ch)
		{
			case Key_F2:
				iOriginalPos = gl_iOriginalPos;
				CM_SetOriginManage();
				gl_iOriginalPos = iOriginalPos;
				break;
				
			case Key_Ok:		// �ж��˳�
				// 
				//����ƫ�Ʊʵ����
				iTotalPulseNum = 0;
				Printf("gl_iCutDistanceY:%d\n", gl_iCutDistanceY);
				if(gl_iAxisYPos == 0 )
				{
					iTotalPulseNum = fabs(gl_iCutDistanceY)  * DotPulseNum;
					Printf("iTotalPulseNum:%d\n", -iTotalPulseNum);
					FixedSpeeDrive(AXIS_Y, 2000);
					ITMoveHandpiece(AXIS_Y, -iTotalPulseNum);
				}
				
				CtrlDownCut();
				Delay(1000);
				Printf("1 gl_iAxisYPos:%d  gl_iOriginalPos:%d\n", gl_iAxisYPos, gl_iOriginalPos);
				FixedSpeeDrive(AXIS_Y, 2500);
				do{
					
					if (iRepeatPress > 50)
					{
						iRepeatPress += 2;
						if (iRepeatPress > 300)
							iRepeatPress = 300;
						dStep = (double)iRepeatPress * (double)iRepeatPress / 50.0 * 0.2 * Y_PPMM;
					}
					else
					{
						iRepeatPress++;
						dStep = (double)iRepeatPress * (double)iRepeatPress / 50.0 * 0.1;
						if (dStep < 0.3)
							dStep = 0.3;
						dStep = dStep * Y_PPMM;
					}

					iPulseNum =  (int)dStep;
										
					if ((gl_iAxisYPos + iPulseNum) >= gl_iYMaxEffectivePMM)
					{		
						if(gl_iAxisYPos == gl_iYMaxEffectivePMM)
							iPulseNum = 0;
						else
							iPulseNum = gl_iYMaxEffectivePMM - gl_iAxisYPos;
					}
					
					//Printf("gl_cKeyBuf[%d]=%d, ", gl_cKeyPE, gl_cKeyBuf[gl_cKeyPE] );
					gl_iAxisYPos += iPulseNum;
					if (iPulseNum != 0)
					{
						ITMoveHandpiece(AXIS_Y, iPulseNum);
					}

					while (gl_cKeyPE != gl_cKeyPH)
					{
						if (gl_cKeyBuf[gl_cKeyPE] != ch)
						{
							cend = 0;
							break;
						}
						else if ((gl_cKeyPE +1) == KeyBufLength)
							gl_cKeyPE = 0;
						else
							gl_cKeyPE++;
					}
					
				}while(cend == 1);
				CtrlUpCut();
				Delay(8000);
				Printf("2 gl_iAxisYPos:%d  gl_iOriginalPos:%d , iPulseNum%d\n", gl_iAxisYPos, gl_iOriginalPos, iPulseNum);
				iPulseNum = gl_iAxisYPos - gl_iOriginalPos - iTotalPulseNum;
				Printf("3 iPulseNum:%d\n", iPulseNum);
				gl_iAxisYPos = gl_iOriginalPos;
				SingleAxisInpDriveSpeed(AXIS_Y,5, 300, 600);
				ITMoveHandpiece(AXIS_Y,  -iPulseNum);
				cState = 0;
				break;

				case Key_Pause:
					cState = 0;
					break;
		}
		
		if(gl_cLanguageSelect == 0)
		{
			strcpy(gl_cLcdDispBuf[0],"�ֶ��ж�ֽ��");
			strcpy(gl_cLcdDispBuf[1],"2 ��λ5 �ж� ");
		}
		else
		{
			strcpy(gl_cLcdDispBuf[0],"Manual Cut Paper");
			strcpy(gl_cLcdDispBuf[1],"2FixedPoint/5 Cut");
		}
		lcd_refresh();
	}
}

//����������ѭ��
void FUN_KBProc(void)
{
	unsigned char cReadKey;
	unsigned char cKeyPE;
	
	while (gl_cKeyPH != gl_cKeyPE)
	{	
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		cKeyPE = gl_cKeyPE + 1;
		
		if (cKeyPE == KeyBufLength)
			cKeyPE = 0;
		cReviceFlag = 1;
		gl_cKeyPE = gl_cKeyPH;

		if(gl_MacRunState != 0)
		{
			if(cReadKey != Key_Pause)
			{
				Printf("!= F7\n");
				cReviceFlag = 1;
				return;
			}
			else 
				PauseRecover();
			if(gl_cLanguageSelect==0)
				LcdDisp("  ��ӭʹ��", "������ī�и�� ");
			else
				LcdDisp("Welcome to use", "RICHPEACE CAD");
			if(gl_cKeyPH != gl_cKeyPE)
				gl_cKeyPE = gl_cKeyPH;
			cReviceFlag = 0;
			return;
		}
		
		switch (cReadKey)
		{
			case Key_F1:
				//������ͨ�˵�����
				M_CommonMenuMange();
				break;

			case Key_F3:
				//�ֶ��ж�
				CM_ManualCutPaper();
				break;
				
			case Key_F2:
			case Key_F4:
			case Key_F6:
			case Key_F8:
				//����ԭ������
				CM_SetOriginManage();
				break;
			case Key_F9:								/*2016-1-11HZX*/
			//  Delete_All_File();					
				break;
			case Key_Pause:
				//��ͣ
				break;
			case Key_Ok:
			//����{code}
//				Printf("gl_lCurPost_XPluse \t= %l    gl_lCurPost_YPluse\t= %l \n",gl_lCurPost_XPluse,gl_lCurPost_YPluse);
//				Printf("gl_lWorkOrig_XXPluse\t= %l    gl_lWorkOrig_YYPluse\t= %l \n",gl_lWorkOrig_XXPluse,gl_lWorkOrig_YYPluse);
//				Printf("gl_lPostMax_XPluse \t= %l    gl_lPostMax_YPluse\t= %l \n",gl_lPostMax_XPluse,gl_lPostMax_YPluse);
//				Printf("gl_iOriginalPos \t= %l  \n" , gl_iOriginalPos);
				break;
		}
		LineReverse(0);
		if(gl_cLanguageSelect==0)
			LcdDisp("  ��ӭʹ��", "������ī�и�� ");
		else
			LcdDisp("Welcome to use", "RICHPEACE CAD");
		cReviceFlag = 0;
	}
}

//����˵���ʾ��ʼ��20141202
void ADM_ViewErrMenuInit(void)
{
	if (gl_cLanguageSelect == 0) {	
		strcpy(gl_cViewErrMenu[0],"��ʼ�����ڳ���");
		strcpy(gl_cViewErrMenu[1],"    ͨѶ����!");
		strcpy(gl_cViewErrMenu[2],"    ͨѶ����!");
		strcpy(gl_cViewErrMenu[3],"��ͼ�����");
		strcpy(gl_cViewErrMenu[4]," X���ƶ�����!");
		strcpy(gl_cViewErrMenu[5]," Y���ƶ�����!");
		strcpy(gl_cViewErrMenu[6],"  ԭ���Ӧ����");
		strcpy(gl_cViewErrMenu[7],"  SRAM�Լ����");
		strcpy(gl_cViewErrMenu[8],"δ�������");
		strcpy(gl_cViewErrMenu[9],"X ��λ��ƫ���");
		strcpy(gl_cViewErrMenu[10],"Y ��λ��ƫ���");
		strcpy(gl_cViewErrMenu[11],"ֽ������!");
		strcpy(gl_cViewErrMenu[12],"�ϸ�Ӧ����");
		strcpy(gl_cViewErrMenu[13],"�¸�Ӧ����");
		strcpy(gl_cViewErrMenu[14],"���Ӧ����");
		strcpy(gl_cViewErrMenu[15],"�Ҹ�Ӧ����");
		strcpy(gl_cViewErrMenu[16]," Z��Ӧ����");
		strcpy(gl_cViewErrMenu[17],"�������ô�");
	}
	else {
		strcpy(gl_cViewErrMenu[0],"Init COM Fault");
		strcpy(gl_cViewErrMenu[1],"Communict Fault");
		strcpy(gl_cViewErrMenu[2],"Communict Fault");
		strcpy(gl_cViewErrMenu[3],"Print Order Err");
		strcpy(gl_cViewErrMenu[4],"X Axis Extend");
		strcpy(gl_cViewErrMenu[5],"Y Axis Extend");
		strcpy(gl_cViewErrMenu[6],"OriginSensorErr");
		strcpy(gl_cViewErrMenu[7],"  SRAM Error");
		strcpy(gl_cViewErrMenu[8],"Undefine Error");
		strcpy(gl_cViewErrMenu[9],"X AxisDeviation");
		strcpy(gl_cViewErrMenu[10],"Y AxisDeviation");
		strcpy(gl_cViewErrMenu[11],"No Paper!");
		strcpy(gl_cViewErrMenu[12],"UpSensorBrokn");
		strcpy(gl_cViewErrMenu[13],"DownSensorBrokn");
		strcpy(gl_cViewErrMenu[14],"LeftSensorBrokn");
		strcpy(gl_cViewErrMenu[15],"RigtSensorBrokn");
		strcpy(gl_cViewErrMenu[16]," Z SensorBrokn");
		strcpy(gl_cViewErrMenu[17],"Knife NUM Err");

	}

}

