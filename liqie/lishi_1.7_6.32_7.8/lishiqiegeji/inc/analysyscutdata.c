#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "..\src\extern.h"
#include "..\src\defconst.h"

#define LT_SOLID					0				//实线
#define LT_DASH					1				//虚线
#define LT_DOT					2				//点线
#define LT_DASHDOT				3				//点划线
#define CONST_PI					3.1415926

static double s_dMoveLength;              //连续移动长度,用于计算线型的各个部分,单位:gl_dLUnit毫米
static unsigned int s_iRevDataPosStart;
static unsigned int s_iRevDataPosEnd;
static long s_lLinetypeStep;              //线型的步长,单位:gl_dLUnit毫米
static long s_lPLCurX;                    //绘图语言当前的XY坐标,单位:gl_dLUnit毫米
static long s_lPLCurY;
static long s_lCutPosX;                   //笔当前的XY坐标(绘图仪坐标系),单位:脉冲
static long s_lCutPosY;
static long s_lCutOffsetX;                //笔的XY坐标偏移量(绘图语言逻辑原点在绘图仪坐标中的位置),单位:脉冲
static long s_lCutOffsetY;			//相对于原点的偏移位置
static long s_lPageMaxX;		  //当前页中X的最大值,单位:gl_dLUnit毫米
static int s_iLineType;                   //线型
static int s_iCurSPNum;			//当前笔号
static int s_iCutPress;			//当前刀压
static char s_iPU00Count;			//pu00
static char cOldCutNo;
static int s_iAmendDownCutOff;	//落刀偏移修正值
static char s_cHaveCtOrder;		//切割文件是否含有CT指令，含有便自动裁纸

extern void Delay(int time);
//extern int ViewError(int iErrorCode);
extern int  get_inp_status(int * value);
extern int  get_inp_status2(int * value);
extern int SetIOBit(unsigned char cBit, unsigned char cEnable);
extern void KBProc(void);
extern void LcdDisp(char * s1, char * s2);
extern void  ReturnOriginal(long lCurPost_XPlus, long lCurPost_YPlus);
extern void DisableIntMsk(void);
extern void EnableIntMsk(void);
extern void lcd_display(unsigned char row, const char * string);
extern void Init_ReturnOrigin(int iAxis);
extern void ID_TestAxisInductor(int iAxis);
extern void AMP_ForwardBackPaper(char cDirection, long lLengthPluse);
extern void SetZAxisDevice(int Ispeed, int Dspeed, int Aspeed);
extern void StopWorkRetu(void);
extern void StopPrint(void);
extern void ITMoveHandpiece(char cAix,int iMovePluse);
extern int get_inp_status(int * value);
extern	void ClearPrintHead(char cOrgState);		//20141211 rwt


void AnalysisRevData(void);
void AnalysisCmd_CT(void);
void AnalysisCmd_IN(void);
void AnalysisCmd_LT(void);
void AnalysisCmd_PUPD(void);
void AnalysisCmd_SP(void);
void AnalysisCmd_RPDefCmd(void);
unsigned char ReadSdramData(void);
void CtrlDownCut(void);
void CtrlUpCut(void);
void GetExternVale(long *lx1, long *ly1, long lx2, long ly2, char cxdir, char cydir);
int GetSirection(long lt1, long lt2);
void CutInit(void);

long f_oldXPostPluse;
/*
void CutOff2Draw(void)
{
	//移到笔工作位置
	long xPluse, yPluse = 0;
	long lDrawPluseY, lDrawPluseX;
	long lOffPostX, lOffPostY;
	
	if(gl_lCurPost_YPluse != 0)
		yPluse = -(gl_lCurPost_YPluse + s_lCutPosY + abs(gl_lPenOffset_YPluse));//
	else
		yPluse = -(s_lCutPosY + abs(gl_lPenOffset_YPluse));//
	
	xPluse = (gl_lCurrFileLenghtPluse - s_lCutPosX) - (long)(5.0*X_PPMM);
	//Printf("scx[%d] scy[%d] x[%d] y[%d] ofX[%d] ofY[%d]\n", s_lCutPosX, s_lCutPosY, xPluse, yPluse, gl_lPenOffset_XPlues, gl_lPenOffset_YPluse);
	
	SetLDPenSpeed(gl_iPenSpeed);
	inp_move2(xPluse, yPluse);
	WaitInpEnd();

	//绘制下三角加竖线
	lDrawPluseY = (long)(5.0 * Y_PPMM);
	lDrawPluseX = (long)(5.0 * X_PPMM);
	
	//gl_lCurPost_YPluse = yPluse + s_lCutPosY + gl_lCurPost_YPluse;
	//gl_lCurPost_XPluse = s_lCutPosX + xPluse + lDrawPluseX;
	lOffPostY = yPluse + s_lCutPosY + gl_lCurPost_YPluse;
	lOffPostX = s_lCutPosX + xPluse + lDrawPluseX;
	//Printf("ofx[%d] ofy[%d]\n", gl_lCurPost_XPluse, gl_lCurPost_YPluse);
	
	CtrDownPen();
	Delay(100);
	
	SetLDPenSpeed(gl_iCutSpeed/2);
	
	inp_move2(0, lDrawPluseY);
	WaitInpEnd();
	Delay(100);
	
	inp_move2(lDrawPluseX, -lDrawPluseY/2);
	WaitInpEnd();
	Delay(100);
	
	inp_move2(-lDrawPluseX, -lDrawPluseY/2);
	WaitInpEnd();
	CtrUpPen();
	
	Delay(1000);
	inp_move2(lDrawPluseX, lDrawPluseY);
	WaitInpEnd();

	CtrDownPen();
	Delay(1000);
	
	inp_move2(0, -lDrawPluseY);
	WaitInpEnd();

	Delay(1000);
	CtrUpPen();

	//回00坐标
	inp_move2(-(gl_lCurPost_XPluse+lOffPostX), abs(lOffPostY));
	WaitInpEnd();
	gl_lCurPost_XPluse = 0;
	gl_lCurPost_YPluse = 0;
}
*/
void CutOffPaper(void)
{
	long xPluse, yPluse = 0;
	int iStatus;
	
	// 移动到Y 轴0位
	if(gl_lCurPost_YPluse != 0)
		yPluse = -(gl_lCurPost_YPluse + s_lCutPosY);
	else
		yPluse = -s_lCutPosY;
	
	xPluse = (gl_lCurrFileLenghtPluse - s_lCutPosX) + (2*X_PPMM);
	
	//SetMoveFrameSpeed(AXIS_X+AXIS_Y);
	//SetReturnOrgSpeed();
	SingleAxisInpDriveSpeed(AXIS_X+AXIS_Y, Frame_SV, 200, 900);
	inp_move2(xPluse, yPluse);
	WaitInpEnd();
	
	if(gl_lCurPost_XPluse != 0)
	{
		gl_lCurPost_XPluse = gl_lCurPost_XPluse + s_lCutPosX + xPluse;
	}
	else
	{
		gl_lCurPost_XPluse = s_lCutPosX + xPluse;
	}
	
	// 偏移9CM后下刀
	gl_lCurPost_YPluse = s_lCutOffsetY/2+PenCarSideInterval * DotPulseNum;
	//inp_move2(0, gl_lCurPost_YPluse);
	//WaitInpEnd();
	//StraightLineADSpeed(AXIS_Y, 10, 200, 300);
	pmove(AXIS_Y, gl_lCurPost_YPluse);
	do{
		get_status(AXIS_Y, &iStatus);
	}while(iStatus!=0);
	
	//Printf("CutOffPaper[%d][%d] gl_cCutUpDownState[%d] gl_cCutPenNum[%d]\n", gl_lCurPost_XPluse, gl_lCurPost_YPluse, gl_cCutUpDownState, gl_cCutPenNum);
	Delay(500);
	CtrlDownCut();

	yPluse = gl_lMachineWidthPluse - gl_lCurPost_YPluse - PenCarSideInterval * DotPulseNum + 40*Y_PPMM;
	//inp_move2(0, yPluse);
	//WaitInpEnd();
	pmove(AXIS_Y, yPluse);
	do{
		get_status(AXIS_Y, &iStatus);
	}while(iStatus!=0);
	
	Delay(500);
	CtrlUpCut();

	gl_lCurPost_YPluse += yPluse;
	gl_lCurPost_XPluse = gl_lMachineLenghtPluse - gl_lCurPost_XPluse;
//	Printf("ymov[%d], yPluse[%d]\n", gl_lCurPost_YPluse, yPluse);
	//SetReturnOrgSpeed();
	inp_move2(gl_lCurPost_XPluse, -(gl_lCurPost_YPluse - (PenCarSideInterval * DotPulseNum)));
	WaitInpEnd();

	gl_lCurPost_XPluse = gl_lMachineLenghtPluse;
	gl_lCurPost_YPluse = PenCarSideInterval * DotPulseNum;

	s_lCutPosX = 0;
	s_lCutPosY = 0;
}

void ReturnRightUp(void)
{
	int iStatus;
//	Printf("1ccx[%d] ccy[%d], plx[%d], ply[%d]\n", s_lCutPosX, s_lCutPosY, gl_lCurPost_XPluse, gl_lCurPost_YPluse);

	SetReturnOrgSpeed();
	if(s_lCutPosX != 0 || s_lCutPosY != 0)
	{
		inp_move2(-s_lCutPosX, -s_lCutPosY);
		WaitInpEnd();
	}
	
	gl_lCurPost_XPluse = gl_lMachineLenghtPluse - gl_lCurPost_XPluse;
	inp_move2(gl_lCurPost_XPluse, -(gl_lCurPost_YPluse-(PenCarSideInterval * DotPulseNum)));
	do
	{
		if (GetIOBit(INDUCTOR_XRIGHT) == XY_INDUCTOR_ORIGIN)
			break;
		get_inp_status(&iStatus);
	}while(iStatus);
			
	gl_lCurPost_XPluse = gl_lMachineLenghtPluse;
	gl_lCurPost_YPluse = PenCarSideInterval * DotPulseNum;
}

void ReturnRightDown(void)
{//回右下位置
	long lXPost, lYPost;
//	Printf("go Point[%d][%d] cpx[%d] cpy[%d]\n", gl_lCurPost_XPluse, gl_lCurPost_YPluse, s_lCutPosX, s_lCutPosY);
//	if(s_lCutPosX == 0 && s_lCutPosY == 0)
//	{//执行过切断纸张
//		lXPost = -gl_lCurPost_XPluse;
//		lYPost = -(gl_lCurPost_YPluse - PenCarSideInterval * DotPulseNum);
//	}
//	else{
			lXPost = -(gl_lCurPost_XPluse + s_lCutPosX);
			lYPost = -(gl_lCurPost_YPluse + s_lCutPosY - PenCarSideInterval * DotPulseNum);
//		}
	//纠错处理 回感应器原点位置
	if((gl_lMaxX - (gl_lCurPost_XPluse+lXPost)) < 0)
		lXPost = -(gl_lMaxX - (gl_lCurPost_XPluse + s_lCutPosX));
	if((gl_lMaxY - (gl_lCurPost_YPluse+lYPost)) < 0)
		lYPost = -(gl_lMaxY - (gl_lCurPost_YPluse + s_lCutPosY) - (PenCarSideInterval * DotPulseNum));
	
//	Printf("go Point[%d][%d]\n", lXPost, lYPost);
	SetReturnOrgSpeed();
	inp_move2(lXPost, lYPost);
	WaitInpEnd();
		
	s_lCutPosX = 0;
	s_lCutPosY = 0;
	gl_lCurPost_XPluse = 0;
	gl_lCurPost_YPluse = (PenCarSideInterval * DotPulseNum);
//	Printf("go Point[%d][%d]\n", gl_lCurPost_XPluse, gl_lCurPost_YPluse);
}

void StartDelayWait(void)
{
	char cDispBuf[17];
	int iDelayS, i;
	lcd_display(0, "Please wait!!!");
	i=0;
	for(iDelayS=RPGL_iPauseTimeValue; iDelayS>0; iDelayS--)
	{
		sprintf(cDispBuf, "Delay Time:%4d", iDelayS);
		lcd_display(1, cDispBuf);
		for(i=0; i<5; i++)
		{
			Delay(2000);
		}
	}
}

void autoMovePaper(long oldXPostPluse)
{
	long lMovPapeLenPluse;
//	Printf("Auto move Paper[%d]\n", gl_lCurrFileLenghtPluse);
	lMovPapeLenPluse = (long)((((double)gl_lCurrFileLenghtPluse / X_PPMM)+RPGL_lPaperOffsetValue) * U_PPMM);
	lMovPapeLenPluse += (long)(((double)oldXPostPluse / X_PPMM) * U_PPMM);
	AMP_ForwardBackPaper(1, lMovPapeLenPluse);
}

extern void WaitKeyExit(unsigned char cKeyValue);
extern void InitFilePaper(void);
extern void  InitSpeedCurve(void);
extern void  InitSpeed(int n);

void RunCutMod(void)
{
	///// 20120809 校验切割数据 待验证//////////////
	int iTempS, iTempE;
	//char cReadData, cCheckState=0;
	long lMovePaperLenX, lMovePaperLenY;
	int iStatus;
	int iPulseNum;
	int iTotalPulseNum;
	//unsigned long taddress, i, chk;
		
	if(gl_lCurrCutFileLength < 0)//	//当前切割文件数据长度
	{
		gl_cCutFileSendFlag = 0;
		gl_lCurrCutFileLength = 0;
		gl_cCutWorkStatus = 0; 
		gl_MacRunState = 0;	
		return;
	}	
	
	s_iRevDataPosStart = 0;			
	s_iRevDataPosEnd = gl_lCurrCutFileLength; //当前切割文件数据长度
	iTempS = s_iRevDataPosStart;
	iTempE = s_iRevDataPosEnd;

/***************调试用
	taddress=CutDataAddr;
	chk = 0;
	Printf("Test Read:\n");
	for(i=0; i< gl_lCurrCutFileLength; i++)
	{
		chk += Readsdram(taddress++,8);
		//Printf("%c", Readsdram(taddress++,8));
	}
	
	Printf("chk:0x%x\n", chk);
	if(gl_cLanguageSelect==0)
		LcdDisp("  欢迎使用", "富怡喷墨切割机 ");
	else
		LcdDisp("Welcome use RP", "Cutter Plotter");
	gl_cCutFileSendFlag = 0;
	gl_lCurrCutFileLength = 0;
	gl_cCutWorkStatus = 0; //已经执行完切割，可以接收下一数据文件
	gl_MacRunState = 0;	//文件打印结束
	return;
*************************/
	/////////////////////////////////////

	gl_cCutFileSendFlag = 0x03;
	Printf("Run Cut mod\n");

	gl_MacRunState = 2;

	f_oldXPostPluse = gl_lCurPost_XPluse; //保存当前脉冲坐标
	cOldCutNo = gl_cCutPenNum;
	if(gl_cLanguageSelect == 0)
		LcdDisp("正在执行切割", "请稍后......");
	else
		LcdDisp("Cut Work...", "Waiting......");

	lMovePaperLenY = gl_iCutDistanceY * DotPulseNum + gl_dMovePaperError_Y * Y_PPMM;
	lMovePaperLenX = gl_iCutDistanceX * DotPulseNum + gl_dMovePaperError_X * X_PPMM; //X向偏移 + 走纸误差
	
	Printf("_>>>LMY:%dpluse, LMX:%dpluse, MX:%fmm, MY:%fmm\n", (int)(lMovePaperLenY), (int)(lMovePaperLenX), gl_dMovePaperError_X, gl_dMovePaperError_Y);
/**/
	SetContinInpCutSpeed( 6 );//衔接速度
	Printf("衔接速度 驱 动xy\n");
	inp_move2(lMovePaperLenX,lMovePaperLenY);
	do
	{
		get_inp_status(&iStatus);
	} while (iStatus != 0); 

	//if(gl_cAutoSendPaperSet != 0)
	//	InitFilePaper();
	SetContinInpCutSpeed( 0 );//速度

	s_iAmendDownCutOff = 0;//落刀偏移修正值
	//4006606677
	while(s_iRevDataPosStart < s_iRevDataPosEnd)
	{
		KBProc();
		AnalysisRevData();
	}
	
	if(gl_cCutUpDownState != 0)
		CtrlUpCut();

	SetContinInpCutSpeed( 6 );//衔接速度	
	// 令Y回原点
	Printf(" 令Y回原点 CEND s_lCutPosY:%d\n", s_lCutPosY);
	if(s_lCutPosY != 0)
	{
		inp_move2(0, -s_lCutPosY);
		do
		{
			get_inp_status(&iStatus);
		} while (iStatus != 0); 
	}
	// 令笔回原点

	Printf("LMY:%dpluse, LMX:%dpluse\n", (int)(lMovePaperLenY), (int)(lMovePaperLenX));
	inp_move2(-(lMovePaperLenX-(long)(gl_dPaperAdjust * X_PPMM)),-lMovePaperLenY);
	do
	{
		get_inp_status(&iStatus);
	} while (iStatus != 0); 

	
	// 令X到下一 位置准备
	Printf("s_lCutPosX:%d\n",s_lCutPosX);
	Printf("CFL:%d, ADCOF:%d\n", gl_lCurrFileLenghtPluse, s_iAmendDownCutOff);
	if(gl_lCurrFileLenghtPluse != s_lCutPosX)
	{
		gl_lCurrFileLenghtPluse += s_iAmendDownCutOff;
		gl_lCurrFileLenghtPluse -= s_lCutPosX;
		Printf("glCFLP:%d\n", gl_lCurrFileLenghtPluse);
		
		inp_move2(-gl_lCurrFileLenghtPluse,0);
		do
		{
			get_inp_status(&iStatus);
		} while (iStatus != 0); 
		gl_lCurrFileLenghtPluse = 0;
	}
	SetContinInpCutSpeed( 0 );//速度	
	CutInit();
	//**************
	
	if(cOldCutNo == 0)
	{
		Printf("CutNo==0\n");
		cOldCutNo = 4;
	}
	gl_cCutPenNum = cOldCutNo;
	
	if((gl_cStopWorkState==1) || (gl_DeleteFileState==1))
	{//按键终止工作  要加要改
		StopPrint();
		if(gl_cLanguageSelect==0)
			LcdDisp("  欢迎使用", "富怡喷墨切割机 ");
		else
			LcdDisp("Welcome to use ", "RICHPEACE CAD");
		return;
	}

	//Printf("@$@$@$@$@$@$@$@\ns_cHaveCtOrder = %d\n", s_cHaveCtOrder);
	if((gl_cOverCutOffPaper == 1) &&(s_cHaveCtOrder== 1))
	{// 自动切断纸张
		if(gl_cAutoSendPaperSet != 0)
			InitFilePaper();
		
		SingleAxisInpDriveSpeed(AXIS_Y,5, 300, 600);
		iPulseNum = fabs(gl_iCutDistanceY)  * DotPulseNum;
		Printf("$$$$*************$$$$\n");
		Printf("iPulseNum = %d\n", iPulseNum);
		if((gl_iOriginalPos != 0) && (gl_iOriginalPos >iPulseNum))
		{
			iPulseNum = gl_iOriginalPos;
		}
		//20150214rwt MJ130右侧纸张靠右，切断补偿距离50mm								
		iPulseNum += Y_PPMM*50;
		
		Printf("iPulseNum = %d\n", iPulseNum);
		Printf("$$$$*************$$$$\n");
		Delay(5000);
		ITMoveHandpiece(AXIS_X, -5*X_PPMM);
		
		CtrlDownCut();	
		ITMoveHandpiece(AXIS_Y, -iPulseNum);
		CtrlUpCut();
		Delay(5000);
		Printf("iPulseNum:%d\n", iPulseNum);

		ITMoveHandpiece(AXIS_Y, iPulseNum);
		Delay(5000);
		
		CtrlDownCut();
		//iPulseNum = fabs(gl_iCutDistanceY)  * DotPulseNum;
		iTotalPulseNum = gl_sPlotType*Y_PPMM;
		Printf("1 iTotalPulseNum:%d O:%d, A:%d B:%d\n", iTotalPulseNum,gl_iAxisYPos, (iTotalPulseNum + gl_iAxisYPos) , gl_iYMaxEffectivePMM);
		if((iTotalPulseNum + gl_iAxisYPos) > gl_iYMaxEffectivePMM)
			iTotalPulseNum -= gl_iAxisYPos;
		//自动裁纸切不断，要优化,左边界差8cm 未切断
		iTotalPulseNum += 100 * Y_PPMM * (gl_sPlotType/1620);
		//SingleAxisInpDriveSpeed(AXIS_Y,5, 300, 600);
		ITMoveHandpiece(AXIS_Y, iTotalPulseNum);
		CtrlUpCut();
		Delay(5000);
		Printf("2 iTotalPulseNum:%d\n", iTotalPulseNum);
		//iTotalPulseNum -= iPulseNum;
		//Printf("3 iTotalPulseNum:%d\n", iTotalPulseNum);
		ITMoveHandpiece(AXIS_Y, -iTotalPulseNum);
		ITMoveHandpiece(AXIS_X, (-100.0 * X_PPMM));

		Printf("RStatus:%d, sp:%d , MoveP:%d\n", gl_cReacterStatus, gl_cSendPaperStar, gl_iMovePaperLength);
		//gl_cReacterStatus = 0;
		//gl_cSendPaperStar = 0;
		//gl_iMovePaperLength = 0;
	}
	
	if(gl_cLanguageSelect==0)
		LcdDisp("  欢迎使用", "富怡喷墨切割机 ");
	else
		LcdDisp("Welcome to use", "RICHPEACE CAD");
//*************延时结束后自动送纸
		
	Printf("====Cut End=====\n");


	gl_cCutFileSendFlag = 0;
	gl_lCurrCutFileLength = 0;
	gl_cCutWorkStatus = 0; //已经执行完切割，可以接收下一数据文件
	gl_MacRunState = 0;	//文件打印结束
	cReviceFlag = 0;
	s_cHaveCtOrder = 0;
	//Printf("@$@$@$@$@$@$@$@\ns_cHaveCtOrder = %d\n", s_cHaveCtOrder);
	//Printf("gl_cPrintStatus: %d\n",gl_cPrintStatus);
}

//******************************
//功能:读SDRAM中数据
//******************************
unsigned char ReadSdramData(void)
{
	unsigned long address;	
	unsigned char cData;
	
	if (s_iRevDataPosStart == s_iRevDataPosEnd)
		cData = 0;
	else
	{
		address = s_iRevDataPosStart + CutDataAddr;
		cData = Readsdram(address, 8);
		s_iRevDataPosStart++;
		//Printf("%x", cData);
	}
	
	return cData;
}

//*******************************************************************
//功能：	从SDRAM取一个有效字符
//*******************************************************************
unsigned char GetValidRevData(void)
{
	unsigned char cData,cFinish;
	unsigned long address;
	
	cFinish = 1;
	while (cFinish)
	{
		while (s_iRevDataPosStart == s_iRevDataPosEnd);		//绘图命令缓冲区空
 		address = s_iRevDataPosStart + CutDataAddr;
 		cData = Readsdram(address, 8);
 		s_iRevDataPosStart++;
		switch (cData)
		{
			case ' ':
			case 0x0a:
			case 0x0d:
			case 0x00:
				break;
			default:
				cFinish = 0;
		}
	}
	return cData;
}

//*******************************************************************
//功能:		看一下绘图命令缓冲区的第一个有效字符(不是空格,回车,换行),
//	   		并将第一个有效字符前的字符删除
//*******************************************************************
unsigned char LookValidRevData(void)
{
	unsigned char cTemp;
	unsigned long address;
	
	while (1)
	{
		while (s_iRevDataPosStart == s_iRevDataPosEnd);		//绘图命令缓冲区空
 		address = s_iRevDataPosStart + CutDataAddr;
 		cTemp = Readsdram(address, 8);
		switch (cTemp)
		{
			case ' ':
			case 0x0a:
			case 0x0d:
			case 0x00:
				s_iRevDataPosStart++;
				break;
			default:
				return cTemp;
		}
	}
	return cTemp;
}

//**************************************
//功能：	分析一个整数
//输入参数：无
//返回值：	整数
//**************************************
long GetAInteger(void)
{
	unsigned char chr, cwe;
	long lValue;
	char cSymbol = 0;
	lValue = 0;
	cwe = 1;
	while (cwe)
	{
		chr = LookValidRevData();
		if(chr == '-')
		{	
			Printf("symbol is -\n");
			s_iRevDataPosStart++;
			cSymbol = 1;
			chr = 0;
			continue;
		}
		
		switch (chr)
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				s_iRevDataPosStart++;
				chr -= 0x30;			//将字符转换成数字
				lValue = lValue * 10L + (unsigned long)chr;
				break;
			case '.':
				s_iRevDataPosStart++;
				while(ReadSdramData() != ',');
				s_iRevDataPosStart--;
				break;
			default:
				cwe = 0;
				break;
		}
	}
	
	if (cSymbol == 1)
		return -lValue;
	else
		return lValue;
	
}


//*******************************************************************
//功能：	分析接收的数据
//输入参数：无
//返回值：	无
//*******************************************************************
void AnalysisRevData(void)
{
	unsigned char chr,cFinish;

	if((gl_DeleteFileState == 1) ||  (gl_cStopWorkState==1))
	{
		s_iRevDataPosStart= 0;
		s_iRevDataPosEnd = 0;
		return;
	}
	

 	cFinish = 1;
 	chr = 0;
 	while (cFinish)
 	{
 		chr = ReadSdramData();	
		switch (chr)
		{
			case ' ': //空格
			case 0x0a://0000 1010 换行键
			case 0x0d://回车键
			case 0x00://空字符
				break;
			default:
				cFinish = 0;
				break;
		}
		
	}
	switch (chr)
	{
		case 'C'://CT自动裁纸
		case 'c':
			AnalysisCmd_CT();
		case 'I': //IN 初始化-HPGL
		case 'i':
			AnalysisCmd_IN();
			break;
		case 'L': //LT线型-HPGL
		case 'l':
			AnalysisCmd_LT();
			break;
		case 'P': //PD 落笔-HPGL;PG 推进整个页面-HPGL;PU 提笔-HPGL
		case 'p':
			AnalysisCmd_PUPD();
			break;
		case 'S': //SP 选笔-HPGL;SP2 选择写字用的笔;其它类型的笔用于画线
		case 's':
			AnalysisCmd_SP();
			break;
		case '!': //! 开始-自定义
			AnalysisCmd_RPDefCmd();
			break;
		case ';':
		case 0x0:
		case 0x0a:
		case 0x0d:
		case 0x20://20150511-rwt
			break;
		default:
			Printf("\nAnalysisRevData ERR s:%d, c:%x\n", s_iRevDataPosStart,chr);
			Printf("\n ER_COMMAND AnalysisRevData 1 \n");
			ViewError(ER_COMMAND);
			break;
	}
}

//初始化
void CutInit(void)
{	
	s_lPLCurX = 0;
	s_lPLCurY = 0;	//调试用
	//x:24mm Y:20mm
	s_lCutOffsetX = 0 ;//201510803 (int)(gl_iCutDistanceX * DotPulseNum);	//20150715-rwt    /*0;//*/
	s_lCutOffsetY = 0 ;//20150803 (int)(gl_iCutDistanceY * DotPulseNum);	//20150715-rwt	//Ts       /* 0;//*/

	s_lCutPosX = 0;
	s_lCutPosY = 0;
	
	s_iLineType = LT_SOLID;
	s_lPageMaxX = 0;
	gl_cINLineNum = 2;//画边框两条直线

	s_iCurSPNum = 0;

	s_iCutPress = 0;

	s_iPU00Count = 0;

}
void AnalysisCmd_CT(void)
{
	
	unsigned char chr;
	chr = GetValidRevData();

	switch (chr)
	{
		case 'T':  //CT 自动裁断纸张
		case 't':
			Printf("AnalysisCmd_CT\n");
			//20150211rwt
			s_cHaveCtOrder = 1;
	 		break;
	}
}

void AnalysisCmd_IN(void)
{
	unsigned char chr;
	chr=ReadSdramData();

	if(chr == 'n' || chr == 'N')
	{
		WaitPrintEnd();		//等待笔的位置到达
		Delay(100);
		// 切割图形初始化
		Printf("AnalysisCmd_IN: %x\n", chr);
		CutInit();
	}
}

void AnalysisCmd_LT(void)
{
	unsigned char chr,cLineType;
	
	chr = GetValidRevData();
	if ((chr != 'T')&&(chr != 't'))	//命令错
	{	
		
		Printf("ER_COMMAND  AnalysisCmd_LT 2 \n");
		ViewError(ER_COMMAND);
		return;
	}
	chr = LookValidRevData();
	if (chr == '-')
	{
		s_iRevDataPosStart++;
		chr = LookValidRevData();
	}
	Printf("AnalysisCmd_LT:%x\n", chr);
	cLineType = chr;
	switch (chr)
	{
		case '0':  //HPGL:点线
		case '1':
		case '2':  //HPGL:虚线
		case '3':
		case '4':  //HPGL:点划线
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
	   	s_iRevDataPosStart++;
    		chr = GetValidRevData();
    		if (chr != ',')
			{
				Printf("ER_COMMAND  AnalysisCmd_LT  3\n");
    			ViewError(ER_COMMAND);
			}
	   	s_lLinetypeStep = IntRound((double)GetAInteger() / HPGL_MM);
   	 	if (s_lLinetypeStep <= 4)
   	    	s_lLinetypeStep = IntRound(4.0 / HPGL_MM);
   	 	chr = GetValidRevData();
		//Printf("chr:%x\n", chr);
   	 	if (chr != ',')
		{
			Printf("ER_COMMAND  AnalysisCmd_LT  4\n");
   	 		ViewError(ER_COMMAND);
	    }
		GetAInteger();
		
    		switch (cLineType)
    		{
    			case '0':  //HPGL:点线
    			case '1':
				//Printf("Is DOT\n");
 		        	s_iLineType = LT_DOT;
         		break;
    			case '2':  //HPGL:虚线
        		case '3':
				//Printf("Is DASH\n");
         			s_iLineType = LT_DASH;
         		break;
        		case '4':  //HPGL:点划线
        		case '5':
        		case '6':
        		case '7':
        		case '8':
        		case '9':
				//Printf("Is DASHDOT\n");
         			s_iLineType = LT_DASHDOT;
         		break;
      		}
    		break;

		default:  		//HPGL:实线
			//Printf("Is Solid\n");
			s_iLineType = LT_SOLID;
		break;
	}
}

#define  PULSE   130//100//60L
#define ADDDECSPEED  300//166//250//166// 41

//反推加速度
void CalSpeedAgainst(long *x,long *y,unsigned char cCount)
{
	long ldx1,ldy1,lx1,ly1,lL,lL2,lL3;
	unsigned char cTemp;
	unsigned int iTemp;

	lL = ADDDECSPEED;						// 2mm为一个加减速段
	lL2 = PULSE;							//线段剩余脉冲数小于60个脉冲时不分割
	lL3 = 0;
	while (cCount > 0)
	{
		lx1 = x[cCount];
		ly1 = y[cCount];
		cCount--;
		ldx1 = labs(x[cCount] - lx1);
		ldy1 = labs(y[cCount] - ly1);
		if (ldx1 > ldy1)
			ldx1 += lL3;
		else
			ldx1 = ldy1 + lL3;
		
		iTemp = (unsigned int)(ldx1 / lL);
		lL3 = ldx1 % lL;
		if (lL3 < lL2)
		{
			if (ldx1 > lL)
				lL3 = 0;
		}
		else if (lL3 > (lL - lL2))
		{
			lL3 = 0;
			iTemp++;
		}
		iTemp += gl_cAccStep[cCount+1];
		if (iTemp > gl_cStep)
			cTemp = gl_cStep;
		else
			cTemp = (unsigned char)iTemp;
		
		if (gl_cAccStep[cCount] > gl_cAccStep[cCount+1])
		{
			if (gl_cAccStep[cCount] > cTemp)
				gl_cAccStep[cCount] = cTemp;
			else
				return;
		}
		else
			return;
	}
}
//反推减速度
void CalSpeedForward(long *x,long *y,unsigned char cCountF,unsigned char cCountE)
{
	long ldx1,ldy1,lx1,ly1,lL,lL2,lL3;
	unsigned char cTemp;
	unsigned int iTemp;
	
	lL = ADDDECSPEED;						//  2mm为一个加减速段
	lL2 = PULSE;							//线段剩余脉冲数小于60个脉冲时不分割
	lL3 = 0;
	cCountE--;
	while (cCountF < cCountE)
	{
		lx1 = x[cCountF];
		ly1 = y[cCountF];
		cCountF++;
		ldx1 = labs(x[cCountF] - lx1);
		ldy1 = labs(y[cCountF] - ly1);
		if (ldx1 > ldy1)
			ldx1 += lL3;
		else
			ldx1 = ldy1 + lL3;
			
		iTemp = (unsigned int)(ldx1 / lL);
		lL3 = ldx1 % lL;
		if (lL3 < lL2)
		{
			if (ldx1 > lL)
				lL3 = 0;
		}
		else if (lL3 > (lL - lL2))
		{
			lL3 = 0;
			iTemp++;
		}
		iTemp += gl_cAccStep[cCountF-1];
		if (iTemp > gl_cStep)
			cTemp = gl_cStep;
		else
			cTemp = (unsigned char)iTemp;
		
		if (gl_cAccStep[cCountF] > gl_cAccStep[cCountF-1])
		{
			if (gl_cAccStep[cCountF] > cTemp)
				gl_cAccStep[cCountF] = cTemp;
			else
				return;
		}
		else
			return;
	}
}

//计算各线段的打印速度
int CalculateSpeed(long *x,long *y,unsigned char cCount)
{
	long ldx1,ldy1,lx1,ly1,lL,lL2,lL3,lL4;
	unsigned char cFlag,cTemp;
	unsigned char cCountAcc,cCountDec;
	unsigned int iTemp;
	
	lL = ADDDECSPEED;//125L;							//  2mm为一个加减速段
	lL2 = PULSE;							//线段剩余脉冲数小于60个脉冲时不分割
	lL3 = 0;
	lL4 = 0;
	cFlag = 0;
	cCountAcc = 0;
	cCountDec = cCount - 1;
	/*
	for(;cCountAcc<cCountDec; cCountAcc++)
		Printf("[%d]",gl_cAccStep[cCountAcc]);
	Printf("\n");
	*/
	cCountAcc = 0;
	
	gl_cAccStep[0] = gl_cMaxSpeed[0];
	gl_cAccStep[cCountDec] = gl_cMaxSpeed[cCountDec];
	for (;(gl_cAccStep[cCountAcc]< gl_cStep) || (gl_cAccStep[cCountDec] < gl_cStep);)
	{
		if (gl_cAccStep[cCountAcc] < gl_cAccStep[cCountDec])
		{//加速步小于减速步，先求加速步
			lx1 = x[cCountAcc];
			ly1 = y[cCountAcc];
			//Printf("A x[%d],y[%d];", lx1, ly1);
			
			
			cCountAcc++;
			if (cCountAcc > cCountDec)
				break;
			ldx1 = labs(x[cCountAcc] - lx1);
			ldy1 = labs(y[cCountAcc] - ly1);
			// 20131104
			//if(ldx1 > 2000*HPGL_MM || ldy1 > 2000*HPGL_MM)
			//	continue;
						
			if (ldx1 > ldy1)
			{
				ldx1 += lL3;
				iTemp = (unsigned int)(ldx1 / lL);
				lL3 = ldx1 % lL;
			}
			else
			{
				ldy1 += lL3;
				iTemp = (unsigned int)(ldy1 / lL);
				lL3 = ldy1 % lL;
				ldx1 = ldy1;
			}
			if (lL3 < lL2)
			{
				if (ldx1 > lL)
					lL3 = 0;
			}
			else if (lL3 > (lL - lL2))
			{
				lL3 = 0;
				iTemp++;
			}
			iTemp += (unsigned int)gl_cAccStep[cCountAcc-1];
			if (iTemp > gl_cStep)
				cTemp = gl_cStep;
			else
				cTemp = (unsigned char)iTemp;
			if (cCountAcc == cCountDec)
			{
				if (cTemp < gl_cAccStep[cCountDec])
				{
					gl_cAccStep[cCountAcc] = cTemp;
					if (cCountDec < (cCount-1))
					{
						cCountDec++;
						if (cCountDec == (cCount-1))
							cFlag = 0;
					}
				}
			}
			else
			{
				gl_cAccStep[cCountAcc] = cTemp;
				if (gl_cAccStep[cCountAcc] > gl_cMaxSpeed[cCountAcc])
				{
					gl_cAccStep[cCountAcc] = gl_cMaxSpeed[cCountAcc];
					iTemp -= (unsigned int)gl_cAccStep[cCountAcc-1];
					if (iTemp > gl_cStep)
						cTemp = gl_cStep;
					else
						cTemp = (unsigned char)iTemp;
						
					if (gl_cAccStep[cCountAcc-1] > gl_cMaxSpeed[cCountAcc])
					{
						if ((gl_cAccStep[cCountAcc-1] - gl_cMaxSpeed[cCountAcc]) > cTemp)
							CalSpeedAgainst(x,y,cCountAcc);
					}
				}
			}
		}
		else
		{
			lx1 = x[cCountDec];
			ly1 = y[cCountDec];
			//Printf("B x[%d],y[%d];", lx1, ly1);
			if (cCountDec == 0)
				break;
			cCountDec--;
			if (cCountAcc > cCountDec)
				break;
			ldx1 = labs(x[cCountDec] - lx1);
			ldy1 = labs(y[cCountDec] - ly1);
			// 20131104
			//if(ldx1 > 2000*HPGL_MM || ldy1 > 2000*HPGL_MM)
			//	continue;
			
			if (ldx1 > ldy1)
			{
				ldx1 += lL4;
				iTemp = (unsigned int)(ldx1 / lL);
				lL4 = ldx1 % lL;
			}
			else
			{
				ldy1 += lL4;
				iTemp = (unsigned int)(ldy1 / lL);
				lL4 = ldy1 % lL;
				ldx1 = ldy1;
			}
			if (lL4 < lL2)
			{
				if (ldx1 > lL)
					lL4 = 0;
			}
			else if (lL4 > (lL - lL2))
			{
				lL4 = 0;
				iTemp++;
			}
			if ((cFlag == 0) && (iTemp > 0))
			{
				iTemp--;
				cFlag = 1;
			}
			iTemp += (unsigned int)gl_cAccStep[cCountDec+1];
			if (iTemp > gl_cStep)
				cTemp = gl_cStep;
			else
				cTemp = (unsigned char)iTemp;
			if (cCountAcc == cCountDec)
			{//重计算速度时，加速步最后节点速度有可能大于减速步最后节点速度
				if (cTemp >= gl_cAccStep[cCountAcc])
				{
					if (cCountAcc > 0)
					{
						if (cTemp > gl_cMaxSpeed[cCountDec])
							gl_cAccStep[cCountDec] = gl_cMaxSpeed[cCountDec];
						else
							gl_cAccStep[cCountDec] = cTemp;
					}
				}
				else
				{
					gl_cAccStep[cCountDec] = cTemp;
					cCountAcc--;
				}
			}
			else
			{
				gl_cAccStep[cCountDec] = cTemp;
				if (gl_cAccStep[cCountDec] > gl_cMaxSpeed[cCountDec])
				{
					iTemp -= (unsigned int)gl_cAccStep[cCountDec+1];
					if (iTemp > gl_cStep)
						cTemp = gl_cStep;
					else
						cTemp = (unsigned char)iTemp;
					if (gl_cAccStep[cCountDec+1] > gl_cMaxSpeed[cCountDec])
					{
						if ((gl_cAccStep[cCountDec+1] - gl_cMaxSpeed[cCountDec]) > cTemp)
						{
							gl_cAccStep[cCountDec] = gl_cMaxSpeed[cCountDec];
							CalSpeedForward(x,y,cCountDec,cCount);
						}
						else
							gl_cAccStep[cCountDec] = gl_cMaxSpeed[cCountDec];
					}
					else
						gl_cAccStep[cCountDec] = gl_cMaxSpeed[cCountDec];
				}
			}
		}	
	}
	
	if (cCountDec > cCountAcc)
	{
		cTemp = cCountAcc;
		cFlag = gl_cAccStep[cCountAcc];
		while (cCountDec > cCountAcc)
		{	//设置加减速外其余线段速度
			cCountAcc++;
			gl_cAccStep[cCountAcc] = cFlag;
		}
		cCountAcc = cTemp;
		while (cCountDec > cCountAcc)
		{
			cCountAcc++;
			if (gl_cAccStep[cCountAcc] > gl_cMaxSpeed[cCountAcc])
			{
				gl_cAccStep[cCountAcc] = gl_cMaxSpeed[cCountAcc];
				CalSpeedAgainst(x,y,cCountAcc);
				CalSpeedForward(x,y,cCountAcc,cCount);
			}
		}
	}
	/*
	Printf("\n****Dec[%d] Acc[%d]******\n", cCountDec, cCountAcc);
	//gl_cAccStep[cCount-1] = 0;
	
	// 打印速度表
	//Printf("\n********CalculateSpeed List[%d]:*********\n",gl_cStep);
	for (iTemp=0; iTemp<gl_cStep; iTemp++)
	{
		Printf("%3d, ", gl_cAccStep[iTemp]);
		if (iTemp%30 == 0)
			Printf("\n");
	}

	Printf("\n*************************************\n");
	*/
	return (cCountAcc+cCountDec);
}
//********************************************************************
//功能：	保持笔的状态(抬起或落下),将笔移到指定位置
//输入参数:x[],y[]新位置,单位:gl_dLUnit毫米,x,y相邻的两个坐标是不同的
//  			iCount 位置点数
//输入参数：无
//返回值：无
//********************************************************************
/*
extern void SetCutSpeed(char AXIS, int initSpeed, int highSpeed, int addSpeed);

void ParaCount(int *initSpeed, int *highSpeed, int *addSpeed)
{
	double dSpeedL,dSpeedH,dAcc;
	double dAccTime;
	long lAcc;
	
	//X,Y轴的初始速度,速度,加速度,减速度
	dSpeedL = (double)gl_iInitV0 * BASE_PPM / (double)SPEED_M;
	dAccTime = (double)(gl_iATime) * 0.001;

	dSpeedH = (double)gl_iCutSpeed * BASE_PPM / (double)SPEED_M;

	dAcc = (dSpeedH - dSpeedL) / dAccTime / 125.0;
	lAcc = IntRound(dAcc);

	*initSpeed = (int)dSpeedL;
	*highSpeed = (int)dSpeedH;
	*addSpeed = (int)lAcc;
}
*/
//*分段*//
void GMoveCutTo(long *x,long *y,unsigned char cCount)
{
	//unsigned int iSpeedH;
	long ldx1,ldy1,lx1,ly1;//,lx2,ly2,lL,lL2,lL3,lL4;
	int iStatus;
	unsigned char cTemp;//,cTemp1,cTemp2;
	//分段算法
	int iSegmentNum,iSegToalX, iSegToalY, iSegPluseX, iSegPluseY, iSegGapPluseX, iSegGapPluseY, iSegCount;
	int iTSegTX, iTSegTY, iTSegPX, iTSegPY, iTSegGPX, iTSegGPY;
	int iMoveGPX, iMoveGPY;
	int iXSegPluse, iYSegPluse;
	int iEndPluseX, iEndPluseY;
	long lTpX, lTpY;
	double dSlope, olddSlope;	//斜率
	//**********
	//unsigned char cCountSpeed1;
	//int initSpeed, highSpeed, addSpeed;
	//double dSpeedH;
	//Printf("CTO[%d]: x[%d], y[%d]\n", cCount, x[0], y[0]);
 	 s_lPLCurX = x[cCount-1];
 	 s_lPLCurY = y[cCount-1];
	 Printf("Start Processing\n");
	 Printf("s_lPLCurX = %ld, s_lPLCurY = %ld\n",s_lPLCurX, s_lPLCurY);	//test1118
	 Printf("s_lPageMaxX = %ld\n",s_lPageMaxX); //test1118	 
	for (cTemp=1;cTemp < cCount;cTemp++)			//计算各点的脉冲坐标
	{
		if (s_lPageMaxX < x[cTemp])		 ////当前页中X的最大值,单位:gl_dLUnit毫米	
		//Printf("x[cTemp] = %d\t",x[cTemp]); //test1118  //20141206 rwt
    			s_lPageMaxX = x[cTemp];
		
		//Printf("x[%d] = %d,y[%d] = %d]\t", cTemp, x[cTemp], cTemp, y[cTemp]);	//test1118 //20141206 rwt
		x[cTemp] = IntRound((double)x[cTemp] * gl_dPosUnitX) + s_lCutOffsetX;
    	y[cTemp] = IntRound((double)y[cTemp] * gl_dPosUnitY) + s_lCutOffsetY;	
		//Printf("x[%d] = %d,y[%d] = %d]\t", cTemp, x[cTemp], cTemp, y[cTemp]);	//test1118 //20141206 rwt
					
		if ((y[cTemp] > gl_iYMaxEffectivePMM))
			ViewError(ER_YOVER);
	}
	Printf("\n************\n");
	iSegPluseX = gl_iCutSectionLen * X_PPMM;
	iSegPluseY = gl_iCutSectionLen * Y_PPMM;
	iSegGapPluseX = gl_iCutSectionGap * gl_dPosUnitX;
	iSegGapPluseY = gl_iCutSectionGap * gl_dPosUnitY;
	iSegToalX = iSegGapPluseX + iSegPluseX;
	iSegToalY = iSegGapPluseY + iSegPluseY;
	iTSegTX = iSegToalX;
	iTSegTY = iSegToalY;
	iTSegPX = iSegPluseX;
	iTSegPY = iSegPluseY;
	iTSegGPX = iSegGapPluseX;
	iTSegGPY = iSegGapPluseY;
	iMoveGPX = 0;
	iMoveGPY = 0;
	/* 解除的注释 start 20141118*/
	Printf("gl_iCutSectionLen[%d],gl_iCutSectionGap[%d]\n",gl_iCutSectionLen, gl_iCutSectionGap);
	Printf("iSegPluseX[%d] iSegPluseY[%d]\n", iSegPluseX, iSegPluseY);
	Printf("iSegGapPluseX[%d] iSegGapPluseY[%d]\n", iSegGapPluseX, iSegGapPluseY);
	Printf("iSegToalX[%d] iSegToalY[%d]\n", iSegToalX, iSegToalY);	
	/* 解除的注释 end 20141118*/
	iEndPluseX = 0;
	iEndPluseY = 0;
	//Printf("\n");
	Printf("s_lCutPosX:%d, s_lCutPosY:%d\n",s_lCutPosX, s_lCutPosY);
	// 1，计算出XY谁是长轴谁是短轴；2，按切割间隙走一定距离后抬刀在走间隙长度后落刀
	SetContinInpCutSpeed(0);
    for (cTemp=1;cTemp < cCount;cTemp++)
	{
		KBProc();
		if((gl_cStopWorkState==1) || (gl_DeleteFileState==1))
		{
			s_iRevDataPosStart = 0;
			s_iRevDataPosEnd = 0;
			return;
		}
		
  		lx1 = x[cTemp];
		ly1 = y[cTemp];
		ldx1 = lx1 - s_lCutPosX;  //X轴需要移动的脉冲数
    	ldy1 = ly1 - s_lCutPosY;  //Y轴需要移动的脉冲数
    	//Printf("ldx[%d], ldy[%d]\n", ldx1, ldy1);
    	if (ldy1 == 0)
			dSlope = 90.0;//CONST_PI / 2.0;
		else if (ldx1 == 0)
			dSlope = 0;
		else
		{
			if(labs(ldx1) < labs(ldy1))
    			dSlope = fabs((double)ldx1/(double)ldy1);
			else
				dSlope = fabs((double)ldy1/(double)ldx1);
		}
		//Printf("\n***********NEW***********\n");
    	//Printf("lx1[%d], ly1[%d], K[%f]\n", (int)(lx1/gl_dPosUnitX), (int)(ly1/gl_dPosUnitY), dSlope);
		do
		{
			get_inp_status(&iStatus);
		} while (iStatus != 0);	

		if((lx1 > 0)&&(gl_lCurrFileLenghtPluse < x[cTemp]))
			gl_lCurrFileLenghtPluse = x[cTemp]; // 20130806 X已走脉冲长度
		//******NEW******************	
		iSegToalX=iTSegTX ;
		iSegToalY=iTSegTY ;
		iSegPluseX=iTSegPX ;
		iSegPluseY=iTSegPY ;
		iSegGapPluseX=iTSegGPX ;
		iSegGapPluseY=iTSegGPY ;
		// 1,判断谁是长轴
		if(labs(ldx1) > labs(ldy1))
		{  //X是长轴
			//Printf("*******X is long axis! ldx[%d] ldy[%d]\n", ldx1, ldy1);
			//SetContinInpCutSpeed();
		   //上一段是否有未满1段脉冲，已走剩余的脉冲数
		   if(iEndPluseX != 0 || iEndPluseY != 0)
		   {
		   		//Printf("EP != 0 iEndPluseX[%d] iEndPluseY[%d]\n", iEndPluseX, iEndPluseY);
				 
				if((labs(ldx1) - (iSegPluseX - iEndPluseX)) > 0)
				{
					//  Printf("***> Seg\n");
					  //******走段剩余脉冲数
    				  //X每段走的脉冲数
    				  iXSegPluse = (iSegPluseX - iEndPluseX) < 0 ? 0 : (iSegPluseX - iEndPluseX);

					  if(ldx1 < 0)
					  	iXSegPluse *= -1;
					  
					   
					  //Y每段走的脉冲数
					  if(ldy1 > 0)
						iYSegPluse = (int)((double)(labs(iXSegPluse)) * dSlope);
					  else if(ldy1 < 0)
						iYSegPluse =-((int)((double)(labs(iXSegPluse)) * dSlope));
					  else
					  	iYSegPluse = 0;	

					  //更新坐标
					  ldx1 -= iXSegPluse;
					  ldy1 -= iYSegPluse;
					  
					  iEndPluseX += labs(iXSegPluse);
					  iEndPluseY += labs(iYSegPluse);
				  //    Printf("A: iXSegP[%d], iYSegP[%d]\n", iXSegPluse, iYSegPluse);
					  
					  if(iYSegPluse != 0 || iXSegPluse != 0)
					  {
						  if(gl_cCutUpDownState != 1)
							  CtrlDownCut();
						  // 行程分段长度
						 
				          inp_move2(-iXSegPluse,iYSegPluse);
						  do
						  {
							get_inp_status(&iStatus);
						  } while (iStatus != 0);	
					  }
					  
					  //**********走间隙
					  //X轴间隙
					  if(labs(ldx1) > (iSegGapPluseX - iMoveGPX))
					  {
					  	  iSegGapPluseX -= iMoveGPX;
						  if(ldx1 < 0)
						  	iSegGapPluseX *= -1;

						  //Y轴间隙
						  if(ldy1 < 0)
							 iSegGapPluseY = -((int)((double)(labs(iSegGapPluseX)) * dSlope));
						  else if(ldy1 == 0)
						     iSegGapPluseY = 0;
						  else
							 iSegGapPluseY = (int)((double)(labs(iSegGapPluseX)) * dSlope);
						  iMoveGPX = 0;
						  iMoveGPY = 0;
					  }
					  else
					  {
						 iSegGapPluseX = ldx1;
						 iSegGapPluseY = ldy1;
						 iMoveGPX = labs(iSegGapPluseX);
						 iMoveGPY = labs(iSegGapPluseY);
					  }

					  
					//  Printf("B: iSegGapPluseX[%d], iSegGapPluseY[%d]\n", iSegGapPluseX, iSegGapPluseY);
					  // 更新坐标
					  ldx1 -= iSegGapPluseX;
					  ldy1 -= iSegGapPluseY;

					  // 行程段间间隙
					  if(gl_cCutUpDownState != 0)
							CtrlUpCut();
  					  inp_move2(-iSegGapPluseX,iSegGapPluseY);
  					  do
  					  {
  						get_inp_status(&iStatus);
  					  } while (iStatus != 0);

					  if(ldx1 != 0)
					  {
					  	iEndPluseX = 0;
					  	iEndPluseY = 0;
					  }
					  else
					  {
					  	iEndPluseX += labs(iSegGapPluseX);
						iEndPluseY += labs(iSegGapPluseY);
						s_lCutPosX = lx1;
				    	s_lCutPosY = ly1;
						olddSlope = dSlope;
					  	continue;
					  }
					  iSegToalX=iTSegTX ;
					  iSegToalY=iTSegTY ;
					  iSegPluseX=iTSegPX ;
					  iSegPluseY=iTSegPY ;
					  iSegGapPluseX=iTSegGPX ;
					  iSegGapPluseY=iTSegGPY ;
				}
				else if((labs(ldx1) - ((iSegPluseX - iEndPluseX)+iSegGapPluseX)) == 0)
				{
					  // Printf("***== Seg\n");
					  //**********走间隙
					  //X轴间隙
					  if(iXSegPluse < 0)
					  	iSegGapPluseX *= -1;
					  else if(iXSegPluse == 0)
						iSegGapPluseX = 0;
					  else
					  	iSegGapPluseX = labs(iSegGapPluseX);

					  //Y轴间隙
					  if(iYSegPluse < 0)
					  	iSegGapPluseY = -((int)((double)(labs(iSegGapPluseX)) * dSlope));
					  else if(iYSegPluse == 0)
						iSegGapPluseY = 0;
					  else
					  	iSegGapPluseY = (int)((double)(labs(iSegGapPluseX)) * dSlope);

					  // 更新坐标
					  ldx1 -= iSegGapPluseX;
					  ldy1 -= iSegGapPluseY;

					  // 行程段间间隙
					  if(gl_cCutUpDownState != 0)
							CtrlUpCut();
  					  inp_move2(-iSegGapPluseX,iSegGapPluseY);
  					  do
  					  {
  						get_inp_status(&iStatus);
  					  } while (iStatus != 0);
					  
					  iEndPluseX = 0;
					  iEndPluseY = 0;
					  s_lCutPosX = lx1;
			    	  s_lCutPosY = ly1;
					  olddSlope = dSlope;
					  continue;
				}
				else
				{
					//Printf("***< Seg\n");
					//if(gl_cCutUpDownState != 1)
					//	CtrlDownCut();
					inp_move2(-ldx1,ldy1);
					iEndPluseX += labs(ldx1);
					iEndPluseY += labs(ldy1);
					s_lCutPosX = lx1;
			    	s_lCutPosY = ly1;	
					olddSlope = dSlope;
					//Printf("EPX[%d] EPY[%d]\n", iEndPluseX, iEndPluseY);
					//Printf("s_lCutPosX[%d], s_lCutPosY[%d]\n", (int)(s_lCutPosX/gl_dPosUnitX), (int)(s_lCutPosY/gl_dPosUnitY));
					continue;
				}
				
		   }
		   //****************************************
		   // 2, 移动脉冲数是否小于段总脉冲数  
		   if(labs(ldx1) < iSegToalX)
		   {// 小于
		   		//Printf("X ***** < gap\n");
				//Printf("ldx1[%d] iSegToalX[%d]\n", ldx1, iSegToalX);
		   		if(gl_cCutUpDownState != 1)
					CtrlDownCut();
				inp_move2(-ldx1,ldy1);
				iEndPluseX += labs(ldx1);
				iEndPluseY += labs(ldy1);
				s_lCutPosX = lx1;
		    	s_lCutPosY = ly1;	
				olddSlope = dSlope;
				//Printf("EPX[%d] EPY[%d]\n", iEndPluseX, iEndPluseY);
				//Printf("s_lCutPosX[%d], s_lCutPosY[%d]\n", (int)(s_lCutPosX/gl_dPosUnitX), (int)(s_lCutPosY/gl_dPosUnitY));
				continue;
		   }
		   else
		   { // 大于, 根据切割分段长度和间隙，计算分段段数
		   	  olddSlope = dSlope;
			  //Printf("X ***** > gap ldx[%d] ldy[%d]\n", ldx1, ldy1);
			  //计算X分段总数
		      iSegmentNum = labs(ldx1) / iSegToalX;		
			 // Printf("iSegmentNum[%d]\n", iSegmentNum);
			  
			  //X每段走的脉冲数
			  if(ldx1 > 0)
				iXSegPluse = iSegToalX - iSegGapPluseY; 
			  else if(ldx1 < 0)
				iXSegPluse = -(iSegToalX - iSegGapPluseX);
			  else
				iXSegPluse = 0;
			  
			  //Y每段走的脉冲数
			  if(ldy1 > 0)
				iYSegPluse = (int)((double)(labs(iXSegPluse)) * dSlope);
			  else if(ldy1 < 0)
				iYSegPluse =-((int)((double)(labs(iXSegPluse)) * dSlope));
			  else
			  	iYSegPluse = 0;			  
		    //  Printf("A: iXSegP[%d], iYSegP[%d]\n", iXSegPluse, iYSegPluse);

			  //X轴间隙
			  if(ldx1 < 0)
			  	iSegGapPluseX *= -1;
			  else if(ldx1 == 0)
				iSegGapPluseX = 0;
			  else
			  	iSegGapPluseX = labs(iSegGapPluseX);

			  //Y轴间隙
			  if(ldy1 < 0)
			  	iSegGapPluseY = -((int)((double)(labs(iSegGapPluseX)) * dSlope));
			  else if(ldy1 == 0)
				iSegGapPluseY = 0;
			  else
			  	iSegGapPluseY = (int)((double)(labs(iSegGapPluseX)) * dSlope);
			  
			//  Printf("B: iSegGapPluseX[%d], iSegGapPluseY[%d]\n", iSegGapPluseX, iSegGapPluseY);
			  //*********************************
		      lTpX = labs(ldx1);
			  lTpY = labs(ldy1);
		      for(iSegCount=0; iSegCount<iSegmentNum; iSegCount++)
		      {
		      	if(gl_cCutUpDownState != 1)
					CtrlDownCut();
				// 行程分段长度
		      	inp_move2(-iXSegPluse,iYSegPluse);
				do
				{
					get_inp_status(&iStatus);
				} while (iStatus != 0);	

				if(lTpX != 0)
					lTpX -= labs(iXSegPluse);
				
				if(lTpY != 0)
					lTpY -= labs(iYSegPluse);
				//****************************
				if(gl_cCutUpDownState != 0)
					CtrlUpCut();
				
				// 行程段间间隙
				inp_move2(-iSegGapPluseX,iSegGapPluseY);
				do
				{
					get_inp_status(&iStatus);
				} while (iStatus != 0);

				if(lTpX != 0)
					lTpX -= labs(iSegGapPluseX);
				
				if(lTpY != 0)
					lTpY -= labs(iSegGapPluseY);
				
			  }

			  // 剩余未分割完的，走完
			  if(lTpX != 0 || lTpY != 0)
			  {
			  	iEndPluseX = lTpX;
				iEndPluseY = lTpY;
				//Printf("residue iTpX[%d] iTpY[%d]\n", lTpX, lTpY);
				if((ldx1 < 0) && (lTpX != 0))
					lTpX *= -1;
				
				if((ldy1 < 0) && (ldy1 != 0))
					lTpY *= -1;
				
				if(gl_cCutUpDownState != 1)
					CtrlDownCut();
				inp_move2(-lTpX,lTpY);
				do
				{
					get_inp_status(&iStatus);
				} while (iStatus != 0);
			  }
			  else
			  {
				iEndPluseX = 0;
				iEndPluseY = 0;
			  }
		   }
		   
		}
		else
		{  //Y是长轴
			//上一段是否有未满1段脉冲，已走剩余的脉冲数
			//SetContinInpCutSpeed(AXIS_Y);
			//Printf("*******Y is long axis! ldx[%d] ldy[%d]\n", ldx1, ldy1);
		   if(iEndPluseX != 0 || iEndPluseY != 0)
		   {
		   		//Printf("EP != 0 iEndPluseX[%d] iEndPluseY[%d]\n", iEndPluseX, iEndPluseY);
				/*
				if(((iSegToalY - iEndPluseY) < 0) && (iSegToalX - iEndPluseX) > 0)
				{ // 从X是长轴变换到Y是长轴
					Printf("X=======>Y olddSlope[%f]\n", olddSlope);
					if(ldx1 > 0)
						iXSegPluse = iSegToalX - iEndPluseX;
					else if(ldx1 < 0)
						iXSegPluse =-(iSegToalX - iEndPluseX);		
					
					if(ldy1 > 0)
						iYSegPluse = (int)((double)(labs(iXSegPluse)) * dSlope);
					else if(ldy1 < 0)
						iYSegPluse =-((int)((double)(labs(iXSegPluse)) * dSlope));
					else
					  	iYSegPluse = 0;

					ldx1 -= iXSegPluse;
				    ldy1 -= iYSegPluse;
					
					inp_move2(-iXSegPluse,iYSegPluse);
					do
					{
						get_inp_status(&iStatus);
					} while (iStatus != 0);	

					iEndPluseX = 0;
					iEndPluseY = 0;
					
					iMoveGPX = 0;
					iMoveGPY = 0;
					Printf("ldx[%d] ldy[%d] iXP[%d] iYP[%d]\n", ldx1, ldy1, iXSegPluse, iYSegPluse);
					//iYSegPluse = iTSegPY;
				}
				*/
				if((labs(ldy1) - (iSegPluseY - iEndPluseY)) > 0)
				{
					//  Printf("***> Seg\n");
					  //******走段剩余脉冲数
    				  //Y每段走的脉冲数
    				  iYSegPluse = (iSegPluseY - iEndPluseY) < 0 ? 0 : (iSegPluseY - iEndPluseY);
					  
					 // if(ldy1 > 0)
					 // 	iYSegPluse = (iSegPluseY - iEndPluseY) < 0 ? 0 : (iSegPluseY - iEndPluseY);
					 // else 
					  if(ldy1 < 0)
					  	iYSegPluse *= -1;
					   
					  //X每段走的脉冲数
					  if(ldx1 > 0)
						iXSegPluse = (int)((double)(labs(iYSegPluse)) * dSlope);
					  else if(ldx1 < 0)
						iXSegPluse =-((int)((double)(labs(iYSegPluse)) * dSlope));
					  else
					  	iXSegPluse = 0;	

					  //更新坐标
					  ldx1 -= iXSegPluse;
					  ldy1 -= iYSegPluse;

					  iEndPluseX += labs(iXSegPluse);
					  iEndPluseY += labs(iYSegPluse);
				      //Printf("A: iXSegP[%d], iYSegP[%d]\n", iXSegPluse, iYSegPluse);

					  if(iYSegPluse != 0 || iXSegPluse != 0)
					  {
						  if(gl_cCutUpDownState != 1)
							  CtrlDownCut();
						  // 行程分段长度
				          inp_move2(-iXSegPluse,iYSegPluse);
						  do
						  {
							get_inp_status(&iStatus);
						  } while (iStatus != 0);	
					  }
					  
					  //Printf("ldy1 > SGPY ldx1:%d ldy1:%d\n", ldx1,ldy1);
					  //**********走间隙
					  //Y轴间隙
					  if(labs(ldy1) > (iSegGapPluseY-iMoveGPY))
					  {
					  	  //Printf("iSGPX;%d, iSGPY:%d iMoveGPY:%d\n", iSegGapPluseX, iSegGapPluseY, iMoveGPY);
						  iSegGapPluseY -= iMoveGPY;
						  if(ldy1 < 0)
						  	iSegGapPluseY *= -1;

						  //X轴间隙
						  if(ldx1 < 0)
							 iSegGapPluseX = -((int)((double)(labs(iSegGapPluseY)) * dSlope));
						  else if(ldx1 == 0)
						     iSegGapPluseX = 0;
						  else
							 iSegGapPluseX = (int)((double)(labs(iSegGapPluseY)) * dSlope);
						  
						  iMoveGPX = 0;
						  iMoveGPY = 0;
					  }
					  else
					  {
						 iSegGapPluseX = ldx1;
						 iSegGapPluseY = ldy1;
						 iMoveGPX = labs(iSegGapPluseX);
						 iMoveGPY = labs(iSegGapPluseY);
					  }
					//  Printf("B: iXSegP[%d], iYSegP[%d]\n", iSegGapPluseX, iSegGapPluseY);
					  
					  // 更新坐标
					  ldx1 -= iSegGapPluseX;
					  ldy1 -= iSegGapPluseY;

					  // 行程段间间隙
					  if(gl_cCutUpDownState != 0)
							CtrlUpCut();
  					  inp_move2(-iSegGapPluseX,iSegGapPluseY);
  					  do
  					  {
  						get_inp_status(&iStatus);
  					  } while (iStatus != 0);
					  
					  if(ldy1 != 0)
					  {
					  	iEndPluseX = 0;
					  	iEndPluseY = 0;
					  }
					  else
					  {
					  	iEndPluseX += labs(iSegGapPluseX);
						iEndPluseY += labs(iSegGapPluseY);
						s_lCutPosX = lx1;
				    	s_lCutPosY = ly1;
						olddSlope = dSlope;
					  	continue;
					  }
					  iSegToalX=iTSegTX ;
					  iSegToalY=iTSegTY ;
					  iSegPluseX=iTSegPX ;
					  iSegPluseY=iTSegPY ;
					  iSegGapPluseX=iTSegGPX ;
					  iSegGapPluseY=iTSegGPY ;
				}
				else if((labs(ldx1) - ((iSegPluseX - iEndPluseX)+iSegGapPluseX)) == 0)
				{
					  // Printf("***== Seg\n");
					  //**********走间隙
					  //Y轴间隙
					  if(iYSegPluse < 0)
					  	iSegGapPluseY *= -1;
					  else if(iYSegPluse == 0)
						iSegGapPluseY = 0;
					  else
					  	iSegGapPluseY = labs(iSegGapPluseY);

					  //X轴间隙
					  if(iXSegPluse < 0)
					  	iSegGapPluseX = -((int)((double)(labs(iSegGapPluseY)) * dSlope));
					  else if(iXSegPluse == 0)
						iSegGapPluseX = 0;
					  else
					  	iSegGapPluseX = (int)((double)(labs(iSegGapPluseY)) * dSlope);

					  // 更新坐标
					  ldx1 -= iSegGapPluseX;
					  ldy1 -= iSegGapPluseY;

					  // 行程段间间隙
					  if(gl_cCutUpDownState != 0)
							CtrlUpCut();
  					  inp_move2(-iSegGapPluseX,iSegGapPluseY);
  					  do
  					  {
  						get_inp_status(&iStatus);
  					  } while (iStatus != 0);
					  
					  iEndPluseX = 0;
					  iEndPluseY = 0;
					  s_lCutPosX = lx1;
			    	  s_lCutPosY = ly1;
					  olddSlope = dSlope;
					  continue;
				}
				else
				{
					//Printf("****< Seg\n");
					//if(gl_cCutUpDownState != 1)
					//	CtrlDownCut();
					inp_move2(-ldx1,ldy1);
					iEndPluseX += labs(ldx1);
					iEndPluseY += labs(ldy1);
					s_lCutPosX = lx1;
			    	s_lCutPosY = ly1;	
					//Printf("EPX[%d] EPY[%d]\n", iEndPluseX, iEndPluseY);
					//Printf("s_lCutPosX[%d], s_lCutPosY[%d]\n", (int)(s_lCutPosX/gl_dPosUnitX), (int)(s_lCutPosY/gl_dPosUnitY));
					olddSlope = dSlope;
					continue;
				}
				
		   }

		   // 2, 移动脉冲数是否小于段总脉冲数  
		   if(labs(ldy1) < iSegToalY)
		   {// 小于
		   		//Printf("****dy < seg\n");
		   		if(gl_cCutUpDownState != 1)
					CtrlDownCut();
				inp_move2(-ldx1,ldy1);
				iEndPluseX += labs(ldx1);
				iEndPluseY += labs(ldy1);
				s_lCutPosX = lx1;
		    	s_lCutPosY = ly1;	
				olddSlope = dSlope;
				//Printf("EPX[%d] EPY[%d]\n", iEndPluseX, iEndPluseY);
				//Printf("s_lCutPosX[%d], s_lCutPosY[%d]\n", (int)(s_lCutPosX/gl_dPosUnitX), (int)(s_lCutPosY/gl_dPosUnitY));
				continue;
		   }
		   else
		   { // 大于, 根据切割分段长度和间隙，计算分段段数
			 // Printf("Y ***** > gap ldx[%d] ldy[%d]\n", ldx1, ldy1);
			  olddSlope = dSlope;
			  //计算X分段总数
		      iSegmentNum = labs(ldy1) / iSegToalY;		
		    //  Printf("iSegmentNum[%d]\n", iSegmentNum);		  
			  //Y每段走的脉冲数
			  if(ldy1 > 0)
				iYSegPluse = iSegToalY - iSegGapPluseY;
			  else if(ldy1 < 0)
				iYSegPluse = -(iSegToalY - iSegGapPluseY);
			  else
			  	iYSegPluse = 0;
			  
			   //X每段走的脉冲数
			  if(ldx1 > 0)
				iXSegPluse = (int)((double)(labs(iYSegPluse)) * dSlope);
			  else if(ldx1 < 0)
				iXSegPluse = -((int)((double)(labs(iYSegPluse)) * dSlope));
			  else
				iXSegPluse = 0;
			  
		     // Printf("A: iXSegP[%d], iYSegP[%d]\n", iXSegPluse, iYSegPluse);

			  // Y轴间隙
			  if(ldy1 < 0)
			  	iSegGapPluseY *= -1;
			  else if(ldy1 == 0)
				iSegGapPluseY = 0;
			  else
			  	iSegGapPluseY = labs(iSegGapPluseY);

			  // X轴间隙
			  if(ldx1 < 0)
			  	iSegGapPluseX = -((int)((double)(labs(iSegGapPluseY)) * dSlope));
			  else if(ldx1 == 0)
				iSegGapPluseX = 0;
			  else
			  	iSegGapPluseX = (int)((double)(labs(iSegGapPluseY)) * dSlope);
			  
			 // Printf("B: iSegGapPluseX[%d], iSegGapPluseY[%d]\n", iSegGapPluseX, iSegGapPluseY);
			  
		      lTpX = labs(ldx1);
			  lTpY = labs(ldy1);
		      for(iSegCount=0; iSegCount<iSegmentNum; iSegCount++)
		      {
		      	if(gl_cCutUpDownState != 1)
					CtrlDownCut();
				// 行程分段长度
		      	inp_move2(-iXSegPluse,iYSegPluse);
				do
				{
					get_inp_status(&iStatus);
				} while (iStatus != 0);	

				if(lTpX != 0)
					lTpX -= labs(iXSegPluse);
				
				if(lTpY != 0)
					lTpY -= labs(iYSegPluse);
				//****************************
				if(gl_cCutUpDownState != 0)
					CtrlUpCut();
				
				// 行程段间间隙
				inp_move2(-iSegGapPluseX,iSegGapPluseY);
				do
				{
					get_inp_status(&iStatus);
				} while (iStatus != 0);

				if(lTpX != 0)
					lTpX -= labs(iSegGapPluseX);
				
				if(lTpY != 0)
					lTpY -= labs(iSegGapPluseY);
				
			  }

			  // 剩余未分割完的，走完
			  if(lTpX != 0 || lTpY != 0)
			  {
			  	iEndPluseX = lTpX;
				iEndPluseY = lTpY;
				//Printf("residue iTpX[%d] iTpY[%d]\n", lTpX, lTpY);
				if((ldx1 < 0) && (lTpX != 0))
					lTpX *= -1;
				
				if((ldy1 < 0) && (lTpY != 0))
					lTpY *= -1;
				
				if(gl_cCutUpDownState != 1)
					CtrlDownCut();
				
				inp_move2(-lTpX,lTpY);
				do
				{
					get_inp_status(&iStatus);
				} while (iStatus != 0);
			  }
			  else
			  {
				iEndPluseX = 0;
				iEndPluseY = 0;
			  }
		   }
		}

		//**************************
		//inp_move2(-ldx1,ldy1);
		//Printf("ldx[%d], ldy[%d]&&", -ldx1, -ldy1);

    	s_lCutPosX = lx1;
    	s_lCutPosY = ly1;	
		Printf("s_lCutPosX[%d], s_lCutPosY[%d]\n", s_lCutPosX, s_lCutPosY);

		//if((lx1 > 0)&&(gl_lCurrFileLenghtPluse < x[cTemp]))
		//	gl_lCurrFileLenghtPluse = x[cTemp]; // 20130806 X已走脉冲长度
    }
	do
  	{
  		get_inp_status(&iStatus);
  	} while (iStatus != 0);
	
	if(gl_cCutUpDownState != 0)
		CtrlUpCut();
	//Printf("*****Run End*****\n");
	return;
}

//********************************************************************
//功能：	保持笔的状态(抬起或落下),将笔移到指定位置
//输入参数:x[],y[]新位置,单位:gl_dLUnit毫米,x,y相邻的两个坐标是不同的
//  			iCount 位置点数
//输入参数：无
//返回值：无
//********************************************************************
void GMovePenToA(long *x,long *y,unsigned char cCount)
{
	unsigned int iSpeedH, iSpeedHX;
	long ldx1,ldy1,lx1,ly1,lx2,ly2,lL,lL2,lL3,lL4;
	int iStatus;
	int cTemp,cTemp1,cTemp2;
	int cCountSpeed1;
	int ic=0;
	int cCountArr[512], cCountArrY[512];
	int i=0;
	
  	s_lPLCurX = x[cCount-1];
 	s_lPLCurY = y[cCount-1];	

	for (cTemp=0,cTemp1=0;cTemp < cCount;cTemp++)			//计算各点的脉冲坐标
	{
		if (s_lPageMaxX < x[cTemp])
    		s_lPageMaxX = x[cTemp];
		//Printf("x[%d]Y[%d],", x[cTemp], y[cTemp]);
		x[cTemp] = IntRound((double)x[cTemp] * gl_dPosUnitX) + s_lCutOffsetX;
    	y[cTemp] = IntRound((double)y[cTemp] * gl_dPosUnitY) + s_lCutOffsetY;
		
		if (y[cTemp] > gl_iYMaxEffectivePMM)
		{  
			ViewError(ER_YOVER);
		}
	}

	ic = CalculateSpeed(x,y,cCount);	//计算各线段起点和终点速度
	Printf("\n******sp[%d] cCount[%d], ic[%d]******\n", s_iCurSPNum, cCount, ic);
	lL = ADDDECSPEED/2;						//2mm为一个加减速段
	lL2 = PULSE;						//线段剩余脉冲数小于60个脉冲时不分割
	lL3 = 0;
	lL4 = 0;
	WaitInpEnd();
	KBProc();
	if((gl_cStopWorkState==1) || (gl_DeleteFileState==1))
	{
		s_iRevDataPosStart = 0;
		s_iRevDataPosEnd = 0;
		return;
	}
	cCountSpeed1 = gl_cAccStep[0];
	iSpeedH = gl_iStepSpeed[cCountSpeed1];
	iSpeedHX = gl_iStepSpeedX[cCountSpeed1];

	if(ic < 2)
	{
		
		//SetContinInpCutSpeed();
		//Printf("PLC = %d, %d\n",s_lPLCurX, s_lPLCurY);
		for (cTemp=1;cTemp < cCount;cTemp++)
		{
			KBProc();
			if((gl_cStopWorkState==1) || (gl_DeleteFileState==1))
			{
				s_iRevDataPosStart = 0;
				s_iRevDataPosEnd = 0;
				return;
			}
	  		lx1 = x[cTemp];
			ly1 = y[cTemp];
			ldx1 = lx1 - s_lCutPosX;  //X轴需要移动的脉冲数
	    	ldy1 = ly1 - s_lCutPosY;  //Y轴需要移动的脉冲数
	    	if (labs(ldx1) > labs(ldy1))
				lL = labs(ldx1);
			else
				lL = labs(ldy1);
			do
			{
				get_inp_status(&iStatus);
			} while (iStatus != 0);		

			if (lL <= 4100L)
				SetSDPenSpeed();
			else
				SetLDPenSpeed();
			
			inp_move2(ldx1,ldy1);
	    	s_lCutPosX = lx1;
	    	s_lCutPosY = ly1;

			if((ldx1 > 0)&&(gl_lCurrFileLenghtPluse < s_lCutPosX))
				gl_lCurrFileLenghtPluse = s_lCutPosX; // 20130806 X已走脉冲长度
			
			if(gl_cStopWorkState==1)
				break;
		}
		return;
	}

  for (cTemp=1;cTemp < cCount;cTemp++)
  {//加速
  	lx1 = x[cTemp];
    ly1 = y[cTemp];
	ldx1 = lx1 - s_lCutPosX;  //X轴需要移动的脉冲数
    ldy1 = ly1 - s_lCutPosY;  //Y轴需要移动的脉冲数

	KBProc();
	if((gl_cStopWorkState==1) || (gl_DeleteFileState==1))
	{
		s_iRevDataPosStart = 0;
		s_iRevDataPosEnd = 0;
		return;
	}

    if ((gl_cAccStep[cTemp-1] == gl_cAccStep[cTemp]) && (gl_cAccStep[cTemp] >= gl_cStep))
    {//恒高速运行
    	do
		{
			get_inp_status(&iStatus);
		} while (iStatus != 0);
		
		FixedSpeeDrive(AXIS_X, iSpeedHX);
		set_startv(AXIS_Y,iSpeedH);//iSpeedH
		set_speed(AXIS_Y,iSpeedH);
		// 获取当前位置   表的构成是从卡中读出来的绝对位置Dx Dy 加 测出来的刀压 组成 
		// 在短距离驱动的时候 可以通过读点改变  但是长距离就不好了
		// 判断 ldx1  ldy1 的行程距离 如果距离大于 20mm(两个方块)
		//查表  看当前位置在表中区域的 计算出的刀压值 
		//修改刀压
		//
		/*第二*/
		//制作一张非常密集的表 单位是50mm*50mm或者更小 由 dx dy 加上测出来的刀压
		
		// 在定时器中断中 读取实际位置的 dx dy 对照表中点 刀压范围当 刀进入某个区域的时候修改刀压并记住修改刀压的位置
		
		//如果还在范围内则不修改刀压
  		inp_move2(ldx1,ldy1);    // 驱动   
		
    	s_lCutPosX = lx1;
    	s_lCutPosY = ly1;
    	lL3 = 0;

		// test
		if(i<512)
		{
			cCountArr[i] = iSpeedHX;
			cCountArrY[i++] = iSpeedH;
		}
		//highCount++;
		
    }
    else
    {
    	//FixedSpeeDrive(AXIS_X, 200);
    	if (labs(ldx1) > labs(ldy1))
    	{// X轴为长轴
	  		lL4 = labs(ldx1) + lL3;//加上小段已打印部分
	  		//iTmpMaxSpX = 0;
  			while (lL4 >= (lL + lL2))
  			{
  				lL4 -= lL;
  				if (ldx1 < 0)						//计算段终点坐标值
  				{
  					lx2 = s_lCutPosX - lL + lL3;
  					ly2 = s_lCutPosY - ldy1 * (lL -lL3) / ldx1;
  				}
  				else
  				{
  					lx2 = s_lCutPosX + lL - lL3;
  					ly2 = s_lCutPosY + ldy1 * (lL -lL3) / ldx1;
  				}
  				lL3 = 0;
  				ldx1 = lx2 - s_lCutPosX;  //X轴需要移动的脉冲数
    			ldy1 = ly2 - s_lCutPosY;  //Y轴需要移动的脉冲数
	    		do
				{
					get_inp_status(&iStatus);
				} while (iStatus != 0);
								
				FixedSpeeDrive(AXIS_X, iSpeedHX);
				set_startv(AXIS_Y,iSpeedH);//iSpeedH
		    	set_speed(AXIS_Y,iSpeedH);
				//获取当前位置
				//
				inp_move2(ldx1,ldy1);    //  驱动 
    			s_lCutPosX = lx2;
    			s_lCutPosY = ly2;
    			ldx1 = lx1 - s_lCutPosX;  //X轴需要移动的脉冲数
    			ldy1 = ly1 - s_lCutPosY;  //Y轴需要移动的脉冲数
    			if (cCountSpeed1 < gl_cAccStep[cTemp])
    				cCountSpeed1++;					//当前速度小于下一线段起始速度，速度++
    			else
    			{
    				cTemp1 = cCountSpeed1 - gl_cAccStep[cTemp];
    				if (cTemp == (cCount-1))
    					cTemp1++;
    				cTemp2 = lL4 / lL;
    				if ((lL4 % lL) > (lL - lL2))
    					cTemp2++;
    				if (cTemp1 < cTemp2)
    					cCountSpeed1++;
    				else if((cTemp1 > cTemp2) && (cCountSpeed1 > 0)){
						//Printf("lL4:[%d] cc[%d], ct1[%d], ct2[%d]", lL4, cCountSpeed1, cTemp1, cTemp2);
    						cCountSpeed1--;
							
    					}
    			}
    			if (cCountSpeed1 > gl_cStep)
    				cCountSpeed1 = gl_cStep;
    			iSpeedH = gl_iStepSpeed[cCountSpeed1];
				iSpeedHX = gl_iStepSpeedX[cCountSpeed1];
				
				if(i<512)
				{
					cCountArr[i] = iSpeedHX;
					cCountArrY[i++] = iSpeedH;
				}
		
    		}
			//Printf("\n");
    	}
    	else
    	{// Y 是长轴
    		lL4 = labs(ldy1) + lL3;
			//iTmpMaxSpY = 0;
  			while (lL4 >= (lL + lL2))
  			{
  				lL4 -= lL;
  				if (ldy1 < 0)
  				{
  					ly2 = s_lCutPosY - lL + lL3;
  					lx2 = s_lCutPosX - ldx1 * (lL -lL3) / ldy1;
  				}
  				else
  				{
  					ly2 = s_lCutPosY + lL - lL3;
  					lx2 = s_lCutPosX + ldx1 * (lL -lL3) / ldy1;
  				}
  				lL3 = 0;
  				ldx1 = lx2 - s_lCutPosX;  //X轴需要移动的脉冲数
    			ldy1 = ly2 - s_lCutPosY;  //Y轴需要移动的脉冲数
	    		do
				{
					get_inp_status(&iStatus);
				} while (iStatus != 0);
				
				FixedSpeeDrive(AXIS_X, iSpeedHX);
				set_startv(AXIS_Y,iSpeedH);//iSpeedH
		    	set_speed(AXIS_Y,iSpeedH);
				//获取当前位置
				//
    			inp_move2(ldx1,ldy1);     // 驱动  

    			s_lCutPosX = lx2;
    			s_lCutPosY = ly2;
    			ldx1 = lx1 - s_lCutPosX;  //X轴需要移动的脉冲数
    			ldy1 = ly1 - s_lCutPosY;  //Y轴需要移动的脉冲数
    			if (cCountSpeed1 < gl_cAccStep[cTemp])
    				cCountSpeed1++;
    			else
    			{
    				cTemp1 = cCountSpeed1 - gl_cAccStep[cTemp];
    				if (cTemp == (cCount-1))
    					cTemp1++;
    				cTemp2 = lL4 / lL;
    				if ((lL4 % lL) > (lL - lL2))
    					cTemp2++;
    				if (cTemp1 < cTemp2)
    					cCountSpeed1++;
    				else if ((cTemp1 > cTemp2) && (cCountSpeed1 > 0))
    					cCountSpeed1--;
    			}
    			if (cCountSpeed1 > gl_cStep)
    				cCountSpeed1 = gl_cStep;
    			iSpeedH = gl_iStepSpeed[cCountSpeed1];
				iSpeedHX = gl_iStepSpeedX[cCountSpeed1];
				if(i<512)
				{
					cCountArr[i] = iSpeedHX;
					cCountArrY[i++] = iSpeedH;
				}
    		}
    	}
  		if (lL4 != 0)
  		{
    		do
			{
				get_inp_status(&iStatus);
			} while (iStatus != 0);

			FixedSpeeDrive(AXIS_X, iSpeedHX);
			set_startv(AXIS_Y,iSpeedH);//iSpeedH
		    set_speed(AXIS_Y,iSpeedH);
			//获取当前位置
			//
  			inp_move2(ldx1,ldy1);    //驱动
			//Printf("z[%d]x[%d]y[%d]c[%d],", gl_iStepSpeed[cCountSpeed1],x[cTemp],y[cTemp],cTemp);
    		s_lCutPosX = lx1;
    		s_lCutPosY = ly1;
    		if (lL4 > (lL - lL2))
    		{
    			if (cCountSpeed1 < gl_cAccStep[cTemp])
    				cCountSpeed1++;
    			else if (cCountSpeed1 > gl_cAccStep[cTemp])
    				cCountSpeed1--;
    			if (cCountSpeed1 > gl_cStep)
    				cCountSpeed1 = gl_cStep;
    			iSpeedH = gl_iStepSpeed[cCountSpeed1];
				iSpeedHX = gl_iStepSpeedX[cCountSpeed1];
				if(i<512)
				{
					cCountArr[i] = iSpeedHX;
					cCountArrY[i++] = iSpeedH;
				}
				lL3 = 0;
    		}
    		else
    			lL3 = lL4;
			//zlencout++;
    	}
    }
	if((lx1 > 0)&&(gl_lCurrFileLenghtPluse < x[cTemp]))
		gl_lCurrFileLenghtPluse = x[cTemp]; // 20130806 X已走脉冲长度
	}
  	SetLDPenSpeed();
	/*
	if(i<512)
		ic = i;
	else
		ic = 512;
	Printf("X:\n");	
	for(i=0; i<ic; i++)
		Printf("[%d]",cCountArr[i]);
		
	Printf("\n");
	Printf("Y:\n");	
	for(i=0; i<ic; i++)
		Printf("[%d]",cCountArrY[i]);
	Printf("\n");
	Printf("************************\n\n");
	*/
  //Printf("\n****h[%d]x[%d]y[%d]z[%d]***\n", highCount, xlencout, ylencout, zlencout);
}


//**************************************************
//功能：	保持笔的状态(抬起或落下),将笔移到指定位置
//输入参数:x,y 新位置,单位:gl_dLUnit毫米
//输出参数：无
//返回值：无
//***************************************************
extern long ReadLp(int axis);
extern void ep(int axis, long wdata);
extern int ReadData(void);
void MoveCutTo(long x,long y)
{
	long lx,ly,ldx,ldy,lL;
	int  iStatus;
	
	Printf("MTO: x[%d], y[%d] s_lCutPosX:%d s_lCutPosY:%d\n", x, y, s_lCutPosX, s_lCutPosY);
	lx = IntRound((double)x * gl_dPosUnitX) + s_lCutOffsetX;
	ly = IntRound((double)y * gl_dPosUnitY) + s_lCutOffsetY;
	Printf("lx:%d, ly:%d\n",lx, ly);

	if (ly > gl_iYMaxEffectivePMM)
	{  //Y轴超界,本来应该 ly < 0就可以了,但为了避免计算误差引起<0就用-20
		ViewError(ER_YOVER);
	}

	if ((lx != s_lCutPosX) || (ly != s_lCutPosY))
	{
		ldx = lx - s_lCutPosX;  //X轴需要移动的脉冲数
		ldy = ly - s_lCutPosY;  //Y轴需要移动的脉冲数
		if (labs(ldx) > labs(ldy))
			lL = labs(ldx);
		else
			lL = labs(ldy);
		//WaitPrintEnd();
		do
		{
			get_inp_status(&iStatus);
		} while (iStatus != 0);	
		//KBProc();
		/*if((gl_cStopWorkState==1) || (gl_DeleteFileState==1))
		{
			s_iRevDataPosStart = 0;
			s_iRevDataPosEnd = 0;
			return;
		}*/
		//Printf("MoveCutTo ldx:%d ldy:%d\n", ldx, ldy);
		//SetContinInpCutSpeed();
		//inp_move2(ldx,ldy);
		FixedSpeeDrive(AXIS_Y, 50);
		FixedSpeeDrive(AXIS_X, 50);

		if (lL <= 4000L)	//
		{  //长轴距离小于10mm,设置短距离行程的笔速
			if (gl_cCutUpDownState == 1)
				SetContinInpCutSpeed(0);
			else
				SetSDPenSpeed();
			
			inp_move2(-ldx,ldy);
		}
		else
		{
			
			if (gl_cCutUpDownState == 1)
				SetContinInpCutSpeed(0);
			else
				SetLDPenSpeed();
			
			inp_move2(-ldx,ldy);
		}

		s_lCutPosX = lx;
		s_lCutPosY = ly;

		if((ldx > 0)&&(gl_lCurrFileLenghtPluse < s_lCutPosX))
			gl_lCurrFileLenghtPluse = s_lCutPosX; // 20130806 X已走脉冲长度
	}
	s_lPLCurX = x;
	s_lPLCurY = y;
}
///************20130809

//顺时针落刀
void CtrlDownCut(void)
{
	SetIOBit(O_CUTDOWN,C_CUTDOWN);
	Delay(gl_iCutDownDelay);
	gl_cCutUpDownState = 1;
}

//逆时针收刀
void CtrlUpCut(void)
{
	//将笔抬起
	SetIOBit(O_CUTDOWN,C_CUTUP);
	Delay(gl_iCutUpDelay);
	gl_cCutUpDownState = 0;
}

// 转角提刀  20130911
void CornerGriffe(long iFx, long iFy, long iBx, long iBy)
{
	int iXdir, iYdir, iStatus;
	long lTempX, lTempY;
	double dTempX, dTempY;
			
	if(gl_cCutUpDownState == 1)
	{//刀具抬起
		//Printf("UP CUT ********\n");
		CtrlUpCut();
	}
	
	// 计算下一坐标方向
	iXdir = GetSirection(iFx, iBx);
	iYdir = GetSirection(iFy, iBy);
	//Printf("C: lDX[%d],lDY[%d] \n", iXdir, iYdir);
	// 计算偏移坐标
	lTempX = labs(iFx - iBx);
	lTempY = labs(iFy - iBy);
	//Printf("A0: lfX[%d],lfY[%d] lbx[%d] lby[%d]  \n", iFx, iFy, iBx, iBy);
	//Printf("A0: lX[%d],lY[%d]  \n", lTempX, lTempY);
	if((lTempX - lTempY) > 0)
	{// X轴为长轴
		if(lTempY == 0)
			lTempY = 0;
		else
		{
			dTempX = (double)lTempX/(double)lTempY;
			lTempY = (long)(((double)gl_dCutRotaExtenValue/dTempX)*DotPulseNum);
		}
		lTempX = (long)((double)gl_dCutRotaExtenValue*DotPulseNum);
	}
	else
	{// Y轴为长轴
		if(lTempX == 0)
		{
			lTempX = 0;
		}
		else
		{
			dTempY = (double)lTempY/(double)lTempX;				
			lTempX = (long)(((double)gl_dCutRotaExtenValue/dTempY)*DotPulseNum);
		}
		lTempY = (long)((double)gl_dCutRotaExtenValue*DotPulseNum);		
	}
	
	if(iXdir == 0)
		lTempX = -lTempX;
	
	if(iYdir == 0)
		lTempY = -lTempY;
	
	// 机头移到偏移坐标处
	//Printf("A1: lX[%d],lY[%d]  dTempX[%d] dTempY[%d]\n", lTempX, lTempY, (int)dTempX, (int)dTempY);
	//RP_SetDoubleAxisADSpeed(gl_iCutSpeed);
	FixedSpeeDrive(AXIS_X, gl_iStepSpeedX[0]);
	FixedSpeeDrive(AXIS_Y, gl_iStepSpeed[0]);
	inp_move2(lTempX,lTempY);
	do
	{
		get_inp_status(&iStatus);
	}while(iStatus!=0);
	
	// 落刀
	WriteZAxisPlus(-gl_iCutDown[gl_cCutPenNum]);
	WriteZAxisDriveCmd(DriveZAxisStartCmd);
	Delay(10);
	while(ZAxisDriveEndState == 0);
	Delay(50);
	
	if(iXdir == 0)
		lTempX = labs(lTempX);
	else
		lTempX = -lTempX;
	
	if(iYdir == 0)
		lTempY = labs(lTempY);
	else
		lTempY = -lTempY;
	//Printf("B: lX[%d],lY[%d] \n**********\n", lTempX, lTempY);
	// 运刀
	inp_move2(lTempX,lTempY);
	do
	{
		get_inp_status(&iStatus);
	}while(iStatus!=0);
}

//*********************

//********************************************
//功能：	将笔落下并移到指定位置
//输入参数:x,y 新位置,单位:gl_dLUnit毫米
//输出参数：无
//返回值：无
//********************************************
void CutDown(long x,long y)
{
	long lXOld,lYOld,lIndexS;
	double dx,dy,dx1,dy1,dL1;
	double dLength,dMoveLengthS,dMoveLengthE;
	double dStep1,dStep2,dStep3,dStep4;
	unsigned char cEnd;
	
	switch (s_iLineType)
	{
		case LT_SOLID:     //实线
		//Printf("CutDown.........\n");
			if (gl_cCutUpDownState == 0)
			{  //将刀具落下
				WaitPrintEnd();
			 	CtrlDownCut();
			}
		
			MoveCutTo(x,y);
			break;
		case LT_DASH:      //虚线
		case LT_DOT:       //点线
		case LT_DASHDOT:   //点划线
		//Printf("is dash\n");
			gl_cINLineNum = 0;
			lXOld = IntRound(((double)s_lCutPosX - (double)s_lCutOffsetX) / gl_dPosUnitX);
			lYOld = IntRound(((double)s_lCutPosY - (double)s_lCutOffsetY) / gl_dPosUnitY);
			dx = (double)x - (double)lXOld;
			dy = (double)y - (double)lYOld;
			dLength = sqrt(dx*dx + dy*dy);
			lIndexS = IntRound(s_dMoveLength / (double)s_lLinetypeStep);
			if (((double)lIndexS * (double)s_lLinetypeStep) < s_dMoveLength)
				lIndexS++;
			if (lIndexS < 1)
				lIndexS = 1;
			lIndexS--;//6358319

			if (dLength > 0.5)
			{
				switch (s_iLineType)
				{
					case LT_DASH:      //虚线
					case LT_DOT:       //点线
						if (s_iLineType == LT_DASH)
						{   //虚线
							dStep1 = (double)s_lLinetypeStep * 0.5;
							dStep2 = (double)s_lLinetypeStep * 0.5;
							//gl_dPrintLength = gl_dPrintLength + dLength * 0.5;
						}
						else
						{   //点线
							dStep1 = (double)s_lLinetypeStep * 0.1;
							dStep2 = (double)s_lLinetypeStep * 0.9;
							//gl_dPrintLength = gl_dPrintLength + dLength * 0.1;
						}
						
						dMoveLengthS = (double)s_lLinetypeStep * (double)lIndexS;
						dMoveLengthE = s_dMoveLength + dLength;
						while (dMoveLengthS < dMoveLengthE)
						{
							cEnd = 0;
							dL1 = dMoveLengthS + dStep1;
							if (dL1 > s_dMoveLength)
							{
								if (dL1 > dMoveLengthE)
								{
									dx1 = (double)x;
									dy1 = (double)y;
									cEnd = 1;
								}
								else
								{
									dx1 = dx / dLength * (dL1 - s_dMoveLength) + lXOld;
									dy1 = dy / dLength * (dL1 - s_dMoveLength) + lYOld;
								}
								
								if (gl_cCutUpDownState == 0)
								{  //将刀具落下
									WaitPrintEnd();
								 	CtrlDownCut();
								}
								MoveCutTo(IntRound(dx1),IntRound(dy1));
							}

							if (cEnd == 0)
							{
								dL1 = dL1 + dStep2;
								if (dL1 > s_dMoveLength)
								{
									if (dL1 > dMoveLengthE)
									{
										dx1 = (double)x;
										dy1 = (double)y;
										cEnd = 1;
									}
									else
									{
										dx1 = dx / dLength * (dL1 - s_dMoveLength) + lXOld;
										dy1 = dy / dLength * (dL1 - s_dMoveLength) + lYOld;
									}
								
									if (gl_cCutUpDownState  != 0)
									{  //将刀具抬起
										WaitPrintEnd();
									 	CtrlUpCut();
									}
									MoveCutTo(IntRound(dx1),IntRound(dy1));
								}
							}

							lIndexS++;
							dMoveLengthS = (double)s_lLinetypeStep * (double)lIndexS;
						}
						break;
					case LT_DASHDOT:   //点划线
						dStep1 = (double)s_lLinetypeStep * 0.55;
						dStep2 = (double)s_lLinetypeStep * 0.15;
						dStep3 = (double)s_lLinetypeStep * 0.15;
						dStep4 = (double)s_lLinetypeStep * 0.15;
						
						//gl_dPrintLength = gl_dPrintLength + dLength * (0.55 + 0.15);
						dMoveLengthS = (double)s_lLinetypeStep * (double)lIndexS;
						dMoveLengthE = s_dMoveLength + dLength;
						while (dMoveLengthS < dMoveLengthE)
						{
							cEnd = 0;
							dL1 = dMoveLengthS + dStep1;
							if (dL1 > s_dMoveLength)
							{
								if (dL1 > dMoveLengthE)
								{
									dx1 = (double)x;
									dy1 = (double)y;
									cEnd = 1;
								}
								else
								{
									dx1 = dx / dLength * (dL1 - s_dMoveLength) + lXOld;
									dy1 = dy / dLength * (dL1 - s_dMoveLength) + lYOld;
								}

								if (gl_cCutUpDownState == 0)
								{  //将刀具落下
									WaitPrintEnd();
								 	CtrlDownCut();
								}
								MoveCutTo(IntRound(dx1),IntRound(dy1));
							}

							if (cEnd == 0)
							{
								dL1 = dL1 + dStep2;
								if (dL1 > s_dMoveLength)
								{
									if (dL1 > dMoveLengthE)
									{
										dx1 = (double)x;
										dy1 = (double)y;
										cEnd = 1;
									}
									else
									{
										dx1 = dx /dLength * (dL1 - s_dMoveLength) + lXOld;
										dy1 = dy /dLength * (dL1 - s_dMoveLength) + lYOld;
									}
								
									if (gl_cCutUpDownState != 0)
									{  //将刀具抬起
										WaitPrintEnd();
									 	CtrlUpCut();
									}
									MoveCutTo(IntRound(dx1),IntRound(dy1));
								}
							}

							if (cEnd == 0)
							{
								dL1 = dL1 + dStep3;
								if (dL1 > s_dMoveLength)
								{
									if (dL1 > dMoveLengthE)
									{
										dx1 = (double)x;
										dy1 = (double)y;
										cEnd = 1;
									}
									else
									{
										dx1 = dx / dLength * (dL1 - s_dMoveLength) + lXOld;
										dy1 = dy / dLength * (dL1 - s_dMoveLength) + lYOld;
									}
								
									if (gl_cCutUpDownState == 0)
									{  //将刀具落下
										WaitPrintEnd();
									 	CtrlDownCut();
									}
									MoveCutTo(IntRound(dx1),IntRound(dy1));
								}
							}

							if (cEnd == 0)
							{
								dL1 = dL1 + dStep4;
								if (dL1 > s_dMoveLength)
								{
									if (dL1 > dMoveLengthE)
									{
										dx1 = (double)x;
										dy1 = (double)y;
										cEnd = 1;
									}
									else
									{
										dx1 = dx / dLength * (dL1 - s_dMoveLength) + lXOld;
										dy1 = dy / dLength * (dL1 - s_dMoveLength) + lYOld;
									}
							    	
									if (gl_cCutUpDownState != 0)
									{  //将刀具抬起
										WaitPrintEnd();
									 	CtrlUpCut();
									}
									MoveCutTo(IntRound(dx1),IntRound(dy1));
								}
							}
							lIndexS++;
							dMoveLengthS = (double)s_lLinetypeStep * (double)lIndexS;
						}
						break;
				}
				//实际走了才加，<0.5时没走不加，以免重复累加
				s_dMoveLength = s_dMoveLength + dLength;
			}
			break;
	}
}

//********************************************************************
//功能：	将笔落下并移到指定位置
//输入参数:x[],y[]新位置,单位:gl_dLUnit毫米,x,y相邻的两个坐标是不同的
//  			iCount 位置点数
//输出参数：无
//返回值：无
//********************************************************************
void GCutDown(long *x,long *y,unsigned char cCount)
{
	unsigned char cNum;
	int iStatus;
	
	gl_cINLineNum = 0;			
	switch (s_iLineType)
	{
		case LT_SOLID:     //实线
			if (gl_cCutUpDownState == 0)
			{  //将刀具落下
				//WaitPrintEnd();
				do
  				{
  					get_inp_status(&iStatus);
  				} while (iStatus != 0);
			 	CtrlDownCut();
			}
			//MoveCutTo(x[1], y[1]);
			GMoveCutTo(x,y,cCount);
			//GMovePenToA(x,y,cCount);
			break;
		case LT_DASH:      //虚线
		case LT_DOT:       //点线
		case LT_DASHDOT:   //点划线
			for (cNum=1;cNum < cCount;cNum++)
				CutDown(x[cNum],y[cNum]);
			break;
	}
}
/*
//20101105
//向后搜索，如果当前命令后PD则继续
//cPDstate:  ==1 下一指令是PD指令
char  BackSearchPD()
{
	unsigned char cData,cFinish, cPDstate;
	unsigned int iPosStart, iPosEnd;
	unsigned long address;

	iPosStart = s_iRevDataPosStart;
	iPosEnd = s_iRevDataPosEnd;
	
	cPDstate = 0;
	cFinish = 1;
	while (cFinish)
	{
		if (iPosStart == iPosEnd)
			return 0;		//绘图命令缓冲区空
 		address = iPosStart + CutDataAddr;
 		cData = Readsdram(address, 8);
 		iPosStart++;
		switch (cData)
		{
			case ' ':
			case 0x0a:
			case 0x0d:
			case 0x00:
			case ';':
				cFinish = 1;
				break;
			case 'P':
			case 'p':
				 address = iPosStart + CutDataAddr;
		 		cData = Readsdram(address, 8);
 				iPosStart++;
				if(cData == 'd' || cData == 'D')
				{
					
					s_iRevDataPosStart = iPosStart;
					cPDstate = 1;
				}
				cFinish = 0;
				break;
			
			default:
				cFinish = 0;
				break;
		}

	}

	return cPDstate;
}
*/
void Cmd_CutDownNew(void)
{
//角度对应直径表

										
	const unsigned char cCirRadTab[75] = {60,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,39,38,
																			  37,37,36,35,34,33,33,32,31,30,30,29,28,27,27,26,25,24,23,23,
																			  22,21,20,20,19,18,18,17,16,16,15,14,14,13,12,12,11,10,10,9,
																			 8,8,7,6,6,5,4,4,3,2,2,1,0,0,0};
	const unsigned int iTanValue[226] = {17,52,87,122,157,192,227,262,297,332,//0.1~1.9
																			 367,402,437,472,507,542,578,612,647,682,
																			 717,752,787,822,857,892,928,963,998,1033,
																			 1069,1104,1139,1175,1210,1246,1281,1317,1352,1388,
																			 1423,1459,1495,1530,1566,1602,1638,1673,1709,1745,
																			 1781,1817,1853,1890,1926,1962,1998,2035,2071,2107,
																			 2144,2180,2217,2253,2290,2327,2364,2401,2438,2475,
																			 2512,2549,2586,2623,2661,2698,2736,2773,2811,2849,
																			 2886,2924,2962,3000,3038,3076,3115,3153,3191,3230,
																			 3269,3307,3346,3385,3424,3463,3502,3541,3581,3620,
																			 3659,3699,3739,3779,3819,3859,3899,3939,3979,4020,
																			 4061,4101,4142,4183,4224,4265,4307,4348,4390,4431,
																			 4473,4515,4557,4599,4642,4684,4727,4770,4813,4856,
																			 4899,4942,4986,5029,5073,5117,5161,5206,5250,5295,
																			 5339,5384,5430,5475,5520,5566,5612,5658,5704,5750,
																			 5797,5844,5890,5938,5985,6032,6080,6128,6176,6224,
																			 6273,6322,6371,6420,6469,6519,6569,6619,6669,6720,
																			 6771,6822,6873,6924,6976,7028,7080,7133,7186,7239,
																			 7292,7346,7400,7454,7508,7563,7618,7673,7729,7785,
																			 7841,7898,7954,8012,8069,8127,8185,8243,8302,8361,
																			 8421,8481,8541,8601,8662,8724,8785,8847,8910,8972,
																			 9036,9099,9163,9228,9293,9358,9424,9490,9556,9623,
																			9691,9759,9827,9896,9965,10000};
																			
	unsigned char chr,cDLength;
	unsigned char cFinish;
	char cIniXAxis_Dir, cIniYAxis_Dir,cNewXAxis_Dir, cNewYAxis_Dir;
	long lX,lY;
	long lPointX[255], rPointX[255];
	long lPointY[255], rPointY[255];
	long oldPointX, oldPointY;
	unsigned char cCount,cTemp, rCount;
	long lTempX,lTempY;
	int iValue;
	double dTempLength, dPrintLength;
	double dPropor,dPropor1;
	double oldPropor;//角度
	unsigned char cFlag;
	double dTempX,dTempY;
	double OlddPropor1;
	int i;
	char cNextPost;
	
	//变量初始化
	lPointX[0] = s_lPLCurX;//当前坐标X
	lPointY[0] = s_lPLCurY;//当前坐标Y
	rPointX[0] = s_lPLCurX;
	rPointY[0] = s_lPLCurY;
	
	gl_cMaxSpeed[0] = 0xff;//每条线段打印结束时的最大速度
	//dPrintLength = 0.0;      
	cCount = 0;//统计当前有多少条线段
	rCount = 0;
	cFinish = 0;//判断当前的PD命令是否已经结束，1:结束
	cFlag = 0;
	dPropor = 0;//角度
	oldPropor = 0;
	oldPointX = 0; 
	oldPointY = 0;
	dPropor1 = 0;
	OlddPropor1 = 0;
	lX = s_lPLCurX;
	lY = s_lPLCurY;
	cIniXAxis_Dir = 0; 
	cIniYAxis_Dir = 0;
	cNewXAxis_Dir = 0;
	cNewYAxis_Dir = 0;
	cNextPost = 0;
	
	while (cFinish == 0)
	{
		KBProc();
		if((gl_cStopWorkState==1) || (gl_DeleteFileState==1))
		{
			s_iRevDataPosStart = 0;
			s_iRevDataPosEnd = 0;
			return;
		}
		
		lX = GetAInteger();//获取X
		chr = GetValidRevData();//读一个值
		if (chr == ',')
		{
			lY = GetAInteger();//获取Y
			chr = GetValidRevData();//读一个值
			if (chr != ',')
				cFinish = 1;
		}
		else
		{
				cFinish = 1;
		}
		Printf("*************cFinish[%d] lx[%d] ly[%d]\n", cFinish, lX, lY);
		//当前X1Y1坐标与下一组X2Y2坐标比较
		if ((lPointX[cCount] != lX) || (lPointY[cCount] != lY) 
				|| ((cFinish == 1) && (cFlag == 1)))
		{
			dPropor1 = dPropor;
			lTempX = labs(lX - lPointX[cCount]);//X移动长度
			lTempY = labs(lY - lPointY[cCount]);//Y移动长度
			dTempX = (double)lTempX;//备份X
			dTempY = (double)lTempY;//备份Y
			Printf("R lx[%d] ly[%d]\n", lX, lY);
			dTempLength = sqrt(dTempX * dTempX + dTempY * dTempY);//
			dPrintLength += dTempLength;
			if (cFinish == 0)
			{
				if (lTempX < lTempY)
					lTempX = lTempY;
				if (lTempX < 10)
				{
					cFlag = 1;
					continue;
				}
				else
					cFlag = 0;
			}
			
			lTempX = lX - lPointX[cCount];
			lTempY = lY - lPointY[cCount];
			if (lTempY == 0)
				dPropor = 90.0;//CONST_PI / 2.0;
			else if (lTempX == 0)
				dPropor = 0;
			else
			{//求线段倾斜角度dPropor = (y2-y1)/(x2-x1)
				dPropor = (double)lTempX / (double)lTempY;
				if (fabs(dPropor) > 1.0)
				{
					dPropor = (double)lTempY / (double)lTempX;
					iValue = (int)IntRound(dPropor * 10000.0);
					dPropor = 45.0;
					if (iValue < 0)
					{
						iValue = 0 - iValue;
						cDLength = 1;
					}
					else
						cDLength = 0;
					for (chr=0;chr < 220;chr++)
					{
						cTemp = chr + 5;
						if (iValue < iTanValue[cTemp])
						{
							while (cTemp != chr)
							{
								if (iValue <= iTanValue[chr])
									break;
								else
									chr++;
							}
							dPropor = 90.0 - (double)chr * 0.2;
							break;
						}
						else
							chr = cTemp + 1;
					}
					
					if (chr > 219)
					{
						for (;chr < 225;chr++)
						{
							if (iValue <= iTanValue[chr])
								break;
						}
						dPropor = 90.0 - (double)chr * 0.2;
					}			
				}
				else
				{
					iValue = (int)IntRound(dPropor * 10000.0);
					dPropor = 45.0;
					if (iValue < 0)
					{
						iValue = 0 - iValue;
						cDLength = 1;
					}
					else
						cDLength = 0;
					for (chr=0;chr < 220;chr++)
					{
						cTemp = chr + 5;
						if (iValue < iTanValue[cTemp])
						{
							while (cTemp != chr)
							{
								if (iValue <= iTanValue[chr])
									break;
								else
									chr++;
							}
							dPropor = (double)chr * 0.2;
							break;
						}
						else
							chr = cTemp + 1;
					}
					if (chr > 219)
					{
						for (;chr < 225;chr++)
						{
							if (iValue <= iTanValue[chr])
								break;
						}
						dPropor = (double)chr * 0.2;
					}			
				}
				if (cDLength == 1)
					dPropor = 0.0 - dPropor;
//				dPropor = atan(dPropor);
			}
			Printf("{ P[%d],",(int)dPropor*1000);

			if(cNextPost == 0)
			{
				cNextPost = 1;
				oldPropor = dPropor;
				cCount++;
				lPointX[cCount] = lX;
				lPointY[cCount] = lY;
				rCount++;
				rPointX[rCount] = lX;
				rPointY[rCount] = lY;
				oldPointX = lX;
				oldPointY = lY;

				cIniXAxis_Dir = GetSirection(lPointX[cCount-1], lPointX[cCount]);/*
																					a - b =1
																					a > b =2
																					a < b =0
																				*/
				cIniYAxis_Dir = GetSirection(lPointY[cCount-1], lPointY[cCount]);
				
				cNewXAxis_Dir = cIniXAxis_Dir;
				cNewYAxis_Dir = cIniYAxis_Dir;
			
			if (gl_cCutUpDownState == 0)
			{/*
				//落刀前退20落刀，校正刀方向
				lTempX = labs(lPointX[cCount] - lPointX[cCount-1]);
				lTempY = labs(lPointY[cCount] - lPointY[cCount-1]);
				if((lTempX - lTempY) > 0)
				{// X轴为长轴
					dTempX = (double)lTempX/(double)lTempY;
					lTempX = DowCutOffset;
					lTempY = (long)(DowCutOffset/dTempX);
				}
				else
				{// Y轴为长轴
					dTempY = (double)lTempY/(double)lTempX;
					lTempX = (long)(DowCutOffset/dTempY);
					lTempY = DowCutOffset;	
				}

				//rPointX[rCount] += lTempX;	
				//rPointY[rCount] += lTempY;
				
				if(cIniXAxis_Dir == 0)
					lTempX = -lTempX;
				if(cIniYAxis_Dir == 0)
					lTempY = -lTempY;
				//Printf("L20==> lTX[%d] lTY[%d] \n", s_lCutPosX, s_lCutPosY, rPointX[rCount], rPointY[rCount]);
				s_lCutPosX += (lTempX*gl_dPosUnitX);
				s_lCutPosY += (lTempY*gl_dPosUnitX);
				//s_iAmendDownCutOff += s_lCutPosX; //
				//Printf("L20==> lTX[%d] lTY[%d] rpx[%d] rpy[%d] ADCO:%d\n", lTempX, lTempY, s_lCutPosX, s_lCutPosY, s_iAmendDownCutOff);
				//SetContinInpCutSpeed();
				SetLDPenSpeed();
				inp_move2(lTempX*gl_dPosUnitX, lTempY*gl_dPosUnitX);
				do
				{
					get_inp_status(&iValue);
				}while(iValue!=0);
				*/
				
			}
		
				Printf("Init*****\n");
				Printf("{IniDirX[%d]Y[%d], NewDirX[%d][%d]}\n", cIniXAxis_Dir, cIniYAxis_Dir, cNewXAxis_Dir, cNewYAxis_Dir);
				Printf("cCount[%d] lPX1[%d] lPY1[%d] lX[%d] lY[%d]\n", cCount, lPointX[0], lPointY[0], lX, lY);
				Printf("****lPX2[%d] lPY21[%d]*****\n", lPointX[1], lPointY[1]);
				Printf("S[%d][X0:%d  Y0:%d][X1:%d, Y1:%d], ", cCount, lPointX[cCount-1], lPointY[cCount-1] , lPointX[cCount], lPointY[cCount] );
			}
			else if (cCount > 0)
			{//求两线段之间的角度

				if (lTempY == 0)
				{
					lTempY = lPointX[cCount] - lPointX[cCount-1];
					lTempY ^= lTempX;
					if (lTempY < 0)
						dPropor1 = dPropor + fabs(dPropor1);
					else
						dPropor1 = dPropor - fabs(dPropor1);
				}
				else if (lPointY[cCount] == lPointY[cCount-1])
				{
					lTempY = lPointX[cCount] - lPointX[cCount-1];
					lTempY ^= lTempX;
					if (lTempY < 0)
						dPropor1 = dPropor1 + fabs(dPropor);
					else
						dPropor1 = dPropor1 - fabs(dPropor);
				}
				else
				{
					dPropor1 = fabs(dPropor - dPropor1);
					lTempX = lPointY[cCount] - lPointY[cCount-1];
					lTempX ^= lTempY;
					if (lTempX < 0)
						dPropor1 = 180.0 - dPropor1;//CONST_PI - dPropor1;
				}
				// 20131019 使用速度曲线
				if (dPropor1 < 0.1)//0.001745)//<0.1度 0.2////
				{
					gl_cMaxSpeed[rCount] = gl_cMaxStep[60];
					//Printf("{00000dP[%f], ms[%d]}, ss[%d]} ** ", dPropor1, gl_cMaxSpeed[rCount], gl_iStepSpeed[gl_cMaxSpeed[rCount]]);
				}
				else
				{//求该线段最大起始速度
					dTempLength = 360.0 * dTempLength / CONST_PI / dPropor1;//   2.0 * dTempLength / dPropor1;
					lTempX = IntRound(dTempLength * HPGL_MM / 5.0);   //绘图仪语言单位0.025 *L /5
					if (lTempX > 60L)
						cDLength = 60;
					else
						cDLength = (unsigned char)lTempX;
					if (dPropor1 > 14.4)//(20131202)14.4 //0.26 ) //15度
						chr = 74;
					else
						chr = (unsigned char)(dPropor1 * 5.0);//(20131202)5.0//((180.0 * dPropor1 * 5.0) / CONST_PI);
					
					chr = cCirRadTab[chr];
					if (chr < cDLength)
						gl_cMaxSpeed[rCount] = gl_cMaxStep[chr];
					else
					{//以免同一线段加减速次数太多，太明显
						cDLength = (chr + cDLength) / 2;
						gl_cMaxSpeed[rCount] = gl_cMaxStep[cDLength];
					}
					//Printf("{dP[%f], ms[%d]}, ss[%d]} ** ", dPropor1, gl_cMaxSpeed[rCount], gl_iStepSpeed[gl_cMaxSpeed[rCount]]);
				}
				//***********************************************
				//if( OlddPropor1 != dPropor1)
				//{
				//	OlddPropor1 = dPropor1;
				//	Printf("{*Propor[%f],px[%d] py[%d] lx[%d],ly[%d]*},", OlddPropor1,lPointX[cCount], lPointY[cCount], lX, lY);
				//}
				
				// 20131127
				cIniXAxis_Dir = cNewXAxis_Dir;			//跟踪方向 ///9
				cIniYAxis_Dir = cNewYAxis_Dir;
				//**************
				
				cNewXAxis_Dir = GetSirection(lPointX[cCount], lX);
				cNewYAxis_Dir = GetSirection(lPointY[cCount], lY);
				
				//if(((cIniXAxis_Dir != cNewXAxis_Dir) || (cIniYAxis_Dir != cNewYAxis_Dir)))// || ((cCount == 1)&&(dPropor1>14.4))////9
				if((dPropor1>14.4) || (dPropor1<-14.4)) ///9
				{
					Printf("{IniDirX[%d]Y[%d], NewDirX[%d][%d] dPropor1:%f}\n", cIniXAxis_Dir, cIniYAxis_Dir, cNewXAxis_Dir, cNewYAxis_Dir, dPropor1);
					Printf("cCount[%d] lPX[%d] lPY[%d] lX[%d] lY[%d]\n", cCount, lPointX[cCount], lPointY[cCount], lX, lY);
					//方向发生改变，调用开始切割当前段
										
					//if((dPropor1>14.4) || (dPropor1<-14.4))////9
					//{////9
						Printf("\ndPropor1[%f] lX:%d lY:%d r[%d & %d] L[%d & %d]\n", dPropor1,lX, lY, rPointX[rCount], rPointY[rCount],lPointX[cCount], lPointY[cCount] );
					/*
					if(gl_iCutOffset != 0)
					{
						lTempX = labs(lPointX[cCount] - lPointX[cCount-1]);
						lTempY = labs(lPointY[cCount] - lPointY[cCount-1]);
						if((lTempX - lTempY) > 0)
						{// X轴为长轴
							dTempX = (double)lTempX/(double)lTempY;
							lTempX = gl_iCutOffset;
							lTempY = (long)(gl_iCutOffset/dTempX);
						}
						else
						{// Y轴为长轴
							dTempY = (double)lTempY/(double)lTempX;
							
							lTempX = (long)(gl_iCutOffset/dTempY);
							lTempY = gl_iCutOffset;	
						}
						s_iAmendDownCutOff = lTempX * gl_dPosUnitX;
						Printf("EXT ADCO:%d\n", s_iAmendDownCutOff);
						//Printf("lTX[%d], lTY[%d] gl_iCutOffset[%d]\n", lTempX, lTempY, gl_iCutOffset);//gl_dCutExtensValue
						//Printf("init Xdir[%d], Ydir[%d], new Xdir[%d] Y[%d]\n",cIniXAxis_Dir, cIniYAxis_Dir, cNewXAxis_Dir, cNewYAxis_Dir );						
						rCount++;
						if(cIniXAxis_Dir == 2)
							rPointX[rCount] = rPointX[rCount-1] - lTempX;	//
						else if(cIniXAxis_Dir == 0)	
							rPointX[rCount] = rPointX[rCount-1] + lTempX;	//
						else
							rPointX[rCount] = rPointX[rCount-1];
						
						if(cIniYAxis_Dir == 2)
							rPointY[rCount] = rPointY[rCount-1] - lTempY;
						else if(cIniYAxis_Dir == 0)
							rPointY[rCount] = rPointY[rCount-1] + lTempY;
						else
							rPointY[rCount] = rPointY[rCount-1];
						
						rCount++;
						rPointX[rCount] = rPointX[rCount-2];
						rPointY[rCount] = rPointY[rCount-2];	
					}
					*/
						Printf("rCount[%d][%d & %d][%d & %d]\n", rCount, rPointX[rCount], rPointY[rCount], rPointX[rCount-1], rPointY[rCount-1] );
					//}///9

					cIniXAxis_Dir = cNewXAxis_Dir;			//跟踪方向
					cIniYAxis_Dir = cNewYAxis_Dir;
					
					for(i=0; i<rCount+1; i++)
						Printf("[%d & %d]", rPointX[i], rPointY[i]);
					Printf("\n");
					Printf("***********************************\n");
					//Delay(300);
					// 20131019 使用速度曲线
					gl_cMaxSpeed[0] = 0;
					gl_cMaxSpeed[rCount] = 0;
					for(lTempX=0;lTempX<rCount+1;lTempX++);
//						Printf("[GsA[%d],MSP[%d],,spx[%d]] ", gl_cMaxSpeed[lTempX], gl_iStepSpeed[gl_cMaxSpeed[lTempX]], gl_iStepSpeedX[gl_cMaxSpeed[lTempX]]);
					Printf("\n");
					//*********************
					
					lTempX = rPointX[rCount];
					lTempY = rPointY[rCount];
					if(rCount == 0)
					{
						rPointX[1] = rPointX[0];
						rPointY[1] = rPointY[0];
						rCount = 1;
					}
					
					GCutDown(rPointX,rPointY,(rCount+1));
					//if(gl_cStopWorkState==1)
						//return;
					if(gl_cSetPenAttr[gl_cCutPenNum] == 2) // 20131208
					{// 转角提刀
						Printf("Run CornerGriffe dPropor1[%f] rx[%d] ry[%d] lx[%d] ly[%d]\n", dPropor1,lTempX, lTempY, lX, lY);
						//if((dPropor1>14.4) || (dPropor1<-14.4))	/////9
						if(gl_dCutRotaExtenValue != 0)
							CornerGriffe(lTempX,lTempY, lX, lY);
					}
					rCount=1;	//20140719
					rPointX[rCount] = lX;
					rPointY[rCount] = lY;
				  }
				  else{				
						rCount++;
						rPointX[rCount] = lX;
						rPointY[rCount] = lY;
				  	}
				  
				cCount++;
				lPointX[cCount] = lX;
				lPointY[cCount] = lY;
			}
		}

		if ((rCount == 249) && (cFinish == 0))
		{
			Printf("1********rCount[%d]\n", rCount+1);
			// 20131019 使用速度曲线
			gl_cMaxSpeed[0] = 0;
			gl_cMaxSpeed[rCount] = 0;
			for(lTempX=0;lTempX<rCount+1;lTempX++)
				Printf("Ls[%d],", gl_cMaxSpeed[lTempX]);
			Printf("rCount[%d]\n", rCount);
			//********************
			GCutDown(rPointX,rPointY,(rCount+1));
			rPointX[0] = s_lPLCurX;
			rPointY[0] = s_lPLCurY;
			rCount = 0;
			cNextPost = 0;
			Printf("\n...rCount>249...\n");
		}
		else if(cCount == 249)
			{
				Printf("&&&&&&&&&&cCount=249  lx[%d]ly[%d]\n", lPointX[cCount], lPointY[cCount]);
				lPointX[0] = lPointX[cCount-1];
				lPointY[0] = lPointX[cCount-1];
				lPointX[1] = lPointX[cCount];
				lPointY[1] = lPointX[cCount];
				cCount = 1;
			}
			
	}
	Printf("\n...End...\n");

	if(rCount < 2)
	{/*
		if (rCount == 1)
		{
			rPointX[0] = rPointX[1];
			rPointY[0] = rPointY[1];
		}*/
		//if(rCount == 0)
			Printf("RX[%d],RY[%d],SPLX[%d],SPLY[%d]\n", rPointX[0], rPointY[0],s_lPLCurX, s_lPLCurY);
		
		Printf("rX[%d] rY[%d] s_lPLCurX[%d] s_lPLCurY[%d]\n", rPointX[0], rPointY[0], s_lPLCurX, s_lPLCurY);
		if(gl_dCutExtensValue != 0)
			GetExternVale(&rPointX[rCount], &rPointY[rCount], s_lPLCurX, s_lPLCurY, cNewXAxis_Dir, cNewYAxis_Dir);
		
		Printf("nX[%d] nY[%d]\n", rPointX[rCount], rPointY[rCount]);
		//rPointX[1] = rPointX[0];
		//rPointY[1] = rPointY[0];
		GCutDown(rPointX,rPointY,rCount+1);
		////////////////////
	}
	else
	{
		// 20131019 使用速度曲线
		gl_cMaxSpeed[0] = 0;
		gl_cMaxSpeed[rCount] = 0;
		
		//**************************
		Printf("\n@@@@@ rc>1****rPointX[%d] rPointY[%d]***rCount[%d]\n", rPointX[rCount], rPointY[rCount], rCount);
		if(gl_dCutExtensValue != 0)
			GetExternVale(&rPointX[rCount], &rPointY[rCount], rPointX[rCount-1], rPointY[rCount-1], cNewXAxis_Dir, cNewYAxis_Dir);
		Printf("\n##### rc>1****rPointX[%d] rPointY[%d]***rCount[%d]\n", rPointX[rCount], rPointY[rCount], rCount);
		for(i=0; i< rCount+1; i++)
			Printf("[%d & %d],", rPointX[i], rPointY[i]);
		for(lTempX=0;lTempX<rCount+1;lTempX++)
			Printf("[Gs[%d],MSP[%d],spx[%d]] ", gl_cMaxSpeed[lTempX], gl_iStepSpeed[gl_cMaxSpeed[lTempX]], gl_iStepSpeedX[gl_cMaxSpeed[lTempX]]);
		Printf("rCount[%d]\n", rCount);
		GCutDown(rPointX,rPointY,(rCount+1));
	}
}

int GetSirection(long lt1, long lt2)
{
	if((lt1 - lt2) == 0)
		return 1;	// +
	else if((lt1 - lt2) > 0)
		return 2;  //
	else
		return 0;  // -
}


void GetExternVale(long *lx1, long *ly1, long lx2, long ly2, char cxdir, char cydir)
{
	long lTempX, lTempY;
	double dTempX, dTempY;
		
	lTempX = labs(*lx1 - lx2);
	lTempY = labs(*ly1 - ly2);
	if((lTempX - lTempY) > 0)
	{// X轴为长轴
		if(lTempY == 0)
			lTempY = 0;
		else
		{	
			dTempX = (double)lTempX/(double)lTempY;
			lTempY = (long)(gl_dCutExtensValue/dTempX);
		}
		lTempX = gl_dCutExtensValue;
	}
	else
	{// Y轴为长轴
		if(lTempX == 0)
			lTempX = 0;
		else
		{
			dTempY = (double)lTempY/(double)lTempX;
			lTempX = (long)(gl_dCutExtensValue/dTempY);
		}		
		lTempY = gl_dCutExtensValue;	
	}
	
	if(cxdir == 2)
		*lx1 = *lx1 - lTempX;	//
	else if(cxdir == 0)	
		*lx1 = *lx1 + lTempX;	//0
						
	if(cydir == 2)
		*ly1 = *ly1 - lTempY;
	else if(cydir == 0)
		*ly1 = *ly1 + lTempY;
	
}

//*************************************
//功能：	落笔
//输入参数：无
//返回值：	无
//*************************************
/*
void Cmd_CutDown(void)
{
//角度对应直径表
	const unsigned char cCirRadTab[75] = {60,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,39,38,
																				37,37,36,35,34,33,33,32,31,30,30,29,28,27,27,26,25,24,23,23,
																				22,21,20,20,19,18,18,17,16,16,15,14,14,13,12,12,11,10,10,9,
																				8,8,7,6,6,5,4,4,3,2,2,1,0,0,0};
	const unsigned int iTanValue[226] = {17,52,87,122,157,192,227,262,297,332,//0.1~1.9
																			367,402,437,472,507,542,578,612,647,682,
																			717,752,787,822,857,892,928,963,998,1033,
																			1069,1104,1139,1175,1210,1246,1281,1317,1352,1388,
																			1423,1459,1495,1530,1566,1602,1638,1673,1709,1745,
																			1781,1817,1853,1890,1926,1962,1998,2035,2071,2107,
																			2144,2180,2217,2253,2290,2327,2364,2401,2438,2475,
																			2512,2549,2586,2623,2661,2698,2736,2773,2811,2849,
																			2886,2924,2962,3000,3038,3076,3115,3153,3191,3230,
																			3269,3307,3346,3385,3424,3463,3502,3541,3581,3620,
																			3659,3699,3739,3779,3819,3859,3899,3939,3979,4020,
																			4061,4101,4142,4183,4224,4265,4307,4348,4390,4431,
																			4473,4515,4557,4599,4642,4684,4727,4770,4813,4856,
																			4899,4942,4986,5029,5073,5117,5161,5206,5250,5295,
																			5339,5384,5430,5475,5520,5566,5612,5658,5704,5750,
																			5797,5844,5890,5938,5985,6032,6080,6128,6176,6224,
																			6273,6322,6371,6420,6469,6519,6569,6619,6669,6720,
																			6771,6822,6873,6924,6976,7028,7080,7133,7186,7239,
																			7292,7346,7400,7454,7508,7563,7618,7673,7729,7785,
																			7841,7898,7954,8012,8069,8127,8185,8243,8302,8361,
																			8421,8481,8541,8601,8662,8724,8785,8847,8910,8972,
																			9036,9099,9163,9228,9293,9358,9424,9490,9556,9623,
																			9691,9759,9827,9896,9965,10000};
																			
	unsigned char chr,cDLength;
	unsigned char cFinish;
	long lX,lY;
	long lPointX[251];
	long lPointY[251];
	unsigned char cCount,cTemp;
	long lTempX,lTempY;
	int iValue;
	double dTempLength;	//dPrintLength
	double dPropor,dPropor1;//角度
	unsigned char cFlag;
	double dTempX,dTempY;

	//变量初始化
	lPointX[0] = s_lPLCurX;//当前坐标X
	lPointY[0] = s_lPLCurY;//当前坐标Y
	gl_cMaxSpeed[0] = 0xff;//每条线段打印结束时的最大速度
	//dPrintLength = 0.0;      
	cCount = 0;//统计当前有多少条线段
	cFinish = 0;//判断当前的PD命令是否已经结束，1:结束
	cFlag = 0;
	dPropor = 0;//角度
	lX = s_lPLCurX;
	lY = s_lPLCurY;
	
	while (cFinish == 0)
	{
		lX = GetAInteger();//获取X
		chr = GetValidRevData();//读一个值
		if (chr == ',')
		{
			lY = GetAInteger();//获取Y
			chr = GetValidRevData();//读一个值
			if (chr != ',')
				cFinish = 1;
		}
		else
		{
				cFinish = 1;
		}
		//当前X1Y1坐标与下一组X2Y2坐标比较
		if ((lPointX[cCount] != lX) || (lPointY[cCount] != lY) 
				|| ((cFinish == 1) && (cFlag == 1)))
		{
			dPropor1 = dPropor;
			lTempX = labs(lX - lPointX[cCount]);//X移动长度
			lTempY = labs(lY - lPointY[cCount]);//Y移动长度
			dTempX = (double)lTempX;//备份X
			dTempY = (double)lTempY;//备份Y
			//dTempLength = sqrt(dTempX * dTempX + dTempY * dTempY);//
			//dPrintLength += dTempLength;
			if (cFinish == 0)
			{
				if (lTempX < lTempY)
					lTempX = lTempY;
				if (lTempX < 20)
				{
					cFlag = 1;
					continue;
				}
				else
					cFlag = 0;
			}
			
			lTempX = lX - lPointX[cCount];
			lTempY = lY - lPointY[cCount];
			if (lTempY == 0)
				dPropor = 90.0;//CONST_PI / 2.0;
			else if (lTempX == 0)
				dPropor = 0;
			else
			{//求线段倾斜角度dPropor = (y2-y1)/(x2-x1)
				dPropor = (double)lTempX / (double)lTempY;
				if (fabs(dPropor) > 1.0)
				{
					dPropor = (double)lTempY / (double)lTempX;
					iValue = (int)IntRound(dPropor * 10000.0);
					dPropor = 45.0;
					if (iValue < 0)
					{
						iValue = 0 - iValue;
						cDLength = 1;
					}
					else
						cDLength = 0;
					for (chr=0;chr < 220;chr++)
					{
						cTemp = chr + 5;
						if (iValue < iTanValue[cTemp])
						{
							while (cTemp != chr)
							{
								if (iValue <= iTanValue[chr])
									break;
								else
									chr++;
							}
							dPropor = 90.0 - (double)chr * 0.2;
							break;
						}
						else
							chr = cTemp + 1;
					}
					
					if (chr > 219)
					{
						for (;chr < 225;chr++)
						{
							if (iValue <= iTanValue[chr])
								break;
						}
						dPropor = 90.0 - (double)chr * 0.2;
					}			
				}
				else
				{
					iValue = (int)IntRound(dPropor * 10000.0);
					dPropor = 45.0;
					if (iValue < 0)
					{
						iValue = 0 - iValue;
						cDLength = 1;
					}
					else
						cDLength = 0;
					for (chr=0;chr < 220;chr++)
					{
						cTemp = chr + 5;
						if (iValue < iTanValue[cTemp])
						{
							while (cTemp != chr)
							{
								if (iValue <= iTanValue[chr])
									break;
								else
									chr++;
							}
							dPropor = (double)chr * 0.2;
							break;
						}
						else
							chr = cTemp + 1;
					}
					if (chr > 219)
					{
						for (;chr < 225;chr++)
						{
							if (iValue <= iTanValue[chr])
								break;
						}
						dPropor = (double)chr * 0.2;
					}			
				}
				if (cDLength == 1)
					dPropor = 0.0 - dPropor;
//				dPropor = atan(dPropor);
			}
			if (cCount > 0)
			{//求两线段之间的角度
				if (lTempY == 0)
				{
					lTempY = lPointX[cCount] - lPointX[cCount-1];
					lTempY ^= lTempX;
					if (lTempY < 0)
						dPropor1 = dPropor + fabs(dPropor1);
					else
						dPropor1 = dPropor - fabs(dPropor1);
				}
				else if (lPointY[cCount] == lPointY[cCount-1])
				{
					lTempY = lPointX[cCount] - lPointX[cCount-1];
					lTempY ^= lTempX;
					if (lTempY < 0)
						dPropor1 = dPropor1 + fabs(dPropor);
					else
						dPropor1 = dPropor1 - fabs(dPropor);
				}
				else
				{
					dPropor1 = fabs(dPropor - dPropor1);
					lTempX = lPointY[cCount] - lPointY[cCount-1];
					lTempX ^= lTempY;
					if (lTempX < 0)
						dPropor1 = 180.0 - dPropor1;//CONST_PI - dPropor1;
				}
				if (dPropor1 < 0.2)//0.001745)//<0.1度
					gl_cMaxSpeed[cCount] = gl_cMaxStep[60];
				else
				{//求该线段最大起始速度
					dTempLength = 360.0 * dTempLength / CONST_PI / dPropor1;//   2.0 * dTempLength / dPropor1;
					lTempX = IntRound(dTempLength * HPGL_MM / 5.0);
					if (lTempX > 60L)
						cDLength = 60;
					else
						cDLength = (unsigned char)lTempX;
					if (dPropor1 > 14.4)//0.26 ) //15度
						chr = 74;
					else
						chr = (unsigned char)(dPropor1 * 5.0);//((180.0 * dPropor1 * 5.0) / CONST_PI);
					chr = cCirRadTab[chr];
					if (chr < cDLength)
						gl_cMaxSpeed[cCount] = gl_cMaxStep[chr];
					else
					{//以免同一线段加减速次数太多，太明显
						cDLength = (chr + cDLength) / 2;
						gl_cMaxSpeed[cCount] = gl_cMaxStep[cDLength];
					}
				}
			}
			cCount++;
			lPointX[cCount] = lX;
			lPointY[cCount] = lY;
		}
		if ((cCount == 249) && (cFinish == 0))
		{
			if (gl_iInitV0 > 30)
			{
				gl_cMaxSpeed[0] = 1;
				gl_cMaxSpeed[cCount] = 1;
			}
			else
			{
				gl_cMaxSpeed[0] = 0;
				gl_cMaxSpeed[cCount] = 0;
			}
			
			GCutDown(lPointX,lPointY,(cCount+1));
			lPointX[0] = s_lPLCurX;
			lPointY[0] = s_lPLCurY;
			gl_cMaxSpeed[0] = 0xff;
			cCount = 0;
			dPropor = 0;
		}
	}
	
	//Printf("cCount:%d\n", cCount);
	if (cCount == 0)
		return;
	else if (cCount == 1)
		CutDown(lPointX[1],lPointY[1]);
	else
	{
		if (gl_iInitV0 > 30)
		{
			gl_cMaxSpeed[0] = 1;
			gl_cMaxSpeed[cCount] = 1;
		}
		else
		{
			gl_cMaxSpeed[0] = 0;
			gl_cMaxSpeed[cCount] = 0;
		}
		GCutDown(lPointX,lPointY,(cCount+1));
	}
}
*/

//**************************************
//功能：	将笔抬起并移到指定位置
//输入参数:x,y 新位置,单位:gl_dLUnit毫米
//输出参数：无
//返回值：无
//**************************************
void CutUp(long x,long y)
{
	//int iPenSpeed;
	int iStatus;
	
	if (gl_cCutUpDownState != 0)
	{  //将刀具收起
		//WaitPrintEnd();
		do
		{
			get_inp_status(&iStatus);
		} while (iStatus != 0);	
	 	CtrlUpCut();
	}
	//Printf("CutUp\n");
	//iPenSpeed = GetPenSpeed();
	//Printf("Move Cut To X[%d],Y[%d], iPenSpeed: %d\n", x, y, iPenSpeed);
	MoveCutTo(x,y);
	s_dMoveLength = 0.0;
}

//*****************************************************
//功能：	PG 推进整个页面-HPGL
//输入参数：无
//返回值：	无
//*****************************************************
void Cmd_PG(void)
{

/* 20120410
	long lTemp;
	Printf("Cmd_PG......\n");
	lTemp = (long)(s_lPageMaxX * gl_dXProp);
	CutUp(lTemp,0);
	WaitPrintEnd();
*/	
	/*
	RollPaper(); //送纸
	
	if (lTemp >= IntRound(398.0 / HPGL_MM))
		GivePaper();  								//直接送纸
	else
		PreFWGivePaper();  						//先往前走一段纸，判断纸的位置后再送纸
	WaitPrintEnd();
	*/
	//判断能否进行自动裁纸
	//Printf("@@@@@@@@@@@@@@@\n");
	//Printf("s_cHaveCtOrder = %d\n", s_cHaveCtOrder);
	//s_cHaveCtOrder = 1;//20150211rwt
	//Printf("s_cHaveCtOrder = %d\n", s_cHaveCtOrder);
	if(gl_cCutUpDownState == 1)
		CtrlUpCut();
	
	Delay(200);
	//CutInit();
}

//************************************************************
//功能：	提笔打印(因为是抬笔走，所以直接走到最后一点就可以了）
//输入参数：无
//返回值：	无
//************************************************************
void Cmd_CutUp(void)
{
	unsigned char chr;
	long lX,lY;
	long lPointX,lPointY;
	
	lPointX = s_lPLCurX;
	lPointY = s_lPLCurY;
	while (1)
	{
		lX = GetAInteger();
		chr = GetValidRevData();
		if (chr == ',')
		{
			lY = GetAInteger();
			lPointX = lX;
			lPointY = lY;
			chr = GetValidRevData();
			if (chr != ',')
				break;
		}
		else
		{
			lPointX = lX;
			break;
		}
	}
	//*********20130809
	/*
	if((gl_cGoOOPointState == 0) && (lPointX == 0 && lPointY == 0))
	{//不执行PU00命令	
		WaitInpEnd();
		if (gl_cCutUpDownState == 1)
		    CtrlUpCut();
		s_iPU00Count++;		
		return;
	}
	*/
	//*******************
	Printf("Cmd_CutUp: LX: %d, LY:%d\n", lPointX, lPointY);
	if ((lPointX != s_lPLCurX) || (lPointY != s_lPLCurY))
		CutUp(lPointX,lPointY);
}

void AnalysisCmd_PUPD(void)
{
	unsigned char chr;
	
	chr = GetValidRevData();
	switch (chr)
	{
		case 'D':  //PD 落笔-HPGL
		case 'd':
			Printf("AnalysisCmd_PD\n");
			//Cmd_CutDown();
			Cmd_CutDownNew();//
	 		break;
		case 'G':  //PG 推进整个页面-HPGL
		case 'g':
			//Printf("AnalysisCmd_PG\n");
			Cmd_PG();
			break;
		case 'U':  //PU 提笔-HPGL
		case 'u':
			Printf("AnalysisCmd_PU\n");
			Cmd_CutUp();
			break;
		default:
			Printf("AnalysisCmd_PUPD ERR : [%c,%x]\n", chr,chr);
			Printf("ER_COMMAND  AnalysisCmd_PUPD  5\n");
			ViewError(ER_COMMAND);
			break;
	}
}

//**************************************
//功能：	SP 选笔
//输入参数：无
//返回值：	无
//**************************************
void Cmd_SP(void)
{
	if(gl_cCutUpDownState == 1)
		CtrlUpCut();
	
	gl_cCutPenNum = (int)GetAInteger();
	Printf("SP[%d]\n", gl_cCutPenNum);
	s_iCurSPNum = gl_cCutPenNum;
}

void AnalysisCmd_SP(void)
{
	unsigned char chr;
	
	chr = GetValidRevData();
	switch (chr)
	{
		case 'P':  //SP 选笔-HPGL
		case 'p':
			//Printf("AnalysisCmd_SP\n");
			Cmd_SP();
			break;
		default:
			Printf("Cmd_SP ERR\n");
			Printf("ER_COMMAND  AnalysisCmd_SP  6\n");
			ViewError(ER_COMMAND);
			break;
	}
}

void AnalysisCmd_RPDefCmd(void)
{
	unsigned char chr;
	Printf("AnalysisCmd_RPDefCmd\n");
	while (1)
	{
		chr = ReadSdramData();
		if ( chr == 0)
			continue;
		else
		{
			switch (chr)
			{
				case '@':
					return;
				default:
					break;
			}
		}
	}
}



