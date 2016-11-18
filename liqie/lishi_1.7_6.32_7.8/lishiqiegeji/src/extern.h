#ifndef __EXTERN_H__
#define __EXTERN_H__

#include "..\src\defconst.h"

extern int gl_time;
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
extern unsigned int 	gl_iMaxPictureLength;
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
extern long 		gl_lAxisXPos;												//X������
extern unsigned	int		gl_iMaxYMM;									//Y�᳤�ȣ���λ��MM��


extern unsigned	int		gl_iDelayCount;
extern unsigned	int		gl_iWaitCount;
extern unsigned	int		gl_iWaitCount1;
extern unsigned	char	gl_cDelayFlag;

extern unsigned	int		gl_iRecDataLen;
extern unsigned	char	gl_cUsbSendBuf[100];
extern unsigned	char	gl_cUsbSendBufPH;
extern unsigned	char	gl_cUsbSendBufPL;
extern unsigned	char	gl_cCommunicationMode;
extern unsigned 	char  gl_scankeyflag;//0:��ֹ����ɨ��
extern unsigned	char	gl_cPrintWorkStatus;			// 20120711 ��ǰ����״̬ 0:����; 1:��ӡ 
extern unsigned	char 	gl_cCutWorkStatus;			//20120711 ��ǰ״̬0:�и��ļ�δ׼���ã�1:�и��ļ�׼����
extern unsigned char	gl_cMachineWorkStatus;		// 20120827 �豸����״̬
extern unsigned char	gl_cKeyStart;				// 20120827 ����������ӡ
//������ȫ�ֶ�����
extern unsigned char gl_cCutPenNum;		//�ʵ�����
extern unsigned char gl_cCutUpDownState;	// ����״̬:	1,�������£�0,����̧��(ԭ���Ӧ��λ��)
extern unsigned char gl_cMovPaper;			//�Զ���ֽ
extern unsigned char gl_cPumpSwitch;		//������������
extern unsigned char gl_cEndReturnRight;	//�����������½�
extern unsigned char gl_cOverCutOffPaper;	//��ӡ&�����ж�ֽ�� 20120906
extern unsigned short gl_sOverCutOffset;	// ƫ�ƶ��ٺ��׺��ж�ֽ�� 20120906
extern unsigned short gl_sMachineWidth;		//���Ϳ��
extern unsigned short gl_sMachineLenght;		//���ͳ���
extern int gl_iCutDown[10];					//��ѹ
extern int gl_cSetPenAttr[10];			//�ʵ����� 0 pen 1 Cut 2 Cut*(ת���ᵶ)
extern int gl_iCutSpeed;					//�и��ٶ�
//extern int gl_iPrintSpeed;					//��ӡ�ٶ�
extern int gl_iMoveFramSpeed;				//�ƿ��ٶ�
extern int gl_iCutOffset;					//ƫ��
extern int gl_iYMaxEffectivePMM;			// Y�����Ч��ӡ������ 20120822
extern int gl_iXMaxEffectivePMM;			// X�����Ч��ӡ������
extern int gl_dCutExtensValue;			//�ᵶ����ֵ
extern int gl_dCutRotaExtenValue;		//ת���ᵶ����ֵ

extern int  gl_iCutDistanceY;				//����
extern int  gl_iCutDistanceX;			// X ��ƫ��

extern double gl_dXLengthCalibration;		//X����У��
extern double gl_dYLengthCalibration;  	//Y����У��
extern double gl_dCutPress;				//��ѹ����

extern double gl_dPosUnitX;                //X��ÿ��ͼ���Ե��߼���λ(ע�ⲻ�ǻ�ͼ�ǵĵ�λ)��Ӧ������ֵ
extern double gl_dPosUnitY;                //Y��ÿ��ͼ���Ե��߼���λ(ע�ⲻ�ǻ�ͼ�ǵĵ�λ)��Ӧ������ֵ
extern double gl_dXProp;                   //XY�����������
extern double gl_dYProp;
//����20100623
//����
extern char gl_MacRunState; //0:��ֹ״̬��1:��ת״̬
extern char language[100][30] ;//��������
extern int  gl_aZInductor[103];

extern unsigned int gl_iStepSpeed[150];		//ÿ����ӡ�ٶ�
extern unsigned char gl_cMaxStep[61];			//ֱ����Ӧ������ٶȲ���

extern unsigned int gl_iStepSpeedX[150];		//ÿ����ӡ�ٶ�
extern unsigned char gl_cMaxStepX[61];			//ֱ����Ӧ������ٶȲ���
extern unsigned char gl_cStepX;

extern unsigned char gl_cAccStep[251];      //��������ӡʱÿ���߶ε���ʼ��ӡ�ٶ����ٶȱ��е�λ��
extern unsigned char gl_cMaxSpeed[251];     //ÿ���߶ν���ʱ������ٶȣ���Ҫ���г�����ӡ����������
extern unsigned char gl_cStep;							//�����ٲ���
extern unsigned char gl_cManualPumpSwitch;		// �ֶ�������������20130104
extern char gl_cMenu[10][17];
extern char gl_cPopMenu[10][17];
extern char gl_cViewErrMenu[18][17];
extern int gl_iATime;                      //����ʱ��,��λ:ms
extern int gl_iInitV0;	
extern unsigned char gl_cCutFileSendFlag;   //�и������ļ������־1:�����и������ļ�
extern long gl_lCurrCutFileLength;	//��ǰ�и��ļ�����
extern int gl_iCutUpDelay;                 //̧���ӳ�,��λ:ms
extern int gl_iCutDownDelay;               //����ӳ�,��λ:ms

extern long gl_lMaxX;                      //�и��XY���ֵ
extern long gl_lMaxY;
extern double gl_dMovePaperError_X, gl_dMovePaperError_Y;  //��ֽ���У��
extern double gl_dPaperAdjust;				//ҳ��϶

extern unsigned char gl_cINLineNum;        //IN��ʼ��X������ֱ���ٶȽ��ͣ����⻭����ֱ��ʱ��ֽ����
extern char gl_DeleteFileState;  // 0:�ļ�û�б�ɾ��
extern unsigned	int		gl_iCurPictureWidth;				//��ǰͼ�ε����ͼ�ο��
extern unsigned	int		gl_iCurPictureLength;				//��ǰͼ�ε����ͼ�γ���
extern unsigned char  gl_cPenNo;  // 20101206 �ʺŵ���
extern unsigned char  gl_cCutNo;
extern unsigned char  gl_cPrintCutSelect; // 20101206 ��ӡ�и�ѡ��
extern int gl_iGapPixel; // ������
extern char gl_cKeyDirectPost;		//������Ŀ���ַ
extern int gl_iTempATime;		//TEST
extern char cReviceFlag;	//���ݿ��Խ��չ�����־
extern long lXlimitNum;	// X�Ἣ��
extern char gl_cSendPaperStar;		// 2014141024 �Զ���ֽ��ͣ

extern int gl_iSVSpeed, gl_iVSpeed, gl_iTimeAdd;		//�ٶ�Y
extern int gl_iSVSpeedX, gl_iVSpeedX, gl_iTimeAddX;    //X
extern void InitSpeedCurve(void);
extern int UpdataAccTable(int iInitV, int iPenSpeed, int iAccTime, unsigned int *iStepSpeed, unsigned char *cMaxStep);
//extern int NewUpdataAccTable(int iInitV, int iPenSpeed, int iAccTime, unsigned int *iStepSpeed, unsigned char *cMaxStep);
//����
extern 	Putch;
extern 	Putchar;
extern	Puts;	
extern 	void Printf(char *f, ...);						//�����ն����
extern 	void  Scanf(char *f, void  *argP);
extern 	void  FloatPrecision(char *buf, int decimalLen);
extern 	void SetLDPenSpeed(void);
extern 	int GetPenSpeed(void);
extern 	void SetSDPenSpeed(void);
extern 	void WaitPrintEnd(void);
extern 	void set_startv(int axis,int wdata);
extern 	void set_speed(int axis,int wdata);
extern 	void pmove(int axis,long pulse);
extern 	void	get_status(int axis,int *value);
extern 	void inp_move2(long pulse1,long pulse2);
extern  void SelecAxis(int axis);
extern 	int ReadAxisStatus(int axis);
extern  int ReadPrintEnd(void);
extern 	unsigned char GetIOBit(unsigned char cBit);
extern 	long IntRound(double x);
extern 	void WriteCommand(int axis, char cmd);
extern 	int ReadData(void);
extern 	int ReadRegister(int  cReg);
extern  void SuddenStop(int axis);		//   312��ͣ
extern  void PrintReturnZero(void);
//extern  int ReadPrintEnd(int axis);
extern  void WaitDriveEnd(int axis);
extern  void SetMovFrameSpeed(int iSpeed);  // 20120906 �����ƿ��ٶ�
extern void SetAxisPrintSpeed(int iAxis, int iSpeed); // 20130317
extern void SetLp(int axis, long wdata);
extern long ReadLp(int axis);
extern void ViewError(int iErrorCode);

extern  void SHOW_data(int type ) ;//CutDataAddr
//***********

//********20130807 New define
extern char gl_cStartWorkFlag;		// 2014141021 �ļ���ʼ������־
extern long int gl_lSWFCount;		// 2014141021 �ļ�����������ʱ��
extern char gl_cSWFStartClearFlag;	// 2014141021 �´δ�ӡ�ļ�ʱ������ӡͷ��ϴ
extern char gl_cPHCDTime;			// 2014141021 ��ϴ��ʱʱ��

extern int  gl_iZInitSpeed;				//Z���ʼ�ٶ�
extern long gl_lZDriveSpeed;				//Z�������ٶ�  
extern long gl_lZAddSpeed;					//Z����ٶ�

extern int  gl_iUInitSpeed;				//U���ʼ�ٶ�
extern long gl_lUDriveSpeed;				//U�������ٶ�
extern long gl_lUAddSpeed;					//U����ٶ�
//*********��ԭ������************//
extern long gl_lCurPost_XPluse,	gl_lCurPost_YPluse;			//��ǰ��������
extern long	gl_lWorkOrig_XXPluse, gl_lWorkOrig_YYPluse ;	//����ԭ����������
extern long	gl_lPostMax_XPluse, gl_lPostMax_YPluse;			//�����������ֵ
//*******�豸���������
extern long	gl_lMachineWidthPluse, gl_lMachineLenghtPluse;		//�ܽ��ܵ����ͼ�γ��� ����
//*******��ǰ�ļ�����������
extern long	gl_lCurrFileLenghtPluse, gl_lCurrFileWidthPluse;			//��ǰ�ļ��ĳ��Ϳ� ����
//******�����ͺ�
extern int gl_iCurrMacType;			//��ǰ�����ͺ�
extern int	gl_iMacWide_MM;				//�����ͺ� ��� ����
extern int	gl_iMacLength_MM;
//******�ƿ��ٶ�
//*********�Զ���ֽ����*********//
extern long	RPGL_lPaperOffsetValue;	// ֽƫ��ֵ
extern long	RPGL_iPaperSpeedValue; 	// 1~3,������ֽ�ٶȵ�ʱ��*100����1000,��ʾȡ0~2
extern long	RPGL_iPauseTimeValue;	// ��ͣʱ�䣬��ӡ������ͣһ��ʱ�����ֽ ?????????
extern long	RPGL_iAutoCutOff;		// �Զ��и� ?????? ON ���ֽ�� OFF2 ����ϣ��������

//******�����������½�
extern char gl_cOverReturnState;	//���귵�����½�  1:ON ���� / 0:OFF ������
extern char gl_cGoOOPointState;	// 1: ����PU0,0ʱ�����ƶ������½ǣ� 0:������
extern char gl_cStopWorkState;		// 1ֹͣ����
extern char gl_cLanguageSelect;
extern char gl_cAutoSendPaperSet;	//�Զ���ֽ

extern int gl_iCutSectionLen;		//�и�ֶγ���
extern int gl_iCutSectionGap;		//�и�ֶμ�϶

//
extern void SetMoveFrameSpeed(int iAxis);			//�����ƿ��ٶ�
extern void SetPrintSpeed(int iAxis);				//���ô�ӡ�ٶ�
extern void CornerGriffe(long iFx, long iFy, long iBx, long iBy);
extern void SetReturnOrgSpeed(void);
extern void SetContinInpCutSpeed(int i);//int iAXIS
//extern void SetSortDistInpCutSpeed(void);
extern void NetSetManage(void);

extern void SingleAxisInpDriveSpeed(int iAxis, int iSV, int iV, int iAddTime);
//extern void TwoAxleInpDriveSpeed(int iSV, int iV, int iAddTime);
extern void StraightLineADSpeed(int iAxis, int iSV, int iV, int iAddTime);
extern void FixedSpeeDrive(int iAxis, int iSpeed);
extern void ArrCpy(int Dest[], int Sour[], int len);

//******************************//
extern void RW_MoveStartPrintPost(void);
extern void SetAxisSpeed(int iSpeed);
extern void WaitInpEnd(void);
extern int IsKeyBound(unsigned char cKeyValue);
extern void SetUAxisInit(int Ispeed, long Dspeed, long Aspeed);
extern void lcd_GoLineRowDisp(char cRow, char cLine, char cDispbuf[]);
extern void ManualSwtichPump(void);
extern void OpenPump(void);
extern void ClosePump(void);							
extern void SetInitAxisSpeed(char cAix, int iSpeed);
extern void RP_SetDoubleAxisADSpeed(int iSpeed);
extern void RP_SetSingleAxisADSpeed(char cAix, int iSpeed);
extern void RestMcx312(void);
extern int KeyEffectState(unsigned char cKeyValue);


//*****************************//

typedef struct
{
	//unsigned char cChinaEn;
	unsigned char cSprinklerCount;
	unsigned char cSprinklerNo;
	unsigned char cBidirectional;
	unsigned char cLineWidth;
	unsigned char cMultiDotLine12;
	//unsigned char cMultiDotLine23;
	//unsigned char cMultiDotLine34;
	unsigned char cPenNo;	// 20101206  �ʺŵ���
	unsigned char cCutNo;
	unsigned char cPrintCutSelect;// 20101206 ��ӡ�и�ѡ��
	unsigned short MyIP[2];
	unsigned short SubnetMask[2];
	unsigned short GatewayIP[2];
	unsigned short sPlotType;
	unsigned short sNull1;
	//int	iOriginalPos;
	int	iYDotSpace12;
	int	iYDotSpace23;
	int	iYDotSpace34;
	int iSNUll;
	double dErrorX;
	double dErrorY;
	double dPictureErrorX;
	double dPictureErrorY;
}PrintSetPara;

//20100622
typedef struct
{ 
	
	//unsigned char cCutPenNum;		//�ʵ�����
	unsigned char cMovPaper;			//�Զ���ֽ
	unsigned char cPumpSwitch;		//������������
	unsigned char cEndReturnRight;	//�����������½�
	unsigned char cOverCutOffPaper; //��ӡ&�����ж�ֽ�� 20120906
	unsigned char cPHCDTime;		// 20141021 
	//**********20130809
	char cLanguageSelect;		// ����ѡ��
		//���귵�����½�  1:ON ���� / 0:OFF ������
	char cOverReturnState;
	// 1: ����PU0,0ʱ�����ƶ������½ǣ� 0:������
	char cGoOOPointState;
	// �Զ���ֽ
	char cAutoSendPaperSet;
	//��ӡ�����Ƿ��Զ��ж�
	int iAutoCutOff;
	//U���ٶȵȼ�
	int iUAxisSpeedInit;
	// ����������ͣʱ��
	int iPauseTimeValue;

	int	iOriginalPos;
	//��ֽƫ��
	long lPaperOffsetValue;
	//***********
	unsigned short sOverCutOffset;  // ƫ�ƺ����ж�ֽ�� 20120906
	unsigned short sMachineWidth;		//���Ϳ��
	unsigned short sMachineLenght;		//���ͳ���
	int iCutSpeed;					//�и��ٶ�
	//int iPrintSpeed;					//��ӡ�ٶ�
	int iMoveFramSpeed;				//�ƿ��ٶ�
	int iCutOffset;					//ƫ��
	int iCutDown[10];					//��ѹ	
	int cSetPenAttr[10];
	//int iOriginalPos_X;			//ԭ������X,��λmm
	//int iOriginalPos_Y;			//ԭ������Y,��λmm
	int iCutUpDelay;                 //̧���ӳ�,��λ:ms
	int iCutDownDelay;               //����ӳ�,��λ:ms

	int iCutSectionLen;				//�и�ֶγ���
	int iCutSectionGap;				//�и�ֶμ�϶
	
	int  iZInitSpeed;
	long lZDriveSpeed;			// 20120808
	long lZAddSpeed;

	int  iUInitSpeed;
	long lUDriveSpeed;
	long lUAddSpeed;
	
	//long lWorkOrig_YYPluse;			//ԭ����������
	//long lWorkOrig_XXPluse;
	
	int dCutExtensValue;			//�ᵶ����ֵ
	int dCutRotaExtenValue;		//ת���ᵶ����ֵ
	
	int iCutDistanceY;				//����
	int iCutDistanceX;				// X ���߾��ȵ���
	
	double dXLengthCalibration;		//X����У��
	double dYLengthCalibration;  	//Y����У��
	double dCutPress;				//��ѹ����
	double dXProp;				////XY�����������
	double dYProp;
	double dPosUnitX;				//XY��ÿ��ͼ���Ե��߼���λ(ע�ⲻ�ǻ�ͼ�ǵĵ�λ)��Ӧ������ֵ
	double dPosUnitY;
	double dMovePaperError_X;
	double dMovePaperError_Y;
	double dPaperAdjust;		//ҳ��϶

	int cTime	; //��ͷ��īʱ��
}CutSetPara;

#endif
