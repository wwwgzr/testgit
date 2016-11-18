#ifndef __EXTERN_H__
#define __EXTERN_H__

#include "..\src\defconst.h"

extern int gl_time;
extern unsigned char	gl_cCanRevChar;
extern unsigned	char	gl_cLCDStatus;

extern unsigned char	gl_cIOReadValue;
extern unsigned char	gl_cIOWriteValue;
extern unsigned char	gl_cKeyBuf[KeyBufLength];
extern unsigned char	gl_cKeyPH;
extern unsigned char	gl_cKeyPE;
extern char	gl_cLcdDispBuf[2][16];

extern unsigned char	gl_cUartRecBuf[UartRecBufLength];
extern unsigned char	gl_cUartSendBuf[UartSendBufLength];
extern unsigned char	gl_cUartRecBufPH;
extern unsigned char	gl_cUartSendBufPH;
extern unsigned char	gl_cUartSendBufPE;
extern unsigned	char	gl_cRecFrameOver;

extern unsigned int   gl_iEthnetRecBufPH;
extern unsigned int   gl_iEthnetRecBufPE;
extern unsigned short TxEthnetFrameBuffer[50];
extern unsigned short RxEthnetFrameBuffer[100];
extern unsigned short RemoteMAC[3];                             // 48 bit MAC
extern unsigned short RecdFrameMAC[3];
extern unsigned short RecdFrameIP[2];
extern unsigned short RecdIpFrameLenth;
extern unsigned short TxFrameSize;
extern unsigned short MyIP[2];
extern unsigned short RemoteIP[2];
extern unsigned short SubnetMask[2];
extern unsigned short GatewayIP[2];

extern unsigned	char	gl_cFileReceive;
extern unsigned	char	gl_cLineWidth;
extern unsigned	char	gl_cFilePrintStatus;
extern unsigned	char	gl_cCountFlag;

extern unsigned char	gl_cSprinklerCount;
extern unsigned	char	gl_cSprinklerNo;
extern unsigned	char	gl_cBidirectional;
extern unsigned	char	gl_cMultiDotLine12;
extern unsigned	char	gl_cMultiDotLine23;
extern unsigned	char	gl_cMultiDotLine34;
extern unsigned	int		gl_iYDotSpace12;
extern unsigned	int		gl_iYDotSpace23;
extern unsigned	int		gl_iYDotSpace34;
extern unsigned	int		gl_iGraphWidth;
extern unsigned	int		gl_iMaxPictureWidth;
extern unsigned int 	gl_iMaxPictureLength;
extern unsigned	int		gl_iLineDataNum;
extern unsigned	int		gl_iReadDataAddr;
extern unsigned	int		gl_iDataEndAddr;
extern unsigned	int		gl_iWriteFIFOAddr;
extern unsigned	int		gl_iDataEndAddr0;
extern unsigned	int		gl_iDataEndAddr1;
extern unsigned	int		gl_iRollPaper;
extern unsigned int		gl_iSendPaper;

extern unsigned	char	gl_cReacterStatus;
extern unsigned	char	gl_cInvertBufStatus;
extern unsigned	char	gl_cCurPrintBuf;
extern unsigned	char	gl_cPrintStatus;
extern unsigned	char	gl_cSelfCheckFlag;
extern unsigned	char	gl_cRemainderBit;

extern unsigned	short	gl_sPlotType;

extern double gl_dPictureErrorX;
extern double gl_dPictureErrorY;
extern double gl_dErrorX;
extern double gl_dErrorY;
extern int		gl_iStaggerDot;
extern int		gl_iTHPulseNumX;
extern int		gl_iPulseNumX;
extern int		gl_iMovePaperLength;
extern int		gl_iPulseNumY;
extern int		gl_iOriginalPos;
extern int		gl_iAxisYPos;
extern long 		gl_lAxisXPos;												//X轴坐标
extern unsigned	int		gl_iMaxYMM;									//Y轴长度（单位：MM）


extern unsigned	int		gl_iDelayCount;
extern unsigned	int		gl_iWaitCount;
extern unsigned	int		gl_iWaitCount1;
extern unsigned	char	gl_cDelayFlag;

extern unsigned	int		gl_iRecDataLen;
extern unsigned	char	gl_cUsbSendBuf[100];
extern unsigned	char	gl_cUsbSendBufPH;
extern unsigned	char	gl_cUsbSendBufPL;
extern unsigned	char	gl_cCommunicationMode;
extern unsigned 	char  gl_scankeyflag;//0:禁止键盘扫描
extern unsigned	char	gl_cPrintWorkStatus;			// 20120711 当前工作状态 0:空闲; 1:打印 
extern unsigned	char 	gl_cCutWorkStatus;			//20120711 当前状态0:切割文件未准备好，1:切割文件准备好
extern unsigned char	gl_cMachineWorkStatus;		// 20120827 设备工作状态
extern unsigned char	gl_cKeyStart;				// 20120827 按键启动打印
//刀属性全局定义区
extern unsigned char gl_cCutPenNum;		//笔刀属性
extern unsigned char gl_cCutUpDownState;	// 刀具状态:	1,刀具落下，0,刀具抬起(原点感应器位置)
extern unsigned char gl_cMovPaper;			//自动送纸
extern unsigned char gl_cPumpSwitch;		//气泵吸附设置
extern unsigned char gl_cEndReturnRight;	//结束返回右下角
extern unsigned char gl_cOverCutOffPaper;	//打印&结束切断纸张 20120906
extern unsigned short gl_sOverCutOffset;	// 偏移多少毫米厚切断纸张 20120906
extern unsigned short gl_sMachineWidth;		//机型宽度
extern unsigned short gl_sMachineLenght;		//机型长度
extern int gl_iCutDown[10];					//刀压
extern int gl_cSetPenAttr[10];			//笔刀属性 0 pen 1 Cut 2 Cut*(转角提刀)
extern int gl_iCutSpeed;					//切割速度
//extern int gl_iPrintSpeed;					//打印速度
extern int gl_iMoveFramSpeed;				//移框速度
extern int gl_iCutOffset;					//偏距
extern int gl_iYMaxEffectivePMM;			// Y最大有效打印脉冲数 20120822
extern int gl_iXMaxEffectivePMM;			// X最大有效打印脉冲数
extern int gl_dCutExtensValue;			//提刀延伸值
extern int gl_dCutRotaExtenValue;		//转角提刀延伸值

extern int  gl_iCutDistanceY;				//刀距
extern int  gl_iCutDistanceX;			// X 向偏移

extern double gl_dXLengthCalibration;		//X长度校正
extern double gl_dYLengthCalibration;  	//Y长度校正
extern double gl_dCutPress;				//刀压补偿

extern double gl_dPosUnitX;                //X轴每绘图语言的逻辑单位(注意不是绘图仪的单位)对应的脉冲值
extern double gl_dPosUnitY;                //Y轴每绘图语言的逻辑单位(注意不是绘图仪的单位)对应的脉冲值
extern double gl_dXProp;                   //XY轴的修正比例
extern double gl_dYProp;
//新增20100623
//变量
extern char gl_MacRunState; //0:静止状态，1:运转状态
extern char language[100][30] ;//语言文字
extern int  gl_aZInductor[103];

extern unsigned int gl_iStepSpeed[150];		//每步打印速度
extern unsigned char gl_cMaxStep[61];			//直径对应的最大速度步数

extern unsigned int gl_iStepSpeedX[150];		//每步打印速度
extern unsigned char gl_cMaxStepX[61];			//直径对应的最大速度步数
extern unsigned char gl_cStepX;

extern unsigned char gl_cAccStep[251];      //连续多点打印时每条线段的起始打印速度在速度表中的位置
extern unsigned char gl_cMaxSpeed[251];     //每条线段结束时的最大速度，主要是有长轴或打印方向变更决定
extern unsigned char gl_cStep;							//最大加速步数
extern unsigned char gl_cManualPumpSwitch;		// 手动气泵吸附设置20130104
extern char gl_cMenu[10][17];
extern char gl_cPopMenu[10][17];
extern char gl_cViewErrMenu[18][17];
extern int gl_iATime;                      //加速时间,单位:ms
extern int gl_iInitV0;	
extern unsigned char gl_cCutFileSendFlag;   //切割数据文件传输标志1:传输切割数据文件
extern long gl_lCurrCutFileLength;	//当前切割文件长度
extern int gl_iCutUpDelay;                 //抬笔延迟,单位:ms
extern int gl_iCutDownDelay;               //落笔延迟,单位:ms

extern long gl_lMaxX;                      //切割的XY最大值
extern long gl_lMaxY;
extern double gl_dMovePaperError_X, gl_dMovePaperError_Y;  //走纸误差校正
extern double gl_dPaperAdjust;				//页间隙

extern unsigned char gl_cINLineNum;        //IN后开始后X轴两条直线速度降低，以免画两边直线时卷纸不齐
extern char gl_DeleteFileState;  // 0:文件没有被删除
extern unsigned	int		gl_iCurPictureWidth;				//当前图形的最大图形宽度
extern unsigned	int		gl_iCurPictureLength;				//当前图形的最大图形长度
extern unsigned char  gl_cPenNo;  // 20101206 笔号刀号
extern unsigned char  gl_cCutNo;
extern unsigned char  gl_cPrintCutSelect; // 20101206 打印切割选择
extern int gl_iGapPixel; // 点行数
extern char gl_cKeyDirectPost;		//按键回目标地址
extern int gl_iTempATime;		//TEST
extern char cReviceFlag;	//数据可以接收工作标志
extern long lXlimitNum;	// X轴极限
extern char gl_cSendPaperStar;		// 2014141024 自动送纸启停

extern int gl_iSVSpeed, gl_iVSpeed, gl_iTimeAdd;		//速度Y
extern int gl_iSVSpeedX, gl_iVSpeedX, gl_iTimeAddX;    //X
extern void InitSpeedCurve(void);
extern int UpdataAccTable(int iInitV, int iPenSpeed, int iAccTime, unsigned int *iStepSpeed, unsigned char *cMaxStep);
//extern int NewUpdataAccTable(int iInitV, int iPenSpeed, int iAccTime, unsigned int *iStepSpeed, unsigned char *cMaxStep);
//函数
extern 	Putch;
extern 	Putchar;
extern	Puts;	
extern 	void Printf(char *f, ...);						//串口终端输出
extern 	void  Scanf(char *f, void  *argP);
extern 	void  FloatPrecision(char *buf, int decimalLen);
extern 	void SetLDPenSpeed(void);
extern 	int GetPenSpeed(void);
extern 	void SetSDPenSpeed(void);
extern 	void WaitPrintEnd(void);
extern 	void set_startv(int axis,int wdata);
extern 	void set_speed(int axis,int wdata);
extern 	void pmove(int axis,long pulse);
extern 	void	get_status(int axis,int *value);
extern 	void inp_move2(long pulse1,long pulse2);
extern  void SelecAxis(int axis);
extern 	int ReadAxisStatus(int axis);
extern  int ReadPrintEnd(void);
extern 	unsigned char GetIOBit(unsigned char cBit);
extern 	long IntRound(double x);
extern 	void WriteCommand(int axis, char cmd);
extern 	int ReadData(void);
extern 	int ReadRegister(int  cReg);
extern  void SuddenStop(int axis);		//   312急停
extern  void PrintReturnZero(void);
//extern  int ReadPrintEnd(int axis);
extern  void WaitDriveEnd(int axis);
extern  void SetMovFrameSpeed(int iSpeed);  // 20120906 设置移框速度
extern void SetAxisPrintSpeed(int iAxis, int iSpeed); // 20130317
extern void SetLp(int axis, long wdata);
extern long ReadLp(int axis);
extern void ViewError(int iErrorCode);

extern  void SHOW_data(int type ) ;//CutDataAddr
//***********

//********20130807 New define
extern char gl_cStartWorkFlag;		// 2014141021 文件开始工作标志
extern long int gl_lSWFCount;		// 2014141021 文件工作结束计时器
extern char gl_cSWFStartClearFlag;	// 2014141021 下次打印文件时启动打印头清洗
extern char gl_cPHCDTime;			// 2014141021 清洗延时时间

extern int  gl_iZInitSpeed;				//Z轴初始速度
extern long gl_lZDriveSpeed;				//Z轴驱动速度  
extern long gl_lZAddSpeed;					//Z轴加速度

extern int  gl_iUInitSpeed;				//U轴初始速度
extern long gl_lUDriveSpeed;				//U轴驱动速度
extern long gl_lUAddSpeed;					//U轴加速度
//*********定原点设置************//
extern long gl_lCurPost_XPluse,	gl_lCurPost_YPluse;			//当前脉冲坐标
extern long	gl_lWorkOrig_XXPluse, gl_lWorkOrig_YYPluse ;	//工作原点脉冲坐标
extern long	gl_lPostMax_XPluse, gl_lPostMax_YPluse;			//脉冲坐标最大值
//*******设备最大脉冲数
extern long	gl_lMachineWidthPluse, gl_lMachineLenghtPluse;		//能接受的最大图形长度 脉冲
//*******当前文件长宽脉冲数
extern long	gl_lCurrFileLenghtPluse, gl_lCurrFileWidthPluse;			//当前文件的长和宽 脉冲
//******机器型号
extern int gl_iCurrMacType;			//当前机器型号
extern int	gl_iMacWide_MM;				//机器型号 宽度 毫米
extern int	gl_iMacLength_MM;
//******移框速度
//*********自动送纸参数*********//
extern long	RPGL_lPaperOffsetValue;	// 纸偏移值
extern long	RPGL_iPaperSpeedValue; 	// 1~3,设置送纸速度的时候*100或者1000,显示取0~2
extern long	RPGL_iPauseTimeValue;	// 暂停时间，打印结束暂停一段时间后送纸 ?????????
extern long	RPGL_iAutoCutOff;		// 自动切割 ?????? ON 割断纸张 OFF2 不割断，仅作标记

//******结束返回右下角
extern char gl_cOverReturnState;	//割完返回右下角  1:ON 返回 / 0:OFF 不返回
extern char gl_cGoOOPointState;	// 1: 读到PU0,0时允许移动到左下角； 0:不允许
extern char gl_cStopWorkState;		// 1停止工作
extern char gl_cLanguageSelect;
extern char gl_cAutoSendPaperSet;	//自动送纸

extern int gl_iCutSectionLen;		//切割分段长度
extern int gl_iCutSectionGap;		//切割分段间隙

//
extern void SetMoveFrameSpeed(int iAxis);			//设置移框速度
extern void SetPrintSpeed(int iAxis);				//设置打印速度
extern void CornerGriffe(long iFx, long iFy, long iBx, long iBy);
extern void SetReturnOrgSpeed(void);
extern void SetContinInpCutSpeed(int i);//int iAXIS
//extern void SetSortDistInpCutSpeed(void);
extern void NetSetManage(void);

extern void SingleAxisInpDriveSpeed(int iAxis, int iSV, int iV, int iAddTime);
//extern void TwoAxleInpDriveSpeed(int iSV, int iV, int iAddTime);
extern void StraightLineADSpeed(int iAxis, int iSV, int iV, int iAddTime);
extern void FixedSpeeDrive(int iAxis, int iSpeed);
extern void ArrCpy(int Dest[], int Sour[], int len);

//******************************//
extern void RW_MoveStartPrintPost(void);
extern void SetAxisSpeed(int iSpeed);
extern void WaitInpEnd(void);
extern int IsKeyBound(unsigned char cKeyValue);
extern void SetUAxisInit(int Ispeed, long Dspeed, long Aspeed);
extern void lcd_GoLineRowDisp(char cRow, char cLine, char cDispbuf[]);
extern void ManualSwtichPump(void);
extern void OpenPump(void);
extern void ClosePump(void);							
extern void SetInitAxisSpeed(char cAix, int iSpeed);
extern void RP_SetDoubleAxisADSpeed(int iSpeed);
extern void RP_SetSingleAxisADSpeed(char cAix, int iSpeed);
extern void RestMcx312(void);
extern int KeyEffectState(unsigned char cKeyValue);


//*****************************//

typedef struct
{
	//unsigned char cChinaEn;
	unsigned char cSprinklerCount;
	unsigned char cSprinklerNo;
	unsigned char cBidirectional;
	unsigned char cLineWidth;
	unsigned char cMultiDotLine12;
	//unsigned char cMultiDotLine23;
	//unsigned char cMultiDotLine34;
	unsigned char cPenNo;	// 20101206  笔号刀号
	unsigned char cCutNo;
	unsigned char cPrintCutSelect;// 20101206 打印切割选择
	unsigned short MyIP[2];
	unsigned short SubnetMask[2];
	unsigned short GatewayIP[2];
	unsigned short sPlotType;
	unsigned short sNull1;
	//int	iOriginalPos;
	int	iYDotSpace12;
	int	iYDotSpace23;
	int	iYDotSpace34;
	int iSNUll;
	double dErrorX;
	double dErrorY;
	double dPictureErrorX;
	double dPictureErrorY;
}PrintSetPara;

//20100622
typedef struct
{ 
	
	//unsigned char cCutPenNum;		//笔刀属性
	unsigned char cMovPaper;			//自动送纸
	unsigned char cPumpSwitch;		//气泵吸附设置
	unsigned char cEndReturnRight;	//结束返回右下角
	unsigned char cOverCutOffPaper; //打印&结束切断纸张 20120906
	unsigned char cPHCDTime;		// 20141021 
	//**********20130809
	char cLanguageSelect;		// 语言选择
		//割完返回右下角  1:ON 返回 / 0:OFF 不返回
	char cOverReturnState;
	// 1: 读到PU0,0时允许移动到左下角； 0:不允许
	char cGoOOPointState;
	// 自动送纸
	char cAutoSendPaperSet;
	//打印结束是否自动切断
	int iAutoCutOff;
	//U轴速度等级
	int iUAxisSpeedInit;
	// 工作结束暂停时间
	int iPauseTimeValue;

	int	iOriginalPos;
	//进纸偏移
	long lPaperOffsetValue;
	//***********
	unsigned short sOverCutOffset;  // 偏移毫米切断纸张 20120906
	unsigned short sMachineWidth;		//机型宽度
	unsigned short sMachineLenght;		//机型长度
	int iCutSpeed;					//切割速度
	//int iPrintSpeed;					//打印速度
	int iMoveFramSpeed;				//移框速度
	int iCutOffset;					//偏距
	int iCutDown[10];					//刀压	
	int cSetPenAttr[10];
	//int iOriginalPos_X;			//原点坐标X,单位mm
	//int iOriginalPos_Y;			//原点坐标Y,单位mm
	int iCutUpDelay;                 //抬笔延迟,单位:ms
	int iCutDownDelay;               //落笔延迟,单位:ms

	int iCutSectionLen;				//切割分段长度
	int iCutSectionGap;				//切割分段间隙
	
	int  iZInitSpeed;
	long lZDriveSpeed;			// 20120808
	long lZAddSpeed;

	int  iUInitSpeed;
	long lUDriveSpeed;
	long lUAddSpeed;
	
	//long lWorkOrig_YYPluse;			//原点脉冲坐标
	//long lWorkOrig_XXPluse;
	
	int dCutExtensValue;			//提刀延伸值
	int dCutRotaExtenValue;		//转角提刀延伸值
	
	int iCutDistanceY;				//刀距
	int iCutDistanceX;				// X 刀具精度调整
	
	double dXLengthCalibration;		//X长度校正
	double dYLengthCalibration;  	//Y长度校正
	double dCutPress;				//刀压补偿
	double dXProp;				////XY轴的修正比例
	double dYProp;
	double dPosUnitX;				//XY轴每绘图语言的逻辑单位(注意不是绘图仪的单位)对应的脉冲值
	double dPosUnitY;
	double dMovePaperError_X;
	double dMovePaperError_Y;
	double dPaperAdjust;		//页间隙

	int cTime	; //机头喷墨时间
}CutSetPara;

#endif
