//*******************************************************************
//ÎÄ¼þÃû£ºnetwork.c
//ÃèÊö£º	ÍøÂçÇý¶¯³ÌÐò
//°æ±¾£º	1.0
//×÷Õß£º	wangwei
//ÈÕÆÚ£º	2007-06-11
//*******************************************************************
#include <string.h>
#include <stdio.h>
#include "..\Target\44blib.h"
#include "..\inc\network.h"
#include "..\inc\extern.h"


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

extern	void	CalVariableValue(void);
extern	void	lcd_refresh(void);
extern	void	LineReverse(unsigned char line);
extern	void	ClearPrintHead(unsigned int iFlag);
extern	int		WriteParameter(void);
extern	unsigned short DrawMalposedGraph(unsigned int iGraphWidth);
extern	unsigned short SparyError(unsigned char cPtintCount);

struct Socket_Type
{                                                                                                                              
	unsigned short  Dest_Port;		 		//¶Ô·½¶Ë¿Ú
	unsigned short	Dest_Ip[2];	 			//¶Ô·½ip
	unsigned long	RecPassSeq;
	unsigned long	RecPassAck;
	unsigned long	SenPassSeq;
	unsigned long	SenPassAck;
	unsigned long	RecFutureAck;
	unsigned long	SenFutureSeq;
	unsigned short	FrameIndex;
	unsigned short	Rcv_Window;		 		//¶Ô·½µÄwindow´óÐ¡
	unsigned short	Snd_Window; 		 	//ÎÒµÄwindow´óÐ¡
	unsigned short	Dest_Max_Seg_Size;		//¶Ô·½½ÓÊÜµÄ×î´óµÄÊý¾Ý°ü´óÐ¡MTU
	unsigned short	State;		//Á¬½Ó×´Ì¬
//	uint8		StaSem;	//BIT7±íÊ¾¸ÃÁ¬½ÓÊÇLISTEN¿ªÊ¼µÄ£¬¼´·þÎñÆ÷µÄ¡£BIT1ÊÇ±íÊ¾¸ÃÁ¬½ÓµÈ´ýAppSemConÐÅºÅÁ¿¡£BIT0ÊÇ±íÊ¾ÕýµÈ´ýAppSemSendÐÅºÅÁ¿
}TcpStatus;

const unsigned short MyMAC[3] = {0xE002,0xA04C,0x7A7E};

//¸ßµÍ×Ö½Ú»¥»»
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
**º¯ÊýÔ­ÐÍ:   void 	Init8900()
**Èë¿Ú²ÎÊý:		ÎÞ
**³ö¿Ú²ÎÊý:		ÎÞ
**·µ »Ø Öµ:		ÎÞ              
**Ëµ    Ã÷:		¶ÔÐ¾Æ¬µÄ¹¤×÷¼Ä´æÆ÷½øÐÐÉèÖÃ,¸÷¸ö¼Ä´æÆ÷µÄÓÃ·¨¿É²Î¿¼ÎÄµµºÍ
**				ÂçÐ¾Æ¬µÄÊý¾ÝÊÖ²á
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
	csWritePacketPage(PKTPG_RX_CFG,RX_CFG_RX_OK_IE|RX_CFG_ID);//¹ØËùÓÐÖÐ¶Ï
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
**º¯ÊýÔ­ÐÍ£º    void        Delete_Socket()
**Èë¿Ú²ÎÊý: 	ÎÞ
**³ö¿Ú²ÎÊý: 	ÎÞ
**·µ »Ø Öµ£º	ÎÞ
**¹¦ÄÜËµÃ÷: 	³·Ïú±¾µØÁ¬½Ó£¬²¢Çå¿ÕÖØ·¢»º³åÇø
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
	if (FrameType == Frame_IP)
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
//	
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
	
	if (*(RxEthnetFrameBuffer+TCP_HEADER_START+CS8900_HEADER_SIZE+1) == MY_TCP_PORT)
	{
		if (TcpStatus.State >= TCP_STATE_SYN_RCVD)//ÒÑ¾­½¨Á¢µÄÁ¬½Ó
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
						Tcp_Established();
						break;
					case 8:
						Tcp_Last_Ack();//¹Ø±Õ
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
//½¨Á¢TCPÁ¬½Ó
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
	SendPara PrintPara;
	
	temp_control = *(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+6);
	if(temp_control&(TCP_CODE_RST+TCP_CODE_SYN))
	{
		Delete_Socket();
		return (0);
	}
	if(temp_control&TCP_CODE_FIN)
	{
		TcpStatus.RecPassSeq = Word2ToLong(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+2);
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
	temp_data = temp_data * 4;	//IPÊ×²¿Õ¼20×Ö½Ú
	temp_data = *(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+IP_HEADER_START+1)-temp_data-IP_HEADER_SIZE*2;//Êý¾Ý±¨×Ü³¤¶È¼õÈ¥TCPºÍIPÊ×²¿³¤¶È,µÃµ½Êý¾Ý³¤¶È
	temp_SeqNum = Word2ToLong(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+2);
	temp_AckNum = Word2ToLong(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+4);
	
	if ((temp_data > 0) && (temp_data <= TcpStatus.Snd_Window))
	{
		if (temp_SeqNum == TcpStatus.SenPassAck)//??
		{
			TcpStatus.SenPassAck=TcpStatus.SenPassAck+temp_data;
			if (temp_data < 8)
			{
				for (i=0;i < temp_data;)
				{
					CalcCkSum = SwapByte(CS8900_RXTX_DATA0);
					cHeadstring[i] = (unsigned char)(CalcCkSum >> 8);
					cHeadstring[i+1] = (unsigned char)CalcCkSum;
					i += 2;
				}
				if (iRecDataLen > (unsigned int)temp_data)
				{
					from_ethnet(cHeadstring,(unsigned int)temp_data);
					iRecDataLen -= (unsigned int)temp_data;
					cHeadstring[0] = 0;
				}
				else
				{
					temp_data = (unsigned short)iRecDataLen;
					from_ethnet(cHeadstring,(unsigned int)temp_data);
					iRecDataLen = 0;
					cHeadstring[0] = 3;
				}
			}
			else
			{
				for (i=0;i < 4;i++)
				{
					CalcCkSum = SwapByte(CS8900_RXTX_DATA0);
					cHeadstring[i*2] = (unsigned char)(CalcCkSum >> 8);
					cHeadstring[i*2+1] = (unsigned char)CalcCkSum;
				}
				if (memcmp(cHeadstring,"Buf_Long",8) == 0)	//²éÑ¯»º³åÇø³¤¶È
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
				else if (memcmp(cHeadstring,"Snd_Long",8) == 0)	//·¢ËÍÊý¾Ý°ü³¤¶È
				{
					cHeadstring[0] = 4;
					CalcCkSum = SwapByte(CS8900_RXTX_DATA0);
					iRecDataLen = (unsigned int)(CalcCkSum << 16) + SwapByte(CS8900_RXTX_DATA0);
				}
				else if (memcmp(cHeadstring,"Msg_Read",8) == 0)//¶Á²ÎÊý
				{
					cHeadstring[0] = 5;
				}
				else if (memcmp(cHeadstring,"Msg_Comm",8) == 0)//ÃüÁî
				{
					cHeadstring[0] = 2;
					com = SwapByte(CS8900_RXTX_DATA0);
					ResultCode = DealEthnetCom(com);
				}
				else
				{
					if (iRecDataLen > (unsigned int)temp_data)
					{
						from_ethnet(cHeadstring,(unsigned int)temp_data);
						iRecDataLen -= (unsigned int)temp_data;
						cHeadstring[0] = 0;
					}
					else
					{
						temp_data = (unsigned short)iRecDataLen;
						from_ethnet(cHeadstring,(unsigned int)temp_data);
						cHeadstring[0] = 3;
						if (iRecDataLen < (unsigned int)temp_data)
						{
							#if(LANGUAGE==0)
							strcpy(gl_cLcdDispBuf[0],"  Í¨Ñ¶¹ÊÕÏ");
							strcpy(gl_cLcdDispBuf[1]," ");
							#elif(LANGUAGE==1)
							strcpy(gl_cLcdDispBuf[0]," Communicate Err");
							strcpy(gl_cLcdDispBuf[1]," ");
							#elif(LANGUAGE==2)
							strcpy(gl_cLcdDispBuf[0],"Error comunicac");
							strcpy(gl_cLcdDispBuf[1]," ");
							#else
							strcpy(gl_cLcdDispBuf[0],"Baglanti Hatasi");
							strcpy(gl_cLcdDispBuf[1]," ");
							#endif
							
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
		{   //±íÊ¾ÕâÊÇÒ»¸ö3´ÎÎÕÊÖµÄÈ·ÈÏ
			if ((TcpStatus.SenPassSeq <= temp_AckNum)
			&& (temp_AckNum <= TcpStatus.RecFutureAck))
			{
				TcpStatus.RecPassAck = temp_AckNum;//È·ÈÏ
				
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
			{//ÃüÁîÖ¡Ó¦´ð
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
				*(TxEthnetFrameBuffer+TCP_HEADER_START+20) = 0x2E33;//Îª½â¾ö¼æÈÝÐÔÎÊÌâ20100413
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
				sTemp = (unsigned short *)&PrintPara;
				com = ((sizeof(PrintPara) + 1) / 2);
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
	{//±íÊ¾ÕâÊÇÒ»¸ö3´ÎÎÕÊÖµÄÈ·ÈÏ
		if ((TcpStatus.SenPassSeq <= temp_AckNum)
		&& (temp_AckNum <= TcpStatus.RecFutureAck))
		{
			TcpStatus.RecPassAck = temp_AckNum;//È·ÈÏ
			return (2);
		}
		return (3);
	}
	return (4);
}
/**********************************************************************
**º¯ÊýÔ­ÐÍ£º  void        Tcp_SYN_Rec( )
**Èë¿Ú²ÎÊý:		ÎÞ
**³ö¿Ú²ÎÊý:		ÎÞ
**·µ »Ø Öµ:º	ÎÞ
**¹¦ÄÜËµÃ÷:		´Ótcp_listen½ÓÊÕµ½synºó,¼´¿É½øÈë¸Ã×´Ì¬
**        :		¸Ã×´Ì¬¿É½ÓÊÕtcp_synºÍTCP_CODE_ACK»òtcp_rst
**************************************************************/
void Tcp_SYN_Rec(void) 
{
	unsigned short temp_control;
	unsigned long temp_AckNum;
	
	temp_control = *(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+6);
	
	temp_AckNum = Word2ToLong(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+4);
	if (temp_control&TCP_CODE_RST)		//reset//´¦Àíreset,¶Ô·½²»½ÓÊÜÇëÇó
	{
		Delete_Socket();
	}
	else if (temp_control&(TCP_CODE_SYN))//ÊÕµ½Í¬²½ÐÅºÅ
	{
		TcpStatus.RecPassAck = temp_AckNum;//È·ÈÏ
		TcpStatus.RecPassSeq = Word2ToLong(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+2);//¶Ô·½µÄ³õÊ¼»¯Ë³ÐòºÅ
		TcpStatus.SenPassSeq = TcpStatus.RecPassAck;
		TcpStatus.SenPassAck = TcpStatus.RecPassSeq;//ÎÒµÄË³ÐòºÅ,ÓÃÓÚ·¢ËÍ
		
		SendRstFrame();
		Delete_Socket();
	}
	else if (temp_control&TCP_CODE_ACK)	//±íÊ¾ÕâÊÇÒ»¸ö3´ÎÎÕÊÖµÄÈ·ÈÏ±íÃ÷Á¬½Ó½¨Á¢
	{
 		if( temp_AckNum == TcpStatus.RecFutureAck)
		{
			TcpStatus.RecPassAck = temp_AckNum;//È·ÈÏ
			TcpStatus.RecPassSeq = Word2ToLong(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+2);//È·ÈÏ
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
		if (*(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+10) == 0x0204)		//0204Îª×î´ósegmentÑ¡Ïî
		{
			TcpStatus.Dest_Max_Seg_Size = *(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+11);
		}
		TcpHeadHandle(0x7000+TCP_CODE_ACK+TCP_CODE_SYN);
	}
}
/**********************************************************************
**º¯ÊýÔ­ÐÍ£º    void        Tcp_Last_Ack( )
**Èë¿Ú²ÎÊý:		ÎÞ
**³ö¿Ú²ÎÊý:		ÎÞ
**·µ »Ø Öµ£º	ÎÞ
**Ëµ    Ã÷£º	½ÓÊÕµ½TCP_RST,TCP_SYN»òTCP_CODE_ACK±¨ÎÄÊ±,¹Ø±ÕÁ¬½Ó
************************************************************************/
void Tcp_Last_Ack(void) 
{
	unsigned short temp_control;
    
	temp_control = *(RxEthnetFrameBuffer+CS8900_HEADER_SIZE+TCP_HEADER_START+6);
	if (temp_control&(TCP_CODE_RST+TCP_CODE_SYN+TCP_CODE_ACK))
		Delete_Socket();
}



//ÂÖÑ¯CS8900ÊÇ·ñÓÐ½ÓÊÕÊý¾Ý
void PollRecNetData(void)
{
	unsigned short iEvent;
	
	iEvent = CS8900_ISQ;//ReadNetDataLH(PORT_ISQ);
	while (iEvent != 0)
	{
		switch (iEvent & REG_NUM_MASK)
		{
			case REG_NUM_RX_EVENT:				//RxEvent(Register 4)
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
}

/**********************************************************************
**º¯ÊýÔ­ÐÍ£º    void 		SendFrame(Uint16  length)
**Èë¿Ú²ÎÊý:		uint length					:·¢ËÍÊý¾Ý°üµÄ³¤¶È
**³ö¿Ú²ÎÊý:		ÎÞ
**·µ »Ø Öµ£º	ÎÞ              
**Ëµ    Ã÷£º	·¢ËÍÊý¾Ý°ü,ÒÔÌ«Íøµ×²ãÇý¶¯³ÌÐò,ËùÓÐµÄÊý¾Ý·¢ËÍ¶¼ÒªÍ¨¹ý¸Ã³ÌÐò
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

//ÒÔÌ«ÍøÃüÁî´¦Àí
unsigned short DealEthnetCom(unsigned short com)
{
	unsigned short ResultCode;
	unsigned short sTemp;
	short sTemp1;
	unsigned char *cTemp;
	unsigned char cTemp1,cTemp2,cTemp3;
	unsigned int iTemp1,iTemp2,iTemp3;
	int iTemp;
	double dTemp;
	SendPara PrintPara;
	//char debugBuff[20];

	if (com >= 10)
	{
		if (gl_cFileReceive != 0)
		{//ÉÏÒ»Í¼ÐÎÎ´´òÓ¡Íê±Ï,²»ÄÜÉèÖÃ´Ë²ÎÊý
			ResultCode = 3;
			return ResultCode;
		}
	}
	switch (com)
	{
		case 1:	//ÎÄ¼þ¿ªÊ¼´«ËÍÃüÁî
			sTemp = SwapByte(CS8900_RXTX_DATA0);
			iTemp1 = (sTemp << 16) + SwapByte(CS8900_RXTX_DATA0);
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
			{//µÈ´ýÉÏÒ»Í¼ÐÎ´òÓ¡Íê
				if ((gl_cFileReceive & 0x60) == 0)
				{
					if ((gl_iEthnetRecBufPE != gl_iEthnetRecBufPH) || (ReadPrintEnd() == 0))
					{
						gl_cFileReceive |= 0x80;
						ResultCode = 2;
						break;
					}
					else if (gl_iPulseNumY < 0)
					{//ÅçÍ·»ØÔ­µã
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
			gl_cCommunicationMode = 0;
			gl_iDelayCount = 0;
			gl_iWaitCount = 0;
			gl_iWaitCount1 = 0;
			gl_cDelayFlag = 0x80;
			if (gl_iAxisYPos <= gl_iOriginalPos)
				ClearPrintHead(0);
			else
				gl_cFileReceive |= 0x08;
			WriteFilePrint(0xaa);
			ClrFIFO;
			break;
		case 2:	//ÎÄ¼þ´«ËÍ½áÊøÃüÁî
			if (gl_iEthnetRecBufPE == gl_iEthnetRecBufPH)
			{
				if ((gl_cFileReceive & 0x60) == 0)
				{
					gl_cFileReceive = 0;
					gl_cCommunicationMode = 0xff;
				}
			}
			else if ((gl_cFileReceive & 0x01) == 0x01)
				{
					gl_cFileReceive |= 0x80;
				}
			ResultCode = 9;
			break;
		case 4:
			Printf("Del file 1: %x\n", gl_cFileReceive);
			if(gl_cFileReceive != 0)
				gl_cFileReceive |= 0x10;	//É¾³ýµ±Ç°´òÓ¡ÎÄ¼þ
			Printf("Del file 2: %x\n", gl_cFileReceive);
			ResultCode = 9;
			break;
		case 5:	//²éÑ¯ÃüÁî
			if (CanRevChar() != 0)
//			if (gl_cCanRevChar == 1)
				ResultCode = 9;
			else
				ResultCode = 2;
			break;
		case 6:	//Í¼ÐÎÎó²îÉèÖÃÃüÁî
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
				ResultCode = 9;		//²ÎÊýÉèÖÃÕýÈ·
			else
				ResultCode = 2;		//Ð´FLASH³ö´í
			break;
		case 7:		//ÔÝÍ£´òÓ¡
			while ((gl_cPrintStatus != 0) && (gl_cPrintStatus != 0x10));
			if (gl_cPrintStatus == 0)
			{
				gl_cPrintStatus = 0x20;	//ÔÝÍ£
				ResultCode = 9;
			}
			else
				ResultCode = 1;
			break;
		case 14:		//´òÓ¡×ßÖ½Îó²îÐÞÕýÍ¼
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
				ResultCode = 1;			//²ÎÊý²»ÕýÈ·
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
		case 15:		//´òÓ¡ÅçÍ·¼äY·½ÏòÎó²îÐÞÕýÍ¼
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
			gl_cMultiDotLine12 = cTemp1;	//´òÓ¡Íê±Ï£¬²ÎÊý»¹Ô­
			gl_cMultiDotLine23 = cTemp2;
			gl_cMultiDotLine34 = cTemp3;
			gl_iYDotSpace12 = iTemp1;
			gl_iYDotSpace23 = iTemp2;
			gl_iYDotSpace34 = iTemp3;
			CalVariableValue();
			break;
		case 17:		//´òÓ¡Ë«Ïò´òÓ¡Îó²îÐÞÕýÍ¼
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
				ResultCode = 1;		//²ÎÊý²»ÕýÈ·
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
		case 81:	//¸üÐÂ²ÎÊý
			PrintPara.cBidirectional = (unsigned char)SwapByte(CS8900_RXTX_DATA0);
			PrintPara.cSprinklerCount = (unsigned char)SwapByte(CS8900_RXTX_DATA0);
			PrintPara.cSprinklerNo = (unsigned char)SwapByte(CS8900_RXTX_DATA0);
			PrintPara.cLineWidth = (unsigned char)SwapByte(CS8900_RXTX_DATA0);
			PrintPara.cMultiDotLine12 = (unsigned char)SwapByte(CS8900_RXTX_DATA0);
			PrintPara.cMultiDotLine23 = (unsigned char)SwapByte(CS8900_RXTX_DATA0);
			PrintPara.cMultiDotLine34 = (unsigned char)SwapByte(CS8900_RXTX_DATA0);
			//20100301 chg
			sTemp1 = SwapByte(CS8900_RXTX_DATA0);
			if ((sTemp1 < -MaxMalposedDot) || (sTemp1 > MaxMalposedDot))
			{
				ResultCode = 1;
				break;
			}
			PrintPara.iYDotSpace12 = (unsigned int)(SprinklerSpace + sTemp1);

			
			sTemp1 = SwapByte(CS8900_RXTX_DATA0);
			if ((sTemp1 < -MaxMalposedDot) || (sTemp1 > MaxMalposedDot))
			{
				ResultCode = 1;
				break;
			}
			PrintPara.iYDotSpace23 = (unsigned int)(SprinklerSpace + sTemp1);

			sTemp1 = SwapByte(CS8900_RXTX_DATA0);
			if ((sTemp1 < -MaxMalposedDot) || (sTemp1 > MaxMalposedDot))
			{
				ResultCode = 1;
				break;
			}
			PrintPara.iYDotSpace34 = (unsigned int)(SprinklerSpace + sTemp1);

			cTemp = (unsigned char *)&PrintPara.dErrorX;
			for (iTemp=0;iTemp<4;iTemp++)
			{
				sTemp = SwapByte(CS8900_RXTX_DATA0);
				*cTemp = (unsigned char)(sTemp >> 8);
				cTemp++;
				*cTemp = (unsigned char)sTemp;
				cTemp++;
			}	
			iTemp = (int)(PrintPara.dErrorX * X_PPMM);
			if ((iTemp > gl_iTHPulseNumX) || (iTemp < -gl_iTHPulseNumX))
			{
				ResultCode = 1;			//²ÎÊý²»ÕýÈ·
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
			for (iTemp=0;iTemp<4;iTemp++)
			{
				sTemp = SwapByte(CS8900_RXTX_DATA0);
				*cTemp = (unsigned char)(sTemp >> 8);
				cTemp++;
				*cTemp = (unsigned char)sTemp;
				cTemp++;
			}
			cTemp = (unsigned char *)&PrintPara.dPictureErrorY;
			for (iTemp=0;iTemp<4;iTemp++)
			{
				sTemp = SwapByte(CS8900_RXTX_DATA0);
				*cTemp = (unsigned char)(sTemp >> 8);
				cTemp++;
				*cTemp = (unsigned char)sTemp;
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
			gl_cBidirectional = PrintPara.cBidirectional;				//Ë«Ïò´òÓ¡
			gl_cLineWidth = PrintPara.cLineWidth;
			gl_cMultiDotLine12 = PrintPara.cMultiDotLine12;
			gl_cMultiDotLine23 = PrintPara.cMultiDotLine23;
			gl_cMultiDotLine34 = PrintPara.cMultiDotLine34;
			gl_iYDotSpace12 = (unsigned int)PrintPara.iYDotSpace12;
			gl_iYDotSpace23 = (unsigned int)PrintPara.iYDotSpace23;
			gl_iYDotSpace34 = (unsigned int)PrintPara.iYDotSpace34;
			gl_dErrorX  = PrintPara.dErrorX;//×ßÖ½Îó²î20100413
			gl_dPictureErrorX = PrintPara.dPictureErrorX;
			gl_dPictureErrorY = PrintPara.dPictureErrorY;
			//20100225 ±£´æ²ÎÊý
			
			if (WriteParameter() == 1)
			{
				//lcd_display(0,"²ÎÊýÉèÖÃ³É¹¦!");
				//lcd_display(1," ");
				ResultCode = 9;		//²ÎÊýÉèÖÃÕýÈ·
				CalVariableValue();
			}
			else
			{
				ResultCode = 2;		//Ð´FLASH³ö´í
				//lcd_display(0,"²ÎÊýÉèÖÃÊ§°Ü!");
				//lcd_display(1," ");
			}
			//ResultCode = 9;
			break;
		default:	//²»Ö§³Ö´ËÃüÁî
			ResultCode = 1;
			break;
	}
	return ResultCode;
}
//½«Êý¾Ý±£´æÔÚSRAM´òÓ¡»º³åÇø
//data_length½ÓÊÕÊý¾Ý³¤¶È
void from_ethnet(unsigned char *cHeadData,unsigned int iLength)
{
	unsigned int iLength1;
	unsigned short iData,iData1;
	static unsigned char cCountBak = 0;
	static unsigned char cCountFlag = 0;
	unsigned int iCount1,iCount2,iCount3;
	unsigned int iCountBak,iRemainCount;
	unsigned long address;
	
	if ((gl_cFileReceive != 0x01) && (gl_cFileReceive != 0x09))		//Ã»ÓÐ½ÓÊÕµ½ÎÄ¼þ¿ªÊ¼±êÖ¾
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
	
	address = gl_iEthnetRecBufPH + BEGINADDR;
	iRemainCount = EthnetBufLength - gl_iEthnetRecBufPH;
	if (cCountFlag != 0)
	{//ÒÑ½ÓÊÕµ½×Ö½ÚÊý
		iCount1 = (unsigned int)cCountBak;
		iData = (unsigned short)(*cHeadData);
		cHeadData++;
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
					iData1 |= Readsdram(address);
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
				iData1 |= Readsdram(address);
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
//ÅÐ¶ÏÊÇ·ñ¿ÉÒÔ½ÓÊÕ×Ö·û
//²ÎÊý£º	ÎÞ
//·µ»ØÖµ:	ÎÞ
//*****************************************************
unsigned int CanRevChar(void)
{
	unsigned int iNullSize;
	
	if ((gl_cFileReceive & 0xe0) != 0)
	{//ÉÏÒ»Í¼ÐÎ»¹Ã»´òÓ¡Íê£¬²»ÄÜ½ÓÊÕÊý¾Ý
		return 0;
	}
	if (gl_iEthnetRecBufPH == gl_iEthnetRecBufPE)
	{	
		iNullSize = EthnetBufLength;
	}
	else if (gl_iEthnetRecBufPH > gl_iEthnetRecBufPE)
		iNullSize = EthnetBufLength - (gl_iEthnetRecBufPH - gl_iEthnetRecBufPE);
	else
		iNullSize = gl_iEthnetRecBufPE - gl_iEthnetRecBufPH;
	
	if (iNullSize < 1460)
		return 0;
	else
	{
		iNullSize -= 1460;
		return iNullSize;
	}
}

void ArpRequest(void)
{
	*(TxEthnetFrameBuffer+ETH_HEADER_START+0) = 0xFFFF;					//ÒÔÌ«ÍøÄ¿µÄµØÖ·
	*(TxEthnetFrameBuffer+ETH_HEADER_START+1) = 0xFFFF;
	*(TxEthnetFrameBuffer+ETH_HEADER_START+2) = 0xFFFF;
	*(TxEthnetFrameBuffer+ETH_HEADER_START+3) = MyMAC[0];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+4) = MyMAC[1];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+5) = MyMAC[2];
	*(TxEthnetFrameBuffer+ETH_HEADER_START+6) = Frame_ARP;			//ÒÔÌ«ÍøÖ¡ÀàÐÍ

	*(TxEthnetFrameBuffer+ARP_HEADER_START+0) = HARDW_ETH;			//Ó²¼þÀàÐÍ
	*(TxEthnetFrameBuffer+ARP_HEADER_START+1) = Frame_IP;				//Ð­ÒéÀàÐÍ
	*(TxEthnetFrameBuffer+ARP_HEADER_START+2) = IP_HLEN_PLEN;		//Ó²¼þ¡¢Ð­ÒéµØÖ·³¤¶È
	*(TxEthnetFrameBuffer+ARP_HEADER_START+3) = OP_ARP_REQUEST;
	*(TxEthnetFrameBuffer+ARP_HEADER_START+4) = MyMAC[0];				//·¢ËÍ¶ËÒÔÌ«ÍøµØÖ·
	*(TxEthnetFrameBuffer+ARP_HEADER_START+5) = MyMAC[1];
	*(TxEthnetFrameBuffer+ARP_HEADER_START+6) = MyMAC[2];
	*(TxEthnetFrameBuffer + ARP_HEADER_START+7) = MyIP[0];			//·¢ËÍ¶ËIPµØÖ·
	*(TxEthnetFrameBuffer + ARP_HEADER_START+8) = MyIP[1];
	*(TxEthnetFrameBuffer + ARP_HEADER_START+9) = 0;						//Ä¿µÄÒÔÌ«ÍøµØÖ·
	*(TxEthnetFrameBuffer + ARP_HEADER_START+10) = 0;
	*(TxEthnetFrameBuffer + ARP_HEADER_START+11) = 0;
	if(((RemoteIP[0]^MyIP[0])&SubnetMask[0])||((RemoteIP[1]^MyIP[1])&SubnetMask[1]))
	{
		*(TxEthnetFrameBuffer+12) = GatewayIP[0];//Ä¿µÄIPµØÖ·
		*(TxEthnetFrameBuffer+13) = GatewayIP[1];
	}
	else
	{
		*(TxEthnetFrameBuffer+ARP_HEADER_START+12) = RemoteIP[0];//Ä¿µÄIPµØÖ·
		*(TxEthnetFrameBuffer+ARP_HEADER_START+13) = RemoteIP[1];
	}
	TxFrameSize = ETH_HEADER_SIZE+ARP_FRAME_SIZE;
	SendFrame(TxFrameSize);
}
