#include "..\src\defconst.h"
#include "..\src\extern.h"
#include <string.h>

void CH375_WR_CMD_PORT(unsigned char cmd);
void CH375_WR_DAT_PORT(unsigned char dat);
void InitCH375(void);
void PollRecUsbData(void);
void __irq Eint1_Int(void);
void ReceiveUsbData(void);
void UsbDataToDram(unsigned int iLength);
void UsbCutDataToDram(unsigned int iLength);
void Delay100ns(int icount);

unsigned char DealUsbCommand(unsigned char *length,unsigned char cOrder);

extern	void	CalVariableValue(void);
extern	void	lcd_refresh(void);
extern	void	LineReverse(unsigned char line);
extern	void ClearPrintHead(char cOrgState);
extern	int		WriteParameter(void);
extern	unsigned short DrawMalposedGraph(unsigned int iGraphWidth);
extern	unsigned short SparyError(unsigned char cPtintCount);
extern	unsigned	int		CanRevChar(void);
extern  void Delay(int time);
extern long gl_TestSendLengPluse;

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

void InitCH375(void)
{
	unsigned short sTemp;
	
	pISR_EINT1=(unsigned)Eint1_Int;
	for (sTemp=0;sTemp<3;sTemp++)
	{
		CH375_WR_CMD_PORT(RESET_ALL);// 硬件复位
		Delay100ns(1000000);//延时100ms
		CH375_WR_CMD_PORT(CHECK_EXIST);// 检测工作状态
		CH375_WR_DAT_PORT(0x55);
		if (CH375_RD_DAT_PORT() == 0xaa)
			break;
	}
/*
	//设置厂商识别码VID和产品识别码PID
	CH375_WR_CMD_PORT(SET_USB_ID);
	CH375_WR_DAT_PORT((unsigned char)USB_VID);
	CH375_WR_DAT_PORT((unsigned char)(USB_VID>>8));
	CH375_WR_DAT_PORT((unsigned char)USB_PID);
	CH375_WR_DAT_PORT((unsigned char)(USB_PID>>8));
	*/
	// 设置USB工作模式为内置固件模式
	CH375_WR_CMD_PORT(SET_USB_MODE);
	CH375_WR_DAT_PORT(0x02);
	for (sTemp=0;sTemp<1000;sTemp++)
	{
		if (CH375_RD_DAT_PORT() == CMD_RET_SUCCESS)
			break;
	}
	
	gl_cUsbSendBufPH = 0;
	gl_cUsbSendBufPL = 0;
}

void RestartCH375(void)
{
	unsigned short sTemp;
	int iBakrINTMSK;
		
	iBakrINTMSK = rINTMSK;
	rINTMSK = BIT_GLOBAL;		//屏蔽所有中断
	rTCON = 0x0;			  //all_stop
	
	pISR_EINT1=(unsigned)Eint1_Int;
	for (sTemp=0;sTemp<3;sTemp++)
	{
		CH375_WR_CMD_PORT(RESET_ALL);// 硬件复位
		Delay100ns(1000000);//延时100ms
		CH375_WR_CMD_PORT(CHECK_EXIST);// 检测工作状态
		CH375_WR_DAT_PORT(0x55);
		if (CH375_RD_DAT_PORT() == 0xaa)
			break;
	}
	
	// 设置USB工作模式为内置固件模式
	CH375_WR_CMD_PORT(SET_USB_MODE);
	CH375_WR_DAT_PORT(0x02);
	for (sTemp=0;sTemp<1000;sTemp++)
	{
		if (CH375_RD_DAT_PORT() == CMD_RET_SUCCESS)
			break;
	}

	rTCON = 0x0a;				//update T0
	rTCON = 0x09;				//T0=one shot,all_start 
	rINTMSK = iBakrINTMSK;	//恢复中断

}

void PollRecUsbData(void)
{
	unsigned long lTemp;
	unsigned char InterruptStatus;
	//unsigned char cPh;

	// 产生USB  中断
	lTemp = rINTPND;
	if ((lTemp & 0x1000000) == 0)
	{
		
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
			break;
		default:
			CH375_WR_CMD_PORT(UNLOCK_USB);	//释放当前USB缓冲区
			
			break;
	}		
	
}


void __irq Eint1_Int(void)
{
	unsigned char InterruptStatus;

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
	//unsigned char *cTemp, cTemp1;
	static unsigned int iRecDataLen = 0;
	PrintSetPara	PrintPara;
	//char *cTemp=0;
	
	CH375_WR_CMD_PORT(RD_USB_DATA);
	Delay100ns(10);//延时不够，补延时1us
	length = CH375_RD_DAT_PORT( );
	if (length > 64)
		length = 64;
	
	if (iRecDataLen > 0)
	{
		if (iRecDataLen <= (unsigned int)length)
		{
			if (gl_cCutFileSendFlag == 0)
				UsbDataToDram((unsigned char)iRecDataLen);
			else if (gl_cCutFileSendFlag == 0x01)
				UsbCutDataToDram((unsigned char)iRecDataLen);
			i = length - (unsigned char)iRecDataLen;
			iRecDataLen = 0;
			while (i != 0)
			{
				CH375_RD_DAT_PORT( );
				i--;
			}
			if (gl_cFileReceive == 0)
			{
				memcpy(gl_cUsbSendBuf,"Msg_End ",8);
				//Printf("length: %d\n",length);
			}
			else
				memcpy(gl_cUsbSendBuf,"Msg_OkPG",8);
			CH375_WR_CMD_PORT(WR_USB_DATA7);
			CH375_WR_DAT_PORT(8);
			for (i=0;i<8;i++)
				CH375_WR_DAT_PORT(gl_cUsbSendBuf[i]);
			//Printf("Re[%x]\n",gl_cFileReceive);
			//Printf("PH: %d\n",gl_iEthnetRecBufPH);
		}
		else
		{
			if (gl_cCutFileSendFlag == 0)
				UsbDataToDram(length);
			else if (gl_cCutFileSendFlag == 0x01)
				UsbCutDataToDram(length);
			iRecDataLen -= (unsigned int)length;

			//Printf("ReceiveUsbData send\n");
		}
	}
	else if (length < 8)
	{
		for (i=0;i<length;i++)
			CH375_RD_DAT_PORT();
		//Printf("CMD len < 8Byte!\n");
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
					{
						//Printf("USB Start revice data...\n");
						gl_cPrintStatus = 0;
					}
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
			//Printf("1,");
			//Printf("USB check buf loog[%d]...\n", iBufLen);
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
			//Printf("iRecDataLen:%d\n", iRecDataLen);
		}
		else if (memcmp(buffer,"Snd_Qfed",8) == 0)	//查询发送是否结束
		{
			for (i=0;i<length;i++)
				CH375_RD_DAT_PORT();
			
			if(iRecDataLen != 0)
			{
				memcpy(gl_cUsbSendBuf,"Msg_QfNo",8);
			}
			else
			{
				if (gl_cFileReceive == 0)
					memcpy(gl_cUsbSendBuf,"Msg_End ",8);
				else
					memcpy(gl_cUsbSendBuf,"Msg_OkPG",8);
			}

			CH375_WR_CMD_PORT(WR_USB_DATA7);
			CH375_WR_DAT_PORT(8);
			for (i=0;i<8;i++)
				CH375_WR_DAT_PORT(gl_cUsbSendBuf[i]);
			
			//Printf("5X,");
		}
		else if (memcmp(buffer,"Msg_Read",8) == 0)//读参数
		{
			for (i=0;i<length;i++)
				CH375_RD_DAT_PORT();
			
			memcpy(gl_cUsbSendBuf,"New Version3.3",14);//解决兼容性问题20100413
			//memcpy(gl_cUsbSendBuf,"N3",2);
			//PrintPara.cChinaEn = 'C';
			PrintPara.cSprinklerCount = gl_cSprinklerCount;
			PrintPara.cSprinklerNo = gl_cSprinklerNo;
			PrintPara.cBidirectional = gl_cBidirectional;
			PrintPara.cLineWidth = gl_cLineWidth;
			
			//printf("gl_cLineWidth  == %d \n " ,gl_cLineWidth );
			
			PrintPara.cMultiDotLine12 = gl_cMultiDotLine12;
			//PrintPara.cMultiDotLine23 = gl_cMultiDotLine23;
			//PrintPara.cMultiDotLine34 = gl_cMultiDotLine34;
			PrintPara.cPenNo = gl_cPenNo;
			PrintPara.cCutNo = gl_cCutNo;
			PrintPara.cPrintCutSelect = gl_cPrintCutSelect;
			//PrintPara.cNull1 = 0;
//			PrintPara.MyIP[0] = MyIP[0];
//			PrintPara.MyIP[1] = MyIP[1];
//			PrintPara.SubnetMask[0] = SubnetMask[0];			
//			PrintPara.SubnetMask[1] = SubnetMask[1];
//			PrintPara.GatewayIP[0] = GatewayIP[0];
//			PrintPara.GatewayIP[1] = GatewayIP[1];
			PrintPara.sPlotType = gl_sPlotType;
			PrintPara.sNull1 = 0;
//			PrintPara.iOriginalPos = gl_iOriginalPos;
			PrintPara.iYDotSpace12 = gl_iYDotSpace12 - SprinklerSpace;
			PrintPara.iYDotSpace23 = gl_iYDotSpace23 - SprinklerSpace;
			PrintPara.iYDotSpace34 = gl_iYDotSpace34 - SprinklerSpace;
			PrintPara.iSNUll = 0xffffffff;
/*
			cTemp = (unsigned char *)&gl_dErrorX;
			i= 54;
			for (cTemp1=0;cTemp1<8;cTemp1++)
			{
				gl_cUsbSendBuf[i++] = *cTemp++;
			}
			
			cTemp = (unsigned char *)&gl_dErrorY;
			for (cTemp1=0;cTemp1<8;cTemp1++)
			{
				gl_cUsbSendBuf[i++] = *cTemp++;
			}

			cTemp = (unsigned char *)&gl_dPictureErrorX;
			for (cTemp1=0;cTemp1<8;cTemp1++)
			{
				gl_cUsbSendBuf[i++] = *cTemp++;
			}

			cTemp = (unsigned char *)&gl_dPictureErrorY;
			for (cTemp1=0;cTemp1<8;cTemp1++)
			{
				gl_cUsbSendBuf[i++] = *cTemp++;
			}
			*/
			PrintPara.dErrorX = gl_dErrorX;
			PrintPara.dErrorY = gl_dErrorY;
			PrintPara.dPictureErrorX = gl_dPictureErrorX;
			PrintPara.dPictureErrorY = gl_dPictureErrorY;
			
//			PrintPara.dPictureErrorX = gl_dPictureErrorX2;
//			PrintPara.dPictureErrorY = gl_dPictureErrorY2;
			
			length = sizeof(PrintPara);// - 32;
			//Printf("SendLenght[%d] sizeof[%d]\n",length, sizeof(short));
			//Printf("SendPara1:SC[%d],SN[%d],BT[%d], LW[%d]\n", gl_cSprinklerCount, gl_cSprinklerNo, gl_cBidirectional, gl_cLineWidth);
			//Printf("SendPara2:PN[0x%d],CN[0x%d],PCS[0x%x]\n",gl_cPenNo, gl_cCutNo, gl_cPrintCutSelect);
			//Printf("SendPara3:ML1[%d],ML2[%d],ML3[%d],YS1[%d],YS2[%d],YS3[%d]\n", gl_cMultiDotLine12, gl_cMultiDotLine23, gl_cMultiDotLine34, PrintPara.iYDotSpace12, PrintPara.iYDotSpace23, PrintPara.iYDotSpace34);
			//Printf("SendPara4:DX[%d],DY[%d],DEX[%d],DEY[%d]\n",(int)gl_dPictureErrorX, (int)gl_dPictureErrorY, (int)gl_dErrorX, (int)gl_dErrorY);
			memcpy(&gl_cUsbSendBuf[14],(unsigned char *)&PrintPara,length);//14
			length += 14;
			//length += 46;
			
			//Printf("Msg_Read[%d]\n", length);
			//for(i=0; i<length; i++)
			//	Printf("0x%x--[0x%x] /", i, gl_cUsbSendBuf[i]);
			//Printf("\n");
			
			//cTemp = (unsigned char *)&gl_dErrorX;
			//for(i=0; i<8; i++)
			//	Printf("%x,", *cTemp++);
			///Printf("\n");
			
			if (length > 63)
			{
				gl_cUsbSendBufPH = length;
				gl_cUsbSendBufPL = 63;
				length = 63;
			}
			CH375_WR_CMD_PORT(WR_USB_DATA7);
			CH375_WR_DAT_PORT(length+1);//当前发送的字节数
			CH375_WR_DAT_PORT(gl_cUsbSendBufPH);//要发送的总字节数
			for (i=0;i<length;i++)
				CH375_WR_DAT_PORT(gl_cUsbSendBuf[i]);
			//Printf("3,");
		}
		else if (memcmp(buffer,"Msg_Comm",8) == 0)//命令
		{
			if (length > 0)
			{
				CH375_RD_DAT_PORT();
				cOrder = CH375_RD_DAT_PORT();
				length -= 2;
				//Printf("cOrder:[%d]\n", cOrder);
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
				//Printf("4,");
			}
		}
	}
	//if(gl_lRecDataLen==0)
	//Printf("ReceiveUsbData[%d] length[%d]\n", iRecDataLen, length);
//	gl_lRecDataLen = iRecDataLen;  //20140806
}
				
void UsbCutDataToDram(unsigned int iLength)
{
	unsigned long address;				//当前缓冲区写入地址
	unsigned int iRemainCount;//
	unsigned char cData;
	static unsigned long lAddLength=0;

	if(iLength == 0)
		return;
	
	lAddLength += iLength;
	
	address = gl_iEthnetRecBufPH + CutDataAddr;
	iRemainCount = CutDataLength- gl_iEthnetRecBufPH;
	//Printf("\n\n");
	while(iLength>0)
	{
		cData = CH375_RD_DAT_PORT();
		Writesdram8bit(address, cData);
		address++;
		iRemainCount--;
		iLength--;
		//Printf("CUTDram::::iL[%d],addr[0x%x] data:[%c ] \n", iLength, address,cData);
		//Printf("%c",cData);
	}
	//Printf("\n\n");
	gl_iEthnetRecBufPH = address - CutDataAddr;
//	Printf("cToDram::::iL[%d],addr[0x%x]\n", iLength, address);
	if ( lAddLength == gl_lCurrCutFileLength)
	{
		lAddLength = 0;
		gl_cFileReceive = 0;
		//Printf("address: %x, lAddLength: %d, gl_iEthnetRecBufPH: %d\n", address, lAddLength, gl_iEthnetRecBufPH);
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
	unsigned int iCountBak,iRemainCount;//iRemainCount	缓冲区剩余空间
	unsigned long address;				//当前缓冲区写入地址

	if ((gl_cFileReceive != 0x01) && (gl_cFileReceive != 0x09))		//没有接收到文件开始标志
	{//当前文件没有开始传送
		while (iLength--)
			CH375_RD_DAT_PORT();
		return;
	}
			
	if (gl_cPrintStatus == 0x20)
		gl_cPrintStatus = 0;
	address = gl_iEthnetRecBufPH + BEGINADDR;
	//Printf("UWaddr:[0x%x]\n", address);
	iRemainCount = EthnetBufLength - gl_iEthnetRecBufPH;
	if (cCountFlag != 0)
	{//已接收到字节数
		iCount1 = (unsigned int)cCountBak;
		iData = (unsigned short)CH375_RD_DAT_PORT();
		iLength--;
	}
	else
		iCount1 = 0;
	//Printf("UsbDataToDram[%d]\n", iLength);
	while (1)
	{
		if (iCount1 != 0)
		{
			if ((address & 0x01) == 1)
			{
				address--;
				iData1 = Readsdram(address, 16);
				iData1 |= (iData << 8);
				//if(iData1!=0)
				//Printf("[a:0x%x]", iData1);
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
					//if(iData1!=0)
					//Printf("[b:0x%x]", iData1);
				
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
				//if(iData1!=0)
				//Printf("[c:0x%x]", iData1);
			
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
	//Printf("USB END[%d], ", gl_iEthnetRecBufPH);
}

extern void  ReturnOriginal(int *iCurrPosX, int *iCurrPosY);
extern void LcdDisp(char * s1, char * s2);
extern void RunCutMod(void);
extern void InitFilePaper(void);

unsigned char DealUsbCommand(unsigned char *length,unsigned char cOrder)
{
	unsigned char ResultCode;
	short sTemp1;
	unsigned char *cTemp;
	unsigned char cTemp1,cTemp2,cTemp3;
	int iTemp1,iTemp2,iTemp3;
	int iTemp, icu, iYPlus;//, iXPlus;
	double dTemp;
	PrintSetPara PrintPara;
	//unsigned long  AddrCut;
	//unsigned char CutData;
	//int i;
	
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
				//Printf("Print length<4\n");
				ResultCode = 1;
				break;
			}
			
			if(cReviceFlag == 1)
			{
				//Printf("No\n");
				ResultCode = 2;
				break;
			}
			
			if (gl_cPrintStatus == 0x10)
			{
				ResultCode = 2;
				break;
			}
			
//			Printf("gl_sMachineWidth:%d\n",gl_sMachineWidth);
//			Printf("*length:%d\n",*length);
			//当前正在执行切割或者当前内存区域内有数据文件未执行，不能接收新的文件 20120711
			if(gl_cCutFileSendFlag == 0x03)
			{
				Printf("Cut Work\n");
				ResultCode = 2;
				break;
			}
			/*
			iTemp1 = 0;
			for (cTemp1=0;cTemp1<4;cTemp1++)
				iTemp1 = (iTemp1 << 8) + CH375_RD_DAT_PORT();
			*length = *length - 4;
			Printf("iTemp1:%d,  gl_iMaxPictureWidth:%d\n",iTemp1, gl_iMaxPictureWidth);
			if (iTemp1 > gl_iMaxPictureWidth)
			{
				ResultCode = 3;
				break;
			}
			*/
			
			iTemp1 = 0;
			for (cTemp1=0;cTemp1<4;cTemp1++)
				iTemp1 = (iTemp1 << 8) + CH375_RD_DAT_PORT();
			*length = *length - 4;
			Printf("iTemp2:%d,  gl_sMachineWidth:%d\n",iTemp1, gl_sMachineWidth);
			/*if ((iTemp1 > gl_sMachineWidth) || (iTemp1 < 0))
			{
				Printf("EMWidth:%d\n", iTemp1);
				ResultCode = 3;
				break;
			}*/
			gl_iCurPictureWidth = iTemp1;

			iTemp1 = 0;
			for (cTemp1=0;cTemp1<4;cTemp1++)
				iTemp1 = (iTemp1 << 8) + CH375_RD_DAT_PORT();
			*length = *length - 4;
			Printf("iTemp3:%d,  gl_sMachineLenght:%d\n",iTemp1, gl_sMachineLenght);
			/*if ((iTemp1 > gl_sMachineLenght) || (iTemp1 < 0))
			{
				Printf("EMLenght:%d\n", iTemp1);
				ResultCode = 3;
				break;
			}			*/
			gl_iCurPictureLength = iTemp1;
			// 判断当前位置是否可以切绘完麦架 20120822
			//gl_iYMaxEffectivePMM = gl_lMaxY - ((gl_iOriginalPos_Y+10) * Y_PPMM);
			//gl_iXMaxEffectivePMM = gl_lMaxX - ((gl_iOriginalPos_X+10) * X_PPMM);
			iYPlus = gl_iCurPictureWidth * Y_PPMM; //( 207 + gl_iCurPictureWidth * 8 + AdditionDot * 2 + OriginalSideDot + MotorSideDot) * DotPulseNum;
			//iXPlus = gl_iCurPictureLength * X_PPMM; //( gl_iCurPictureLength * 8 + 50) * DotPulseNum;
//			Printf("IS PLT DATA FILE!!!\n");
//			Printf("EffectYMaxPmm: %d, EffectXMaxPmm: %d\n",gl_iYMaxEffectivePMM, gl_iXMaxEffectivePMM);
//			Printf("CurrYPmm: %d\n", iYPlus);
//			Printf("CurrPltWith: %d, CurrPltLenght: %d\n", gl_iCurPictureWidth, gl_iCurPictureLength);
			//if ((iYPlus > gl_iYMaxEffectivePMM) || (iXPlus > gl_iXMaxEffectivePMM))
			if (iYPlus > gl_iYMaxEffectivePMM)
			{
				ResultCode = 3;	
				break;
			}
			////////////////////////////////////
			if (gl_cPrintStatus == 0x10)
			{//  Y轴报警
				//Printf("gl_cPrintStatus == 0x10\n");
				ResultCode = 2;
				break;
			}
			if (gl_cPrintStatus == 0x20)
			{//	Printf("gl_cPrintStatus == 0x20\n");
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
						Printf("gl_iEthnetRecBufPE != gl_iEthnetRecBufPH\n");
						ResultCode = 2;
						break;
					}
					else if (gl_iPulseNumY < 0)
					{//喷头回原点
						gl_cFileReceive = 0xe1;
						Printf("gl_iPulseNumY < 0\n");
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
					Printf("(gl_cFileReceive & 0x60) != 0\n");
					break;
				}
			}
			Printf("gl_sMachineWidth:%d\n",gl_sMachineWidth);
			gl_lCurrFileLenghtPluse = 0;
			
			//*************20130807机头定位到切割位置
			Printf("A RStatus:%d, sp:%d , MoveP:%d\n", gl_cReacterStatus, gl_cSendPaperStar, gl_iMovePaperLength);
			if(gl_cAutoSendPaperSet != 0)
				//InitFilePaper();//20150409-rwt-test_remove;
			Printf("B RStatus:%d, sp:%d , MoveP:%d\n", gl_cReacterStatus, gl_cSendPaperStar, gl_iMovePaperLength);
			gl_cReacterStatus = 0;
			//PrintReturnZero();
			//PrintHeadClean(2);
			
			gl_iGraphWidth = gl_iCurPictureWidth;
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
			gl_MacRunState = 1;	//文件开始打印，只响应暂停键

			/*20141201 rwt*/
			if (gl_cLanguageSelect == 0)
			{
				LcdDisp("正在执行喷绘" , "请稍后......");// 正在执行喷绘任务
			}
			else
			{
				LcdDisp("Executing print" , "Please Wait...");// 正在执行喷绘任务
			}
			//在这传输数据       改变设备的速度使其不对太快影响他的精度
//			Printf("PH[%d], PE[%d]\n",gl_iEthnetRecBufPH, gl_iEthnetRecBufPE);
//			Printf("Start Print gl_iAxisYPos:%d, gl_iOriginalPos:%d\n", gl_iAxisYPos, gl_iOriginalPos);
			
			gl_TestSendLengPluse=0;
			gl_cStartWorkFlag = 1; //20141021
			// 20141021 修改if (gl_iAxisYPos <= gl_iOriginalPos)
			if (gl_iAxisYPos <= gl_iOriginalPos)//y轴在原点的左侧(左右是相对于人来看) 
			{
				if(gl_cSWFStartClearFlag != 1)	//20141215 rwt
				{
					gl_cSWFStartClearFlag = 1;
				}
				Printf("gl_iAxisYPos  = %d  gl_iOriginalPos = %d \n", gl_iAxisYPos ,gl_iOriginalPos );
				ClearPrintHead(0);//每次打印文件前清洗喷头20151104
				//纸张回到最低点
				{
					while(GetIOBit(I_GIVEBACK) != StatusOk)
					{	if (GetIOBit(I_GIVEBACK) == StatusErr)
						{
							SetIOBit(O_GIVEMOTORDIR,G_SENDPAPER);	//20141213 rwt
							SetIOBit(O_STARTGIVEPMOTOR,0);			//启动送纸电机
							gl_iSendPaper = 0;						//送纸计数器清零
							Printf("BBBB\n");
						}
						Printf("AAAA\n");
					}
					{
						SetIOBit(O_STARTGIVEPMOTOR,1);			//关闭送纸电机
						gl_iSendPaper = 0;						//送纸计数器清零
						Printf("BaBa\n");
					}
					Printf("AbAb\n");
				}
				Printf("usb1 ------->>>>清洗喷头\n");
			}
				Printf("usb2 ------->>>>没清洗喷头\n");
			//else
				//gl_cFileReceive |= 0x08;
			WriteFilePrint(0xaaaa);
			DrivePrint;
			ClrFIFO;
			gl_cPrintWorkStatus = 1;
			gl_lAxisXPos = 0;		//开始打印前要把坐标脉冲清空 20120719
			//gl_iAxisYPos = 0;
			gl_cMachineWorkStatus = 1;	// 20120827 
			gl_cKeyStart = 0;	// 20120827
			//lXlimitNum = gl_lCurPost_XPluse;
			
			Printf("Send plt data\n");
			
			Printf("gl_cPrintStatus =[0x%x]\n", gl_cPrintStatus);
			break;
		case 2:	//文件传送结束命令
			if( gl_cCutFileSendFlag == 0x01)
			{//切割数据文件传送结束
				ResultCode = 9;
				gl_iEthnetRecBufPE = 0;
				gl_iEthnetRecBufPH = 0;
				gl_cCutFileSendFlag = 0;
				gl_cFileReceive = 0;
				gl_cInvertBufStatus = 0;
				gl_DeleteFileState = 0;
				//gl_cWorkStatus = 2;	//切割文件传输结束，准备切割
				Printf("CutFileSendEnd\n");
				gl_cCutWorkStatus = 1;
				//if(gl_cPrintCutSelect==0x02)				
				//RunCutMod();
				//return 0;
				break;
			}
			
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
			ResultCode = 9;
			Printf("gl_sMachineWidth:%d\n",gl_sMachineWidth);
			Printf("***Plt file Receive ok! PE[%d] PH[%d]****\n", gl_iEthnetRecBufPE, gl_iEthnetRecBufPH);
			break;
			
		case 3: 	//  切割文件传送命令
			Printf("gl_sMachineWidth:%d\n",gl_sMachineWidth);
			if (*length < 4)
			{
				Printf("rev length < 4\n");
				ResultCode = 1;
				break;
			}
			//当前正在执行切割或者当前内存区域内有数据文件未执行，不能接收新的文件 20120711
			if((gl_cCutFileSendFlag == 0x03) || (gl_cCutWorkStatus==1))
			{
				//Printf("Work....\n");
				ResultCode = 2;
				break;
			}
			
			iTemp1 = 0;
			for (cTemp1=0;cTemp1<4;cTemp1++)
				iTemp1 = (iTemp1 << 8) + CH375_RD_DAT_PORT();
			if(iTemp1 > CutDataLength)
			{
//				Printf("EDLength:%d\n", iTemp1);
				ResultCode = 3;
				break;
			}
			gl_lCurrCutFileLength = iTemp1;
////////////////20120822 校验麦架长宽是否可打 ///////////
			iTemp1 = 0;
			for (cTemp1=0;cTemp1<4;cTemp1++)
				iTemp1 = (iTemp1 << 8) + CH375_RD_DAT_PORT();
			*length = *length - 4;
//			Printf("iTemp2:%d,  gl_sMachineWidth:%d\n",iTemp1, gl_sMachineWidth);
			/*if ((iTemp1 > gl_sMachineWidth) || (iTemp1 < 0))
			{
				Printf("EMWidth:%d\n", iTemp1);
				ResultCode = 3;
				break;
			}*/
			gl_iCurPictureWidth = iTemp1;

			iTemp1 = 0;
			for (cTemp1=0;cTemp1<4;cTemp1++)
				iTemp1 = (iTemp1 << 8) + CH375_RD_DAT_PORT();
			*length = *length - 4;
		//	Printf("iTemp3:%d,  gl_sMachineLenght:%d\n",iTemp1, gl_sMachineLenght);
			/*if ((iTemp1 > gl_sMachineLenght) || (iTemp1 < 0))
			{
				Printf("EMLenght:%d\n", iTemp1);
				ResultCode = 3;
				break;
			}	*/
			gl_iCurPictureLength= iTemp1;	

			//gl_iYMaxEffectivePMM = gl_lMaxY - ((gl_iOriginalPos_Y+10) * Y_PPMM);
			//gl_iXMaxEffectivePMM = gl_lMaxX - ((gl_iOriginalPos_X+10) * X_PPMM);
			iYPlus = gl_iCurPictureWidth * Y_PPMM; 
			//iXPlus = gl_iCurPictureLength * X_PPMM;
			//iYPlus = (gl_iCurPictureWidth * Y_PPMM) + ((SprinklerSpace + AdditionDot * 2 + OriginalSideDot + MotorSideDot) * DotPulseNum);
			//iXPlus = ( gl_iCurPictureLength * X_PPMM) + (50 * DotPulseNum);
//			Printf("IS CUT DATA FILE!!!\n");
//			Printf("EffectYMaxPmm: %d, EffectXMaxPmm: %d\n",gl_iYMaxEffectivePMM, gl_iXMaxEffectivePMM);
//			Printf("CurrYPmm: %d\n", iYPlus);
//			Printf("CurrPltWith: %d, CurrPltLenght: %d\n", gl_iCurPictureWidth, gl_iCurPictureLength);
			//if ((iYPlus > gl_iYMaxEffectivePMM) || (iXPlus > gl_iXMaxEffectivePMM))
			if (iYPlus > gl_iYMaxEffectivePMM)
			{
				ResultCode = 3;	
				break;
			}
/////////////////////////////////////////////////////////			
			if (gl_cPrintStatus == 0x10)
			{
				//Printf("gl_cPrintStatus == 0x10\n");
				ResultCode = 2;
				break;
			}
			if (gl_cPrintStatus == 0x20)
			{
				//Printf("gl_cPrintStatus == 0x20\n");
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
 						//Printf("gl_cPrintStatus == 0x60\n");
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
			gl_iGraphWidth = gl_iCurPictureWidth;
			gl_cCutFileSendFlag = 0x01;
			gl_cFileReceive = 0x01;
			gl_cCutWorkStatus = 0;
			ResultCode = 9;
			gl_iEthnetRecBufPH = 0;
			gl_iEthnetRecBufPE = 0;
			gl_cPrintStatus = 0;
			gl_cMachineWorkStatus = 1;	// 20120827 
			gl_cKeyStart = 0;	// 20120827
			//Printf("CUT Receive Data\n");
			//Printf("CFL:%d, PH:%d,PE:%d,PS:%d\n",gl_lCurrCutFileLength,gl_iEthnetRecBufPH, gl_iEthnetRecBufPE, gl_cPrintStatus);
			break;
			
		case 4:
			if(gl_cFileReceive != 0)
				gl_cFileReceive |= 0x10;	//删除当前打印文件
			gl_DeleteFileState = 1;
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
		case 8:	// 查询当前状态是否处于空闲
			if (gl_cMachineWorkStatus == 0)
			{
				//if (gl_cKeyStart == 1)
				//	ResultCode = 7;	// 已经按键启动上位机打印程序
				//else
					ResultCode = 6;	// 当前设备空闲
			}
			else
			{
				ResultCode = 2;
				//Printf("T");
			}
			break;
		case 10:	//相邻两次扫描之间X轴和Y轴的误差
			Printf("10相邻两次扫描之间X轴和Y轴的误差");
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
			Printf("14打印走纸误差修正图");
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
			Printf("15打印喷头间Y方向误差修正图");
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
			if (SparyError(1) == 1)//进入函数
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
			Printf("17打印双向打印误差修正图");
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
			if (DrawMalposedGraph(iTemp1) == 1)//进入函数
				ResultCode = 1;
			else
				ResultCode = 9;
			lcd_refresh();
			LineReverse(0x40);
			gl_dErrorY = dTemp;
			gl_iStaggerDot = (int)(gl_dErrorY * Y_PPMM / DotPulseNum);
			break;
		case 81:
			icu = 0;
			CH375_RD_DAT_PORT();icu++; 
			PrintPara.cBidirectional = CH375_RD_DAT_PORT();icu++;
			CH375_RD_DAT_PORT();icu++;
			PrintPara.cSprinklerCount = CH375_RD_DAT_PORT();icu++;
			CH375_RD_DAT_PORT();icu++;
			PrintPara.cSprinklerNo = CH375_RD_DAT_PORT();icu++;
			CH375_RD_DAT_PORT();icu++;
			PrintPara.cLineWidth = CH375_RD_DAT_PORT();icu++;
			CH375_RD_DAT_PORT();icu++;
			PrintPara.cMultiDotLine12 = CH375_RD_DAT_PORT();icu++;
			
			sTemp1 = (unsigned short)CH375_RD_DAT_PORT();icu++;
			sTemp1 = (sTemp1 << 8) | (unsigned short)CH375_RD_DAT_PORT();icu++;
			PrintPara.iYDotSpace12 = (unsigned int)(SprinklerSpace + sTemp1);
			sTemp1 = (unsigned short)CH375_RD_DAT_PORT();icu++;
			sTemp1 = (sTemp1 << 8) | (unsigned short)CH375_RD_DAT_PORT();icu++;
			PrintPara.iYDotSpace23 = (unsigned int)(SprinklerSpace + sTemp1);
			sTemp1 = (unsigned short)CH375_RD_DAT_PORT();icu++;
			sTemp1 = (sTemp1 << 8) | (unsigned short)CH375_RD_DAT_PORT();icu++;
			PrintPara.iYDotSpace34 = (unsigned int)(SprinklerSpace + sTemp1);
			//走纸误差20100413
			cTemp = (unsigned char *)&PrintPara.dErrorX ;
			for (iTemp=0;iTemp<8;iTemp++)
			{
				*cTemp = CH375_RD_DAT_PORT();icu++;
				cTemp++;
			}	
			iTemp = (int)(PrintPara.dErrorX  * X_PPMM);
			if ((iTemp > gl_iTHPulseNumX) || (iTemp < -gl_iTHPulseNumX))
			{
				Printf("revice -1\n");
				ResultCode = 1;			//参数不正确
				break;
			}
			//*****		
			
			cTemp = (unsigned char *)&PrintPara.dErrorY;
			for (cTemp1=0;cTemp1<8;cTemp1++)
			{
				*cTemp = CH375_RD_DAT_PORT();icu++;
				cTemp++;
			}
			//20100302
			if ((PrintPara.dErrorY > 6.0) || (PrintPara.dErrorY < -6.0))
			{
				Printf("revice 0\n");
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
				*cTemp = CH375_RD_DAT_PORT();icu++;
				cTemp++;
			}
			cTemp = (unsigned char *)&PrintPara.dPictureErrorY;
			for (cTemp1=0;cTemp1<8;cTemp1++)
			{
				*cTemp = CH375_RD_DAT_PORT();icu++;
				//Printf("JSY:%x ", *cTemp);
				
				cTemp++;
			}

			// 20101206 笔号刀号
			CH375_RD_DAT_PORT();icu++;
			PrintPara.cPenNo = CH375_RD_DAT_PORT();icu++;
			CH375_RD_DAT_PORT();icu++;
			PrintPara.cCutNo = CH375_RD_DAT_PORT();icu++;
			CH375_RD_DAT_PORT();icu++;
			// 20101206 打印切割选择
			CH375_RD_DAT_PORT();icu++;
			PrintPara.cPrintCutSelect = CH375_RD_DAT_PORT();icu++;
			//Printf("SetPara1:PN[%d],CN[%d],PCS[%d]\n",PrintPara.cPenNo, PrintPara.cCutNo, PrintPara.cPrintCutSelect);
			
			if ((PrintPara.cSprinklerCount > SprinklerNum) || (PrintPara.cSprinklerCount == 0)
				|| (PrintPara.cSprinklerNo > (SprinklerNum - PrintPara.cSprinklerCount)))
			{
				Printf("revice 1\n");
				ResultCode = 1;
				break;
			}
			if ((PrintPara.cBidirectional != 0) && (PrintPara.cBidirectional != 1))
			{
				Printf("revice 2\n");
				ResultCode = 1;
				break;
			}
			if ((PrintPara.cLineWidth < 1) || (PrintPara.cLineWidth > 9))
			{
				Printf("revice 3\n");
				ResultCode = 1;
				break;
			}
			if (PrintPara.cMultiDotLine12 >= SprinklerWidth)
			{
				Printf("revice 4\n");
				ResultCode = 1;
				break;
			}
			//if (PrintPara.cMultiDotLine23 >= SprinklerWidth)
			//{
			//	Printf("revice 5\n");
			//	ResultCode = 1;
			//	break;
			//}
			//if (PrintPara.cMultiDotLine34 >= SprinklerWidth)
			//{
			//	Printf("revice 6\n");
			//	ResultCode = 1;
			//	break;
			//}
			if ((PrintPara.iYDotSpace12 > (SprinklerSpace + MaxMalposedDot)) || (PrintPara.iYDotSpace12 < (SprinklerSpace - MaxMalposedDot)))
			{
				Printf("revice 7\n");
				ResultCode = 1;
				break;
			}
			if ((PrintPara.iYDotSpace23 > (SprinklerSpace + MaxMalposedDot)) || (PrintPara.iYDotSpace23 < (SprinklerSpace - MaxMalposedDot)))
			{
				Printf("revice 8\n");
				ResultCode = 1;
				break;
			}
			if ((PrintPara.iYDotSpace34 > (SprinklerSpace + MaxMalposedDot)) || (PrintPara.iYDotSpace34 < (SprinklerSpace - MaxMalposedDot)))
			{
				Printf("revice 9\n");
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
				Printf("revice 10\n");
				ResultCode = 1;
				break;
			}
			if ((PrintPara.dPictureErrorY > 1100.0) || (PrintPara.dPictureErrorY < 900.0))
			{
				Printf("revice 11\n");
				ResultCode = 1;
				break;
			}
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
			gl_iYDotSpace12 = (unsigned int)PrintPara.iYDotSpace12;
			gl_iYDotSpace23 = (unsigned int)PrintPara.iYDotSpace23;
			gl_iYDotSpace34 = (unsigned int)PrintPara.iYDotSpace34;
			gl_dErrorX = PrintPara.dErrorX ; //走纸误差20100413
			gl_dPictureErrorX = PrintPara.dPictureErrorX;
			gl_dPictureErrorY = PrintPara.dPictureErrorY;
			
			Printf("revice[%d]\n",icu);
			Printf("SetPara1:SC[%d],SN[%d],BT[%d], LW[%d]\n", gl_cSprinklerCount, gl_cSprinklerNo, gl_cBidirectional, gl_cLineWidth);
			Printf("SetPara2:PN[0x%d],CN[0x%d],PCS[0x%x]\n",gl_cPenNo, gl_cCutNo, gl_cPrintCutSelect);
			Printf("SetPara3:ML1[%d],ML2[%d],ML3[%d],YS1[%d],YS2[%d],YS3[%d]\n", gl_cMultiDotLine12, gl_cMultiDotLine23, gl_cMultiDotLine34, gl_iYDotSpace12, gl_iYDotSpace23, gl_iYDotSpace34);
			Printf("SetPara4:DX[%d],DY[%d],DEX[%d],DEY[%f]\n",(int)gl_dPictureErrorX, (int)gl_dPictureErrorY, (int)gl_dErrorX, gl_dErrorY);
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
	Printf("cOrder  = %d _____ ResultCode = %d _____ gl_cMachineWorkStatus = %d \n" , cOrder  , ResultCode ,gl_cMachineWorkStatus);
	return ResultCode;
}

