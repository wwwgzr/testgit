
#include    <stdio.h> 
#include "..\src\defconst.h" 
//-----mcx312�Ĵ�����ַ���� 
#define  adr     0x06000000   //������ַ 
/*
#define  wr0     0x0    //����Ĵ��� 
#define  wr1     0x2    //ģʽ�Ĵ���1 
#define  wr2     0x4    //ģʽ�Ĵ���2 
#define  wr3     0x6    //ģʽ�Ĵ���3 
#define  wr4     0x8    //����Ĵ��� 
#define  wr5     0xa    //�岹ģʽ�Ĵ��� 
#define  wr6     0xc    //��λд�����ݼĴ��� 
#define  wr7     0xe    //��λд�����ݼĴ��� 
#define  rr0      0x0    //��״̬�Ĵ��� 
#define  rr1      0x2    //״̬�Ĵ���1 
#define  rr2      0x4    //״̬�Ĵ���2 
#define  rr3      0x6    //״̬�Ĵ���3 
#define  rr4      0x8    //����Ĵ���1 
#define  rr5      0xa    //����Ĵ���2 
#define  rr6      0xc    //��λ�������ݼĴ��� 
#define  rr7      0xe    //��λ�������ݼĴ��� 
#define  bp1p 0x4    //BP��X��+�������ݼĴ��� 
#define  bp1m  0x6    //BP��X��-�������ݼĴ��� 
#define  bp2p 0x8    //BP��Y��+�������ݼĴ��� 
#define  bp2m  0xa    //BP��Y��-�������ݼĴ��� 
//   wreg1(��ָ��,����)------------ д��Ĵ���1
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
	outpw(adr+wr0, (axis<<8) + 0xf); //��ָ�� 
	outpw(adr+wr1, wdata);  
}  
  
//   wreg2(��ָ��,����)------------- д��Ĵ���2
static void wreg2(int axis, int wdata) 
{
	outpw(adr+wr0,(axis<<8) + 0xf); //��ָ�� 
	outpw(adr+wr2,wdata); 
} 
 
//   wreg3(��ָ��,����)------------ д��Ĵ���3
static void wreg3(int axis, int wdata)  
{ 
outpw(adr+wr0,(axis<<8) + 0xf); //��ָ�� 
outpw(adr+wr3, wdata);  
}  
//  command(��ָ��,�������)----- ����д�� 
  
void command(int axis, int cmd)  
{  
outpw(adr+wr0,(axis<<8) + cmd);  
}  
//  range(��ָ��,����)---------------- ��Χ R ��
void range(int axis, long wdata)  
{  
outpw(adr+wr7,(wdata>>16)&0xffff); 
outpw(adr+wr6,wdata & 0xffff); 
outpw(adr+wr0,(axis<<8)+ 0x00); 
}  
//   acac(��ָ��,����)---------------- �仯��(K)�趨
void acac(int axis, int wdata) 
{  
outpw(adr+wr6,wdata);  
outpw(adr+wr0,(axis<<8) + 0x01);  
}  
 
//   acc(��ָ��,����)------------------ ��/���ٶ�(A)
  
void acc(int axis, int wdata)  
{  
outpw(adr+wr6,wdata);  
outpw(adr+wr0,(axis<<8) + 0x02);  
} 
 
//   dec(��ָ��,����) ------------------- ���ٶ� D �趨 
void dec(int axis, int wdata) 
{
outpw(adr+wr6,  wdata); 
outpw(adr+wr0,(axis<<8)+ 0x03); 
} 
 
//   startv(��ָ��,����) -----------------  ��ʼ�ٶ� SV �趨 
void startv(int axis, int wdata) 
{ 
outpw(adr+wr6,wdata);  
outpw(adr+wr0,(axis<<8)+ 0x04);  
}  
  //   speed(��ָ��,����) ----------------- �����ٶ� V �趨 
  void speed(int axis, int wdata)  
{  
outpw(adr+wr6,wdata);  
outpw(adr+wr0,(axis<<8)+ 0x05); 
}  
  //   pulse(��ָ��,����) ----------------- ���������/�յ� P �趨 
  void pulse(int axis, long wdata) 
{     
outpw(adr+wr7,(wdata>>16)& 0xffff); 
outpw(adr+wr6,wdata & 0xffff); 
outpw(adr+wr0,(axis<<8) + 0x06);  
} 
//   decp(��ָ��,����) ----------------- �ֶ����ٵ� DP �趨 
void decp(int axis, long wdata) 
{ 
outpw(adr+wr7,(wdata>>16)& 0xffff); 
outpw(adr+wr6,wdata & 0xffff);  
outpw(adr+wr0,(axis<<8) + 0x07);  
} 
 
//   center(��ָ��,����) ---------------- Բ�����ĵ� C �趨 
 void center(int axis, long wdata)  
{ 
outpw(adr+wr7,(wdata>>16)& 0xffff); 
outpw(adr+wr6,wdata & 0xffff); 
outpw(adr+wr0,(axis<<8) + 0x08); 
} 
  //   lp(��ָ��,����) -------------------- �߼�λ�ü�����(LP)�趨 
void lp(int axis, long wdata)  
{  
outpw(wr7,(wdata>>16)& 0xffff);  
outpw(wr6,wdata & 0xffff);  
outpw(wr0,(axis<<8) + 0x09);    
}
//   ep(��ָ��,����) ------------------ �߼�λ�ü�����(EP)�趨 
void ep(int axis, long wdata)  
{  
outpw(adr+wr7,(wdata>>16)&0xffff); 
outpw(adr+wr6,wdata & 0xffff); 
outpw(adr+wr0,(axis<<8) + 0x0a); 
}  
//   compp(��ָ��,����) ------------ COMP + (CP) �趨 
void compp(int axis, long wdata) 
{  
outpw(adr+wr7,(wdata>>16)&0xffff);  
outpw(adr+wr6,wdata & 0xffff);  
outpw(adr+wr0,(axis<<8) + 0x0b);  
} 
  
//   compm(��ָ��,����) ----------- COMP - (CM) �趨 
void compm(int axis, long wdata)  
{  
outpw(adr+wr7,(wdata>>16)&0xffff);  
outpw(adr+wr6,wdata & 0xffff);  
outpw(adr+wr0,(axis<<8) + 0x0c);  
} 
//   accofst(��ָ��,����) ----------- ���ټ�����ƫ��(AO)�趨 
static void accofst(int axis, long wdata)
{
	outpw(adr+wr7,(wdata>>16)&0xffff);
	outpw(adr+wr6,wdata&0xffff);
	outpw(adr+wr0,(axis<<8)+0x0d);
}  
 //   readep(��ָ��)-------------------- �߼�λ�ü�������ֵ(EP)��
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
  
//   wait(��ָ��)------------------------  ���������� 
  
void wait(int axis) 
{  
	while(inpw(adr+rr0) & axis);  
}  
  
 
//   next_wait( )--------------------------  �������岹��һ�������趨
//  
void next_wait(void)  
{  
	while((inpw(adr+rr0) & 0x0200) == 0); 
}   

//   bp_wait( )----------------------------  ��BP�岹��һ�������趨
void bp_wait(void) 
{  
	while((inpw(adr+rr0) & 0x6000) == 0x6000); 
} 
 
//   homesrch( )---------------------  ȫ��ԭ���� 
void homesrch(void)  
{ //[����]   (1)~(3)���ᶼ��ͬ 
	wreg1(0xf, 0x0008); //(1)ԭ���ź� STOP1 �ر�ʱ
	speed(0xf, 2000); //(1)���ٶ�20,000PPS-��������
	if((inpw(adr+rr4) & 0x2) == 0x2)  //(1)STOP1�ź�ON��,����ͣ
	{  
		command(0x1, 0x23);   
  } 
	if((inpw(adr+rr4) & 0x200) == 0x200) 
	{  
		command(0x2, 0x23);  
	}  
	wait(0x3);  
	wreg1(0x3, 0x000c); //(2)���ٶ�500PPS+�������� 
	speed(0x3, 50); //(1)STOP1�ź�OFF��,����ֹͣ
	command(0x3, 0x22);  
	wait(0x3);  
	wreg1(0x3, 0x0000); //(3)���ٶ�40,000PPS��-����
	speed(0x3, 4000); //(1)100����ƫ���ƶ� 
	pulse(0x3, 100); 
	command(0x3, 0x21); 
	wait(0x3); 
	lp(0x3, 0); //(4) X, Y��LP = 0 
	wreg2(0x3, 0x0003); //(1) X, Y���������:ON 
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
	outpw(adr+wr0, 0x8000); //�����λ 
	for(count = 0; count < 2; ++count); 
	command(0x3, 0xf); //------------X,Y
	outpw(adr+wr1, 0x0000); //ģʽ�Ĵ���1
	outpw(adr+wr2, 0x0000); //ģʽ�Ĵ���2
	outpw(adr+wr3, 0x0000); //ģʽ�Ĵ���3
	outpw(adr+wr4, 0x0000); //ͨ������Ĵ�
	outpw(adr+wr5, 0x0024); //�岹ģʽ�Ĵ�  
 //--------X,Y��
 	
	accofst(0x3, 0); //A0 = 0 
	range(0x3, 800000); //R = 800,000(��
	acac(0x3, 1010); //K = 1,010(�仯
	acc(0x3, 100); //A = 100(��/��
	dec(0x3, 100); //D = 100(����
	startv(0x3, 100); //SV = 100(��ʼ
	speed(0x3, 4000); //V = 4,000(����

	//pulse(0x3, 100000); //P = 100,000(��
	

	readPlus = ReadRegister(rr0);
	Printf("RR0 Reg:0x%x\n", readPlus);
	readPlus = ReadRegister(rr1);
	Printf("RR1 Reg:0x%x\n", readPlus);
	readPlus = ReadRegister(rr2);
	Printf("RR2 Reg:0x%x\n", readPlus);	
	 WriteCommand(3, 0x12);
	 ReadData();

	lp(0x1, 100); //LP = 0(�߼�λ
	Delay(20);
	WriteCommand(0x1, 0x10);

	readPlus = ReadData();

	Printf("10 plus:%d\n\n", readPlus);

 	homesrch( ); //------X,Y��ԭ   

	//------X Y��
	acc(0x3, 200) //A=200(��/����
	speed(0x3, 4,000); //V = 4,000(����
	pulse(0x1, 80,000); //xP = 80,000 
	pulse(0x2, 40,000); //yP = 40,000 
	command(0x3, 0x20); //+�������� 
	wait(0x3);  //���������� 
	 
	wreg3(0x3, 0x0,004); //------X Y��
	acac(0x3, 1,010); //K= 1,010(�仯
	acc(0x3, 200); //A=200(��/����
	speed(0x3, 4,000); //V = 4,000(����
 	pulse(0x1, 50,000); xP = 50,000 
	pulse(0x2, 25,000); yP = 25,000 
	command(0x3, 0x21); //-�������� 
	wait(0x3);  
	wreg3(0x3, 0x0000); //S���߼�/����
 	//---------2��ֱ
	outpw(adr+wr5, 0x0124); //���ٹ̶� 
	range(0x1, 800,000); xR = 800,000(��
	range(0x2, 1,131,371); yR = 800,000��1
 	startv(0x1,100) //SV=100(��ʼ
	speed(0x1, 100); //V=100������
	//xP = +5,000(��pulse(0x1, 5,000); //yP = -2,000(��pulse(0x2, -2,000); 
	command(0x0, 0x30); //2��ֱ�߲岹
	wait(0x3);  
	  
 	//---------CCW��
	outpw(adr+wr5, 0x0100); //���ٳ��� 
	range(0x1, 800,000); //xR = 800,000(range(0x2, 1,131,371); //yR = 800,000��startv(0x1,100) //V=100������
	speed(0x1, 100); //xC = -5,000(��
	center(0x1, -5,000); //yC = 0    (����
	center(0x2, 0); //xP = 0    (�յ�
	pulse(0x1, 0); //yP = 0    (�յ�
	pulse(0x2, 0); //CCWԲ���岹
	command(0x0, 0x33); 
	wait(0x3); 
	//---------X Y��
	// ͼ2.30����
	//V=1�����ٶ� 
 	//λģʽ����д
	speed(0x1, 1); 
	command(0, 0x36); 
	outpw(adr+bp1p,0x0000); //0~15λģʽ��
	outpw(adr+bp1m, 0x2bff);  
 	outpw(adr+bp2p, 0xffd4); 
 	outpw(adr+bp2m,0x0000); 
	command(0, 0x38); //��ջ 
	outpw(adr+bp1p, 0xf6fe); //16~31λ����
	outpw(adr+bp1m,0x0000);  
	outpw(adr+bp2p,0x000f);  outpw(adr+bp2m,0x3fc0);  command(0, 0x38);  

	outpw(adr+bp1p, 0x1fdb); //32 ~ 47λ����д�� 
	outpw(adr+bp1m, 0x0000);  
	outpw(adr+bp2p, 0x00ff);  
	outpw(adr+bp2m, 0xfc00);  
	command(0, 0x38); 
	command(0, 0x34); //2��BP�岹������ʼ 
	bp_wait( ); //���ݵȴ�д�� 
	outpw(adr+bp1p, 0x4000); //48 ~ 63λ����д�� 
	outpw(adr+bp1m, 0x7ff5);  
	outpw(adr+bp2p, 0x0000);  
	outpw(adr+bp2m, 0x0aff);  
	command(0, 0x38);
	command(0, 0x37); //������λģʽ����д�� 
	wait(0x3);// �ȴ��������� 
 	//------X, Y�������岹 ͼ
	speed(0x1, 100); //100�����ٶ� = 1,000PPS
	pulse(0x1, 4,500); //node1 
	pulse(0x2, 0);  
	command(0, 0x30);  
	next_wait( ); //�����ݵȴ� center(0x1, 0); //node2 center(0x2, 1,500);  pulse(0x1, 1,500);  pulse(0x2, 1,500);  command(0, 0x33);  
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
