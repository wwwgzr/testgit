
#include    <stdio.h> 
#include "..\src\defconst.h" 
//-----mcx312寄存器地址定义 
#define  adr     0x06000000   //基本地址 
/*
#define  wr0     0x0    //命令寄存器 
#define  wr1     0x2    //模式寄存器1 
#define  wr2     0x4    //模式寄存器2 
#define  wr3     0x6    //模式寄存器3 
#define  wr4     0x8    //输出寄存器 
#define  wr5     0xa    //插补模式寄存器 
#define  wr6     0xc    //低位写入数据寄存器 
#define  wr7     0xe    //高位写入数据寄存器 
#define  rr0      0x0    //主状态寄存器 
#define  rr1      0x2    //状态寄存器1 
#define  rr2      0x4    //状态寄存器2 
#define  rr3      0x6    //状态寄存器3 
#define  rr4      0x8    //输入寄存器1 
#define  rr5      0xa    //输入寄存器2 
#define  rr6      0xc    //低位读出数据寄存器 
#define  rr7      0xe    //高位读出数据寄存器 
#define  bp1p 0x4    //BP第X轴+方向数据寄存器 
#define  bp1m  0x6    //BP第X轴-方向数据寄存器 
#define  bp2p 0x8    //BP第Y轴+方向数据寄存器 
#define  bp2m  0xa    //BP第Y轴-方向数据寄存器 
//   wreg1(轴指定,数据)------------ 写入寄存器1
extern void Printf(char * f,...);
extern void Delay(int time);
extern void WriteMcxData(unsigned long address, int data);
extern int ReadMcxData(int address);
extern unsigned short ReadRegister(int cReg);
#define MCX312csAddr 0x06000000
static void outpw(unsigned long address,int wdata)
{
	//address += MCX312csAddr;
	//WriteMcxData(address,wdata);
	*((volatile unsigned short *)(MCX312csAddr+(address)))=(unsigned short)wdata;
	//address = wdata;
	//Printf("W address:0x%x, wdata:0x%x\n", address, wdata);
	//Delay(50);
}

static unsigned int inpw(long address)
{
	return ((unsigned int)ReadMcxData(address));Delay(50);
}

static void wreg1(int axis, int wdata)  
{
	outpw(adr+wr0, (axis<<8) + 0xf); //轴指定 
	outpw(adr+wr1, wdata);  
}  
  
//   wreg2(轴指定,数据)------------- 写入寄存器2
static void wreg2(int axis, int wdata) 
{
	outpw(adr+wr0,(axis<<8) + 0xf); //轴指定 
	outpw(adr+wr2,wdata); 
} 
 
//   wreg3(轴指定,数据)------------ 写入寄存器3
static void wreg3(int axis, int wdata)  
{ 
outpw(adr+wr0,(axis<<8) + 0xf); //轴指定 
outpw(adr+wr3, wdata);  
}  
//  command(轴指定,命令编码)----- 命令写入 
  
void command(int axis, int cmd)  
{  
outpw(adr+wr0,(axis<<8) + cmd);  
}  
//  range(轴指定,数据)---------------- 范围 R 设
void range(int axis, long wdata)  
{  
outpw(adr+wr7,(wdata>>16)&0xffff); 
outpw(adr+wr6,wdata & 0xffff); 
outpw(adr+wr0,(axis<<8)+ 0x00); 
}  
//   acac(轴指定,数据)---------------- 变化率(K)设定
void acac(int axis, int wdata) 
{  
outpw(adr+wr6,wdata);  
outpw(adr+wr0,(axis<<8) + 0x01);  
}  
 
//   acc(轴指定,数据)------------------ 加/减速度(A)
  
void acc(int axis, int wdata)  
{  
outpw(adr+wr6,wdata);  
outpw(adr+wr0,(axis<<8) + 0x02);  
} 
 
//   dec(轴指定,数据) ------------------- 减速度 D 设定 
void dec(int axis, int wdata) 
{
outpw(adr+wr6,  wdata); 
outpw(adr+wr0,(axis<<8)+ 0x03); 
} 
 
//   startv(轴指定,数据) -----------------  初始速度 SV 设定 
void startv(int axis, int wdata) 
{ 
outpw(adr+wr6,wdata);  
outpw(adr+wr0,(axis<<8)+ 0x04);  
}  
  //   speed(轴指定,数据) ----------------- 驱动速度 V 设定 
  void speed(int axis, int wdata)  
{  
outpw(adr+wr6,wdata);  
outpw(adr+wr0,(axis<<8)+ 0x05); 
}  
  //   pulse(轴指定,数据) ----------------- 输出脉冲数/终点 P 设定 
  void pulse(int axis, long wdata) 
{     
outpw(adr+wr7,(wdata>>16)& 0xffff); 
outpw(adr+wr6,wdata & 0xffff); 
outpw(adr+wr0,(axis<<8) + 0x06);  
} 
//   decp(轴指定,数据) ----------------- 手动减速点 DP 设定 
void decp(int axis, long wdata) 
{ 
outpw(adr+wr7,(wdata>>16)& 0xffff); 
outpw(adr+wr6,wdata & 0xffff);  
outpw(adr+wr0,(axis<<8) + 0x07);  
} 
 
//   center(轴指定,数据) ---------------- 圆弧中心点 C 设定 
 void center(int axis, long wdata)  
{ 
outpw(adr+wr7,(wdata>>16)& 0xffff); 
outpw(adr+wr6,wdata & 0xffff); 
outpw(adr+wr0,(axis<<8) + 0x08); 
} 
  //   lp(轴指定,数据) -------------------- 逻辑位置计数器(LP)设定 
void lp(int axis, long wdata)  
{  
outpw(wr7,(wdata>>16)& 0xffff);  
outpw(wr6,wdata & 0xffff);  
outpw(wr0,(axis<<8) + 0x09);    
}
//   ep(轴指定,数据) ------------------ 逻辑位置计数器(EP)设定 
void ep(int axis, long wdata)  
{  
outpw(adr+wr7,(wdata>>16)&0xffff); 
outpw(adr+wr6,wdata & 0xffff); 
outpw(adr+wr0,(axis<<8) + 0x0a); 
}  
//   compp(轴指定,数据) ------------ COMP + (CP) 设定 
void compp(int axis, long wdata) 
{  
outpw(adr+wr7,(wdata>>16)&0xffff);  
outpw(adr+wr6,wdata & 0xffff);  
outpw(adr+wr0,(axis<<8) + 0x0b);  
} 
  
//   compm(轴指定,数据) ----------- COMP - (CM) 设定 
void compm(int axis, long wdata)  
{  
outpw(adr+wr7,(wdata>>16)&0xffff);  
outpw(adr+wr6,wdata & 0xffff);  
outpw(adr+wr0,(axis<<8) + 0x0c);  
} 
//   accofst(轴指定,数据) ----------- 加速计数器偏移(AO)设定 
static void accofst(int axis, long wdata)
{
	outpw(adr+wr7,(wdata>>16)&0xffff);
	outpw(adr+wr6,wdata&0xffff);
	outpw(adr+wr0,(axis<<8)+0x0d);
}  
 //   readep(轴指定)-------------------- 逻辑位置计数器数值(EP)读
long readep(int axis)  
{ 
	long a; 
	long d6; 
	long d7; 
	outpw(adr+wr0,(axis<<8) + 0x11); 
	d6 = inpw(adr+rr6);
	d7 = inpw(adr+rr7); 
	a = d6 + (d7<<16); 
	return(a); 
} 
  
//   wait(轴指定)------------------------  等驱动结束 
  
void wait(int axis) 
{  
	while(inpw(adr+rr0) & axis);  
}  
  
 
//   next_wait( )--------------------------  等连续插补下一个数据设定
//  
void next_wait(void)  
{  
	while((inpw(adr+rr0) & 0x0200) == 0); 
}   

//   bp_wait( )----------------------------  等BP插补下一个数据设定
void bp_wait(void) 
{  
	while((inpw(adr+rr0) & 0x6000) == 0x6000); 
} 
 
//   homesrch( )---------------------  全轴原点检出 
void homesrch(void)  
{ //[动作]   (1)~(3)各轴都相同 
	wreg1(0xf, 0x0008); //(1)原点信号 STOP1 关闭时
	speed(0xf, 2000); //(1)用速度20,000PPS-连续驱动
	if((inpw(adr+rr4) & 0x2) == 0x2)  //(1)STOP1信号ON后,减速停
	{  
		command(0x1, 0x23);   
  } 
	if((inpw(adr+rr4) & 0x200) == 0x200) 
	{  
		command(0x2, 0x23);  
	}  
	wait(0x3);  
	wreg1(0x3, 0x000c); //(2)用速度500PPS+连续驱动 
	speed(0x3, 50); //(1)STOP1信号OFF后,立即停止
	command(0x3, 0x22);  
	wait(0x3);  
	wreg1(0x3, 0x0000); //(3)用速度40,000PPS向-方向
	speed(0x3, 4000); //(1)100脉冲偏移移动 
	pulse(0x3, 100); 
	command(0x3, 0x21); 
	wait(0x3); 
	lp(0x3, 0); //(4) X, Y轴LP = 0 
	wreg2(0x3, 0x0003); //(1) X, Y轴软件限制:ON 
	compp(0x1, 100000); //(1) X: -1,000 ~ +100,000 
	compm(0x1, -1000);
	compp(0x2, 50000); //(1) Y: -500 ~ +50,000 
	compm(0x2, -500);  
} 



static void WriteCommand(int axis, char cmd)
{
	outpw(wr0, (axis<<8) + cmd);
}

static int ReadData(void)
{
	int Data;
	Data = inpw(rr7);
	Printf("R7:%x\n", Data);
	Data = inpw(rr6);
	Printf("R6:%x\n", Data);
	return Data;
}


void testMcx312(void) 
{ 
	int count; 
 	int readPlus;
	outpw(adr+wr0, 0x8000); //软件复位 
	for(count = 0; count < 2; ++count); 
	command(0x3, 0xf); //------------X,Y
	outpw(adr+wr1, 0x0000); //模式寄存器1
	outpw(adr+wr2, 0x0000); //模式寄存器2
	outpw(adr+wr3, 0x0000); //模式寄存器3
	outpw(adr+wr4, 0x0000); //通用输出寄存
	outpw(adr+wr5, 0x0024); //插补模式寄存  
 //--------X,Y轴
 	
	accofst(0x3, 0); //A0 = 0 
	range(0x3, 800000); //R = 800,000(倍
	acac(0x3, 1010); //K = 1,010(变化
	acc(0x3, 100); //A = 100(加/减
	dec(0x3, 100); //D = 100(减速
	startv(0x3, 100); //SV = 100(初始
	speed(0x3, 4000); //V = 4,000(驱动

	//pulse(0x3, 100000); //P = 100,000(输
	

	readPlus = ReadRegister(rr0);
	Printf("RR0 Reg:0x%x\n", readPlus);
	readPlus = ReadRegister(rr1);
	Printf("RR1 Reg:0x%x\n", readPlus);
	readPlus = ReadRegister(rr2);
	Printf("RR2 Reg:0x%x\n", readPlus);	
	 WriteCommand(3, 0x12);
	 ReadData();

	lp(0x1, 100); //LP = 0(逻辑位
	Delay(20);
	WriteCommand(0x1, 0x10);

	readPlus = ReadData();

	Printf("10 plus:%d\n\n", readPlus);

 	homesrch( ); //------X,Y轴原   

	//------X Y轴
	acc(0x3, 200) //A=200(加/减速
	speed(0x3, 4,000); //V = 4,000(驱动
	pulse(0x1, 80,000); //xP = 80,000 
	pulse(0x2, 40,000); //yP = 40,000 
	command(0x3, 0x20); //+定量驱动 
	wait(0x3);  //等驱动结束 
	 
	wreg3(0x3, 0x0,004); //------X Y轴
	acac(0x3, 1,010); //K= 1,010(变化
	acc(0x3, 200); //A=200(加/减速
	speed(0x3, 4,000); //V = 4,000(驱动
 	pulse(0x1, 50,000); xP = 50,000 
	pulse(0x2, 25,000); yP = 25,000 
	command(0x3, 0x21); //-定量驱动 
	wait(0x3);  
	wreg3(0x3, 0x0000); //S曲线加/减速
 	//---------2轴直
	outpw(adr+wr5, 0x0124); //线速固定 
	range(0x1, 800,000); xR = 800,000(倍
	range(0x2, 1,131,371); yR = 800,000×1
 	startv(0x1,100) //SV=100(初始
	speed(0x1, 100); //V=100驱动速
	//xP = +5,000(终pulse(0x1, 5,000); //yP = -2,000(终pulse(0x2, -2,000); 
	command(0x0, 0x30); //2轴直线插补
	wait(0x3);  
	  
 	//---------CCW轴
	outpw(adr+wr5, 0x0100); //线速常数 
	range(0x1, 800,000); //xR = 800,000(range(0x2, 1,131,371); //yR = 800,000×startv(0x1,100) //V=100驱动速
	speed(0x1, 100); //xC = -5,000(中
	center(0x1, -5,000); //yC = 0    (中心
	center(0x2, 0); //xP = 0    (终点
	pulse(0x1, 0); //yP = 0    (终点
	pulse(0x2, 0); //CCW圆弧插补
	command(0x0, 0x33); 
	wait(0x3); 
	//---------X Y轴
	// 图2.30例子
	//V=1驱动速度 
 	//位模式数据写
	speed(0x1, 1); 
	command(0, 0x36); 
	outpw(adr+bp1p,0x0000); //0~15位模式数
	outpw(adr+bp1m, 0x2bff);  
 	outpw(adr+bp2p, 0xffd4); 
 	outpw(adr+bp2m,0x0000); 
	command(0, 0x38); //堆栈 
	outpw(adr+bp1p, 0xf6fe); //16~31位数据
	outpw(adr+bp1m,0x0000);  
	outpw(adr+bp2p,0x000f);  outpw(adr+bp2m,0x3fc0);  command(0, 0x38);  

	outpw(adr+bp1p, 0x1fdb); //32 ~ 47位数据写入 
	outpw(adr+bp1m, 0x0000);  
	outpw(adr+bp2p, 0x00ff);  
	outpw(adr+bp2m, 0xfc00);  
	command(0, 0x38); 
	command(0, 0x34); //2轴BP插补驱动开始 
	bp_wait( ); //数据等待写入 
	outpw(adr+bp1p, 0x4000); //48 ~ 63位数据写入 
	outpw(adr+bp1m, 0x7ff5);  
	outpw(adr+bp2p, 0x0000);  
	outpw(adr+bp2m, 0x0aff);  
	command(0, 0x38);
	command(0, 0x37); //不允许位模式数据写入 
	wait(0x3);// 等待驱动结束 
 	//------X, Y轴连续插补 图
	speed(0x1, 100); //100驱动速度 = 1,000PPS
	pulse(0x1, 4,500); //node1 
	pulse(0x2, 0);  
	command(0, 0x30);  
	next_wait( ); //次数据等待 center(0x1, 0); //node2 center(0x2, 1,500);  pulse(0x1, 1,500);  pulse(0x2, 1,500);  command(0, 0x33);  
	next_wait( ); 
	pulse(0x1, 0);  pulse(0x2, 1,500); //node3 command(0, 0x30);  next_wait( );  center(0x1, -1,500); 
	center(0x2, 0);  
	pulse(0x1, -1,500); //node4 
	pulse(0x2, 1,500):;  
	command(0, 0x33);  
	 
	 
 
	next_wait( );  
	pulse(0x1, -4,500); //node5 
	pulse(0x2, 0);  
	command(0, 0x30); 
	next_wait( );  
	center(0x1, 0); //node6 
	center(0x2, -1,500); 
	pulse(0x1, -1,500); 
	pulse(0x2, -1,500); 
	command(0, 0x33); 
	next_wait( );  
	pulse(0x1, 0); //node7 
	pulse(0x2, -1,500); 
	command(0, 0x30); 
	next_wait( );  
	center(0x1, 1,500); //node8 
	center(0x2, 0); 
	pulse(0x1, 1,500); 
	pulse(0x2, -1,500); 
	command(0, 0x33); 
	wait(0x3);  

} 
*/
