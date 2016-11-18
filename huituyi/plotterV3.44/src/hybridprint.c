#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "..\target\44b.h"
#include "..\target\44blib.h"
#include "..\inc\uart.h"
#include "..\inc\hybridprint.h"
#include "..\inc\extern.h"
#include "..\inc\defconst.h"

extern void WaitKeyExit(unsigned char cKeyValue);
extern void LineReverse(unsigned char line);
extern void lcd_refresh(void);
extern int WriteParameter(void);

#define RECIVE_BUFLEN 256
 u8bit reciveBuff[RECIVE_BUFLEN];
char *sendBuff;

int rec_len=0;
//volatile static char *uart0TxStr;
volatile  char sendflag=0;
volatile int rec_cout=0;
volatile char recSuccFlag = 0;
volatile int Send_Datalen = 0;

int RecDataLend;
char Sig_PullOut=0, Sig_Inset=0;
char Sig_CartridgeLevel = 0xff;     //���ֵΪ0~3˵����ī����ī
char Cdg_MaxNum, Cdg_CurrNum;		//ī�������ֵ��ī������ǰ��ʼֵ

void __irq Uart1_RxInt(void)
{
  	rI_ISPC=BIT_URXD1;
  
	reciveBuff[rec_cout++]=RdURXH0();
	if(rec_cout>2)
	{
		rec_len = reciveBuff[1];
		rec_len |= reciveBuff[2]*256;
		if(rec_cout > rec_len-1)
			recSuccFlag =1;
	}
}

//unsigned int txcount=0;
void __irq Uart1_TxInt(void)
{
    //if(*uart0TxStr != '\0')
    if(sendflag == 1)
    {
		Serial_1_TxChar(sendBuff, Send_Datalen);
		sendflag = 0;
		rI_ISPC=BIT_UTXD1; //Because UART operates using level trigger mode.
		
    }
}

void Serial_1_TxEmpty(void)
{
	while(!(rUTRSTAT1 & 0x4)); //wait until tx shifter is empty.
}

void Serial_1_init(int baud)
{
  int i;
  pISR_UTXD1 = (unsigned)Uart1_TxInt;
  pISR_URXD1 = (unsigned)Uart1_RxInt;
  rUFCON1 = 0x0;     //FIFO disable
  rUMCON1 = 0x0;
  rULCON1 = 0x3;     //Normal,No parity,1 stop,8 bit
  rUCON1 = 0x245;     //rx=edge,tx=edge,disable timeout int.,disable rx error int.,
                     //normal,interrupt or polling
  rUBRDIV1 = ((int)(MCLK/16./baud + 0.5) -1);
  
  for (i=0;i<100;i++);
  rec_cout=0;
  rec_len=0;
}

void Serial_1_TxChar(char *data, int len)
{
	char i=0;
	while(i<len)
	{
		while(!(rUTRSTAT1 & 0x2));  //Wait until THR is empty.
		WrUTXH0(*data++);	
		i++;
	}
}

u8bit Serial_1_RxChar(void)
{
	while(!(rUTRSTAT1 & 0x1)); //Receive data ready
		return RdURXH0();
}

void Hybrid_Send(packet_t *ptrSend, int len)
{
	char i=0;
	int dataLen, packLen;

	rINTMSK |= BIT_UTXD1;	//ES3
	rI_ISPC=BIT_UTXD1;	//ES3	
	
	dataLen = len - PACKET_HEAD_SIZE;
	packLen = 0;
	sendBuff = (char *)_malloc_(len+1);
	sendBuff[packLen++] = 0x04;
	sendBuff[packLen++] = (u8bit)(ptrSend->packetSize&0x00FF);
	sendBuff[packLen++] = (u8bit)((ptrSend->packetSize>>8)&0x00FF);
	sendBuff[packLen++] = 0x0;
	sendBuff[packLen++] = 0x0;
	sendBuff[packLen++] = 0x0;
	sendBuff[packLen++] = 0x0;


	for(i=0; i< dataLen; i++)
	{
		sendBuff[packLen++] = ptrSend->data[i];

	}
	sendBuff[packLen++] = '\0';
	sendflag = 1;
	Send_Datalen = ptrSend->packetSize;
	rINTMSK = ~(BIT_GLOBAL | BIT_UTXD1);	//ES3
	rI_ISPC=BIT_UTXD1;	//ES3	
	
	while(sendflag == 1);

	recSuccFlag = 0;
	rec_cout = 0;
	rec_len = 0;
	rINTMSK = ~(BIT_GLOBAL | BIT_URXD1 |BIT_TIMER0);	//ES3
	rI_ISPC=BIT_URXD1;	//ES3		
	
	while(recSuccFlag != 1);
	// ������һ������
	
}

/* debug
void Hybrid_Send(packet_t *ptrSend, int len)
{
	u8bit senBuf_1[16], senBuf_2[16];
	char dispbuff[20]={'\0'};
	char i=0, j=0, bak=0;

	char string[] = {"0123456789ABCDEF"};
	int dataLen, packLen;
	dataLen = len - PACKET_HEAD_SIZE;
	packLen = 0;

	senBuf_1[packLen++] = 0x04;
	senBuf_1[packLen++] = (u8bit)(ptrSend->packetSize&0x00FF);
	senBuf_1[packLen++] = (u8bit)((ptrSend->packetSize>>8)&0x00FF);
	senBuf_1[packLen++] = 0x0;
	senBuf_1[packLen++] = 0x0;
	senBuf_1[packLen++] = 0x0;
	senBuf_1[packLen++] = 0x0;

	for(i=0; i<packLen; i++)
	{
		dispbuff[bak++] = string[senBuf_1[i]];
	}
	
	for(i=0; i< dataLen; i++)
	{
		senBuf_1[packLen++] = ptrSend->data[i];
		senBuf_2[j++] = string[(senBuf_1[packLen-1]>>4)&0x0f];
		dispbuff[bak++] = senBuf_2[j-1];
		senBuf_2[j++] = string[(senBuf_1[packLen-1])&0x0f];
		dispbuff[bak++] = senBuf_2[j-1];
	}
	dispbuff[bak]='\0';
	senBuf_1[packLen++] = '\0';
	//Printf("Send data:\n");

	//for(i=0; i<len; i++)
	{
//		Serial_1_TxChar(senBuf_1[i], len);
		
	}
	
	//LcdDisp("��������", dispbuff);
	//while(1);
}
*/
char recBuff[50];
void Hybrid_WaitDataReceive(void)
{
	int i=0;
	while((rUTRSTAT1 & 0x1) != 0x01);

	while(rUTRSTAT1 & 0x1)
	recBuff[i++] = RdURXH0();
	
		
}
/*
void Hybrid_Receive(void)
{
	u8bit RevData=1;
	u16bit RevLen, tRev;
	char dispbuff[20]={'\0'};
	char string1[] = {"0123456789ABCDEF"};
	//while((rUTRSTAT0 & 0x1) == 0x0);
	//Printf("Receive!\n");
	while(RevData!=0)
	{
		RevData = Serial_1_RxChar();
	}
	//if(RevData == 1 || RevData == 2 || RevData == 4)
	{
		// Data Type
		RevPacket.dataType = RevData;
		//RevData = 0x02;
		dispbuff[0] = string1[RevData>>4];
		dispbuff[1] = string1[RevData&0x0f];
		// Packet Size
		RevLen= Serial_1_RxChar();		// L
		//RevLen = 0x03;
		dispbuff[2] = string1[RevLen>>4];
		dispbuff[3] = string1[RevLen&0x0f];
		tRev= Serial_1_RxChar();		// H
		//tRev = 0x04;
		dispbuff[4] = string1[tRev>>4];
		dispbuff[5] = string1[tRev&0x0f];
		dispbuff[6] = '\0';
		LcdDisp("�����ַ�", dispbuff);
		while(1);
		tRev <<= 8;
		RevLen |= tRev;
		RevPacket.packetSize = RevLen;

		// Checksum
		tRev = 0;
		if(tRev < 4)
			Serial_1_RxChar();

		// Data
		tRev = 0;
		RevLen -= PACKET_HEAD_SIZE;
		if(RevLen > PACKET_MAX_DATA_SIZE)
		{
			LcdDisp("Error", "data break bounds");
			while(1);
		}
		
		while(tRev < RevLen)
		{
			RevPacket.data[tRev++] = Serial_1_RxChar();
		}

		Hybrid_RecCmdParse();
	}
}
*/

void Hybrid_Receive(void)	
{
	recSuccFlag = 0;
	while(recSuccFlag == 0);
	Hybrid_RecCmdParse();

}

// �����������
void Hybrid_RecCmdParse(void)
{
	char dispBuff[20], cTemp;
		
	switch(reciveBuff[7])	
	{
		case U_CMD_CartridgeErrWarning:		// 0xa0
			switch(reciveBuff[9])
			{
				case U_CMD_CEW_CODE_LowInk:	// 0x01
				case U_CMD_CEW_CODE_OutInk: // 0x02
					// 1#ī�� ī���� �Ƿ������ӡ
					dispBuff[0] = reciveBuff[8]+0x30;
					dispBuff[1] = '\0';
					if(reciveBuff[9] == U_CMD_CEW_CODE_LowInk)
						strcat(dispBuff, "#ī����");
					else
						strcat(dispBuff, "#ī����ī");
					LcdDisp( dispBuff, "7:����  9:����" );
					
					WaitKeyExit(0xff);		//�ȴ��м�����
					cTemp = gl_cKeyBuf[gl_cKeyPE];
					if ((gl_cKeyPE+1) == KeyBufLength)
						gl_cKeyPE = 0;
					else
						gl_cKeyPE++;
					
					if(cTemp == Key_Pause)
					{
						Sig_CartridgeLevel = reciveBuff[8];
						//����ī��ʱ��Ӧ�ùرո�ī��
						Cdg_CurrNum = reciveBuff[8];
						Hybrid_Off_lineCartridge();
					}
					else
						Sig_CartridgeLevel =  0xff;
					break;

				case U_CMD_CEW_CODE_ILHardwareFailure:
				case U_CMD_CEW_CODE_ILSenseError:
					// 1#�豸����
					LcdDisp( "IDS Error: A0", "������" );
					while(1)
					break;
			}
			break;

		case U_CMD_ConnectionStateChange:	// 0xa2
			switch(reciveBuff[9])
			{
				case U_CMD_CSC_CODE_CartridgeOut: // 0x0
					dispBuff[0] = reciveBuff[8]+0x30;
					dispBuff[1] = '\0';
					strcat(dispBuff, "#ī�аγ�");
					LcdDisp(dispBuff,"�����²���ī��");
					
					//ī�аγ�Ӧ�ر�ī������
					//Cdg_CurrNum = reciveBuff[8];
					//Hybrid_Off_lineCartridge();
					Sig_CartridgeInOrOut[reciveBuff[8]] = 0;		// ī�аγ� �������ܴ�ӡ
					Sig_PullOut = 1;
					Sig_Inset = 0;
					break;

				case U_CMD_CSC_CODE_CartridgeIn:  // 0x1
					dispBuff[0] = reciveBuff[8]+0x30;
					dispBuff[1] = '\0';
					strcat(dispBuff, "#ī�в���");
					LcdDisp(dispBuff,"���ڳ�ʼ��");
					//ī�в����Ӧ���ͳ�ʼ��
					Cdg_CurrNum=reciveBuff[8];
					Cdg_MaxNum=1;;
					// ����ī������ʹ�� ����80���óɹ�
					Hybrid_InkCartridgeEnableCheck(S_CMD_SetPrintCartridgeEnable);	

					// ��ѯī������״̬ ����80���ӳɹ�
					Hybrid_CheckInkCartridgeIDAuthInit(D_CMD_GetConnectionState);

					// ���ī��ID ī����֤  
					Hybrid_CheckInkCartridgeIDAuthInit(D_CMD_GetCartridgeInfo);

					// ȡ��OEM ID
					Hybrid_CheckInkCartridgeIDAuthInit(D_CMD_GetOEMID);
					//
					Hybrid_CheckInkCartridgeIDAuthInit(D_CMD_GetOEMPRODUCT);
					
					// ī�г�ʼ��
					Hybrid_CheckInkCartridgeIDAuthInit(A_CMD_PrintCartridgeOnLine);

					Sig_CartridgeInOrOut[reciveBuff[8]] = 1;	   // ī�в��� ��ʼ��ī��
					Sig_Inset = 1;
					Sig_PullOut = 0;
					break;

				case U_CMD_CSC_CODE_UnreconizedHW: // 0x2
					LcdDisp("IDS Error: A2","������");
					while(1)
					break;
				
			}
			break;

		case U_CMD_CommunicationError:		// 0xa3
			switch(reciveBuff[8])
			{
				case U_CMD_CE_CODE_UnknowErr:
					LcdDisp("IDS Error: A3","������");
					while(1)
					break;

				case U_CMD_CE_CODE_PacketCorrupt:
					LcdDisp("���ݰ�У���","������");
					while(1)
					break;

				case U_CMD_CE_CODE_PacketTooBig:
					LcdDisp("���ݰ���С����","������");
					while(1)
					break;

				case U_CMD_CE_CODE_TimeOut:
					LcdDisp("ͨѶ��ʱ","������");
					while(1)
					break;
			}
			break;

		case R_CMD_CommandFailure:			// 0x81
			switch(reciveBuff[8])
			{
				case 1:
					LcdDisp("��īϵͳ","δ����֤");
					while(1);
					break;

				case 2:
					LcdDisp("��������","������Χ");
					while(1);
					break;

				case 3:
					LcdDisp("����","ī��δʹ��");
					while(1);
					break;

				case 4:
					LcdDisp("����","ī��δ��֤");
					while(1);
					break;

				case 5:
					LcdDisp("����","ī��δ����");
					//while(1);
					break;

				case 6:
					LcdDisp("����","ī��δ��ʼ��");
					while(1);
					break;

				case 7:
					LcdDisp("����","Ӳ������");
					while(1);
					break;

				case 8:
					LcdDisp("ERROR","Ink level failure");
					while(1);
					break;

				case 9:
					LcdDisp("ERROR","Broken ink bag");
					while(1);
					break;

				case 10:
					LcdDisp("ERROR","Calibration error");
					while(1);
					break;
					
			}
			break;
			
		case R_CMD_CommandSuccess:			// 0x80
			Sig_CmdExecuteState= 1;
			break;
	}	
}

// �ر�ī������
void Hybrid_Off_lineCartridge(void)
{
	SendPacket.dataType = 0x04;
	SendPacket.packetSize = 0x09;
	SendPacket.checksum = 0x0;
	// Packet data
	SendPacket.data[0] = 0x21;	// command  ID
	SendPacket.data[1] = Cdg_CurrNum;	// parameter  IDS enable

	Hybrid_Send(&SendPacket, SendPacket.packetSize);
	if(reciveBuff[7] != 0x80)
		LcdDisp("ī�йر�ʧ��", " ");
	
}

// IDSʹ��
void Hybrid_IDSEnable(void)
{
	// Packet Head
	SendPacket.dataType = 0x04;
	SendPacket.packetSize = 0x09;
	SendPacket.checksum = 0x0;
	// Packet data
	SendPacket.data[0] = 0x10;	// command  ID
	SendPacket.data[1] = 0x01;	// parameter  IDS enable

	LcdDisp("��īϵͳ��ʼ��", "IDS");

	Hybrid_Send(&SendPacket, SendPacket.packetSize);

	if(reciveBuff[7] == 0x80)
		LcdDisp("��īϵͳ��ʼ��", "IDS OK");
	else
	{
		LcdDisp("��īϵͳ��ʼ��", "IDS Fail!");
		while(1);
	}
}

// ī��ʹ�� 0x12 ,ī�����Ӽ�� 0x33
// CDG_Max: ��Чī����������ͷ��Ч��һ��
// gl_cSprinklerCount ��ͷ��	1~4
// gl_cSprinklerNo	��ͷ��ʼ��	0~3
void Hybrid_InkCartridgeEnableCheck(u8bit cmd)
{
	u8bit uCdg_Num = Cdg_CurrNum;
	u8bit uCdg_Max = Cdg_MaxNum;
	char  errInfo[20];
	char string1[] = {"0123456789ABCDEF"};
	
	LcdDisp("��īϵͳ��ʼ��", "ī��ʹ��");
	
	while(uCdg_Max--)
	{
		// Open a ink cartridge enable 0x12
		// Packet Head
		SendPacket.dataType = 0x04;
		SendPacket.packetSize = 0x0a;
		SendPacket.checksum = 0x0;
		// Packet data
		SendPacket.data[0] = cmd;		// command ID 
		SendPacket.data[1] = uCdg_Num++;		// parameter  ink cartridge Number
		SendPacket.data[2] = ENABLE;	// parameter  
	
		Hybrid_Send(&SendPacket, SendPacket.packetSize);
		if(reciveBuff[7] == 0x80)
		{
			LcdDisp("��īϵͳ��ʼ��", "Check enable OK");
			Sig_CartridgeInOrOut[uCdg_Num-1] = 1;
		}
		else
		{
			strcpy( errInfo, "fail ");
			errInfo[5] = string1[reciveBuff[7]>>4];
			errInfo[6] = string1[reciveBuff[7]&0x0f];
			errInfo[7] = string1[reciveBuff[8]>>4];
			errInfo[8] = string1[reciveBuff[8]&0x0f];			
			errInfo[9] = string1[cmd>>4];//(uCdg_Num-1)+0x30;
			errInfo[10] = string1[cmd&0x0f];		
			errInfo[11] = (uCdg_Num-1)+0x30;
			errInfo[12] = '\0';
			LcdDisp("��īϵͳ��ʼ��", errInfo);
			while(1);
		}
			
	}
	
	
}

// ī��ID ��֤
void Hybrid_CheckInkCartridgeIDAuthInit(u8bit cmd)
{
	u8bit uChangState = 0;
	u8bit uCdg_Num = Cdg_CurrNum;
	u8bit uCdg_Max = Cdg_MaxNum;
	char dispBuff[20],dispNum[2];
	
	while(uCdg_Max--)
	{
		// Open a ink cartridge enable 0x12
		// Packet Head
		SendPacket.dataType = 0x04;
		SendPacket.packetSize = 0x09;
		SendPacket.checksum = 0x0;
		// Packet data
		SendPacket.data[0] = cmd;		// command ID 
		SendPacket.data[1] = uCdg_Num++;		// parameter  ink cartridge Number
		
		Hybrid_Send(&SendPacket, SendPacket.packetSize);

		if( cmd == D_CMD_GetConnectionState )
		{
			if( reciveBuff[7] != 0x80 )
			{
				LcdDisp("��īϵͳ��ʼ��", "Check connec fail");
			}
		}
		if( cmd == D_CMD_GetCartridgeInfo )
		{
			if(( reciveBuff[7] == 0x80 ) && (reciveBuff[14] != 0 ))
			{
				strcpy(dispBuff,"�����ī��");
				dispNum[0] = uCdg_Num-1; dispNum[1] = '\0';
				strcat(dispBuff, dispNum);
				LcdDisp("ī��δ����֤", dispBuff);
				// �ȴ�ī�аγ��ź� 0xA2+0������ī��
				Hybrid_Receive();
				if(Sig_CartridgeInOrOut[uCdg_Num-1]  == 1)
					continue;
		
				// �ȴ�ī�в����ź� 0xA2+1, 
				Hybrid_Receive();
				if(Sig_CartridgeInOrOut[uCdg_Num-1]  == 0)
					continue;
				// ���¼�⵱ǰī��
				uCdg_Max++;
				uCdg_Num--;
				uChangState = 0;
			}
		}
		else if( cmd == D_CMD_GetOEMID )
		{
			//if( reciveBuff[7] == 0x80 && reciveBuff[8] == 0x52)
			if( reciveBuff[7] == 0x80 )
			{
				LcdDisp("��īϵͳ��ʼ��", "OEM Info Ok");
			}
			else
			{
				LcdDisp("��īϵͳ��ʼ��", "OEM Info fail");
				//while(1);
			}
		}
		else if( cmd == D_CMD_GetOEMPRODUCT) 
		{	// �ж�ī���Ƿ����
			if( reciveBuff[7] == 0x80)
			{
				
				if(reciveBuff[8] != HybridOldProductInfo[uCdg_Num-1])
				{// ī���Ѿ���������Ҫ���¼���ʣ��ī��  20111124 
					HybridOldProductInfo[uCdg_Num-1] = reciveBuff[8];
					dispBuff[0] = (HybridOldProductInfo[uCdg_Num-1] >> 4) + 0x30;
					dispBuff[1] = (HybridOldProductInfo[uCdg_Num-1] & 0x0f) + 0x30;
					dispBuff[2] = '\0';
					LcdDisp( "OEM Product Ok", dispBuff);
					WriteParameter();
				}
				
				LcdDisp("��īϵͳ��ʼ��", "OEM Product Ok");
			}
			else
			{
				LcdDisp("��īϵͳ��ʼ��", "OEM Product fail");
				//while(1);
			}
		}
		else// cmd = A_CMD_PrintCartridgeOnLine
		{
			if( reciveBuff[7] == 0x80 )
				LcdDisp("��īϵͳ��ʼ��", "ī�г�ʼ���ɹ�");
			else
			{
				LcdDisp("��īϵͳ��ʼ��", "ī�г�ʼ��ʧ��");
				//while(1);
			}
		}
	}	
}

// ��ī��ϵͳ��ʼ��
void Hybrid_Init()
{
	//interTest();
	Cdg_MaxNum = HybridInkCartridgeNum;
	Cdg_CurrNum = 0;
	// IDS ʹ��
	Hybrid_IDSEnable();

	// ����ī������ʹ�� ����80���óɹ�
	Hybrid_InkCartridgeEnableCheck(S_CMD_SetPrintCartridgeEnable);	

	// ��ѯī������״̬ ����80���ӳɹ�
	Hybrid_InkCartridgeEnableCheck(D_CMD_GetConnectionState);

	// ���ī��ID ī����֤  
	Hybrid_CheckInkCartridgeIDAuthInit(D_CMD_GetCartridgeInfo);

	// ȡ��OEM ID
	Hybrid_CheckInkCartridgeIDAuthInit(D_CMD_GetOEMID);
	// 
	Hybrid_CheckInkCartridgeIDAuthInit(D_CMD_GetOEMPRODUCT);
	
	// ī�г�ʼ��
	Hybrid_CheckInkCartridgeIDAuthInit(A_CMD_PrintCartridgeOnLine);

	Hybrid_InspectRemainVolume();
	Sig_Inset = 1;
	Sig_PullOut = 0;
	rI_ISPC = 0x1ffffff;
	rINTMSK = ~(BIT_GLOBAL |BIT_UERR01|BIT_TIMER0);
	recSuccFlag = 0;
	LcdDisp("��īϵͳ��ʼ��", "���");
	
	
}

//��ѯʣ��ģ��
//LED Display
//1#  2#  3#  4#
//99% 99% 99% 99%
void Hybrid_InspectRemainVolume(void)
{
	char dispLine_1[16], dispLine_2[18];
	int InkCartridgePercent[4]={0};

	Hybrid_CalculateRemainVolume(InkCartridgePercent);

	strcpy(dispLine_1," 1#  2#  3#  4#");
	sprintf(dispLine_2, "%d%% %d%% %d%% %d%%", InkCartridgePercent[0], InkCartridgePercent[1], InkCartridgePercent[2], InkCartridgePercent[3]);
	LcdDisp(dispLine_1, dispLine_2);
}

#define INK_CARTRIDGE_VOLUME	350
#define NOMINAL_INK_VOLUME		352
// ����ʣ��ī��
void Hybrid_CalculateRemainVolume(int ICP[])
{
	u8bit uCdg_Num = Cdg_CurrNum;
	u8bit uCdg_Max = Cdg_MaxNum;
	u16bit TmpInkVolume;
	while(uCdg_Max--)
	{
		// Packet Head
		SendPacket.dataType = 0x04;
		SendPacket.packetSize = 0x09;
		SendPacket.checksum = 0x0;
		// Packet data
		SendPacket.data[0] = 0x30;		// command ID 
		SendPacket.data[1] = uCdg_Num++;		// parameter  ink cartridge Number
	
		Hybrid_Send(&SendPacket, SendPacket.packetSize);
		Hybrid_RecCmdParse();
		TmpInkVolume = reciveBuff[9];
		TmpInkVolume |= (reciveBuff[8]*256);
		ICP[uCdg_Num-1] = (((INK_CARTRIDGE_VOLUME-TmpInkVolume)*100)/NOMINAL_INK_VOLUME);
	}
}

// ��������ī��
// ��ѯʹ�ô�ӡʵ������ī�����ұ��浽IDS
void Hybrid_SetUseInkVolume(void)
{
	u8bit uCdg_Num = Cdg_CurrNum;
	u8bit uCdg_Max = Cdg_MaxNum;
	
	u32bit TmpCurrInkVolume[4]={0};
	//char dispLine_1[16];
	
	//read use ink volume
	//TmpCurrInkVolume[0] = 0x01020304;
	
	//set use ink volume
	while(uCdg_Max--)
	{
		// Packet Head
		SendPacket.dataType = 0x04;
		SendPacket.packetSize = 0x09;
		SendPacket.checksum = 0x0;
		// Packet data
		SendPacket.data[0] = 0x22;		// command ID 
		SendPacket.data[1] = uCdg_Num;		// parameter  ink cartridge Number
		SendPacket.data[2] = (TmpCurrInkVolume[uCdg_Num]>>24)&0x000000ff;;
		SendPacket.data[3] = (TmpCurrInkVolume[uCdg_Num]>>16)&0x000000ff;;
		SendPacket.data[4] = (TmpCurrInkVolume[uCdg_Num]>>8)&0x000000ff;
		SendPacket.data[5] = TmpCurrInkVolume[uCdg_Num++]&0x000000ff;
		Hybrid_Send(&SendPacket, SendPacket.packetSize);
		Hybrid_RecCmdParse();
		if(reciveBuff[7]!=0x80)
		{
			LcdDisp("ī�����ʧ��", "���ī���Ƿ����");
		}
		/*
		dispLine_1[0] = SendPacket.data[2]+0x30;
		dispLine_1[1] = SendPacket.data[3]+0x30;
		dispLine_1[2] = SendPacket.data[4]+0x30;
		dispLine_1[3] = SendPacket.data[5]+0x30;
		LcdDisp(dispLine_1, " ");
		while(1);
		*/
	}	

}

// ����ī������
// ����1~4 5���� 7�˳� ������
void Hybrid_SetInkCartridgeNum(void)
{
	short int  iTemp=1;
	char tempBuff[5], dispBuff[3], cEnd, ch;
	
	iTemp = HybridInkCartridgeNum;
	_itoa_(iTemp, tempBuff, 10);
	
	strcpy(gl_cLcdDispBuf[0],"ī������"); // 1~5
	strcpy(gl_cLcdDispBuf[1],tempBuff);

	lcd_refresh();

	cEnd = 1;
	while (cEnd)
	{
		WaitKeyExit(0xff);		//�ȴ��м�����
		ch = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (ch)
		{
			case Key_Up:
				iTemp++;
				if(iTemp > 4)
					iTemp = 1;
				_itoa_(iTemp, dispBuff, 10);
				LcdDisp("ī������", dispBuff);
				break;
				
			case Key_Down:
				iTemp--;
				if(iTemp < 1)
					iTemp = 4;
				_itoa_(iTemp, dispBuff, 10);
				LcdDisp("ī������", dispBuff);
				break;
				
			case Key_Ok:
				HybridInkCartridgeNum = iTemp;
				WriteParameter();
				cEnd = 0;
				break;
				
			case Key_Pause:
				cEnd = 0;
				break;
				
		}
	}	
}

//�ļ���ʼ��ӡǰ���īˮ�����Ƿ���ھ���ֵ����Ҫ���ī���Ƿ��Ѿ����ӡ�
//�ļ���ӡ�����У���ֽ������������ī�У���Ҫ���¼��ī������״̬
void Hybrid_CheckInkLevel(void)
{
	u8bit uCdg_Num = Cdg_CurrNum;
	u8bit uCdg_Max = Cdg_MaxNum;
	int i=0;
	//char string1[] = {"0123456789ABCDEF"}, disp[20];

	// ��ѯī������״̬ ����80���ӳɹ�
	//Hybrid_InkCartridgeEnableCheck(D_CMD_GetConnectionState);
	
	//LcdDisp("��īϵͳ", "ī��ī�����");
	
	while(uCdg_Max--)
	{
		// Open a ink cartridge enable 0x12
		// Packet Head
		SendPacket.dataType = 0x04;
		SendPacket.packetSize = 0x09;
		SendPacket.checksum = 0x0;
		// Packet data
		SendPacket.data[0] = 0x31;		// command ID 
		SendPacket.data[1] = uCdg_Num++;		// parameter  ink cartridge Number
	
		Hybrid_Send(&SendPacket, SendPacket.packetSize);
		Hybrid_RecCmdParse();

		while(Sig_PullOut == 1)
		{// ��⵽ī�аγ���ֻ�е���⵽ī�����²��벢��ʼ����ɲ����˳�				
			recSuccFlag=0;
			rec_cout = 0;
			rec_len = 0;
			while(recSuccFlag == 0)
			{
				if(i++ > 10000)
				{
					LcdDisp("�����ī��", " ");
					i=0;
				}
			}
			Hybrid_RecCmdParse();

			if(Sig_Inset==1)
			{
				LcdDisp("ī��������", " ");
				uCdg_Num = Cdg_CurrNum;
		 		uCdg_Max = Cdg_MaxNum;
				Sig_CartridgeLevel = 0xff;
				LineReverse(0);
				#if( LANGUAGE == 0 )
				strcpy(gl_cLcdDispBuf[0],"  ��ӭʹ��");
				strcpy(gl_cLcdDispBuf[1],"������ī��ͼ��");
				#else
				strcpy(gl_cLcdDispBuf[0],"Welcome to use");
				strcpy(gl_cLcdDispBuf[1],"enrich plotter");
				#endif
				lcd_refresh();
				break;
			}
		}
		
		while(Sig_CartridgeLevel != 0xff)
		{//ī����ī  ī��δȡ������ 
			recSuccFlag=0;
			rec_cout = 0;
			rec_len = 0;
			while(recSuccFlag == 0)
			{
				if(i++ > 10000)
				{
					LcdDisp("�����ī��", " ");
					i=0;
				}
			}
			Hybrid_RecCmdParse();
			
			if(Sig_PullOut == 1)
			{
				recSuccFlag=0;
				rec_cout = 0;
				rec_len = 0;
				while(recSuccFlag == 0)
				{
					if(i++ > 10000)
					{
						LcdDisp("�����ī��", " ");
						i=0;
					}
				}
				Hybrid_RecCmdParse();
				if(Sig_Inset==1)
				{
					LcdDisp("ī��������", " ");
					uCdg_Num = Cdg_CurrNum;
		 			uCdg_Max = Cdg_MaxNum;
					Sig_CartridgeLevel =  0xff;
					LineReverse(0);
					#if( LANGUAGE == 0 )
					strcpy(gl_cLcdDispBuf[0],"  ��ӭʹ��");
					strcpy(gl_cLcdDispBuf[1],"������ī��ͼ��");
					#else
					strcpy(gl_cLcdDispBuf[0],"Welcome to use");
					strcpy(gl_cLcdDispBuf[1],"enrich plotter");
					#endif
					lcd_refresh();
					break;
				}
			}
		}
		
	}

	rI_ISPC = 0x1ffffff;
	rINTMSK = ~(BIT_GLOBAL |BIT_UERR01|BIT_TIMER0);
}

//����״̬�¼�����������
void Hybrid_ExectInterruptCmd(void)
{
	//��ǰ��ͼ�Ǵ��ڴ���״̬���жϽ��յ�����
	if(gl_cFileReceive == 0)
	{
		Hybrid_CheckInkLevel();
		recSuccFlag = 0;
	}
}


