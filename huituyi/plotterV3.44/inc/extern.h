#ifndef __EXTERN_H__
#define __EXTERN_H__

#include "..\inc\defconst.h"

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
extern unsigned	int		gl_iMaxYMM;
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

extern unsigned char  gl_WatchDogState;		// 1:启动看门狗 20101118
extern unsigned long  gl_WatchDogCount;		// 看门狗计数器
extern unsigned char  SprinklerNum;   // 20101214   喷头数可选
extern unsigned short gl_sSendPaperLength;// 20110221 设定送纸长度，单位mm
extern unsigned short gl_sSendPaperDelay; // 20110221 设定送纸延迟1~4
extern unsigned short gl_sPrintHeadCleanCount;// 20110221 设置打印头清洗次数
extern unsigned char gl_DeviceType;	// 20111101 电机设备类型 有刷:0/无刷 :1
extern unsigned char gl_FlashChipSelect; // 20111101 Flash 芯片选择 0= 3201B

//20100623 
extern void LcdDsip(char *s1, char *s2);//LCD显示
extern void Printf(char *f, ...);//串口打印输出
//*************

typedef struct
{
	unsigned char cChinaEn;
	unsigned char cSprinklerCount;
	unsigned char cSprinklerNo;
	unsigned char cBidirectional;
	unsigned char cLineWidth;
	unsigned char cMultiDotLine12;
	unsigned char cMultiDotLine23;
	unsigned char cMultiDotLine34;
	unsigned short MyIP[2];
	unsigned short SubnetMask[2];
	unsigned short GatewayIP[2];
	unsigned short sPlotType;
	unsigned short sNull1;
	int iOriginalPos;
	int	iYDotSpace12;
	int	iYDotSpace23;
	int	iYDotSpace34;
	double dErrorX;
	double dErrorY;
	double dPictureErrorX;
	double dPictureErrorY;
	unsigned char cSprnklerNum;   // 20101214   喷头数可选 只保存在flash不上传
	unsigned char cDeviceType; 		// 20111101 电机设置 1无刷电机，
	unsigned char cFlashChipSelect; // 20111101 Flash 芯片选择 0= 3201B
	unsigned short sSendPaperDelay; // 20110221 设定送纸延迟1~4
	unsigned short sSendPaperLength;// 20110221 设定送纸长度，单位mm
	unsigned short sPrintHeadCleanCount;// 20110221 设置打印头清洗次数	
}PrintSetPara;
/*
typedef struct
{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int sec;
}TermTime;

typedef struct
{
	char startTermFlag;//启动分期标志0，未启动；1：启动
	unsigned short TermTotalTimes;//分期总次数
	unsigned short TermDays;//每期天数
	unsigned short TermCurrTimes;//当前次数
	long UpdatePassword;//上期解密密码
}TermSet;
*/
#endif
