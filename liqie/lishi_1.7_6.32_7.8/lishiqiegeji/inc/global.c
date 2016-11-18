#include "..\src\defconst.h"

//unsigned	char	Daochushi; 

unsigned	char	gl_cCanRevChar;       		//=1 可以接收字符
unsigned	char	gl_cLCDStatus;						//0位为'1'表示LCD第0行反白，1位为'1'表示LCD第1行反白

unsigned	char	gl_cIOReadValue;					//读IO口值
unsigned	char	gl_cIOWriteValue;					//写IO口值
unsigned	char	gl_cKeyBuf[KeyBufLength];	        //按键缓冲区
unsigned	char	gl_cKeyPH;								//按键缓冲区头指针
unsigned	char	gl_cKeyPE;								//按键缓冲区尾指针
char	gl_cLcdDispBuf[2][16];		//显示内容备份

unsigned	char	gl_cUartRecBuf[UartRecBufLength];		//串口接收缓冲区
unsigned	char	gl_cUartSendBuf[UartSendBufLength];	//串口发送缓冲区
unsigned	char	gl_cUartRecBufPH;					//串口接收缓冲区头指针
unsigned	char	gl_cUartSendBufPH;				//串口发送缓冲区头指针
unsigned	char	gl_cUartSendBufPE;				//串口发送缓冲区尾指针
unsigned	char	gl_cRecFrameOver;					//串口接收标志
        	
unsigned	int		gl_iEthnetRecBufPH;			//以太网接收缓冲区头指针
unsigned	int		gl_iEthnetRecBufPE;			//以太网接收缓冲区尾指针
unsigned	short	TxEthnetFrameBuffer[50];	//网络数据发送缓冲区
unsigned	short	RxEthnetFrameBuffer[100];	//网络数据接收缓冲区
unsigned	short	RemoteMAC[3];             // 48 bit MAC
unsigned	short	RecdFrameMAC[3];
unsigned	short	RecdFrameIP[2];
unsigned	short	RecdIpFrameLenth;
unsigned	short	TxFrameSize;
unsigned	short	MyIP[2];
unsigned	short	RemoteIP[2];
unsigned	short	SubnetMask[2];
unsigned	short	GatewayIP[2];

int		gl_time = 4;							//每次清洗喷头的时间长度
unsigned	char	gl_cFileReceive;			//bit0:文件开始传送标志,为0没有文件开始传送,为1有文件在传送
												//bit7:文件传送结束标志,为1传送结束
												//bit6:最后一行数据补0标志,为1表示已经补0
												//bit5:打印结束标志,为1表示喷头已经回到原点位置
												//bit4:文件删除标志,为1表示删除当前文件
												//bit3:清洗喷头标志,为1表示清洗喷头
unsigned	char	gl_cLineWidth;						//线宽,'S'细(默认值),'M'中,'L'粗
unsigned	char	gl_cFilePrintStatus;						//文件打印状态,为0未开始打印,为1正在打印
unsigned	char	gl_cCountFlag;							//=1表示接收到字节数;=0表示没有剩余数据

unsigned 	char	gl_cSprinklerCount;						//喷头数
unsigned	char	gl_cSprinklerNo;						//有效喷头起始号（0～3）
unsigned	char	gl_cBidirectional;						//双向打印标志,为1双向打印
unsigned	char	gl_cMultiDotLine12;					//1、2喷头之间的重叠点行数
unsigned	char	gl_cMultiDotLine23;					//2、3喷头之间的重叠点行数
unsigned	char	gl_cMultiDotLine34;					//3、4喷头之间的重叠点行数
unsigned	int		gl_iYDotSpace12;					//1、2喷头之间的Y方向错开的点数
unsigned	int		gl_iYDotSpace23;					//2、3喷头之间的Y方向错开的点数
unsigned	int		gl_iYDotSpace34;					//3、4喷头之间的Y方向错开的点数
unsigned	int		gl_iGraphWidth;					//一个点行的字节数(图宽)
unsigned	int		gl_iMaxPictureWidth;				//能接收的最大图形宽度
unsigned	int		gl_iMaxPictureLength;				//能接收的最大图形长度
unsigned	int		gl_iLineDataNum;					//打印一行所需要接收的字节数
unsigned	int		gl_iReadDataAddr;					//写FIFO数据时当前SDRAM地址
unsigned	int		gl_iDataEndAddr;					//写FIFO数据时SDRAM结束地址
unsigned	int		gl_iWriteFIFOAddr;					//向喷头FIFO写数据起始地址
unsigned	int		gl_iDataEndAddr0;					//数据块0结束地址
unsigned	int		gl_iDataEndAddr1;					//数据块1结束地址
unsigned	int		gl_iRollPaper;						//收纸时间计数器
unsigned	int		gl_iSendPaper;					//送纸时间计数器

unsigned	char	gl_cReacterStatus;					//收送纸状态.bit0为1:已启动送纸,bit1为1已启动收纸
unsigned	char	gl_cInvertBufStatus;				//数据转换缓冲区状态.bit0为1:正在打印或已经打印的是缓冲区1中的数据
																						//bit1为1表示当前应准备的是缓冲区1中的数据
																						//bit2为1表示缓冲区0数据已准备好
																						//bit3为1表示缓冲区1数据已准备好
unsigned	char	gl_cCurPrintBuf;					//为1:当前准备打印或正在打印的是缓冲区1中的数据
unsigned	char	gl_cPrintStatus;					//为0表示没有打印,为1表示Y轴正在打印,为2表示Y轴打印结束 3 时Y回原点
																						//为3表示X轴正在打印,为4表示X轴打印结束
unsigned	char	gl_cSelfCheckFlag;					//感应器自检标志,为0不自检,为1自检
unsigned	char	gl_cRemainderBit;					//数据转换时剩余的位数

unsigned	short	gl_sPlotType;					//机型

double gl_dPictureErrorX;						//图形误差,X轴理论上1000mm对应的实际绘出长度
double gl_dPictureErrorY;						//图形误差,Y轴理论上1000mm对应的实际绘出长度
double gl_dErrorX;								//X轴走纸误差(单位:MM)
double gl_dErrorY;								//相邻两次扫描之间错开的MM数
int		gl_iStaggerDot;						//相邻两次扫描之间错开的点数
int		gl_iTHPulseNumX;						//X轴理论上因走的脉冲数（没有误差的情况下）
int		gl_iPulseNumX;						//X轴驱动脉冲数
int		gl_iMovePaperLength;					//已走纸长度(单位:脉冲数)
int		gl_iPulseNumY;						//Y轴驱动脉冲数
int		gl_iOriginalPos;						//原点位置(单位:Pulse)
int		gl_iAxisYPos;							//Y轴坐标
long 	gl_lAxisXPos;							//X轴坐标
unsigned	int		gl_iMaxYMM;					//Y轴长度（单位：MM）

unsigned	int		gl_iDelayCount;				//延时循环次数
unsigned	int		gl_iWaitCount;					//等待打印循环次数
unsigned	int		gl_iWaitCount1;
unsigned	char	gl_cDelayFlag;						//是否需要打印延时标志

unsigned	int		gl_iRecDataLen;				//USB等待接收数据长度
unsigned	char	gl_cUsbSendBuf[120];				//USB发送缓冲区
unsigned	char	gl_cUsbSendBufPH;					//USB发送数据头指针
unsigned	char	gl_cUsbSendBufPL;					//USB发送数据尾指针
unsigned	char	gl_cCommunicationMode;			//通讯方式选择0:网络接收数据 1:USB接收数据 2:网络和USB都可接收数据
unsigned char  gl_scankeyflag;				//0:禁止键盘扫描20100223
unsigned	char	gl_cPrintWorkStatus;			// 20120711 当前工作状态 0:空闲; 1:打印 
unsigned	char 	gl_cCutWorkStatus;			//20120711 当前状态0:切割文件未准备好，1:切割文件准备好
unsigned char	gl_cMachineWorkStatus;		// 20120827 设备工作状态 0:空闲   1:工作
unsigned char	gl_cKeyStart;				// 20120827 按键启动打印
//刀属性全局定义区
unsigned char gl_cCutPenNum;		//笔刀属性
unsigned char gl_cCutUpDownState;	// 刀具状态:	1,刀具落下，0,刀具抬起(原点感应器位置)
unsigned char gl_cMovPaper;			//自动送纸
unsigned char gl_cPumpSwitch;		//气泵吸附设置
unsigned char gl_cManualPumpSwitch;		// 手动气泵吸附设置20130104
unsigned char gl_cEndReturnRight;	//结束返回右下角
unsigned char gl_cOverCutOffPaper;	//打印&结束切断纸张 20120906
unsigned short gl_sOverCutOffset;	// 偏移多少毫米厚切断纸张 20120906
unsigned short gl_sMachineWidth;		//机型宽度
unsigned short gl_sMachineLenght;		//机型长度

unsigned	int		gl_iCurPictureWidth;				//当前图形的最大图形宽度
unsigned	int		gl_iCurPictureLength;				//当前图形的最大图形长度

int gl_iCutDown[10];					//刀压
int gl_cSetPenAttr[10];			//笔刀属性 0 pen 1 Cut 2 Cut*(转角提刀)
int gl_iCutSpeed;					//切割速度
//int gl_iPrintSpeed;					//打印速度
int gl_iMoveFramSpeed;				//移框速度
int gl_iYMaxEffectivePMM;			// Y最大有效打印脉冲数 20120822
int gl_iXMaxEffectivePMM;			// X最大有效打印脉冲数
int gl_iCutOffset;					//偏距
int gl_dCutExtensValue;			//提刀延伸值
int gl_dCutRotaExtenValue;		//转角提刀延伸值
int gl_iCutDistanceY;				//刀距:刀与1号喷头之间的距离 单位像素点
int gl_iCutDistanceX;			// X 向偏移

double gl_dXLengthCalibration;		//X长度校正
double gl_dYLengthCalibration;  	//Y长度校正
double gl_dCutPress;				//刀压补偿

double gl_dPosUnitX;                //X轴每绘图语言的逻辑单位(注意不是绘图仪的单位)对应的脉冲值
double gl_dPosUnitY;                //Y轴每绘图语言的逻辑单位(注意不是绘图仪的单位)对应的脉冲值
double gl_dXProp;                   //XY轴的修正比例
double gl_dYProp;
double gl_dPaperAdjust;				//页间隙

int gl_iCutUpDelay;                 //抬笔延迟,单位:ms
int gl_iCutDownDelay;               //落笔延迟,单位:ms

long gl_lMaxX;                      //切割的XY最大值 20130807 单位脉冲
long gl_lMaxY;

unsigned char gl_cINLineNum;        //IN后开始后X轴两条直线速度降低，以免画两边直线时卷纸不齐
//新增
char gl_MacRunState; //0:静止状态，1:运转状态
char gl_DeleteFileState;  // 0:文件没有被删除
// Z轴脉冲
//int  gl_aZInductor[103] = {0,95,193,293,396,500,607,715,825,937,1051,1166,1283,1401,1520,1641,1762,1885,2008,2132,2257,2382,2507,2633,2758,2884,3009,3135,3260,3384,3507,3630,3752,3873,3993,4111,4228,4343,4457,4569,4679,4787,4892,4996,5097,5195,5290,5383,5473,5560,5644,5725,5802,5876,5947,6014,6077,6136,6192,6244,6292,6335,6375,6411,6442,6470,6493,6511,6526,6536,6542,6543,6540,6533,6522,6506,6485,6461,6432,6399,6362,6320,6275,6225,6172,6114,6053,5988,5919,5847,5771,5691,5609,5523,5434,5341,5246,5148,5048,4945,4839,4731,4621};

unsigned int gl_iStepSpeed[150];		//每步打印速度
unsigned char gl_cMaxStep[61];			//直径对应的最大速度步数

unsigned int gl_iStepSpeedX[150];		//每步打印速度
unsigned char gl_cMaxStepX[61];			//直径对应的最大速度步数
unsigned char gl_cStepX;

unsigned char gl_cAccStep[251];      //连续多点打印时每条线段的起始打印速度在速度表中的位置
unsigned char gl_cMaxSpeed[251];     //每条线段结束时的最大速度，主要是有长轴或打印方向变更决定
unsigned char gl_cStep;							//最大加速步数

int gl_iATime;                      //加速时间,单位:ms
int gl_iInitV0;											//初速度，单位:mm/s

unsigned char gl_cCutFileSendFlag;   //0x0:无切割数据文件；0x01有切割文件，等待操作; 
								//0x02: 切割文件等待操作;0x03执行切割操作
long gl_lCurrCutFileLength;	//当前切割文件数据长度

unsigned char  gl_cPenNo;  // 20101206 笔号刀号
unsigned char  gl_cCutNo;
unsigned char  gl_cPrintCutSelect; // 20101206 打印切割选择
int gl_iGapPixel; // 点行数

unsigned int gl_intoruncutmod = 0;     //进入函数runcutmod的标志位

//********20130807 New define
int  gl_iZInitSpeed;				//Z轴初始速度
long gl_lZDriveSpeed;				//Z轴驱动速度  
long gl_lZAddSpeed;					//Z轴加速度

int  gl_iUInitSpeed;				//U轴初始速度
long gl_lUDriveSpeed;				//U轴驱动速度
long gl_lUAddSpeed;					//U轴加速度
//*********定原点设置************//
long 	gl_lCurPost_XPluse,	gl_lCurPost_YPluse;			//当前脉冲坐标
long	gl_lWorkOrig_XXPluse, gl_lWorkOrig_YYPluse ;	//工作原点脉冲坐标
long	gl_lPostMax_XPluse, gl_lPostMax_YPluse;			//脉冲坐标最大值
//*******设备最大脉冲数
long	gl_lMachineWidthPluse, gl_lMachineLenghtPluse;		//能接受的最大图形长度 脉冲
//*******当前文件长宽脉冲数
long	gl_lCurrFileLenghtPluse, gl_lCurrFileWidthPluse;			//当前文件的长和宽 脉冲
//******机器型号
int gl_iCurrMacType;			//当前机器型号
int	gl_iMacWide_MM;				//机器型号 宽度 毫米
int	gl_iMacLength_MM;

//********I0状态
unsigned char gl_cSetIOStates;		// IO 状态 Read:Bit0：X轴原点|Bit1：Y轴原点|Bit2：Z轴原点；其余：保留。值为1：无检测；0：检测有效			
								    //         Write: Bit0：画笔，Bit1：风机， Bit2:LED 绿灯 Bit3:LED 红灯, 其余:保留
//*********自动送纸参数*********//
long	RPGL_lPaperOffsetValue;	// 纸偏移值
long	RPGL_iPaperSpeedValue; 	// 1~3,设置送纸速度的时候*100或者1000,显示取0~2
long	RPGL_iPauseTimeValue;	// 暂停时间，打印结束暂停一段时间后送纸 ?????????
long	RPGL_iAutoCutOff;		// 自动切割 ?????? ON 割断纸张 OFF2 不割断，仅作标记
//******************************//
//******结束返回右下角
char gl_cOverReturnState;	//割完返回右下角  1:ON 返回 / 0:OFF 不返回
char gl_cGoOOPointState;	// 1: 读到PU0,0时允许移动到左下角； 0:不允许
char gl_cStopWorkState;		// 1停止工作
//*****************************//
char gl_cKeyDirectPost;		//按键回目标地址

long gl_rate =1.0; //调节xy速度比率//还没使用
int gl_iSVSpeed, gl_iVSpeed, gl_iTimeAdd;		//速度Y
int gl_iSVSpeedX, gl_iVSpeedX, gl_iTimeAddX;    //X
//*********
int gl_iTempATime;		//TEST
char cReviceFlag;	//数据可以接收工作标志
long lXlimitNum;	// X轴极限
char gl_cLanguageSelect; //语言选择 0 中文  1英文
char gl_cMenu[12][17];
char gl_cPopMenu[11][17];
char gl_cViewErrMenu[18][17];		//20141202 rwt
int gl_iCutSectionLen;		//切割分段长度
int gl_iCutSectionGap;		//切割分段间隙
double gl_dMovePaperError_X, gl_dMovePaperError_Y;  //走纸误差校正
char gl_cAutoSendPaperSet;	//自动送纸
// 原来每次文件开始打印前需要清洗，现在修改为空闲多长时间后在打印文件就清洗
char gl_cStartWorkFlag;		// 2014141021 文件开始工作标志
long int gl_lSWFCount;		// 2014141021 文件工作结束计时器
char gl_cSWFStartClearFlag;	// 2014141021 下次打印文件时启动打印头清洗
char gl_cPHCDTime;			// 2014141021 清洗延时时间
char gl_cSendPaperStar;		// 2014141024 自动送纸启停

