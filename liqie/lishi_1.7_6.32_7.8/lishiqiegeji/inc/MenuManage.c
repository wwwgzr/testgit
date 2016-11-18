//******************MENUMANAG.C****************
//操作界面偏距设置
//**********************************************

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "..\src\defconst.h"
#include "..\Target\44blib.h"
#include "..\src\extern.h"
#include "..\src\MenuManage.h"

//#define 	AXIS_X		2							//X轴
//#define 	AXIS_Y		1							//Y轴
#define LANGUAGE    0


//void  MoveFrame(unsigned char KeyNum, int *iCurPosX, int *iCurPosY);
void  ReturnOriginal(long lCurPost_XPlus, long lCurPost_YPlus);
void  DrawTestGraph(void);
void  DrawTestGraphManage(void);
void  RP_SetOrigin(char cIsSetOrg);
void  RP_AutoMovPaper(void);
void ReturnDirectPost(char cDirectPost);

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
//网络设置F2
extern int ReceiveData(void);
extern int lcd_display(unsigned char row,const char *string);
extern int WaitKeyBound(unsigned char cKeyValue);
extern char language[100][30] ;//语言文字
extern void DecSpeedState(int axis);
extern void StopWorkRetu(void);
extern void InitFlatPlotter(void);

//版本号显示
void VersionDisplay(void)
{
	unsigned char cReadKey;//键值
	char  state;

	if(gl_cLanguageSelect == 0)
	{
		strcpy(gl_cLcdDispBuf[0],"版本号:V1.05");
		strcpy(gl_cLcdDispBuf[1],"日期:16/6/21");
	}
	else
	{
		strcpy(gl_cLcdDispBuf[0],"Ver:V1.05");
		strcpy(gl_cLcdDispBuf[1],"Date:16/6/21");
	}
	lcd_refresh();

	state = 1;
	while(state)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (cReadKey)
		{
			case Key_Parameter:
				OffsetDistanceManage();
				break;

			case Key_Ok:
			case Key_Pause:
				state = 0;
				break;
		}
	}
	//WaitKeyExit(Key_Pause);
}

void ArrCpy(int Dest[], int Sour[], int len)
{
	int i;
	for(i=0; i<len; i++)
		Dest[i] = Sour[i];
}
/*
void TestCutFW(void)
{//校正切割位置
	long  ypluse;
	int iStatus;
	
	//移动到右下
	RP_SetDoubleAxisADSpeed(gl_iCutSpeed);
	if(gl_lCurPost_XPluse != gl_lMaxX)
	{
		pmove(AXIS_X, gl_lMaxX-gl_lCurPost_XPluse);
		do
		{
			get_status(AXIS_X,&iStatus);
		}while(iStatus!=0);
		gl_lCurPost_XPluse = gl_lMaxX;
	}	
	
	if(gl_lCurPost_YPluse != 0)
	{
		pmove(AXIS_Y, -gl_lCurPost_YPluse);
		do
		{
			get_status(AXIS_Y,&iStatus);
		}while(iStatus!=0);
		gl_lCurPost_YPluse = 0;
	}
		
	//偏移8.5
	gl_lCurPost_YPluse = (long)(85.0*Y_PPMM);
	pmove(AXIS_Y, gl_lCurPost_YPluse);
	do
	{
		get_status(AXIS_Y,&iStatus);
	}while(iStatus!=0);
	
	//切1cm十字
	CtrlDownCut();
	pmove(AXIS_Y, 10*Y_PPMM);
	do
	{
		get_status(AXIS_Y,&iStatus);
	}while(iStatus!=0);
	Delay(500);
	
	pmove(AXIS_Y, -5*Y_PPMM);
	do
	{
		get_status(AXIS_Y,&iStatus);
	}while(iStatus!=0);
	Delay(500);
	
	pmove(AXIS_X, -10*Y_PPMM);
	do
	{
		get_status(AXIS_X,&iStatus);
	}while(iStatus!=0);
	Delay(500);
	
	pmove(AXIS_X, 10*Y_PPMM);
	do
	{
		get_status(AXIS_X,&iStatus);
	}while(iStatus!=0);
	CtrlUpCut();
		
	//移动机器实际长度
	ypluse = gl_lMaxY - (long)(175*Y_PPMM);
	pmove(AXIS_Y, ypluse);
	do
	{
		get_status(AXIS_Y,&iStatus);
	}while(iStatus!=0);
	
	//切1cm十字
	CtrlDownCut();
	pmove(AXIS_Y, 10*Y_PPMM);
	do
	{
		get_status(AXIS_Y,&iStatus);
	}while(iStatus!=0);
	Delay(500);
	
	pmove(AXIS_Y, -5*Y_PPMM);
	do
	{
		get_status(AXIS_Y,&iStatus);
	}while(iStatus!=0);
	Delay(500);
	
	pmove(AXIS_X, -10*X_PPMM);
	do
	{
		get_status(AXIS_X,&iStatus);
	}while(iStatus!=0);
	Delay(500);
	
	pmove(AXIS_X, 10*X_PPMM);
	do
	{
		get_status(AXIS_X,&iStatus);
	}while(iStatus!=0);
	CtrlUpCut();

	//移动到左下
	pmove(AXIS_X, -gl_lCurPost_XPluse);
	do
	{
		get_status(AXIS_X,&iStatus);
	}while(iStatus!=0);
	
	//切1cm十字
	CtrlDownCut();
	pmove(AXIS_Y, 5*Y_PPMM);
	do
	{
		get_status(AXIS_Y,&iStatus);
	}while(iStatus!=0);
	Delay(500);
	
	pmove(AXIS_Y, -10*Y_PPMM);
	do
	{
		get_status(AXIS_Y,&iStatus);
	}while(iStatus!=0);
	Delay(500);
	
	pmove(AXIS_Y, 5*Y_PPMM);
	do
	{
		get_status(AXIS_Y,&iStatus);
	}while(iStatus!=0);
	Delay(500);
	
	pmove(AXIS_X, 10*X_PPMM);
	do
	{
		get_status(AXIS_X,&iStatus);
	}while(iStatus!=0);
	Delay(500);
	
	pmove(AXIS_X, -10*X_PPMM);
	do
	{
		get_status(AXIS_X,&iStatus);
	}while(iStatus!=0);
	Delay(500);
	
	pmove(AXIS_Y, -5*Y_PPMM);
	do
	{
		get_status(AXIS_Y,&iStatus);
	}while(iStatus!=0);
	CtrlUpCut();
	

	//移动到右下
	ypluse = gl_lMaxY - (long)(170*Y_PPMM);
	pmove(AXIS_Y, -ypluse);
	WaitDriveEnd(AXIS_Y);
	
	//切1cm十字
	CtrlDownCut();
	pmove(AXIS_Y, 10*Y_PPMM);
	do
	{
		get_status(AXIS_Y,&iStatus);
	}while(iStatus!=0);
	Delay(500);
	
	pmove(AXIS_Y, -5*Y_PPMM);
	do
	{
		get_status(AXIS_Y,&iStatus);
	}while(iStatus!=0);
	Delay(500);
	
	pmove(AXIS_X, 10*X_PPMM);
	do
	{
		get_status(AXIS_X,&iStatus);
	}while(iStatus!=0);
	Delay(500);
	
	pmove(AXIS_X, -10*X_PPMM);
	do
	{
		get_status(AXIS_X,&iStatus);
	}while(iStatus!=0);
	Delay(500);
	
	pmove(AXIS_Y, -5*Y_PPMM);
	do
	{
		get_status(AXIS_Y,&iStatus);
	}while(iStatus!=0);
	CtrlUpCut();

	gl_lCurPost_YPluse = (long)(85.0*Y_PPMM);
	pmove(AXIS_Y, -gl_lCurPost_YPluse);
	WaitDriveEnd(AXIS_Y);
	
	gl_lCurPost_YPluse = 0;
	gl_lCurPost_XPluse = 0;
}


void TestPrintFw(void)
{
	int iStatus;
	
	//移动到右下
	RP_SetDoubleAxisADSpeed(gl_iCutSpeed);
	if(gl_lCurPost_XPluse != gl_lMaxX)
	{
		pmove(AXIS_X, gl_lMaxX-gl_lCurPost_XPluse);
		do
		{
			get_status(AXIS_X,&iStatus);
		}while(iStatus!=0);
		gl_lCurPost_XPluse = gl_lMaxX;
	}	

	gl_lCurPost_XPluse -= (50*X_PPMM);
	pmove(AXIS_X, -50*X_PPMM);
	do
	{
		get_status(AXIS_X,&iStatus);
	}while(iStatus!=0);
	
	
	if(gl_lCurPost_YPluse != 0)
	{
		pmove(AXIS_Y, -gl_lCurPost_YPluse);
		do
		{
			get_status(AXIS_Y,&iStatus);
		}while(iStatus!=0);
		gl_lCurPost_YPluse = 0;
	}

	//Y回到0位
	gl_lCurPost_YPluse = 50*Y_PPMM;
	pmove(AXIS_Y, gl_lCurPost_YPluse);
	do
	{
		get_status(AXIS_Y,&iStatus);
	}while(iStatus!=0);
	
	WaitKeyExit(0xff);
	if ((gl_cKeyPE+1) == KeyBufLength)
		gl_cKeyPE = 0;
	else
		gl_cKeyPE++;
	
	//模拟打印动作
	gl_lCurPost_YPluse = -50*Y_PPMM;
	pmove(AXIS_Y, gl_lCurPost_YPluse);
	do
	{
		get_status(AXIS_Y,&iStatus);
	}while(iStatus!=0);
	gl_lCurPost_YPluse = 0;
	Delay(1000);
	
	//打印长度1200mm + 两侧空走长度 
	//打印方向左侧
	gl_lCurPost_YPluse = (1200 + 50)*Y_PPMM;
	pmove(AXIS_Y, gl_lCurPost_YPluse);
	do
	{
		get_status(AXIS_Y,&iStatus);
	}while(iStatus!=0);
	
	WaitKeyExit(0xff);
	if ((gl_cKeyPE+1) == KeyBufLength)
		gl_cKeyPE = 0;
	else
		gl_cKeyPE++;

	gl_lCurPost_YPluse += (50*Y_PPMM);
	pmove(AXIS_Y, 50*Y_PPMM);
	do
	{
		get_status(AXIS_Y,&iStatus);
	}while(iStatus!=0);
	WaitKeyExit(0xff);
	if ((gl_cKeyPE+1) == KeyBufLength)
		gl_cKeyPE = 0;
	else
		gl_cKeyPE++;
	
	
	//打印方向右侧
	pmove(AXIS_Y, -gl_lCurPost_YPluse);
	do
	{
		get_status(AXIS_Y,&iStatus);
	}while(iStatus!=0);
	gl_lCurPost_YPluse = 0;
	
}
*/

void TestCutSpeed(void)
{
	long lTempXPluse,lTempYPluse, lTOldX, lTOldY;
	int iStatus, iExtOffset, iCutOffset, i;
	int PostArrX[5],PostArrY[5];

	//准数据
	PostArrX[0] = (50 * X_PPMM);
	PostArrY[0] = (50 * Y_PPMM);

	PostArrX[1] = 0;
	PostArrY[1] = (50 * Y_PPMM);

	PostArrX[2] = (50 * X_PPMM);
	PostArrY[2] = 0;

	PostArrX[3] = 0;
	PostArrY[3] = 0;
	
	PostArrX[4] = 0;
	PostArrY[4] = 0;
	
	
	iCutOffset = gl_iCutOffset*gl_dPosUnitX;
	iExtOffset = gl_dCutExtensValue*gl_dPosUnitX;
	lTOldX = 0;
	lTOldY = 0;
	
	//落刀
	if (gl_cCutUpDownState == 0)
	{  //将刀具落下
		CtrlDownCut();
	}
	//Delay(1000);
	//设置短剧行程两轴直线插补驱动速度
	//RP_SetDoubleAxisADSpeed(100);
	//StraightLineADSpeed(AXIS_Y, gl_iSVSpeed, gl_iVSpeed, gl_iTimeAdd);
	//StraightLineADSpeed(AXIS_Y, 10, 50, 200);
	//TwoAxleInpDriveSpeed(gl_iSVSpeed, gl_iVSpeed, gl_iTimeAdd);
	SingleAxisInpDriveSpeed(AXIS_X+AXIS_Y,gl_iSVSpeed, gl_iVSpeed, gl_iTimeAdd);
	
	for (i=0;i <=3;i++)
	{
		lTempXPluse = PostArrX[i] - lTOldX;  //X轴需要移动的脉冲数
    	lTempYPluse = PostArrY[i] - lTOldY;  //Y轴需要移动的脉冲数
    	
		do
		{
			get_inp_status(&iStatus);
		} while (iStatus != 0);		
		
		/*
		if (labs(lTempXPluse) > labs(lTempYPluse))
			FixedSpeeDrive(AXIS_X,gl_iVSpeed);
		else
			FixedSpeeDrive(AXIS_Y,gl_iVSpeed);
		*/
		inp_move2(lTempXPluse,lTempYPluse);
		lTOldX = PostArrX[i];
		lTOldY = PostArrY[i];
	}
//收刀
	if (gl_cCutUpDownState != 0)
	{  //将刀具提起
	 	CtrlUpCut();
	}
	Delay(1000);

}

void DebugMenuPara(int iMenu, int iValue, char cst)
{
	char cDispbuf[16];
	if(iMenu == 0)
	{
		if(cst == 0)
			strcpy(gl_cLcdDispBuf[0], "Init Speed Y:");
		else
			strcpy(gl_cLcdDispBuf[0], "Init Speed X:");
		sprintf(cDispbuf, "%d", iValue);
		strcpy(gl_cLcdDispBuf[1], cDispbuf);
		lcd_refresh();
	}
	else if(iMenu == 1)
	{
		if(cst == 0)
			strcpy(gl_cLcdDispBuf[0], "Speed Y:");
		else
			strcpy(gl_cLcdDispBuf[0], "Speed X:");
		sprintf(cDispbuf, "%d", iValue);
		strcpy(gl_cLcdDispBuf[1], cDispbuf);
		lcd_refresh();
	}
	else
	{
		if(cst == 0)
			strcpy(gl_cLcdDispBuf[0], "Add Time Y:");
		else
			strcpy(gl_cLcdDispBuf[0], "Add Time X:");
		sprintf(cDispbuf, "%d", iValue);
		strcpy(gl_cLcdDispBuf[1], cDispbuf);
		lcd_refresh();
	}
}

void DebugSpeed(void)
{
	unsigned char cReadKey;
	int iSV, iV, iTime, istate, iMenu, iValue;
	char cDispbuf[16];
	char cXYFlag = 0;
	
	iSV = gl_iSVSpeed;
	iV = gl_iVSpeed;
	iTime = gl_iTimeAdd;
	//gl_iTempATime = gl_iVSpeed-100;
	
	strcpy(gl_cLcdDispBuf[0], "Init Speed Y:");
	sprintf(cDispbuf, "%d", iSV);
	strcpy(gl_cLcdDispBuf[1], cDispbuf);
	lcd_refresh();
	iMenu = 0;
	istate = 1;
	iValue= iSV;
	while(istate)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (cReadKey)
		{
			case Key_F1:
				iMenu++;
				if(iMenu > 2)
					iMenu = 0;
				
				if(iMenu == 0)
				{
					iTime = iValue;
					iValue = iSV;
					if (cXYFlag == 0)
						strcpy(gl_cLcdDispBuf[0], "Init Speed Y:");
					else
						strcpy(gl_cLcdDispBuf[0], "Init Speed X:");
					sprintf(cDispbuf, "%d", iSV);
					strcpy(gl_cLcdDispBuf[1], cDispbuf);
					lcd_refresh();
				}
				else if(iMenu == 1)
				{
					iSV = iValue;
					iValue = iV;
					if (cXYFlag == 0)
						strcpy(gl_cLcdDispBuf[0], "Speed Y:");
					else
						strcpy(gl_cLcdDispBuf[0], "Speed X:");
					sprintf(cDispbuf, "%d", iV);
					strcpy(gl_cLcdDispBuf[1], cDispbuf);
					lcd_refresh();
				}
				else
				{
					iV = iValue;
					iValue = iTime;
					if (cXYFlag == 0)
						strcpy(gl_cLcdDispBuf[0], "Add Time Y:");
					else
						strcpy(gl_cLcdDispBuf[0], "Add Time X:");
					sprintf(cDispbuf, "%d", iTime);
					strcpy(gl_cLcdDispBuf[1], cDispbuf);
					lcd_refresh();
				}
				break;

			case Key_F2:
				iValue += 100;
				if(iValue > 20000)
					iValue = 0;
				DebugMenuPara(iMenu, iValue, cXYFlag);
				break;

			case Key_F3:
				iValue -= 100;
				if(iValue < 0)
					iValue = 20000;
				DebugMenuPara(iMenu, iValue, cXYFlag);
				break;

			case Key_F4:
				if(cXYFlag == 0)
				{
					cXYFlag = 1;
					//Printf("\nYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY\n");
					//gl_cStep = UpdataAccTable(iSV, iV, iTime, gl_iStepSpeed, gl_cMaxStep);
					//Printf("Set Y SetV: iSV[%d] iV[%d] iT[%d] Step[%d]\n", iSV, iV, iTime, gl_cStep);
					
					iSV = gl_iSVSpeedX;
					iV = gl_iVSpeedX;
					iTime = gl_iTimeAddX;
					iValue = iSV;
					strcpy(gl_cLcdDispBuf[0], "Init Speed  X:");
					sprintf(cDispbuf, "%d", iSV);
					strcpy(gl_cLcdDispBuf[1], cDispbuf);
					lcd_refresh();
				}
				else
				{
					cXYFlag = 0;
					//Printf("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
					//gl_cStepX = UpdataAccTable(iSV, iV, iTime, gl_iStepSpeedX, gl_cMaxStepX);
					//Printf("Set X SetV: iSV[%d] iV[%d] iT[%d] Step[%d]\n", iSV, iV, iTime, gl_cStepX);
					
					iSV = gl_iSVSpeed;
					iV = gl_iVSpeed;
					iTime = gl_iTimeAdd;
					iValue = iSV;
					strcpy(gl_cLcdDispBuf[0], "Init Speed  Y:");
					sprintf(cDispbuf, "%d", iSV);
					strcpy(gl_cLcdDispBuf[1], cDispbuf);
					lcd_refresh();
				}
				break;
				
			case Key_F8:
				iValue++;
				if(iValue > 20000)
					iValue = 0;
				DebugMenuPara(iMenu, iValue, cXYFlag);
				break;

			case Key_F9:
				iValue--;
				if(iValue < 0)
					iValue = 20000;
				DebugMenuPara(iMenu, iValue, cXYFlag);	
				break;

			case Key_Ok:
				iValue += 10;
				if(iValue > 20000)
					iValue = 0;
				DebugMenuPara(iMenu, iValue, cXYFlag);
				break;

			case Key_F6:
				iValue -= 10;
				if(iValue < 0)
					iValue = 20000;
				DebugMenuPara(iMenu, iValue, cXYFlag);
				break;
/*
			case Key_Drop:
				gl_iTempATime += 10;
				sprintf(cDispbuf, "%d", gl_iTempATime);
				strcpy(gl_cLcdDispBuf[0], "TTime:");
				strcpy(gl_cLcdDispBuf[1], cDispbuf);
				lcd_refresh();
				break;

			case Key_Speed:
				gl_iTempATime -= 10;
				sprintf(cDispbuf, "%d", gl_iTempATime);
				strcpy(gl_cLcdDispBuf[0], "TTime:");
				strcpy(gl_cLcdDispBuf[1], cDispbuf);
				lcd_refresh(); 
				break;
			*/
			case Key_Pause:
				if(iMenu == 0)
					iSV = iValue;
				else if(iMenu == 1)
					iV = iValue;
				else
					iTime = iValue;

				if(cXYFlag == 0)
				{
					gl_iSVSpeed = iSV;
					gl_iVSpeed = iV;
					gl_iTimeAdd = iTime;
					//Printf("\nYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY\n");
					//gl_cStep = UpdataAccTable(iSV, iV, iTime, gl_iStepSpeed, gl_cMaxStep);
					//Printf("Y SetV: iSV[%d] iV[%d] iT[%d] Step[%d]\n", iSV, iV, iTime, gl_cStep);
				}
				else
				{
					gl_iSVSpeedX = iSV;
					gl_iVSpeedX = iV;
					gl_iTimeAddX = iTime;
					//Printf("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
					//gl_cStepX = UpdataAccTable(iSV, iV, iTime, gl_iStepSpeedX, gl_cMaxStepX);
					//Printf("X SetV: iSV[%d] iV[%d] iT[%d] Step[%d]\n", iSV, iV, iTime, gl_cStepX);
				}
				istate = 0;
				break;

			//case Key_Pause:
				//istate = 0;
			//	break;
		}
	}
	
	//LcdDisp("  欢迎使用", "富怡喷墨切割机 ");
}
/*
void DebugMenuPara(int iMenu, int iValue)
{
	char cDispbuf[16];
	if(iMenu == 0)
	{
		strcpy(gl_cLcdDispBuf[0], "初始速度:");
		sprintf(cDispbuf, "%d", iValue);
		strcpy(gl_cLcdDispBuf[1], cDispbuf);
		lcd_refresh();
	}
	else if(iMenu == 1)
	{
		strcpy(gl_cLcdDispBuf[0], "速度设置:");
		sprintf(cDispbuf, "%d", iValue);
		strcpy(gl_cLcdDispBuf[1], cDispbuf);
		lcd_refresh();
	}
	else
	{
		strcpy(gl_cLcdDispBuf[0], "加速时间:");
		sprintf(cDispbuf, "%d", iValue);
		strcpy(gl_cLcdDispBuf[1], cDispbuf);
		lcd_refresh();
	}
}

void DebugSpeed(void)
{
	unsigned char cReadKey;
	int iSV, iV, iTime, istate, iMenu, iValue;
	char cDispbuf[16];
	
	iSV = gl_iSVSpeed;
	iV = gl_iVSpeed;
	iTime = gl_iTimeAdd;

	strcpy(gl_cLcdDispBuf[0], "初始速度:");
	sprintf(cDispbuf, "%d", iSV);
	strcpy(gl_cLcdDispBuf[1], cDispbuf);
	lcd_refresh();
	iMenu = 0;
	istate = 1;
	iValue= iSV;
	while(istate)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (cReadKey)
		{
			case Key_F1:
				iMenu++;
				if(iMenu > 2)
					iMenu = 0;
				
				if(iMenu == 0)
				{
					iTime = iValue;
					iValue = iSV;
					strcpy(gl_cLcdDispBuf[0], "初始速度:");
					sprintf(cDispbuf, "%d", iSV);
					strcpy(gl_cLcdDispBuf[1], cDispbuf);
					lcd_refresh();
				}
				else if(iMenu == 1)
				{
					iSV = iValue;
					iValue = iV;
					strcpy(gl_cLcdDispBuf[0], "速度设置:");
					sprintf(cDispbuf, "%d", iV);
					strcpy(gl_cLcdDispBuf[1], cDispbuf);
					lcd_refresh();
				}
				else
				{
					iV = iValue;
					iValue = iTime;
					strcpy(gl_cLcdDispBuf[0], "加速时间:");
					sprintf(cDispbuf, "%d", iTime);
					strcpy(gl_cLcdDispBuf[1], cDispbuf);
					lcd_refresh();
				}
				break;

			case Key_F2:
				iValue += 100;
				if(iValue > 20000)
					iValue = 0;
				DebugMenuPara(iMenu, iValue);
				break;

			case Key_F3:
				iValue -= 100;
				if(iValue < 0)
					iValue = 20000;
				DebugMenuPara(iMenu, iValue);
				break;

			case Key_F4:
				iValue += 1000;
				if(iValue > 20000)
					iValue = 0;
				DebugMenuPara(iMenu, iValue);
				break;

			case Key_F5:
				iValue -= 1000;
				if(iValue < 0)
					iValue = 20000;
				DebugMenuPara(iMenu, iValue);
				break;
				
			case Key_Up:
				iValue++;
				if(iValue > 20000)
					iValue = 0;
				DebugMenuPara(iMenu, iValue);
				break;

			case Key_Down:
				iValue--;
				if(iValue < 0)
					iValue = 20000;
				DebugMenuPara(iMenu, iValue);	
				break;

			case Key_Right:
				iValue += 10;
				if(iValue > 20000)
					iValue = 0;
				DebugMenuPara(iMenu, iValue);
				break;

			case Key_Left:
				iValue -= 10;
				if(iValue < 0)
					iValue = 20000;
				DebugMenuPara(iMenu, iValue);
				break;

			case Key_Ok:
				if(iMenu == 0)
					iSV = iValue;
				else if(iMenu == 1)
					iV = iValue;
				else
					iTime = iValue;
				
				gl_iSVSpeed = iSV;
				gl_iVSpeed = iV;
				gl_iTimeAdd = iTime;
				Printf("SetV: iSV[%d] iV[%d] iT[%d]\n", gl_iSVSpeed, gl_iVSpeed, gl_iTimeAdd);
				istate = 0;
				break;

			case Key_Pause:
				istate = 0;
				break;
		}
	}
}
*/
	
void StopPrint(void)
{
	if(gl_cCutUpDownState != 0)
		CtrlUpCut();
	
	ClrFIFO;
	gl_cDelayFlag &= 0xB0;
	gl_iWaitCount = 0;
	gl_iWaitCount1 = 0;
	gl_iDelayCount = 0;
	gl_iEthnetRecBufPH = 0;
	gl_iEthnetRecBufPE = 0;
	gl_cFileReceive = 0;
	gl_cCommunicationMode = 0xff;
	gl_cInvertBufStatus = 0;
	gl_cCurPrintBuf = 0;
	gl_cRemainderBit = 16;
	gl_cCountFlag = 0;
	gl_cDelayFlag &= 0xb0;

	gl_cCutFileSendFlag = 0;
	gl_cCutWorkStatus = 0;
	gl_cPrintStatus = 0;
	gl_cMachineWorkStatus = 0;	// 20120827 
	
	gl_lCurrCutFileLength = 0;
	gl_MacRunState = 0;	//文件打印结束
	InitFlatPlotter();	
	gl_cStopWorkState = 0;
	Printf("gl_cSendPaperStar:%d, gl_cReacterStatus:%d\n", gl_cSendPaperStar, gl_cReacterStatus);
	gl_cReacterStatus = 0;
	gl_cSendPaperStar = 1;
}

void PauseRecover(void)
{
	char cWstate, cReadKey;
	char cPrintSta;
	int Daochushi = 0;

	WaitPrintEnd();
	//if(gl_MacRunState == 1)
	//	strcpy(gl_cLcdDispBuf[0], "暂停打印");
	//else
	//	strcpy(gl_cLcdDispBuf[0], "暂停切割");
	if(gl_cLanguageSelect == 0)
	{
		strcpy(gl_cLcdDispBuf[0], "PAUSE   停止");
		strcpy(gl_cLcdDispBuf[1], "OK      启动");
	}
	else
	{
		strcpy(gl_cLcdDispBuf[0], "PAUSE  STOP");
		strcpy(gl_cLcdDispBuf[1], "OK     START");
	}
	lcd_refresh();
	
	if(gl_MacRunState == 1)
	{
		gl_cDelayFlag |= 0x40;
		gl_iWaitCount = 0;
		gl_iWaitCount1 = 0;
		gl_iDelayCount = 0;
		ClrFIFO;
		cPrintSta = gl_cPrintStatus;
		Printf("\n=1==gl_cPrintStatus[%d] PE[%d] PH[%d]===\n", gl_cPrintStatus, gl_iEthnetRecBufPE, gl_iEthnetRecBufPH);	
		gl_cPrintStatus = 0x20;
	}
	
 
	
	if(gl_cCutUpDownState == 1)
		{
		    Daochushi=1;
		    //将刀抬起
			SetIOBit(O_CUTDOWN,C_CUTUP);
			Delay(gl_iCutUpDelay);
			gl_cCutUpDownState = 0;
		}
		
	
	cWstate = 1;
	while(cWstate)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		gl_cKeyPE = gl_cKeyPH;
		
		switch(cReadKey)
		{
			case Key_Pump:			//气泵吸附
				if((gl_cManualPumpSwitch&0x20) == 0x20)
					gl_cManualPumpSwitch &= 0xdf;
				else
					gl_cManualPumpSwitch |= 0x20;
				WriteIO(gl_cManualPumpSwitch);
				break;

			case Key_Drop:
				CutPressManage();		//修改笔属性6
				break;

			case Key_Speed:
				CutSpeedManage();	//切割速度
				break;
				
			case Key_Pause:
				if(gl_MacRunState == 1)
				{
				//	{//等待文件接收结束
					//gzr20160831 解开注释 解决取消打印后上位机无法删除文件
					//while((gl_cFileReceive&0x80)==0)//char 最高位为1时文件传输结束 
					//{ }
				//		while((gl_cFileReceive&0x80)==0)//char 最高位为1时文件传输结束 
				//		{ }
				//	}
					StopPrint();		
				}
				else
					gl_cStopWorkState = 1;
				cWstate = 0;
				break;

			case Key_Ok:

				if(Daochushi==1)
					{
				     //将刀落下
						SetIOBit(O_CUTDOWN,C_CUTDOWN);
						Delay(gl_iCutDownDelay);
						gl_cCutUpDownState = 1;
					}
			
				if(gl_MacRunState == 1)
				{
					Printf("\n=2==gl_cPrintStatus[%d] PE[%d] PH[%d]===\n", gl_cPrintStatus, gl_iEthnetRecBufPE, gl_iEthnetRecBufPH);
					gl_cPrintStatus = cPrintSta;
					gl_cDelayFlag &= 0xB0;
				}
				gl_cStopWorkState = 0;
				cWstate = 0;
				break;
		}

		if(gl_cLanguageSelect == 0)
		{
			strcpy(gl_cLcdDispBuf[0], "PAUSE   停止");
			strcpy(gl_cLcdDispBuf[1], "OK      启动");
		}
		else
		{
			strcpy(gl_cLcdDispBuf[0], "PAUSE  STOP");
			strcpy(gl_cLcdDispBuf[1], "OK     START");
		}
		lcd_refresh();
	}
	
}

void LanguageSelect(void)
{
	char cWstate, cReadKey;
	if(gl_cLanguageSelect == 0)
		strcpy(gl_cLcdDispBuf[0], "语言选择: ");
	else
		strcpy(gl_cLcdDispBuf[0], "Language: ");
	strcpy(gl_cLcdDispBuf[1], "F3  中文 F4 En");
	lcd_refresh();
	cWstate = 1;
	while(cWstate)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;

		switch(cReadKey)
		{
			case Key_F3:
				gl_cLanguageSelect = 0;
				WriteParameter();
				cWstate = 0;
				break;

			case Key_F4:
				gl_cLanguageSelect = 1;
				WriteParameter();
				cWstate = 0;
				break;

			case Key_Parameter:
				//查询版本
				VersionDisplay();
				break;
			
			case Key_Ok:
			case Key_Pause:
				cWstate = 0;
				break;
		}
	}
	
}


//功能：按键处理20100623
void KBProc(void)
{
	unsigned char cReadKey;
	unsigned char cKeyPE;
	//static char MenuState=0;//, cRunSta=0;
	//static unsigned char cPrintSta=0;
//CYC:
	// 文件已删除操作,暂时不用20100623
	//if ((gl_cFileReceive & 0x10) == 0x10)
	//{
	//	StopPrint();
	//}
	
	//Printf("KBProc **  %x!\n",gl_MacRunState);
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
				cReviceFlag = 1;
				return;
			}
			else 
				PauseRecover();
			if(gl_cLanguageSelect==0)
				LcdDisp("  欢迎使用", "富怡喷墨切割机 ");
			else
				LcdDisp("Welcome to use", "RICHPEACE CAD");
			return;
		}
		
		switch (cReadKey)
		{
			//新薄膜键盘功能定义
			case Key_F1:	// 一键机头移动到右上/右下
				if(gl_cKeyDirectPost != 1)
					gl_cKeyDirectPost = 1;	// 机头右上
				else 
					gl_cKeyDirectPost = 2;	// 机头右下
						
				ReturnDirectPost(gl_cKeyDirectPost);
				
				//DebugSpeed();
				break;

			case Key_F2:	// XY长度校正
				XYLengthReviseManage();	
				break;

			case Key_F3:	
				// 设置IP
				//NetSetManage();
				//喷刀间距设置
				CutDistanceManage();
				break;

			case Key_F4:	// 自动送纸设置
				RP_AutoMovPaper();
				break;
			
			case Key_Pump: //气泵吸附
				ManualSwtichPump(); 	//手动开关气泵 -> 气泵吸附设置
				break;

			case Key_Test: //打印测试图
				DrawTestGraphManage(); //打印测试图 -> 喷刀间距设置
				break;

			case Key_Drop: //刀压
				CutPressManage();     //刀压 -> 喷刀属性 -> 提落刀延时
				break;

			case Key_Speed: //速度设置
				CutSpeedManage();	 //切割速度设置 -> 移框速度设置
				break;

			case Key_Parameter: //参数设置
				OffsetDistanceManage();  //偏距设置 -> 提刀延生值 -> 转角提刀延伸值 -> 结束机头位置设置  -> 机型设置 
				break;
						
			case Key_Left:
			case Key_Right:
			case Key_Down:
			case Key_Up:     //原点设置
				RP_SetOrigin(1);				
				break;
			
			//case Key_Pause:
				//if ((gl_MacRunState != 0) || (gl_cPrintStatus == 0x20))
			//		PauseRecover();
			//	break;
			
		}
		if(gl_cLanguageSelect==0)
			LcdDisp("  欢迎使用", "富怡喷墨切割机 ");
		else
			LcdDisp("Welcome to use", "RICHPEACE CAD");
		cReviceFlag = 0;
	}
	
}
	

// 打印切割结束切断纸 20120906
void RunOverCutOffPaper(void)
{
	int iXMovPlus, iYMovPlus, iStatus;
	int iDTempX;
	// Y回到0，X走设定长度后，落刀，Y回原点感应器位置，提刀，回打印切割原点
	iXMovPlus = (iDTempX+gl_sOverCutOffset) * X_PPMM;
	iYMovPlus = -gl_lCurPost_YPluse;  //-(gl_iOriginalPos_Y * Y_PPMM);
	
	Printf("iDTempX[%d]+gl_sOverCutOffset[%d]=[%d]\n",iDTempX, gl_sOverCutOffset,iXMovPlus);
	//Printf("gl_iCurrentPos_Y[%d]+gl_iOriginalPos_Y[%d]=[%d]\n", gl_iCurrentPos_Y,gl_iOriginalPos_Y, iYMovPlus );
	if( (iXMovPlus > gl_iXMaxEffectivePMM) || (iYMovPlus < -gl_iYMaxEffectivePMM))
	{
		if (gl_cLanguageSelect == 0) {				//20141201 rwt
			LcdDisp("移动超出范围", "终止纸张切断");
		}
		else {
			LcdDisp("Move Extend", "TerminateCutOff");
		}
		//LcdDisp("移动超出范围","终止纸张切断");
		Delay(8000);
		return;
	}
	////////////////////////////
	//SetMovFrameSpeed(200);
	RP_SetSingleAxisADSpeed(AXIS_Y, 200); //20130807
	
	pmove(AXIS_Y,iYMovPlus);
	do
	{
		get_status(AXIS_Y,&iStatus);
	} while (iStatus != 0);
	
	Delay(2000);
	pmove(AXIS_X, iXMovPlus);
	do
	{
		get_status(AXIS_X,&iStatus);
	} while (iStatus != 0);

	Delay(2000);
	gl_cCutPenNum = 4;
	if (gl_cCutUpDownState == 0)
	{  //将刀具落下
	 	CtrlDownCut();
	}

	pmove(AXIS_Y,gl_lMaxY+(3*Y_PPMM));
	do
	{
		if (GetIOBit(I_YUPINDUCTOR) == StatusOk)
		{
			//pmove(AXIS_Y,0);
			SuddenStop(AXIS_Y);
			Printf("break Inductor\n");
			break;
		}
		get_status(AXIS_Y,&iStatus);
	} while (iStatus != 0);
	
	CtrlUpCut();
	gl_lCurPost_YPluse = gl_lMaxY;
	//gl_iCurrentPos_Y = gl_sMachineWidth;
	//Printf("gl_iOriginalPos_X[%d], gl_iOriginalPos_Y[%d]\n", gl_iOriginalPos_X,gl_iOriginalPos_Y);
	//Printf("====RunOverCutOffPaper() run over======\n");
}

/*
// 打印&切割结束，切断纸张 20120906
void OverCutOffPaperSet(void)
{
	char cReadKey, cModeSelect, cEnd, cTempOverState, cDispNum, cDispBuf[10];
	char dipLoc[2][5];
	int iCutOffset;

	iCutOffset = gl_sOverCutOffset;
	cTempOverState = gl_cOverCutOffPaper;
	strcpy(dipLoc[1], "否");
	strcpy(dipLoc[0], "是");
	
	if( cTempOverState == 0 )
		cDispNum = 1;//否
	else
		cDispNum = 0;//是
	LcdDisp( "是否自动切断纸",dipLoc[cDispNum]);// language[cDispNum]);

	cModeSelect = 0;
	cEnd = 1;
	while(cEnd)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		switch(cReadKey)
		{
			case Key_F1:
				cModeSelect = 0;
				cTempOverState = (cTempOverState == 0) ? 1 : 0;
				if( cTempOverState == 0 )
					cDispNum = 1;
				else
					cDispNum = 0;
				LcdDisp( "是否自动切断纸", dipLoc[cDispNum]);
				break;
				
			case Key_F2:
				cModeSelect = 1;
				sprintf(cDispBuf,"%d", iCutOffset);
				LcdDisp( "切断偏移距离" , cDispBuf);
				break;
				
			case Key_Up:
				if(cModeSelect == 1)
				{
					iCutOffset++;
					if( iCutOffset > 20 )
						iCutOffset = 5;
					sprintf(cDispBuf,"%d", iCutOffset);
					LcdDisp( "$$$", cDispBuf);
				}
				break;
				
			case Key_Down:
				if(cModeSelect == 1)
				{
					iCutOffset++;
					if( iCutOffset < 0 )
						iCutOffset = 20;
					sprintf(cDispBuf,"%d", iCutOffset);
					LcdDisp( "$$$", cDispBuf);
				}
				break;
				
			case Key_Ok:
				gl_sOverCutOffset =	iCutOffset;
				gl_cOverCutOffPaper	= cTempOverState;
				if(!WriteParameter())
					LcdDisp("写入参数失败", "请重试");//写入参数失败, 请重试
				cEnd = 0;			
				break;
			case Key_Pause:
				cEnd = 0;
				break;
		}
	}
}
*/
//提落刀延时 20120906
void PenUpDownDelay(void)
{
	char cReadKey, cMen, cModeSelect,tDispBuf[16];
	int iPenUpDelay, iPenDownDelay, iDelayTimes;
	iPenUpDelay = gl_iCutUpDelay/10;
	iPenDownDelay = gl_iCutDownDelay/10;
	Printf("Ini UpD[%d] DnD[%d]\n", iPenUpDelay, iPenDownDelay);
	sprintf(tDispBuf, "%d", iPenUpDelay);

	if(gl_cLanguageSelect == 0)
		LcdDisp( "提刀延时设置", tDispBuf);
	else
		LcdDisp( "Cut Up Delay", tDispBuf);
	iDelayTimes = iPenUpDelay;
	cModeSelect = 0;
	cMen = 1;
	while(cMen)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		switch(cReadKey)
		{
			case Key_F1:
				cModeSelect = (cModeSelect == 0) ? 1 : 0;
				if( cModeSelect == 0 )
				{// 提刀延时设置
					iPenDownDelay = iDelayTimes;
					iDelayTimes = iPenUpDelay;
					sprintf(tDispBuf, "%d", iPenUpDelay);
					if(gl_cLanguageSelect == 0)
						LcdDisp( "提刀延时设置", tDispBuf);
					else
						LcdDisp( "Cut Up Delay", tDispBuf);
					
				}
				else{
				//落刀延时设置
					iPenUpDelay = iDelayTimes;
					iDelayTimes = iPenDownDelay;
					sprintf(tDispBuf, "%d", iPenDownDelay);
					if(gl_cLanguageSelect == 0)
						LcdDisp("落刀延时设置", tDispBuf);
					else
						LcdDisp( "Cut Down Delay", tDispBuf);
				}
				break;
				
			case Key_Up:
				iDelayTimes += 10;
				if(iDelayTimes > 500)
					iDelayTimes = 0;
				if( cModeSelect == 0 )
				{
					sprintf(tDispBuf, "%d", iDelayTimes);
					lcd_display(1, tDispBuf);
				}
				else{
						sprintf(tDispBuf, "%d", iDelayTimes);
						lcd_display(1, tDispBuf);
					}
				break;
				
			case Key_Down:
				iDelayTimes -= 10;
				if(iDelayTimes < 0)
					iDelayTimes = 500;
				if( cModeSelect == 0 )
				{
					sprintf(tDispBuf, "%d", iDelayTimes);
					lcd_display(1, tDispBuf);
				}
				else{
						sprintf(tDispBuf, "%d", iDelayTimes);
						lcd_display(1, tDispBuf);
						
					}
				break;
				
			case Key_Left:
				iDelayTimes -= 50;
				if(iDelayTimes < 0)
					iDelayTimes = 500;
				if( cModeSelect == 0 )
				{
					sprintf(tDispBuf, "%d", iDelayTimes);
					lcd_display(1, tDispBuf);
				}
				else{
						sprintf(tDispBuf, "%d", iDelayTimes);
						lcd_display(1, tDispBuf);
						
					}
				break;
				
			case Key_Right:
				iDelayTimes += 50;
				if(iDelayTimes > 500)
					iDelayTimes = 0;
				if( cModeSelect == 0 )
				{
					sprintf(tDispBuf, "%d", iDelayTimes);
					lcd_display(1, tDispBuf);
				}
				else{
						sprintf(tDispBuf, "%d", iDelayTimes);
						lcd_display(1, tDispBuf);
						
					}
				break;

			case Key_Drop:
				CutPressManage();
				cMen = 0;
				break;
							
			case Key_Ok:
				if(cModeSelect == 0)
					iPenUpDelay = iDelayTimes;
				else
					iPenDownDelay= iDelayTimes;
				
				gl_iCutUpDelay  = iPenUpDelay * 10;
				gl_iCutDownDelay = iPenDownDelay * 10;
				Printf("UpDelay[%d] DownDelay[%d]\n", gl_iCutUpDelay, gl_iCutDownDelay);
				if(!WriteParameter()) {
					if (gl_cLanguageSelect == 0) {
						LcdDisp("写入参数失败", "请重试");//写入参数失败, 请重试
					}
					else {
						LcdDisp("Data Write Fail", "Retry");
					}
				}
				cMen = 0;
				break;
				
			case Key_Pause:
				cMen = 0;
				break;
		}
	}
}

/*
// 按键重复喷切
void PessKeyRepetPrint(void)
{
	char cReadKey;
	char cMen;
	cMen = 1;
	LcdDisp("是否执行喷切？", "");
	while(cMen)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		switch(cReadKey)
		{
			case Key_Ok:
				gl_cKeyStart = 1;
				cMen = 0;
				break;
			case Key_Pause:
				cMen = 0;
				break;
		}
			
	}	
}
*/
	/*

//按键回打印&切割原点	20120905
//如果当前在是暂停状态应提示用户是否回原点，否则直接回原点。
void KeyReOriginalPos(void)
{
	char cReadKey;
	char cRunFlag;
	cRunFlag = 0;
	if (gl_cMachineWorkStatus == 1)
	{
		LcdDisp("打印没有结束","是否继续回原点");
		while(1)
		{
			WaitKeyExit(0xff);		//等待有键按下
			cReadKey = gl_cKeyBuf[gl_cKeyPE];
			if ((gl_cKeyPE+1) == KeyBufLength)
				gl_cKeyPE = 0;
			else
				gl_cKeyPE++;
			if(cReadKey == Key_Ok)
				cRunFlag = 1;
			else if(cReadKey == Key_Pause)
				return;
		}		
	}

	if (cRunFlag == 1 || gl_cMachineWorkStatus == 0)
	{
		LcdDisp("回原点","请稍候。。。");
		ReturnOriginal(gl_lCurPost_XPluse, gl_lCurPost_YPluse);
	}
	
}
*/	

//********************
//气泵吸附设置20100623  dgj
void  DispPumpAdsorbMenu(char state)
{
	switch(state)
	{
		case 0x0:
			if(gl_cLanguageSelect == 0)
				LcdDisp("自动开气泵:OFF", "自动关气泵:OFF");//OFF, OFF
			else
				LcdDisp("Auto Open:   OFF", "Auto Close:  OFF");//OFF, OFF
			break;
		case 0x01:
			if(gl_cLanguageSelect == 0)
				LcdDisp("自动开气泵:ON", "自动关气泵:OFF");//ON, OFF
			else
				LcdDisp("Auto Open:   ON", "Auto Close:  OFF");//OFF, OFF
			break;
		case 0x02:
			if(gl_cLanguageSelect == 0)
				LcdDisp("自动开气泵:OFF", "自动关气泵:ON");//OFF, ON
			else
				LcdDisp("Auto Open:   OFF", "Auto Close:  ON");//OFF, OFF	
			break;
		case 0x03:
			if(gl_cLanguageSelect == 0)
				LcdDisp("自动开气泵:ON", "自动关气泵:ON");//ON, ON
			else
				LcdDisp("Auto Open:   ON", "Auto Close:  ON");//OFF, OFF	
			break;
	}
}

void  PumpAdsorbManage()
{
	unsigned char cReadKey;
	char  cPumpSwitch;
	char  state;
	//Printf("Init Pump 0: 0x%x\n", gl_cPumpSwitch);
	gl_cPumpSwitch &= 0x03;
	cPumpSwitch = gl_cPumpSwitch;
	//Printf("Init Pump 1: 0x%x\n", cPumpSwitch);
	DispPumpAdsorbMenu(cPumpSwitch);

	state = 1;
	while(state)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (cReadKey)
		{
			case Key_F1:
			case Key_F2:
				cPumpSwitch = ((cPumpSwitch&0x01) == 0x01) ? (cPumpSwitch & 0x02) : (cPumpSwitch | 0x01);
				DispPumpAdsorbMenu(cPumpSwitch);
				break;
			
			case Key_F3:
			case Key_F4:
				cPumpSwitch = ((cPumpSwitch&0x02) == 0x02) ? (cPumpSwitch & 0x01) : (cPumpSwitch | 0x02);
				DispPumpAdsorbMenu(cPumpSwitch);				
				break;
				
			case Key_Ok:
				gl_cPumpSwitch = cPumpSwitch;
				//Printf("Save Pump: %x\n", cPumpSwitch);
				if(!WriteParameter()) {
					if (gl_cLanguageSelect == 0) {
						LcdDisp("写入参数失败", "请重试");//写入参数失败, 请重试
					}
					else {
						LcdDisp("Data Write Fail", "Retry");
					}
				}
					//LcdDisp("写入参数失败", "请重试");//写入参数失败, 请重试
				state = 0;
				break;
				
			case Key_Pause:
				state = 0;
				break;
		}
	}
		
}
//***********************
/*
//删除当前文件
void DeleteCurFile(void)
{
	unsigned char ch;
	
	LcdDisp("删除当前任务否","等待操作");
	while (1)
	{
		WaitKeyExit(0xff);	//等待有按键
		ch = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		if (ch == Key_Ok)
		{
			gl_iEthnetRecBufPH = 0;
			gl_iEthnetRecBufPE = 0;
			gl_cFileReceive = 0;
			gl_cCommunicationMode = 0xff;
			gl_cInvertBufStatus = 0;
			gl_cCurPrintBuf = 0;
			gl_cRemainderBit = 16;
			gl_cCountFlag = 0;
			gl_DeleteFileState = 1;
			gl_MacRunState = 0;
			Printf("del gl_iPulseNumY:%d, gl_cPrintStatus:%x\n",gl_iPulseNumY ,gl_cPrintStatus);
			//判断刀是否落下 20120905 删除文件
			if (gl_cCutUpDownState == 1)
			{
				CtrlUpCut();
			}
			if ((gl_iPulseNumY < 0) && (gl_cPrintStatus != 0x10))
			{//喷头回原点
				LcdDisp("喷头回原点","请稍后......" );
				ClrFIFO;
				DriveSprinkler(0,gl_iPulseNumY);//驱动Y轴
				gl_iAxisYPos += gl_iPulseNumY;
				gl_iPulseNumY = -gl_iPulseNumY;
				WaitPrintEnd();
			}
			break;
		}
		else if (ch == Key_Pause)
			break;
		else
			continue;
	}	
}
*/
/*
void DrawTestGraph(void)
{
    unsigned char m,j;
    unsigned char cSendPosFlag;
    unsigned int address;
    unsigned int iYDotSpace[3];
    int i, idat1;
    int iPosNumY;
    int iTotalPixel;
    int sdat[4];
    int sdat1[4];

    //计算Y轴脉冲长度
    iTotalPixel = 300 * 8 + 2 * AdditionDot + OriginalSideDot + MotorSideDot;
    for (m=0;m<(gl_cSprinklerCount-1);m++)
	iTotalPixel = iTotalPixel + iYDotSpace[m];
    iPosNumY = (iTotalPixel + 320) * DotPulseNum;
    //判断Y轴是否会超界
    if (((gl_iAxisYPos + iPosNumY) < 0) || ((gl_iAxisYPos + iPosNumY) > gl_iYMaxEffectivePMM))
    {
	strcpy(gl_cLcdDispBuf[0],"超界");
	strcpy(gl_cLcdDispBuf[1],"退出");
	lcd_refresh();
	return ;
    }
    strcpy(gl_cLcdDispBuf[0],"测试图形打印");
    strcpy(gl_cLcdDispBuf[1],"请稍后...");
    lcd_refresh();
    while (ReadPrintEnd() == 0);

    WriteFilePrint(0xaaaa);
    ClrFIFO;

    WriteFIFOFull(gl_cSprinklerCount-1);
	// WriteFIFOFull(0);
    Printf("gl_cSprinklerCount:%d\n", gl_cSprinklerCount);
    sdat[0] = AdditionDot + OriginalSideDot;//320
    sdat1[0] = AdditionDot + OriginalSideDot + 300 * 8;//2620

    sdat[1] = AdditionDot + OriginalSideDot;
    sdat1[1] = AdditionDot + OriginalSideDot + 300 * 8;

    i = 0;
    cSendPosFlag = 0;
	//
    //写空走附加段
     idat1 =	AdditionDot + OriginalSideDot; 
     for(i=0; i<idat1; i++) 
     {
         while (ReadFIFOFull() != 0)	//缓冲满
         {
             if (cSendPosFlag == 0)
             {
                 Printf("Print start 0 \n");
                 DriveSprinkler(0,iPosNumY);
                 iPosNumY = -iPosNumY;
                 cSendPosFlag = 1;
             }
             else if(ReadPrintEnd() != 0)
                 break;
         }
         address = gl_iWriteFIFOAddr;
         for (j=0;j<14;j++)
         {
             WriteFIFO(address,0);
             address += 4;
          }
     }
   //

    //i = 0;
    while (1)
    {
	if (i >= iTotalPixel)
	    break;

	while (ReadFIFOFull() != 0)	//缓冲满
	{
	    if (cSendPosFlag == 0)
	    {
		Printf("Print start 1 \n");
		DriveSprinkler(0,iPosNumY);
		iPosNumY = -iPosNumY;
		cSendPosFlag = 1;
	    }
	    else if(ReadPrintEnd() != 0)	//驱动结束
		break;
	}

	if ((cSendPosFlag == 1) && (ReadPrintEnd() != 0))
	    break;		

	address = WriteFIFOAddr;//gl_iWriteFIFOAddr;

	if(i%320 == 0) {
		for(idat1=0; idat1<2; idat1++)
		{
			while (ReadFIFOFull() != 0)
			{
			 if(ReadPrintEnd() != 0)	//驱动结束
				break;
			}
			//if ((cSendPosFlag == 1) && (ReadPrintEnd() != 0))
	  		//	  break;
			address = WriteFIFOAddr;//gl_iWriteFIFOAddr;
	    	for (j=0;j<14;j++)
	    	{
				WriteFIFO(address,0x3ffffff);
				address += 4;
	    	}
			//
			for (j=0;j<7;j++)
	    	{
				WriteFIFO(address,0x3ffffff);
				address += 4;
	    	}	//
			//i++;
		}
	}
	//else if(i%320 == 0) {
		for(idat1=0; idat1<2; idat1++)
		{
			while (ReadFIFOFull() != 0)
			{
			 if(ReadPrintEnd() != 0)	//驱动结束
				break;
			}
			//if ((cSendPosFlag == 1) && (ReadPrintEnd() != 0))
	  		//	  break;
			address = WriteFIFOAddr+28;//gl_iWriteFIFOAddr;
	    	for (j=0;j<7;j++)
	    	{
				WriteFIFO(address,0x3ffffff);
				address += 4;
	    	}
			//
			for (j=0;j<7;j++)
	    	{
				WriteFIFO(address,0x3ffffff);
				address += 4;
	    	}	//
			//i++;
		}
	}//
	else {
	    for (j=0;j<14;j++)
	    {
		WriteFIFO(address,0x0);
		address += 4;
	    }
	}

       //
	for (m=0;m<gl_cSprinklerCount;m++)
	 {
	     //Printf("Start addr[0x%x]", address);
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
	      //Printf("End addr[0x%x]", address);
	  }
       //

	i++;

	if ((cSendPosFlag == 1) && (ReadPrintEnd() != 0))
	    break;
    }
    WaitDriveEnd(AXIS_Y);
    Printf("Print end\n");
    Delay(3000);
    ClrFIFO;		//清FIFO缓冲区
    DriveSprinkler(0,iPosNumY);
    WaitDriveEnd(AXIS_Y);

}
*/
/*
void DrawTestGraph(void)
{
	unsigned char m,j;
	unsigned char cSendPosFlag;
	unsigned int address;
	unsigned int iYDotSpace[3];
	int i, idat1,delay;
	int iPosNumY;
	int iTotalPixel;
	int sdat[4];
	int sdat1[4];

	//计算Y轴脉冲长度
	iTotalPixel = 30 * 8 + 2 * AdditionDot + OriginalSideDot + MotorSideDot;
	for (m=0;m<(gl_cSprinklerCount-1);m++)
		iTotalPixel = iTotalPixel + iYDotSpace[m];
	iPosNumY = (iTotalPixel + 320) * DotPulseNum;
	//判断Y轴是否会超界
	if (((gl_iAxisYPos + iPosNumY) < 0) || ((gl_iAxisYPos + iPosNumY) > gl_iYMaxEffectivePMM))
	{
		strcpy(gl_cLcdDispBuf[0],"超界");
		strcpy(gl_cLcdDispBuf[1],"退出");
		lcd_refresh();
		return ;
	}
	strcpy(gl_cLcdDispBuf[0],"测试图形打印");
	strcpy(gl_cLcdDispBuf[1],"请稍后...");
	lcd_refresh();
	while (ReadPrintEnd() == 0);

	WriteFilePrint(0xaaaa);
	ClrFIFO;

	WriteFIFOFull(gl_cSprinklerCount-1);

	Printf("gl_cSprinklerCount:%d\n", gl_cSprinklerCount);
	sdat[0] = AdditionDot + OriginalSideDot;//320
	sdat1[0] = AdditionDot + OriginalSideDot + 300 * 8;//2620
	
	sdat[1] = sdat[0] + gl_iYDotSpace12;
	sdat1[1] = sdat1[0] + gl_iYDotSpace12;
	Printf("sdat[0]:%d, sdat[1]:%d, sdat1[0]:%d, sdat1[1]:%d\n", sdat[0],sdat[1],sdat1[0],sdat1[1]);
	i = 0;
	cSendPosFlag = 0;
	idat1 = 0;
	delay = 0;

	while (1)
	{
		if (i >= iTotalPixel)
			break;
		while (ReadFIFOFull() != 0)	//缓冲满
		{
			delay++;
			if (cSendPosFlag == 0)
			{
				Printf("Print start 1 \n");
				DriveSprinkler(0,iPosNumY);
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
			//Printf("Start addr[0x%x]", address);
			
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
				//if(address == gl_iWriteFIFOAddr+28)
				//	idat1++;
				//if( i == sdat[m])
				Printf("L m[%d],addr[0x%x],i[%d], sdat[%d]/", m, address, i, sdat[m]);
				
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
				Printf("sdat[%d]=%d,", m, sdat[m]);
			}
			//Printf("End addr[0x%x]", address);
		}
		
		i++;
		if ((cSendPosFlag == 1) && (ReadPrintEnd() != 0))
			break;
	}
	WaitDriveEnd(AXIS_Y);
	Printf("\nPrint end line:%d, delay:%d\n", idat1, delay);
	Delay(3000);
	ClrFIFO;		//清FIFO缓冲区
	DriveSprinkler(0,iPosNumY);
	WaitDriveEnd(AXIS_Y);
	
}
*/
// 20130317 打印测试图形，测试完一行向前走X 
static long iToalPosNumX;

void DrawTestGraphManage(void)
{
	char state, cReadKey;
	//long iToalXPulse = 0;
	iToalPosNumX = 0;

	if(gl_cLanguageSelect == 0)
	{
		strcpy(gl_cLcdDispBuf[0],"打印测试图形");
		strcpy(gl_cLcdDispBuf[1],"OK :打印");
	}
	else
	{
		strcpy(gl_cLcdDispBuf[0],"Print Test");
		strcpy(gl_cLcdDispBuf[1],"OK :Print");
	}
	lcd_refresh();
	
	state = 1;
	while(state)
	{
		WaitKeyExit(0xff);		//等待有键按下
		
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (cReadKey)
		{
		/*
			case Key_Test://喷刀间距设置
				CutDistanceManage();
				state = 0;
				break;
		*/
			case Key_Ok:
				ReturnOriginal(gl_lCurPost_XPluse, gl_lCurPost_YPluse);
				DrawTestGraph();
				if(gl_cLanguageSelect == 0)
					lcd_display(1, "Pause:退出"); 
				else
					lcd_display(1, "Pause:EXIT"); 
				break;
				
			case Key_Pause:
				//SetAxisPrintSpeed(AXIS_X, gl_iPrintSpeed);
				//StraightLineADSpeed(AXIS_X, gl_iSVSpeed, gl_iVSpeed, gl_iTimeAdd);
				SetReturnOrgSpeed();
				pmove(AXIS_X, -iToalPosNumX);
				WaitDriveEnd(AXIS_X);
				state = 0;
				break;
		}
		
	}
}

/*
//打印测试图形(测试喷嘴好坏)
void  DrawTestGraph(void)
{
	unsigned int address;
	unsigned int i,j,k;
	unsigned int icount;
	int iPosNumY;
	int iGapPixel;
	int iTotalX=0;
	int iStatus=0;
	unsigned char cSprinklerNum;
	unsigned int sdat[7];
	
	char tBidirectional; 
	long lCount=0;
	
	
	strcpy(gl_cLcdDispBuf[0],"测试图形打印");
	strcpy(gl_cLcdDispBuf[1],"请稍后...");
	lcd_refresh();

	ClearPrintHead();
	
	k=0;
	
	tBidirectional = gl_cBidirectional;
	gl_cBidirectional = 1;
	
	iGapPixel = (int)(30.0 * Y_PPMM / DotPulseNum);
	iPosNumY = ((15 * iGapPixel+320+3) * DotPulseNum);
	
	//gl_iGapPixel = iPosNumY;//15 * iGapPixel * DotPulseNum;
	gl_scankeyflag	= 0;
	Printf("MAXPLUS[%d], IY[%d], IGP[%d]\n", gl_iYMaxEffectivePMM, iPosNumY, gl_iGapPixel);
	if (((gl_iAxisYPos + iPosNumY) < 0) || ((gl_iAxisYPos + iPosNumY) > gl_iYMaxEffectivePMM))
	{
		strcpy(gl_cLcdDispBuf[1]," ");
		strcpy(gl_cLcdDispBuf[0]," Y轴移动超界!");
		lcd_refresh();
		WaitKeyExit(Key_Pause);
		return;
	}

	WriteFilePrint(0xaaaa);
	ClrFIFO;
	
	Printf("spkcount:[%d],gl_cPrintStatus[%d]\n", gl_cSprinklerCount, gl_cPrintStatus);
	//iPosNumX = (int)((float)SprinklerWidth * DotLineWidth * X_PPMM);//(unsigned int)(50.0 * DotLineWidth * X_PPMM);
	//Printf("iPosNumX[%d]\n",iPosNumX);

	//WriteFIFOFull(0);
	for (cSprinklerNum=0;cSprinklerNum<gl_cSprinklerCount;cSprinklerNum++)
	{       
		Printf("S1:[%d]\n", gl_cSprinklerNo+cSprinklerNum);
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
				WriteFIFO(address,0x3fffff);
				address += 4;
			}
		}
		//Printf("S2\n");
		while (ReadFIFOFull() == 0)//缓冲区不满
		{
			address = gl_iWriteFIFOAddr + 28 * cSprinklerNum;
			for (j=0;j<7;j++)
			{//画边框
				WriteFIFO(address,sdat[j]);
				address += 4;
			}
			i++;
		}
		Printf("Y Print 1...fifoAddr[0x%x]\n", address);
		Printf("i[%d],iGapPixel[%d],iPosNumY[%d]\n ", i, iGapPixel, iPosNumY);
		DriveSprinkler(0,iPosNumY);

		gl_iAxisYPos += iPosNumY;
		iPosNumY = -iPosNumY;

////////////////////////////////////////
		for (icount=0;icount<15;icount++)
		{
			if (ReadPrintEnd() != 0)
			{
				Printf("A0\n");
				break;
			}
			
			for (;i<iGapPixel;i++)
			{
				while (ReadFIFOFull() != 0)
				{//缓冲区满
					if (ReadPrintEnd() != 0)
					{
						Printf("A1\n");
						break;
					}
					++k;
				}

				if (ReadPrintEnd() != 0)
				{
					Printf("A2\n");
					break;
				}
				
				address = gl_iWriteFIFOAddr + 28 * cSprinklerNum;
				for (j=0;j<7;j++)
				{
					WriteFIFO(address, sdat[j]);
					address += 4;
					lCount++;
				}
			}

			for (i=0;i<3;i++)
			{
				while (ReadFIFOFull() != 0)
				{//缓冲区满
					if (ReadPrintEnd() != 0)
					{
						iStatus = 1;
						Printf("A3\n");
						break;
					}
					++k;
				}

				if (ReadPrintEnd() != 0)
				{
					Printf("A4\n");
					break;
				}
				address = gl_iWriteFIFOAddr + 28 * cSprinklerNum;
				for (j=0;j<7;j++)
				{
					WriteFIFO(address,0x3fffff);
					address += 4;
					lCount++;
				}
			}
		
		}
	
/////////////////////////////
		Printf("icount:%d, k= %d lCount:%d\n", icount, k, lCount);
		//WaitDriveEnd(AXIS_Y);
		while(ReadPrintEnd() == 0);
		Delay(1000);
		
		//DriveSprinkler(iPosNumX,0);
		//iTotalX += iPosNumX;
		//WaitDriveEnd(AXIS_X);
		//while(ReadPrintEnd() == 0);

		//Delay(1000);
		Printf("Y Drive org2[%d]\n",cSprinklerNum);
	}
	Printf("Print end icount[%d] iTotalX[%d]\n", icount, iTotalX);
//	CtrlUpCut();
	//Printf("iPY:%d\n",iPosNumY);

	if(iPosNumY<0)
	{
		DriveSprinkler(0,iPosNumY);
		gl_iAxisYPos += iPosNumY;
		WaitDriveEnd(AXIS_Y);
	}
	
	Delay(2000);
	iToalPosNumX += gl_iPulseNumX;
	if(iToalPosNumX < (gl_iXMaxEffectivePMM-gl_iPulseNumX))
	{
		DriveSprinkler(gl_iPulseNumX,0);
		WaitDriveEnd(AXIS_X);
	}
	else
		iToalPosNumX -= gl_iPulseNumX;
	gl_scankeyflag	= 1;
	gl_cBidirectional = tBidirectional;
	return ;
}
*/

//打印测试图形(测试喷嘴好坏)
void  DrawTestGraph(void)
{ 
	unsigned int address;
	unsigned int i,j,k;
	unsigned int icount;
	int iPosNumY;
	int iGapPixel;
	int iTotalX=0;
	int iStatus=0;
	unsigned char cSprinklerNum;
	unsigned int sdat[7];
	long lCount = 0;
	char tBidirectional; 

	if(gl_cLanguageSelect == 0)
		LcdDisp("测试图形打印", "请稍后......");
	else
		LcdDisp("TestChart Print", "Please Wait...");

	PrintHeadClean(1);
	Delay(200);
	k=0;
	
	tBidirectional = gl_cBidirectional;
	gl_cBidirectional = 1;
	
	iGapPixel = (int)(30.0 * Y_PPMM / DotPulseNum);
	iPosNumY = ((15 * iGapPixel+320+3) * DotPulseNum);
	gl_iYMaxEffectivePMM = 1800 * Y_PPMM;
	gl_iXMaxEffectivePMM = 1800 * X_PPMM;
	gl_lCurPost_YPluse = 0;
	gl_lCurPost_XPluse = 0;
	//gl_iGapPixel = iPosNumY;//15 * iGapPixel * DotPulseNum;
	gl_scankeyflag	= 0;
	Printf("MAXPLUS[%d], IY[%d], IGP[%d]\n", gl_iYMaxEffectivePMM, iPosNumY, gl_iGapPixel);
	//判断Y轴是否会超界
	if (((gl_lCurPost_YPluse + iPosNumY) > gl_iYMaxEffectivePMM) || ((gl_lCurPost_XPluse + gl_iPulseNumX) > gl_iXMaxEffectivePMM))
	{
		lcd_display(0,"越界请重新定位");
		lcd_display(1,"任意键继续");
		WaitKeyExit(0xff);		//等待有键按下
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		return ;
	}	

	PrintHeadClean(1);
	WriteFilePrint(0xaaaa);
	ClrFIFO;
	
	Printf("spkcount:[%d],gl_cPrintStatus[%d]\n", gl_cSprinklerCount, gl_cPrintStatus);
	//iPosNumX = (int)((float)SprinklerWidth * DotLineWidth * X_PPMM);//(unsigned int)(50.0 * DotLineWidth * X_PPMM);
	//Printf("iPosNumX[%d]\n",iPosNumX);

	//WriteFIFOFull(0);
	for (cSprinklerNum=0;cSprinklerNum<gl_cSprinklerCount;cSprinklerNum++)
	{       
		Printf("S1:[%d][%d]\n", gl_cSprinklerNo, cSprinklerNum);
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
				WriteFIFO(address,0x3fffff);
				address += 4;
			}
		}
		
		while (ReadFIFOFull() == 0)//缓冲区不满
		{
			address = gl_iWriteFIFOAddr + 28 * cSprinklerNum;
			for (j=0;j<7;j++)
			{//画边框
				WriteFIFO(address,sdat[j]);
				address += 4;
			}
			i++;
		}
		Printf("Y Print 1...fifoAddr[0x%x]\n", address);
		Printf("i[%d],iGapPixel[%d],iPosNumY[%d]\n ", i, iGapPixel, iPosNumY);
		DriveSprinkler(0,iPosNumY);

		gl_iAxisYPos += iPosNumY;
		iPosNumY = -iPosNumY;

////////////////////////////////////////
		for (icount=0;icount<15;icount++)
		{
			if (ReadPrintEnd() != 0)
			{
				//Printf("A0\n");
				break;
			}
			
			for (;i<iGapPixel;i++)
			{
				while (ReadFIFOFull() != 0)
				{//缓冲区满
					if (ReadPrintEnd() != 0)
					{
						break;
					}
					++k;
				}
				
				if (ReadPrintEnd() != 0)
					break;
				
				address = gl_iWriteFIFOAddr + 28 * cSprinklerNum;
				for (j=0;j<7;j++)
				{
					WriteFIFO(address, sdat[j]);
					address += 4;
					lCount++;
				}
			}
			
			for (i=0;i<3;i++)
			{
				while (ReadFIFOFull() != 0)
				{//缓冲区满
					if (ReadPrintEnd() != 0)
					{
						iStatus = 1;
						break;
					}
					++k;
				}

				if (ReadPrintEnd() != 0)
					break;
				
				address = gl_iWriteFIFOAddr + 28 * cSprinklerNum;
				for (j=0;j<7;j++)
				{
					WriteFIFO(address,0x3fffff);
					address += 4;
					lCount++;
				}
			}
			
		}
	
/////////////////////////////
		//WaitDriveEnd(AXIS_Y);
		while(ReadPrintEnd() == 0);
		Printf("\nicount:%d, k= %d  lCount:%d\n", icount, k, lCount);
		k=0;
		lCount=0;
		Delay(5000);
		
		//DriveSprinkler(iPosNumX,0);
		//iTotalX += iPosNumX;
		//WaitDriveEnd(AXIS_X);
		//while(ReadPrintEnd() == 0);

		//Delay(1000);
		Printf("Y Drive org2[%d]\n",cSprinklerNum);
	}
	Printf("Print end icount[%d] iTotalX[%d]\n", icount, iTotalX);
//	CtrlUpCut();
	Printf("iPY:%d\n",iPosNumY);

	if(iPosNumY<0)
	{
		DriveSprinkler(0,iPosNumY);
		gl_iAxisYPos += iPosNumY;
		WaitDriveEnd(AXIS_Y);
	}
	Printf("Drive X go=====\n");
	//Delay(2000);
	iToalPosNumX += gl_iPulseNumX;
	if(iToalPosNumX < (gl_iXMaxEffectivePMM-gl_iPulseNumX))
	{
		DriveSprinkler(gl_iPulseNumX,0);
		WaitDriveEnd(AXIS_X);
	}
	else
		iToalPosNumX -= gl_iPulseNumX;
	gl_scankeyflag	= 1;
	gl_cBidirectional = tBidirectional;
	Printf("Test end=====\n");
	return ;
}


void PrintDrive(int iPulseNumY)
{
	if(iPulseNumY != 0)
	{
		if(gl_cBidirectional == 1)
		{
			if(iPulseNumY < 0)
			{
				WriteDirct(0xaaaa);//打印反向
				Printf("-");
			}
			else
			{
				WriteDirct(0x5555);//打印正向
				Printf("+");
			}
		}
		else
		{
			if(iPulseNumY > 0)
			{
				WriteDirct(0x5555);//打印正向
				//Delay(50);
				Printf("+");
			}
		}
			
		Write312YAxisPlus(iPulseNumY);// 408
			
		//StraightLineADSpeed(AXIS_Y, gl_iSVSpeed, gl_iVSpeed, gl_iTimeAdd);
		//SetPrintSpeed(AXIS_Y);
		StraightLineADSpeed(AXIS_Y, Print_SV, Print_V, Print_T+300);
		if(iPulseNumY < 0)
			iPulseNumY += -3000;
		else
			iPulseNumY += 3000;
		//Printf("AA:YPlus[%d]\n", iPulseNumY);
		pmove(AXIS_Y, iPulseNumY);
					
		while (ReadPrintEnd() != 0);
	}

}

//打印测试图
#define COUNTGRPPIXS(len, axis) ((len) * (axis) / DotPulseNum)
void PrintTestGraph(int i_scale)
{ 
	unsigned char m,j;
	unsigned char cSendPosFlag;
	unsigned int address;
	int i;// iStatus;
	int iPosNumY; // Y总脉冲数
	int iTotalPixel; // 打印总像素
	int spurtHeadStartPrtPix[2], spurtHeadEndtPrtPix[2]; //喷头1,2开始/结束像素点；0: 喷头1  , 1: 喷头2
	int lineWidth; //打印线宽
	int iGapPixel; // 图形像素点数
	//int iOffsetPixel; 
	
	char tBidirec;
	//int iStatus;
	//long lTempYPluse;
	
	// 2*2cm 正方形数据 
	unsigned int ptdataA[7] = {0xffffff, 0xffffff, 0xffffff, 0xffffff, 0x03ff, 0x0, 0x0};
	unsigned int ptdataB[7] = {0x0, 0x0, 0x0, 0x0, 0x300, 0x0, 0x0};

	PrintHeadClean(1);

	tBidirec = gl_cBidirectional;
	
	lineWidth = 2;
	
	//计算Y轴脉冲长度
	iGapPixel = (int)COUNTGRPPIXS(20, Y_PPMM);
	//iOffsetPixel = (int)COUNTGRPPIXS(i_scale*25, Y_PPMM);	// Y向偏移多少个像素点后打印
	iTotalPixel = iGapPixel+2 * AdditionDot + OriginalSideDot + MotorSideDot;// + iOffsetPixel;

	//Printf("TPX1: %d, i_scale[%d]\n", iTotalPixel, i_scale);
	iPosNumY = (iTotalPixel) * DotPulseNum;// + 220
	//Printf("PNY 1: %d\n", iPosNumY);

	//判断Y是否超界 20120905
	gl_iYMaxEffectivePMM = gl_lMaxY - gl_lWorkOrig_YYPluse; //((gl_iOriginalPos_Y+10) * Y_PPMM);
	gl_iPulseNumX = (unsigned int)(((float)(SprinklerWidth * 2 - gl_cMultiDotLine12 + 200) * DotLineWidth) * X_PPMM);
	if(iPosNumY > gl_iYMaxEffectivePMM)
	{
		DriveSprinkler(-gl_iPulseNumX,0);//驱动X轴走纸
		gl_lAxisXPos += gl_iPulseNumX;
	}
	
	
	//刷新显示
	if(gl_cLanguageSelect == 0)
		LcdDisp("测试图形打印", "请稍后......");
	else
		LcdDisp("TestChart Print", "Please Wait...");
	//等待打印是否结束
	WaitPrintEnd();
	
	WriteFilePrint(0xaaaa);
	WriteDirct(0x5555);//打印正向
	ClrFIFO;
	
	WriteFIFOFull(gl_cSprinklerCount-1);
	//Printf("FIFOADDR:0x%x, %d\n",gl_iWriteFIFOAddr,gl_cSprinklerCount);
	//Printf("iGapPixel:%d\n",iGapPixel);

	spurtHeadStartPrtPix[0] = AdditionDot + OriginalSideDot; // + iOffsetPixel;//喷头1开始像素点
	spurtHeadStartPrtPix[1] = spurtHeadStartPrtPix[0]  + SprinklerSpace;

	spurtHeadEndtPrtPix[0] = spurtHeadStartPrtPix[0] + iGapPixel;//喷头1结束像素点
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
		{//  m:0 喷头1数据  m:1 喷头2数据
			while (ReadFIFOFull() != 0)
			{
				if (cSendPosFlag == 0)
				{
					//DriveSprinkler(0,iPosNumY);
					PrintDrive(iPosNumY);
					cSendPosFlag = 1;
					//Printf("Start Printf! i:%d\n", i);
				}
				
				if (ReadPrintEnd() != 0)
					break;
			}

			if((1==cSendPosFlag) && (ReadPrintEnd() != 0))
				break;

			if ((i > spurtHeadEndtPrtPix[m]+lineWidth) || (i < spurtHeadStartPrtPix[m]))
			{//空走
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
					{// 左线
						WriteFIFO(address,0x3ffffff);
					}
					else
					{// 右线
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
					{//  上线
						if(j == 0)
							WriteFIFO(address,0x03);
						else
							WriteFIFO(address, 0);
					}
					else
					{// 下线
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

	//Printf("PNY 2: %d  i:%d\n", iPosNumY, i);
	
	//DriveSprinkler(0,-iPosNumY);
	PrintDrive(-iPosNumY);
	WaitDriveEnd(AXIS_Y);
	
	gl_cBidirectional = tBidirec ;
	 
	return;
}

void CutTestGraph(int i_scale)
{
	int iStatus;
	int i_dYProp, i_dXProp;
	int iPosNumX, iPosNumY;
	int CurOrigPosX, CurOrigPosY;
	int iCutOffset, iExtOffset;
	long lTempYPluse, lTempXPluse;
	
	i_dYProp = 1;
	i_dXProp = 1;
	//20120905 偏移位置开始执行切割
	CurOrigPosY = (int)((i_scale*25) * Y_PPMM) +  (gl_iCutDistanceY * DotPulseNum);
	CurOrigPosX = (int)(gl_iCutDistanceX * DotPulseNum);
	//判断当前位置是否能切割完一个正方形 20120905
	if ( (CurOrigPosY+25*Y_PPMM) >  gl_lMaxY )
		CurOrigPosX += (i_scale * 30 * X_PPMM);
	if ( (CurOrigPosX+(i_scale * 30 * 2 * X_PPMM)) > gl_lMaxX)
	{
		if (gl_cLanguageSelect == 0) {
			LcdDisp("移动超界", "请重新选择原点");
		}
		else {
			LcdDisp("Move Extend", "Reselect Origin");
		}
		//LcdDisp("移动超界", "请重新选择原点");
		Delay(5000);
		return;
	}
	////////////////////////////////
	
	iPosNumX = (int)(20 * X_PPMM);//(int)(800 * gl_dPosUnitX);
	iPosNumY = (int)(20.2 * Y_PPMM);//(int)(800 * gl_dPosUnitY);
	// 12236.90642
	Printf("Cut test graph!CX:%d CY:%d\n", gl_iCutDistanceX, gl_iCutDistanceY);
	Printf("X[%d],Y[%d],PU[%d]\n", iPosNumX, iPosNumY,(int)gl_dPosUnitX );
	if (gl_cLanguageSelect == 0) {
		LcdDisp("测试图形切割", "请稍后......");
	}
	else {
		LcdDisp("Test Chart Cut", "Please Wait...");
	}
	//LcdDisp("测试图形切割", "请稍后......");

//提刀刀至落刀点
	if (gl_cCutUpDownState == 1)
	{
		CtrlUpCut();
	}
	
	iCutOffset = gl_iCutOffset*gl_dPosUnitX;
	iExtOffset = gl_dCutExtensValue*gl_dPosUnitX;
	Printf("Coff[%d] iEof[%d]\n", iCutOffset, iExtOffset);
	
	//SetMoveFrameSpeed(AXIS_X+AXIS_Y);
	SingleAxisInpDriveSpeed(AXIS_X+AXIS_Y, Frame_SV, 200, 800);
	inp_move2(CurOrigPosX,CurOrigPosY-DowCutOffset*gl_dPosUnitY);
	do
	{
		get_inp_status(&iStatus);
	} while (iStatus != 0);	
	Delay(500);
//落刀
	if (gl_cCutUpDownState == 0)
	{  //将刀具落下
		WaitPrintEnd();
		CtrlDownCut();
	}
	
	//设置短剧行程两轴直线插补驱动速度
	//RP_SetDoubleAxisADSpeed(100);
	//StraightLineADSpeed(AXIS_Y, gl_iSVSpeed, gl_iVSpeed, gl_iTimeAdd);
	StraightLineADSpeed(AXIS_Y, 10, 50, 200);
	lTempXPluse = iPosNumY*i_dYProp+iCutOffset;
	lTempYPluse = iPosNumX*i_dXProp+iCutOffset;
	//********
	pmove(AXIS_Y,  lTempYPluse+DowCutOffset*gl_dPosUnitX);
	do
	{
		get_status(AXIS_Y, &iStatus);
	} while (iStatus != 0);
	Delay(50);
	
	pmove(AXIS_Y,  -iCutOffset);
	do
	{
		get_status(AXIS_Y, &iStatus);
	} while (iStatus != 0);
	
	Delay(500);
	//*********

	if(gl_cSetPenAttr[gl_cCutPenNum] == 1) // 20131208
	{// 转角提刀
		CornerGriffe(0,iPosNumY, iPosNumX, iPosNumY);
	}	
	
	//*********
	//X2Y2 -> X3Y3
	StraightLineADSpeed(AXIS_X, 10, 50, 200);
	pmove(AXIS_X, lTempXPluse);
	do
	{
		get_status(AXIS_X, &iStatus);
	} while (iStatus != 0);
	Delay(50);
	
	pmove(AXIS_X, -iCutOffset);
	do
	{
		get_status(AXIS_X, &iStatus);
	} while (iStatus != 0);
	Delay(500);

	if(gl_cSetPenAttr[gl_cCutPenNum] == 1)
	{// 转角提刀
		CornerGriffe(iPosNumX, iPosNumY, iPosNumX, 0);
	}	
	
	//X3Y3 -> X4Y4
	StraightLineADSpeed(AXIS_Y, 10, 50, 200);
	pmove(AXIS_Y, -lTempYPluse );
	do
	{
		get_status(AXIS_Y, &iStatus);
	} while (iStatus != 0);
	Delay(50);
	pmove(AXIS_Y,  iCutOffset);
	do
	{
		get_status(AXIS_Y, &iStatus);
	} while (iStatus != 0);
	Delay(500);

	if(gl_cSetPenAttr[gl_cCutPenNum] == 1)
	{// 转角提刀
		CornerGriffe(iPosNumX, 0, 0, 0 );
	}
	
	//X4Y4 -> X1Y1
	StraightLineADSpeed(AXIS_X, 10, 50, 200);
	pmove(AXIS_X, -(iPosNumX+iExtOffset));
	do
	{
		get_status(AXIS_X, &iStatus);
	} while (iStatus != 0);
	Delay(50);
	pmove(AXIS_X, iExtOffset);
	do
	{
		get_status(AXIS_X, &iStatus);
	}while (iStatus != 0);
	Delay(500);

//收刀
	if (gl_cCutUpDownState != 0)
	{  //将刀具提起
	 	CtrlUpCut();
	}
	Delay(1000);
	//回原点	
	SetMoveFrameSpeed(AXIS_X+AXIS_Y);
	inp_move2(-CurOrigPosX,-CurOrigPosY);
	do
	{
		get_inp_status(&iStatus);
	} while (iStatus != 0);	
	
}

//*******************
//偏距设置:补偿转角角度  没用到
void OffsetDistanceManage()
{
	char cDispTemp[10];
	unsigned char cReadKey;
	char  state;
	int  iCutOffset;

	sprintf(cDispTemp, "%d", gl_iCutOffset);
	if(gl_cLanguageSelect == 0)
		LcdDisp("偏距设置:", cDispTemp);
	else
		LcdDisp("Offset:", cDispTemp);
	iCutOffset = gl_iCutOffset;
	
	//Printf("Init CutOffset: %d\n", iCutOffset);
	
	state = 1;
	while(state)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (cReadKey)
		{
			case Key_Up:
				iCutOffset += 1;
				if(iCutOffset > 120)
					iCutOffset = 10;
				sprintf(cDispTemp, "%d", iCutOffset);
				lcd_display(1, cDispTemp);
				break;
				
			case Key_Down:
				iCutOffset -= 1;
				if(iCutOffset < 10)
					iCutOffset = 120;
				sprintf(cDispTemp, "%d", iCutOffset);
				lcd_display(1, cDispTemp);		
				break;
				
			case Key_Right:
				iCutOffset += 10;
				if(iCutOffset > 120)
					iCutOffset = 10;
				sprintf(cDispTemp, "%d", iCutOffset);
				lcd_display(1, cDispTemp);
				break;

			case Key_Left:
				iCutOffset -= 10;
				if(iCutOffset < 10)
					iCutOffset = 120;
				sprintf(cDispTemp, "%d", iCutOffset);
				lcd_display(1, cDispTemp);	
				break;
  
			case Key_Parameter://转角提刀
				LiftCutExtensionValueManage();
				state = 0;
				break;
				
			case Key_Ok:
				gl_iCutOffset = iCutOffset;
				Printf("Save gl_iCutOffset: %d\n", gl_iCutOffset);
				if(!WriteParameter())
					LcdDisp(language[11], language[12]);//写入参数失败, 请重试
				state = 0;
				break;
			case Key_Pause:
				state = 0;
				break;
		}
	}
	
}
//******************************
/*用于速度选择*/
void InitSpeedCurve(void)
{//200, 275, 350, 425, 500}
	switch(gl_iCutSpeed)
	{
		case 4:
			
		
				gl_iSVSpeed = 40;
				gl_iVSpeed = 700;		//735 149
				gl_iTimeAdd = 600;		// step 114 spy 3500 spyx 1500
				
				gl_iSVSpeedX = 35;
				gl_iVSpeedX = 300;		//1650
				gl_iTimeAddX = 1290;	//
			 /*
				gl_iSVSpeed = 70;
				gl_iVSpeed  = 1090;		
				gl_iTimeAdd = 500;		
			
				gl_iSVSpeedX = 60;
				gl_iVSpeedX = 430;									            
				gl_iTimeAddX = 800;  
			 */ 
			break;
		
			case 3:
				gl_iSVSpeed = 40;
				gl_iVSpeed = 575;		// 850 135
				gl_iTimeAdd = 800;		//step 127 spy 2875 spx 1300
				
				gl_iSVSpeedX = 35;
				gl_iVSpeedX = 260;		//
				gl_iTimeAddX = 1600;	//1710
				break;
		
			case 2:
				gl_iSVSpeed = 40;		//
				gl_iVSpeed = 440;		//
				gl_iTimeAdd = 1030; 	//step 129 spy 2200 syx 1100
				
				gl_iSVSpeedX = 35;		//
				gl_iVSpeedX = 220;		//
				gl_iTimeAddX = 1850;	//
				break;
		
			case 1:
				gl_iSVSpeed = 30;
				gl_iVSpeed = 305;		//step 78 spy1525 spx 900
				gl_iTimeAdd = 900;
				
				gl_iSVSpeedX = 25;
				gl_iVSpeedX = 180;
				gl_iTimeAddX = 1430;	//
				break;
		
			case 0:
				gl_iSVSpeed = 30;
				gl_iVSpeed = 170;		//step36 spy850 spx700
				gl_iTimeAdd = 700;
				
				gl_iSVSpeedX = 25;
				gl_iVSpeedX = 140;
				gl_iTimeAddX = 840;    //
				break;
		
			default:
				gl_iSVSpeed = 40;		//
				gl_iVSpeed = 440;		//
				gl_iTimeAdd = 1030; 	//step 129 spy 2200 syx 1100
				
				gl_iSVSpeedX = 35;		//
				gl_iVSpeedX = 220;		//
				gl_iTimeAddX = 1850;	//
				break;
	}
	//Printf("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
	gl_cStepX = UpdataAccTable(gl_iSVSpeedX, gl_iVSpeedX, gl_iTimeAddX, gl_iStepSpeedX, gl_cMaxStepX);
	//Printf("\nYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY\n");
	gl_cStep = UpdataAccTable(gl_iSVSpeed, gl_iVSpeed, gl_iTimeAdd, gl_iStepSpeed, gl_cMaxStep);

	gl_iTempATime = gl_iVSpeed - 100;
}


void CSM_DispMenu(char cMenuNo, int iPara)
{
	char cDispPara[5][16];// = {{"低速"},{"中低"},{"中速"}, {"中高"},{"高速"}};

	if(gl_cLanguageSelect == 0)
	{
		strcpy(cDispPara[0], "低速");
		strcpy(cDispPara[1], "中低");
		strcpy(cDispPara[2], "中速");
		strcpy(cDispPara[3], "中高");
		strcpy(cDispPara[4], "高速");
	}
	else
	{
		strcpy(cDispPara[0], "Low");
		strcpy(cDispPara[1], "Low-middle");
		strcpy(cDispPara[2], "Middle");
		strcpy(cDispPara[3], "Middle-high");
		strcpy(cDispPara[4], "High");
	}
	
	if(cMenuNo == 0)
	{
		if(gl_cLanguageSelect == 0)
			LcdDisp("切割速度设置:", cDispPara[iPara]);
		else
			LcdDisp("Cut Speed:", cDispPara[iPara]);
	}
	else
	{
		if(gl_cLanguageSelect == 0)
			LcdDisp("移框速度设置:", cDispPara[iPara]);
		else
			LcdDisp("Mov Speed:", cDispPara[iPara]);
	}	
	
}

void CutSpeedManage(void)
{
	char cDispPara[5][16];// = {{"低速"},{"中低"},{"中速"}, {"中高"},{"高速"}};
	int  iCutSpeed,  iMoveFramSpeed, istate, iPara;	//iPrintSpeed,
	unsigned char cReadKey, cMenuNo;

	iCutSpeed = gl_iCutSpeed;
	//iPrintSpeed = gl_iPrintSpeed;
	iMoveFramSpeed = gl_iMoveFramSpeed;
	Printf("RSM: CS[%d]  MS[%d]\n", iCutSpeed, iMoveFramSpeed);// PS[%d] iPrintSpeed,
	if(gl_cLanguageSelect == 0)
	{
	    strcpy(cDispPara[0], "低速");
		strcpy(cDispPara[1], "中低");
		strcpy(cDispPara[2], "中速");
		strcpy(cDispPara[3], "中高");
		strcpy(cDispPara[4], "高速");
		LcdDisp("切割速度设置:", cDispPara[iCutSpeed]);
	}
	else
	{
		strcpy(cDispPara[0], "Low");
		strcpy(cDispPara[1], "Low-middle");
		strcpy(cDispPara[2], "Middle");
		strcpy(cDispPara[3], "Middle-high");
		strcpy(cDispPara[4], "High");
	
		LcdDisp("CUT SPEED:", cDispPara[iCutSpeed]);
	}
	cMenuNo = 0;
	iPara = iCutSpeed;
	istate = 1;
	while(istate)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (cReadKey)
		{
			case Key_Speed:
				cMenuNo++;
				if(cMenuNo > 1)
					cMenuNo = 0;
					
				if(cMenuNo == 0)
				{
					iMoveFramSpeed = iPara;
					iPara = iCutSpeed;
					if(gl_cLanguageSelect == 0)
						LcdDisp("切割速度设置:", cDispPara[iCutSpeed]);
					else
						LcdDisp("CUT SPEED:", cDispPara[iCutSpeed]);
				}
				else
				{
					iCutSpeed = iPara;
					iPara = iMoveFramSpeed;
					if(gl_cLanguageSelect == 0)
						LcdDisp("移框速度设置:", cDispPara[iMoveFramSpeed]);
					else
						LcdDisp("MOV SPEED:", cDispPara[iCutSpeed]);
				}
				break;

			case Key_Right:
			case Key_Up:
				iPara++;
				if(iPara > 4)
					iPara = 0;
				CSM_DispMenu(cMenuNo, iPara);
				break;

			case Key_Left:
			case Key_Down:
				iPara--;
				if(iPara < 0)
					iPara = 4;
				CSM_DispMenu(cMenuNo, iPara);
				break;

			case Key_Ok:
				if(cMenuNo == 0)
					iCutSpeed = iPara;
				else
					iMoveFramSpeed = iPara;

				gl_iCutSpeed = iCutSpeed;
				InitSpeedCurve();// 刀速度曲线
				
				//gl_iPrintSpeed = iPrintSpeed;
				gl_iMoveFramSpeed = iMoveFramSpeed;
				Printf("SetSp: CS[%d]  MS[%d]\n", gl_iCutSpeed,  gl_iMoveFramSpeed); //PS[%d] gl_iPrintSpeed,
				if(!WriteParameter()) {
					if (gl_cLanguageSelect == 0) {
						LcdDisp("写入参数失败", "请重试");//写入参数失败, 请重试
					}
					else {
						LcdDisp("Data Write Fail", "Retry");
					}

				}
					//LcdDisp("写入参数失败", "请重试");//写入参数失败, 请重试
				istate = 0;
				break;

			case Key_Pause:
				istate = 0;
				break;
		}
	}
	
}

//***********************

//刀压设置:控制刀下降深浅,刀压最大值2mm
//修改刀号，其对应不同的刀压值
/*
//手动测试刀压
char ManualTestCutPress()
{
	unsigned char cReadKey, state;
	//int iCutPostX, iCutPostY;
	//int iStatus, iPluseX, iPluseY;
	LcdDisp("手动刀压测试","等待操作");
	
	state = 1;
	while(state)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
	
		switch (cReadKey)
		{
			case Key_F1://落刀
				LcdDisp("手动刀压测试","落刀");
				if (gl_cCutUpDownState == 0)
				{  //将刀具落下
					CtrlDownCut();
				}
				break;
				
			case Key_F2://提刀
				LcdDisp("手动刀压测试","提刀");
				if (gl_cCutUpDownState == 1)
				{  //将刀具提起
					CtrlUpCut();
				}
				break;
				
			case Key_Up://Y移动
			case Key_Down:
			case Key_Left:
			case Key_Right:
				//LcdDisp("手动刀压测试","移框");
				RP_SetOrigin(0);
				LcdDisp("手动刀压测试","等待操作");
				//MoveFrame(cReadKey, &iCutPostX, &iCutPostY);
				break;

			case Key_Ok:
			case Key_Pause:
				LcdDisp("手动刀压测试","退出");
				if (gl_cCutUpDownState == 1)
				{  //将刀具提起
					WaitPrintEnd();
					CtrlUpCut();
				}
				//ReturnOriginal(gl_lCurPost_XPluse, gl_lCurPost_YPluse);
				
				state = 0;
				break;
		}
	}		
	return 0;
}

*/
#define XXPLUSE_2CM		(long)(20.0*X_PPMM)
#define YYPLUSE_2CM		(long)(20.0*Y_PPMM)

#define XXPLUSE_24MM	(long)(24.0*X_PPMM)
#define YYPLUSE_24MM	(long)(24.0*Y_PPMM)
//测试刀压  20130820
//去下一工作位置(0,0)
void CCPA_GoNextPost(long lNewPostPluse_X, long lNewPostPluse_Y)
{
	long lMovePluse_X, lMovePluse_Y;
		
	lMovePluse_X = ((gl_lCurPost_XPluse - lNewPostPluse_X) * -1);
	lMovePluse_Y = ((gl_lCurPost_YPluse - lNewPostPluse_Y) * -1);
	//Printf("GNP X[%d], Y[%d]\n",lMovePluse_X, lMovePluse_Y);
	//StraightLineADSpeed(AXIS_Y, gl_iSVSpeed, gl_iVSpeed, gl_iTimeAdd);
	//TwoAxleInpDriveSpeed(10, 200, 300);
	SetReturnOrgSpeed();
	inp_move2(lMovePluse_X, lMovePluse_Y);
	WaitInpEnd();
	gl_lCurPost_XPluse = lNewPostPluse_X;
	gl_lCurPost_YPluse = lNewPostPluse_Y;
		
}

//试刀压
void CCPA_TestCutPress(void)
{
	int iStatus, iCutOffset, iExtOffset;
	long lTempXPluse, lTempYPluse;
	//char cReadKey;
	//Printf("*****TestCutPress******\n");
	iCutOffset = gl_iCutOffset*gl_dPosUnitX;
	iExtOffset = gl_dCutExtensValue*gl_dPosUnitX;
	lTempXPluse = XXPLUSE_2CM + iCutOffset;
	lTempYPluse = YYPLUSE_2CM + iCutOffset;
	
	pmove(AXIS_Y,  -DowCutOffset*gl_dPosUnitX);
	do
	{
		get_status(AXIS_Y, &iStatus);
	} while (iStatus != 0);
	Delay(500);
	
	//落刀，切割2X2正方形
	CtrlDownCut();
	//RP_SetDoubleAxisADSpeed(gl_iCutSpeed);
	StraightLineADSpeed(AXIS_Y, 10, 50, 200);

	//X1Y1 -> X2Y2
	pmove(AXIS_Y,  lTempYPluse+DowCutOffset*gl_dPosUnitX);
	do
	{
		get_status(AXIS_Y, &iStatus);
	} while (iStatus != 0);
	Delay(50);
	
	pmove(AXIS_Y,  -iCutOffset);
	do
	{
		get_status(AXIS_Y, &iStatus);
	} while (iStatus != 0);
		
	if(gl_cSetPenAttr[gl_cCutPenNum] == 1)
	{// 转角提刀
		CornerGriffe(0,YYPLUSE_2CM, XXPLUSE_2CM, YYPLUSE_2CM);
	}
		
	Delay(500);
	//*********

	//*********
	//X2Y2 -> X3Y3
	StraightLineADSpeed(AXIS_X, 10, 50, 200);
	pmove(AXIS_X, lTempXPluse);
	do
	{
		get_status(AXIS_X, &iStatus);
	} while (iStatus != 0);
	Delay(50);
	
	pmove(AXIS_X, -iCutOffset);
	do
	{
		get_status(AXIS_X, &iStatus);
	} while (iStatus != 0);
	
	if(gl_cSetPenAttr[gl_cCutPenNum] == 1)
	{// 转角提刀
		CornerGriffe(XXPLUSE_2CM, YYPLUSE_2CM, XXPLUSE_2CM, 0);
	}
	Delay(500);
	
	//X3Y3 -> X4Y4
	StraightLineADSpeed(AXIS_Y, 10, 50, 200);
	pmove(AXIS_Y, -lTempYPluse );
	do
	{
		get_status(AXIS_Y, &iStatus);
	} while (iStatus != 0);
	Delay(50);

	pmove(AXIS_Y,  iCutOffset);
	do
	{
		get_status(AXIS_Y, &iStatus);
	} while (iStatus != 0);

	if(gl_cSetPenAttr[gl_cCutPenNum] == 1)
	{// 转角提刀
		CornerGriffe(XXPLUSE_2CM, 0, 0, 0);
	}
	Delay(500);
	
	//X4Y4 -> X1Y1
	StraightLineADSpeed(AXIS_X, 10, 50, 200);
	pmove(AXIS_X, -(XXPLUSE_2CM+iExtOffset));
	do
	{
		get_status(AXIS_X, &iStatus);
	} while (iStatus != 0);
	Delay(50);
	pmove(AXIS_X, iExtOffset);
	do
	{
		get_status(AXIS_X, &iStatus);
	}while (iStatus != 0);
	Delay(500);
	//Printf("Draw X-\n");
	//提刀,回右下角
	CtrlUpCut();
}

void TestPressCut(char cTestNo)
{
	static long lInitStartXPluse = 0, lInitStartYPluse = 0;
	//static char cRunState = 0;
	long lNewPost_XPluse, lNewPost_YPluse;
	
	//cRunState = 0;
	switch(cTestNo)
	{
		case 1:
			lNewPost_XPluse = (gl_lMaxX - (long)(72.0 * X_PPMM));
			lNewPost_YPluse = (long)(165.0 * Y_PPMM);
			lInitStartXPluse = lNewPost_XPluse;
			lInitStartYPluse = lNewPost_YPluse;
			break;

		case 2:
			lNewPost_XPluse = lInitStartXPluse + XXPLUSE_24MM;
			lNewPost_YPluse = lInitStartYPluse;
			break;

		case 3:
			lNewPost_XPluse = lInitStartXPluse + XXPLUSE_24MM;
			lNewPost_YPluse = lInitStartYPluse - YYPLUSE_24MM;
			break;

		case 4:
			lNewPost_XPluse = lInitStartXPluse;
			lNewPost_YPluse = lInitStartYPluse - YYPLUSE_24MM;
			break;
	}
		
	CCPA_GoNextPost(lNewPost_XPluse, lNewPost_YPluse);
	CCPA_TestCutPress();
	CCPA_GoNextPost( gl_lMaxX, (PenCarSideInterval * DotPulseNum));
}
/*
//自动测试刀压
char AutoTestCutPress(int iTestCutPreNum)
{
	LcdDisp("测试刀压","请稍后......");	
	Printf("AutoTestCutPress...\n");
	CutTestGraph(iTestCutPreNum);	// 1000*0.025HPGL=25mm  2000*0.025=50mm 800*0.025=20mm
	return 0;
}
//6013822000643922860
//刀压测试
void CutPressTest()
{
	char cReadKey, state;
	//int iCurPos_Xmm, iCurPos_Ymm;
	LcdDisp("1:自动刀压测试","2:手动刀压测试");
	state = 1;
	while(state)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
	
		switch (cReadKey)
		{
			case Key_F1:
				//state = AutoTestCutPress();
				break;
				
			case Key_F2:
				state = ManualTestCutPress();
				break;
				
			case Key_Ok:
			case Key_Pause:
				state = 0;
				break;
		}
	}
}
*/
// 笔刀属性
void SetPenCutAttribute(void)
{
	unsigned char cReadKey;//键值
	char  state;
	char cParaBuf[][5] = {{"PEN "},{"CUT "}, {"CUT*"}};// 20131208 不要笔属性
	int cPenAttrBuf[16];
	char cDispBuf[16];
	int iCurrPenNo;//, iDispPenNo;

	iCurrPenNo = gl_cCutPenNum; 
	ArrCpy(cPenAttrBuf, gl_cSetPenAttr, 10);
	Printf("iCurNo[%d], cPA[%d][%s]", iCurrPenNo, cPenAttrBuf[iCurrPenNo], cParaBuf[cPenAttrBuf[iCurrPenNo]]);
	if(gl_cLanguageSelect == 0)
		strcpy(gl_cLcdDispBuf[0], "刀属性设置:");
	else
		strcpy(gl_cLcdDispBuf[0], "Cut Attribute:");
	sprintf(cDispBuf, "%2d#    %s", iCurrPenNo, cParaBuf[cPenAttrBuf[iCurrPenNo]]);
	strcpy(gl_cLcdDispBuf[1], cDispBuf);
	lcd_refresh();
	
	state = 1;
	while(state)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (cReadKey)
		{
			case Key_Up:  //修改笔号 +
				iCurrPenNo++;
				if(iCurrPenNo > 8)
					iCurrPenNo = 1;
				sprintf(cDispBuf, "%2d#    %s", iCurrPenNo, cParaBuf[cPenAttrBuf[iCurrPenNo]]);
				lcd_display(1, cDispBuf);
				break;

			case Key_Down: //修改笔号 -
				iCurrPenNo--;
				if(iCurrPenNo < 1)
					iCurrPenNo = 8;
				sprintf(cDispBuf, "%2d#    %s", iCurrPenNo, cParaBuf[cPenAttrBuf[iCurrPenNo]]);
				lcd_display(1, cDispBuf);
				break;

			case Key_Left:  //修改笔刀属性 -
				cPenAttrBuf[iCurrPenNo]--;
				if(cPenAttrBuf[iCurrPenNo] < 0)
					cPenAttrBuf[iCurrPenNo] = 2;
				sprintf(cDispBuf, "%2d#    %s", iCurrPenNo, cParaBuf[cPenAttrBuf[iCurrPenNo]]);
				lcd_display(1, cDispBuf);
				break;

			case Key_Right:	//修改笔刀属性 +
				cPenAttrBuf[iCurrPenNo]++;
				if(cPenAttrBuf[iCurrPenNo] > 2)
					cPenAttrBuf[iCurrPenNo] = 0;
				sprintf(cDispBuf, "%2d#    %s", iCurrPenNo, cParaBuf[cPenAttrBuf[iCurrPenNo]]);
				lcd_display(1, cDispBuf);
				break;

			case Key_Drop:
				PenUpDownDelay();	//提落刀延时
				state = 0;
				break;
			
			case Key_Ok:
				ArrCpy(gl_cSetPenAttr, cPenAttrBuf, 10);
				WriteParameter();
				state = 0;
				break;

			case Key_Pause:
				state = 0;
				break;			
		}
	}
	
}

//刀压设置管理
void  CutPressManage()
{
	char aDispCutPress[16];
	char aDispCutSet[16];
	char cParaBuf[][5] = {{"CUT "}, {"CUT*"}};  //{"PEN "}, 20131208
	static char cTestNo = 0;
	unsigned char cReadKey;//键值
	char  state;
	int  iCutDown[10], iCutPress;//, iStrlen;
	int  iTmpCutD;//iTmpNum,
	char  cDispCutPenNum;
	
	Printf("gl_cCutPenNum: %d \n",gl_cCutPenNum);
	Printf("gl_iCutDown: %d, %d, %d, %d \n", gl_iCutDown[1],gl_iCutDown[2],gl_iCutDown[3],gl_iCutDown[4]);

	cDispCutPenNum = gl_cCutPenNum;
	ArrCpy(iCutDown, gl_iCutDown, 10);
	if(gl_cLanguageSelect == 0)
		sprintf(aDispCutSet,"刀压设置: #%d", cDispCutPenNum);
	else
		sprintf(aDispCutSet,"CUT PRESS: #%d", cDispCutPenNum);
	sprintf(aDispCutPress, "%3d     %s", iCutDown[cDispCutPenNum]/10,  cParaBuf[gl_cSetPenAttr[gl_cCutPenNum]]);//刀压
	LcdDisp(aDispCutSet, aDispCutPress);	
	
	iCutPress = iCutDown[cDispCutPenNum]/10;

	state = 1;
	while(state)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (cReadKey)
		{
			case Key_F1://修改刀号
				cDispCutPenNum += 1;
				if (cDispCutPenNum > 9)
					cDispCutPenNum = 1;
				if(gl_cLanguageSelect == 0)
					sprintf(aDispCutSet,"刀压设置: #%d", cDispCutPenNum);
				else
					sprintf(aDispCutSet,"CUT PRESS: #%d", cDispCutPenNum);
				sprintf(aDispCutPress, "%3d     %s", iCutDown[cDispCutPenNum]/10,  cParaBuf[gl_cSetPenAttr[gl_cCutPenNum]]);//刀压
				LcdDisp(aDispCutSet, aDispCutPress);
				iCutPress = iCutDown[cDispCutPenNum]/10;
				break;
				
			case Key_Test://自动试刀压
				iTmpCutD = gl_iCutDown[cDispCutPenNum];
				gl_cCutPenNum = cDispCutPenNum;
				gl_iCutDown[cDispCutPenNum] = iCutPress*10;
				//AutoTestCutPress(iTestCutPreNum);
				cTestNo++;
				if(cTestNo > 4)
					cTestNo = 1;
				TestPressCut(cTestNo);
				gl_iCutDown[cDispCutPenNum] = iTmpCutD;
				LcdDisp(aDispCutSet, aDispCutPress);	
				break;
/*				
			case Key_F4: //手动试刀压 20120905
				iTmpCutD = gl_iCutDown[cDispCutPenNum];
				gl_cCutPenNum = cDispCutPenNum;
				gl_iCutDown[cDispCutPenNum] = iCutPress * 10;
				ManualTestCutPress();
				gl_iCutDown[cDispCutPenNum] = iTmpCutD;
				LcdDisp(aDispCutSet, aDispCutPress);
				break;
*/
			case Key_Drop:
				SetPenCutAttribute();
				state = 0;
				break;

			case Key_Up:
				iCutPress += 1;
				if(iCutPress > CUTPRESSMAX)
					iCutPress = 0;
				sprintf(aDispCutPress, "%3d     %s", iCutPress,  cParaBuf[gl_cSetPenAttr[gl_cCutPenNum]]);//刀压	
				lcd_display(1, aDispCutPress);
				break;

			case Key_Down:
				iCutPress -= 1;
				if(iCutPress > CUTPRESSMAX)
					iCutPress = 0;
				sprintf(aDispCutPress, "%3d     %s", iCutPress,  cParaBuf[gl_cSetPenAttr[gl_cCutPenNum]]);//刀压	
				lcd_display(1, aDispCutPress);
				break;

			case Key_Left:
				iCutPress -= 10;
				if(iCutPress > CUTPRESSMAX)
					iCutPress = 0;
				sprintf(aDispCutPress, "%3d     %s", iCutPress,  cParaBuf[gl_cSetPenAttr[gl_cCutPenNum]]);//刀压	
				lcd_display(1, aDispCutPress);
				break;

			case Key_Right:
				iCutPress += 10;
				if(iCutPress > CUTPRESSMAX)
					iCutPress = 0;
				sprintf(aDispCutPress, "%3d     %s", iCutPress,  cParaBuf[gl_cSetPenAttr[gl_cCutPenNum]]);//刀压	
				lcd_display(1, aDispCutPress);
				break;
				
			case Key_Ok:
				gl_iCutDown[cDispCutPenNum] = iCutPress * Z_PPMM;
				
				Printf("Save gl_cCutPenNum: %d \n",cDispCutPenNum);
				Printf("Save gl_iCutDown: %d, %d, %d, %d \n", gl_iCutDown[1],gl_iCutDown[2],gl_iCutDown[3],gl_iCutDown[4]);

				if(!WriteParameter()) {
					if (gl_cLanguageSelect == 0) {
						LcdDisp("写入参数失败", "请重试");//写入参数失败, 请重试
					}
					else {
						LcdDisp("Data Write Fail", "Retry");
					}

				}
					//LcdDisp("写入参数失败", "请重试");//写入参数失败, 请重试
				state = 0;
				break;
				
			case Key_Pause:
				//gl_cCutPenNum = 0;
				state = 0;
				break;				
			
		}
	}	
}
//*****************************
//#define  DISPSETVALUEDIST(cXY, cDispDir, dXY, pD, iN) {sprintf(cXY, "%c:%5.2f", cDispDir, dXY);pD = cXY;	DOTCHARCHANGE(pD);LcdDisp( language[iN],  cXY);	}
//#define  DISPSETVALUEDIST_T(cXY, cDispDir, dXY, iN) {sprintf(cXY, "%c:%d", cDispDir, dXY); LcdDisp( "喷刀间距设置:",  cXY);	}
// 8个像素点1mm
void CutDistanceManage(void)
{
	unsigned char cReadKey;//键值
	char  state ;
	char  cDispCutDistanceBuf[20];
	int  iWorkNum;
	long iTempXPluse, iTempYPluse;
	int tdCutDistanceY, tdCutDistanceX;
	int iCutDistanceY, iCutDistanceX;
	int iStatus;
	char WorkMode;
	
	iCutDistanceY = gl_iCutDistanceY;
	iCutDistanceX = gl_iCutDistanceX;
	//显示Y轴偏距
	if(gl_cLanguageSelect == 0)
		strcpy(gl_cLcdDispBuf[0], "喷刀间距设置:");
	else
		strcpy(gl_cLcdDispBuf[0], "Cut Spacing:");
	
	sprintf(cDispCutDistanceBuf, "Y:%9d", iCutDistanceY);
	strcpy(gl_cLcdDispBuf[1], cDispCutDistanceBuf);
	lcd_refresh();
	
	WorkMode = 0;
	iWorkNum = 2;
	iTempXPluse = 0;
	iTempYPluse = 0;
	gl_lAxisXPos = 0;
	state = 1;
	while(state)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (cReadKey)
		{
			case Key_F3:
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
				
			case Key_Drop://测试笔刀间距
				if(gl_cLanguageSelect == 0)
					LcdDisp("测试中", "请稍后......");
				else
					LcdDisp("TEST", "Waiting......");
				if((gl_cPumpSwitch&0x01) == 0x01)
				{
					OpenPump();
				}
				ReturnDirectPost(1);
				tdCutDistanceX = gl_iCutDistanceX;
				tdCutDistanceY = gl_iCutDistanceY;
				gl_iCutDistanceY  = iCutDistanceY;		
				gl_iCutDistanceX = iCutDistanceX;

				//PrintHeadClean(1);
				//Printf("A00: CPY[%d]\n", gl_lCurPost_YPluse);
				// 移到偏移位置
				//SetMoveFrameSpeed(AXIS_Y);
				SingleAxisInpDriveSpeed(AXIS_X+AXIS_Y, Frame_SV, 200, 800);
				iTempYPluse = iWorkNum*25 * Y_PPMM;
				pmove(AXIS_Y, iTempYPluse);
				do{
					get_status(AXIS_Y, &iStatus);
				}while(iStatus != 0);
				gl_lCurPost_YPluse += iTempYPluse;
				//Printf("A01: CPY[%d], TYP[%d]\n", gl_lCurPost_YPluse, iTempYPluse);
				
				
				iTempYPluse = (AdditionDot+OriginalSideDot) * DotPulseNum;
				pmove(AXIS_Y, -iTempYPluse);
				do{
					get_status(AXIS_Y, &iStatus);
				}while(iStatus != 0);
				gl_lCurPost_YPluse -= iTempYPluse;
				//Printf("A02: CPY[%d], TYP[%d]\n", gl_lCurPost_YPluse, iTempYPluse);
				PrintTestGraph(iWorkNum);
				
				//SetMoveFrameSpeed(AXIS_Y);
				SingleAxisInpDriveSpeed(AXIS_X+AXIS_Y, Frame_SV, 200, 800);
				iTempYPluse = PenCarSideInterval * DotPulseNum - gl_lCurPost_YPluse;
				gl_lCurPost_YPluse = PenCarSideInterval * DotPulseNum;
				Printf("A03: CPY[%d], TYP[%d]\n", gl_lCurPost_YPluse, iTempYPluse);
				
				pmove(AXIS_Y, iTempYPluse);
				do
				{
					get_status(AXIS_Y, &iStatus);
				} while (iStatus != 0);	
				
				CutTestGraph(iWorkNum);
				
				iWorkNum++;
				
				gl_iCutDistanceX = tdCutDistanceX;
				gl_iCutDistanceY = tdCutDistanceY;
				
				if((gl_cPumpSwitch & 0x02) == 0x02)
				{//自动关气泵
					ClosePump();
				}	
				if(gl_cLanguageSelect == 0)
					LcdDisp("喷刀间距设置:", cDispCutDistanceBuf);
				else
					LcdDisp("CUT OFFSET:", cDispCutDistanceBuf);
				break;
	
			case Key_Up:
				if(WorkMode == 0)
				{
					iCutDistanceY += 1;
					if (iCutDistanceY > 700)
						iCutDistanceY = 500;
					sprintf(cDispCutDistanceBuf, "Y:%9d", iCutDistanceY);
					lcd_display(1, cDispCutDistanceBuf);
				}
				else{
						iCutDistanceX += 1;
						if (iCutDistanceX > -200)
							iCutDistanceX = -400;	
						sprintf(cDispCutDistanceBuf, "X:%9d", iCutDistanceX);
						lcd_display(1, cDispCutDistanceBuf);
				}
					
				break;

			case Key_Down:
				if(WorkMode == 0)
				{
					iCutDistanceY -= 1;
					if (iCutDistanceY < 500)
						iCutDistanceY = 700;	
					sprintf(cDispCutDistanceBuf, "Y:%9d", iCutDistanceY);
					lcd_display(1, cDispCutDistanceBuf);
				}
				else{
						iCutDistanceX-= 1;
						if (iCutDistanceX < -400)
							iCutDistanceX = -200;	
						sprintf(cDispCutDistanceBuf, "X:%9d", iCutDistanceX);
						lcd_display(1, cDispCutDistanceBuf);
				}
				break;

			case Key_Left:
				if(WorkMode == 0)
				{
					iCutDistanceY -= 10;
					if (iCutDistanceY < 500)
						iCutDistanceY = 700;	
					sprintf(cDispCutDistanceBuf, "Y:%9d", iCutDistanceY);
					lcd_display(1, cDispCutDistanceBuf);
				}
				else{
						iCutDistanceX-= 10;
						if (iCutDistanceX > -200)
							iCutDistanceX = -400;	
						sprintf(cDispCutDistanceBuf, "X:%9d", iCutDistanceX);
						lcd_display(1, cDispCutDistanceBuf);
				}
				break;

			case Key_Right:
				if(WorkMode == 0)
				{
					iCutDistanceY += 10;
					if (iCutDistanceY > 700)
						iCutDistanceY = 500;	
					sprintf(cDispCutDistanceBuf, "Y:%9d", iCutDistanceY);
					lcd_display(1, cDispCutDistanceBuf);
				}
				else{
						iCutDistanceX += 10;
						if (iCutDistanceX < -400)
							iCutDistanceX = -200;	
						sprintf(cDispCutDistanceBuf, "X:%9d", iCutDistanceX);
						lcd_display(1, cDispCutDistanceBuf);
				}
				break;

			case Key_Ok:
				gl_iCutDistanceY  = iCutDistanceY;		
				gl_iCutDistanceX = iCutDistanceX;	
				Printf("Save Y: %d X: %d\n",gl_iCutDistanceY, gl_iCutDistanceX);
				if(!WriteParameter()) {
					if (gl_cLanguageSelect == 0) {
						LcdDisp("写入参数失败", "请重试");//写入参数失败, 请重试
					}
					else {
						LcdDisp("Data Write Fail", "Retry");
					}

				}
					//LcdDisp("写入参数失败", "请重试");//写入参数失败, 请重试
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
//*******************************

//************************20130807 自动送纸及移框

//********结束返回右下角 7 
void RP_EndReturnLowerRight(void)
{
	char cWstate, cReadKey;
	char cOverReturnState;	//割完返回右下角  1:ON 返回 / 0:OFF 不返回
	char cDispOFON[2][16];// = {{"右下"},{"右上"}};
	
	cOverReturnState = gl_cOverReturnState;
	
	Printf("or[0x%x]\n", cOverReturnState);

	if(gl_cLanguageSelect == 0)
	{
		strcpy(cDispOFON[0], "右下");
		strcpy(cDispOFON[1], "右上");
		strcpy(gl_cLcdDispBuf[0], "结束机头停位置");
		
	}
	else
	{
		strcpy(cDispOFON[0], "Lower Right");
		strcpy(cDispOFON[1], "Upper Right");
		strcpy(gl_cLcdDispBuf[0], "Over Returns:");
		
	}
	strcpy(gl_cLcdDispBuf[1], cDispOFON[cOverReturnState]);
	lcd_refresh();

	cWstate = 1;
	while(cWstate)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;

		switch(cReadKey)
		{
			case Key_F1:
			case Key_F2:
			case Key_F3:
			case Key_F4:
				cOverReturnState = cOverReturnState == 0 ? 1 : 0;
				lcd_display(1, cDispOFON[cOverReturnState]);
				break;
				
			case Key_Parameter://机器类型
				MacTypeSetManage();
				cWstate = 0;
				break;
				
			case Key_Ok:
				gl_cOverReturnState = cOverReturnState;
				WriteParameter();
				cWstate = 0;
				break;

			case Key_Pause:
				cWstate = 0;
				break;
		}
	}	
	
}
/***********************************/

//*********手动开关气泵
void ManualSwtichPump(void)
{
	long iKeyEffectCount;

	iKeyEffectCount = 0;
	//Printf("Maual Pump Test\n");
	while(1)
	{
		iKeyEffectCount++;
		if(iKeyEffectCount > 5000000l)
			break;
		
		if (gl_cKeyPH == gl_cKeyPE)
			continue;
		
		if (gl_cKeyBuf[gl_cKeyPE] != Key_Pump)
			break;
		else if ((gl_cKeyPE +1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
	}
	
	//Printf("iKeyC[%d]\n", iKeyEffectCount);
	if(iKeyEffectCount < 5000000l)
	{	// 手动气泵吸附
		if((gl_cManualPumpSwitch&0x20) == 0x20)
			gl_cManualPumpSwitch &= 0xdf;
		else
			gl_cManualPumpSwitch |= 0x20;
		//Printf("gl_cManualPumpSwitch[0x%x]\n", gl_cManualPumpSwitch);
		WriteIO(gl_cManualPumpSwitch);
	}
	else
	{	// 气泵吸附设置
		PumpAdsorbManage();
	}
	
}

void AutoPump(char cPumpState)
{
	WriteIO(cPumpState);
}

void OpenPump(void)
{
	gl_cManualPumpSwitch &= 0xdf;
	AutoPump(gl_cManualPumpSwitch);
	Delay(20000);
}

void ClosePump(void)
{
	gl_cManualPumpSwitch |= 0x20;
	AutoPump(gl_cManualPumpSwitch);
}


//********自动送纸

// 显示二级子菜单
void AMP_LcdDispSecondMem(int iMenuNum, char cParam[])
{
	switch(iMenuNum)
	{
		case 0:
			if(gl_cLanguageSelect == 0)
				strcpy(gl_cLcdDispBuf[0],"自动送纸:");
			else
				strcpy(gl_cLcdDispBuf[0],"AutoSendPaper:");
			strcpy(gl_cLcdDispBuf[1],cParam);
			break;

		case 1:
			if(gl_cLanguageSelect == 0)
				strcpy(gl_cLcdDispBuf[0],"进纸偏移:");
			else
				strcpy(gl_cLcdDispBuf[0],"Paper Offset:");
			strcpy(gl_cLcdDispBuf[1],cParam);
			break;

		case 2:
			if(gl_cLanguageSelect == 0)
				strcpy(gl_cLcdDispBuf[0],"进纸速度:");
			else
				strcpy(gl_cLcdDispBuf[0],"Paper Speed:");
			strcpy(gl_cLcdDispBuf[1],cParam);
			break;

		case 3:
			if(gl_cLanguageSelect == 0)
				strcpy(gl_cLcdDispBuf[0],"暂停时间:");
			else
				strcpy(gl_cLcdDispBuf[0],"Pause Time:");
			strcpy(gl_cLcdDispBuf[1],cParam);
			break;

		case 4:
			if(gl_cLanguageSelect == 0)
				strcpy(gl_cLcdDispBuf[0],"自动回结束点:");
			else
				strcpy(gl_cLcdDispBuf[0],"Auto Cut OFF:");
			strcpy(gl_cLcdDispBuf[1],cParam);
			break;
	}
	lcd_refresh();
	
}

//自动送纸参数设置管理
//Auto Cut OFF: 设置为0FF2 在工作结束后，在回PU0,0前，先在唛架y最下笔位置，绘制有三角加1竖线，在回到PU0,0位置
//               设置为0N  在工作结束后，在回PU0,0前，Y回0点，落刀割断纸张，然后回右下角。
//Pause Time: 设置延时X秒后自动进纸。
void AMP_ParamMemManage(void)
{
	char cReadKey, cWstate;
	char cDispPara[17];
	long lPaperOffsetValue;  // -99999 ~ 99999
	char cPaperSpeedDisp[3][17];// = {{"低速"},{"中速"},{"高速"}};
	int  iPaperSpeedValue;	// 暂时未定 单位脉冲数
	int  iPauseTimeValue;	// 0:OFF 1~60
	char cPaperOtherDisp[3][17];// = {{"关闭"},{"结束回右下"},{"结束切断回右上"}};//
	int iAutoCutOff;		// 0:OFF  1:OFF2  2:ON
	int iMemNum;
	int iADValue;

	if(gl_cLanguageSelect == 0)
	{
		strcpy(cPaperSpeedDisp[0], "低速");
		strcpy(cPaperSpeedDisp[1], "中速");
		strcpy(cPaperSpeedDisp[2], "高速");
		strcpy(cPaperOtherDisp[0], "关闭");
		strcpy(cPaperOtherDisp[1], "结束回右下");
		strcpy(cPaperOtherDisp[2], "结束切断回右上");
	}
	else
	{
		strcpy(cPaperSpeedDisp[0], "     Low");
		strcpy(cPaperSpeedDisp[1], "     Medium");
		strcpy(cPaperSpeedDisp[2], "     High");
		strcpy(cPaperOtherDisp[0], "      OFF");
		strcpy(cPaperOtherDisp[1], "      OFF2");
		strcpy(cPaperOtherDisp[2], "      ON");
	}
	
	lPaperOffsetValue = RPGL_lPaperOffsetValue;	//
	iPaperSpeedValue = RPGL_iPaperSpeedValue-1; // 1~3,设置送纸速度的时候*100或者1000,显示取0~2
	iPauseTimeValue = RPGL_iPauseTimeValue;		// 
	iAutoCutOff	= RPGL_iAutoCutOff;				// 
	
	iADValue = 1;
	iMemNum = 1;
	//显示初始子界面
	sprintf(cDispPara, "%10ldmm", lPaperOffsetValue);
	AMP_LcdDispSecondMem(iMemNum, cDispPara);
	
	cWstate = 1;
	while(cWstate)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;

		switch(cReadKey)
		{
			case Key_Left:		// -
			case Key_Down:
				switch(iMemNum)
				{
					case 1:			//Paper Offset
						if(cReadKey == Key_Left)
							lPaperOffsetValue-=10;
						else
							lPaperOffsetValue--;
						
						if(lPaperOffsetValue < 0)
							lPaperOffsetValue = gl_sMachineLenght;
						sprintf(cDispPara, "%10ldmm", lPaperOffsetValue);
						lcd_display(1, cDispPara);
						break;
							
					case 2:			//Paper speed
						iPaperSpeedValue--;
						if(iPaperSpeedValue < 0)
							iPaperSpeedValue = 2;
						
						lcd_display(1, cPaperSpeedDisp[iPaperSpeedValue]);
						break;
							
					case 3:			//Pause time
						if(cReadKey == Key_Left)
							iPauseTimeValue-=10;
						else
							iPauseTimeValue--;
						
						if(iPauseTimeValue < 0)
							iPauseTimeValue = 60;
						sprintf(cDispPara, "%10dS", iPauseTimeValue);
						lcd_display(1, cDispPara);
						break;
						
					case 4:			//Auto Cut off
						iAutoCutOff--;
						if( iAutoCutOff < 0 )
							iAutoCutOff = 2;
						lcd_display(1, cPaperOtherDisp[iAutoCutOff]);
						break;	
				}
				break;

			case Key_Right:		// +
			case Key_Up:
				switch(iMemNum)
				{
					case 1:			//Paper Offset
						if(cReadKey == Key_Right)
							lPaperOffsetValue+=10;
						else
							lPaperOffsetValue++;
						
						if(lPaperOffsetValue > gl_sMachineLenght)
							lPaperOffsetValue = 0;
						sprintf(cDispPara, "%10ldmm", lPaperOffsetValue);
						lcd_display(1, cDispPara);
						break;
						
					case 2:			//Paper speed
						iPaperSpeedValue++;
						if(iPaperSpeedValue > 2)
							iPaperSpeedValue = 0;
						
						lcd_display(1, cPaperSpeedDisp[iPaperSpeedValue]);
						break;
							
					case 3:			//Pause time
						if(cReadKey == Key_Right)
							iPauseTimeValue+=10;
						else
							iPauseTimeValue++;
						
						if(iPauseTimeValue > 60)
							iPauseTimeValue = 0;
						sprintf(cDispPara, "%10dS", iPauseTimeValue);
						lcd_display(1, cDispPara);
						break;
							
					case 4:			//Auto Cut off
						iAutoCutOff++;
						if( iAutoCutOff > 2 )
							iAutoCutOff = 0;
						lcd_display(1, cPaperOtherDisp[iAutoCutOff]);	
						break;	
				}
				break;

			case Key_F4:
				iMemNum++;
				if( iMemNum > 4 )
				   iMemNum = 1;
				
				//Printf("menu selec[%d]\n", iMemNum);
				switch(iMemNum)
				{
					case 1:
						sprintf(cDispPara, "%10ldmm", lPaperOffsetValue);
						break;

					case 2:
						strcpy(cDispPara, cPaperSpeedDisp[iPaperSpeedValue]);
						//Printf("CP[%d][%s]\n", iPaperSpeedValue,cDispPara);
						break;

					case 3:
						if(iPauseTimeValue == 0)
							strcpy(cDispPara, cPaperOtherDisp[0]);
						else
							sprintf(cDispPara, "%10dS", iPauseTimeValue);
						break;

					case 4:
						strcpy(cDispPara, cPaperOtherDisp[iAutoCutOff]);
						break;
				}
				AMP_LcdDispSecondMem(iMemNum, cDispPara);
				break;

			case Key_Pause:
				cWstate = 0;
				break;

			case Key_Ok:
				RPGL_lPaperOffsetValue = lPaperOffsetValue;	//工作结束进纸偏移
				RPGL_iPaperSpeedValue = iPaperSpeedValue + 1; // 1~3,设置送纸速度的时候*100或者1000,显示取0~2
				RPGL_iPauseTimeValue = iPauseTimeValue;		// 
				RPGL_iAutoCutOff	= iAutoCutOff;			// 打完唛架是否自动切断
				if(RPGL_iPaperSpeedValue == 1)
				{
					gl_iUInitSpeed = 6000;
					gl_lUDriveSpeed = 30000;
					gl_lUAddSpeed = 60000;
				}
				else if(RPGL_iPaperSpeedValue == 2)
					{
						gl_iUInitSpeed = 7000;
						gl_lUDriveSpeed = 40000;
						gl_lUAddSpeed = 70000;	
					}
					else{
							gl_iUInitSpeed = 8000;
							gl_lUDriveSpeed = 50000;
							gl_lUAddSpeed = 80000;
						}
				SetUAxisInit(gl_iUInitSpeed, gl_lUDriveSpeed, gl_lUAddSpeed);
				//保存参数 
				WriteParameter();
				cWstate = 0;
				break;
		}

		//等待按键弹起
		WaitKeyBound(cReadKey);
	}
	
}

// 进/退纸
void AMP_ForwardBackPaper(char cDirection, long lLengthPluse)
{
	//long count=0;
	if(cDirection == 0)
		lLengthPluse = -lLengthPluse;
	SetUAxisInit( gl_iUInitSpeed, gl_lUDriveSpeed, gl_lUAddSpeed);
	WriteUAxisPlus(lLengthPluse);
	WriteUAxisDriveCmd(DriveUAxisStartCmd);
	Delay(10);
	do
	{		 
	 //	count++;
	}while(UAxisDriveEndState == 0);
	WriteUAxisPlus(0);
	WriteUAxisDriveCmd(DriveUAxisStopCmd);
	Delay(100);
	//Printf("For Delay Count[%d]\n", count);
}
/*
// 当前位置割断纸张
void AMP_CutPaper(void)
{
	char cReadKey, cStat;
	int iStat;
	long lTempYPluse, lTempXPluse;

	strcpy(gl_cLcdDispBuf[0], "是否切断纸张?");
	strcpy(gl_cLcdDispBuf[1], "OK: 是");
	lcd_refresh();
	cStat = 1;
	while(cStat)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;

		switch(cReadKey)
		{
			case Key_Ok:
				// 开气泵
				OpenPump();
				Delay(2000);
				
				SetMoveFrameSpeed(AXIS_X+AXIS_Y);
				// 提刀至落刀点
				lTempYPluse = (gl_iCutDistanceY/2 + PenCarSideInterval) * DotPulseNum - gl_lCurPost_YPluse;
				
				pmove(AXIS_Y, lTempYPluse);
				do{
					get_status(AXIS_Y, &iStat);
				}while(iStat!=0);
				Delay(200);
				//落刀
				CtrlDownCut();
				
				// 运刀
				lTempYPluse = gl_lMaxY - ((gl_iCutDistanceY/2 + PenCarSideInterval*2) * DotPulseNum);
				
				pmove(AXIS_Y, lTempYPluse);
				do{
					get_status(AXIS_Y, &iStat);
				}while(iStat!=0);
				
				Delay(200);
				//提刀
				CtrlUpCut();
				
				//回右上
				lTempXPluse = gl_lMaxX - gl_lCurPost_XPluse;
				lTempYPluse = lTempYPluse + (gl_iCutDistanceY/2 * DotPulseNum);
				
				inp_move2(lTempXPluse, -lTempYPluse);
				WaitInpEnd();
				
				//关气泵
				ClosePump();

				gl_lCurPost_XPluse = gl_lMaxX;
				gl_lCurPost_YPluse = PenCarSideInterval * DotPulseNum;
				break;

			case Key_Pause:
				cStat = 0;
				break;
		}
	}
}
*/
// 当前行位置切断纸张
void AMP_CutOffPaper(void)
{
	int iStatus;
	long yPluse;
	
	OpenPump();
	
	SingleAxisInpDriveSpeed(AXIS_X+AXIS_Y, Frame_SV, 200, 800);
	if((gl_lCurPost_YPluse != 0))
	{
		
		inp_move2( 0, -gl_lCurPost_YPluse);
		do{
			get_status(AXIS_Y, &iStatus);
		}while(iStatus!=0);
		gl_lCurPost_YPluse = 0;
	}
	// 偏移9CM后下刀
	gl_lCurPost_YPluse = PenCarSideInterval * DotPulseNum + 40*Y_PPMM;
	pmove(AXIS_Y, gl_lCurPost_YPluse);
	do{
		get_status(AXIS_Y, &iStatus);
	}while(iStatus!=0);
		
	CtrlDownCut();
	
	yPluse = gl_lMachineWidthPluse - gl_lCurPost_YPluse - PenCarSideInterval * DotPulseNum + 40*Y_PPMM;

	pmove(AXIS_Y, yPluse);
	do{
			get_status(AXIS_Y, &iStatus);
	}while(iStatus!=0);
	gl_lCurPost_YPluse += yPluse;
	
	Delay(500);
	
	CtrlUpCut();
	
	gl_lCurPost_YPluse = gl_lCurPost_YPluse - PenCarSideInterval * DotPulseNum;
	pmove(AXIS_Y, -gl_lCurPost_YPluse);
	do{
			get_status(AXIS_Y, &iStatus);
	}while(iStatus!=0);
	
	gl_lCurPost_YPluse = PenCarSideInterval * DotPulseNum;;
	ClosePump();
}

// 24
void RP_AutoMovPaper(void)
{
	char cReadKey, cWstate;
	char cStopState;
	char cMovPaperDir;
	long lMovePaperLen, lMoveInchToal, lMoveInch;
	long lMacLenMaxPluse;
	
	AMP_LcdDispSecondMem(0, "A");
	lMoveInchToal = 0;
	lMoveInch = 0;
	lMacLenMaxPluse = (long)(gl_sMachineLenght * U_PPMM);
	lMovePaperLen = 0;
	//主控制部分
	cStopState = 0;
	cWstate = 1;
	while(cWstate)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;

		switch(cReadKey)
		{
			case Key_F4:	// 自动送纸设置
				AMP_ParamMemManage();
				break;

			case Key_F2:	// 当前行位置断纸
				AMP_CutOffPaper();
				break;
			
			case Key_Drop:	//进纸长度为切割面的1/3
				cMovPaperDir = 1; 
				lMovePaperLen = (lMacLenMaxPluse / 3);
				//Printf("ForwardBackPape 1/3 Start [%d]\n", lMovePaperLen);
				AMP_ForwardBackPaper(cMovPaperDir, lMovePaperLen);
				//Printf("ForwardBackPape 1/3 END\n");
				break;

			case Key_Speed:	//退纸长度为切割面的1/3
				//Printf("ForwardBackPaper Start [%d]\n", lMovePaperLen);
				cMovPaperDir = 0; 
				lMovePaperLen = (lMacLenMaxPluse / 3);
				AMP_ForwardBackPaper(cMovPaperDir, lMovePaperLen);
				//Printf("ForwardBackPaper End\n");
				break;

			case Key_Parameter:	//进纸长度为切割面长度
				//Printf("ForwardBackPaper  All Start\n");
				cMovPaperDir = 1; 
				AMP_ForwardBackPaper(cMovPaperDir, lMacLenMaxPluse);
				//Printf("ForwardBackPaper  All End\n");
				break;
			
			case Key_Right:	//定原点
				//Printf("SetOrigin  Start\n");
				RP_SetOrigin(0);
				//Printf("SetOrigin  End\n");
				break;

			//case Key_Left:	//当前位置割断纸张
			//	AMP_CutPaper();
			//	break;

			case Key_Up:
				lMoveInchToal= 0;
				do{
					lMoveInch = 1*U_PPMM;
					lMoveInchToal += lMoveInch;
					if(lMoveInchToal > 8000)
					{
						lMoveInch = 8000;
						lMoveInchToal= 0;
						SetUAxisInit(2000, 4000, 8000);
					}
					else
						SetUAxisInit(500, 2000, 4000);
					WriteUAxisPlus(-lMoveInch);
					WriteUAxisDriveCmd(DriveUAxisStartCmd);
					Delay(10);
					while(UAxisDriveEndState == 0);
					WriteUAxisPlus(0);
					WriteUAxisDriveCmd(DriveUAxisStopCmd);
					Delay(10);
				}while( KeyEffectState(cReadKey) != 0 );
				break;

			case Key_Down:
				lMoveInchToal= 0;
				do{
					lMoveInch = 1*U_PPMM;
					lMoveInchToal += lMoveInch;
					if(lMoveInchToal > 8000)
					{
						lMoveInch = 8000;
						lMoveInchToal= 0;
						SetUAxisInit(2000, 4000, 8000);
					}
					else
						SetUAxisInit(500, 2000, 4000);
					WriteUAxisPlus(lMoveInch);
					WriteUAxisDriveCmd(DriveUAxisStartCmd);
					Delay(10);
					while(UAxisDriveEndState == 0);
					WriteUAxisPlus(0);
					WriteUAxisDriveCmd(DriveUAxisStopCmd);
					Delay(10);
				}while( KeyEffectState(cReadKey) != 0 );
				break;

			case Key_Pause:
				//暂停/取消
				cWstate = 0;
				break;

			case Key_Ok:
				//确认
				cWstate = 0;
				break;
				
		}
		AMP_LcdDispSecondMem(0, "A");
	}
		
}
/***********************************/

//********定原点***********************************
//显示当前坐标及移动步长
void SO_LcdDispOrgMenu(int iXPostMM, int iYPostMM, char cDispBuf_2[17])
{
	char cDispBuf_1[17];

	sprintf(cDispBuf_1, "X:%4d  Y:%4d", iXPostMM, iYPostMM);
	LcdDisp(cDispBuf_1, cDispBuf_2);
}

//刷新第一行显示
void SO_LcdRefreshFirstLine(int iXPostPlus, int iYPostPlus)
{
	//int iX_mm, iY_mm;
	char cDispBuf[17];
	sprintf(cDispBuf, "X:%4d  Y:%4d", iXPostPlus, iYPostPlus);
	lcd_display(0, cDispBuf);
}


//移动机头到指定位置
void SO_MoveHandpiece(char cAix, int iMovePluse)
{
	int iStatus;

	//设置移动速度
	SetMoveFrameSpeed(cAix);
	pmove(cAix, iMovePluse);
	//等待移动结束
	do
	{
		get_status(cAix, &iStatus);
	} while (iStatus != 0);

}

//一键回指定位置 1:右下 2:左下
void ReturnDirectPost(char cDirectPost)
{
	int iStatus;
	
	//SetReturnOrgSpeed();
	SingleAxisInpDriveSpeed(AXIS_X+AXIS_Y, Frame_SV, 200, 900);

	if(cDirectPost == 1)
	{// 回右上
		
		if (gl_lCurPost_YPluse != PenCarSideInterval * DotPulseNum)
		{//Printf("if (gl_lCurPost_YPluse != PenCarSideInterval * DotPulseNum)\n");
			//SetMoveFrameSpeed(AXIS_Y);
			//SetReturnOrgSpeed();
			pmove(AXIS_Y, PenCarSideInterval * DotPulseNum-gl_lCurPost_YPluse);
			do
			{
				get_status(AXIS_Y, &iStatus);
			} while (iStatus != 0);
			gl_lCurPost_YPluse = PenCarSideInterval * DotPulseNum;	
		}

		if (GetIOBit(INDUCTOR_XRIGHT) != XY_INDUCTOR_ORIGIN)
		{
			gl_lCurPost_XPluse = gl_lMaxX - gl_lCurPost_XPluse;
			Printf("Return Right[%d]\n", gl_lCurPost_XPluse);
			//SetMoveFrameSpeed(AXIS_X);
			//SetReturnOrgSpeed();
			pmove(AXIS_X, gl_lCurPost_XPluse);
			do
			{
				get_status(AXIS_X, &iStatus);
			} while (iStatus != 0);
			gl_lCurPost_XPluse = gl_lMaxX;
		}
	}
	else if(cDirectPost == 2)
		{// 回右下
			Printf("R left\n");
			if( gl_lCurPost_XPluse != 0 )
			{
				//SetMoveFrameSpeed(AXIS_X);
				//SetReturnOrgSpeed();
				Printf("Return left[%d]\n", gl_lCurPost_XPluse);
				pmove(AXIS_X, -gl_lCurPost_XPluse);
				do
				{
					get_status(AXIS_X, &iStatus);
				} while (iStatus != 0);
				gl_lCurPost_XPluse = 0;
			}

			if( gl_lCurPost_YPluse != PenCarSideInterval * DotPulseNum )
			{
				//SetMoveFrameSpeed(AXIS_Y);
				//SetReturnOrgSpeed();
				pmove(AXIS_Y, PenCarSideInterval * DotPulseNum-gl_lCurPost_YPluse);
				do
				{
					get_status(AXIS_Y, &iStatus);
				} while (iStatus != 0);
				gl_lCurPost_YPluse = PenCarSideInterval * DotPulseNum;	
			}
			
		}
	
}

//回工作原点
//void SO_ReturnWorkOrigin(long lCurPost_XPlus, long lCurPost_YPlus)
void  ReturnOriginal(long lCurPost_XPlus, long lCurPost_YPlus)
{
	// 工作原点
	long lPluseXX, lPluseYY;
	int iStatus;

	if((gl_lWorkOrig_XXPluse != gl_lCurPost_XPluse) || (gl_lWorkOrig_YYPluse != gl_lCurPost_YPluse))
	{
		lPluseXX = gl_lWorkOrig_XXPluse - lCurPost_XPlus;
		lPluseYY = gl_lWorkOrig_YYPluse - lCurPost_YPlus;

		SetReturnOrgSpeed();
		inp_move2(lPluseXX, lPluseYY);
		//等待移动结束
		do
		{
			get_inp_status(&iStatus);
		} while (iStatus != 0);		
		//Printf("Rorg Wx[%d] Wy[%d] CX[%d] CY[%d]\n", gl_lWorkOrig_XXPluse, gl_lWorkOrig_YYPluse, gl_lCurPost_XPluse, gl_lCurPost_YPluse);
		gl_lCurPost_XPluse += lPluseXX;
		gl_lCurPost_YPluse += lPluseYY;
		//Printf("Rorg CX[%d] CY[%d]\n", gl_lCurPost_XPluse, gl_lCurPost_YPluse);
	}
}

#define STEPADD				50//81
#define ADDSPEEDLEN_YPLUSE	( (int)(STEPADD * Y_PPMM) )	//	长按方向键，快进长度
#define ADDSPEEDLEN_XPLUSE	( (int)(STEPADD * X_PPMM) )

/*
extern void RP_LowSpeedMove(int iSpeed);

//等待移框结束
long WaitMoveEnd(char cAxis, long lToalPulseNum,  unsigned char cKeyValue, int *iDispPostX, int *iDispPostY)
{
	long LpCount;
	int iStatus;
	Printf("toalPulse:%d\n", lToalPulseNum);
	LpCount = 0;
	SetLp(cAxis, 0);
	pmove(cAxis, lToalPulseNum);
	while (1)
	{		
		get_status(cAxis, &iStatus);
		if (iStatus == 0)
			break;
		//
		LpCount += ReadLp(cAxis);
		if(cAxis == AXIS_X)
		{
			if((LpCount/X_PPMM) > 1)
			{
				*iDispPostX += (LpCount/X_PPMM);
				SO_LcdRefreshFirstLine(*iDispPostX, *iDispPostY);
				LpCount %= X_PPMM;
				Printf("Lc1[%d],", LpCount);
			}
		}
		else{
				if((LpCount/Y_PPMM) > 1)
				{
					*iDispPostY += (LpCount/Y_PPMM);
					SO_LcdRefreshFirstLine(*iDispPostX, *iDispPostY);
					LpCount %= Y_PPMM;
					Printf("Lc2[%d],", LpCount);
				}
		}
		//	
		if (gl_cKeyPH == gl_cKeyPE)
			continue;
		
		if (gl_cKeyBuf[gl_cKeyPE] != cKeyValue)
			break;
		else if ((gl_cKeyPE +1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
	}
	DecSpeedState(cAxis);
	Delay(1000);
	LpCount = ReadLp(cAxis);
	Printf("LpCount: %d\n", LpCount);
	return LpCount;
}
*/
// 要用脉冲来标识 23 定原点
void RP_SetOrigin(char cIsSetOrg)
{
	char cReadKey, cWstate;
	char cHeadPostState;		//机头位置状态 bit1:Y轴， bit2:Y轴  bit3:X 0:没有到最大值  1:已经是最大值
	long lCurPost_XPluse, lCurPost_YPluse;		//当前脉冲坐标
	long lMovLength_XPluse, lMovLength_YPluse;	//移动脉冲长度
	long lMovLength_XPluseToal, lMovLength_YPluseToal;
	int iStepPluse_X, iStepPluse_Y;				//每步脉冲数
	int iDispX_mm, iDispY_mm, iDispStep_mm;					//显示当前XY坐标
	char cDispStepMM[][17] = {{"Step: 250mm   "}, {"Step: 50mm  "},{"Step: 0.2mm "}};	
	char cDispStepNum;							//显示步长序号

	long lYPluseMax, lYPluseMin;		// Y最大脉冲值和最小脉冲值 20130830
	
	int iStatus;

	Printf("Set Origin CpX[%d] CpY[%d]\n", gl_lCurPost_XPluse, gl_lCurPost_YPluse);
	lCurPost_XPluse = gl_lCurPost_XPluse;
	lCurPost_YPluse= gl_lCurPost_YPluse;
	iStepPluse_X  = (250 * X_PPMM) ;
	iStepPluse_Y  = (250 * Y_PPMM) ;

	lYPluseMax = (gl_sMachineWidth* Y_PPMM + PenCarSideInterval * DotPulseNum) ;
	Printf("PenCarSideInterval  =  %d \n " , PenCarSideInterval);
	lYPluseMin = PenCarSideInterval  * DotPulseNum;
	
	lMovLength_XPluse = 0;
	lMovLength_YPluse = 0;
	lMovLength_XPluseToal = 0;
	lMovLength_YPluseToal = 0;

	iDispX_mm = (int)((lCurPost_XPluse) / X_PPMM);
	iDispY_mm = (int)((lCurPost_YPluse - lYPluseMin) / Y_PPMM);
	iDispStep_mm = 250;
	cDispStepNum = 0;
	SO_LcdDispOrgMenu(iDispX_mm, iDispY_mm, cDispStepMM[cDispStepNum]);

	cHeadPostState = 0;
	cWstate = 1;
	while(cWstate)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;

		switch(cReadKey)
		{
			case Key_Up:		//上移   X轴 +
				//Printf("Mov Up Pluse = ");
				if((cHeadPostState & 0x08) == 0x08)
					cHeadPostState &= 0xf7;

				if((cHeadPostState & 0x04) == 0x04)
					continue;
				
				if(cDispStepNum != 2)
				{
					if((lCurPost_XPluse + iStepPluse_X) > gl_lMaxX)
					{//当前脉冲坐标如果大于最大脉冲坐标
						lMovLength_XPluse = gl_lMaxX - lCurPost_XPluse;
						lCurPost_XPluse = gl_lMaxX;
						cHeadPostState = 0x04;
						//显示
						iDispX_mm = gl_sMachineLenght;
					}
					else{//当前脉冲坐标小于最大脉冲坐标
							lMovLength_XPluse = iStepPluse_X;
							lCurPost_XPluse += iStepPluse_X;
							//显示
							iDispX_mm += iDispStep_mm;
						}
					//Printf("[%d]\n", lMovLength_XPluse);
					//X向 机头右移 
					SO_MoveHandpiece(AXIS_X, lMovLength_XPluse);
					//显示XY坐标
					SO_LcdRefreshFirstLine(iDispX_mm, iDispY_mm);
					//Printf("L Go M[%d] L[%d]\n",lMovLength_XPluse, lCurPost_XPluse);
					//WaitKeyBound(cReadKey);
				}
				else
				{			
					lMovLength_XPluseToal = 0;
					lMovLength_XPluse = iStepPluse_X;
					do
					{//按键移动步长为0.2mm时，有长按键状态
						lMovLength_XPluseToal += lMovLength_XPluse;
						FixedSpeeDrive(AXIS_X,200);
						if((lCurPost_XPluse + lMovLength_XPluseToal) > gl_lMaxX)
						{
							lMovLength_XPluse = gl_lMaxX - lCurPost_XPluse;
							lCurPost_XPluse = gl_lMaxX;
							cHeadPostState = 0x04;
							//显示
							iDispX_mm = gl_sMachineLenght;
						}
						else if(lMovLength_XPluseToal > ADDSPEEDLEN_XPLUSE)
						{//按键一直有效，且低速移动大于设定值时，快速移动机头
							FixedSpeeDrive(AXIS_X,200);
							if((lCurPost_XPluse + lMovLength_XPluseToal) > gl_lMaxX)
							{//如果当前脉冲坐标加上Y移动总步长大于最大脉冲值
								lMovLength_XPluse = gl_lMaxX - lCurPost_XPluse;
								lCurPost_XPluse = gl_lMaxX;
								cHeadPostState = 0x04;
								//显示
								iDispX_mm = gl_sMachineLenght;
							}
							else{
									
									lMovLength_XPluse = ADDSPEEDLEN_XPLUSE;
									lMovLength_XPluseToal = 0;
									lCurPost_XPluse += ADDSPEEDLEN_XPLUSE;
									//显示
									iDispX_mm += STEPADD;
								}
						}
						else{
								lMovLength_XPluse = iStepPluse_X;
								lCurPost_XPluse += iStepPluse_X;

								if((lCurPost_XPluse/X_PPMM) != iDispX_mm)
									iDispX_mm = (lCurPost_XPluse/X_PPMM);
							}
						//X向 机头上移 
						//pmove(AXIS_X, lMovLength_XPluse);
						inp_move2(lMovLength_XPluse, 0);
						do
						{//等待移动结束
							//get_status(AXIS_X, &iStatus);
							get_inp_status(&iStatus);
						} while (iStatus != 0);
						lMovLength_XPluse = iStepPluse_X;
						//显示XY坐标
						//SO_LcdDirctPostDisp(0, iDispX_mm);
						//SO_LcdRefreshFirstLine(iDispX_mm, iDispY_mm);
						if((cHeadPostState & 0x04) == 0x04)
							break;
					}while( KeyEffectState(cReadKey) != 0 );
					SO_LcdRefreshFirstLine(iDispX_mm, iDispY_mm);
				}
				break;

			case Key_Down:		//下移   X轴 - 
				//Printf("Key Down = ");
				if((cHeadPostState & 0x04) == 0x04)
					cHeadPostState &= 0xfB;

				if((cHeadPostState & 0x08) == 0x08)
					continue;
				
				if(cDispStepNum != 2)
				{
					if((lCurPost_XPluse - iStepPluse_X) < 0)
					{//当前脉冲坐标如果大于最大脉冲坐标
						lMovLength_XPluse = -lCurPost_XPluse;
						lCurPost_XPluse = 0;
						cHeadPostState = 0x08;
						//显示
						iDispX_mm = 0;
					}
					else{//当前脉冲坐标小于最大脉冲坐标
							lMovLength_XPluse = -iStepPluse_X;
							lCurPost_XPluse -= iStepPluse_X;
							//显示
							iDispX_mm -= iDispStep_mm;
						}
					//Printf("[%d]\n", lMovLength_XPluse);
					//X向 机头右移 
					SO_MoveHandpiece(AXIS_X, lMovLength_XPluse);
					//显示XY坐标
					SO_LcdRefreshFirstLine(iDispX_mm, iDispY_mm);
					//Printf("L Go M[%d] L[%d]\n",lMovLength_XPluse, lCurPost_XPluse);
					//WaitKeyBound(cReadKey);
				}
				else
				{
					lMovLength_XPluseToal = 0;
					lMovLength_XPluse = -iStepPluse_X;
										
					//Printf("iStepX[%d] iCupX[%d]\n", iStepPluse_X, lCurPost_XPluse);
					do
					{//按键移动步长为0.2mm时，有长按键状态
						lMovLength_XPluseToal += abs(lMovLength_XPluse);
						FixedSpeeDrive(AXIS_X,200);
						if(lMovLength_XPluseToal > ADDSPEEDLEN_XPLUSE)
						{//按键一直有效，且低速移动大于设定值时，快速移动机头
							FixedSpeeDrive(AXIS_X,200);
							if((lCurPost_XPluse - lMovLength_XPluseToal) < 0)
							{//如果当前脉冲坐标加上Y移动总步长大于最大脉冲值
								lMovLength_XPluse = -lCurPost_XPluse;
								lCurPost_XPluse = 0;
								cHeadPostState = 0x08;
								//显示
								iDispX_mm = 0;
							}
							else{
									lMovLength_XPluse = -ADDSPEEDLEN_XPLUSE;
									lMovLength_XPluseToal = 0;
									lCurPost_XPluse -= ADDSPEEDLEN_XPLUSE;
									//显示
									iDispX_mm -= STEPADD;
								}
						}
						else if((lCurPost_XPluse - lMovLength_XPluseToal) < 0)
						{
							lMovLength_XPluse = -lCurPost_XPluse;
							lCurPost_XPluse = 0;
							cHeadPostState = 0x08;
							//显示
							iDispX_mm = 0;
						}
						else{
								lMovLength_XPluse = -iStepPluse_X;
								lCurPost_XPluse -= iStepPluse_X;
								//显示
								if((lCurPost_XPluse/X_PPMM) != iDispX_mm)
									iDispX_mm = (lCurPost_XPluse/X_PPMM);
							}
						//X向 机头右移 
						//Printf("x Send Pluse[%d]\n", lMovLength_XPluse);
						//X向 机头上移 
						//pmove(AXIS_X, lMovLength_XPluse);
						inp_move2(lMovLength_XPluse, 0);
						do
						{//等待移动结束
							//get_status(AXIS_X, &iStatus);
							get_inp_status(&iStatus);
						} while (iStatus != 0);
						lMovLength_XPluse = -iStepPluse_X;
											
						if((cHeadPostState & 0x08) == 0x08)
							break;
					}while( KeyEffectState(cReadKey) != 0);
					SO_LcdRefreshFirstLine(iDispX_mm, iDispY_mm);
				}
				break;

			case Key_Left:		//左移  Y轴 +
				//Printf("Key Left = ");
				//当前机头位置在最大或最小位置
				if((cHeadPostState & 0x02) == 0x02)
					cHeadPostState &= 0xfd;
				
				if((cHeadPostState & 0x01) == 0x01)
					continue;
				
				if(cDispStepNum != 2)
				{
					if((lCurPost_YPluse + iStepPluse_Y) > lYPluseMax)
					{//当前脉冲坐标如果大于最大脉冲坐标
						Printf("M++ Go M[%d] YM[%d] L[%d]\n",(lYPluseMax - lCurPost_YPluse),lYPluseMax, lCurPost_YPluse);
						lMovLength_YPluse = lYPluseMax - lCurPost_YPluse;
						lCurPost_YPluse = lYPluseMax;
						cHeadPostState = 0x01;	//机头在Y最大位置
						//显示
						iDispY_mm = gl_sMachineWidth;
						
					}
					else{//当前脉冲坐标小于最大脉冲坐标
							lMovLength_YPluse = iStepPluse_Y;
							lCurPost_YPluse += iStepPluse_Y;
							//显示
							iDispY_mm += iDispStep_mm;
							//Printf("L Go M[%d] L[%d]\n",lMovLength_YPluse, lCurPost_YPluse);
						}
					//Printf("[%d]\n", lMovLength_YPluse);
					//Y向 机头上移
					SO_MoveHandpiece(AXIS_Y, lMovLength_YPluse);
					//显示XY坐标
					SO_LcdRefreshFirstLine(iDispX_mm, iDispY_mm);
					//Printf("L Go M[%d] L[%d]\n",lMovLength_YPluse, lCurPost_YPluse);
					//WaitKeyBound(cReadKey);
				}						
				else{//按键移动步长为0.2mm时，有长按键状态
						lMovLength_YPluseToal = 0;
						lMovLength_YPluse = iStepPluse_Y;
						
						do{
							lMovLength_YPluseToal += lMovLength_YPluse;
							FixedSpeeDrive(AXIS_Y,1000);
							if(lMovLength_YPluseToal > ADDSPEEDLEN_YPLUSE)
							{//按键一直有效，且低速移动大于设定值时，快速移动机头
								FixedSpeeDrive(AXIS_Y,1000);
								if((lCurPost_YPluse + lMovLength_YPluseToal) > lYPluseMax)
								{//如果当前脉冲坐标加上Y移动总步长大于最大脉冲值
									lMovLength_YPluse = lYPluseMax - lCurPost_YPluse;
									lCurPost_YPluse = lYPluseMax;
									cHeadPostState = 0x01;	//机头在Y最大位置
									//显示
									iDispY_mm = gl_sMachineWidth;
								}
								else
								{
									lMovLength_YPluse = ADDSPEEDLEN_YPLUSE;
									lMovLength_YPluseToal = 0;
									lCurPost_YPluse += ADDSPEEDLEN_YPLUSE;
									//显示
									iDispY_mm += STEPADD;
									//Printf("MLYP[%d]\n", lMovLength_YPluse);
								}
							}
							else if((lCurPost_YPluse + lMovLength_YPluseToal) > lYPluseMax)
							{
								lMovLength_YPluse = lYPluseMax - lCurPost_YPluse;
								lCurPost_YPluse = lYPluseMax;
								cHeadPostState = 0x01;	//机头在Y最大位置	
								//显示
								iDispY_mm = gl_sMachineWidth;
							}
							else{
									lMovLength_YPluse = iStepPluse_Y;
									lCurPost_YPluse += iStepPluse_Y;
									//显示
									if((lCurPost_YPluse/Y_PPMM) != iDispY_mm)
										iDispY_mm = lCurPost_YPluse/Y_PPMM;
								}
							//Y向 机头上移
							//pmove(AXIS_Y, lMovLength_YPluse);
							inp_move2(0, lMovLength_YPluse);
							do
							{//等待移动结束
								//get_status(AXIS_Y, &iStatus);
								get_inp_status(&iStatus);
							} while (iStatus != 0);
							lMovLength_YPluse = iStepPluse_Y;
							
							if((cHeadPostState & 0x01) == 0x01)
								break;
						}while( KeyEffectState(cReadKey) != 0);
						//显示XY坐标
						SO_LcdRefreshFirstLine(iDispX_mm, iDispY_mm);
					}
				break;


			case Key_Right:		//右移 Y轴 -
				//Printf("Key Right = ");
				if((cHeadPostState & 0x01) == 0x01)
					cHeadPostState &= 0xfe;

				if((cHeadPostState & 0x02) == 0x02)
					continue;
				if(cDispStepNum != 2)
				{
					if((lCurPost_YPluse - iStepPluse_Y) < lYPluseMin)
					{//当前脉冲坐标如果减去步长小于0
						Printf("M-- Go M[%d] YM[%d] L[%d]\n",(lCurPost_YPluse - lYPluseMin),lYPluseMin, lCurPost_YPluse);
						lMovLength_YPluse =  -(lCurPost_YPluse - lYPluseMin);
						lCurPost_YPluse = lYPluseMin;
						cHeadPostState = 0x02;	//机头在Y最小位置
						//显示
						iDispY_mm = 0;
					}
					else{//当前脉冲坐标减去步长大于0
							lMovLength_YPluse = -iStepPluse_Y;
							lCurPost_YPluse -= iStepPluse_Y;
							//显示
							iDispY_mm -= iDispStep_mm;
						}
					//Printf("[%d]\n", lMovLength_YPluse);
					//Y向 机头下移
					SO_MoveHandpiece(AXIS_Y, lMovLength_YPluse);
					//显示XY坐标
					SO_LcdRefreshFirstLine(iDispX_mm, iDispY_mm);
					//Printf("L Go M[%d] L[%d]\n",lMovLength_YPluse, lCurPost_YPluse);
					//WaitKeyBound(cReadKey);
				}
				else
				{
					lMovLength_YPluseToal = 0;
					lMovLength_YPluse = -iStepPluse_Y;
					do{//按键移动步长为0.2mm时，有长按键状态
						lMovLength_YPluseToal += abs(lMovLength_YPluse);
						FixedSpeeDrive(AXIS_Y,1000);
						if(lMovLength_YPluseToal > ADDSPEEDLEN_YPLUSE)
						{//按键一直有效，且低速移动大于设定值时，快速移动机头
							FixedSpeeDrive(AXIS_Y,1000);
							if((lCurPost_YPluse - lMovLength_YPluseToal) < lYPluseMin)
							{//如果当前脉冲坐标
								lMovLength_YPluse = -(lCurPost_YPluse - lYPluseMin);
								lCurPost_YPluse = lYPluseMin;
								cHeadPostState = 0x02;	//机头在Y最小位置
								//显示
								iDispY_mm = 0;
							}
							else{
									lMovLength_YPluse = (ADDSPEEDLEN_YPLUSE * -1);
									lMovLength_YPluseToal = 0;
									lCurPost_YPluse -= ADDSPEEDLEN_YPLUSE;
									//显示
									iDispY_mm -= STEPADD;
								}
						}
						else if((lCurPost_YPluse - lMovLength_YPluseToal) < lYPluseMin)
						{
							lMovLength_YPluse = -(lCurPost_YPluse - lYPluseMin);
							lCurPost_YPluse = lYPluseMin;
							cHeadPostState = 0x02;	//机头在Y最小位置
							//显示
							iDispY_mm = 0;
						}
						else{
								lMovLength_YPluse = -iStepPluse_Y;
								lCurPost_YPluse -= iStepPluse_Y;
								//显示
								if((lCurPost_YPluse/Y_PPMM) != iDispY_mm)
									iDispY_mm = lCurPost_YPluse/Y_PPMM;
							}
						//Y向 机头上移
						//pmove(AXIS_Y, lMovLength_YPluse);
						inp_move2(0, lMovLength_YPluse);
						do
						{//等待移动结束
							//get_status(AXIS_Y, &iStatus);
							get_inp_status(&iStatus);
						} while (iStatus != 0);
						lMovLength_YPluse = -iStepPluse_Y;
						
						if((cHeadPostState & 0x02) == 0x02)
							break;
						
					}while( KeyEffectState(cReadKey) != 0 );
					//显示XY坐标
					SO_LcdRefreshFirstLine(iDispX_mm, iDispY_mm);
				}
				break;

			case Key_F3:		//修改移动步长
				cDispStepNum++;
				if(cDispStepNum>2)
					cDispStepNum = 0;
				lcd_display(1, cDispStepMM[cDispStepNum]);

				//设置XY脉冲步长
				if(cDispStepNum == 0)
				{
					iStepPluse_X  = (int)(250 * X_PPMM);
					iStepPluse_Y  = (int)(250 * Y_PPMM);
					//显示
					iDispStep_mm = 250;
				}
				else if(cDispStepNum == 1)
					{
						iStepPluse_X  = (int)(50 * X_PPMM);
						iStepPluse_Y  = (int)(50 * Y_PPMM);
						//显示
						iDispStep_mm = 50;
					}
					else{
							iStepPluse_X  = (int)(0.2 * X_PPMM)+1;
							iStepPluse_Y  = (int)(0.2 * Y_PPMM)+1;
							//显示
							iDispStep_mm = 1;		//没用，以实际走的为准
						}
				//Printf("Fram Move Step X[%d] Y[%d]\n", iStepPluse_X, iStepPluse_Y);
				break;

			case Key_F2:		//回原切割原点
				gl_lCurPost_XPluse = lCurPost_XPluse;
				gl_lCurPost_YPluse = lCurPost_YPluse;	
				ReturnOriginal(gl_lCurPost_XPluse, gl_lCurPost_YPluse);
				lCurPost_XPluse = gl_lCurPost_XPluse;
				lCurPost_YPluse = gl_lCurPost_YPluse;
				iDispX_mm = (int)((lCurPost_XPluse+1) / X_PPMM);
				iDispY_mm = (int)((lCurPost_YPluse+1) / Y_PPMM);
				
				SO_LcdRefreshFirstLine(iDispX_mm, iDispY_mm);
				break;

			case Key_Ok:		//保存切割原点退出
				gl_lCurPost_XPluse = lCurPost_XPluse;
				gl_lCurPost_YPluse = lCurPost_YPluse;
				if(cIsSetOrg == 1)
				{
					gl_lWorkOrig_XXPluse = lCurPost_XPluse;
					gl_lWorkOrig_YYPluse = lCurPost_YPluse;
					gl_iYMaxEffectivePMM = gl_lMaxY - gl_lWorkOrig_YYPluse - ((SprinklerSpace + AdditionDot * 2 + OriginalSideDot + MotorSideDot) * DotPulseNum);
					gl_iXMaxEffectivePMM = gl_lMaxX - gl_lWorkOrig_XXPluse;
					Printf("=====SetOrg WX[%d] WY[%d] Cx[%d] Cy[%d]======\n", gl_lWorkOrig_XXPluse, gl_lWorkOrig_YYPluse, gl_lCurPost_XPluse, gl_lCurPost_YPluse);
				}
				cWstate = 0;
				break;

			case Key_Pause:		//取消保存退出
				gl_lCurPost_XPluse = lCurPost_XPluse;
				gl_lCurPost_YPluse = lCurPost_YPluse;
				Printf("=====SetOrg Pause[%d] WY[%d] Cx[%d] Cy[%d]======\n", gl_lWorkOrig_XXPluse, gl_lWorkOrig_YYPluse, gl_lCurPost_XPluse, gl_lCurPost_YPluse);
				cWstate = 0;
				break;
		}
		gl_cKeyPE = gl_cKeyPH;
	}
	//Printf("Set Org End\n");
}
/***********************************/


//***********************************************

//高级菜单
//机器类型设置F1
//906, 1209, 1509, 1512, 1812
extern void SetMacLengthWith(int MacType);

int GetMacTypeNum(int iCurrType)
{
	int typeNum;
	
	switch(iCurrType)
	{
		case 906:
			typeNum = 0;
			break;

		case 1209:
			typeNum = 1;
			break;

		case 1509:
			typeNum = 2;
			break;

		case 1412:
			typeNum = 3;
			break;

		case 1812:
			typeNum = 3;
			break;
	}
	return typeNum;
	
}

extern void LineReverse(unsigned char line);
void  MacTypeSetManage()
{
	unsigned int i;
	unsigned char ch;
	
	LineReverse(0x02);
	if (gl_sPlotType == 1830)
		i = 1;
	else if (gl_sPlotType == 2030)
		i = 2;
	else if (gl_sPlotType == 2230)
		i = 3;
	else if (gl_sPlotType == 2430)
		i = 4;
	else if (gl_sPlotType == 1320)
		i = 5;
	else if (gl_sPlotType == 2830)// 20100813 新增2.83米机器
		i = 6;
	else if (gl_sPlotType == 3030)// 20150527 新增1.83米机器
		i = 7;
	else
		i = 0;
#if( LANGUAGE == 0 )
	if (i == 1)
		strcpy(gl_cLcdDispBuf[0],"当前:RP_MJ180 ");
	else if (i == 2)
		strcpy(gl_cLcdDispBuf[0],"当前:RP_MJ200 ");
	else if (i == 3)
		strcpy(gl_cLcdDispBuf[0],"当前:RP_MJ220 ");
	else if (i == 4)
		strcpy(gl_cLcdDispBuf[0],"当前:RP_MJ240 ");
	else if (i == 5)
		strcpy(gl_cLcdDispBuf[0],"当前:RP_MJ130 ");
	else if (i == 6)
		strcpy(gl_cLcdDispBuf[0],"当前:RP_MJ283");	// 20100813 新增2.83米机器
	else if (i == 7)
		strcpy(gl_cLcdDispBuf[0],"当前:RP_MJ180");	// 20150527 新增1.83米机器
	else
		strcpy(gl_cLcdDispBuf[0],"当前:RP_MJ260 ");
#else
	if (i == 1)
		strcpy(gl_cLcdDispBuf[0],"Type:SP180");
	else if (i == 2)
		strcpy(gl_cLcdDispBuf[0],"Type:SP200 ");
	else if (i == 3)
		strcpy(gl_cLcdDispBuf[0],"Type:SP220 ");
	else if (i == 4)
		strcpy(gl_cLcdDispBuf[0],"Type:SP240 ");
	else if (i == 5)
		strcpy(gl_cLcdDispBuf[0],"Type:SP135 ");
	else if (i == 6)
		strcpy(gl_cLcdDispBuf[0],"Type:SP283");	// 20100813 新增2.83米机器
	else if (i == 7)
		strcpy(gl_cLcdDispBuf[0],"Type:SP180");	// 20150527 新增1.83米机器
	else
		strcpy(gl_cLcdDispBuf[0],"Type:SP260 ");
#endif
	
	while (1)
	{
	#if( LANGUAGE == 0 )
		if (i == 7)
			strcpy(gl_cLcdDispBuf[1],"选择:RP_MJ180 ");     // 20100926 新增3.03米机器
		else if (i == 6)
			strcpy(gl_cLcdDispBuf[1],"选择:RP_MJ283 ");	// 20100813 新增2.83米机器
		else if (i == 5)
			strcpy(gl_cLcdDispBuf[1],"选择:RP_MJ130 ");
		else if (i == 4)
			strcpy(gl_cLcdDispBuf[1],"选择:RP_MJ240 ");
		else if (i == 3)
			strcpy(gl_cLcdDispBuf[1],"选择:RP_MJ220 ");
		else if (i == 2)
			strcpy(gl_cLcdDispBuf[1],"选择:RP_MJ200 ");
		else if (i == 1)
			strcpy(gl_cLcdDispBuf[1],"选择:RP_MJ180 ");
		else
			strcpy(gl_cLcdDispBuf[1],"选择:RP_MJ260 ");
	#else
		if (i == 7)
			strcpy(gl_cLcdDispBuf[1],"Select:SP180 ");     // 20100926 新增3.03米机器
		else if (i == 6)
			strcpy(gl_cLcdDispBuf[1],"Select:SP283 ");	// 20100813 新增2.83米机器
		else if (i == 5)
			strcpy(gl_cLcdDispBuf[1],"Select:SP130 ");
		else if (i == 4)
			strcpy(gl_cLcdDispBuf[1],"Select:SP240 ");
		else if (i == 3)
			strcpy(gl_cLcdDispBuf[1],"Select:SP220 ");
		else if (i == 2)
			strcpy(gl_cLcdDispBuf[1],"Select:SP200 ");
		else if (i == 1)
			strcpy(gl_cLcdDispBuf[1],"Select:SP180 ");
		else
			strcpy(gl_cLcdDispBuf[1],"Select:SP260 ");
	#endif
		
		lcd_refresh();
		WaitKeyExit(0xff);		//等待有键按下
		ch = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		if (ch == Key_Up)
		{
			if (i == 0)
				i = 7;
			else
				i--;
		}
		else if (ch == Key_Down)
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
					gl_sPlotType = 1830;	 // 20150527 新增1.83米机器
				else if (i == 6)
					gl_sPlotType = 2830;	// 20100813 新增2.83米机器			
				else if (i == 5)
					gl_sPlotType = 1320;
				else if (i == 4)
					gl_sPlotType = 2430;
				else if (i == 3)
					gl_sPlotType = 2230;
				else if (i == 2)
					gl_sPlotType = 2030;
				else if (i == 1)
					gl_sPlotType = 1830;
				else
					gl_sPlotType = 2630;		// 1630 改 2630
				WaitPrintEnd();//等待打印结束
				gl_iMaxYMM = (gl_sPlotType + 3) * Y_PPMM;
				//gl_iGraphWidth = (int)((double)(gl_iMaxYMM - gl_iOriginalPos) / (double)(8 * DotPulseNum));	
				//gl_iMaxYMM += (3 * (MaxMalposedDot + SprinklerSpace) + 2 * AdditionDot + OriginalSideDot + MotorSideDot + 320) * DotPulseNum;
				gl_iMaxYMM += ((MaxMalposedDot + SprinklerSpace) + 2 * AdditionDot + OriginalSideDot + MotorSideDot + 320) * DotPulseNum;
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

//*******************

// 20131206
void NetSetManage(void)
{
	unsigned char ch;
	unsigned char cState;

	if(gl_cLanguageSelect == 0)
		strcpy(gl_cLcdDispBuf[0],"网络管理");
	else
		strcpy(gl_cLcdDispBuf[0],"Network manage");
	strcpy(gl_cLcdDispBuf[1],"            ");
	lcd_refresh();

	cState = 1;
	while (cState)
	{
		WaitKeyExit(0xff);		//等待有键按下
		ch = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (ch)
		{
			case Key_F1:
				MacIpSetManage(MyIP);		// IP
				
				break;

			case Key_F2:
				MacIpSetManage(SubnetMask);	//子网掩码
				
				break;

			case Key_F3:
				MacIpSetManage(GatewayIP);	//网关
				
				break;

			case Key_Parameter:
				//语言设置
				LanguageSelect();
				
				cState = 0;
				break;
				
			case Key_Ok:
			case Key_Pause:
				cState = 0;
				break;
		}
		
		if(gl_cLanguageSelect == 0)
			strcpy(gl_cLcdDispBuf[0],"网络管理");
		else
			strcpy(gl_cLcdDispBuf[0],"Network manage");
		strcpy(gl_cLcdDispBuf[1],"            ");
		lcd_refresh();
	}
	
}

void  MacIpSetManage(unsigned short *sNetWorkValue)
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
		WaitKeyExit(0xff);		//等待有键按下
		ch = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (ch)
		{
			case Key_F1:
			case Key_F3:
				if (cPin > 0)
					cPin--;
				cTemp = cPin * 4 + 1;
				for (i=0;i<cTemp;i++)
					gl_cLcdDispBuf[0][i] = ' ';
				gl_cLcdDispBuf[0][i++] = '*';
				gl_cLcdDispBuf[0][i] = '\0';
				lcd_refresh();
				break;
				
			case Key_F2:
			case Key_F4:
				if (cPin < 3)
					cPin++;
				cTemp = cPin * 4 + 1;
				for (i=0;i<cTemp;i++)
					gl_cLcdDispBuf[0][i] = ' ';
				gl_cLcdDispBuf[0][i++] = '*';
				gl_cLcdDispBuf[0][i] = '\0';
				lcd_refresh();
				break;
				
			case Key_Right:
				if (IPAddr[cPin] < 0xff)
						IPAddr[cPin]+=10;
					else
						IPAddr[cPin] = 0;
					sprintf(gl_cLcdDispBuf[1],"%3d.%3d.%3d.%3d",IPAddr[0],IPAddr[1],IPAddr[2],IPAddr[3]);
					
					lcd_refresh();
				break;

			case Key_Left:
				if (IPAddr[cPin] > 0)
						IPAddr[cPin]-=10;
					else
						IPAddr[cPin] = 0xff;
					sprintf(gl_cLcdDispBuf[1],"%3d.%3d.%3d.%3d",IPAddr[0],IPAddr[1],IPAddr[2],IPAddr[3]);
					
					lcd_refresh();
				break;
				
			case Key_Up:
				if (IPAddr[cPin] < 0xff)
					IPAddr[cPin]++;
				else
					IPAddr[cPin] = 0;
				sprintf(gl_cLcdDispBuf[1],"%3d.%3d.%3d.%3d",IPAddr[0],IPAddr[1],IPAddr[2],IPAddr[3]);
					
				lcd_refresh();
				break;
			case Key_Down:
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
						lcd_display(0,"参数设置失败!");
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


//XY长度校正F3
// +/- 1 增加或减小0.025mm  +/- 10 增加或减小 0.25mm 最大值放大1.25mm
void  XYLengthReviseManage(void)
{
	unsigned char cReadKey;//键值
	char  state;
	char cBuff[17];
	char cMenu;
	int iValue;
	int  iXProp, iYProp;
		
	iXProp = (int)(gl_dXProp * 100);
	iYProp = (int)(gl_dXProp * 100);
	cMenu = 0;
	iValue = iXProp;
	if(gl_cLanguageSelect==0)
		strcpy(gl_cLcdDispBuf[0], "X 轴长度校正");
	else
		strcpy(gl_cLcdDispBuf[0], "Calibrate: X");
	sprintf(gl_cLcdDispBuf[1], "%2d", iXProp);
	lcd_refresh();
		
	state = 1;
	while(state)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (cReadKey)
		{	
			case Key_F3://切换XY设置
				cMenu = (cMenu == 0) ? 1 : 0;
				if(cMenu == 0)
				{
					iYProp = iValue;
					iValue = iXProp;
					if(gl_cLanguageSelect==0)
						strcpy(gl_cLcdDispBuf[0], "X 轴长度校正");
					else
						strcpy(gl_cLcdDispBuf[0], "Calibrate: X");
					sprintf(gl_cLcdDispBuf[1], "%2d", iXProp);
					lcd_refresh();
				}
				else
				{
					iXProp = iValue;
					iValue = iYProp;
					if(gl_cLanguageSelect==0)
						strcpy(gl_cLcdDispBuf[0], "Y 轴长度校正");
					else
						strcpy(gl_cLcdDispBuf[0], "Calibrate: Y");
					sprintf(gl_cLcdDispBuf[1], "%2d", iYProp);
					lcd_refresh();
				}
				break;
			
			case Key_Up:
				iValue++;
				if(iValue > 200)
					iValue = 50;
				sprintf(cBuff, "%d", iValue);
				lcd_display(1, cBuff);
				break;
				
			case Key_Down:
				iValue--;
				if(iValue < 50)
					iValue = 200;
				sprintf(cBuff, "%d", iValue);
				lcd_display(1, cBuff);
				break;
				
			case Key_Left:
				iValue-=10;
				if(iValue < 50)
					iValue = 200;
				sprintf(cBuff, "%d", iValue);
				lcd_display(1, cBuff);
				break;

			case Key_Right:	
				iValue+=10;
				if(iValue > 200)
					iValue = 50;
				sprintf(cBuff, "%d", iValue);
				lcd_display(1, cBuff);
				break;
			
			case Key_Ok:
				if(cMenu == 0)
					iXProp = iValue;
				else
					iYProp = iValue;
				gl_dXProp = (double)iXProp/100;
				gl_dYProp = (double)iYProp/100;
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


//提刀延伸值设置F4/F5
//SetMenuState :
// 1: 提刀延伸值
// 0: 转角提刀延伸值
void  LiftCutExtensionValueManage(void)
{
	unsigned char cReadKey;//键值
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
		LcdDisp( "提刀延伸值",  aLiftCutDispBuff);	
	else
		LcdDisp( "Cut Extension:",  aLiftCutDispBuff);	
	
	Printf("ReadValue Rota[%d], Exter[%d]\n", gl_dCutRotaExtenValue, gl_dCutExtensValue);
	state = 1;
	while(state)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (cReadKey)
		{	
			case Key_Drop:
				SetMenuState = (SetMenuState == 0) ? 1 : 0;
				if (SetMenuState == 0)
				{//提刀延伸值
					iRota = dLiftCutValueTemp;
					dLiftCutValueTemp = iExten;
					sprintf(aLiftCutDispBuff, "%d", dLiftCutValueTemp);
					if(gl_cLanguageSelect == 0)
						LcdDisp( "提刀延伸值",  aLiftCutDispBuff);	
					else
						LcdDisp( "Cut Extension:",  aLiftCutDispBuff);	
				}
				else{//转角提刀延伸值
						iExten = dLiftCutValueTemp;
						dLiftCutValueTemp = iRota;
						sprintf(aLiftCutDispBuff, "%d", dLiftCutValueTemp);
						if(gl_cLanguageSelect == 0)
							LcdDisp( "转角提刀延伸值",  aLiftCutDispBuff);
						else
							LcdDisp( "Cut *Extension:",  aLiftCutDispBuff);
					}
				break;
				
			case Key_Up:
				dLiftCutValueTemp += 1;
				if (dLiftCutValueTemp > 120.0)
					dLiftCutValueTemp = 0;
				sprintf(aLiftCutDispBuff, "%d", dLiftCutValueTemp);
				lcd_display(1, aLiftCutDispBuff);
				break;
				
			case Key_Down:
				dLiftCutValueTemp -= 1;
				if (dLiftCutValueTemp < 0)
					dLiftCutValueTemp = 120;
				sprintf(aLiftCutDispBuff, "%d", dLiftCutValueTemp);
				lcd_display(1, aLiftCutDispBuff);
				break;

			case Key_Right:
				dLiftCutValueTemp += 10;
				if (dLiftCutValueTemp > 120.0)
					dLiftCutValueTemp = 0;
				sprintf(aLiftCutDispBuff, "%d", dLiftCutValueTemp);
				lcd_display(1, aLiftCutDispBuff);
				break;
				
			case Key_Left:
				dLiftCutValueTemp -= 10;
				if (dLiftCutValueTemp < 0)
					dLiftCutValueTemp = 120;
				sprintf(aLiftCutDispBuff, "%d", dLiftCutValueTemp);
				lcd_display(1, aLiftCutDispBuff);
				break;

			case Key_Parameter:// 结束机头回右下或右上
				RP_EndReturnLowerRight();
				state = 0;
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

