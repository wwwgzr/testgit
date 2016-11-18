
#ifndef __HYBRIDPRINT_H__
#define __HYBRIDPRINT_H__
// 默认通讯速率9600
#define PACKET_HEAD_SIZE		7			//包头数据长度
#define PACKET_MAX_DATA_SIZE	248			//包数据最大长度

// Settings
#define S_CMD_SetUnsolicitedStatus		0x10	//Byte 1:  0 off / 1 on / def off
#define S_CMD_SetRS232BaudRate			0x11	//Set Rs232 baud rate / def 9600
#define S_CMD_SetPrintCartridgeEnable 	0x12	//Set cartridge enable
												//Byte 1: 0~3 cartridge
												//Byte 2: 0 Disable / 1 Enable
// Actions
#define A_CMD_PrintCartridgeOnLine		0x20	//OnLine/initialization 0~3 cartridge
#define A_CMD_PrintCartridgeOffLine		0x21	//off-line  0~3 cartridge 
#define A_CMD_SwitchPump				0x27	// 0 off / 1 on 

// Data/Status requests cmd
#define	D_CMD_GetCartridgeInfo			0x32	// 0~3 cartridge 
#define D_CMD_GetConnectionState		0x33	// 0~3 cartridge 
#define D_CMD_GetOEMID					0x35
#define D_CMD_GetOEMPRODUCT				0x36	//
// Unsolicited status Hp主动提供的状态指令
#define	U_CMD_CartridgeErrWarning		0xa0	//Byte 1: 墨袋编号0~3
												//Byte 2: 0x01 Low ink
												// 		  0x02 outof ink
												//		  0x40 IL sense error
												//		  0x80 IL hardware failure
#define U_CMD_CEW_CODE_LowInk				0x01
#define U_CMD_CEW_CODE_OutInk				0x02
#define U_CMD_CEW_CODE_ILSenseError			0x40
#define U_CMD_CEW_CODE_ILHardwareFailure 	0x80

#define U_CMD_ConnectionStateChange	    0xa2	//Byte 1: 墨袋号
												//Byte 2: 0 cartridge out
												//		  1 cartridge in
												//		  2 unrecognized HW
#define U_CMD_CSC_CODE_CartridgeOut		0x00
#define U_CMD_CSC_CODE_CartridgeIn		0x01
#define U_CMD_CSC_CODE_UnreconizedHW	0x02
												
#define U_CMD_CommunicationError		0xa3	//Byte 1:
												// 0x1 unknow error
												// 0x2 packet corrupt 
												// 0x4 packet too big
#define U_CMD_CE_CODE_UnknowErr		 0x1
#define U_CMD_CE_CODE_PacketCorrupt  0x2
#define U_CMD_CE_CODE_PacketTooBig	 0x4
#define U_CMD_CE_CODE_TimeOut		 0x8
// Command Acknowledgement
#define R_CMD_CommandSuccess		0x80		// 2B: Ink delivered in milliliters
												// 1B: Ink level status, 0x1 low ink/0x2 out of ink/0x40 IL sense error/IL hardware error 0x80
												//12B: Ink cartridge info: 1~4B unique serial #form smart chip
												//						   5~6B Nominial ink cartridge ink volume in ml
												//						   	 7B Primary color code, 0 Black/1 Cyan/ 3 Yellow/ 
												//							 8B Secondary color code
												//							 9B Manufacture ID code
												//							10B Manufacture line code
												//							11B Manufacture year
												//							12B Manufacture week
												// 1B: Connection state, 0 Ink cartridge out / 1 Ink cartridge in / 2 Unrecognized HW
												// 1B: OEM ID
												// 1B: OEM product #
												// 2B: User data
												// 1B: System status, 0 OK / 1 Firmware error
												// 3B: Firmware revision, B1 Revision Level/ B2 Major release/ B3 Minor release
												

#define R_CMD_CommandFailure		0x81		//Error Code 1Byte
												// 1: Invalid command
												// 2: Invalid parameter
												// 3: Disabled cartridge
												// 4: invalid cartridge
												// 5: No cartridge
												// 6: Need init
												// 7: Hardware failure
												// 8: Ink level failure
												// 9: Broken ink bag
												//10: Calibration error
#define DISABLE		0x0
#define ENABLE		0x1

typedef unsigned char u8bit;
typedef unsigned int u16bit;
typedef unsigned long u32bit;

// signal variable
u8bit Sig_CartridgeInOrOut[4];  // 0拔出 / 1插入
u8bit Sig_CmdExecuteState; // 1命令执行返回成功 / 0失败
u8bit HybridOldProductInfo[4];	// 
u8bit HybridInkCartridgeNum=1;	// 墨盒数量

typedef struct
{
	u8bit dataType;	//<header> =1,binary data；=2,ASCII text data；=4,ignore the checksum
	u16bit packetSize;	//<header> 
	u32bit checksum;	//<header>
	u8bit data[PACKET_MAX_DATA_SIZE]; // PACKET_MAX_DATA_SIZE=248
} packet_t;

packet_t SendPacket;
packet_t RevPacket;

void Serial_1_init(int baud);
//void Serial_1_TxChar(char data);
void Serial_1_TxChar(char *data, int len);
u8bit Serial_1_RxChar(void);

void Hybrid_Send(packet_t *ptrSend, int len);
void Hybrid_Receive(void);
void Hybrid_RecCmdParse(void);
void Hybrid_DispErrorInfo(void);

void Hybrid_CheckInkCartridgeIDAuthInit(u8bit cmd);
void Hybrid_InkCartridgeEnableCheck(u8bit cmd);
void Hybrid_ExectInterruptCmd(void);
void Hybrid_CheckInkLevel(void);
void Hybrid_Off_lineCartridge(void);
void Hybrid_CalculateRemainVolume(int ICP[]);
void Hybrid_InspectRemainVolume(void);
void Hybrid_SetUseInkVolume(void);

void Hybrid_Init(void);


/*
void interTest()
{
	unsigned long lTemp;
	char key,i;
	char dpbuf[18], dp[18];
	char string1[] = {"0123456789ABCDEF"};
	
	rINTMSK |= BIT_UTXD0;	//ES3
	rI_ISPC=BIT_UTXD0;	//ES3
	sendBuff = (char *)_malloc_(10);
	sendBuff[0] =  0x04;
	sendBuff[1] =  0x09;
	sendBuff[2] =  0x0;
	sendBuff[3] =  0x0;
	sendBuff[4] =  0x0;
	sendBuff[5] =  0x0;
	sendBuff[6] =  0x0;
	sendBuff[7] =  0x10;
	sendBuff[8] =  0x01;
	sendBuff[9] =  '\0';
	sendflag = 1;
	rINTMSK = ~(BIT_GLOBAL | BIT_UTXD0);	//ES3
	rI_ISPC=BIT_UTXD0;	//ES3	
	//LcdDisp("大墨盒初始化", "rr");
	while(sendflag == 1);
	*uart0TxStr = '\0';
	//LcdDisp("大墨盒初始化", "dd");
 	rec_cout=0;
	recSuccFlag=0;
	rINTMSK = ~(BIT_GLOBAL | BIT_URXD0);	//ES3
	rI_ISPC=BIT_URXD0;	//ES3		
	//Delay(1000);
	while(recSuccFlag != 1);
	
i=0;
	while(1)
	{
		
		//TestUart0();
		lTemp = rINTPND;
		if((lTemp & 0x04) == 0x04)
		{// Tx
			//Puts("Tx \n");
		}
		else if((lTemp & 0x08) == 0x08)
		{// Rx
			//if(recSuccFlag == 1)
			dp[i++] = RdURXH0();
			
			if(i>3)
			{
				
				break;
			}
			
			while(recSuccFlag != 1);
			
			{
				dpbuf[0] = string1[reciveBuff[0]>>4];
				dpbuf[1] = string1[reciveBuff[0]&0x0f];
				dpbuf[2] = string1[reciveBuff[1]>>4];
				dpbuf[3] = string1[reciveBuff[1]&0x0f];		
				dpbuf[4] = string1[reciveBuff[2]>>4];
				dpbuf[5] = string1[reciveBuff[2]&0x0f];		
				dpbuf[6] = string1[reciveBuff[3]>>4];
				dpbuf[7] = string1[reciveBuff[3]&0x0f];	
				dpbuf[8] = string1[reciveBuff[4]>>4];
				dpbuf[9] = string1[reciveBuff[4]&0x0f];
				//dpbuf[10] = string1[reciveBuff[5]>>4];
				//dpbuf[11] = string1[reciveBuff[5]&0x0f];	
				dpbuf[10] = string1[rec_len>>8];
				dpbuf[11] = string1[rec_len&0x00ff];
				dpbuf[12] = string1[recSuccFlag>>4];
				dpbuf[13] = string1[recSuccFlag&0x0f];				
				dpbuf[14] = '\0';	
				
				LcdDisp("大墨盒初始化", dpbuf); while(1);
			}
			
			//key = RdURXH0();
			//Putch(string1[key]);
			//Putch('\n');
		}
		else
		{
			//key = Serial_1_RxChar();
			//Putch('R');
			//Putch(key);
		}
		
	}
	dpbuf[0] = string1[dp[0]>>4];
	dpbuf[1] = string1[dp[0]&0x0f];					
	dpbuf[2] = string1[dp[1]>>4];
	dpbuf[3] = string1[dp[1]&0x0f];						
	dpbuf[4] = string1[dp[2]>>4];
	dpbuf[5] = string1[dp[2]&0x0f];					
	dpbuf[6] = '\0';	
				
	LcdDisp("大墨盒初始化", dpbuf); while(1);
}
*/
#endif








