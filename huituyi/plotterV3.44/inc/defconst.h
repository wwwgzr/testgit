#ifndef __DEFCONST_H__
#define __DEFCONST_H__

#include "..\target\44b.h"

#define MCLK 							64000000

#define	SectorSize				0x800			//2K word
#define	SetParameterAddr	0x1ff000

#define UartRecBufLength	20				//串口接收缓冲区长度
#define UartSendBufLength 8					//串口发送缓冲区长度
#define	EthnetBufLength		0x1000000
#define BEGINADDR			0x0c700000     //原始数据存储空间为：0xC700000~0xD6FFFFF(16M)
#define	InvertDataAddr0		0x0d800000    //转换数据块0空间为：0xD800000~0xDAFFFFF(2M)
#define	InvertDataAddr1		0x0db00000    //转换数据块1空间为：0xDB00000~0xDDFFFFF(2M)

#define	KeyBufLength			20				//按键缓冲区长度
#define	SCANTIMES1				3					//3次扫描键值一样，证明该键按下
#define	SCANTIMES2				33				//50次扫描该键一直被按下，重复记值一次

#define X_PPMM						67.66552	//每毫米对应绘图仪X轴的脉冲数,X轴电机每1600个脉冲转一圈
#define Y_PPMM						111.11113//83.33335//166.6667//416.66675 //每毫米对应绘图仪Y轴的脉冲数,Y轴电机每4000个脉冲转一圈
#define	AdditionDot				80//120//1000//16//356//16					//为了调整相邻两次扫描之间的错开误差，多打印的点数
#define	OriginalSideDot		240//1030			//为了避免错位原点侧空走的点数(13cm)
#define	MotorSideDot			240//555				//为了避免错位电机侧空走的点数(7cm)

#define	ReadIOAddr				0xA000008	//读IO口数据地址
#define	WriteIOAddr				0xA00000C	//写IO口数据地址
#define  KEYSCANADDR   (*(volatile unsigned *)0xa000010) //键盘扫描读写地址20100220
#define WriteLcdComAddr		0xA000004	//写液晶命令地址
#define WriteLcdDataAddr	0xA000000	//写液晶数据地址
#define	WritePulseAddrX		0x8000004	//X轴驱动写脉冲数地址
#define	WritePulseAddrY		0x8000000	//Y轴驱动写脉冲数地址
#define	WriteFIFOAddr			0x8000018	//写FIFO数据地址
#define WriteFIFOFullAddr	0x8000200	//写FIFO满地址
#define WriteFileAddr			0x8000100	//写文件开始打印地址
#define ParaFirstAddr			0xA000018	//参数保存起始地址
#define	ParaEndAddr				0xA0001FC	//参数保存结束地址
#define ParaEraseAddr			0xA000010	//参数擦除地址
#define	ParaWREndAddr			0xA000014	//读参数读写结束标志地址
#define	WRCH375ComAddr		0x2000002	//CH375写命令地址
#define WRCh375DataAddr		0x2000000	//CH375写数据地址
#define RDCH375DataAddr		0x2000000	//CH375读数据地址
#define WritePrintHeadCleanAddr    0x8000104  // 打印头清洗地址20110414

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
#define	ReadFIFOFull()				(rPDATC & 0x0001)	//GPC0为FIFO满状态标志位，为1满
#define ReadPrintEnd()				(rPDATC & 0x0004)	//GPC2为打印是否结束状态位，为1结束
#define	ClrFIFO								rPDATC |= 0x0002;\
															rPDATC &= 0xfffd	//GPC1为清FIFO控制位,为1清FIFO

//#define PixelY						300					//Y轴象素(单位:象素/英寸)
#define	SprinklerWidth		150					//单位bit
#define DotPulseNum			14	// 14					//一个像素点所需的脉冲数
#define DotLineWidth			0.08467			//一个点行所占的宽度
#define	SprinklerSpace		207//310					//喷头之间错开的距离(单位bit)
#define	MaxMalposedDot		20					//喷头之间错开的最大点数
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

#define LCD_Line0					0x01				//LCD第0行反白标志（1已反白）
#define LCD_Line1					0x02				//LCD第1行反白标志（1已反白）

#define ER_INITPORT				0						//初始化串口出错
#define ER_FRAME					1						//帧出错
#define ER_PARITY					2						//校验错
#define ER_COMMAND				3						//绘图命令错
#define ER_XOVER					4						//X轴移动超界
#define ER_YOVER					5						//Y轴移动超界
#define ER_YOBAD					6						//Y轴原点传感器坏
#define	ER_SRAM						7						//sram自检出错
#define ER_POSITIONX			9						//X轴打印位置偏差大
#define ER_POSITIONY			10					//Y轴打印位置偏差大
#define	ER_NOPAPER				11					//没纸

#define	I_YORIGIN					3						//Y轴原点
#define I_ROLLPEND				4						//收纸停
#define I_ROLLSTART				5						//收纸启动
#define	I_GIVEBACK				6						//送纸停
#define I_GIVEPEND				7						//送纸启动

#define O_STARTGIVEPMOTOR 0x08				//启动送纸电机
#define O_STARTROLLPMOTOR	0x02				//启动收纸电机
#define	O_GIVEMOTORDIR		0x04				//送纸电机方向控制
#define	O_ROLLMOTORDIR		0x01				//收纸电机方向控制

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

#define CleanCommand()			*((volatile unsigned short *)(WritePrintHeadCleanAddr))=0x0		// 清楚清洗指令
#define WritePrintHeadCleanAddr    0x8000104  // 打印头清洗地址20110414

#define LANGUAGE					4			// 0中文 1 英文 2西班牙语 3土耳其 4葡萄牙文
//#define	SprinklerNum			2	//最多喷头数20101214 喷头数可选
	
#endif
