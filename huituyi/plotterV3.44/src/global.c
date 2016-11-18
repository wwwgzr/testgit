#include "..\inc\defconst.h"

unsigned	char	gl_cCanRevChar;       		//=1 可以接收字符
unsigned	char	gl_cLCDStatus;						//0位为'1'表示LCD第0行反白，1位为'1'表示LCD第1行反白

unsigned	char	gl_cIOReadValue;					//读IO口值
unsigned	char	gl_cIOWriteValue;					//写IO口值
unsigned	char	gl_cKeyBuf[KeyBufLength];	//按键缓冲区
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

unsigned	char	gl_cFileReceive;						//bit0:文件开始传送标志,为0没有文件开始传送,为1有文件在传送
																						//bit7:文件传送结束标志,为1传送结束
																						//bit6:最后一行数据补0标志,为1表示已经补0
																						//bit5:打印结束标志,为1表示喷头已经回到原点位置
																						//bit4:文件删除标志,为1表示删除当前文件
																						//bit3:清洗喷头标志,为1表示清洗喷头
unsigned	char	gl_cLineWidth;							//线宽,'S'细(默认值),'M'中,'L'粗
unsigned	char	gl_cFilePrintStatus;				//文件打印状态,为0未开始打印,为1正在打印
unsigned	char	gl_cCountFlag;							//=1表示接收到字节数;=0表示没有剩余数据

unsigned 	char	gl_cSprinklerCount;					//喷头数
unsigned	char	gl_cSprinklerNo;						//有效喷头起始号（0～3）
unsigned	char	gl_cBidirectional;					//双向打印标志,为1双向打印
unsigned	char	gl_cMultiDotLine12;					//1、2喷头之间的重叠点行数
unsigned	char	gl_cMultiDotLine23;					//2、3喷头之间的重叠点行数
unsigned	char	gl_cMultiDotLine34;					//3、4喷头之间的重叠点行数
unsigned	int		gl_iYDotSpace12;						//1、2喷头之间的Y方向错开的点数
unsigned	int		gl_iYDotSpace23;						//2、3喷头之间的Y方向错开的点数
unsigned	int		gl_iYDotSpace34;						//3、4喷头之间的Y方向错开的点数
unsigned	int		gl_iGraphWidth;							//一个点行的字节数(图宽)
unsigned	int		gl_iMaxPictureWidth;				//能接收的最大图形宽度
unsigned	int		gl_iMaxYMM;									//Y轴长度（单位：MM）
unsigned	int		gl_iLineDataNum;						//打印一行所需要接收的字节数
unsigned	int		gl_iReadDataAddr;						//写FIFO数据时当前SDRAM地址
unsigned	int		gl_iDataEndAddr;						//写FIFO数据时SDRAM结束地址
unsigned	int		gl_iWriteFIFOAddr;					//向喷头FIFO写数据起始地址
unsigned	int		gl_iDataEndAddr0;						//数据块0结束地址
unsigned	int		gl_iDataEndAddr1;						//数据块1结束地址
unsigned	int		gl_iRollPaper;							//收纸时间计数器
unsigned	int		gl_iSendPaper;							//送纸时间计数器

unsigned	char	gl_cReacterStatus;					//收送纸状态.bit0为1:已启动送纸,bit1为1已启动收纸
unsigned	char	gl_cInvertBufStatus;				//数据转换缓冲区状态.bit0为1:正在打印或已经打印的是缓冲区1中的数据
																						//bit1为1表示当前应准备的是缓冲区1中的数据
																						//bit2为1表示缓冲区0数据已准备好
																						//bit3为1表示缓冲区0数据已准备好
unsigned	char	gl_cCurPrintBuf;						//为1:当前准备打印或正在打印的是缓冲区1中的数据
unsigned	char	gl_cPrintStatus;						//为0表示没有打印,为1表示Y轴正在打印,为2表示Y轴打印结束
																						//为3表示X轴正在打印,为4表示X轴打印结束
unsigned	char	gl_cSelfCheckFlag;					//感应器自检标志,为0不自检,为1自检
unsigned	char	gl_cRemainderBit;						//数据转换时剩余的位数

unsigned	short	gl_sPlotType;								//机型

double gl_dPictureErrorX;										//图形误差,X轴理论上1000mm对应的实际绘出长度
double gl_dPictureErrorY;										//图形误差,Y轴理论上1000mm对应的实际绘出长度
double gl_dErrorX;													//X轴走纸误差(单位:MM)
double gl_dErrorY;													//相邻两次扫描之间错开的MM数
int		gl_iStaggerDot;												//相邻两次扫描之间错开的点数
int		gl_iTHPulseNumX;											//X轴理论上因走的脉冲数（没有误差的情况下）
int		gl_iPulseNumX;												//X轴驱动脉冲数
int		gl_iMovePaperLength;									//已走纸长度(单位:脉冲数)
int		gl_iPulseNumY;												//Y轴驱动脉冲数
int		gl_iOriginalPos;											//原点位置(单位:Pulse)
int		gl_iAxisYPos;													//Y轴坐标

unsigned	int		gl_iDelayCount;							//延时循环次数
unsigned	int		gl_iWaitCount;							//等待打印循环次数
unsigned	int		gl_iWaitCount1;
unsigned	char	gl_cDelayFlag;							//是否需要打印延时标志

unsigned	int		gl_iRecDataLen;							//USB等待接收数据长度
unsigned	char	gl_cUsbSendBuf[100];				//USB发送缓冲区
unsigned	char	gl_cUsbSendBufPH;						//USB发送数据头指针
unsigned	char	gl_cUsbSendBufPL;						//USB发送数据尾指针
unsigned	char	gl_cCommunicationMode;			//通讯方式选择0:网络接收数据 1:USB接收数据 2:网络和USB都可接收数据
unsigned char  gl_scankeyflag;				//0:禁止键盘扫描20100223

unsigned char  gl_WatchDogState;		// 1:启动看门狗//20101118
unsigned long  gl_WatchDogCount;		// 看门狗计数器
unsigned char  SprinklerNum;   // 20101214   喷头数可选
unsigned short gl_sSendPaperLength;// 20110221 设定送纸长度，单位mm
unsigned short gl_sSendPaperDelay; // 20110221 设定送纸延迟1~4
unsigned short gl_sPrintHeadCleanCount; // 20110221 设置打印头清洗次数
unsigned char gl_DeviceType;	// 20111101 电机设备类型 有刷:0/无刷 :1
unsigned char gl_FlashChipSelect; // 20111101 Flash 芯片选择 0= 3201B

