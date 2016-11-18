#ifndef __DEFCONST_H__
#define __DEFCONST_H__

#include "..\target\44b.h"

#define MCLK 							64000000

#define	SectorSize				0x800			//2K word
#define	SetParameterAddr	0x1ff000

#define UartRecBufLength	20				//串口接收缓冲区长度
#define UartSendBufLength 8					//串口发送缓冲区长度
#define	EthnetBufLength		0x1800000	//0x1000000	//数据接收缓冲区长度16M	1677216B
#define	BEGINADDR			0x0c300000	//0x0c700000	//打印接收数据缓冲区起始地址(16M)
#define	InvertDataAddr0		0x0db00000	//0x0d800000	//打印数据转换区0    起始地址(2M)
#define	InvertDataAddr1		0x0dc00000	//0x0db00000	//打印数据转换区1 	起始地址(2M)
#define CutDataAddr			0x0dd00000	//0x0de00000	//切割数据存储区起始地址0xDE00000~0xDFFFFFF(2M)  20100909
#define	CutDataLength		0x100000	//切割文件缓冲区长度
#define	KeyBufLength			20				//按键缓冲区长度
#define	SCANTIMES1				2 //3					//3次扫描键值一样，证明该键按下
#define	SCANTIMES2				33//16//33				//50次扫描该键一直被按下，重复记值一次


//速度参数设定
#define Print_SV	100   //100
#define Print_V		500 //500
#define	Print_T		400

#define	Frame_SV	10

#define  CUTPRESSMAX     150   //刀压最大值
												//169.7652726
#define X_PPMM						166.935851	//169.140989//169.1638171////169.040989//200//611.845321//200////83.33333//67.66552	//每毫米对应绘图仪X轴的脉冲数,X轴电机每1600个脉冲转一圈
#define Y_PPMM						166.6667//167.256///111.11113//185.185185//611.845321//192.30769////83.33333//111.11113//83.33335//166.6667//416.66675 //每毫米对应绘图仪Y轴的脉冲数,Y轴电机每4000个脉冲转一圈
//********20130807 Z/U AXIS Pluse define
#define Z_PPMM						10//9.91268//31.847133//4.218634	// Z轴每个单位脉冲数 每毫米脉冲数(4000/((2*2*3.14)*10) = 31.847133)  (20*31.847133)/151 = 4.218634
#define U_PPMM						100	  			// 进纸电机没毫米脉冲数
//*************

#define BASE_PPM    600//600

//*************20130807Z轴一圈的脉冲数
#define RP_ZAXISLAPPLUS   8000						//Z轴一圈的脉冲数

#define HPGL_MM		0.025						//每绘图语言的逻辑单位(注意不是绘图仪的单位)对应的毫米值
#define	AdditionDot				80//120//1000//16//356//16	 //为了调整相邻两次扫描之间的错开误差，多打印的点数
#define	OriginalSideDot			316//240//1030			//为了避免错位原点侧空走的点数(13cm)
#define	MotorSideDot			316//240//555				//为了避免错位电机侧空走的点数(7cm) 喷嘴一个点宽1/7.9 = 0.1265

#define LeftMargin				(90*Y_PPMM)			//笔车左边距190
#define PenCarSideInterval		(AdditionDot + OriginalSideDot)			//笔车侧预留间距 50mm 20130830

#define 	SPEED_M		40//40		//倍率16范围500K    20//速度倍率,按160000PPS,倍率是20    20100622

#define	ReadIOAddr				0xA000008	//读IO口数据地址
#define	WriteIOAddr				0xA00000C	//写IO口数据地址
#define  KEYSCANADDR   (*(volatile unsigned *)0xa000010) //键盘扫描读写地址20100220
#define WriteLcdComAddr		0xA000004	//写液晶命令地址
#define WriteLcdDataAddr	0xA000000	//写液晶数据地址
//#define	WritePulseAddrX		0x8000004	//X轴驱动写脉冲数地址kk
//#define	WritePulseAddrY		0x8000000	//Y轴驱动写脉冲数地址kk
#define	WriteFIFOAddr			0x8000000	//写FIFO数据地址
#define WriteFIFOFullAddr		0x8000400	//0x8000404	//写FIFO满地址  设置有效喷头数
#define WriteFileAddr			0x8000404	//0x8000400	//写文件开始打印地址
#define	WriteDirctAddr			0x800040C	// 写打印方向
#define ParaFirstAddr			0xA000018	//参数保存起始地址
#define	ParaEndAddr				0xA0001FC	//参数保存结束地址
#define ParaEraseAddr			0xA000010	//参数擦除地址
#define	ParaWREndAddr			0xA000014	//读参数读写结束标志地址
#define	WRCH375ComAddr		0x2000002	//CH375写命令地址
#define WRCh375DataAddr		0x2000000	//CH375写数据地址
#define RDCH375DataAddr		0x2000000	//CH375读数据地址

#define  WRITE312ADDR			0x8000408//0x8000120//喷墨时向该地址写Y轴驱动脉冲数
#define  NULL312ADDR				0x8000124// 编码器反馈
#define  Z_AxisSendPulseAddr		0x8000140// Z轴脉冲数写入地址
#define  Z_AxisInitSpeedAddr		0x8000144// Z轴初始速度写入地址
#define  Z_AxisDriveSpeedAddr		0x8000148// Z轴驱动速度写入地址
#define  Z_AxisAddSpeedAddr		0x800014C	// Z轴加速度写入地址
#define  Z_AxisCmdAddr			0x8000208	//0x8000200// Z轴命令写入地址
//U轴驱动地址
#define  U_AxisSendPulseAddr	0x8000160	// U轴脉冲数写入地址
#define  U_AxisInitSpeedAddr	0x8000164	// U轴初始速度写入地址
#define  U_AxisDriveSpeedAddr	0x8000168	// U轴驱动速度写入地址
#define  U_AxisAddSpeedAddr		0x800016C	// U轴加速度写入地址
#define  U_AxisCmdAddr			0x800020C	// U轴命令写入地址
//**************************************************
#define WritePrintHeadCleanAddr    0x8000410  // 打印头清洗地址20110414
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
#define 	AXIS_X		1							//X轴
#define 	AXIS_Y		2							//Y轴
#define 	AXIS_Z		3							//Z轴	20130807

//***************感应器定义*************************
#define INDUCTOR_XRIGHT		0						//X 右感应器
#define INDUCTOR_YUP		2						//Y 上感应器
#define INDUCTOR_ZORG		4						//Z 感应器

#define Z_INDUCTOR_ORIGIN	0x00					// Z轴感应器在原点
#define XY_INDUCTOR_ORIGIN  0x01					// X/Y轴感应器在原点
//**************************************************
//*************20130807U轴驱动
#define   WriteUAxisPlus(dat)			*((volatile  int *)(U_AxisSendPulseAddr))=dat	//发送脉冲数
#define   ReadUAxisPlus()				*((volatile  int *)(U_AxisSendPulseAddr))		//读发送脉冲数
#define	  WriteUAxisInitSpeed(dat)		*((volatile  int *)(U_AxisInitSpeedAddr))=dat		//设置Z初始速度
#define   WriteUAxisDriveSpeed(dat)		*((volatile  int *)(U_AxisDriveSpeedAddr))=dat	//设置Z驱动速度
#define   WriteUAxisAddSpeed(dat)		*((volatile  int *)(U_AxisAddSpeedAddr))=dat	//设置Z加速度
#define   WriteUAxisDriveCmd(cmd)		*((volatile  int *)(U_AxisCmdAddr))=cmd		//发送Z驱动命令
#define	  UAxisDriveEndState			((rPDATD & 0x0020) == 0x0020 ? 1 : 0)		//读Z轴驱动状态 标志,1结束
#define   DriveUAxisStartCmd			0x5555
#define   DriveUAxisStopCmd				0x5555
//************测试读点击编码器
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
//写FIFO原来16位，现升级为一次写32位20100813
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

#define   Write312YAxisPlus(dat)	*((volatile  int *)(WRITE312ADDR))=dat	//写312Y时要向该地址写入Y脉冲数
#define   WriteZAxisPlus(dat)			*((volatile  int *)(Z_AxisSendPulseAddr))=dat	//发送脉冲数
#define   ReadZAxisPlus()			*((volatile  int *)(Z_AxisSendPulseAddr))		//读发送脉冲数
#define	WriteZAxisInitSpeed(dat)		*((volatile  int *)(Z_AxisInitSpeedAddr))=dat		//设置Z初始速度
#define   WriteZAxisDriveSpeed(dat)		*((volatile  int *)(Z_AxisDriveSpeedAddr))=dat	//设置Z驱动速度
#define   WriteZAxisAddSpeed(dat)		*((volatile  int *)(Z_AxisAddSpeedAddr))=dat	//设置Z加速度
#define   WriteZAxisDriveCmd(cmd)			*((volatile  int *)(Z_AxisCmdAddr))=cmd		//发送Z驱动命令
#define   Read312YAxisPlus()		*((volatile  int *)(WRITE312ADDR))	//写312Y时要向该地址写入Y脉冲数
#define   ReadF312()				*((volatile  int *)(NULL312ADDR))
#define	WriteDL16							rPDATC &= 0xffef  //平板切割机未用
#define	WriteDH16							rPDATC |= 0x0010  //平板切割机未用

#define	DrivePrint					rPDATD &= 0xffbf; \
												rPDATD |= 0x040;
#define	ReadFIFOFull()				(rPDATD & 0x0010)	//GPC0为FIFO满状态标志位，为1满
//#define ReadPrintEnd()				(rPDATD & 0x08) 	// 判断312工作状态是否忙 0忙 1空闲 没用

#define	ClrFIFO								rPDATD |= 0x080; \
															rPDATD &= 0xff7f	//GPC1为清FIFO控制位,为1清FIFO
#define	ZAxisDriveEndState				((rPDATD & 0x0020) == 0x0020 ? 1 : 0)		//读Z轴驱动状态 标志,1结束
#define   DriveZAxisStartCmd				0x5555
#define   DriveZAxisStopCmd				0x5555
//#define PixelY						300					//Y轴象素(单位:象素/英寸)
#define	SprinklerWidth		150					//单位bit
#define DotPulseNum				21  //75.5997//23.3333333//77.09251101//14		//一个像素点所需的脉冲数 1200*600/9600
#define DotLineWidth			0.08467			//一个点行所占的宽度
#define	SprinklerSpace		207//310					//喷头之间错开的距离(单位bit)
#define	MaxMalposedDot		20					//喷头之间错开的最大点数
//#define Order_Start				0xf5
//#define Order_LineWidth		0x01
//20100623键值定义
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

// 没有使用
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
#define LCD_Line0					0x01				//LCD第0行反白标志（1已反白）
#define LCD_Line1					0x02				//LCD第1行反白标志（1已反白）

#define ER_INITPORT				0						//初始化串口出错
#define ER_FRAME					1						//帧出错
#define ER_PARITY					2						//校验错
#define ER_COMMAND					3						//绘图命令错
#define ER_XOVER					4						//X轴移动超界
#define ER_YOVER					5						//Y轴移动超界
#define ER_YOBAD					6						//Y轴原点传感器坏
#define	ER_SRAM						7						//sram自检出错
#define ER_POSITIONX			9						//X轴打印位置偏差大
#define ER_POSITIONY			10					//Y轴打印位置偏差大
#define	ER_NOPAPER				11					//没纸
#define  ER_UINDERR				12				//	Y上感应器坏
#define  ER_DINDERR				13				//	Y下感应器坏
#define  ER_LINDERR				14				//	X左感应器坏
#define  ER_RINDERR				15				//	X右感应器坏
#define  ER_ZINDERR				16				//	Z感应器坏
#define  ER_CUTNUMSETERR		17				//  刀号设置错误

//平板切割机20100623
//感应器
//0X0A000008
#define I_XLEFTINDUCTOR			0 ///4					// X轴左上感应器
#define I_XRIGHINDUCTOR			1 ///5					// X轴左下感应器
#define I_YUPINDUCTOR			2 ///6					// Y轴左感应器
#define I_YDOWNINDUCTOR		3 ///7					// Y轴右感应器
#define I_ZINDUCTOR				4 ///3					// Z轴原点感应器
// 
#define I_WARNINGX				5				//x轴驱动器报警
#define I_WARNINGY				6				//y轴驱动器报警
#define I_WARNINGZ				7				//z轴驱动器报警
#define I_WARNINGU				8				//z轴驱动器报警

// 立式喷墨切割机 20140108
#define	I_YORIGIN				4						//Y轴原点
#define I_ROLLPEND				7						//收纸停
#define I_ROLLSTART				0						//收纸启动  没用
#define	I_GIVEBACK				6						//送纸停
#define I_GIVEPEND				5						//送纸启动

#define	O_CUTDOWN			0x20//0x08000414				//刀具控制 电磁铁20141205 rwt
#define O_STARTGIVEPMOTOR 	0x01				//启动送纸电机
#define O_STARTROLLPMOTOR	0x04				//启动收纸电机

#define	O_GIVEMOTORDIR		0x02				//送纸电机方向控制
#define	O_ROLLMOTORDIR		0x08				//收纸电机方向控制
// ************

#define C_CUTDOWN			0			//落刀
#define C_CUTUP				1//0x20			//抬刀//20141205 rwt

#define B_SENDPAPER				0x01				//送纸状态位
#define B_ROLLPAPER				0x02				//收纸状态位
#define B_NOPAPER					0x10				//没纸状态位

#define G_SENDPAPER				1						//送纸电机方向为送纸
#define G_ROLLPAPER				0						//送纸电机方向为收纸
#define R_RECEIVEPAPER		1						//收纸电机收纸
#define R_BACKPAPER				0						//收纸电机退纸

#define	StatusOk					0x00				//感应器位置被挡住
#define	StatusErr					0x01				//感应器位置没有被挡住

//gl_cInvertBufStatus
#define	PrintBit					0x01				//bit0为1:当前准备打印或正在打印的是缓冲区1中的数据
#define PrepData					0x02				//bit1为1:当前应该准备的是缓冲区1中的数据
#define	Buf0Presetbit			0x04				//bit2为1表示缓冲区0数据已准备好
#define	Buf1Presetbit			0x08				//bit3为1表示缓冲区1数据已准备好

#define GET_IC_VER				0x01				//获取芯片及固件版本
#define ENTER_SLEEP				0x03				//进入低功耗睡眠挂起状态
#define RESET_ALL					0x05				//执行硬件复位
#define CHECK_EXIST				0x06				//测试工作状态
#define CHK_SUSPEND				0x0B				//设置检查USB总线挂起状态的方式
#define SET_USB_ID				0x12				//设置USB的厂商识别码VID和产品识别码PID
#define SET_USB_MODE			0x15				//设置USB工作模式
#define GET_STATUS				0x22				//获取中断状态并取消请求
#define UNLOCK_USB				0x23				//释放当前USB缓冲区
#define RD_USB_DATA0			0x27				//从当前USB中断的端点缓冲区读取数据块
#define RD_USB_DATA				0x28				//从当前USB中断的端点缓冲区读取数据块并释放当前缓冲区
#define WR_USB_DATA5			0x2A				//向USB端点1的上传缓冲区写入数据块
#define WR_USB_DATA7			0x2B				//向USB端点2的上传缓冲区写入数据块
#define CMD_RET_SUCCESS		0x51				//操作成功
#define CMD_RET_ABORT			0x5F				//操作失败
#define GEt_DESCR					0x46				//获取描述符
#define USB_INT_EP1_OUT		0x01				//辅助端点/端点1接收到数据,OUT成功
#define USB_INT_EP1_IN		0x09				//中断端点/端点1发送完数据,IN成功
#define USB_INT_EP2_OUT		0x02				//批量端点/端点2接收到数据,OUT成功
#define USB_INT_EP2_IN		0x0A				//批量端点/端点2发送完数据,IN成功
#define USB_INT_USB_SUSPEND	0x05			//USB总线挂起(如果已CHK_SUSPEND)
#define USB_INT_WAKE_UP		0x06				//从睡眠中被唤醒事件(如果已ENTER_SLEEP)

#define USB_VID						0x5250			//厂商识别码VID为RP
#define USB_PID						0x4D49			//产品识别码PID为MI

// 切割速度
#define   HIGHSPEED     320//800
#define   MHSPEED		240//600
#define   MIDDLESPEED   160//400
#define   MLSPEED		80//200
#define   LOWSPEED	    40//100	
// 打印速度
#define   PRINTSPEEDMAX 320//800
#define	  PRINTMHSPEED  240//600
#define	  PRINTMSPEED	160//400
#define   PRINTMLSPEED  80//200
#define	  PRINTSPEEDMIN 40//100
// 移框速度
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
//初始驱动速度
#define   INISPEEDMAX  100
#define   INISPEEDMIN  20

//落刀偏移
#define	DowCutOffset		40
 
#define	SprinklerNum			2	//最多喷头数

#endif
