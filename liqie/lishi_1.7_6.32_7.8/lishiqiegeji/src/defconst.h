#ifndef __DEFCONST_H__
#define __DEFCONST_H__

#include "..\target\44b.h"

#define MCLK 							64000000

#define	SectorSize				0x800			//2K word
#define	SetParameterAddr	0x1ff000

#define UartRecBufLength	20				//���ڽ��ջ���������
#define UartSendBufLength 8					//���ڷ��ͻ���������
#define	EthnetBufLength		0x1800000	//0x1000000	//���ݽ��ջ���������16M	1677216B
#define	BEGINADDR			0x0c300000	//0x0c700000	//��ӡ�������ݻ�������ʼ��ַ(16M)
#define	InvertDataAddr0		0x0db00000	//0x0d800000	//��ӡ����ת����0    ��ʼ��ַ(2M)
#define	InvertDataAddr1		0x0dc00000	//0x0db00000	//��ӡ����ת����1 	��ʼ��ַ(2M)
#define CutDataAddr			0x0dd00000	//0x0de00000	//�и����ݴ洢����ʼ��ַ0xDE00000~0xDFFFFFF(2M)  20100909
#define	CutDataLength		0x100000	//�и��ļ�����������
#define	KeyBufLength			20				//��������������
#define	SCANTIMES1				2 //3					//3��ɨ���ֵһ����֤���ü�����
#define	SCANTIMES2				33//16//33				//50��ɨ��ü�һֱ�����£��ظ���ֵһ��


//�ٶȲ����趨
#define Print_SV	100   //100
#define Print_V		500 //500
#define	Print_T		400

#define	Frame_SV	10

#define  CUTPRESSMAX     150   //��ѹ���ֵ
												//169.7652726
#define X_PPMM						166.935851	//169.140989//169.1638171////169.040989//200//611.845321//200////83.33333//67.66552	//ÿ���׶�Ӧ��ͼ��X���������,X����ÿ1600������תһȦ
#define Y_PPMM						166.6667//167.256///111.11113//185.185185//611.845321//192.30769////83.33333//111.11113//83.33335//166.6667//416.66675 //ÿ���׶�Ӧ��ͼ��Y���������,Y����ÿ4000������תһȦ
//********20130807 Z/U AXIS Pluse define
#define Z_PPMM						10//9.91268//31.847133//4.218634	// Z��ÿ����λ������ ÿ����������(4000/((2*2*3.14)*10) = 31.847133)  (20*31.847133)/151 = 4.218634
#define U_PPMM						100	  			// ��ֽ���û����������
//*************

#define BASE_PPM    600//600

//*************20130807Z��һȦ��������
#define RP_ZAXISLAPPLUS   8000						//Z��һȦ��������

#define HPGL_MM		0.025						//ÿ��ͼ���Ե��߼���λ(ע�ⲻ�ǻ�ͼ�ǵĵ�λ)��Ӧ�ĺ���ֵ
#define	AdditionDot				80//120//1000//16//356//16	 //Ϊ�˵�����������ɨ��֮��Ĵ������ӡ�ĵ���
#define	OriginalSideDot			316//240//1030			//Ϊ�˱����λԭ�����ߵĵ���(13cm)
#define	MotorSideDot			316//240//555				//Ϊ�˱����λ�������ߵĵ���(7cm) ����һ�����1/7.9 = 0.1265

#define LeftMargin				(90*Y_PPMM)			//�ʳ���߾�190
#define PenCarSideInterval		(AdditionDot + OriginalSideDot)			//�ʳ���Ԥ����� 50mm 20130830

#define 	SPEED_M		40//40		//����16��Χ500K    20//�ٶȱ���,��160000PPS,������20    20100622

#define	ReadIOAddr				0xA000008	//��IO�����ݵ�ַ
#define	WriteIOAddr				0xA00000C	//дIO�����ݵ�ַ
#define  KEYSCANADDR   (*(volatile unsigned *)0xa000010) //����ɨ���д��ַ20100220
#define WriteLcdComAddr		0xA000004	//дҺ�������ַ
#define WriteLcdDataAddr	0xA000000	//дҺ�����ݵ�ַ
//#define	WritePulseAddrX		0x8000004	//X������д��������ַkk
//#define	WritePulseAddrY		0x8000000	//Y������д��������ַkk
#define	WriteFIFOAddr			0x8000000	//дFIFO���ݵ�ַ
#define WriteFIFOFullAddr		0x8000400	//0x8000404	//дFIFO����ַ  ������Ч��ͷ��
#define WriteFileAddr			0x8000404	//0x8000400	//д�ļ���ʼ��ӡ��ַ
#define	WriteDirctAddr			0x800040C	// д��ӡ����
#define ParaFirstAddr			0xA000018	//����������ʼ��ַ
#define	ParaEndAddr				0xA0001FC	//�������������ַ
#define ParaEraseAddr			0xA000010	//����������ַ
#define	ParaWREndAddr			0xA000014	//��������д������־��ַ
#define	WRCH375ComAddr		0x2000002	//CH375д�����ַ
#define WRCh375DataAddr		0x2000000	//CH375д���ݵ�ַ
#define RDCH375DataAddr		0x2000000	//CH375�����ݵ�ַ

#define  WRITE312ADDR			0x8000408//0x8000120//��īʱ��õ�ַдY������������
#define  NULL312ADDR				0x8000124// ����������
#define  Z_AxisSendPulseAddr		0x8000140// Z��������д���ַ
#define  Z_AxisInitSpeedAddr		0x8000144// Z���ʼ�ٶ�д���ַ
#define  Z_AxisDriveSpeedAddr		0x8000148// Z�������ٶ�д���ַ
#define  Z_AxisAddSpeedAddr		0x800014C	// Z����ٶ�д���ַ
#define  Z_AxisCmdAddr			0x8000208	//0x8000200// Z������д���ַ
//U��������ַ
#define  U_AxisSendPulseAddr	0x8000160	// U��������д���ַ
#define  U_AxisInitSpeedAddr	0x8000164	// U���ʼ�ٶ�д���ַ
#define  U_AxisDriveSpeedAddr	0x8000168	// U�������ٶ�д���ַ
#define  U_AxisAddSpeedAddr		0x800016C	// U����ٶ�д���ַ
#define  U_AxisCmdAddr			0x800020C	// U������д���ַ
//**************************************************
#define WritePrintHeadCleanAddr    0x8000410  // ��ӡͷ��ϴ��ַ20110414
//MCX312 define const
#define	WR0		0x00			//Command register
#define	WR1		(0x02)			//Mode register 1
#define	WR2		(0x04)			//Mode register 2
#define	WR3		(0x06)			//Mode register 3
#define	WR4		(0x08)			//Output register
#define	WR5		(0x0a)			//Interpolation mode register
#define	WR6		(0x0c)			//Low word bits data writing register
#define	WR7		(0x0e)			//High word bits data writing register

#define	RR0		0x00			//Main status register
#define	RR1		(0x02)			//Status register 1
#define	RR2		(0x04)			//Status register 2
#define	RR3		(0x06)			//Status register 3
#define	RR4		(0x08)			//Input register 1
#define	RR5		(0x0a)			//Input register 2
#define	RR6		(0x0c)			//Low word bits data reading register
#define	RR7		(0x0e)			//High word bits data reading register

#define	BP1P	(0x04)			//BP+ direction data register for the X axis control
#define	BP1M	(0x06)			//BP- direction data register for the X axis control
#define	BP2P	(0x08)			//BP+ direction data register for the Y axis control
#define	BP2M	(0x0a)			//BP- direction data register for the Y axis control							
#define 	AXIS_X		1							//X��
#define 	AXIS_Y		2							//Y��
#define 	AXIS_Z		3							//Z��	20130807

//***************��Ӧ������*************************
#define INDUCTOR_XRIGHT		0						//X �Ҹ�Ӧ��
#define INDUCTOR_YUP		2						//Y �ϸ�Ӧ��
#define INDUCTOR_ZORG		4						//Z ��Ӧ��

#define Z_INDUCTOR_ORIGIN	0x00					// Z���Ӧ����ԭ��
#define XY_INDUCTOR_ORIGIN  0x01					// X/Y���Ӧ����ԭ��
//**************************************************
//*************20130807U������
#define   WriteUAxisPlus(dat)			*((volatile  int *)(U_AxisSendPulseAddr))=dat	//����������
#define   ReadUAxisPlus()				*((volatile  int *)(U_AxisSendPulseAddr))		//������������
#define	  WriteUAxisInitSpeed(dat)		*((volatile  int *)(U_AxisInitSpeedAddr))=dat		//����Z��ʼ�ٶ�
#define   WriteUAxisDriveSpeed(dat)		*((volatile  int *)(U_AxisDriveSpeedAddr))=dat	//����Z�����ٶ�
#define   WriteUAxisAddSpeed(dat)		*((volatile  int *)(U_AxisAddSpeedAddr))=dat	//����Z���ٶ�
#define   WriteUAxisDriveCmd(cmd)		*((volatile  int *)(U_AxisCmdAddr))=cmd		//����Z��������
#define	  UAxisDriveEndState			((rPDATD & 0x0020) == 0x0020 ? 1 : 0)		//��Z������״̬ ��־,1����
#define   DriveUAxisStartCmd			0x5555
#define   DriveUAxisStopCmd				0x5555
//************���Զ����������
#define   ReadEncoderA()					*((volatile unsigned int *)(0x0800010c))
#define   ReadEncoderB()					*((volatile unsigned int *)(0x08000110))

//****************
#define	Writeflash(addr,dat)	*((volatile unsigned short *)(addr<<1))=(unsigned short)dat
#define	Readflash(addr)				(*((volatile unsigned short *)(addr<<1)))
#define	WriteIO(dat)					*((volatile unsigned char *)(WriteIOAddr))=(unsigned char)dat
#define	ReadIO()							(*((volatile unsigned char *)(ReadIOAddr)))
#define	WriteLcdCommand(dat)	*((volatile unsigned char *)(WriteLcdComAddr))=(unsigned char)dat;Delay(50)
#define	WriteLcdData(dat)			*((volatile unsigned char *)(WriteLcdDataAddr))=(unsigned char)dat;Delay(50)
#define Writesdram(addr,dat)	*((volatile unsigned short *)(addr))=(unsigned short)dat
#define Writesdram8bit(addr,dat)	*((volatile unsigned char *)(addr))=(unsigned char)dat
#define Readsdram(addr, len)		  	((len== 16) ? (*((volatile unsigned short *)(addr))) :  (len==32) ?(*((volatile unsigned int *)(addr))) : (*((volatile unsigned char *)(addr))))

//#define WritePulseX(dat)			*((volatile short *)(WritePulseAddrX))=(short)dat
//#define WritePulseY(dat)			*((volatile short *)(WritePulseAddrY))=(short)dat
//дFIFOԭ��16λ��������Ϊһ��д32λ20100813
#define	WriteFIFO(addr,dat)		*((volatile unsigned int *)(addr))=dat
#define	ReadFIFO(addr)		*((volatile unsigned int *)(addr))

#define   PrintHeadClean(cout)			*((volatile unsigned short *)(WritePrintHeadCleanAddr))=0x03; Delay(cout*1000);Printf("cout === %d\n", cout);*((volatile unsigned short *)(WritePrintHeadCleanAddr))=0x0
//******
#define WriteFIFOFull(dat)		*((volatile unsigned int *)(WriteFIFOFullAddr))=(unsigned int)dat
#define	WriteFilePrint(dat)		*((volatile unsigned int *)(WriteFileAddr))=(unsigned int)dat
#define WriteDirct(dat)			*((volatile unsigned int *)(WriteDirctAddr))=(short int)dat
#define ReadParaData(addr)		(*((volatile unsigned char *)(addr)))
#define	ReadParaFlag()				(*((volatile unsigned char *)(ParaWREndAddr)))
#define	WriteParaData(addr,dat)	*((volatile unsigned char *)(addr))=(unsigned char)dat
#define WriteCH375Command(dat)	*((volatile unsigned char *)(WRCH375ComAddr))=(unsigned char)dat
#define WriteCH375Data(dat)		*((volatile unsigned char *)(WRCh375DataAddr))=(unsigned char)dat
#define ReadCH375Data()				(*((volatile unsigned char *)(RDCH375DataAddr)))

#define   Write312YAxisPlus(dat)	*((volatile  int *)(WRITE312ADDR))=dat	//д312YʱҪ��õ�ַд��Y������
#define   WriteZAxisPlus(dat)			*((volatile  int *)(Z_AxisSendPulseAddr))=dat	//����������
#define   ReadZAxisPlus()			*((volatile  int *)(Z_AxisSendPulseAddr))		//������������
#define	WriteZAxisInitSpeed(dat)		*((volatile  int *)(Z_AxisInitSpeedAddr))=dat		//����Z��ʼ�ٶ�
#define   WriteZAxisDriveSpeed(dat)		*((volatile  int *)(Z_AxisDriveSpeedAddr))=dat	//����Z�����ٶ�
#define   WriteZAxisAddSpeed(dat)		*((volatile  int *)(Z_AxisAddSpeedAddr))=dat	//����Z���ٶ�
#define   WriteZAxisDriveCmd(cmd)			*((volatile  int *)(Z_AxisCmdAddr))=cmd		//����Z��������
#define   Read312YAxisPlus()		*((volatile  int *)(WRITE312ADDR))	//д312YʱҪ��õ�ַд��Y������
#define   ReadF312()				*((volatile  int *)(NULL312ADDR))
#define	WriteDL16							rPDATC &= 0xffef  //ƽ���и��δ��
#define	WriteDH16							rPDATC |= 0x0010  //ƽ���и��δ��

#define	DrivePrint					rPDATD &= 0xffbf; \
												rPDATD |= 0x040;
#define	ReadFIFOFull()				(rPDATD & 0x0010)	//GPC0ΪFIFO��״̬��־λ��Ϊ1��
//#define ReadPrintEnd()				(rPDATD & 0x08) 	// �ж�312����״̬�Ƿ�æ 0æ 1���� û��

#define	ClrFIFO								rPDATD |= 0x080; \
															rPDATD &= 0xff7f	//GPC1Ϊ��FIFO����λ,Ϊ1��FIFO
#define	ZAxisDriveEndState				((rPDATD & 0x0020) == 0x0020 ? 1 : 0)		//��Z������״̬ ��־,1����
#define   DriveZAxisStartCmd				0x5555
#define   DriveZAxisStopCmd				0x5555
//#define PixelY						300					//Y������(��λ:����/Ӣ��)
#define	SprinklerWidth		150					//��λbit
#define DotPulseNum				21  //75.5997//23.3333333//77.09251101//14		//һ�����ص������������ 1200*600/9600
#define DotLineWidth			0.08467			//һ��������ռ�Ŀ��
#define	SprinklerSpace		207//310					//��ͷ֮����ľ���(��λbit)
#define	MaxMalposedDot		20					//��ͷ֮�����������
//#define Order_Start				0xf5
//#define Order_LineWidth		0x01
//20100623��ֵ����
#define 	Key_F1						'1'
#define 	Key_F2						'2'
#define 	Key_F3						'3'
#define 	Key_F4						'4'
#define 	Key_Ok    					'5'
#define 	Key_F6						'6'
#define 	Key_Pause					'7'
#define 	Key_F8						'8'
#define 	Key_F9						'9'	

#define		Key_F5						'a'
#define 	Key_F7						'b'

// û��ʹ��
#define 	Key_Pump    				'd'
#define 	Key_Test					'e'
#define 	Key_Drop					'f'
#define 	Key_Speed					'g'
#define 	Key_Parameter				'h'	
#define		Key_Up						'i'
#define		Key_Down					'j'
#define		Key_Left					'k'
#define		Key_Right					'l'

//*********
#define LCD_Line0					0x01				//LCD��0�з��ױ�־��1�ѷ��ף�
#define LCD_Line1					0x02				//LCD��1�з��ױ�־��1�ѷ��ף�

#define ER_INITPORT				0						//��ʼ�����ڳ���
#define ER_FRAME					1						//֡����
#define ER_PARITY					2						//У���
#define ER_COMMAND					3						//��ͼ�����
#define ER_XOVER					4						//X���ƶ�����
#define ER_YOVER					5						//Y���ƶ�����
#define ER_YOBAD					6						//Y��ԭ�㴫������
#define	ER_SRAM						7						//sram�Լ����
#define ER_POSITIONX			9						//X���ӡλ��ƫ���
#define ER_POSITIONY			10					//Y���ӡλ��ƫ���
#define	ER_NOPAPER				11					//ûֽ
#define  ER_UINDERR				12				//	Y�ϸ�Ӧ����
#define  ER_DINDERR				13				//	Y�¸�Ӧ����
#define  ER_LINDERR				14				//	X���Ӧ����
#define  ER_RINDERR				15				//	X�Ҹ�Ӧ����
#define  ER_ZINDERR				16				//	Z��Ӧ����
#define  ER_CUTNUMSETERR		17				//  �������ô���

//ƽ���и��20100623
//��Ӧ��
//0X0A000008
#define I_XLEFTINDUCTOR			0 ///4					// X�����ϸ�Ӧ��
#define I_XRIGHINDUCTOR			1 ///5					// X�����¸�Ӧ��
#define I_YUPINDUCTOR			2 ///6					// Y�����Ӧ��
#define I_YDOWNINDUCTOR		3 ///7					// Y���Ҹ�Ӧ��
#define I_ZINDUCTOR				4 ///3					// Z��ԭ���Ӧ��
// 
#define I_WARNINGX				5				//x������������
#define I_WARNINGY				6				//y������������
#define I_WARNINGZ				7				//z������������
#define I_WARNINGU				8				//z������������

// ��ʽ��ī�и�� 20140108
#define	I_YORIGIN				4						//Y��ԭ��
#define I_ROLLPEND				7						//��ֽͣ
#define I_ROLLSTART				0						//��ֽ����  û��
#define	I_GIVEBACK				6						//��ֽͣ
#define I_GIVEPEND				5						//��ֽ����

#define	O_CUTDOWN			0x20//0x08000414				//���߿��� �����20141205 rwt
#define O_STARTGIVEPMOTOR 	0x01				//������ֽ���
#define O_STARTROLLPMOTOR	0x04				//������ֽ���

#define	O_GIVEMOTORDIR		0x02				//��ֽ����������
#define	O_ROLLMOTORDIR		0x08				//��ֽ����������
// ************

#define C_CUTDOWN			0			//�䵶
#define C_CUTUP				1//0x20			//̧��//20141205 rwt

#define B_SENDPAPER				0x01				//��ֽ״̬λ
#define B_ROLLPAPER				0x02				//��ֽ״̬λ
#define B_NOPAPER					0x10				//ûֽ״̬λ

#define G_SENDPAPER				1						//��ֽ�������Ϊ��ֽ
#define G_ROLLPAPER				0						//��ֽ�������Ϊ��ֽ
#define R_RECEIVEPAPER		1						//��ֽ�����ֽ
#define R_BACKPAPER				0						//��ֽ�����ֽ

#define	StatusOk					0x00				//��Ӧ��λ�ñ���ס
#define	StatusErr					0x01				//��Ӧ��λ��û�б���ס

//gl_cInvertBufStatus
#define	PrintBit					0x01				//bit0Ϊ1:��ǰ׼����ӡ�����ڴ�ӡ���ǻ�����1�е�����
#define PrepData					0x02				//bit1Ϊ1:��ǰӦ��׼�����ǻ�����1�е�����
#define	Buf0Presetbit			0x04				//bit2Ϊ1��ʾ������0������׼����
#define	Buf1Presetbit			0x08				//bit3Ϊ1��ʾ������1������׼����

#define GET_IC_VER				0x01				//��ȡоƬ���̼��汾
#define ENTER_SLEEP				0x03				//����͹���˯�߹���״̬
#define RESET_ALL					0x05				//ִ��Ӳ����λ
#define CHECK_EXIST				0x06				//���Թ���״̬
#define CHK_SUSPEND				0x0B				//���ü��USB���߹���״̬�ķ�ʽ
#define SET_USB_ID				0x12				//����USB�ĳ���ʶ����VID�Ͳ�Ʒʶ����PID
#define SET_USB_MODE			0x15				//����USB����ģʽ
#define GET_STATUS				0x22				//��ȡ�ж�״̬��ȡ������
#define UNLOCK_USB				0x23				//�ͷŵ�ǰUSB������
#define RD_USB_DATA0			0x27				//�ӵ�ǰUSB�жϵĶ˵㻺������ȡ���ݿ�
#define RD_USB_DATA				0x28				//�ӵ�ǰUSB�жϵĶ˵㻺������ȡ���ݿ鲢�ͷŵ�ǰ������
#define WR_USB_DATA5			0x2A				//��USB�˵�1���ϴ�������д�����ݿ�
#define WR_USB_DATA7			0x2B				//��USB�˵�2���ϴ�������д�����ݿ�
#define CMD_RET_SUCCESS		0x51				//�����ɹ�
#define CMD_RET_ABORT			0x5F				//����ʧ��
#define GEt_DESCR					0x46				//��ȡ������
#define USB_INT_EP1_OUT		0x01				//�����˵�/�˵�1���յ�����,OUT�ɹ�
#define USB_INT_EP1_IN		0x09				//�ж϶˵�/�˵�1����������,IN�ɹ�
#define USB_INT_EP2_OUT		0x02				//�����˵�/�˵�2���յ�����,OUT�ɹ�
#define USB_INT_EP2_IN		0x0A				//�����˵�/�˵�2����������,IN�ɹ�
#define USB_INT_USB_SUSPEND	0x05			//USB���߹���(�����CHK_SUSPEND)
#define USB_INT_WAKE_UP		0x06				//��˯���б������¼�(�����ENTER_SLEEP)

#define USB_VID						0x5250			//����ʶ����VIDΪRP
#define USB_PID						0x4D49			//��Ʒʶ����PIDΪMI

// �и��ٶ�
#define   HIGHSPEED     320//800
#define   MHSPEED		240//600
#define   MIDDLESPEED   160//400
#define   MLSPEED		80//200
#define   LOWSPEED	    40//100	
// ��ӡ�ٶ�
#define   PRINTSPEEDMAX 320//800
#define	  PRINTMHSPEED  240//600
#define	  PRINTMSPEED	160//400
#define   PRINTMLSPEED  80//200
#define	  PRINTSPEEDMIN 40//100
// �ƿ��ٶ�
#define   MOVEFRAMSPEEDMAX 700//280//
#define	  MOVEFRAMMHSPEED  201//81//
#define   MOVEFRAMMSPEED   400//160//
#define	  MOVEFRAMMLSPEED  601//241//
#define	  MOVEFRAMSPEEDMIN 41
//
#define   ZDRIVESPEEDMAX 11000
#define   ZDRIVESPEEDMH  10000
#define   ZDRIVESPEEDMID 9000
#define   ZDRIVESPEEDML  8000
#define	  ZDRIVESPEEDMIN 7000
//
//#define	  ZADDSPEEDMIN 10000
//��ʼ�����ٶ�
#define   INISPEEDMAX  100
#define   INISPEEDMIN  20

//�䵶ƫ��
#define	DowCutOffset		40
 
#define	SprinklerNum			2	//�����ͷ��

#endif
