#include "..\inc\defconst.h"
#include "..\inc\extern.h"
#include <string.h>

void CH375_WR_CMD_PORT(unsigned char cmd);
void CH375_WR_DAT_PORT(unsigned char dat);
void InitCH375(void);
void PollRecUsbData(void);
void __irq Eint1_Int(void);
void ReceiveUsbData(void);
void UsbDataToDram(unsigned int iLength);
void Delay100ns(int icount);

unsigned char DealUsbCommand(unsigned char *length,unsigned char cOrder);

extern	void	CalVariableValue(void);
extern	void	lcd_refresh(void);
extern	void	LineReverse(unsigned char line);
extern	void	ClearPrintHead(unsigned int iFlag);
extern	int		WriteParameter(void);
extern	unsigned short DrawMalposedGraph(unsigned int iGraphWidth);
extern	unsigned short SparyError(unsigned char cPtintCount);
extern	unsigned	int		CanRevChar(void);
extern	 void WaitKeyExit(unsigned char cKeyValue);
// 20110221 上传参数
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
}SendPara;
//unsigned char  TestCheckSendState=0;


void Delay100ns(int icount)//64M, delay about 100ns
{
	int i;
	
	icount = (int)((double)icount * 3.2);
	for(i=0;i<icount;i++);
}

void CH375_WR_CMD_PORT(unsigned char cmd)
{//最低需延时1.5us，现至少延时1.8us
	Delay100ns(18);
	WriteCH375Command(cmd);
}

void CH375_WR_DAT_PORT(unsigned char dat)
{//最低需延时1.5us，现至少延时1.8us
	Delay100ns(18);
	WriteCH375Data(dat);
}

unsigned char CH375_RD_DAT_PORT(void)
{//最低需延时1us，现至少延时1.2us
	Delay100ns(8);
	return ReadCH375Data();
}

/*
unsigned char CheckUSBWorkState()
{
	unsigned short sTemp;
	for (sTemp=0; sTemp<3; sTemp++)
	{
		//Printf("Check USB link: 1\n");
		CH375_WR_CMD_PORT(CHECK_EXIST);
		Printf("Check USB link: 2\n");
		CH375_WR_DAT_PORT(0x55);
		Printf("Check USB link: 3\n");
		if (CH375_RD_DAT_PORT() == 0xaa)
			return 0;
		//Printf("Check USB link: 4\n");
		Delay100ns(100);
	}
	return 1;
}
*/
void InitCH375(void)
{
	unsigned short sTemp;
	
	pISR_EINT1=(unsigned)Eint1_Int;
	for (sTemp=0;sTemp<3;sTemp++)
	{
		CH375_WR_CMD_PORT(RESET_ALL);
		Delay100ns(1000000);//延时100ms
		CH375_WR_CMD_PORT(CHECK_EXIST);
		CH375_WR_DAT_PORT(0x55);
		if (CH375_RD_DAT_PORT() == 0xaa)
			break;
	}

	//设置厂商识别码VID和产品识别码PID
/*  // 20111108 如果设置了VID和PID 无法更新新的驱动程序
	CH375_WR_CMD_PORT(SET_USB_ID);
	CH375_WR_DAT_PORT((unsigned char)USB_VID);
	CH375_WR_DAT_PORT((unsigned char)(USB_VID>>8));
	CH375_WR_DAT_PORT((unsigned char)USB_PID);
	CH375_WR_DAT_PORT((unsigned char)(USB_PID>>8));
*/
	CH375_WR_CMD_PORT(SET_USB_MODE);
	CH375_WR_DAT_PORT(0x02);
	for (sTemp=0;sTemp<1000;sTemp++)
	{
		if (CH375_RD_DAT_PORT() == CMD_RET_SUCCESS)
			break;
	}

	CH375_WR_CMD_PORT(CHK_SUSPEND);
	CH375_WR_DAT_PORT(0x10);
	CH375_WR_DAT_PORT(0x04);// 以50ms为间隔检测USB设备挂起
	
	gl_cUsbSendBufPH = 0;
	gl_cUsbSendBufPL = 0;
}


void PollRecUsbData(void)
{
	unsigned long lTemp;
	unsigned char InterruptStatus;
	int i;

	lTemp = rINTPND;
	if ((lTemp & 0x1000000) == 0)
	{
		if (gl_WatchDogState==1 && gl_WatchDogCount > 500)
		{// 20101118 解决打印传输过程中中断传输，通讯连接失败造成的打印停止的问题
			//Printf("USB Bus NO inter signal> %d ms\n", gl_WatchDogCount*8);
			gl_WatchDogCount = 0;
			CH375_WR_CMD_PORT(UNLOCK_USB );
			if (gl_cFileReceive == 0)
				memcpy(gl_cUsbSendBuf,"Msg_End ",8);
			else
				memcpy(gl_cUsbSendBuf,"Msg_OkPG",8);
			CH375_WR_CMD_PORT(WR_USB_DATA7);
			CH375_WR_DAT_PORT(8);
			for (i=0;i<8;i++)
				CH375_WR_DAT_PORT(gl_cUsbSendBuf[i]);
		}
		
		return;
	}
	
	rI_ISPC = BIT_EINT1;
	CH375_WR_CMD_PORT(GET_STATUS);
	Delay100ns(10);//延时不够，补延时1us
	InterruptStatus = CH375_RD_DAT_PORT();
	switch (InterruptStatus)
	{
		case USB_INT_EP1_OUT:
			break;
		case USB_INT_EP1_IN:
			break;
		case USB_INT_EP2_OUT:
			ReceiveUsbData();
			gl_WatchDogCount = 0;
			break;
		case USB_INT_EP2_IN://批量端点/端点2发送完数据,IN成功
			CH375_WR_CMD_PORT(UNLOCK_USB );
			if (gl_cUsbSendBufPH > gl_cUsbSendBufPL)
			{
				CH375_WR_CMD_PORT(WR_USB_DATA7);
				CH375_WR_DAT_PORT(gl_cUsbSendBufPH - gl_cUsbSendBufPL);
				while (gl_cUsbSendBufPH > gl_cUsbSendBufPL)
				{
					CH375_WR_DAT_PORT(gl_cUsbSendBuf[gl_cUsbSendBufPL]);
					gl_cUsbSendBufPL++;
				}
				gl_cUsbSendBufPH = 0;
				gl_cUsbSendBufPL = 0;
			}
			gl_WatchDogCount = 0;
			break;
		case USB_INT_USB_SUSPEND:
			CH375_WR_CMD_PORT(UNLOCK_USB );
			gl_WatchDogCount = 0;
			break;
		default:
			CH375_WR_CMD_PORT(UNLOCK_USB);	//释放当前USB缓冲区
			break;
	}		
	
}

void __irq Eint1_Int(void)
{
	unsigned char InterruptStatus;
	//int  i;
	rI_ISPC = BIT_EINT1;
	CH375_WR_CMD_PORT(GET_STATUS);
	Delay100ns(10);//延时不够，补延时1us
	InterruptStatus = CH375_RD_DAT_PORT();
	switch (InterruptStatus)
	{
		case USB_INT_EP1_OUT:
			break;
		case USB_INT_EP1_IN:
			break;
		case USB_INT_EP2_OUT:
			ReceiveUsbData();
			gl_WatchDogCount = 0;
			break;
		case USB_INT_EP2_IN://批量端点/端点2发送完数据,IN成功
			CH375_WR_CMD_PORT(UNLOCK_USB );
			if (gl_cUsbSendBufPH > gl_cUsbSendBufPL)
			{
				CH375_WR_CMD_PORT(WR_USB_DATA7);
				CH375_WR_DAT_PORT(gl_cUsbSendBufPH - gl_cUsbSendBufPL);
				while (gl_cUsbSendBufPH > gl_cUsbSendBufPL)
				{
					CH375_WR_DAT_PORT(gl_cUsbSendBuf[gl_cUsbSendBufPL]);
					gl_cUsbSendBufPL++;
				}
				gl_cUsbSendBufPH = 0;
				gl_cUsbSendBufPL = 0;
			}
			gl_WatchDogCount = 0;
			break;
		case USB_INT_USB_SUSPEND:
			CH375_WR_CMD_PORT(UNLOCK_USB );
			gl_WatchDogCount = 0;
			break;
		default:
			CH375_WR_CMD_PORT(UNLOCK_USB);	//释放当前USB缓冲区
			break;
	}		
}

void ReceiveUsbData(void)
{
	unsigned char i,length,cOrder;
	unsigned char buffer[10];
	unsigned int iBufLen;
	static unsigned int iRecDataLen = 0;
	SendPara PrintPara;

	CH375_WR_CMD_PORT(RD_USB_DATA);
	Delay100ns(10);//延时不够，补延时1us
	length = CH375_RD_DAT_PORT( );

	if (length > 64)
		length = 64;
	if (iRecDataLen > 0)
	{
		if (iRecDataLen <= (unsigned int)length)
		{
			UsbDataToDram((unsigned char)iRecDataLen);
			i = length - (unsigned char)iRecDataLen;
			iRecDataLen = 0;
			while (i != 0)
			{
				CH375_RD_DAT_PORT( );
				i--;
			}
			if (gl_cFileReceive == 0)
				memcpy(gl_cUsbSendBuf,"Msg_End ",8);
			else
				memcpy(gl_cUsbSendBuf,"Msg_OkPG",8);
			CH375_WR_CMD_PORT(WR_USB_DATA7);
			CH375_WR_DAT_PORT(8);
			for (i=0;i<8;i++)
				CH375_WR_DAT_PORT(gl_cUsbSendBuf[i]);
			//Printf("Close watch dog! gl_WatchDogCount= %d\n", gl_WatchDogCount);
		}
		else
		{
			UsbDataToDram(length);
			iRecDataLen -= (unsigned int)length;
		}
	}
	else if (length < 8)
	{
		for (i=0;i<length;i++)
			CH375_RD_DAT_PORT();
		return;
	}
	else
	{
		for (i=0;i<8;i++)
			buffer[i] = CH375_RD_DAT_PORT();
		length -= 8;
		if (memcmp(buffer,"Buf_Long",8) == 0)	//查询缓冲区长度
		{
			if (gl_cPrintStatus == 0x20)
			{
				if (length >= 6)
				{
					for (i=0;i<6;i++)
						buffer[i] = CH375_RD_DAT_PORT();
					if (memcmp(buffer,"_Start",6) == 0)
						gl_cPrintStatus = 0;
				}
			}
			for (i=0;i<length;i++)
				CH375_RD_DAT_PORT();
			iBufLen = CanRevChar();
			
			if (gl_cFileReceive == 0)
				memcpy(gl_cUsbSendBuf,"End_",4);
			else
				memcpy(gl_cUsbSendBuf,"Msg_",4);
			gl_cUsbSendBuf[4] = (unsigned char)(iBufLen >> 24);
			gl_cUsbSendBuf[5] = (unsigned char)(iBufLen >> 16);
			gl_cUsbSendBuf[6] = (unsigned char)(iBufLen >> 8);
			gl_cUsbSendBuf[7] = (unsigned char)iBufLen;
			CH375_WR_CMD_PORT(WR_USB_DATA7);
			CH375_WR_DAT_PORT(8);
			for (i=0;i<8;i++)
				CH375_WR_DAT_PORT(gl_cUsbSendBuf[i]);
		}
		else if (memcmp(buffer,"Snd_Long",8) == 0)	//发送数据包长度
		{
			if (length < 4)
			{
				iRecDataLen = 0;
				for (i=0;i<length;i++)
					CH375_RD_DAT_PORT();
				return;
			}
			for (i=0;i<4;i++)
				iRecDataLen = (iRecDataLen << 8) + (unsigned int)CH375_RD_DAT_PORT();
			length -= 4;
			for (i=0;i<length;i++)
				CH375_RD_DAT_PORT();
			memcpy(gl_cUsbSendBuf,"Msg_OkS ",8);
			CH375_WR_CMD_PORT(WR_USB_DATA7);
			CH375_WR_DAT_PORT(8);
			for (i=0;i<8;i++)
				CH375_WR_DAT_PORT(gl_cUsbSendBuf[i]);
			//Printf("Start watch dog! iRecDataLen=%d\n", iRecDataLen);
		}
		else if (memcmp(buffer,"Msg_Read",8) == 0)//读参数
		{
			for (i=0;i<length;i++)
				CH375_RD_DAT_PORT();
			memcpy(gl_cUsbSendBuf,"Msg_ParaNew Version3.3",22);//解决兼容性问题20100413
			PrintPara.cChinaEn = 'C';
			PrintPara.cSprinklerCount = gl_cSprinklerCount;
			PrintPara.cSprinklerNo = gl_cSprinklerNo;
			PrintPara.cBidirectional = gl_cBidirectional;
			PrintPara.cLineWidth = gl_cLineWidth;
			PrintPara.cMultiDotLine12 = gl_cMultiDotLine12;
			PrintPara.cMultiDotLine23 = gl_cMultiDotLine23;
			PrintPara.cMultiDotLine34 = gl_cMultiDotLine34;
			PrintPara.sPlotType = gl_sPlotType;
			PrintPara.sNull1 = 0;
			PrintPara.iOriginalPos = gl_iOriginalPos;
			PrintPara.iYDotSpace12 = gl_iYDotSpace12 - SprinklerSpace;
			PrintPara.iYDotSpace23 = gl_iYDotSpace23 - SprinklerSpace;
			PrintPara.iYDotSpace34 = gl_iYDotSpace34 - SprinklerSpace;
			PrintPara.dErrorX = gl_dErrorX;
			PrintPara.dErrorY = gl_dErrorY;
			PrintPara.dPictureErrorX = gl_dPictureErrorX;
			PrintPara.dPictureErrorY = gl_dPictureErrorY;
			length = sizeof(PrintPara);
			memcpy(&gl_cUsbSendBuf[22],(unsigned char *)&PrintPara,length);
			length += 22;
			if (length > 63)
			{
				gl_cUsbSendBufPH = length;
				gl_cUsbSendBufPL = 63;
				length = 63;
			}
			
			CH375_WR_CMD_PORT(WR_USB_DATA7);
			CH375_WR_DAT_PORT(length+1);
			CH375_WR_DAT_PORT(gl_cUsbSendBufPH);
			for (i=0;i<length;i++)
				CH375_WR_DAT_PORT(gl_cUsbSendBuf[i]);
			
		}
		else if (memcmp(buffer,"Msg_Comm",8) == 0)//命令
		{
			if (length > 0)
			{
				CH375_RD_DAT_PORT();
				cOrder = CH375_RD_DAT_PORT();
				length -= 2;
				i = DealUsbCommand(&length,cOrder);
				memcpy(gl_cUsbSendBuf,"Msg_",4);
				gl_cUsbSendBuf[4] = 0;
				gl_cUsbSendBuf[5] = cOrder;
				gl_cUsbSendBuf[6] = 0;
				gl_cUsbSendBuf[7] = i;
				for (i=0;i<length;i++)
					CH375_RD_DAT_PORT();
				CH375_WR_CMD_PORT(WR_USB_DATA7);
				CH375_WR_DAT_PORT(8);
				for (i=0;i<8;i++)
					CH375_WR_DAT_PORT(gl_cUsbSendBuf[i]);
			}
		}
	}
}
				
				
//将数据保存在SRAM打印缓冲区
//data_length接收数据长度
void UsbDataToDram(unsigned int iLength)
{
	unsigned short iData,iData1;
	static unsigned char cCountBak = 0;
	static unsigned char cCountFlag = 0;
	unsigned int iCount1,iCount2,iCount3;
	unsigned int iCountBak,iRemainCount;
	unsigned long address;
		
	if ((gl_cFileReceive != 0x01) && (gl_cFileReceive != 0x09))		//没有接收到文件开始标志
	{
		while (iLength--)
			CH375_RD_DAT_PORT();
		return;
	}
	if (gl_cPrintStatus == 0x20)
		gl_cPrintStatus = 0;
	address = gl_iEthnetRecBufPH + BEGINADDR;
	iRemainCount = EthnetBufLength - gl_iEthnetRecBufPH;
	if (cCountFlag != 0)
	{//已接收到字节数
		iCount1 = (unsigned int)cCountBak;
		iData = (unsigned short)CH375_RD_DAT_PORT();
		iLength--;
	}
	else
		iCount1 = 0;
	while (1)
	{
		if (iCount1 != 0)
		{
			if ((address & 0x01) == 1)
			{
				address--;
				iData1 = Readsdram(address);
				iData1 |= (iData << 8);
				Writesdram(address,iData1);
				iRemainCount--;
				if (iRemainCount == 0)
				{
					iRemainCount = EthnetBufLength;
					address = BEGINADDR;
				}
				else
					address += 2;
				iCount1--;
			}
			iData1 = iData | (iData << 8);
			iCount3 = iCount1 & 0xfffffffe;
			if (iRemainCount > iCount3)
				iCountBak = iCount3;
			else
				iCountBak = iRemainCount;
			iCount2 = 0;
			while (iCount2 < iCount3)
			{
				for (;iCount2<iCountBak;iCount2+=2)
				{
					Writesdram(address,iData1);
					address += 2;
				}
				if (iCountBak == iRemainCount)
				{
					address = BEGINADDR;
					iRemainCount = EthnetBufLength;
					iCountBak = iCount3;
				}
				else
					iRemainCount -= iCountBak;
			}
			if (iCount3 != iCount1)
			{
				iData1 &= 0xff;
				Writesdram(address,iData1);
				address++;
				iRemainCount--;
			}
		}
		if (iLength > 1)
		{
			iCount1 = (unsigned int)CH375_RD_DAT_PORT();
			iData = (unsigned short)CH375_RD_DAT_PORT();
			iLength -= 2;
		}
		else if (iLength > 0)
		{
			cCountBak = (unsigned short)CH375_RD_DAT_PORT();
			cCountFlag = 1;
			break;
		}
		else
		{
			cCountFlag = 0;
			break;
		}
	}
	gl_iEthnetRecBufPH = address - BEGINADDR;
}

unsigned char DealUsbCommand(unsigned char *length,unsigned char cOrder)
{
	unsigned char ResultCode;
	short sTemp1;
	unsigned char *cTemp;
	unsigned char cTemp1,cTemp2,cTemp3;
	unsigned int iTemp1,iTemp2,iTemp3;
	int iTemp;
	double dTemp;
	SendPara PrintPara;
	
	if (cOrder >= 10)
	{
		if (gl_cFileReceive != 0)
		{//上一图形未打印完毕,不能设置此参数
			ResultCode = 3;
			return ResultCode;
		}
	}
	switch (cOrder)
	{
		case 1:	//文件开始传送命令
			if (*length < 4)
			{
				ResultCode = 1;
				break;
			}
			iTemp1 = 0;
			for (cTemp1=0;cTemp1<4;cTemp1++)
				iTemp1 = (iTemp1 << 8) + CH375_RD_DAT_PORT();
			*length = *length - 4;
			if (iTemp1 > gl_iMaxPictureWidth)
			{
				ResultCode = 3;
				break;
			}
			if (gl_cPrintStatus == 0x10)
			{
				ResultCode = 2;
				break;
			}
			if (gl_cPrintStatus == 0x20)
			{
				gl_iEthnetRecBufPH = 0;
				gl_iEthnetRecBufPE = 0;
				gl_cPrintStatus = 0;
			}
			if (((gl_cFileReceive & 0x01) == 0x01) || (ReadPrintEnd() == 0))
			{//等待上一图形打印完
				if ((gl_cFileReceive & 0x60) == 0)
				{
					if ((gl_iEthnetRecBufPE != gl_iEthnetRecBufPH) || (ReadPrintEnd() == 0))
					{
						gl_cFileReceive |= 0x80;
						ResultCode = 2;
						break;
					}
					else if (gl_iPulseNumY < 0)
					{//喷头回原点
						gl_cFileReceive = 0xe1;
						ResultCode = 2;
						break;
					}
					else
					{
						gl_cFileReceive = 0;
						gl_cCommunicationMode = 0xff;
						gl_cInvertBufStatus = 0;
						gl_cCurPrintBuf = 0;
					}
				}
				else
				{
					ResultCode = 2;		
					break;
				}
			}
			gl_iGraphWidth = iTemp1;
			gl_cInvertBufStatus = 0;
			gl_cCurPrintBuf = 0;
			gl_cCountFlag = 0;
			gl_cRemainderBit = 16;
			ResultCode = 9;
			gl_cFileReceive = 0x01;
			CalVariableValue();
			gl_cCommunicationMode = 1;
			gl_iDelayCount = 0;
			gl_iWaitCount = 0;
			gl_iWaitCount1 = 0;
			gl_cDelayFlag = 0x80;
			gl_WatchDogState = 1;//20101118
			gl_WatchDogCount = 0;
			//Printf("File Send , Start Watch Dog !\n");
			if (gl_iAxisYPos <= gl_iOriginalPos)
				ClearPrintHead(0);
			else
				gl_cFileReceive |= 0x08;
			WriteFilePrint(0xaa);
			ClrFIFO;
			break;
		case 2:	//文件传送结束命令
			if (gl_iEthnetRecBufPE == gl_iEthnetRecBufPH)
			{
				if ((gl_cFileReceive & 0x60) == 0)
				{
					gl_cFileReceive = 0;
					gl_cCommunicationMode = 0xff;
				}
			}
			else if ((gl_cFileReceive & 0x01) == 0x01)
				gl_cFileReceive |= 0x80;
			gl_WatchDogState = 0;//20101118
			gl_WatchDogCount = 0;
			//Printf("File Send end, Close Watch Dog !\n");
			ResultCode = 9;
			break;
		case 4:
			if(gl_cFileReceive != 0)
			{
				gl_cFileReceive |= 0x10;	//删除当前打印文件
				gl_WatchDogState = 0;//20101118
				gl_WatchDogCount = 0;
				//Printf("File Del, Close Watch Dog !\n");
			}
			ResultCode = 9;
			break;
		case 5:	//查询命令
			if (CanRevChar() != 0)
				ResultCode = 9;
			else
				ResultCode = 2;
			break;
		case 6:	//图形误差设置命令
			if (*length < 16)
			{
				ResultCode = 1;			//参数不正确
				break;
			}
			cTemp = (unsigned char *)&dTemp;
			for (cTemp1=0;cTemp1<8;cTemp1++)
			{
				*cTemp = CH375_RD_DAT_PORT();
				cTemp++;
			}
			*length = *length - 8;
			if ((dTemp > 1100) && (dTemp < 900))
			{
				ResultCode = 1;			//参数不正确
				break;
			}
			gl_dPictureErrorX = dTemp;
			cTemp = (unsigned char *)&dTemp;
			for (cTemp1=0;cTemp1<8;cTemp1++)
			{
				*cTemp = CH375_RD_DAT_PORT();
				cTemp++;
			}
			*length = *length - 8;
			if ((dTemp > 1100) && (dTemp < 900))
			{
				ResultCode = 1;			//参数不正确
				break;
			}
			gl_dPictureErrorY = dTemp;
			if (WriteParameter() == 1)
				ResultCode = 9;		//参数设置正确
			else
				ResultCode = 2;		//写FLASH出错
			break;
		case 7:		//暂停打印
			while ((gl_cPrintStatus != 0) && (gl_cPrintStatus != 0x10));
			if (gl_cPrintStatus == 0)
			{
				gl_cPrintStatus = 0x20;	//暂停
				ResultCode = 9;
			}
			else
				ResultCode = 1;
			break;
		case 10:	//相邻两次扫描之间X轴和Y轴的误差
			if (*length < 16)
			{
				ResultCode = 1;			//参数不正确
				break;
			}
			cTemp = (unsigned char *)&dTemp;
			for (cTemp1=0;cTemp1<8;cTemp1++)
			{
				*cTemp = CH375_RD_DAT_PORT();
				cTemp++;
			}
			*length = *length - 8;
			iTemp = (int)(dTemp * X_PPMM);
			if ((iTemp > gl_iTHPulseNumX) || (iTemp < -gl_iTHPulseNumX))
			{
				ResultCode = 1;			//参数不正确
				break;
			}
			gl_dErrorX = dTemp;
			cTemp = (unsigned char *)&dTemp;
			for (cTemp1=0;cTemp1<8;cTemp1++)
			{
				*cTemp = CH375_RD_DAT_PORT();
				cTemp++;
			}
			*length = *length - 8;
			if ((dTemp > 6.0) || (dTemp < -6.0))
			{
				ResultCode = 1;		//参数不正确
				break;
			}
			gl_dErrorY = dTemp;
			gl_iStaggerDot = (int)(gl_dErrorY * Y_PPMM / DotPulseNum);
			CalVariableValue();
			if (WriteParameter() == 1)
				ResultCode = 9;		//参数设置正确
			else
				ResultCode = 2;		//写FLASH出错
			break;
		case 14:		//打印走纸误差修正图
			if (gl_cPrintStatus == 0x10)
				break;
			if (*length < 8)
			{
				ResultCode = 1;
				break;
			}
			dTemp = gl_dErrorX;
			cTemp = (unsigned char *)&gl_dErrorX;
			for (cTemp1=0;cTemp1<8;cTemp1++)
			{
				*cTemp = CH375_RD_DAT_PORT();
				cTemp++;
			}
			*length = *length - 8;
			iTemp = (int)(gl_dErrorX * X_PPMM);
			if ((iTemp > gl_iTHPulseNumX) || (iTemp < -gl_iTHPulseNumX))
			{
				gl_dErrorX = dTemp;
				ResultCode = 1;			//参数不正确
				break;
			}
			CalVariableValue();
			if (SparyError(2) == 1)
				ResultCode = 1;
			else
				ResultCode = 9;
			lcd_refresh();
			LineReverse(0x40);
			gl_dErrorX = dTemp;
			CalVariableValue();
			break;
		case 15:		//打印喷头间Y方向误差修正图
			if (gl_cPrintStatus == 0x10)
				break;
			if (*length < 12)
			{
				ResultCode = 1;
				break;
			}
			cTemp1 = gl_cMultiDotLine12;
			cTemp2 = gl_cMultiDotLine23;
			cTemp3 = gl_cMultiDotLine34;
			CH375_RD_DAT_PORT();
			gl_cMultiDotLine12 = CH375_RD_DAT_PORT();
			CH375_RD_DAT_PORT();
			gl_cMultiDotLine23 = CH375_RD_DAT_PORT();
			CH375_RD_DAT_PORT();
			gl_cMultiDotLine34 = CH375_RD_DAT_PORT();
			*length = *length - 6;
			if ((gl_cMultiDotLine12 > SprinklerWidth) 
			 || (gl_cMultiDotLine23 > SprinklerWidth) 
			 || (gl_cMultiDotLine34 > SprinklerWidth))
			{
				gl_cMultiDotLine12 = cTemp1;
				gl_cMultiDotLine23 = cTemp2;
				gl_cMultiDotLine34 = cTemp3;
				ResultCode = 1;
				break;
			}
			iTemp1 = gl_iYDotSpace12;
			sTemp1 = (unsigned short)CH375_RD_DAT_PORT();
			sTemp1 = (sTemp1 << 8) | (unsigned short)CH375_RD_DAT_PORT();
			*length = *length - 2;
			if ((sTemp1 < -MaxMalposedDot) && (sTemp1 > MaxMalposedDot))
			{
				ResultCode = 1;
				break;
			}
			gl_iYDotSpace12 = (unsigned int)(SprinklerSpace + sTemp1);
			iTemp2 = gl_iYDotSpace23;
			sTemp1 = (unsigned short)CH375_RD_DAT_PORT();
			sTemp1 = (sTemp1 << 8) | (unsigned short)CH375_RD_DAT_PORT();
			*length = *length - 2;
			if ((sTemp1 < -MaxMalposedDot) && (sTemp1 > MaxMalposedDot))
			{
				gl_iYDotSpace12 = iTemp1;
				ResultCode = 1;
				break;
			}
			gl_iYDotSpace23 = (unsigned int)(SprinklerSpace + sTemp1);
			iTemp3 = gl_iYDotSpace34;
			sTemp1 = (unsigned short)CH375_RD_DAT_PORT();
			sTemp1 = (sTemp1 << 8) | (unsigned short)CH375_RD_DAT_PORT();
			*length = *length - 2;
			if ((sTemp1 < -MaxMalposedDot) && (sTemp1 > MaxMalposedDot))
			{
				gl_iYDotSpace12 = iTemp1;
				gl_iYDotSpace23 = iTemp2;
				ResultCode = 1;
				break;
			}
			gl_iYDotSpace34 = (unsigned int)(SprinklerSpace + sTemp1);
			CalVariableValue();
			if (SparyError(1) == 1)
				ResultCode = 1;
			else
				ResultCode = 9;
			lcd_refresh();
			LineReverse(0x40);
			gl_cMultiDotLine12 = cTemp1;	//打印完毕，参数还原
			gl_cMultiDotLine23 = cTemp2;
			gl_cMultiDotLine34 = cTemp3;
			gl_iYDotSpace12 = iTemp1;
			gl_iYDotSpace23 = iTemp2;
			gl_iYDotSpace34 = iTemp3;
			CalVariableValue();
			break;
		case 17:		//打印双向打印误差修正图
			if (gl_cPrintStatus == 0x10)
				break;
			if (*length < 16)
			{
				ResultCode = 1;
				break;
			}
			cTemp = (unsigned char *)&dTemp;
			for (cTemp1=0;cTemp1<8;cTemp1++)
			{
				*cTemp = CH375_RD_DAT_PORT();
				cTemp++;
			}
			*length = *length - 8;
			iTemp1 = (unsigned int)((dTemp * Y_PPMM / DotPulseNum + 7.0) / 8.0);
			if (iTemp1 > gl_iMaxPictureWidth)
			{
				ResultCode = 1;
				break;
			}
			dTemp = gl_dErrorY;
			cTemp = (unsigned char *)&gl_dErrorY;
			for (cTemp1=0;cTemp1<8;cTemp1++)
			{
				*cTemp = CH375_RD_DAT_PORT();
				cTemp++;
			}
			*length = *length - 8;
			if ((gl_dErrorY > 6.0) || (gl_dErrorY < -6.0))
			{
				gl_dErrorY = dTemp;
				ResultCode = 1;		//参数不正确
				break;
			}
			gl_iStaggerDot = (int)(gl_dErrorY * Y_PPMM / DotPulseNum);
			if (DrawMalposedGraph(iTemp1) == 1)
				ResultCode = 1;
			else
				ResultCode = 9;
			lcd_refresh();
			LineReverse(0x40);
			gl_dErrorY = dTemp;
			gl_iStaggerDot = (int)(gl_dErrorY * Y_PPMM / DotPulseNum);
			break;
		case 81:
			CH375_RD_DAT_PORT();
			PrintPara.cBidirectional = CH375_RD_DAT_PORT();
			CH375_RD_DAT_PORT();
			PrintPara.cSprinklerCount = CH375_RD_DAT_PORT();
			CH375_RD_DAT_PORT();
			PrintPara.cSprinklerNo = CH375_RD_DAT_PORT();
			CH375_RD_DAT_PORT();
			PrintPara.cLineWidth = CH375_RD_DAT_PORT();
			CH375_RD_DAT_PORT();
			PrintPara.cMultiDotLine12 = CH375_RD_DAT_PORT();
			CH375_RD_DAT_PORT();
			PrintPara.cMultiDotLine23 = CH375_RD_DAT_PORT();
			CH375_RD_DAT_PORT();
			PrintPara.cMultiDotLine34 = CH375_RD_DAT_PORT();
			sTemp1 = (unsigned short)CH375_RD_DAT_PORT();
			sTemp1 = (sTemp1 << 8) | (unsigned short)CH375_RD_DAT_PORT();
			PrintPara.iYDotSpace12 = (unsigned int)(SprinklerSpace + sTemp1);
			sTemp1 = (unsigned short)CH375_RD_DAT_PORT();
			sTemp1 = (sTemp1 << 8) | (unsigned short)CH375_RD_DAT_PORT();
			PrintPara.iYDotSpace23 = (unsigned int)(SprinklerSpace + sTemp1);
			sTemp1 = (unsigned short)CH375_RD_DAT_PORT();
			sTemp1 = (sTemp1 << 8) | (unsigned short)CH375_RD_DAT_PORT();
			PrintPara.iYDotSpace34 = (unsigned int)(SprinklerSpace + sTemp1);
			//走纸误差20100413
			cTemp = (unsigned char *)&PrintPara.dErrorX ;
			for (iTemp=0;iTemp<8;iTemp++)
			{
				*cTemp = CH375_RD_DAT_PORT();
				cTemp++;
			}	
			iTemp = (int)(PrintPara.dErrorX  * X_PPMM);
			if ((iTemp > gl_iTHPulseNumX) || (iTemp < -gl_iTHPulseNumX))
			{
				ResultCode = 1;			//参数不正确
				break;
			}
			//*****		
			
			cTemp = (unsigned char *)&PrintPara.dErrorY;
			for (cTemp1=0;cTemp1<8;cTemp1++)
			{
				*cTemp = CH375_RD_DAT_PORT();
				cTemp++;
			}
			//20100302
			if ((PrintPara.dErrorY > 6.0) || (PrintPara.dErrorY < -6.0))
			{
				ResultCode = 1;
				break;
			}
			//gl_dErrorX = 0;//20100413 del
			gl_dErrorY = PrintPara.dErrorY;
			gl_iStaggerDot = (int)(gl_dErrorY * Y_PPMM / DotPulseNum);
			//********
			
			cTemp = (unsigned char *)&PrintPara.dPictureErrorX;
			for (cTemp1=0;cTemp1<8;cTemp1++)
			{
				*cTemp = CH375_RD_DAT_PORT();
				cTemp++;
			}
			cTemp = (unsigned char *)&PrintPara.dPictureErrorY;
			for (cTemp1=0;cTemp1<8;cTemp1++)
			{
				*cTemp = CH375_RD_DAT_PORT();
				cTemp++;
			}
			
			if ((PrintPara.cSprinklerCount > SprinklerNum) || (PrintPara.cSprinklerCount == 0)
				|| (PrintPara.cSprinklerNo > (SprinklerNum - PrintPara.cSprinklerCount)))
			{
				ResultCode = 1;
				break;
			}
			if ((PrintPara.cBidirectional != 0) && (PrintPara.cBidirectional != 1))
			{
				ResultCode = 1;
				break;
			}
			if ((PrintPara.cLineWidth < 1) || (PrintPara.cLineWidth > 9))
			{
				ResultCode = 1;
				break;
			}
			if (PrintPara.cMultiDotLine12 >= SprinklerWidth)
			{
				ResultCode = 1;
				break;
			}
			if (PrintPara.cMultiDotLine23 >= SprinklerWidth)
			{
				ResultCode = 1;
				break;
			}
			if (PrintPara.cMultiDotLine34 >= SprinklerWidth)
			{
				ResultCode = 1;
				break;
			}
			if ((PrintPara.iYDotSpace12 > (SprinklerSpace + MaxMalposedDot)) || (PrintPara.iYDotSpace12 < (SprinklerSpace - MaxMalposedDot)))
			{
				ResultCode = 1;
				break;
			}
			if ((PrintPara.iYDotSpace23 > (SprinklerSpace + MaxMalposedDot)) || (PrintPara.iYDotSpace23 < (SprinklerSpace - MaxMalposedDot)))
			{
				ResultCode = 1;
				break;
			}
			if ((PrintPara.iYDotSpace34 > (SprinklerSpace + MaxMalposedDot)) || (PrintPara.iYDotSpace34 < (SprinklerSpace - MaxMalposedDot)))
			{
				ResultCode = 1;
				break;
			}
			//if ((PrintPara.dErrorY > 6.0) || (PrintPara.dErrorY < -6.0))
			//{
			//	ResultCode = 1;
			//	break;
			//}
			if ((PrintPara.dPictureErrorX > 1100.0) || (PrintPara.dPictureErrorX < 900.0))
			{
				ResultCode = 1;
				break;
			}
			if ((PrintPara.dPictureErrorY > 1100.0) || (PrintPara.dPictureErrorY < 900.0))
			{
				ResultCode = 1;
				break;
			}
			gl_cSprinklerCount = PrintPara.cSprinklerCount;
			gl_cSprinklerNo = PrintPara.cSprinklerNo;
			gl_cBidirectional = PrintPara.cBidirectional;				//双向打印
			gl_cLineWidth = PrintPara.cLineWidth;
			gl_cMultiDotLine12 = PrintPara.cMultiDotLine12;
			gl_cMultiDotLine23 = PrintPara.cMultiDotLine23;
			gl_cMultiDotLine34 = PrintPara.cMultiDotLine34;
			gl_iYDotSpace12 = (unsigned int)PrintPara.iYDotSpace12;
			gl_iYDotSpace23 = (unsigned int)PrintPara.iYDotSpace23;
			gl_iYDotSpace34 = (unsigned int)PrintPara.iYDotSpace34;
			gl_dErrorX = PrintPara.dErrorX ; //走纸误差20100413
			gl_dPictureErrorX = PrintPara.dPictureErrorX;
			gl_dPictureErrorY = PrintPara.dPictureErrorY;
			//20100225 保存参数
			if (WriteParameter() == 1)
			{
				ResultCode = 9;		//参数设置正确
				CalVariableValue();
			}
			else
				ResultCode = 2;		//写FLASH出错
			break;
		default:	//不支持此命令
			ResultCode = 1;
			break;
	}
	return ResultCode;
}
