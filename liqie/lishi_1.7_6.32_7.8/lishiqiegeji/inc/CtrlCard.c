//*******************************************************************
//文件名：CtrlCard.c
//描述：	运动控制芯片MCX312驱动程序
//版本：	1.0
//作者：	dgj
//日期：	2010-6-22
//*******************************************************************
#include <stdlib.h>
#include "DefConst.h"
#include "extern.h"
/*
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
*/
//#define 	AXIS_X		2							//X轴
//#define 	AXIS_Y		1							//Y轴

unsigned int WR3_value;             //驱动状态寄存器值保存
unsigned int WR1_value;				//
unsigned int WR2_value;				//

extern	void Delay(int time);
//双脉冲输出
//#define ccw 

long IntRound(double x);

void	set_speed(int axis,int wdata);
void	set_startv(int axis,int wdata);
void	pmove(int axis,long pulse);
void	inp_move2(long pulse1,long pulse2);
void	set_ad_mode(int axis,int value);
void	set_dec1_mode(int axis,int value);
void	set_dec2_mode(int axis,int value);
void	inp_dec_enable(void);
void	inp_dec_disable(void);
void	set_command_pos(int axis,long wdata);
void	get_command_pos(int axis,long *pos);
void	get_status(int axis,int *value);
void	get_inp_status(int *value);
void	get_inp_status2(int *value);
void	outpw(unsigned long address,int wdata);
void	wreg1(int axis,int wdata);
void	wreg2(int axis,int wdata);
void	wreg3(int axis,int wdata);
void	set_range(int axis,long wdata);
void	set_acac(int axis,int wdata);
void	set_acc(int axis,int wdata);
void	set_dec(int axis,int wdata);
void	set_pulse(int axis,long wdata);
void	set_decp(int axis,long wdata);
void	accofst(int axis,long wdata);
void	Init_MCX312(void);
extern void ReceiveData(void);


//四舍五入取整
//入口:
//  x
//返回值:
//  四舍五入后的整数
long IntRound(double x)
{
	double d;
	long lResult;
	
	if (x<0)
		d = x - 0.5;
	else
		d = x + 0.5;
	lResult = (long)d;
	return lResult;
}

#define RWMcx312BasicAddr		0x06000000	//基地址暂时未定
//define WriteMcxData(addr, dat)		*((volatile unsigned short *)(RWMcx312BasicAddr+(addr)))=(unsigned short)dat
//define ReadMcxData(addr)			(*((volatile unsigned short *)(RWMcx312BasicAddr+(addr))))
void WriteMcxData(unsigned long  address, int data)
{
	//Printf("W : %x, D : %x\n", ((volatile unsigned short *)(RWMcx312BasicAddr+(address))), data);
	*((volatile unsigned short *)(RWMcx312BasicAddr+(address)))=(unsigned short)data;
	//Delay(10);
	//Printf("W V: %x\n", * ((volatile unsigned short *)(RWMcx312BasicAddr+(address))));
}

unsigned short ReadMcxData(int address)
{
	unsigned short sValue;
	Delay(1);
	sValue = (*((volatile unsigned short *)(RWMcx312BasicAddr+(address))));
	//Delay(10);
//	Printf("R addr : 0x%x\n",  RWMcx312BasicAddr+(address));
	return sValue;
}

void outpw(unsigned long address,int wdata)
{
	//address += RWMcx312BasicAddr;
	//intf("Write add: 0x%x\n", address);
	//WriteMcxData(address,wdata);
	*((volatile unsigned short *)(RWMcx312BasicAddr+(address)))=(unsigned short)wdata;
}

 unsigned short inpw(long address)
{
	 unsigned short iTemp;
	
	//address += RWMcx312BasicAddr;
	//intf("Read add: 0x%x\n", address);
	//iTemp = ReadMcxData(address);
	iTemp = (*((volatile unsigned short *)(RWMcx312BasicAddr+(address))));
	//return iTemp;
	return (iTemp);
}
//write register 1 setting
void wreg1(int axis,int wdata)
{
	outpw(WR0,(axis<<8)+0x0f);		//axis assignment
	outpw(WR1,wdata);
}

//write register 2 setting
void wreg2(int axis,int wdata)
{
	outpw(WR0,(axis<<8)+0x0f);		//axis assignment
	outpw(WR2,wdata);
}

//write register 3 setting
void wreg3(int axis,int wdata)
{
	outpw(WR0,(axis<<8)+0x0f);		//axis assignment
	outpw(WR3,wdata);
}

//rangw(R) setting
void set_range(int axis,long wdata)
{
	outpw(WR7,(wdata>>16)&0xffff);
	outpw(WR6,wdata&0xffff);
	outpw(WR0,(axis<<8)+0x00);
}

//Jerk(K) setting
void set_acac(int axis,int wdata)
{
	outpw(WR6,wdata);
	outpw(WR0,(axis<<8)+0x01);
}

//For acceleration/deceleration(A) setting
void set_acc(int axis,int wdata)
{
	outpw(WR6,wdata);
	outpw(WR0,(axis<<8)+0x02);
}

//For deceleration(D) setting
void set_dec(int axis,int wdata)
{
	outpw(WR6,wdata);
	outpw(WR0,(axis<<8)+0x03);
}

//For initial speed(SV) setting
void set_startv(int axis,int wdata)
{
	outpw(WR6,wdata);
	outpw(WR0,(axis<<8)+0x04);
}

//For drive speed(V) setting
void set_speed(int axis,int wdata)
{
	outpw(WR6,wdata);
	outpw(WR0,(axis<<8)+0x05);
}

//For output pulse output/finish point(P) setting
void set_pulse(int axis,long wdata)
{
	//Printf("set_pulse:%x\n", wdata);
	outpw(WR7,(wdata>>16)&0xffff);
	outpw(WR6,wdata&0xffff);
	outpw(WR0,(axis<<8)+0x06);
}

//fixed driving
void pmove(int axis,long pulse)
{
	if (pulse >= 0)
	{
		set_pulse(axis,pulse);
		outpw(WR0,(axis<<8)+0x20);
		//Printf("A\n");
	}
	else
	{
		pulse = labs(pulse);
		set_pulse(axis,pulse);
		outpw(WR0,(axis<<8)+0x21);
		//Printf("B\n");
	}
}

//2-axis linear interpolation driving
void inp_move2(long pulse1,long pulse2)
{
	set_pulse(AXIS_X,pulse1);
	set_pulse(AXIS_Y,pulse2);
	outpw(WR0,0x30);
}

//For manual deceleration(DP) setting
void set_decp(int axis,long wdata)
{
	outpw(WR7,(wdata>>16)&0xffff);
	outpw(WR6,wdata&0xffff);
	outpw(WR0,(axis<<8)+0x07);
}

//For logical position counter(EP) setting
void set_command_pos(int axis,long wdata)
{
	outpw(WR7,(wdata>>16)&0xffff);
	outpw(WR6,wdata&0xffff);
	outpw(WR0,(axis<<8)+0x09);
}

//For acceleration counter shift(AO) setting
void accofst(int axis,long wdata)
{
	outpw(WR7,(wdata>>16)&0xffff);
	outpw(WR6,wdata&0xffff);
	outpw(WR0,(axis<<8)+0x0d);
}

//For logical position counter(LP) reading
void get_command_pos(int axis,long *pos)
{
	long d6,d7;
	
	outpw(WR0,(axis<<8)+0x10);
	d6 = (long)inpw(RR6);
	d6 &= 0xffff;
	d7 = (long)inpw(RR7);
	d7 = (d7 & 0xffff) << 16;
	*pos = d6 | d7;
}

  //   lp(轴指定,数据) -------------------- 逻辑位置计数器(LP)设定 
void SetLp(int axis, long wdata)  
{  
	outpw(WR7,(wdata>>16)& 0xffff);  
	outpw(WR6,wdata & 0xffff);  
	outpw(WR0,(axis<<8) + 0x09);    
}

long ReadLp(int axis)
{
	long d6, d7;
	outpw(WR0,(axis<<8) + 0x10); 
	Delay(5);
	d6 = (long)inpw(RR6);
	d6 &= 0xffff;
	d7 = (long)inpw(RR7);
	d7 = (d7 & 0xffff) << 16;
	return (d6 | d7);
}
  
//   ep(轴指定,数据) ------------------ 逻辑位置计数器(EP)设定 
void ep(int axis, long wdata)  
{  
outpw(WR7,(wdata>>16)&0xffff); 
outpw(WR6,wdata & 0xffff); 
outpw(WR0,(axis<<8) + 0x0a); 
}  

void Init_MCX312(void)
{

	Delay(5000);
	outpw(WR0,0x8000);		//Software reset
	Delay(5000);
	//指定轴XY	
	outpw(WR0,0x30f);

	//停止信号STOP0,STOP1,STOP2均为无效
	outpw(WR1,0x8080);//Delay(10000);0x8080
	//限位信号LMT+,LMT-为低电平有效，立即停止
	//软件限位无效
	//脉冲输出方式为：脉冲＋方向方式
	//反馈输入为A/B相编码脉冲输入，1倍频
	//伺服到位信号nINPOS无效
	//伺服报警信号nALARM无效
	outpw(WR2,0x01C0);//Delay(10000);0x0040
	//加速方式为直线加/减速，对称加/减速，自动减速
	//计数器的可变环功能无效
	//输入滤波无效
	//阻止三角波输入
	outpw(WR3,0x0020);//Delay(10000);
	
	WR3_value = 0x0020;
	outpw(WR4,0x0000);//Delay(10000);
	outpw(WR5,0x0100);//Delay(10000);
	//倍率20，驱动速度400KPPS
	//Printf("Range X: \n");
	set_range(AXIS_X,IntRound((double)(1.414 * 8000000.0 / 20)));//Delay(10000);SPEED_M
	//Printf("Range Y: \n");
	set_range(AXIS_Y,IntRound((double)(8000000.0 / 20)));//Delay(10000);SPEED_M
	//lp(0x1, 0);
	//ep(0x1, 0);
	/*
	 以下为cw/CCW双脉冲驱动方式  //配合 驱动器参数
	*/
	#ifdef ccw
	outpw(WR0,0x10f);   /* set x axis only.. */
	outpw(WR2,0x0);//Delay(10000)
	#endif
}

//加/减速方式的设定
void set_ad_mode(int axis,int value)
{
	outpw(WR0,(axis<<8)+0x0f);
	if (value == 0)
		WR3_value &= 0xfffb;
	else
		WR3_value |= 0x0004;
	outpw(WR3,WR3_value);
}

//非对称梯形加/减速的设定
void set_dec1_mode(int axis,int value)
{
	outpw(WR0,(axis<<8)+0x0f);
	if (value == 0)
		WR3_value &= 0xfffd;
	else
		WR3_value |= 0x0002;
	outpw(WR3,WR3_value);
}

//加/减速定量驱动的减速方式的设定
void set_dec2_mode(int axis,int value)
{
	outpw(WR0,(axis<<8)+0x0f);
	if (value == 0)
		WR3_value &= 0xfffe;
	else
		WR3_value |= 0x0001;
	outpw(WR3,WR3_value);
}

//插补减速允许
void inp_dec_enable(void)
{
	outpw(WR0,0x3b);
}

//插补减速禁止
void inp_dec_disable(void)
{
	outpw(WR0,0x3c);
}

void SelecAxis(int axis)
{
	outpw(WR0,(axis<<8)+0x0f);
}

int ReadAxisStatus(int axis)
{
	Delay(1);
	return ((inpw(RR0))&axis);
}

// 返回0，正在打印，返回1，打印结束
int ReadPrintEnd()
{
	int iAxisState;
	
	outpw(WR0,(AXIS_X<<8)+0x0f);
	iAxisState = (inpw(RR0) & AXIS_X);
	//Printf("X[0x%x]&", iAxisState);
	Delay(1);
	outpw(WR0,(AXIS_Y<<8)+0x0f);
	iAxisState |= (inpw(RR0) & AXIS_Y);
	//Printf("Y[0x%x],", iAxisState);
	
	if(iAxisState != 0)
		iAxisState = 0;
	else
		iAxisState = 1;
	
	//ReceiveData();  //20141114
	return iAxisState;
}

//获取各轴的驱动状态
void get_status(int axis,int *value)
{
	outpw(WR0,(axis<<8)+0x0f);
	*value = inpw(RR0) & axis;
//	if (*value != 0)
//		*value = inpw(RR1) & 0xff;
}

//获取插补的驱动状态, 为1表示正在输出差补驱动脉冲
void get_inp_status(int *value)
{
	*value = (inpw(RR0) >> 8) & 0x01;
}

//Next data setting of waiting for continues interpolation
void get_inp_status2(int *value)
{
	*value = (inpw(RR0) >> 9) & 0x01;
}
/*
extern  int gl_iCutSpeed;
extern  int gl_iATime;
extern  int gl_iInitV0;
*/
//获取笔速,速度不超过1200mm/s
//返回值:
//  笔速,单位:mm/s
/*
int GetPenSpeed(void)
{
	return gl_iCutSpeed;
}
*/
/*
void SetCutSpeed(char AXIS, int initSpeed, int highSpeed, int addSpeed)
{
	set_startv(AXIS,initSpeed);
	set_speed(AXIS,highSpeed);
	set_acc(AXIS,addSpeed);
	set_dec(AXIS,addSpeed);
	set_ad_mode(AXIS,0);  		//S曲线线加/减速
	set_dec1_mode(AXIS,0);  	//对称加减速
	set_dec2_mode(AXIS,0);  	//自动减速
	inp_dec_enable();  	
}
*/
/*
void SetAxisPrintSpeed(int iAxis, int iSpeed)
{
	double dSpeedL,dSpeedH,dAcc;
	double dAccTime;
	long lAcc;

	//Printf("SetAxisPrintSpeed: %d\n", iSpeed);
	
	//X,Y轴的初始速度,速度,加速度,减速度
	dSpeedL = ((double)gl_iInitV0 * BASE_PPM) / (double)SPEED_M;
	dAccTime = (double)(gl_iATime) * 0.001;
	dSpeedH = ((double)iSpeed * BASE_PPM) / (double)SPEED_M;

	dAcc = (dSpeedH - dSpeedL) / dAccTime / 125.0;
	lAcc = IntRound(dAcc);

	set_startv(iAxis,(int)dSpeedL);
	set_speed(iAxis,(int)dSpeedH);
	set_acc(iAxis,(int)lAcc);
	set_dec(iAxis,(int)lAcc);
	//20130831 加
	set_ad_mode(iAxis,0);  		//S曲线线加/减速
	set_dec1_mode(iAxis,0);  	//对称加减速
	set_dec2_mode(iAxis,0);  	//自动减速
	inp_dec_enable();  					//插值减速允许
}
*/

void SetMovFrameSpeed(int iSpeed)
{
	double dSpeedL,dSpeedH,dAcc;
	double dAccTime;
	long lAcc;

	Printf("iv:%d SV:%d IT:%d\n", gl_iInitV0,iSpeed, gl_iATime);
	
	//X,Y轴的初始速度,速度,加速度,减速度
	dSpeedL = ((double)gl_iInitV0 * BASE_PPM) / (double)SPEED_M;
	dAccTime = (double)(gl_iATime) * 0.001;
	dSpeedH = ((double)iSpeed * BASE_PPM) / (double)SPEED_M;

	dAcc = (dSpeedH - dSpeedL) / dAccTime / 125.0;
	lAcc = IntRound(dAcc);
	
	set_startv(AXIS_Y+AXIS_X,(int)dSpeedL);
	set_speed(AXIS_Y+AXIS_X,(int)dSpeedH);
	set_acc(AXIS_Y+AXIS_X,(int)lAcc);
	set_dec(AXIS_Y+AXIS_X,(int)lAcc);
	
	//20130831 加
	set_ad_mode(AXIS_Y,0);  		//S曲线线加/减速
	set_dec1_mode(AXIS_Y,0);  	//对称加减速
	set_dec2_mode(AXIS_Y,0);  	//自动减速
	inp_dec_enable();  					//插值减速允许
}


//设置长距离行程的笔速
void SetLDPenSpeed(void)
{
	double dSpeedL,dSpeedH,dAcc;
	double dAccTime;
	long lAcc;
	
	//X,Y轴的初始速度,速度,加速度,减速度
	dSpeedL = (double) 5 * BASE_PPM / (double)SPEED_M;//gl_iInitV0
	//dAccTime = (double)(gl_iATime) * 0.001;

	if(gl_iCutSpeed == 0)
	{
		dAccTime = (double)(gl_iTimeAdd) * 0.001;
		dSpeedH = (double)(gl_iVSpeed-70) * BASE_PPM / (double)SPEED_M;	//20.4k
	}
	else if(gl_iCutSpeed == 1)
		{
			dSpeedH = (double)(gl_iVSpeed-138) * BASE_PPM / (double)SPEED_M;	//40.2
			dAccTime = (double)(gl_iTimeAdd) * 0.001;
		}
		else if(gl_iCutSpeed == 2)
			{
				dSpeedH = (double)(gl_iVSpeed-220) * BASE_PPM / (double)SPEED_M;		//60K
				dAccTime = (double)(gl_iTimeAdd) * 0.001;
			}
			else if(gl_iCutSpeed == 3)
				{
					dSpeedH = (double)(gl_iVSpeed-320) * BASE_PPM / (double)SPEED_M;   //350 325
					dAccTime = (double)(gl_iTimeAdd) * 0.001; 
				}
				else
				{
					dSpeedH = (double)(gl_iVSpeed-420 )* BASE_PPM / (double)SPEED_M;  // 450 300
					dAccTime = (double)(gl_iTimeAdd) * 0.001; 
				}
	//dSpeedH = (double)gl_iVSpeed * BASE_PPM / (double)SPEED_M;

	dAcc = (dSpeedH - dSpeedL) / dAccTime / 125.0;
	lAcc = IntRound(dAcc);
	//Printf("gl_iCutSpeed: %d SetLDPenSpeed: %d, dAccTime:%d\n",gl_iVSpeed-350, gl_iVSpeed, gl_iTimeAdd);
	//Printf("dSpeedL:%d dSpeedH:%d\n", (int)dSpeedL, (int)dSpeedH);
	//Printf("lAcc:%d gl_iCutSpeed:%d\n", lAcc, gl_iCutSpeed);
	//Printf("********\n");	
	set_startv(AXIS_Y+AXIS_X,(int)dSpeedL);
	set_speed(AXIS_Y+AXIS_X,(int)dSpeedH);
	set_acc(AXIS_Y+AXIS_X,(int)lAcc);
	set_dec(AXIS_Y+AXIS_X,(int)lAcc);
	set_ad_mode(AXIS_Y+AXIS_X,0);  		//S曲线线加/减速
	set_dec1_mode(AXIS_Y+AXIS_X,0);  	//对称加减速
	set_dec2_mode(AXIS_Y+AXIS_X,0);  	//自动减速
	inp_dec_enable();  					//插值减速允许
}

//设置短距离行程的笔速
void SetSDPenSpeed(void)
{
	double dSpeedL,dSpeedH,dAcc;
	double dAccTime;
	long lAcc;
	
	//X,Y轴的初始速度,速度,加速度,减速度
	dSpeedL = 10.0 * (double)BASE_PPM / (double)SPEED_M;	// 初始速度
	dSpeedH = 100.0 * (double)BASE_PPM / (double)SPEED_M;	// 驱动速度
	dAccTime = 1000.0 * 0.001;				// 加速时间 750
	/*
	if(gl_iCutSpeed == 0)
	{
		dAccTime = (double)(gl_iTimeAdd) * 0.001;
		dSpeedH = (double)(gl_iVSpeed-66) * BASE_PPM / (double)SPEED_M;	//20.4k
	}
	else if(gl_iCutSpeed == 1)
		{
			dSpeedH = (double)(gl_iVSpeed-gl_iTempATime ) * BASE_PPM / (double)SPEED_M;	//40.2
			dAccTime = (double)(gl_iTimeAdd+25) * 0.001;
		}
		else if(gl_iCutSpeed == 2)
			{
				dSpeedH = (double)(gl_iVSpeed-gl_iTempATime ) * BASE_PPM / (double)SPEED_M;		//60K
				dAccTime = (double)(gl_iTimeAdd+50) * 0.001;
			}
			else if(gl_iCutSpeed == 3)
				{
					dSpeedH = (double)(gl_iVSpeed-gl_iTempATime ) * BASE_PPM / (double)SPEED_M;
					dAccTime = (double)(gl_iTimeAdd+100) * 0.001; 
				}
				else
				{
					dSpeedH = (double)(gl_iVSpeed-600  )* BASE_PPM / (double)SPEED_M;
					dAccTime = (double)(gl_iTimeAdd+150) * 0.001; 
				}

	*/
	dAcc= (dSpeedH - dSpeedL) / dAccTime / 125.0;				// 加速度
	lAcc = IntRound(dAcc);
	//Printf("SetSDPenSpeed: %d, dAccTime:%d\n", gl_iVSpeed-gl_iTempATime , gl_iTimeAdd);
	//Printf("dSpeedL:%d dSpeedH:%d\n", (int)dSpeedL, (int)dSpeedH);
	//Printf("lAcc:%d gl_iCutSpeed:%d\n", lAcc, gl_iCutSpeed);
	//Printf("********\n");
	set_startv(AXIS_Y+AXIS_X,(int)dSpeedL);
	set_speed(AXIS_Y+AXIS_X,(int)dSpeedH);//
	set_acc(AXIS_Y+AXIS_X,(int)lAcc);
	set_dec(AXIS_Y+AXIS_X,(int)lAcc);
	set_ad_mode(AXIS_Y+AXIS_X,0);  		//直线加/减速
	set_dec1_mode(AXIS_Y+AXIS_X,0);  	//对称加减速
	set_dec2_mode(AXIS_Y+AXIS_X,0);  	//自动减速
	inp_dec_enable();  					//插值减速允许
	
}

//等待驱动结束
void WaitPrintEnd(void)
{

	int iStatus;
	do
	{
		//ReceiveData();
		get_inp_status(&iStatus);
	} while (iStatus != 0);
	

	while (ReadPrintEnd() == 0);
		//ReceiveData();				//接收数据

}

//读取插补驱动状态; 1:正在输出插补驱动脉冲
int GetInpDriveState(void)
{
	return ( (inpw(RR0) >> 8) & 0x01) == 0x01 ? 0x0 : 0x01 ;
}
/*
// 1正在输出脉冲 0脉冲输入完
int ReadPrintEnd(int axis)
{
	int iStatus;
	get_status(axis,&iStatus);
	return iStatus;
}
*/
// 等带驱动结束
void WaitDriveEnd(int axis)
{
	//while(ReadPrintEnd(axis)==1);
	int iStatus;
	do
	{
//		ReceiveData();	
		get_status(axis,&iStatus);
		//Printf("WaitDEnd[0x%x]\n", iStatus);
	}while(iStatus!=0);
}


void SuddenStop(int axis)
{
	outpw(WR0, (axis<<8) + 0x27);
}

void WriteCommand(int axis, char cmd)
{
	outpw(WR0, (axis<<8) + cmd);
}

int ReadData(void)
{
	long d6, d7;
	d6 = (long)inpw(RR6);
	d6 &= 0xffff;
	d7 = (long)inpw(RR7);
	d7 = (d7 & 0xffff) << 16;
	return (d6 | d7);;
}

int ReadRegister(int  cReg)
{
	return (inpw(cReg));
}

//*********20130807
void DecSpeedState(int axis)
{
	outpw(WR0, (axis<<8) + 0x26);
}

//连续驱动
void ContinuousDrive(int axis,long pulse)
{
	if (pulse >= 0)
	{
		set_pulse(axis,pulse);
		outpw(WR0,(axis<<8)+0x22);
		//Printf("A[%d]\n", pulse);
	}
	else
	{
		pulse = labs(pulse);
		set_pulse(axis,pulse);
		outpw(WR0,(axis<<8)+0x23);
		//Printf("B[%d]\n", pulse);
	}	
}


void SetInitAxisSpeed(char cAix, int iSpeed)
{
	double dSpeedL,dSpeedH,dAcc;
	double dAccTime;
	long lAcc;

	//Printf("Set move frame speed: %d\n", iSpeed);
	//X,Y轴的初始速度,速度,加速度,减速度
	dSpeedL = ((double)gl_iInitV0 * 600) / (double)SPEED_M;
	dAccTime = (double)(gl_iATime) * 0.001;
	dSpeedH = ((double)iSpeed * 600) / (double)SPEED_M;

	dAcc = (dSpeedH - dSpeedL) / dAccTime / 125.0;
	lAcc = IntRound(dAcc);
	set_startv(cAix,(int)dSpeedL);
	set_speed(cAix,(int)dSpeedH);
	set_acc(cAix,(int)lAcc);
	set_dec(cAix,(int)lAcc*10);
	set_ad_mode(cAix,0);  		//直线加/减速
	set_dec1_mode(cAix,1);  	//对称加减速
	set_dec2_mode(cAix,0);  	//自动减速
	inp_dec_enable();  
}

void RP_SetSingleAxisADSpeed(char cAix, int iSpeed)
{
	double dSpeedL,dSpeedH,dAcc;
	double dAccTime;
	long lAcc;

	//Printf("Set move frame speed: %d\n", iSpeed);
	
	//X,Y轴的初始速度,速度,加速度,减速度
	dSpeedL = ((double)gl_iInitV0 * BASE_PPM) / (double)SPEED_M;
	dAccTime = (double)(gl_iATime) * 0.001;
	dSpeedH = ((double)iSpeed * BASE_PPM) / (double)SPEED_M;

	dAcc = (dSpeedH - dSpeedL) / dAccTime / 125.0;
	lAcc = IntRound(dAcc);
	//Printf("V[%d], S[%d], A[%d]\n", (int)dSpeedL, (int)dSpeedH, (int)lAcc);
	set_startv(cAix,(int)dSpeedL);
	set_speed(cAix,(int)dSpeedH);
	set_acc(cAix,(int)lAcc);
	set_dec(cAix,(int)lAcc);
	set_ad_mode(cAix,0);  		//直线加/减速
	set_dec1_mode(cAix,0);  	//对称加减速
	set_dec2_mode(cAix,0);  	//自动减速
	inp_dec_enable();  
}

void RP_SetDoubleAxisADSpeed(int iSpeed)
{
	double dSpeedL,dSpeedH,dAcc;
	double dAccTime;
	long lAcc;

	//Printf("Set move frame speed: %d\n", iSpeed);
	
	//X,Y轴的初始速度,速度,加速度,减速度
	dSpeedL = ((double)gl_iInitV0 * BASE_PPM) / (double)SPEED_M;
	dAccTime = (double)(gl_iATime) * 0.001;
	dSpeedH = ((double)iSpeed * BASE_PPM) / (double)SPEED_M;

	dAcc = (dSpeedH - dSpeedL) / dAccTime / 125.0;
	lAcc = IntRound(dAcc);
	
	set_startv(AXIS_Y,(int)dSpeedL);
	set_speed(AXIS_Y,(int)dSpeedH);
	set_acc(AXIS_Y,(int)lAcc);
	set_dec(AXIS_Y,(int)lAcc);
	set_ad_mode(AXIS_Y,0);  		//直线加/减速
	set_dec1_mode(AXIS_Y,0);  	//对称加减速
	set_dec2_mode(AXIS_Y,0);  	//自动减速
	inp_dec_enable();  
}

// 两轴插补驱动
void TwoAxleInpDriveSpeed(int iSV, int iV, int iAddTime)
{
	double dSpeedL,dSpeedH,dAcc;
	double dAccTime;
	long lAcc;

	dSpeedL = ((double)iSV * BASE_PPM) / (double)SPEED_M;
	dAccTime = (double)(iAddTime) * 0.001;
	dSpeedH = ((double)iV * BASE_PPM) / (double)SPEED_M;

	dAcc = (dSpeedH - dSpeedL) / dAccTime / 125.0;
	lAcc = IntRound(dAcc);
	//Printf("TID: iSV[%d] iV[%d] iT[%d]ms\n",iSV, iV,  iAddTime);
	//Printf("TID: SV[%d] V[%d] AD[%d] T[%d]ms\n",(int)dSpeedL, (int)dSpeedH, (int)dAcc, iAddTime);
	set_startv(AXIS_Y,(int)dSpeedL);
	set_speed(AXIS_Y,(int)dSpeedH);
	set_acc(AXIS_Y,(int)lAcc);
	set_dec(AXIS_Y,(int)lAcc);
	set_ad_mode(AXIS_Y,0);  	//直线加/减速
	set_dec1_mode(AXIS_Y,0);  	//对称加减速
	set_dec2_mode(AXIS_Y,0);  	//自动减速
	inp_dec_enable(); 			//插补减速允许
}


// 单轴插补驱动速度
void SingleAxisInpDriveSpeed(int iAxis,int iSV, int iV, int iAddTime)
{
	double dSpeedL,dSpeedH,dAcc;
	double dAccTime;
	long lAcc;

	dSpeedL = ((double)iSV * BASE_PPM) / (double)SPEED_M;
	dAccTime = (double)(iAddTime) * 0.001;
	dSpeedH = ((double)iV * BASE_PPM) / (double)SPEED_M;

	dAcc = (dSpeedH - dSpeedL) / dAccTime / 125.0;
	lAcc = IntRound(dAcc);
	//Printf("SID: iSV[%d] iV[%d] iT[%d]ms\n",iSV, iV,  iAddTime);
//	Printf("SID: Axis[%d] SV[%d] V[%d] AD[%d] T[%d]\n",iAxis,(int)dSpeedL, (int)dSpeedH, (int)dAcc, iAddTime);

	set_startv(iAxis,(int)dSpeedL);
	set_speed(iAxis,(int)dSpeedH);
	set_acc(iAxis,(int)lAcc);
	set_dec(iAxis,(int)lAcc);
	set_ad_mode(iAxis,0);  		//直线加/减速
	set_dec1_mode(iAxis,0);  	//对称加减速
	set_dec2_mode(iAxis,0);  	//自动减速
	inp_dec_enable(); 			//插补减速允许
	
/*
	set_startv(iAxis,300);
	set_speed(iAxis,3750);
	set_acc(iAxis,60);
	set_dec(iAxis,60);
	set_ad_mode(iAxis,0);  	//直线加/减速
	set_dec1_mode(iAxis,0);  	//对称加减速
	set_dec2_mode(iAxis,0);  	//自动减速
	inp_dec_enable(); 			//插补减速允许
	*/
}

// 直线加减速
void StraightLineADSpeed(int iAxis, int iSV, int iV, int iAddTime)
{
	double dSpeedL,dSpeedH,dAcc;
	double dAccTime;
	long   lAcc;

	// 计算加速度 打印速度最高310mm
	dSpeedL = ((double)iSV * BASE_PPM) / (double)SPEED_M;
	dAccTime = (double)(iAddTime) * 0.001;
	dSpeedH = ((double)iV * BASE_PPM) / (double)SPEED_M;
	dAcc = (dSpeedH - dSpeedL) / dAccTime / 125.0;
	lAcc = IntRound(dAcc);
	//Printf("ZAD: iSV[%d] iV[%d] iT[%d]ms\n",iSV, iV,  iAddTime);
	//Printf("ZAD: Axis[%d] SV[%d] V[%d] AD[%d] T[%d]\n",iAxis,(int)dSpeedL, (int)dSpeedH, (int)dAcc, iAddTime);
	//初始速度
	set_startv(iAxis,(int)dSpeedL);
	//速度
	set_speed(iAxis,(int)dSpeedH);
	//加速度
	set_acc(iAxis,(int)lAcc);
	//速度
	set_dec(iAxis,(int)lAcc);
	//直线加/减速
	set_ad_mode(iAxis,0);  	
	//对称加减速
	set_dec1_mode(iAxis,0);  	
	//自动减速
	set_dec2_mode(iAxis,0);  		
}


// 固定线速度
void FixedSpeeDrive(int iAxis, int iSpeed)
{
	set_startv(iAxis,iSpeed);
	set_speed(iAxis,iSpeed);
	//Printf("FSD: Axis[%d] V[%d]\n",iAxis,iSpeed);
}

void SetReturnOrgSpeed(void)
{
	int iVSpeed[5] = {240, 260, 280, 300, 320};
	int iTime[5] = {1000, 1200, 1300, 1400, 1500};
	SingleAxisInpDriveSpeed(AXIS_X+AXIS_Y, Frame_SV, iVSpeed[gl_iMoveFramSpeed], iTime[gl_iMoveFramSpeed]);
}

//设置切割速度 连续插补驱动速度 //这里int i 用于速度速度选择如果是i = 0 就是普通的全局速度 如果不是 0 就是选择其他速度
void SetContinInpCutSpeed(int i )//int iAXIS
{
	//int iVSpeed[3] = {200, 350, 500};
	//int iTime[3] = {500, 650, 800};
	//SingleAxisInpDriveSpeed(iAXIS,5, iVSpeed[gl_iCutSpeed], iTime[gl_iCutSpeed]);
	//SingleAxisInpDriveSpeed(iAXIS,10, gl_iVSpeed, gl_iTimeAdd);
	int iVSpeed, iTime;
	if( i == 0)
		i = gl_iCutSpeed;
	//Printf("UP MOVE****\n");
	switch(i)
	{
		case 0:
			iTime = 3000;
			iVSpeed =  40;  
		break;

		case 1:
			iTime = 700;
			iVSpeed =  200;  
		break;

		case 2:
			iTime = 700;
		    iVSpeed =  200;  	//120k
		break;

		case 3:
			iTime = gl_iTimeAdd;
			iVSpeed =  gl_iVSpeed-220;	//120k
		break;

		case 4:
			iTime = gl_iTimeAdd;
			iVSpeed =  gl_iVSpeed;//-275;	//180k
		break;

		default:
			iTime = gl_iTimeAdd;
			iVSpeed =  gl_iVSpeed-300;	//240k
		break;
	}
	SingleAxisInpDriveSpeed(AXIS_X+AXIS_Y,5, iVSpeed, iTime);
	//SingleAxisInpDriveSpeed(AXIS_Y,5, iVSpeed*2, iTime);//GL_rate//应当增大GL_rate以减小速度的影响//hzx
	//SingleAxisInpDriveSpeed(iAXIS,50, iVSpeed, iTime);//
		
}
/*
// 短距插补驱动
void SetSortDistInpCutSpeed(void)
{
	//int iVSpeed[3] = {100, 200, 500};
	//int iTime[3] = {400, 500, 800};
	//Printf("gl_iCutSpeed[%d]\n", gl_iCutSpeed);
	//SingleAxisInpDriveSpeed(AXIS_X+AXIS_Y,10, iVSpeed[gl_iCutSpeed], iTime[gl_iCutSpeed]);
	SingleAxisInpDriveSpeed(AXIS_X+AXIS_Y,10, 200, 500);
}

void SetLongDistInpCutSpeed(void)
{
	int iVSpeed[3] = {100, 350, 600};
	int iTime[3] = {300, 500, 800};
	SingleAxisInpDriveSpeed(AXIS_X+AXIS_Y,10, iVSpeed[gl_iCutSpeed], iTime[gl_iCutSpeed]);
}

*/
//设置打印速度
void SetPrintSpeed(int iAxis)
{
	StraightLineADSpeed(iAxis, Print_SV, Print_V, Print_T-100);
	//StraightLineADSpeed(iAxis, gl_iSVSpeed, gl_iVSpeed, gl_iTimeAdd);
	//FixedSpeeDrive(iAxis, 2000);
}

// 设置移框速度
void SetMoveFrameSpeed(int iAxis)
{
	int iVSpeed[5] = {50, 500, 1000, 1500, 2000};
	FixedSpeeDrive(iAxis, iVSpeed[gl_iMoveFramSpeed]);
	//int iTime[5] = {400, 500, 600, 700, 800};
	//StraightLineADSpeed(iAxis, Frame_SV, iVSpeed[gl_iMoveFramSpeed], iTime[gl_iMoveFramSpeed]);
}

void RestMcx312(void)
{
	Delay(1000);
	outpw(WR0,0x8000);		//Software reset
	Delay(100);//5000
	//指定轴XY	
	outpw(WR0,0x30f);

	//停止信号STOP0,STOP1,STOP2均为无效
	WR1_value = 0x8080;
	outpw(WR1,WR1_value);//Delay(10000); 0x8080
	//限位信号LMT+,LMT-为低电平有效，立即停止 / 减速停止
	//软件限位无效
	//脉冲输出方式为：脉冲＋方向方式
	//反馈输入为A/B相编码脉冲输入，1倍频
	//伺服到位信号nINPOS无效
	//伺服报警信号nALARM无效
	WR2_value = 0x0040;
	outpw(WR2,WR2_value);//Delay(10000); 0x0040
	//加速方式为直线加/减速，对称加/减速，自动减速
	//计数器的可变环功能无效
	//输入滤波无效
	//阻止三角波输入
	WR3_value = 0x0020;
	outpw(WR3,WR3_value);//Delay(10000);0x0020
	
	outpw(WR4,0x0000);//Delay(10000);
	outpw(WR5,0x0100);//Delay(10000);
	//倍率20，驱动速度400KPPS 倍率16，驱动速度500KPPS
	//Printf("Range X: \n");
	set_range(AXIS_X,IntRound((double)(1.414 * 8000000.0 / SPEED_M)));//Delay(10000);
	//Printf("Range Y: \n");
	set_range(AXIS_Y,IntRound((double)(8000000.0 / SPEED_M)));//Delay(10000);
	
	/*
	 以下为cw/CCW双脉冲驱动方式  //配合 驱动器参数
	*/
	#ifdef ccw
	outpw(WR0,0x10f);   /* set x axis only.. */
	outpw(WR2,0x0);//Delay(10000)
	#endif
}

void WaitInpEnd(void)
{
	int iStatus;
	
	do
	{
//		ReceiveData();	
		get_inp_status(&iStatus);
	} while (iStatus != 0);
}

void SetAxisSpeed(int iSpeed)
{
	double dSpeedL,dSpeedH,dAcc;
	double dAccTime;
	long lAcc;
	
	//X,Y轴的初始速度,速度,加速度,减速度
	dSpeedL = (double)gl_iInitV0 * BASE_PPM / (double)SPEED_M;
	dAccTime = (double)(gl_iATime) * 0.001;

	dSpeedH = (double)iSpeed * BASE_PPM / (double)SPEED_M;

	dAcc = (dSpeedH - dSpeedL) / dAccTime / 125.0;
	lAcc = IntRound(dAcc);
	//Printf("LD V[%d], S[%d], A[%d]\n", (int)dSpeedL, (int)dSpeedH, (int)lAcc);
	set_startv(AXIS_Y,(int)dSpeedL);
	set_speed(AXIS_Y,(int)dSpeedH);
	set_acc(AXIS_Y,(int)lAcc);
	set_dec(AXIS_Y,(int)lAcc);
	set_ad_mode(AXIS_Y,0);  		//S曲线线加/减速
	set_dec1_mode(AXIS_Y,0);  	//对称加减速
	set_dec2_mode(AXIS_Y,0);  	//自动减速
	inp_dec_enable();  					//插值减速允许
}
//***********************

