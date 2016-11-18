#ifndef __DEFCONST_H__
#define __DEFCONST_H__

#include "..\target\44b.h"

#define MCLK 							64000000

#define	SectorSize				0x800			//2K word
#define	SetParameterAddr	0x1ff000

#define UartRecBufLength	20				//���ڽ��ջ���������
#define UartSendBufLength 8					//���ڷ��ͻ���������
#define	EthnetBufLength		0x1000000
#define BEGINADDR			0x0c700000     //ԭʼ���ݴ洢�ռ�Ϊ��0xC700000~0xD6FFFFF(16M)
#define	InvertDataAddr0		0x0d800000    //ת�����ݿ�0�ռ�Ϊ��0xD800000~0xDAFFFFF(2M)
#define	InvertDataAddr1		0x0db00000    //ת�����ݿ�1�ռ�Ϊ��0xDB00000~0xDDFFFFF(2M)

#define	KeyBufLength			20				//��������������
#define	SCANTIMES1				3					//3��ɨ���ֵһ����֤���ü�����
#define	SCANTIMES2				33				//50��ɨ��ü�һֱ�����£��ظ���ֵһ��

#define X_PPMM						67.66552	//ÿ���׶�Ӧ��ͼ��X���������,X����ÿ1600������תһȦ
#define Y_PPMM						111.11113//83.33335//166.6667//416.66675 //ÿ���׶�Ӧ��ͼ��Y���������,Y����ÿ4000������תһȦ
#define	AdditionDot				80//120//1000//16//356//16					//Ϊ�˵�����������ɨ��֮��Ĵ������ӡ�ĵ���
#define	OriginalSideDot		240//1030			//Ϊ�˱����λԭ�����ߵĵ���(13cm)
#define	MotorSideDot			240//555				//Ϊ�˱����λ�������ߵĵ���(7cm)

#define	ReadIOAddr				0xA000008	//��IO�����ݵ�ַ
#define	WriteIOAddr				0xA00000C	//дIO�����ݵ�ַ
#define  KEYSCANADDR   (*(volatile unsigned *)0xa000010) //����ɨ���д��ַ20100220
#define WriteLcdComAddr		0xA000004	//дҺ�������ַ
#define WriteLcdDataAddr	0xA000000	//дҺ�����ݵ�ַ
#define	WritePulseAddrX		0x8000004	//X������д��������ַ
#define	WritePulseAddrY		0x8000000	//Y������д��������ַ
#define	WriteFIFOAddr			0x8000018	//дFIFO���ݵ�ַ
#define WriteFIFOFullAddr	0x8000200	//дFIFO����ַ
#define WriteFileAddr			0x8000100	//д�ļ���ʼ��ӡ��ַ
#define ParaFirstAddr			0xA000018	//����������ʼ��ַ
#define	ParaEndAddr				0xA0001FC	//�������������ַ
#define ParaEraseAddr			0xA000010	//����������ַ
#define	ParaWREndAddr			0xA000014	//��������д������־��ַ
#define	WRCH375ComAddr		0x2000002	//CH375д�����ַ
#define WRCh375DataAddr		0x2000000	//CH375д���ݵ�ַ
#define RDCH375DataAddr		0x2000000	//CH375�����ݵ�ַ
#define WritePrintHeadCleanAddr    0x8000104  // ��ӡͷ��ϴ��ַ20110414

#define	Writeflash(addr,dat)	*((volatile unsigned short *)(addr<<1))=(unsigned short)dat
#define	Readflash(addr)				(*((volatile unsigned short *)(addr<<1)))
#define	WriteIO(dat)					*((volatile unsigned char *)(WriteIOAddr))=(unsigned char)dat
#define	ReadIO()							(*((volatile unsigned char *)(ReadIOAddr)))
#define	WriteLcdCommand(dat)	*((volatile unsigned char *)(WriteLcdComAddr))=(unsigned char)dat;Delay(50)
#define	WriteLcdData(dat)			*((volatile unsigned char *)(WriteLcdDataAddr))=(unsigned char)dat;Delay(50)
#define Writesdram(addr,dat)	*((volatile unsigned short *)(addr))=(unsigned short)dat
#define Readsdram(addr)		  	(*((volatile unsigned short *)(addr)))
#define WritePulseX(dat)			*((volatile short *)(WritePulseAddrX))=(short)dat
#define WritePulseY(dat)			*((volatile short *)(WritePulseAddrY))=(short)dat
#define	WriteFIFO(addr,dat)		*((volatile unsigned short *)(addr))=(unsigned short)dat
#define WriteFIFOFull(dat)		*((volatile unsigned char *)(WriteFIFOFullAddr))=(unsigned char)dat
#define	WriteFilePrint(dat)		*((volatile unsigned char *)(WriteFileAddr))=(unsigned char)dat
#define   PrintHeadClean(cout)			*((volatile unsigned short *)(WritePrintHeadCleanAddr))=0x03; Delay(cout*1000);*((volatile unsigned short *)(WritePrintHeadCleanAddr))=0x0
#define ReadParaData(addr)		(*((volatile unsigned char *)(addr)))
#define	ReadParaFlag()				(*((volatile unsigned char *)(ParaWREndAddr)))
#define	WriteParaData(addr,dat)	*((volatile unsigned char *)(addr))=(unsigned char)dat
#define WriteCH375Command(dat)	*((volatile unsigned char *)(WRCH375ComAddr))=(unsigned char)dat
#define WriteCH375Data(dat)		*((volatile unsigned char *)(WRCh375DataAddr))=(unsigned char)dat
#define ReadCH375Data()				(*((volatile unsigned char *)(RDCH375DataAddr)))

#define	WriteDL16							rPDATC &= 0xffef
#define	WriteDH16							rPDATC |= 0x0010
#define	DrivePrint						rPDATC |= 0x0008;\
															rPDATC &= 0xfff7
#define	ReadFIFOFull()				(rPDATC & 0x0001)	//GPC0ΪFIFO��״̬��־λ��Ϊ1��
#define ReadPrintEnd()				(rPDATC & 0x0004)	//GPC2Ϊ��ӡ�Ƿ����״̬λ��Ϊ1����
#define	ClrFIFO								rPDATC |= 0x0002;\
															rPDATC &= 0xfffd	//GPC1Ϊ��FIFO����λ,Ϊ1��FIFO

//#define PixelY						300					//Y������(��λ:����/Ӣ��)
#define	SprinklerWidth		150					//��λbit
#define DotPulseNum			14	// 14					//һ�����ص������������
#define DotLineWidth			0.08467			//һ��������ռ�Ŀ��
#define	SprinklerSpace		207//310					//��ͷ֮����ľ���(��λbit)
#define	MaxMalposedDot		20					//��ͷ֮�����������
//#define Order_Start				0xf5
//#define Order_LineWidth		0x01

#define Key_Pause					'7'
#define Key_Ok						'5'
#define	Key_Down					'8'
#define	Key_Up						'2'
#define	Key_Left					'4'
#define	Key_Right					'6'
#define Key_Menu					'1'
#define Key_3						'3'

#define LCD_Line0					0x01				//LCD��0�з��ױ�־��1�ѷ��ף�
#define LCD_Line1					0x02				//LCD��1�з��ױ�־��1�ѷ��ף�

#define ER_INITPORT				0						//��ʼ�����ڳ���
#define ER_FRAME					1						//֡����
#define ER_PARITY					2						//У���
#define ER_COMMAND				3						//��ͼ�����
#define ER_XOVER					4						//X���ƶ�����
#define ER_YOVER					5						//Y���ƶ�����
#define ER_YOBAD					6						//Y��ԭ�㴫������
#define	ER_SRAM						7						//sram�Լ����
#define ER_POSITIONX			9						//X���ӡλ��ƫ���
#define ER_POSITIONY			10					//Y���ӡλ��ƫ���
#define	ER_NOPAPER				11					//ûֽ

#define	I_YORIGIN					3						//Y��ԭ��
#define I_ROLLPEND				4						//��ֽͣ
#define I_ROLLSTART				5						//��ֽ����
#define	I_GIVEBACK				6						//��ֽͣ
#define I_GIVEPEND				7						//��ֽ����

#define O_STARTGIVEPMOTOR 0x08				//������ֽ���
#define O_STARTROLLPMOTOR	0x02				//������ֽ���
#define	O_GIVEMOTORDIR		0x04				//��ֽ����������
#define	O_ROLLMOTORDIR		0x01				//��ֽ����������

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

#define CleanCommand()			*((volatile unsigned short *)(WritePrintHeadCleanAddr))=0x0		// �����ϴָ��
#define WritePrintHeadCleanAddr    0x8000104  // ��ӡͷ��ϴ��ַ20110414

#define LANGUAGE					4			// 0���� 1 Ӣ�� 2�������� 3������ 4��������
//#define	SprinklerNum			2	//�����ͷ��20101214 ��ͷ����ѡ
	
#endif
