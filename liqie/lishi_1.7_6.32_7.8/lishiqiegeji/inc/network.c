//*******************************************************************
//文件名：network.c
//描述：	网络驱动程序
//版本：	1.0
//作者：	wangwei
//日期：	2007-06-11
//*******************************************************************
#include <string.h>
#include <stdio.h>
#include "..\Target\44blib.h"
#include "..\src\network.h"
#include "..\src\extern.h"


void	csWritePacketPage(unsigned short Offset,unsigned short Value);
void	Init8900(void);
void	ArpAnswer(void);
void	CopyFrameFromBE(unsigned short Offset,unsigned short Size);
void	csReceiveEvent( unsigned short RxEvent );
void	Delete_Socket(void);
void	PollRecNetData(void);
void	Prepare_ICMP_Answer(void);
void	Process_ICMP_Frame(void);
void	Process_UDP_Frame(void);
void	SendRstFrame(void);
void	Tcp_Last_Ack(void);
void	Tcp_Listen(void);
void	Tcp_SYN_Rec(void);
void	TcpHeadHandle(unsigned short Tcp_headlength_Control);
void	SendFrame(unsigned short length);
void	from_ethnet(unsigned char *cHeadData,unsigned int iLength);
void	ArpRequest(void);
unsigned	int		CanRevChar(void);
unsigned	short DealEthnetCom(unsigned short com);
unsigned	short SwapByte(unsigned short dat);
unsigned	short	csReadPacketPage(unsigned short Offset);
unsigned	short	CalcCheckSum(unsigned short *Start,unsigned short Count,unsigned short IsTCP,unsigned short IsUDP);
unsigned	long	csInitFromBSPDefs(void);
unsigned	long	csResetChip(void);
unsigned	long	csVerifyChip(void);
unsigned	short	Process_TCP_Frame(void);
unsigned	short	SwapWord(unsigned short Data);
unsigned	short	Tcp_Established(void);
unsigned	long	Word2ToLong(unsigned short *temp_ptr);
extern void LcdDisp(char * s1, char * s2);

extern	void	CalVariableValue(void);
extern	void	lcd_refresh(void);
extern	void	LineReverse(unsigned char line);
extern	void ClearPrintHead(char cOrgState);
extern	int		WriteParameter(void);
extern	unsigned short DrawMalposedGraph(unsigned int iGraphWidth);
extern	unsigned short SparyError(unsigned char cPtintCount);
extern  void from_ethnet_Cut(unsigned char *cHeadData,unsigned int iLength);

struct Socket_Type
{                                                                                                                              
	unsigned short  Dest_Port;		 		//对方端口
	unsigned short	Dest_Ip[2];	 			//对方ip
	unsigned long	RecPassSeq;
	unsigned long	RecPassAck;
	unsigned long	SenPassSeq;
	unsigned long	SenPassAck;
	unsigned long	RecFutureAck;
	unsigned long	SenFutureSeq;
	unsigned short	FrameIndex;
	unsigned short	Rcv_Window;		 		//对方的window大小
	unsigned short	Snd_Window; 		 	//我的window大小
	unsigned short	Dest_Max_Seg_Size;		//对方接受的最大的数据包大小MTU
	unsigned short	State;		//连接状态
//	uint8		StaSem;	//BIT7表示该连接是LISTEN开始的，即服务器的。BIT1是表示该连接等待AppSemCon信号量。BIT0是表示正等待AppSemSend信号量
}TcpStatus;

const unsigned short MyMAC[3] = {0xE002,0xA04C,0x7A7E};

//高低字节互换
unsigned short SwapByte(unsigned short dat)
{
	unsigned short sTemp1,sTemp2;
	
	sTemp1 = (dat & 0xff) << 8;
	sTemp2 = ((dat >> 8) & 0xff) | sTemp1;
	return sTemp2;
}


/*****************************************************************************/
/*  csReadPacketPage()                                                       */
/*                                                                           */
/*  Reads the PacketPage register at the specified offset.                   */
/*                                                                           */
/*****************************************************************************/
unsigned short csReadPacketPage(unsigned short Offset)
{
	unsigned short iTemp;
	
	CS8900_PKTPG_PTR = Offset;
	iTemp = CS8900_PKTPG_DATA;
  return iTemp;
}

/*****************************************************************************/
/*  csWritePacketPage()                                                      */
/*                                                                           */
/*  Writes to the PacketPage register at the specified offset.               */
/*                                                                           */
/*****************************************************************************/
void csWritePacketPage(unsigned short Offset,unsigned short Value)
{
	CS8900_PKTPG_PTR = Offset;
	CS8900_PKTPG_DATA = Value;
}

/*****************************************************************************/
/*  csVerifyChip()                                                           */
/*                                                                           */
/*  This routine verifies that the Crystal Semiconductor chip is present     */
/*  and that it is a CS8900.                                                 */
/*                                                                           */
/*  IMPORTANT!  This routine will fail if the IO base address programmed     */
/*              in the chip's EEPROM does not match the IO base address      */
/*              specified with BSP_CS8900_IO_BASE in the BSP.H file.         */
/*                                                                           */
/*****************************************************************************/
unsigned long csVerifyChip(void)
{
   /* See if the PacketPage Pointer port contains the correct signature */
   if ((CS8900_PKTPG_PTR & SIGNATURE_PKTPG_PTR) != SIGNATURE_PKTPG_PTR)
      return FAILURE;

   /* Verify that the chip is a Crystal Semiconductor chip */
   if (csReadPacketPage(PKTPG_EISA_NUM) != EISA_NUM_CRYSTAL)
      return FAILURE;

   /* Verify that the chip is a CS8900 */
   if ((csReadPacketPage(PKTPG_PRODUCT_ID) & PROD_ID_MASK) != PROD_ID_CS8900)
      return FAILURE;

   return SUCCESS;
}

/******************************************************************/
/*  csResetChip()                                                 */
/*                                                                */
/*  This routine resets the chip                                  */
/*                                                                */
/******************************************************************/
unsigned long csResetChip(void)
{
	unsigned short iTemp;

	/* Issue a reset command to the chip */
	csWritePacketPage(PKTPG_SELF_CTL,SELF_CTL_RESET|SELF_CTL_ID);

	/* Delay for 1 ms */
	Delay(2);

	/* Wait until the EEPROM is not busy */
	for (iTemp=0; iTemp < MAXLOOP; iTemp++)
	{
		if (!(csReadPacketPage(PKTPG_SELF_ST) & SELF_ST_SI_BUSY))
			break;
	}
	if (iTemp == MAXLOOP)
		return FAILURE;

	/* Wait until initialization is done */
	for (iTemp=0; iTemp < MAXLOOP; iTemp++)
	{
		if (csReadPacketPage(PKTPG_SELF_ST) & SELF_ST_INIT_DONE)
			break;
	}
	if (iTemp == MAXLOOP)
		return FAILURE;

	return SUCCESS;
}

/*****************************************************************************/
/*  csInitFromBSPDefs()                                                      */
/*                                                                           */
/*  This routine initializes the chip using definitions in the BSP.H file.   */
/*  This routine also converts the hardware address string specified by the  */
/*  global variable.  The defined interrupt level is stored in the           */
/*  csIntNumber global variable.                                             */
/*                                                                           */
/*****************************************************************************/
unsigned long csInitFromBSPDefs(void)
{
	unsigned short BusCtl;

	/* If IOCHRDY is disabled then set the bit in the BusCtl register */
	BusCtl = csReadPacketPage(PKTPG_BUS_CTL);
	csWritePacketPage(PKTPG_BUS_CTL,BusCtl | BUS_CTL_IOCHRDY);

	/* Set the Line Control register to match the media type */
	csWritePacketPage(PKTPG_LINE_CTL,LINE_CTL_10BASET|LINE_CTL_ID);

//   /* Set the BSTATUS/HC1 pin to be used as HC1 */
//   /* HC1 is used to enable the DC/DC converter */
//   SelfCtl = SELF_CTL_HC1E;
//
//   /* Disable the DC/DC converter */
//   /* If the DC/DC converter has a high enable */
//   /* Set the HCB1 bit, which causes the HC1 pin to go low */
//   SelfCtl |= SELF_CTL_HCB1;
//   csWritePacketPage( PKTPG_SELF_CTL, SelfCtl );

	/* Convert and save the Individual Address string */
	return SUCCESS;
}

/**********************************************************************
**函数原型:   void 	Init8900()
**入口参数:		无
**出口参数:		无
**返 回 值:		无              
**说    明:		对芯片的工作寄存器进行设置,各个寄存器的用法可参考文档和
**				络芯片的数据手册
************************************************************************/
void Init8900(void)
{
	unsigned short iTemp;
	
	Delay(10);
	
	/* Reset the chip */
	if (csResetChip() == FAILURE)
		return;

/* Verify that it is the correct chip */
	if (csVerifyChip() == FAILURE)
		return;

	if (csInitFromBSPDefs() == FAILURE)
		return;
	
	/* Initialize the config and control registers */
	csWritePacketPage(PKTPG_RX_CFG,RX_CFG_RX_OK_IE|RX_CFG_ID);//关所有中断
	csWritePacketPage(PKTPG_RX_CTL,RX_CTL_RX_OK_A|RX_CTL_IND_A|RX_CTL_BCAST_A|RX_CTL_ID);
	csWritePacketPage(PKTPG_TX_CFG,TX_CFG_ALL_IE|TX_CFG_ID);
	/* @jla added to support rdy4tx interrupt*/
	csWritePacketPage(PKTPG_BUF_CFG,BUF_CFG_RDY4TX_IE|BUF_CFG_ID); 
	
	/* Put hardware address into the Individual Address register */
	csWritePacketPage(PKTPG_IND_ADDR,SwapWord(MyMAC[0]));
	csWritePacketPage(PKTPG_IND_ADDR+2,SwapWord(MyMAC[1]));
	csWritePacketPage(PKTPG_IND_ADDR+4,SwapWord(MyMAC[2]));
	
	iTemp = csReadPacketPage(PKTPG_LINE_CTL) | LINE_CTL_RX_ON | LINE_CTL_TX_ON;
	csWritePacketPage(PKTPG_LINE_CTL,iTemp);
	
	RemoteIP[0] = 0xC0A8;
	RemoteIP[1] = 0x0176;
	Delete_Socket();
	Delay(100);
	ArpRequest();
}

/**********************************************************************
**函数原型：    void        Delete_Socket()
**入口参数: 	无
**出口参数: 	无
**返 回 值：	无
**功能说明: 	撤销本地连接，并清空重发缓冲区
******************************************************************/
void Delete_Socket(void) 
{
	TcpStatus.State = TCP_STATE_LISTEN;
	TcpStatus.RecPassSeq = 0;
	TcpStatus.RecPassAck = 0;
	TcpStatus.SenPassSeq = 0;
	TcpStatus.SenPassAck = 0;
	TcpStatus.RecFutureAck = 0;
	TcpStatus.SenFutureSeq = 0;
	TcpStatus.FrameIndex = 0;
	TcpStatus.Dest_Port = 0;
	TcpStatus.Dest_Ip[0] = 0;
	TcpStatus.Dest_Ip[1] = 0;
	TcpStatus.Snd_Window = MY_MAX_SEG_SIZE;
	RecdFrameIP[0] = 0;
	RecdFrameIP[1] = 0;
}

/*****************************************************************************/
/*  csReceiveEvent()                                                         */
/*                                                                           */
/*  This routine is called whenever a frame has been received at the chip.   */
/*  This routine gets a data buffer from pNA, copies the received frame      */
/*  into the data buffer and passes the frame up to pNA by calling pNA's     */
/*  Announce_Packet() entry point.                                           */
/*                                                                           */
/*****************************************************************************/
void csReceiveEvent( unsigned short RxEvent )
{
	unsigned short iHeadPH;
	unsigned short iTemp;
	unsigned short FrameType,IpHeaderLenth,ProtocolType;
	unsigned short TargetIP[2];
    
	/* Verify that it is an RxOK event */
	if (!(RxEvent & RX_EVENT_RX_OK))
		return;

	/* Read the header from the frame buffer */
	iHeadPH = 0;
	for (iTemp=0;iTemp < CS8900_HEADER_SIZE;iTemp++)
		RxEthnetFrameBuffer[iHeadPH++] = CS8900_RXTX_DATA0;
	
	for (iTemp=0;iTemp < ETH_HEADER_SIZE;iTemp++)
		RxEthnetFrameBuffer[iHeadPH++] = SwapByte(CS8900_RXTX_DATA0);
	
	FrameType = RxEthnetFrameBuffer[8];
	RecdFrameMAC[0] = *(RxEthnetFrameBuffer+ETH_HEADER_START+CS8900_HEADER_SIZE+3);
	RecdFrameMAC[1] = *(RxEthnetFrameBuffer+ETH_HEADER_START+CS8900_HEADER_SIZE+4);
	RecdFrameMAC[2] = *(RxEthnetFrameBuffer+ETH_HEADER_START+CS8900_HEADER_SIZE+5);
	
	/* Verify that the frame type is acceptable */
	if (FrameType == Frame_IP) //0x0800
	{	
		for (iTemp=0;iTemp < IP_HEADER_SIZE;iTemp++)
			RxEthnetFrameBuffer[iHeadPH++] = SwapByte(CS8900_RXTX_DATA0);
		
		if ((MyIP[0] == *(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+IP_HEADER_START+8))
	     && (MyIP[1] == *(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+IP_HEADER_START+9)))
	  {
			IpHeaderLenth = *(RxEthnetFrameBuffer+IP_HEADER_START+CS8900_HEADER_SIZE);
			if ((IpHeaderLenth & 0xFF00) == Ip_Edition)
			{
				IpHeaderLenth = 10;
				RecdIpFrameLenth = *(RxEthnetFrameBuffer+IP_HEADER_START+CS8900_HEADER_SIZE+1) / 2;
				ProtocolType = *(RxEthnetFrameBuffer+IP_HEADER_START+CS8900_HEADER_SIZE+4) & 0x00FF;
				RecdFrameIP[0] = *(RxEthnetFrameBuffer+IP_HEADER_START+CS8900_HEADER_SIZE+6);
				RecdFrameIP[1] = *(RxEthnetFrameBuffer+IP_HEADER_START+CS8900_HEADER_SIZE+7);
				switch(ProtocolType)
				{
					case PROTOCOL_ICMP:
						Process_ICMP_Frame();
						break;
					case PROTOCOL_TCP:
						Process_TCP_Frame();
						break;
					case PROTOCOL_UDP:
						Process_UDP_Frame();
						break;
					default:
						break;
				}
			}
		}
	}
	else if (FrameType == Frame_ARP)
	{
		for (iTemp=0;iTemp < ARP_FRAME_SIZE;iTemp++)
			RxEthnetFrameBuffer[iHeadPH++] = SwapByte(CS8900_RXTX_DATA0);
		
		iHeadPH = (iHeadPH - 2) * 2;
		iTemp = *(RxEthnetFrameBuffer+1);
		while (iHeadPH < iTemp)
		{
			SwapByte(CS8900_RXTX_DATA0);
			iHeadPH += 2;
		}
		
		if (*(RxEthnetFrameBuffer+ARP_HEADER_START+CS8900_HEADER_SIZE+2) == IP_HLEN_PLEN)
		{
			if (*(RxEthnetFrameBuffer+ARP_HEADER_START+CS8900_HEADER_SIZE+3) == OP_ARP_ANSWER)
			{
				RecdFrameMAC[0] = *(RxEthnetFrameBuffer+ARP_HEADER_START+CS8900_HEADER_SIZE+4);
				RecdFrameMAC[1] = *(RxEthnetFrameBuffer+ARP_HEADER_START+CS8900_HEADER_SIZE+5);
				RecdFrameMAC[2] = *(RxEthnetFrameBuffer+ARP_HEADER_START+CS8900_HEADER_SIZE+6);
				RecdFrameIP[0] = *(RxEthnetFrameBuffer+ARP_HEADER_START+CS8900_HEADER_SIZE+7);
				RecdFrameIP[1] = *(RxEthnetFrameBuffer+ARP_HEADER_START+CS8900_HEADER_SIZE+8);
			}
			else if (*(RxEthnetFrameBuffer+ARP_HEADER_START+CS8900_HEADER_SIZE+3) == OP_ARP_REQUEST)
			{
				RecdFrameIP[0] = *(RxEthnetFrameBuffer+ARP_HEADER_START+CS8900_HEADER_SIZE+7);
				RecdFrameIP[1] = *(RxEthnetFrameBuffer+ARP_HEADER_START+CS8900_HEADER_SIZE+8);
				TargetIP[0] = *(RxEthnetFrameBuffer+ARP_HEADER_START+CS8900_HEADER_SIZE+12);
				TargetIP[1] = *(RxEthnetFrameBuffer+ARP_HEADER_START+CS8900_HEADER_SIZE+13);
				if ((MyIP[0] == TargetIP[0]) && (MyIP[1] == TargetIP[1]))
					ArpAnswer();
			}
		}
		return;
	}
	/* Discard the received frame */
	iTemp = csReadPacketPage(PKTPG_RX_CFG) | RX_CFG_SKIP;
	csWritePacketPage(PKTPG_RX_CFG,iTemp);
	return;
}

/*****************************************************************************/
/*  csTransmitEvent()                                                        */
/*                                                                           */
/*  This routine is called whenever the transmission of a frame has          */
/*  completed (either successfully or unsuccessfully).  This routine         */
/*  removes the completed transmit request from the transmit request queue.  */
/*  If there are more transmit requests waiting, then start the transmission */
/*  of the next transmit request.                                            */
/*                                                                           */
/*****************************************************************************/
void csTransmitEvent(void)
{
//	unsigned short iTxEvent;
	
//	iTxEvent = csReadPacketPage(PKTPG_TX_EVENT);
}

unsigned short CalcCheckSum(unsigned short *Start,unsigned short Count,unsigned short IsTCP,unsigned short IsUDP)
{
	unsigned long Sum;
  
	Sum = 0;
	if ((IsTCP)||(IsUDP)) 
	{
		Sum += MyIP[0];
		Sum += MyIP[1];
		Sum += RecdFrameIP[0];
		Sum += RecdFrameIP[1];
		Sum += (Count*2);
		if(IsTCP)
			Sum += PROTOCOL_TCP;
		else
			Sum += PROTOCOL_UDP;
	}
	while (Count > 0) 
	{
		Sum += *Start;
		Start++;
		Count--;
	}
	Sum = (Sum & 0xFFFF) + ((Sum >> 16)&0xFFFF);
	if (Sum & 0xFFFF0000)
		Sum++;
	return ((unsigned short)((~Sum)&0xFFFF));
}

void ArpAnswer(void)
{
	*(TxEthnetFrameBuffer+ETH_HEADER_START+0) = RecdFrameMAC[0];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+1) = RecdFrameMAC[1];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+2) = RecdFrameMAC[2];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+3) = MyMAC[0];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+4) = MyMAC[1];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+5) = MyMAC[2];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+6) = Frame_ARP;
//ARP    
	*(TxEthnetFrameBuffer+ARP_HEADER_START+0) = HARDW_ETH;
	*(TxEthnetFrameBuffer+ARP_HEADER_START+1) = Frame_IP;
	*(TxEthnetFrameBuffer+ARP_HEADER_START+2) = IP_HLEN_PLEN;
	*(TxEthnetFrameBuffer+ARP_HEADER_START+3) = OP_ARP_ANSWER;
	*(TxEthnetFrameBuffer+ARP_HEADER_START+4) = MyMAC[0];
	*(TxEthnetFrameBuffer+ARP_HEADER_START+5) = MyMAC[1];
	*(TxEthnetFrameBuffer+ARP_HEADER_START+6) = MyMAC[2];
	*(TxEthnetFrameBuffer+ARP_HEADER_START+7) = MyIP[0];
	*(TxEthnetFrameBuffer+ARP_HEADER_START+8) = MyIP[1];
	*(TxEthnetFrameBuffer+ARP_HEADER_START+9) = RecdFrameMAC[0];
	*(TxEthnetFrameBuffer+ARP_HEADER_START+10) = RecdFrameMAC[1];
	*(TxEthnetFrameBuffer+ARP_HEADER_START+11) = RecdFrameMAC[2];
	*(TxEthnetFrameBuffer+ARP_HEADER_START+12) = RecdFrameIP[0];
	*(TxEthnetFrameBuffer+ARP_HEADER_START+13) = RecdFrameIP[1];
	TxFrameSize = ETH_HEADER_SIZE+ARP_FRAME_SIZE;
	SendFrame(TxFrameSize);
}

void Process_ICMP_Frame(void)
{
	unsigned short ICMPTypeAndCode;
  unsigned short iHeadPH;
  
  iHeadPH = CS8900_HEADER_SIZE + ETH_HEADER_SIZE + IP_HEADER_SIZE;
  RxEthnetFrameBuffer[iHeadPH++] = SwapByte(CS8900_RXTX_DATA0);
  RxEthnetFrameBuffer[iHeadPH++] = SwapByte(CS8900_RXTX_DATA0);
  
	ICMPTypeAndCode = *(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+ICMP_HEADER_START);
	if ((ICMPTypeAndCode>>8) == ICMP_ECHO)
		Prepare_ICMP_Answer();
}

void Prepare_ICMP_Answer(void)
{
	unsigned short iTemp,ICMPDataCount;
	unsigned short CalcCkSum;

	ICMPDataCount = RecdIpFrameLenth - IP_HEADER_SIZE - ICMP_HEADER_SIZE;
//Ethnet
	*(TxEthnetFrameBuffer+ETH_HEADER_START+0) = RecdFrameMAC[0];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+1) = RecdFrameMAC[1];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+2) = RecdFrameMAC[2];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+3) = MyMAC[0];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+4) = MyMAC[1];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+5) = MyMAC[2];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+6) = Frame_IP;
//IP
	*(TxEthnetFrameBuffer+IP_HEADER_START+0) = Ip_Edition;
	*(TxEthnetFrameBuffer+IP_HEADER_START+1) = RecdIpFrameLenth*2;
	*(TxEthnetFrameBuffer+IP_HEADER_START+2) = 0;
	*(TxEthnetFrameBuffer+IP_HEADER_START+3) = 0;
	*(TxEthnetFrameBuffer+IP_HEADER_START+4) = (DEFUALT_TTL<<8)|PROTOCOL_ICMP;
	*(TxEthnetFrameBuffer+IP_HEADER_START+5) = 0;
	*(TxEthnetFrameBuffer+IP_HEADER_START+6) = MyIP[0];
	*(TxEthnetFrameBuffer+IP_HEADER_START+7) = MyIP[1];
	*(TxEthnetFrameBuffer+IP_HEADER_START+8) = RecdFrameIP[0];
	*(TxEthnetFrameBuffer+IP_HEADER_START+9) = RecdFrameIP[1];

	CalcCkSum = CalcCheckSum((TxEthnetFrameBuffer+IP_HEADER_START),IP_HEADER_SIZE,0,0);
	if(!CalcCkSum)
		CalcCkSum = 0xFFFF;
	*(TxEthnetFrameBuffer+IP_HEADER_START+5) = CalcCkSum;
//ICMP    
	*(TxEthnetFrameBuffer+ICMP_HEADER_START+0) = ICMP_ECHO_REPLY << 8;
	*(TxEthnetFrameBuffer+ICMP_HEADER_START+1) = 0;
	for (iTemp=0;iTemp < ICMPDataCount;iTemp++)
		*(TxEthnetFrameBuffer+ICMP_HEADER_START+2+iTemp) = SwapByte(CS8900_RXTX_DATA0);
	CalcCkSum=CalcCheckSum((TxEthnetFrameBuffer+ICMP_HEADER_START),(ICMPDataCount+ICMP_HEADER_SIZE),0,0);
	if(!CalcCkSum)
		CalcCkSum = 0xFFFF;
	*(TxEthnetFrameBuffer+ICMP_HEADER_START+1) = CalcCkSum;
	TxFrameSize = ETH_HEADER_SIZE+IP_HEADER_SIZE+ICMP_HEADER_SIZE+ICMPDataCount;
	SendFrame(TxFrameSize);
}

unsigned short Process_TCP_Frame(void)  
{
	unsigned short length,iTemp;
	unsigned short iHeadPH;
	
	length = *(RxEthnetFrameBuffer+IP_HEADER_START+CS8900_HEADER_SIZE+1);
	if (length > (MY_MAX_SEG_SIZE+40))
	{
		return 0;
	}
	
	iHeadPH = CS8900_HEADER_SIZE + ETH_HEADER_SIZE + IP_HEADER_SIZE;
	for (iTemp=0;iTemp < TCP_HEADER_SIZE;iTemp++)
		RxEthnetFrameBuffer[iHeadPH++] = SwapByte(CS8900_RXTX_DATA0);
	length = (*(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+6) >> 12) & 0x0f;
	length *= 2;
	for (iTemp=TCP_HEADER_SIZE;iTemp < length;iTemp++)
		RxEthnetFrameBuffer[iHeadPH++] = SwapByte(CS8900_RXTX_DATA0);
	//（进入的字节转换）
	
	
	if (*(RxEthnetFrameBuffer+TCP_HEADER_START+CS8900_HEADER_SIZE+1) == MY_TCP_PORT)
	{
		if (TcpStatus.State >= TCP_STATE_SYN_RCVD)//已经建立的连接
		{
			if (TcpStatus.Dest_Ip[0] == RecdFrameIP[0])
			if (TcpStatus.Dest_Ip[1] == RecdFrameIP[1])
			if (*(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START) == TcpStatus.Dest_Port)
			{
				switch (TcpStatus.State)
				{
					case 1:
						Tcp_SYN_Rec();
						break;
					case 3:
						Tcp_Established();//状态转换进入接受状态走这里
						break;
					case 8:
						Tcp_Last_Ack();//关闭
						break;
				}
				return (2);
			}
		}
		else
		{
			if(TcpStatus.State == TCP_STATE_LISTEN)
			{
				iHeadPH = (iHeadPH - 2) * 2;
				iTemp = *(RxEthnetFrameBuffer+1);
				while (iHeadPH < iTemp)
				{
					SwapByte(CS8900_RXTX_DATA0);
					iHeadPH += 2;
				}
				TcpStatus.Dest_Ip[0] = RecdFrameIP[0];
				TcpStatus.Dest_Ip[1] = RecdFrameIP[1];
				Tcp_Listen();
				return (2);
			}
		}
		SendRstFrame(); 
	}
	return (1);
}
//建立TCP连接
unsigned short Tcp_Established(void) 
{
	unsigned short i;
	unsigned short temp_data,temp_control;
	unsigned long temp_AckNum,temp_SeqNum;
	unsigned short CalcCkSum;
	unsigned char cHeadstring[10];
	unsigned short ResultCode=0xffff;
	unsigned short com;
	unsigned int	iBufLen;
	static unsigned int iRecDataLen=0;
	unsigned short *sTemp;
	PrintSetPara PrintPara;
	
	temp_control = *(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+6);
	if(temp_control&(TCP_CODE_RST+TCP_CODE_SYN))
	{
		Delete_Socket();
		return (0);
	}
	if(temp_control&TCP_CODE_FIN)
	{
		TcpStatus.RecPassSeq = Word2ToLong(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+2);///
		TcpStatus.RecPassAck = Word2ToLong(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+4);
		TcpStatus.SenPassSeq = TcpStatus.RecPassAck;
		TcpStatus.SenPassAck = TcpStatus.RecPassSeq+1;
		TcpStatus.SenFutureSeq = TcpStatus.SenPassSeq;
		TcpStatus.RecFutureAck = TcpStatus.SenPassSeq+1;
		
		TcpHeadHandle(0x5000+TCP_CODE_ACK);
		/***************CLOSING WAIT*****************/	
		Delay(10);
		TcpHeadHandle(0x5000+TCP_CODE_FIN+TCP_CODE_ACK);//headlength 0x20,
		TcpStatus.State = TCP_STATE_LAST_ACK;
		return(0);
	}
	temp_data = ((*(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+6))>>12)&0x0f;
	temp_data = temp_data * 4;	//IP首部占20字节
	temp_data = *(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+IP_HEADER_START+1)-temp_data-IP_HEADER_SIZE*2;//数据报总长度减去TCP和IP首部长度,得到数据长度
	temp_SeqNum = Word2ToLong(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+2);
	temp_AckNum = Word2ToLong(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+4);
	
	if ((temp_data > 0) && (temp_data <= TcpStatus.Snd_Window))///
	{
		if (temp_SeqNum == TcpStatus.SenPassAck)//??
		{
			TcpStatus.SenPassAck=TcpStatus.SenPassAck+temp_data;
			if (temp_data < 8)///
			{
				for (i=0;i < temp_data;)//大小端
				{
					CalcCkSum = SwapByte(CS8900_RXTX_DATA0);
					cHeadstring[i] = (unsigned char)(CalcCkSum >> 8);
					cHeadstring[i+1] = (unsigned char)CalcCkSum;
					i += 2;
				}
				if (iRecDataLen > (unsigned int)temp_data)/////写数据
				{
//					Printf("A* iRecDataLen[%d] > tempdata[%d]\n",iRecDataLen, temp_data);
					if (gl_cCutFileSendFlag == 0)
						from_ethnet(cHeadstring,(unsigned int)temp_data);
					else if (gl_cCutFileSendFlag == 0x01)
						from_ethnet_Cut(cHeadstring,(unsigned int)temp_data);
					//from_ethnet(cHeadstring,(unsigned int)temp_data);
					iRecDataLen -= (unsigned int)temp_data;
					cHeadstring[0] = 0;
				}
				else									///////写数据
				{
//					Printf("a* iRecDataLen[%d] < tempdata[%d]\n",iRecDataLen, temp_data);
					temp_data = (unsigned short)iRecDataLen;
					if (gl_cCutFileSendFlag == 0)
						from_ethnet(cHeadstring,(unsigned int)temp_data);
					
					else if (gl_cCutFileSendFlag == 0x01)
						from_ethnet_Cut(cHeadstring,(unsigned int)temp_data);
					//from_ethnet(cHeadstring,(unsigned int)temp_data);
					iRecDataLen = 0;
					cHeadstring[0] = 3;
				}
			}
			else////命令字
			{
				for (i=0;i < 4;i++)
				{
					CalcCkSum = SwapByte(CS8900_RXTX_DATA0);
					cHeadstring[i*2] = (unsigned char)(CalcCkSum >> 8);
					cHeadstring[i*2+1] = (unsigned char)CalcCkSum;
				}
				if (memcmp(cHeadstring,"Buf_Long",8) == 0)	//查询缓冲区长度
				{
					if (gl_cPrintStatus == 0x20)
					{
						for (i=0;i < 3;i++)
						{
							CalcCkSum = SwapByte(CS8900_RXTX_DATA0);
							cHeadstring[i*2] = (unsigned char)(CalcCkSum >> 8);
							cHeadstring[i*2+1] = (unsigned char)CalcCkSum;
						}
						if (memcmp(cHeadstring,"_Start",6) == 0)
							gl_cPrintStatus = 0;
					}
					cHeadstring[0] = 1;
					iBufLen = CanRevChar();
				}
				else if (memcmp(cHeadstring,"Snd_Long",8) == 0)	//发送数据包长度
				{
					cHeadstring[0] = 4;
					CalcCkSum = SwapByte(CS8900_RXTX_DATA0);
					iRecDataLen = (unsigned int)(CalcCkSum << 16) + SwapByte(CS8900_RXTX_DATA0);
				}
				else if (memcmp(cHeadstring,"Msg_Read",8) == 0)//读参数
				{
					cHeadstring[0] = 5;
				}
				else if (memcmp(cHeadstring,"Msg_Comm",8) == 0)//命令
				{
					cHeadstring[0] = 2;
					com = SwapByte(CS8900_RXTX_DATA0);
					ResultCode = DealEthnetCom(com);
				}
				else
				{
					if (iRecDataLen > (unsigned int)temp_data)
					{
						//Printf("B* iRecDataLen[%d] > tempdata[%d]\n",iRecDataLen, temp_data);
						if (gl_cCutFileSendFlag == 0)
							from_ethnet(cHeadstring,(unsigned int)temp_data);
						else if (gl_cCutFileSendFlag == 0x01)
							from_ethnet_Cut(cHeadstring,(unsigned int)temp_data);
						//from_ethnet(cHeadstring,(unsigned int)temp_data);
						iRecDataLen -= (unsigned int)temp_data;
						cHeadstring[0] = 0;
					}
					else
					{
						//Printf("b* iRecDataLen[%d] < tempdata[%d]\n",iRecDataLen, temp_data);
						temp_data = (unsigned short)iRecDataLen;
						if (gl_cCutFileSendFlag == 0)
							from_ethnet(cHeadstring,(unsigned int)temp_data);
						else if (gl_cCutFileSendFlag == 0x01)
							from_ethnet_Cut(cHeadstring,(unsigned int)temp_data);
						//from_ethnet(cHeadstring,(unsigned int)temp_data);
						cHeadstring[0] = 3;
						if (iRecDataLen < (unsigned int)temp_data)
						{
							if (gl_cLanguageSelect == 0) {
								strcpy(gl_cLcdDispBuf[0],"  通讯故障");
							}
							else {
								strcpy(gl_cLcdDispBuf[0],"Communict Fault");
							}
							//strcpy(gl_cLcdDispBuf[0],"  通讯故障");
							strcpy(gl_cLcdDispBuf[1]," ");
							lcd_refresh();
							while(1);
						}
						else
							iRecDataLen = 0;
					}
				}
			}
		}
		else
			return (1);
		if(temp_control&TCP_CODE_ACK)
		{   //表示这是一个3次握手的确认
			if ((TcpStatus.SenPassSeq <= temp_AckNum)
			&& (temp_AckNum <= TcpStatus.RecFutureAck))
			{
				TcpStatus.RecPassAck = temp_AckNum;//确认
				
				//if(Resend_Buff.TcpFrame.SeqNum<TCP1024.Sent_UnAck);
			}
			TcpStatus.RecPassSeq = temp_SeqNum;
			TcpStatus.SenPassSeq = temp_AckNum;
			TcpStatus.SenFutureSeq = TcpStatus.SenFutureSeq;
			TcpStatus.RecFutureAck = TcpStatus.SenPassSeq;
			
			*(TxEthnetFrameBuffer+ETH_HEADER_START+0) = RecdFrameMAC[0];
			*(TxEthnetFrameBuffer+ETH_HEADER_START+1) = RecdFrameMAC[1];
			*(TxEthnetFrameBuffer+ETH_HEADER_START+2) = RecdFrameMAC[2];
			*(TxEthnetFrameBuffer+ETH_HEADER_START+3) = MyMAC[0];
			*(TxEthnetFrameBuffer+ETH_HEADER_START+4) = MyMAC[1];
			*(TxEthnetFrameBuffer+ETH_HEADER_START+5) = MyMAC[2];
			*(TxEthnetFrameBuffer+ETH_HEADER_START+6) = Frame_IP;
//IP
			*(TxEthnetFrameBuffer+IP_HEADER_START+0) = Ip_Edition;
			if (cHeadstring[0] == 0)
				*(TxEthnetFrameBuffer+IP_HEADER_START+1) = (IP_HEADER_SIZE+TCP_HEADER_SIZE)*2;
			else if (cHeadstring[0] == 5)
			{
				com = (sizeof(PrintPara) + 1) / 2;
				*(TxEthnetFrameBuffer+IP_HEADER_START+1) = (IP_HEADER_SIZE+TCP_HEADER_SIZE+11+com)*2;
			}
			else
				*(TxEthnetFrameBuffer+IP_HEADER_START+1) = (IP_HEADER_SIZE+TCP_HEADER_SIZE+4)*2;
			*(TxEthnetFrameBuffer+IP_HEADER_START+2) = TcpStatus.FrameIndex;
			TcpStatus.FrameIndex++;
			*(TxEthnetFrameBuffer+IP_HEADER_START+3) = 0x0000;
			*(TxEthnetFrameBuffer+IP_HEADER_START+4) = (DEFUALT_TTL<<8)|PROTOCOL_TCP;
			*(TxEthnetFrameBuffer+IP_HEADER_START+5) = 0;
			*(TxEthnetFrameBuffer+IP_HEADER_START+6) = MyIP[0];
			*(TxEthnetFrameBuffer+IP_HEADER_START+7) = MyIP[1];
			*(TxEthnetFrameBuffer+IP_HEADER_START+8) = TcpStatus.Dest_Ip[0];
			*(TxEthnetFrameBuffer+IP_HEADER_START+9) = TcpStatus.Dest_Ip[1];
			CalcCkSum = CalcCheckSum((TxEthnetFrameBuffer+IP_HEADER_START),IP_HEADER_SIZE,0,0);
			if(!CalcCkSum)
				CalcCkSum = 0xFFFF;
			*(TxEthnetFrameBuffer+IP_HEADER_START+5) = CalcCkSum;		
//TCP		
			*(TxEthnetFrameBuffer+TCP_HEADER_START+0) = MY_TCP_PORT;
			*(TxEthnetFrameBuffer+TCP_HEADER_START+1) = TcpStatus.Dest_Port;
			*(TxEthnetFrameBuffer+TCP_HEADER_START+2) = (unsigned short)(TcpStatus.SenPassSeq>>16);
			*(TxEthnetFrameBuffer+TCP_HEADER_START+3) = (unsigned short)TcpStatus.SenPassSeq;
			*(TxEthnetFrameBuffer+TCP_HEADER_START+4) = (unsigned short)(TcpStatus.SenPassAck>>16);
			*(TxEthnetFrameBuffer+TCP_HEADER_START+5) = (unsigned short)TcpStatus.SenPassAck;
			*(TxEthnetFrameBuffer+TCP_HEADER_START+6) = 0x5010;
			*(TxEthnetFrameBuffer+TCP_HEADER_START+7) = TcpStatus.Snd_Window;
			*(TxEthnetFrameBuffer+TCP_HEADER_START+8) = 0;
			*(TxEthnetFrameBuffer+TCP_HEADER_START+9) = 0;
			
			if (cHeadstring[0] == 1)
			{
				cHeadstring[0] = 4;
				if (gl_cFileReceive == 0)
				{
					*(TxEthnetFrameBuffer+TCP_HEADER_START+10) = 0x456e;//End
					*(TxEthnetFrameBuffer+TCP_HEADER_START+11) = 0x645F;
				}
				else
				{
					*(TxEthnetFrameBuffer+TCP_HEADER_START+10) = 0x4D73;//Msg_
					*(TxEthnetFrameBuffer+TCP_HEADER_START+11) = 0x675F;
				}
				*(TxEthnetFrameBuffer+TCP_HEADER_START+12) = (unsigned short)(iBufLen >> 16);
				*(TxEthnetFrameBuffer+TCP_HEADER_START+13) = (unsigned short)iBufLen;
			}
			else if (cHeadstring[0] == 2)
			{//命令帧应答
				cHeadstring[0] = 4;
				*(TxEthnetFrameBuffer+TCP_HEADER_START+10) = 0x4D73;//Msg_
				*(TxEthnetFrameBuffer+TCP_HEADER_START+11) = 0x675F;
				*(TxEthnetFrameBuffer+TCP_HEADER_START+12) = com;
				*(TxEthnetFrameBuffer+TCP_HEADER_START+13) = ResultCode;
			}
			else if (cHeadstring[0] == 3)
			{
				cHeadstring[0] = 4;
				*(TxEthnetFrameBuffer+TCP_HEADER_START+10) = 0x4D73;//Msg_
				*(TxEthnetFrameBuffer+TCP_HEADER_START+11) = 0x675F;
				if (gl_cFileReceive == 0)
				{
					*(TxEthnetFrameBuffer+TCP_HEADER_START+12) = 0x456e;
					*(TxEthnetFrameBuffer+TCP_HEADER_START+13) = 0x6420;//End
				}
				else
				{
					*(TxEthnetFrameBuffer+TCP_HEADER_START+12) = 0x4f6B;
					*(TxEthnetFrameBuffer+TCP_HEADER_START+13) = 0x5047;//OkPG
				}
			}
			else if (cHeadstring[0] == 4)
			{
				cHeadstring[0] = 4;
				*(TxEthnetFrameBuffer+TCP_HEADER_START+10) = 0x4D73;//Msg_
				*(TxEthnetFrameBuffer+TCP_HEADER_START+11) = 0x675F;
				*(TxEthnetFrameBuffer+TCP_HEADER_START+12) = 0x4f6B;//OkS
				*(TxEthnetFrameBuffer+TCP_HEADER_START+13) = 0x5320;
			}
			else if (cHeadstring[0] == 5)
			{
				*(TxEthnetFrameBuffer+TCP_HEADER_START+10) = 0x4D73;//Msg_
				*(TxEthnetFrameBuffer+TCP_HEADER_START+11) = 0x675F;
				*(TxEthnetFrameBuffer+TCP_HEADER_START+12) = 0x5061;//OkS
				*(TxEthnetFrameBuffer+TCP_HEADER_START+13) = 0x7261;
				*(TxEthnetFrameBuffer+TCP_HEADER_START+14) = 0x4E65;//New Version3.2 20100225
				*(TxEthnetFrameBuffer+TCP_HEADER_START+15) = 0x7720;
				*(TxEthnetFrameBuffer+TCP_HEADER_START+16) = 0x5665;
				*(TxEthnetFrameBuffer+TCP_HEADER_START+17) = 0x7273;
				*(TxEthnetFrameBuffer+TCP_HEADER_START+18) = 0x696F;
				*(TxEthnetFrameBuffer+TCP_HEADER_START+19) = 0x6E33;
				*(TxEthnetFrameBuffer+TCP_HEADER_START+20) = 0x2E33;//为解决兼容性问题20100413
				//PrintPara.cChinaEn = 'C';
				PrintPara.cSprinklerCount = gl_cSprinklerCount;
				PrintPara.cSprinklerNo = gl_cSprinklerNo;
				PrintPara.cBidirectional = gl_cBidirectional;
				PrintPara.cLineWidth = gl_cLineWidth;
				PrintPara.cMultiDotLine12 = gl_cMultiDotLine12;
				PrintPara.cPenNo = gl_cPenNo;
				PrintPara.cCutNo = gl_cCutNo;
				PrintPara.cPrintCutSelect = gl_cPrintCutSelect;
				//PrintPara.cMultiDotLine23 = gl_cMultiDotLine23;
				//PrintPara.cMultiDotLine34 = gl_cMultiDotLine34;
				//PrintPara.cNull1 = 0;
				PrintPara.sPlotType = gl_sPlotType;
				PrintPara.sNull1= 0;
				//PrintPara.sNull1[0]=0;PrintPara.sNull1[1]=0;PrintPara.sNull1[2]=0;
				//PrintPara.iOriginalPos = gl_iOriginalPos;
				PrintPara.iYDotSpace12 = gl_iYDotSpace12 - SprinklerSpace;
				PrintPara.iYDotSpace23 = gl_iYDotSpace23 - SprinklerSpace;
				PrintPara.iYDotSpace34 = gl_iYDotSpace34 - SprinklerSpace;
				PrintPara.iSNUll = 0xffffffff;
				PrintPara.dErrorX = gl_dErrorX;
				PrintPara.dErrorY = gl_dErrorY;
				PrintPara.dPictureErrorX = gl_dPictureErrorX;
				PrintPara.dPictureErrorY = gl_dPictureErrorY;
				sTemp = (unsigned short *)&PrintPara;
				com = (sizeof(PrintPara) + 1) / 2;
				cHeadstring[0] = 11 + com;;
				for (i=0;i<com;i++)
				{
					*(TxEthnetFrameBuffer+TCP_HEADER_START+21+i) = *sTemp;
					sTemp++;
				}		
			}
			
			CalcCkSum = CalcCheckSum((TxEthnetFrameBuffer+TCP_HEADER_START),TCP_HEADER_SIZE+cHeadstring[0],1,0);
			if (!CalcCkSum)
				CalcCkSum = 0xFFFF;
			*(TxEthnetFrameBuffer+TCP_HEADER_START+8) = CalcCkSum;
            
			TxFrameSize = ETH_HEADER_SIZE+IP_HEADER_SIZE+TCP_HEADER_SIZE+cHeadstring[0];
			SendFrame(TxFrameSize);
		}
		return (1);
	}
//	else if (((temp_control&TCP_CODE_ACK) == TCP_CODE_ACK) && (temp_data > 0)) 
//	{
//		TcpStatus.RecPassAck = temp_AckNum;
//		TcpStatus.RecPassSeq = temp_SeqNum;
//		TcpStatus.SenPassSeq = temp_AckNum;
//		TcpStatus.SenFutureSeq = TcpStatus.SenFutureSeq;
//		TcpStatus.RecFutureAck = TcpStatus.SenPassSeq;
//		TcpStatus.SenPassAck = TcpStatus.SenPassAck+temp_data;
//		TcpHeadHandle(0x5010);//headlength 0x20 , ack
//	}
	else if (((temp_control&TCP_CODE_ACK) == TCP_CODE_ACK) && (temp_data == 0))
	{//表示这是一个3次握手的确认
		if ((TcpStatus.SenPassSeq <= temp_AckNum)
		&& (temp_AckNum <= TcpStatus.RecFutureAck))
		{
			TcpStatus.RecPassAck = temp_AckNum;//确认
			return (2);
		}
		return (3);
	}
	return (4);
}
/**********************************************************************
**函数原型：  void        Tcp_SYN_Rec( )
**入口参数:		无
**出口参数:		无
**返 回 值:无
**功能说明:		从tcp_listen接收到syn后,即可进入该状态
**        :		该状态可接收tcp_syn和TCP_CODE_ACK或tcp_rst
**************************************************************/
void Tcp_SYN_Rec(void) 
{
	unsigned short temp_control;
	unsigned long temp_AckNum;
	
	temp_control = *(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+6);
	
	temp_AckNum = Word2ToLong(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+4);
	if (temp_control&TCP_CODE_RST)		//reset//处理reset,对方不接受请求
	{
		Delete_Socket();
	}
	else if (temp_control&(TCP_CODE_SYN))//收到同步信号
	{
		TcpStatus.RecPassAck = temp_AckNum;//确认
		TcpStatus.RecPassSeq = Word2ToLong(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+2);//对方的初始化顺序号
		TcpStatus.SenPassSeq = TcpStatus.RecPassAck;
		TcpStatus.SenPassAck = TcpStatus.RecPassSeq;//我的顺序号,用于发送
		
		SendRstFrame();
		Delete_Socket();
	}
	else if (temp_control&TCP_CODE_ACK)	//表示这是一个3次握手的确认表明连接建立
	{
 		if( temp_AckNum == TcpStatus.RecFutureAck)
		{
			TcpStatus.RecPassAck = temp_AckNum;//确认
			TcpStatus.RecPassSeq = Word2ToLong(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+2);//确认
			TcpStatus.State = TCP_STATE_ESTABLISHED;		//tcp_syn_established;
 	  }
  }
}

void SendRstFrame(void)
{
	unsigned long temp_AckNum;
	unsigned short CalcCkSum;
	
	*(TxEthnetFrameBuffer+ETH_HEADER_START+0) = RecdFrameMAC[0];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+1) = RecdFrameMAC[1];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+2) = RecdFrameMAC[2];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+3) = MyMAC[0];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+4) = MyMAC[1];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+5) = MyMAC[2];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+6) = Frame_IP;
//IP
	*(TxEthnetFrameBuffer+IP_HEADER_START+0) = Ip_Edition;
	*(TxEthnetFrameBuffer+IP_HEADER_START+1) = (IP_HEADER_SIZE+TCP_HEADER_SIZE)*2;
	*(TxEthnetFrameBuffer+IP_HEADER_START+2) = TcpStatus.FrameIndex;
	TcpStatus.FrameIndex++;
	*(TxEthnetFrameBuffer+IP_HEADER_START+3) = 0x0000;
	*(TxEthnetFrameBuffer+IP_HEADER_START+4) = (DEFUALT_TTL<<8)|PROTOCOL_TCP;
	*(TxEthnetFrameBuffer+IP_HEADER_START+5) = 0;
	
	*(TxEthnetFrameBuffer+IP_HEADER_START+6) = MyIP[0];
	*(TxEthnetFrameBuffer+IP_HEADER_START+7) = MyIP[1];
	*(TxEthnetFrameBuffer+IP_HEADER_START+8) = RecdFrameIP[0];
	*(TxEthnetFrameBuffer+IP_HEADER_START+9) = RecdFrameIP[1];
	CalcCkSum = CalcCheckSum((TxEthnetFrameBuffer+IP_HEADER_START),IP_HEADER_SIZE,0,0);
	if(!CalcCkSum)
		CalcCkSum = 0xFFFF;
	*(TxEthnetFrameBuffer+IP_HEADER_START+5) = CalcCkSum;		
//TCP	
	*(TxEthnetFrameBuffer+TCP_HEADER_START+0) = MY_TCP_PORT;
	*(TxEthnetFrameBuffer+TCP_HEADER_START+1) = *(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START);
	*(TxEthnetFrameBuffer+TCP_HEADER_START+2) = 0;
	*(TxEthnetFrameBuffer+TCP_HEADER_START+3) = 0;
	temp_AckNum = Word2ToLong(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+4);
	temp_AckNum++;
	*(TxEthnetFrameBuffer+TCP_HEADER_START+4) = (unsigned short)(temp_AckNum>>16);
	*(TxEthnetFrameBuffer+TCP_HEADER_START+5) = (unsigned short)temp_AckNum;
	*(TxEthnetFrameBuffer+TCP_HEADER_START+6) = 0x5000+TCP_CODE_ACK+TCP_CODE_RST;
	*(TxEthnetFrameBuffer+TCP_HEADER_START+7) = 0;
	*(TxEthnetFrameBuffer+TCP_HEADER_START+8) = 0;
	*(TxEthnetFrameBuffer+TCP_HEADER_START+9) = 0;
	CalcCkSum=CalcCheckSum((TxEthnetFrameBuffer+TCP_HEADER_START),(TCP_HEADER_SIZE),1,0);
	if(!CalcCkSum)
		CalcCkSum = 0xFFFF;
	*(TxEthnetFrameBuffer+TCP_HEADER_START+8) = CalcCkSum;
	TxFrameSize = ETH_HEADER_SIZE+IP_HEADER_SIZE+TCP_HEADER_SIZE;
	SendFrame(TxFrameSize);
}

void TcpHeadHandle(unsigned short Tcp_headlength_Control)
{
	unsigned short CalcCkSum;
	unsigned short Tcp_HeadLength;
	
	Tcp_HeadLength = (Tcp_headlength_Control >> 12) & 0x0f;
	Tcp_HeadLength *= 2;
	*(TxEthnetFrameBuffer+ETH_HEADER_START+0) = RecdFrameMAC[0];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+1) = RecdFrameMAC[1];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+2) = RecdFrameMAC[2];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+3) = MyMAC[0];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+4) = MyMAC[1];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+5) = MyMAC[2];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+6) = Frame_IP;
//IP
	*(TxEthnetFrameBuffer+IP_HEADER_START+0) = Ip_Edition;
	*(TxEthnetFrameBuffer+IP_HEADER_START+1) = (IP_HEADER_SIZE+Tcp_HeadLength)*2;
	*(TxEthnetFrameBuffer+IP_HEADER_START+2) = TcpStatus.FrameIndex;
	TcpStatus.FrameIndex++;
	*(TxEthnetFrameBuffer+IP_HEADER_START+3) = 0x0000;
	*(TxEthnetFrameBuffer+IP_HEADER_START+4) = (DEFUALT_TTL<<8)|PROTOCOL_TCP;
	*(TxEthnetFrameBuffer+IP_HEADER_START+5) = 0;
	*(TxEthnetFrameBuffer+IP_HEADER_START+6) = MyIP[0];
	*(TxEthnetFrameBuffer+IP_HEADER_START+7) = MyIP[1];
	*(TxEthnetFrameBuffer+IP_HEADER_START+8) = TcpStatus.Dest_Ip[0];
	*(TxEthnetFrameBuffer+IP_HEADER_START+9) = TcpStatus.Dest_Ip[1];
	CalcCkSum = CalcCheckSum((TxEthnetFrameBuffer+IP_HEADER_START),IP_HEADER_SIZE,0,0);
	if(!CalcCkSum)
		CalcCkSum = 0xFFFF;
	*(TxEthnetFrameBuffer+IP_HEADER_START+5) = CalcCkSum;		
//TCP		
	*(TxEthnetFrameBuffer+TCP_HEADER_START+0) = MY_TCP_PORT;
	*(TxEthnetFrameBuffer+TCP_HEADER_START+1) = TcpStatus.Dest_Port;
	*(TxEthnetFrameBuffer+TCP_HEADER_START+2) = (unsigned short)(TcpStatus.SenPassSeq>>16);
	*(TxEthnetFrameBuffer+TCP_HEADER_START+3) = (unsigned short)TcpStatus.SenPassSeq;
	*(TxEthnetFrameBuffer+TCP_HEADER_START+4) = (unsigned short)(TcpStatus.SenPassAck>>16);
	*(TxEthnetFrameBuffer+TCP_HEADER_START+5) = (unsigned short)TcpStatus.SenPassAck;
	*(TxEthnetFrameBuffer+TCP_HEADER_START+6) = Tcp_headlength_Control;
	*(TxEthnetFrameBuffer+TCP_HEADER_START+7) = TcpStatus.Snd_Window;
	*(TxEthnetFrameBuffer+TCP_HEADER_START+8) = 0;
	*(TxEthnetFrameBuffer+TCP_HEADER_START+9) = 0;
	if (Tcp_HeadLength > TCP_HEADER_SIZE)
	{
		*(TxEthnetFrameBuffer+TCP_HEADER_START+10) = 0x0204;
		*(TxEthnetFrameBuffer+TCP_HEADER_START+11) = TcpStatus.Snd_Window;
		*(TxEthnetFrameBuffer+TCP_HEADER_START+12) = 0x0101;
		*(TxEthnetFrameBuffer+TCP_HEADER_START+13) = 0x0101;
	}
    	
	CalcCkSum = CalcCheckSum((TxEthnetFrameBuffer+TCP_HEADER_START),Tcp_HeadLength,1,0);
	if(!CalcCkSum)
		CalcCkSum = 0xFFFF;
	*(TxEthnetFrameBuffer+TCP_HEADER_START+8) = CalcCkSum;
	TxFrameSize = ETH_HEADER_SIZE+IP_HEADER_SIZE+Tcp_HeadLength;
	SendFrame(TxFrameSize);
}

void Tcp_Listen(void) 
{
	static unsigned long initalseq = 32451;
	unsigned short temp_control;
	
	temp_control = *(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+6);
	if (temp_control & TCP_CODE_SYN)
	{
		TcpStatus.State = TCP_STATE_SYN_RCVD;
		TcpStatus.Dest_Port = *(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START);
		TcpStatus.RecPassSeq = Word2ToLong(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+2);
		TcpStatus.RecPassAck = 0;
		TcpStatus.SenPassSeq = initalseq;
		TcpStatus.SenPassAck = TcpStatus.RecPassSeq+1;
		TcpStatus.RecFutureAck = initalseq+1;
		TcpStatus.SenFutureSeq = initalseq+1;
		TcpStatus.Rcv_Window = *(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+7);
		if ((*(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+6)&0xf000) > 0x5000)
		if (*(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+10) == 0x0204)		//0204为最大segment选项
		{
			TcpStatus.Dest_Max_Seg_Size = *(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+11);
		}
		TcpHeadHandle(0x7000+TCP_CODE_ACK+TCP_CODE_SYN);
	}
}
/**********************************************************************
**函数原型：    void        Tcp_Last_Ack( )
**入口参数:		无
**出口参数:		无
**返 回 值：	无
**说    明：	接收到TCP_RST,TCP_SYN或TCP_CODE_ACK报文时,关闭连接
************************************************************************/
void Tcp_Last_Ack(void) 
{
	unsigned short temp_control;
    
	temp_control = *(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+6);
	if (temp_control&(TCP_CODE_RST+TCP_CODE_SYN+TCP_CODE_ACK))
		Delete_Socket();
}



//轮询CS8900是否有接收数据
void PollRecNetData(void)
{
	unsigned short iEvent;
	
	iEvent = CS8900_ISQ;//ReadNetDataLH(PORT_ISQ);
	while (iEvent != 0)
	{
		switch (iEvent & REG_NUM_MASK)
		{
			case REG_NUM_RX_EVENT:				//RxEvent(Register 4)
//				Printf("Receive Net  ][ gl_cOverCutOffPaper = %d \n",gl_cOverCutOffPaper);
				csReceiveEvent(iEvent);
				break;
			case REG_NUM_TX_EVENT:				//TxEvent(Register 8)
				csTransmitEvent();
			break;
			case REG_NUM_BUF_EVENT:				//BufEvent(Register C)
//				csBufferEvent(iEvent);
				break;
			case REG_NUM_RX_MISS:				//RxMISS(Register 10)
				break;
			case REG_NUM_TX_COL:				//TxCOL(Register 12)
				break;
			default:
				break;
		}
		iEvent = CS8900_ISQ;//ReadNetDataLH(PORT_ISQ);
	}
}

unsigned short SwapWord(unsigned short Data)
{
    return((Data>>8)|(Data<<8));
}

unsigned long Word2ToLong(unsigned short *temp_ptr)
{
	unsigned long lTemp;
	unsigned short iTemp;
	
	iTemp = *temp_ptr;
	lTemp = ((unsigned long)iTemp) << 16;
	temp_ptr++;
	iTemp = *temp_ptr;
	lTemp += (unsigned long)iTemp;
  return(lTemp);
}

void Process_UDP_Frame(void)
{
//	unsigned short UdpLenth;
//	unsigned short *add_ptr;
//    
//	add_ptr = RxEthnetFrameBuffer+RTL8019_HEADER_SIZE+ETH_HEADER_SIZE+IP_HEADER_SIZE+UDP_HEADER_SIZE;
//	CopyFrameFromBE(RTL8019_HEADER_SIZE+UDP_HEADER_START,4);
//	if(*(RxEthnetFrameBuffer+RTL8019_HEADER_SIZE+UDP_HEADER_START+1)==MY_UDP_PORT)
//	{
//		UdpLenth=*(RxEthnetFrameBuffer+RTL8019_HEADER_SIZE+UDP_HEADER_START+2)-UDP_HEADER_SIZE*2;
//		from_ethnet(add_ptr,UdpLenth);
//	}
}

/**********************************************************************
**函数原型：    void 		SendFrame(Uint16  length)
**入口参数:		uint length					:发送数据包的长度
**出口参数:		无
**返 回 值：	无              
**说    明：	发送数据包,以太网底层驱动程序,所有的数据发送都要通过该程序
************************************************************************/
void	SendFrame(unsigned short length)
{
	unsigned short iCount;
	
	CS8900_TX_CMD = 0x00C0;
	CS8900_TX_LENGTH = length*2;
	while ((csReadPacketPage(PKTPG_BUS_ST)&0x100) == 0);
	for (iCount=0;iCount < length;iCount++)
		CS8900_RXTX_DATA0 = SwapByte(*(TxEthnetFrameBuffer+iCount));
}
//extern lcd_clr(void);
//extern lcd_display(unsigned char row, const char * string);

//以太网命令处理
unsigned short DealEthnetCom(unsigned short com)
{
	unsigned short ResultCode;
	unsigned short sTemp;
	short sTemp1;
	unsigned char *cTemp;
	unsigned char cTemp1,cTemp2,cTemp3;
	int iTemp1,iTemp2,iTemp3;
	int iTemp;
	long ltemp;
	double dTemp;
	PrintSetPara PrintPara;
	int iYPlus, iXPlus;
	//char debugBuff[20];

	if (com >= 10)
	{
		if (gl_cFileReceive != 0)
		{//上一图形未打印完毕,不能设置此参数
			ResultCode = 3;
			return ResultCode;
		}
//		Printf("com %d\n" , com);
	}
//		Printf("com %d\n" , com);
	switch  (com)
	{
		case 1:	//文件开始传送命令
			//sTemp = SwapByte(CS8900_RXTX_DATA0);
			//iTemp1 = (sTemp << 16) + SwapByte(CS8900_RXTX_DATA0);

			if(cReviceFlag == 1)
			{
				ResultCode = 2;
				break;
			}
			
			if(gl_cCutFileSendFlag == 0x03)
			{
//				Printf("Cut Work\n");
				ResultCode = 2;
				break;
			}
			//*****************************************
			
			ltemp = SwapByte(CS8900_RXTX_DATA0);
			ltemp <<= 16;
			ltemp |= SwapByte(CS8900_RXTX_DATA0);
			/*if ((ltemp > gl_sMachineWidth) || (ltemp < 0))
			{
				Printf("gl_sMachineWidth:%d\n", gl_sMachineWidth);
				Printf("EMWidth:%d\n", ltemp);
				ResultCode = 3;
				break;
			}*/
			gl_iCurPictureWidth = ltemp;
//			Printf("iTemp2:%d,  gl_sMachineWidth:%d\n",iTemp1, gl_sMachineWidth);
			//*******************************************
			
			ltemp = SwapByte(CS8900_RXTX_DATA0);
			ltemp <<= 16;
			ltemp |= SwapByte(CS8900_RXTX_DATA0);
			/*if ((ltemp > gl_sMachineLenght) || (ltemp < 0))
			{
				Printf("gl_sMachineWidth:%d\n", gl_sMachineWidth);
				Printf("EMLenght:%d\n", ltemp);
				ResultCode = 3;
				break;
			}		*/	
			gl_iCurPictureLength = ltemp;
//			Printf("iTemp3:%d,	gl_sMachineLenght:%d\n",iTemp1, gl_sMachineLenght);
			//*******************************************
			
			// 判断当前位置是否可以切绘完麦架 20120822
			//gl_iYMaxEffectivePMM = gl_lMaxY - ((gl_iOriginalPos_Y+10) * Y_PPMM);
			//gl_iXMaxEffectivePMM = gl_lMaxX - ((gl_iOriginalPos_X+10) * X_PPMM);
			iYPlus = gl_iCurPictureWidth * Y_PPMM; //( 207 + gl_iCurPictureWidth * 8 + AdditionDot * 2 + OriginalSideDot + MotorSideDot) * DotPulseNum;
			iXPlus = gl_iCurPictureLength * X_PPMM; //( gl_iCurPictureLength * 8 + 50) * DotPulseNum;
			//Printf("IS PLT DATA FILE!!!\n");
			//Printf("EffectYMaxPmm: %d, EffectXMaxPmm: %d\n",gl_iYMaxEffectivePMM, gl_iXMaxEffectivePMM);
			//Printf("CurrYPmm: %d, CurrXPmm: %d\n", iYPlus, iXPlus);
			//Printf("CurrPltWith: %d, CurrPltLenght: %d\n", gl_iCurPictureWidth, gl_iCurPictureLength);
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
						//Printf("gl_iEthnetRecBufPE != gl_iEthnetRecBufPH\n");
						ResultCode = 2;
						break;
					}
					else if (gl_iPulseNumY < 0)
					{//喷头回原点
						gl_cFileReceive = 0xe1;
						//Printf("gl_iPulseNumY < 0\n");
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
					//Printf("(gl_cFileReceive & 0x60) != 0\n");
					break;
				}
			}

			gl_lCurrFileLenghtPluse = 0;
			// 20130104 自动开启气泵
			if((gl_cPumpSwitch&0x01) == 0x01)
			{
				OpenPump();
			}
			//*************20130807机头定位到切割位置
			
			//PrintReturnZero();
			//PrintHeClean(1);
			
			gl_iGraphWidth = gl_iCurPictureWidth;
			gl_cInvertBufStatus = 0;
			gl_cCurPrintBuf = 0;
			gl_cCountFlag = 0;
			gl_cRemainderBit = 16;
			ResultCode = 9;
			gl_cFileReceive = 0x01;
			CalVariableValue();
			gl_cCommunicationMode = 0;
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
			//Printf("PH[%d], PE[%d]\n",gl_iEthnetRecBufPH, gl_iEthnetRecBufPE);
				Printf("\n\n gl_cSWFStartClearFlag = %d \n", gl_cSWFStartClearFlag);
			if (gl_iAxisYPos <= gl_iOriginalPos)//gl_lWorkOrig_YYPluse
				{
					if(gl_cSWFStartClearFlag != 1)	//20141215 rwt
					{
						gl_cSWFStartClearFlag = 1;
					}
					
					ClearPrintHead(0);//ClearPrintHead(1);//2016-2-19
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
					Printf("net1 ------->>>>清洗喷头\n");
				}
					Printf("gl_iAxisYPos  = %d  gl_iOriginalPos = %d \n", gl_iAxisYPos ,gl_iOriginalPos );
					Printf("net2 ------->>>>没清洗喷头\n\n\n");
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
			break;
		case 2:	//文件传送结束命令
//			Printf("gl_sMachineWidth:%d\n", gl_sMachineWidth);
			if( gl_cCutFileSendFlag == 0x01)
			{//切割数据文件传送结束
				ResultCode = 9;
				gl_iEthnetRecBufPE = 0;
				gl_iEthnetRecBufPH = 0;
				gl_cCutFileSendFlag = 0;
				gl_cFileReceive = 0;
				gl_cInvertBufStatus = 0;
				gl_DeleteFileState = 0;
				gl_cCutWorkStatus = 1;
				Printf("Cut Send End\n");
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
			Printf("Plt Send End\n");
			break;

		case 3: 	//	切割文件传送命令
			//当前正在执行切割或者当前内存区域内有数据文件未执行，不能接收新的文件 20120711
//			Printf("gl_sMachineWidth:%d\n", gl_sMachineWidth);
			if((gl_cCutFileSendFlag == 0x03) || (gl_cCutWorkStatus==1))
			{
				//Printf("Work....\n");
				ResultCode = 2;
				break;
			}
			
			ltemp = SwapByte(CS8900_RXTX_DATA0);
			ltemp <<= 16;
			ltemp |= SwapByte(CS8900_RXTX_DATA0);
			if(ltemp > CutDataLength)
			{
//				Printf("EDLength:%d\n", ltemp);
				ResultCode = 3;
				break;
			}
			gl_lCurrCutFileLength = ltemp;
		////////////////20120822 校验麦架长宽是否可打 ///////////

			ltemp = SwapByte(CS8900_RXTX_DATA0);
			ltemp <<= 16;
			ltemp |= SwapByte(CS8900_RXTX_DATA0);
			/*if ((ltemp > gl_sMachineWidth) || (ltemp < 0))
			{
				Printf("EMWidth:%d\n", ltemp);
				ResultCode = 3;
				break;
			}*/
			gl_iCurPictureWidth = ltemp;
			//Printf("iTemp2:%d,  gl_sMachineWidth:%d\n",iTemp1, gl_sMachineWidth);

			ltemp = SwapByte(CS8900_RXTX_DATA0);
			ltemp <<= 16;
			ltemp |= SwapByte(CS8900_RXTX_DATA0);
			/*if ((ltemp > gl_sMachineLenght) || (ltemp < 0))
			{
				Printf("EMLenght:%d\n", ltemp);
				ResultCode = 3;
				break;
			}	*/
			gl_iCurPictureLength= ltemp;	
			//	Printf("iTemp3:%d,	gl_sMachineLenght:%d\n",iTemp1, gl_sMachineLenght);
			
			//gl_iYMaxEffectivePMM = gl_lMaxY - ((gl_iOriginalPos_Y+10) * Y_PPMM);
			//gl_iXMaxEffectivePMM = gl_lMaxX - ((gl_iOriginalPos_X+10) * X_PPMM);
			iYPlus = gl_iCurPictureWidth * Y_PPMM; 
			iXPlus = gl_iCurPictureLength * X_PPMM;
			//iYPlus = (gl_iCurPictureWidth * Y_PPMM) + ((SprinklerSpace + AdditionDot * 2 + OriginalSideDot + MotorSideDot) * DotPulseNum);
			//iXPlus = ( gl_iCurPictureLength * X_PPMM) + (50 * DotPulseNum);
			//Printf("IS CUT DATA FILE!!![%d]\n", gl_lCurrCutFileLength);
			//Printf("EffectYMaxPmm: %d, EffectXMaxPmm: %d\n",gl_iYMaxEffectivePMM, gl_iXMaxEffectivePMM);
			//Printf("CurrYPmm: %d, CurrXPmm: %d\n", gl_iCurPictureWidth, gl_iCurPictureLength);
			//Printf("CurrPltWith: %d, CurrPltLenght: %d\n", gl_iCurPictureWidth, gl_iCurPictureLength);
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
			//Printf("CFL:%d, PH:%d,PE:%d,PS:%d\n\n",gl_lCurrCutFileLength,gl_iEthnetRecBufPH, gl_iEthnetRecBufPE, gl_cPrintStatus);
			break;

		
		case 4:
			if(gl_cFileReceive != 0)
				gl_cFileReceive |= 0x10;	//删除当前打印文件
			ResultCode = 9;
			break;
		case 5:	//查询命令
			if (CanRevChar() != 0)
//			if (gl_cCanRevChar == 1)
				ResultCode = 9;
			else
				ResultCode = 2;
			break;
		case 6:	//图形误差设置命令
			cTemp = (unsigned char *)&gl_dPictureErrorX;
			for (iTemp=0;iTemp<4;iTemp++)
			{
				sTemp = SwapByte(CS8900_RXTX_DATA0);
				*cTemp = (unsigned char)(sTemp >> 8);
				cTemp++;
				*cTemp = (unsigned char)sTemp;
				cTemp++;
			}
			cTemp = (unsigned char *)&gl_dPictureErrorY;
			for (iTemp=0;iTemp<4;iTemp++)
			{
				sTemp = SwapByte(CS8900_RXTX_DATA0);
				*cTemp = (unsigned char)(sTemp >> 8);
				cTemp++;
				*cTemp = (unsigned char)sTemp;
				cTemp++;
			}
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
				ResultCode = 2;
			break;
		case 14:		//打印走纸误差修正图
			if (gl_cPrintStatus == 0x10)
				break;
			dTemp = gl_dErrorX;
			cTemp = (unsigned char *)&gl_dErrorX;
			for (iTemp=0;iTemp<4;iTemp++)
			{
				sTemp = SwapByte(CS8900_RXTX_DATA0);
				*cTemp = (unsigned char)(sTemp >> 8);
				cTemp++;
				*cTemp = (unsigned char)sTemp;
				cTemp++;
			}
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
			cTemp1 = gl_cMultiDotLine12;
			cTemp2 = gl_cMultiDotLine23;
			cTemp3 = gl_cMultiDotLine34;
			gl_cMultiDotLine12 = (unsigned char)SwapByte(CS8900_RXTX_DATA0);
			gl_cMultiDotLine23 = (unsigned char)SwapByte(CS8900_RXTX_DATA0);
			gl_cMultiDotLine34 = (unsigned char)SwapByte(CS8900_RXTX_DATA0);
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
			sTemp1 = SwapByte(CS8900_RXTX_DATA0);
			if ((sTemp1 < -MaxMalposedDot) && (sTemp1 > MaxMalposedDot))
			{
				ResultCode = 1;
				break;
			}
			gl_iYDotSpace12 = (unsigned int)(SprinklerSpace + sTemp1);
			iTemp2 = gl_iYDotSpace23;
			sTemp1 = SwapByte(CS8900_RXTX_DATA0);
			if ((sTemp1 < -MaxMalposedDot) && (sTemp1 > MaxMalposedDot))
			{
				gl_iYDotSpace12 = iTemp1;
				ResultCode = 1;
				break;
			}
			gl_iYDotSpace23 = (unsigned int)(SprinklerSpace + sTemp1);
			iTemp3 = gl_iYDotSpace34;
			sTemp1 = SwapByte(CS8900_RXTX_DATA0);
			if ((sTemp1 < -MaxMalposedDot) && (sTemp1 > MaxMalposedDot))
			{
				gl_iYDotSpace12 = iTemp1;
				gl_iYDotSpace23 = iTemp2;
				ResultCode = 1;
				break;
			}
			gl_iYDotSpace34 = (unsigned int)(SprinklerSpace + sTemp1);;
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
			cTemp = (unsigned char *)&dTemp;
			for (iTemp=0;iTemp<4;iTemp++)
			{
				sTemp = SwapByte(CS8900_RXTX_DATA0);
				*cTemp = (unsigned char)(sTemp >> 8);
				cTemp++;
				*cTemp = (unsigned char)sTemp;
				cTemp++;
			}
			iTemp1 = (unsigned int)((dTemp * Y_PPMM / DotPulseNum + 7.0) / 8.0);
			if (iTemp1 > gl_iMaxPictureWidth)
			{
				ResultCode = 1;
				break;
			}
			dTemp = gl_dErrorY;
			cTemp = (unsigned char *)&gl_dErrorY;
			for (iTemp=0;iTemp<4;iTemp++)
			{
				sTemp = SwapByte(CS8900_RXTX_DATA0);
				*cTemp = (unsigned char)(sTemp >> 8);
				cTemp++;
				*cTemp = (unsigned char)sTemp;
				cTemp++;
			}
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
//******NEW
			PrintPara.cBidirectional = (unsigned char)SwapByte(CS8900_RXTX_DATA0);
			//Printf(" -> %c \n", PrintPara.cBidirectional);
			PrintPara.cSprinklerCount = (unsigned char)SwapByte(CS8900_RXTX_DATA0);
			//Printf(" -> %c \n", PrintPara.cSprinklerCount);
			PrintPara.cSprinklerNo = (unsigned char)SwapByte(CS8900_RXTX_DATA0);
			//Printf(" -> %c \n", PrintPara.cSprinklerNo);	
			PrintPara.cLineWidth = (unsigned char)SwapByte(CS8900_RXTX_DATA0);
			//Printf(" -> %c \n", PrintPara.cLineWidth);
			
			PrintPara.cMultiDotLine12 = (unsigned char)SwapByte(CS8900_RXTX_DATA0);
			//Printf(" -> %c \n", PrintPara.cMultiDotLine12);			
			sTemp1 = SwapByte(CS8900_RXTX_DATA0);
			if ((sTemp1 < -MaxMalposedDot) || (sTemp1 > MaxMalposedDot))
			{
				ResultCode = 1;
				break;
			}
			PrintPara.iYDotSpace12 = (unsigned int)(SprinklerSpace + sTemp1);
			//Printf(" -> %d \n", PrintPara.iYDotSpace12);		
						
			sTemp1 = SwapByte(CS8900_RXTX_DATA0);
			if ((sTemp1 < -MaxMalposedDot) || (sTemp1 > MaxMalposedDot))
			{
				ResultCode = 1;
				break;
			}
			PrintPara.iYDotSpace23 = (unsigned int)(SprinklerSpace + sTemp1);
			//Printf(" -> %d \n", PrintPara.iYDotSpace23);
			
			sTemp1 = SwapByte(CS8900_RXTX_DATA0);
			if ((sTemp1 < -MaxMalposedDot) || (sTemp1 > MaxMalposedDot))
			{
				ResultCode = 1;
				break;
			}
			PrintPara.iYDotSpace34 = (unsigned int)(SprinklerSpace + sTemp1);
			//Printf(" -> %d \n", PrintPara.iYDotSpace34);
			
			//走纸误差20100413
			cTemp = (unsigned char *)&PrintPara.dErrorX;
			for (iTemp=0;iTemp<4;iTemp++)
			{
				sTemp = SwapByte(CS8900_RXTX_DATA0);
				*cTemp = (unsigned char)(sTemp >> 8);
				cTemp++;
				*cTemp = (unsigned char)sTemp;
				cTemp++;
			}
			//Printf(" -> %f \n", PrintPara.dErrorX);			
			iTemp = (int)(PrintPara.dErrorX * X_PPMM);
			if ((iTemp > gl_iTHPulseNumX) || (iTemp < -gl_iTHPulseNumX))
			{
				ResultCode = 1; 		//参数不正确
				break;
			}
			//*****
			cTemp = (unsigned char *)&PrintPara.dErrorY;
			for (iTemp=0;iTemp<4;iTemp++)
			{
				sTemp = SwapByte(CS8900_RXTX_DATA0);
				*cTemp = (unsigned char)(sTemp >> 8);
				cTemp++;
				*cTemp = (unsigned char)sTemp;
				cTemp++;
			}
			//Printf(" -> %f \n", PrintPara.dErrorY);					
			//20100302
			if ((PrintPara.dErrorY > 6.0) || (PrintPara.dErrorY < -6.0))
			{
				ResultCode = 1;
				break;
			}
			
			gl_dErrorY = PrintPara.dErrorY;
			gl_iStaggerDot = (int)(gl_dErrorY * Y_PPMM / DotPulseNum);
			//********

			cTemp = (unsigned char *)&PrintPara.dPictureErrorX;
			for (iTemp=0;iTemp<4;iTemp++)
			{
				sTemp = SwapByte(CS8900_RXTX_DATA0);
				*cTemp = (unsigned char)(sTemp >> 8);
				cTemp++;
				*cTemp = (unsigned char)sTemp;
				cTemp++;
			}
			//Printf(" -> %f \n", PrintPara.dPictureErrorX);				
			cTemp = (unsigned char *)&PrintPara.dPictureErrorY;
			for (iTemp=0;iTemp<4;iTemp++)
			{
				sTemp = SwapByte(CS8900_RXTX_DATA0);
				*cTemp = (unsigned char)(sTemp >> 8);
				cTemp++;
				*cTemp = (unsigned char)sTemp;
				cTemp++;
			}
		//	Printf(" -> %f \n", PrintPara.dPictureErrorY);	
			
			PrintPara.cPenNo = (unsigned char)SwapByte(CS8900_RXTX_DATA0);
			PrintPara.cCutNo = (unsigned char)SwapByte(CS8900_RXTX_DATA0);
			PrintPara.cPrintCutSelect = (unsigned char)SwapByte(CS8900_RXTX_DATA0);

			if ((PrintPara.cSprinklerCount > SprinklerNum) || (PrintPara.cSprinklerCount == 0)
				|| (PrintPara.cSprinklerNo > (SprinklerNum - PrintPara.cSprinklerCount)))
			{
//				Printf("revice 1\n");
				ResultCode = 1;
				break;
			}
			if ((PrintPara.cBidirectional != 0) && (PrintPara.cBidirectional != 1))
			{
//				Printf("revice 2\n");
				ResultCode = 1;
				break;
			}
			if ((PrintPara.cLineWidth < 1) || (PrintPara.cLineWidth > 9))
			{
//				Printf("revice 3\n");
				ResultCode = 1;
				break;
			}
			if (PrintPara.cMultiDotLine12 >= SprinklerWidth)
			{
//				Printf("revice 4\n");
				ResultCode = 1;
				break;
			}
			if ((PrintPara.iYDotSpace12 > (SprinklerSpace + MaxMalposedDot)) || (PrintPara.iYDotSpace12 < (SprinklerSpace - MaxMalposedDot)))
			{
//				Printf("revice 7\n");
				ResultCode = 1;
				break;
			}
			if ((PrintPara.iYDotSpace23 > (SprinklerSpace + MaxMalposedDot)) || (PrintPara.iYDotSpace23 < (SprinklerSpace - MaxMalposedDot)))
			{
//				Printf("revice 8\n");
				ResultCode = 1;
				break;
			}
			if ((PrintPara.iYDotSpace34 > (SprinklerSpace + MaxMalposedDot)) || (PrintPara.iYDotSpace34 < (SprinklerSpace - MaxMalposedDot)))
			{
//				Printf("revice 9\n");
				ResultCode = 1;
				break;
			}
			if ((PrintPara.dPictureErrorX > 1100.0) || (PrintPara.dPictureErrorX < 900.0))
			{
//				Printf("revice 10\n");
				ResultCode = 1;
				break;
			}
			if ((PrintPara.dPictureErrorY > 1100.0) || (PrintPara.dPictureErrorY < 900.0))
			{
//				Printf("revice 11\n");
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
			
//			Printf("SetPara1:SC[%d],SN[%d],BT[%d], LW[%d]\n", gl_cSprinklerCount, gl_cSprinklerNo, gl_cBidirectional, gl_cLineWidth);
//			Printf("SetPara2:PN[0x%d],CN[0x%d],PCS[0x%x]\n",gl_cPenNo, gl_cCutNo, gl_cPrintCutSelect);
//			Printf("SetPara3:ML1[%d],ML2[%d],ML3[%d],YS1[%d],YS2[%d],YS3[%d]\n", gl_cMultiDotLine12, gl_cMultiDotLine23, gl_cMultiDotLine34, gl_iYDotSpace12, gl_iYDotSpace23, gl_iYDotSpace34);
//			Printf("SetPara4:DX[%d],DY[%d],DEX[%d],DEY[%f]\n",(int)gl_dPictureErrorX, (int)gl_dPictureErrorY, (int)gl_dErrorX, gl_dErrorY);
//*******NEW END
			if (WriteParameter() == 1)
			{
				//lcd_display(0,"参数设置成功!");
				//lcd_display(1," ");
				ResultCode = 9;		//参数设置正确
				CalVariableValue();
			}
			else
			{
				ResultCode = 2;		//写FLASH出错
				//lcd_display(0,"参数设置失败!");
				//lcd_display(1," ");
			}
			//ResultCode = 9;
			break;
		default:	//不支持此命令
			ResultCode = 1;
			break;
	}
	Printf("com = %d _____ ResultCode = %d _____ gl_cMachineWorkStatus = %d \n" , com , ResultCode,gl_cMachineWorkStatus);
	return ResultCode;
}
//将数据保存在SRAM打印缓冲区
void from_ethnet_Cut(unsigned char *cHeadData,unsigned int iLength)
{
	unsigned long address;//, taddress;				//当前缓冲区写入地址
	unsigned int iRemainCount;//
	//unsigned char ;
	static unsigned long lAddLength=0;
	unsigned short iData1;
	int i , j ;
	
	lAddLength += iLength;
	
	address = gl_iEthnetRecBufPH + CutDataAddr;				//段首地址
	//taddress = address;
	iRemainCount = CutDataLength- gl_iEthnetRecBufPH;		//剩余空间

	//iData = (unsigned short)(*cHeadData);
	//Printf("lAddLength:%d, iLength: %d\n",lAddLength, iLength);
	
	for(i = 0; i<8; i++)
	{
		Writesdram8bit(address++, cHeadData[i]);
		//Printf("%c", cHeadData[i]);
	}
	
	while(i<iLength)
	{
		//iData1 = SwapByte(CS8900_RXTX_DATA0);
		//iData1 |= (iData << 8);
		iData1 = CS8900_RXTX_DATA0;
		//Printf("\n(%d)[%c%c] \n", i, (char)(iData1>>8),(char)(iData1));
		Writesdram(address, iData1);
		address+=2;
		iRemainCount-=2;
		i+=2;
	}
	{
//		Printf("\n\n");
//		for(j = 0 ; j < iLength; j++ )
//		{
//			//Printf("CUTDram::::iL[%d],addr[0x%x] data:[%c] \n", iLength, address , Readsdram(address-iLength, 8));
//			Printf("%c", Readsdram(address-iLength, 8));
//			address++;
//		}
//		Printf("\n\n");
	}
	
	
	gl_iEthnetRecBufPH = address - CutDataAddr;
//	Printf("\ncToDram:iL[%d],addr[0x%x]\n", iLength, address);
	if ( lAddLength == gl_lCurrCutFileLength)
	{
		lAddLength = 0;
		gl_cFileReceive = 0;
//		Printf("address: %x, lAddLength: %d, gl_iEthnetRecBufPH: %d\n", address, lAddLength, gl_iEthnetRecBufPH);
	}
/*
	Printf("Test Read:\n");
	for(i=0; i< iLength; i++)
	{
		Printf("%c", Readsdram(taddress++,8));
		
	}
	Printf("\n");
	*/
}

//data_length接收数据长度
void from_ethnet(unsigned char *cHeadData,unsigned int iLength)//无切割的写入这里
{
	unsigned int iLength1;
	unsigned short iData,iData1;
	static unsigned char cCountBak = 0;
	static unsigned char cCountFlag = 0;
	unsigned int iCount1,iCount2,iCount3;
	unsigned int iCountBak,iRemainCount;
	unsigned long address;
	
	if ((gl_cFileReceive != 0x01) && (gl_cFileReceive != 0x09))		//没有接收到文件开始标志
		return;
	if (gl_cPrintStatus == 0x20)
		gl_cPrintStatus = 0;
	iLength1 = iLength;
	if (iLength > 8)
	{
		iLength1 -= 8;
		iLength = 8;
	}
	else
		iLength1 = 0;
	
	address = gl_iEthnetRecBufPH + BEGINADDR; //起始 基址
	iRemainCount = EthnetBufLength - gl_iEthnetRecBufPH;
	if (cCountFlag != 0)
	{//已接收到字节数
		iCount1 = (unsigned int)cCountBak;
		iData = (unsigned short)(*cHeadData);
		cHeadData++;
		iLength--;
	}
	else
		iCount1 = 0;
	//Printf("======iData:%d, iLength: %d\n",iData, iLength);
	while (1)
	{
		if (iCount1 != 0)
		{
			if ((address & 0x01) == 1)
			{
				address--;
				iData1 = Readsdram(address,16);
				iData1 |= (iData << 8);
				Writesdram(address,iData1);///////无切割的写入这里
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
			iCount1 = (unsigned short)(*cHeadData);
			cHeadData++;
			iData = (unsigned short)(*cHeadData);
			cHeadData++;
			iLength -= 2;
		}
		else if (iLength > 0)
		{
			cCountBak = (unsigned short)(*cHeadData);
			break;
		}
		else
		{
			cCountFlag = 0;
			break;
		}
	}
	
	if (iLength1 == 0)
	{
		gl_iEthnetRecBufPH = address - BEGINADDR;
		return;
	}
	
	if (cCountFlag != 0)
	{
		iCount1 = (unsigned int)cCountBak;
		iData = CS8900_RXTX_DATA0;
		iLength1--;
		iData1 = iData & 0xff;
		while (1)
		{
			if (iCount1 != 0)
			{
				if ((address & 0x01) == 1)
				{
					address--;
					iData1 <<= 8;
					iData1 |= Readsdram(address, 16);
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
				iData1 = iData & 0xff;
				iData1 = iData1 | (iData1 << 8);
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
			if (iLength1 > 1)
			{
				iCount1 = iData >> 8;
				iData = CS8900_RXTX_DATA0;
				iData1 = iData & 0xff;
				iLength1 -= 2;
			}
			else if (iLength1 > 0)
			{
				cCountBak = iData >> 8;
				break;
			}
			else
			{
				cCountFlag = 0;
				break;
			}
		}
	}
	else
	{
		while (1)
		{
			if (iLength1 > 1)
			{
				iData = CS8900_RXTX_DATA0;
				iCount1 = iData & 0xff;
				iData1 = iData & 0xff00;
				iLength1 -= 2;
				if (iCount1 == 0)
					continue;
			}
			else if (iLength1 > 0)
			{
				iData = CS8900_RXTX_DATA0;
				cCountBak = (unsigned char)iData;
				break;
			}
			else
			{
				cCountFlag = 0;
				break;
			}
			if ((address & 0x01) == 1)
			{
				address--;
				iData1 |= Readsdram(address, 16);
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
			iData1 = iData & 0xff00;
			iData1 = iData1 | (iData1 >> 8);
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
				iData1 >>= 8;
				Writesdram(address,iData1);
				address++;
				iRemainCount--;
			}
		}
	}
	gl_iEthnetRecBufPH = address - BEGINADDR;
	return;
}

//*****************************************************
//判断是否可以接收字符
//参数：	无
//返回值:	无
//*****************************************************
unsigned int CanRevChar(void)
{
	unsigned int iNullSize, DataBufLength;
	
	if ((gl_cFileReceive & 0xe0) != 0)
	{//上一图形还没打印完，不能接收数据
		return 0;
	}
	//Printf("CanRevChar:gl_cCutFileSendFlag:%x\n", gl_cCutFileSendFlag);
	if(gl_cCutFileSendFlag == 0)
		DataBufLength = EthnetBufLength;
	else	if (gl_cCutFileSendFlag == 0x01)
		DataBufLength = CutDataLength;// 20100909   切割数据文件缓冲区长度
	
	if (gl_iEthnetRecBufPH == gl_iEthnetRecBufPE)
	{	
//		gl_cCanRevChar = 1;
		iNullSize = DataBufLength;
	}
	else if (gl_iEthnetRecBufPH > gl_iEthnetRecBufPE)
		iNullSize = DataBufLength - (gl_iEthnetRecBufPH - gl_iEthnetRecBufPE);
	else
		iNullSize = gl_iEthnetRecBufPE - gl_iEthnetRecBufPH;
	///Printf("PH[%d], PE[%d], iNullSize[%d]\n",gl_iEthnetRecBufPH, gl_iEthnetRecBufPE, iNullSize);
	if (iNullSize < 1460)
		return 0;
	else
	{
		iNullSize -= 1460;
		return iNullSize;
	}
//	//从不接收状态到接收状态需要的空余空间要比从接收状态到不接收状态需要的空余空间大,否则在接收缓冲满后每空出一个字节就接收一个字节,中间对通讯的设置会影响绘图速度
//	if (gl_cCanRevChar == 1)
//	{
//		if (iNullSize < 2048)	
//			gl_cCanRevChar = 0;
//	}
//	else
//	{
//		if (iNullSize > 4096)
//			gl_cCanRevChar = 1;
//	}
}

////判断是否已收到一行打印数据
////收到返回1，没收到返回0
//short RecLineData(void)
//{
//	unsigned int iReceivedSize;
//
//	if (gl_iEthnetRecBufPH == gl_iEthnetRecBufPE)
//		return 0;
//	else if (gl_iEthnetRecBufPH > gl_iEthnetRecBufPE)
//		iReceivedSize = gl_iEthnetRecBufPH - gl_iEthnetRecBufPE;
//	else
//		iReceivedSize = EthnetBufLength - (gl_iEthnetRecBufPE - gl_iEthnetRecBufPH);
//
//	if (iReceivedSize >= gl_iLineDataNum)
//		return 1;
//	else
//		return 0;
//}

void ArpRequest(void)
{
	*(TxEthnetFrameBuffer+ETH_HEADER_START+0) = 0xFFFF;					//以太网目的地址
	*(TxEthnetFrameBuffer+ETH_HEADER_START+1) = 0xFFFF;
	*(TxEthnetFrameBuffer+ETH_HEADER_START+2) = 0xFFFF;
	*(TxEthnetFrameBuffer+ETH_HEADER_START+3) = MyMAC[0];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+4) = MyMAC[1];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+5) = MyMAC[2];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+6) = Frame_ARP;			//以太网帧类型

	*(TxEthnetFrameBuffer+ARP_HEADER_START+0) = HARDW_ETH;			//硬件类型
	*(TxEthnetFrameBuffer+ARP_HEADER_START+1) = Frame_IP;				//协议类型
	*(TxEthnetFrameBuffer+ARP_HEADER_START+2) = IP_HLEN_PLEN;		//硬件、协议地址长度
	*(TxEthnetFrameBuffer+ARP_HEADER_START+3) = OP_ARP_REQUEST;
	*(TxEthnetFrameBuffer+ARP_HEADER_START+4) = MyMAC[0];				//发送端以太网地址
	*(TxEthnetFrameBuffer+ARP_HEADER_START+5) = MyMAC[1];
	*(TxEthnetFrameBuffer+ARP_HEADER_START+6) = MyMAC[2];
	*(TxEthnetFrameBuffer + ARP_HEADER_START+7) = MyIP[0];			//发送端IP地址
	*(TxEthnetFrameBuffer + ARP_HEADER_START+8) = MyIP[1];
	*(TxEthnetFrameBuffer + ARP_HEADER_START+9) = 0;						//目的以太网地址
	*(TxEthnetFrameBuffer + ARP_HEADER_START+10) = 0;
	*(TxEthnetFrameBuffer + ARP_HEADER_START+11) = 0;
	if(((RemoteIP[0]^MyIP[0])&SubnetMask[0])||((RemoteIP[1]^MyIP[1])&SubnetMask[1]))
	{
		*(TxEthnetFrameBuffer+12) = GatewayIP[0];//目的IP地址
		*(TxEthnetFrameBuffer+13) = GatewayIP[1];
	}
	else
	{
		*(TxEthnetFrameBuffer+ARP_HEADER_START+12) = RemoteIP[0];//目的IP地址
		*(TxEthnetFrameBuffer+ARP_HEADER_START+13) = RemoteIP[1];
	}
	TxFrameSize = ETH_HEADER_SIZE+ARP_FRAME_SIZE;
	SendFrame(TxFrameSize);
}
