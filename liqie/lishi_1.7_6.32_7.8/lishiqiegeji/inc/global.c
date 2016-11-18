#include "..\src\defconst.h"

//unsigned	char	Daochushi; 

unsigned	char	gl_cCanRevChar;       		//=1 ���Խ����ַ�
unsigned	char	gl_cLCDStatus;						//0λΪ'1'��ʾLCD��0�з��ף�1λΪ'1'��ʾLCD��1�з���

unsigned	char	gl_cIOReadValue;					//��IO��ֵ
unsigned	char	gl_cIOWriteValue;					//дIO��ֵ
unsigned	char	gl_cKeyBuf[KeyBufLength];	        //����������
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

int		gl_time = 4;							//ÿ����ϴ��ͷ��ʱ�䳤��
unsigned	char	gl_cFileReceive;			//bit0:�ļ���ʼ���ͱ�־,Ϊ0û���ļ���ʼ����,Ϊ1���ļ��ڴ���
												//bit7:�ļ����ͽ�����־,Ϊ1���ͽ���
												//bit6:���һ�����ݲ�0��־,Ϊ1��ʾ�Ѿ���0
												//bit5:��ӡ������־,Ϊ1��ʾ��ͷ�Ѿ��ص�ԭ��λ��
												//bit4:�ļ�ɾ����־,Ϊ1��ʾɾ����ǰ�ļ�
												//bit3:��ϴ��ͷ��־,Ϊ1��ʾ��ϴ��ͷ
unsigned	char	gl_cLineWidth;						//�߿�,'S'ϸ(Ĭ��ֵ),'M'��,'L'��
unsigned	char	gl_cFilePrintStatus;						//�ļ���ӡ״̬,Ϊ0δ��ʼ��ӡ,Ϊ1���ڴ�ӡ
unsigned	char	gl_cCountFlag;							//=1��ʾ���յ��ֽ���;=0��ʾû��ʣ������

unsigned 	char	gl_cSprinklerCount;						//��ͷ��
unsigned	char	gl_cSprinklerNo;						//��Ч��ͷ��ʼ�ţ�0��3��
unsigned	char	gl_cBidirectional;						//˫���ӡ��־,Ϊ1˫���ӡ
unsigned	char	gl_cMultiDotLine12;					//1��2��ͷ֮����ص�������
unsigned	char	gl_cMultiDotLine23;					//2��3��ͷ֮����ص�������
unsigned	char	gl_cMultiDotLine34;					//3��4��ͷ֮����ص�������
unsigned	int		gl_iYDotSpace12;					//1��2��ͷ֮���Y������ĵ���
unsigned	int		gl_iYDotSpace23;					//2��3��ͷ֮���Y������ĵ���
unsigned	int		gl_iYDotSpace34;					//3��4��ͷ֮���Y������ĵ���
unsigned	int		gl_iGraphWidth;					//һ�����е��ֽ���(ͼ��)
unsigned	int		gl_iMaxPictureWidth;				//�ܽ��յ����ͼ�ο��
unsigned	int		gl_iMaxPictureLength;				//�ܽ��յ����ͼ�γ���
unsigned	int		gl_iLineDataNum;					//��ӡһ������Ҫ���յ��ֽ���
unsigned	int		gl_iReadDataAddr;					//дFIFO����ʱ��ǰSDRAM��ַ
unsigned	int		gl_iDataEndAddr;					//дFIFO����ʱSDRAM������ַ
unsigned	int		gl_iWriteFIFOAddr;					//����ͷFIFOд������ʼ��ַ
unsigned	int		gl_iDataEndAddr0;					//���ݿ�0������ַ
unsigned	int		gl_iDataEndAddr1;					//���ݿ�1������ַ
unsigned	int		gl_iRollPaper;						//��ֽʱ�������
unsigned	int		gl_iSendPaper;					//��ֽʱ�������

unsigned	char	gl_cReacterStatus;					//����ֽ״̬.bit0Ϊ1:��������ֽ,bit1Ϊ1��������ֽ
unsigned	char	gl_cInvertBufStatus;				//����ת��������״̬.bit0Ϊ1:���ڴ�ӡ���Ѿ���ӡ���ǻ�����1�е�����
																						//bit1Ϊ1��ʾ��ǰӦ׼�����ǻ�����1�е�����
																						//bit2Ϊ1��ʾ������0������׼����
																						//bit3Ϊ1��ʾ������1������׼����
unsigned	char	gl_cCurPrintBuf;					//Ϊ1:��ǰ׼����ӡ�����ڴ�ӡ���ǻ�����1�е�����
unsigned	char	gl_cPrintStatus;					//Ϊ0��ʾû�д�ӡ,Ϊ1��ʾY�����ڴ�ӡ,Ϊ2��ʾY���ӡ���� 3 ʱY��ԭ��
																						//Ϊ3��ʾX�����ڴ�ӡ,Ϊ4��ʾX���ӡ����
unsigned	char	gl_cSelfCheckFlag;					//��Ӧ���Լ��־,Ϊ0���Լ�,Ϊ1�Լ�
unsigned	char	gl_cRemainderBit;					//����ת��ʱʣ���λ��

unsigned	short	gl_sPlotType;					//����

double gl_dPictureErrorX;						//ͼ�����,X��������1000mm��Ӧ��ʵ�ʻ������
double gl_dPictureErrorY;						//ͼ�����,Y��������1000mm��Ӧ��ʵ�ʻ������
double gl_dErrorX;								//X����ֽ���(��λ:MM)
double gl_dErrorY;								//��������ɨ��֮�����MM��
int		gl_iStaggerDot;						//��������ɨ��֮����ĵ���
int		gl_iTHPulseNumX;						//X�����������ߵ���������û����������£�
int		gl_iPulseNumX;						//X������������
int		gl_iMovePaperLength;					//����ֽ����(��λ:������)
int		gl_iPulseNumY;						//Y������������
int		gl_iOriginalPos;						//ԭ��λ��(��λ:Pulse)
int		gl_iAxisYPos;							//Y������
long 	gl_lAxisXPos;							//X������
unsigned	int		gl_iMaxYMM;					//Y�᳤�ȣ���λ��MM��

unsigned	int		gl_iDelayCount;				//��ʱѭ������
unsigned	int		gl_iWaitCount;					//�ȴ���ӡѭ������
unsigned	int		gl_iWaitCount1;
unsigned	char	gl_cDelayFlag;						//�Ƿ���Ҫ��ӡ��ʱ��־

unsigned	int		gl_iRecDataLen;				//USB�ȴ��������ݳ���
unsigned	char	gl_cUsbSendBuf[120];				//USB���ͻ�����
unsigned	char	gl_cUsbSendBufPH;					//USB��������ͷָ��
unsigned	char	gl_cUsbSendBufPL;					//USB��������βָ��
unsigned	char	gl_cCommunicationMode;			//ͨѶ��ʽѡ��0:����������� 1:USB�������� 2:�����USB���ɽ�������
unsigned char  gl_scankeyflag;				//0:��ֹ����ɨ��20100223
unsigned	char	gl_cPrintWorkStatus;			// 20120711 ��ǰ����״̬ 0:����; 1:��ӡ 
unsigned	char 	gl_cCutWorkStatus;			//20120711 ��ǰ״̬0:�и��ļ�δ׼���ã�1:�и��ļ�׼����
unsigned char	gl_cMachineWorkStatus;		// 20120827 �豸����״̬ 0:����   1:����
unsigned char	gl_cKeyStart;				// 20120827 ����������ӡ
//������ȫ�ֶ�����
unsigned char gl_cCutPenNum;		//�ʵ�����
unsigned char gl_cCutUpDownState;	// ����״̬:	1,�������£�0,����̧��(ԭ���Ӧ��λ��)
unsigned char gl_cMovPaper;			//�Զ���ֽ
unsigned char gl_cPumpSwitch;		//������������
unsigned char gl_cManualPumpSwitch;		// �ֶ�������������20130104
unsigned char gl_cEndReturnRight;	//�����������½�
unsigned char gl_cOverCutOffPaper;	//��ӡ&�����ж�ֽ�� 20120906
unsigned short gl_sOverCutOffset;	// ƫ�ƶ��ٺ��׺��ж�ֽ�� 20120906
unsigned short gl_sMachineWidth;		//���Ϳ��
unsigned short gl_sMachineLenght;		//���ͳ���

unsigned	int		gl_iCurPictureWidth;				//��ǰͼ�ε����ͼ�ο��
unsigned	int		gl_iCurPictureLength;				//��ǰͼ�ε����ͼ�γ���

int gl_iCutDown[10];					//��ѹ
int gl_cSetPenAttr[10];			//�ʵ����� 0 pen 1 Cut 2 Cut*(ת���ᵶ)
int gl_iCutSpeed;					//�и��ٶ�
//int gl_iPrintSpeed;					//��ӡ�ٶ�
int gl_iMoveFramSpeed;				//�ƿ��ٶ�
int gl_iYMaxEffectivePMM;			// Y�����Ч��ӡ������ 20120822
int gl_iXMaxEffectivePMM;			// X�����Ч��ӡ������
int gl_iCutOffset;					//ƫ��
int gl_dCutExtensValue;			//�ᵶ����ֵ
int gl_dCutRotaExtenValue;		//ת���ᵶ����ֵ
int gl_iCutDistanceY;				//����:����1����ͷ֮��ľ��� ��λ���ص�
int gl_iCutDistanceX;			// X ��ƫ��

double gl_dXLengthCalibration;		//X����У��
double gl_dYLengthCalibration;  	//Y����У��
double gl_dCutPress;				//��ѹ����

double gl_dPosUnitX;                //X��ÿ��ͼ���Ե��߼���λ(ע�ⲻ�ǻ�ͼ�ǵĵ�λ)��Ӧ������ֵ
double gl_dPosUnitY;                //Y��ÿ��ͼ���Ե��߼���λ(ע�ⲻ�ǻ�ͼ�ǵĵ�λ)��Ӧ������ֵ
double gl_dXProp;                   //XY�����������
double gl_dYProp;
double gl_dPaperAdjust;				//ҳ��϶

int gl_iCutUpDelay;                 //̧���ӳ�,��λ:ms
int gl_iCutDownDelay;               //����ӳ�,��λ:ms

long gl_lMaxX;                      //�и��XY���ֵ 20130807 ��λ����
long gl_lMaxY;

unsigned char gl_cINLineNum;        //IN��ʼ��X������ֱ���ٶȽ��ͣ����⻭����ֱ��ʱ��ֽ����
//����
char gl_MacRunState; //0:��ֹ״̬��1:��ת״̬
char gl_DeleteFileState;  // 0:�ļ�û�б�ɾ��
// Z������
//int  gl_aZInductor[103] = {0,95,193,293,396,500,607,715,825,937,1051,1166,1283,1401,1520,1641,1762,1885,2008,2132,2257,2382,2507,2633,2758,2884,3009,3135,3260,3384,3507,3630,3752,3873,3993,4111,4228,4343,4457,4569,4679,4787,4892,4996,5097,5195,5290,5383,5473,5560,5644,5725,5802,5876,5947,6014,6077,6136,6192,6244,6292,6335,6375,6411,6442,6470,6493,6511,6526,6536,6542,6543,6540,6533,6522,6506,6485,6461,6432,6399,6362,6320,6275,6225,6172,6114,6053,5988,5919,5847,5771,5691,5609,5523,5434,5341,5246,5148,5048,4945,4839,4731,4621};

unsigned int gl_iStepSpeed[150];		//ÿ����ӡ�ٶ�
unsigned char gl_cMaxStep[61];			//ֱ����Ӧ������ٶȲ���

unsigned int gl_iStepSpeedX[150];		//ÿ����ӡ�ٶ�
unsigned char gl_cMaxStepX[61];			//ֱ����Ӧ������ٶȲ���
unsigned char gl_cStepX;

unsigned char gl_cAccStep[251];      //��������ӡʱÿ���߶ε���ʼ��ӡ�ٶ����ٶȱ��е�λ��
unsigned char gl_cMaxSpeed[251];     //ÿ���߶ν���ʱ������ٶȣ���Ҫ���г�����ӡ����������
unsigned char gl_cStep;							//�����ٲ���

int gl_iATime;                      //����ʱ��,��λ:ms
int gl_iInitV0;											//���ٶȣ���λ:mm/s

unsigned char gl_cCutFileSendFlag;   //0x0:���и������ļ���0x01���и��ļ����ȴ�����; 
								//0x02: �и��ļ��ȴ�����;0x03ִ���и����
long gl_lCurrCutFileLength;	//��ǰ�и��ļ����ݳ���

unsigned char  gl_cPenNo;  // 20101206 �ʺŵ���
unsigned char  gl_cCutNo;
unsigned char  gl_cPrintCutSelect; // 20101206 ��ӡ�и�ѡ��
int gl_iGapPixel; // ������

unsigned int gl_intoruncutmod = 0;     //���뺯��runcutmod�ı�־λ

//********20130807 New define
int  gl_iZInitSpeed;				//Z���ʼ�ٶ�
long gl_lZDriveSpeed;				//Z�������ٶ�  
long gl_lZAddSpeed;					//Z����ٶ�

int  gl_iUInitSpeed;				//U���ʼ�ٶ�
long gl_lUDriveSpeed;				//U�������ٶ�
long gl_lUAddSpeed;					//U����ٶ�
//*********��ԭ������************//
long 	gl_lCurPost_XPluse,	gl_lCurPost_YPluse;			//��ǰ��������
long	gl_lWorkOrig_XXPluse, gl_lWorkOrig_YYPluse ;	//����ԭ����������
long	gl_lPostMax_XPluse, gl_lPostMax_YPluse;			//�����������ֵ
//*******�豸���������
long	gl_lMachineWidthPluse, gl_lMachineLenghtPluse;		//�ܽ��ܵ����ͼ�γ��� ����
//*******��ǰ�ļ�����������
long	gl_lCurrFileLenghtPluse, gl_lCurrFileWidthPluse;			//��ǰ�ļ��ĳ��Ϳ� ����
//******�����ͺ�
int gl_iCurrMacType;			//��ǰ�����ͺ�
int	gl_iMacWide_MM;				//�����ͺ� ��� ����
int	gl_iMacLength_MM;

//********I0״̬
unsigned char gl_cSetIOStates;		// IO ״̬ Read:Bit0��X��ԭ��|Bit1��Y��ԭ��|Bit2��Z��ԭ�㣻���ࣺ������ֵΪ1���޼�⣻0�������Ч			
								    //         Write: Bit0�����ʣ�Bit1������� Bit2:LED �̵� Bit3:LED ���, ����:����
//*********�Զ���ֽ����*********//
long	RPGL_lPaperOffsetValue;	// ֽƫ��ֵ
long	RPGL_iPaperSpeedValue; 	// 1~3,������ֽ�ٶȵ�ʱ��*100����1000,��ʾȡ0~2
long	RPGL_iPauseTimeValue;	// ��ͣʱ�䣬��ӡ������ͣһ��ʱ�����ֽ ?????????
long	RPGL_iAutoCutOff;		// �Զ��и� ?????? ON ���ֽ�� OFF2 ����ϣ��������
//******************************//
//******�����������½�
char gl_cOverReturnState;	//���귵�����½�  1:ON ���� / 0:OFF ������
char gl_cGoOOPointState;	// 1: ����PU0,0ʱ�����ƶ������½ǣ� 0:������
char gl_cStopWorkState;		// 1ֹͣ����
//*****************************//
char gl_cKeyDirectPost;		//������Ŀ���ַ

long gl_rate =1.0; //����xy�ٶȱ���//��ûʹ��
int gl_iSVSpeed, gl_iVSpeed, gl_iTimeAdd;		//�ٶ�Y
int gl_iSVSpeedX, gl_iVSpeedX, gl_iTimeAddX;    //X
//*********
int gl_iTempATime;		//TEST
char cReviceFlag;	//���ݿ��Խ��չ�����־
long lXlimitNum;	// X�Ἣ��
char gl_cLanguageSelect; //����ѡ�� 0 ����  1Ӣ��
char gl_cMenu[12][17];
char gl_cPopMenu[11][17];
char gl_cViewErrMenu[18][17];		//20141202 rwt
int gl_iCutSectionLen;		//�и�ֶγ���
int gl_iCutSectionGap;		//�и�ֶμ�϶
double gl_dMovePaperError_X, gl_dMovePaperError_Y;  //��ֽ���У��
char gl_cAutoSendPaperSet;	//�Զ���ֽ
// ԭ��ÿ���ļ���ʼ��ӡǰ��Ҫ��ϴ�������޸�Ϊ���ж೤ʱ����ڴ�ӡ�ļ�����ϴ
char gl_cStartWorkFlag;		// 2014141021 �ļ���ʼ������־
long int gl_lSWFCount;		// 2014141021 �ļ�����������ʱ��
char gl_cSWFStartClearFlag;	// 2014141021 �´δ�ӡ�ļ�ʱ������ӡͷ��ϴ
char gl_cPHCDTime;			// 2014141021 ��ϴ��ʱʱ��
char gl_cSendPaperStar;		// 2014141024 �Զ���ֽ��ͣ

