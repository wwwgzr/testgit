#include "..\inc\defconst.h"

unsigned	char	gl_cCanRevChar;       		//=1 ���Խ����ַ�
unsigned	char	gl_cLCDStatus;						//0λΪ'1'��ʾLCD��0�з��ף�1λΪ'1'��ʾLCD��1�з���

unsigned	char	gl_cIOReadValue;					//��IO��ֵ
unsigned	char	gl_cIOWriteValue;					//дIO��ֵ
unsigned	char	gl_cKeyBuf[KeyBufLength];	//����������
unsigned	char	gl_cKeyPH;								//����������ͷָ��
unsigned	char	gl_cKeyPE;								//����������βָ��
char	gl_cLcdDispBuf[2][16];		//��ʾ���ݱ���

unsigned	char	gl_cUartRecBuf[UartRecBufLength];		//���ڽ��ջ�����
unsigned	char	gl_cUartSendBuf[UartSendBufLength];	//���ڷ��ͻ�����
unsigned	char	gl_cUartRecBufPH;					//���ڽ��ջ�����ͷָ��
unsigned	char	gl_cUartSendBufPH;				//���ڷ��ͻ�����ͷָ��
unsigned	char	gl_cUartSendBufPE;				//���ڷ��ͻ�����βָ��
unsigned	char	gl_cRecFrameOver;					//���ڽ��ձ�־
        	
unsigned	int		gl_iEthnetRecBufPH;			//��̫�����ջ�����ͷָ��
unsigned	int		gl_iEthnetRecBufPE;			//��̫�����ջ�����βָ��
unsigned	short	TxEthnetFrameBuffer[50];	//�������ݷ��ͻ�����
unsigned	short	RxEthnetFrameBuffer[100];	//�������ݽ��ջ�����
unsigned	short	RemoteMAC[3];             // 48 bit MAC
unsigned	short	RecdFrameMAC[3];
unsigned	short	RecdFrameIP[2];
unsigned	short	RecdIpFrameLenth;
unsigned	short	TxFrameSize;
unsigned	short	MyIP[2];
unsigned	short	RemoteIP[2];
unsigned	short	SubnetMask[2];
unsigned	short	GatewayIP[2];

unsigned	char	gl_cFileReceive;						//bit0:�ļ���ʼ���ͱ�־,Ϊ0û���ļ���ʼ����,Ϊ1���ļ��ڴ���
																						//bit7:�ļ����ͽ�����־,Ϊ1���ͽ���
																						//bit6:���һ�����ݲ�0��־,Ϊ1��ʾ�Ѿ���0
																						//bit5:��ӡ������־,Ϊ1��ʾ��ͷ�Ѿ��ص�ԭ��λ��
																						//bit4:�ļ�ɾ����־,Ϊ1��ʾɾ����ǰ�ļ�
																						//bit3:��ϴ��ͷ��־,Ϊ1��ʾ��ϴ��ͷ
unsigned	char	gl_cLineWidth;							//�߿�,'S'ϸ(Ĭ��ֵ),'M'��,'L'��
unsigned	char	gl_cFilePrintStatus;				//�ļ���ӡ״̬,Ϊ0δ��ʼ��ӡ,Ϊ1���ڴ�ӡ
unsigned	char	gl_cCountFlag;							//=1��ʾ���յ��ֽ���;=0��ʾû��ʣ������

unsigned 	char	gl_cSprinklerCount;					//��ͷ��
unsigned	char	gl_cSprinklerNo;						//��Ч��ͷ��ʼ�ţ�0��3��
unsigned	char	gl_cBidirectional;					//˫���ӡ��־,Ϊ1˫���ӡ
unsigned	char	gl_cMultiDotLine12;					//1��2��ͷ֮����ص�������
unsigned	char	gl_cMultiDotLine23;					//2��3��ͷ֮����ص�������
unsigned	char	gl_cMultiDotLine34;					//3��4��ͷ֮����ص�������
unsigned	int		gl_iYDotSpace12;						//1��2��ͷ֮���Y������ĵ���
unsigned	int		gl_iYDotSpace23;						//2��3��ͷ֮���Y������ĵ���
unsigned	int		gl_iYDotSpace34;						//3��4��ͷ֮���Y������ĵ���
unsigned	int		gl_iGraphWidth;							//һ�����е��ֽ���(ͼ��)
unsigned	int		gl_iMaxPictureWidth;				//�ܽ��յ����ͼ�ο��
unsigned	int		gl_iMaxYMM;									//Y�᳤�ȣ���λ��MM��
unsigned	int		gl_iLineDataNum;						//��ӡһ������Ҫ���յ��ֽ���
unsigned	int		gl_iReadDataAddr;						//дFIFO����ʱ��ǰSDRAM��ַ
unsigned	int		gl_iDataEndAddr;						//дFIFO����ʱSDRAM������ַ
unsigned	int		gl_iWriteFIFOAddr;					//����ͷFIFOд������ʼ��ַ
unsigned	int		gl_iDataEndAddr0;						//���ݿ�0������ַ
unsigned	int		gl_iDataEndAddr1;						//���ݿ�1������ַ
unsigned	int		gl_iRollPaper;							//��ֽʱ�������
unsigned	int		gl_iSendPaper;							//��ֽʱ�������

unsigned	char	gl_cReacterStatus;					//����ֽ״̬.bit0Ϊ1:��������ֽ,bit1Ϊ1��������ֽ
unsigned	char	gl_cInvertBufStatus;				//����ת��������״̬.bit0Ϊ1:���ڴ�ӡ���Ѿ���ӡ���ǻ�����1�е�����
																						//bit1Ϊ1��ʾ��ǰӦ׼�����ǻ�����1�е�����
																						//bit2Ϊ1��ʾ������0������׼����
																						//bit3Ϊ1��ʾ������0������׼����
unsigned	char	gl_cCurPrintBuf;						//Ϊ1:��ǰ׼����ӡ�����ڴ�ӡ���ǻ�����1�е�����
unsigned	char	gl_cPrintStatus;						//Ϊ0��ʾû�д�ӡ,Ϊ1��ʾY�����ڴ�ӡ,Ϊ2��ʾY���ӡ����
																						//Ϊ3��ʾX�����ڴ�ӡ,Ϊ4��ʾX���ӡ����
unsigned	char	gl_cSelfCheckFlag;					//��Ӧ���Լ��־,Ϊ0���Լ�,Ϊ1�Լ�
unsigned	char	gl_cRemainderBit;						//����ת��ʱʣ���λ��

unsigned	short	gl_sPlotType;								//����

double gl_dPictureErrorX;										//ͼ�����,X��������1000mm��Ӧ��ʵ�ʻ������
double gl_dPictureErrorY;										//ͼ�����,Y��������1000mm��Ӧ��ʵ�ʻ������
double gl_dErrorX;													//X����ֽ���(��λ:MM)
double gl_dErrorY;													//��������ɨ��֮�����MM��
int		gl_iStaggerDot;												//��������ɨ��֮����ĵ���
int		gl_iTHPulseNumX;											//X�����������ߵ���������û����������£�
int		gl_iPulseNumX;												//X������������
int		gl_iMovePaperLength;									//����ֽ����(��λ:������)
int		gl_iPulseNumY;												//Y������������
int		gl_iOriginalPos;											//ԭ��λ��(��λ:Pulse)
int		gl_iAxisYPos;													//Y������

unsigned	int		gl_iDelayCount;							//��ʱѭ������
unsigned	int		gl_iWaitCount;							//�ȴ���ӡѭ������
unsigned	int		gl_iWaitCount1;
unsigned	char	gl_cDelayFlag;							//�Ƿ���Ҫ��ӡ��ʱ��־

unsigned	int		gl_iRecDataLen;							//USB�ȴ��������ݳ���
unsigned	char	gl_cUsbSendBuf[100];				//USB���ͻ�����
unsigned	char	gl_cUsbSendBufPH;						//USB��������ͷָ��
unsigned	char	gl_cUsbSendBufPL;						//USB��������βָ��
unsigned	char	gl_cCommunicationMode;			//ͨѶ��ʽѡ��0:����������� 1:USB�������� 2:�����USB���ɽ�������
unsigned char  gl_scankeyflag;				//0:��ֹ����ɨ��20100223

unsigned char  gl_WatchDogState;		// 1:�������Ź�//20101118
unsigned long  gl_WatchDogCount;		// ���Ź�������
unsigned char  SprinklerNum;   // 20101214   ��ͷ����ѡ
unsigned short gl_sSendPaperLength;// 20110221 �趨��ֽ���ȣ���λmm
unsigned short gl_sSendPaperDelay; // 20110221 �趨��ֽ�ӳ�1~4
unsigned short gl_sPrintHeadCleanCount; // 20110221 ���ô�ӡͷ��ϴ����
unsigned char gl_DeviceType;	// 20111101 ����豸���� ��ˢ:0/��ˢ :1
unsigned char gl_FlashChipSelect; // 20111101 Flash оƬѡ�� 0= 3201B

