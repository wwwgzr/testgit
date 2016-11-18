#include "..\inc\defconst.h"
#include "..\Target\44blib.h"
#include "..\inc\extern.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void lcd_init(void);
void lcd_clr(void);
void char_reverse(unsigned char line);
void lcd_refresh(void);
void lcd_display(unsigned char row,const char *string);
void KBProc(void);
void MenuProc(void);
void SetOriginalPos(void);
void SelectLanguage(void);
void SelectPlotType(void);
void SetupIPAddr(unsigned short *sNetWorkValue);
void disp_menu(unsigned char cFirstMenuNo,unsigned char cInverseMenuNo);
void char_reverse(unsigned char line);
void LineReverse(unsigned char line);
void WaitKeyBound(unsigned char cKeyValue);
void WaitKeyExit(unsigned char cKeyValue);
void VersionDisplay(void);
int	 VerifySecrity(void);
void DealPopMenu(void);
void DispPopMenu(unsigned char cFirstMenuNo,unsigned char cInverseMenuNo);
void ViewError(int iErrorCode);
void WaitPrintEnd(void);
void ReactorCheck(void);
void KeyboardCheck(void);
void DeleteCurFile(void);
void TestRecSendMotor(unsigned char cFlag);
void SetPrintHeadCleanCount(void);

extern void InitPaper(void);
extern void ReceiveData(void);
extern void DrawTestGraph(void);
extern void DriveSprinkler(int iPulseNumX,int iPulseNumY);
extern void	CalVariableValue(void);
extern void ClearPrintHead(unsigned int iFlag);
extern int WriteParameter(void);
extern unsigned char GetIOBit(unsigned char cBit);
extern void SetIOBit(unsigned char cBit,unsigned char cEnable);

#define CleanCommand()			*((volatile unsigned short *)(WritePrintHeadCleanAddr))=0x0		// 清楚清洗指令
#define WritePrintHeadCleanAddr    0x8000104  // 打印头清洗地址20110414

//LCD初始化设置
void lcd_init(void)
{
	gl_scankeyflag = 0;
	Delay(50);
	WriteLcdCommand(0x30); 		//发送8位控制命令00110000
	WriteLcdCommand(0x30); 		//发送8位控制命令00110000
	WriteLcdCommand(0xc); 			//发送开显示关光标命令00001100
	WriteLcdCommand(0x1);			//发送清除显示命令00000001
	Delay(200);
	WriteLcdCommand(0x06);
	gl_cLCDStatus = 0;
#if( LANGUAGE == 0 )
	strcpy(gl_cLcdDispBuf[0],"    初始化中");
	strcpy(gl_cLcdDispBuf[1],"    请稍后...");
#elif(LANGUAGE==1)
	strcpy(gl_cLcdDispBuf[0],"system initing");
	strcpy(gl_cLcdDispBuf[1],"please waiting!");
#elif(LANGUAGE==2)
	strcpy(gl_cLcdDispBuf[0],"Inicializando");
	strcpy(gl_cLcdDispBuf[1],"Espere...!");
#elif(LANGUAGE==3)
	strcpy(gl_cLcdDispBuf[0],"Sistem Kontrol");
	strcpy(gl_cLcdDispBuf[1],"Lutfen Bekleyin!");
#else
		strcpy(gl_cLcdDispBuf[0],"Inicializando");
		strcpy(gl_cLcdDispBuf[1],"PorFavorAguarde!");
#endif
	lcd_refresh();
	gl_scankeyflag = 1;
}

//清屏（清除屏幕上所有数据）
void lcd_clr(void)
{
	gl_scankeyflag = 0;
	Delay(50);
	WriteLcdCommand(0x1); 			//发送清除显示命令00000001
	Delay(200);
	gl_scankeyflag = 1;
}

//设置字符反白行
void char_reverse(unsigned char line)
{
	//gl_scankeyflag = 0;
	Delay(50);
	line &= 0x03;
	line |= 0x04;
	WriteLcdCommand(0x34);
	WriteLcdCommand(line);
	WriteLcdCommand(0x30);
	WriteLcdCommand(0x30);
	//gl_scankeyflag = 1;
}

//设置字符行显示状态（第1位代表第0行,第2位代表第1行;值为0不反白,为0反白)
void LineReverse(unsigned char line)
{
	gl_scankeyflag = 0;
	Delay(50);
	if (line == 0x80)
	{
		if ((gl_cLCDStatus&LCD_Line0) != 0)
			char_reverse(0);
		if ((gl_cLCDStatus&LCD_Line1) != 0)
			char_reverse(1);
	}
	else if (line == 0x40)
	{
		if ((gl_cLCDStatus&LCD_Line0) != 0)
			char_reverse(0);
		if ((gl_cLCDStatus&LCD_Line1) != 0)
			char_reverse(1);
	}
	else
	{
		line = line & 0x03;
		if (line == 0)
		{
			if ((gl_cLCDStatus&LCD_Line0) != 0)
			{
				char_reverse(0);
				gl_cLCDStatus &= ~LCD_Line0;
			}
			if ((gl_cLCDStatus&LCD_Line1) != 0)
			{
				char_reverse(1);
				gl_cLCDStatus &= ~LCD_Line1;
			}
		}
		else if (line == 1)
		{
			if ((gl_cLCDStatus&LCD_Line1) != 0)
			{
				char_reverse(1);
				gl_cLCDStatus &= ~LCD_Line1;
			}
			if ((gl_cLCDStatus&LCD_Line0) == 0)
			{
				char_reverse(0);
				gl_cLCDStatus |= LCD_Line0;
			}
		}
		else if (line == 2)
		{
			if ((gl_cLCDStatus&LCD_Line0) != 0)
			{
				char_reverse(0);
				gl_cLCDStatus &= ~LCD_Line0;
			}
			if ((gl_cLCDStatus&LCD_Line1) == 0)
			{
				char_reverse(1);
				gl_cLCDStatus |= LCD_Line1;
			}
		}
	}
	gl_scankeyflag = 1;
}

//**************************************************************
//功能：	屏幕刷新
//入口参数：无
//返回值：无
//**************************************************************
void lcd_refresh(void)
{
	unsigned short i,j;
	char *string;
	gl_scankeyflag = 0;//20100222
	Delay(50);
	j = 0;
	while (j < 2)
	{
		WriteLcdCommand(0x80 + j * 16);
		string = &gl_cLcdDispBuf[j][0];
		j++;
		for (i=0;i<16;i++)
		{
			if (*string == '\0')
			{
				WriteLcdData(' ');
			}
			else
			{
				WriteLcdData(*string);
				string++;
			}
		}
	}
	WriteLcdCommand(0xc); //20100222
	gl_scankeyflag =1 ;
}

//**************************************************************
//功能：	在指定位置显示数据
//入口参数：row显示行，col显示列，string显示字符串
//返回值：无
//**************************************************************
void lcd_display(unsigned char row,const char *string)
{
	unsigned short i;
	gl_scankeyflag = 0;//20100222
	Delay(50);//20100222
	if (row < 2)
	{
		//汉字字符为2行7.5列(汉字必须偶数对齐)
		//发送设定DDRAM地址row * 16命令
		WriteLcdCommand(0x80 + row * 16);
		for (i=0;i<16;i++)
		{
			if (*string == '\0')
			{
				WriteLcdData(' ');
			}
			else
			{
				WriteLcdData(*string);
				string++;
			}
		}
	}
	WriteLcdCommand(0xc); //20100222
	gl_scankeyflag =1 ;
}

//高级菜单显示
void DispPopMenu(unsigned char cFirstMenuNo,unsigned char cInverseMenuNo)
{
#if( LANGUAGE == 0 )	
	const char cPopMenu[11][17] = {{"机型选择"},{"IP地址设置"},{"子网掩码设置"},{"网关设置"},{"送纸电机检测"},{"收纸电机检测"},{"喷头数设置"}, {"走纸长度设置"}, {"送纸感应器延时"}, {"电机类型设置"}, {"Ｆｌａｓｈ设置"}};// 20101214 喷头选择
#elif(LANGUAGE==1)
	const char cPopMenu[11][17] = {{"Plotter types"},{"IP Address"},{"SubnetMask Addr"},{"GatewayIP Addr"},{"Feed control"},{"Roll control"},{"Print Set"},{"Send paper set"},{"Inductor delay"},{"Motor type set"}, {"Flash chip set"}};
#elif(LANGUAGE==2)
	const char cPopMenu[11][17] = {{"Tipo de plotter"},{"Direccion IP"},{"Mascara sub red"},{"Puerta d enlace"},{"Control avance"},{"Control enrolla"},{"Configurar"},{"Envio de papel"},{"Tiempo inductor"},{"Tipo de motor"}, {"Chip flash"}};
#elif(LANGUAGE==3)
	const char cPopMenu[11][17] = {{"Cizici Modeli"},{"IP Adresi"},{"Ag Alt Geciti"},{"Varsay-Ag Geciti"},{"Kagit-Bes-Kntrl"},{"On-Bar-Kontrol"},{"Cizim Ayarlari"},{"Kagit-Gond-Ayar"},{"Induktor Ayar"},{"Motor Tipi"}, {"Flash Ayar"}};
#else
	const char cPopMenu[11][17] = {{"Plotter tipo"},{"IP"},{"Submascara"},{"Gateway"},{"Entrega Papel"},{"Rebobina Papel"},{"Define Cabecas"},{"Define Entrega"},{"Atraso Sensor"},{"Tipo de motor"}, {"Versao da Placa"}};
#endif
	if (cFirstMenuNo < cInverseMenuNo)
	{
		strcpy(gl_cLcdDispBuf[0],cPopMenu[cInverseMenuNo-1]);
		strcpy(gl_cLcdDispBuf[1],cPopMenu[cInverseMenuNo]);
		lcd_refresh();
		LineReverse(0x02);
	}
	else
	{
		strcpy(gl_cLcdDispBuf[0],cPopMenu[cInverseMenuNo]);
		strcpy(gl_cLcdDispBuf[1],cPopMenu[cInverseMenuNo+1]);
		lcd_refresh();
		LineReverse(0x01);
	}
}

//一级菜单显示
//20100413 新增收纸启停功能
void disp_menu(unsigned char cFirstMenuNo,unsigned char cInverseMenuNo)
{
#if( LANGUAGE == 0 )
	const char cMenu[7][17] = {{"绘制测试图形"},{"删除当前文件"},{"感应器自检"},{"键盘自检"},{"收纸启停"}, {"打印头清洗次数"},{"版本信息"}};//,{" Y轴打印误差"},{"走纸误差"},{"喷头错位误差"},{"喷头重叠误差"}};
#elif(LANGUAGE==1)
	const char cMenu[7][17] = {{"plot test graph"},{"delete file"},{"reactor check"},{"keyboard check"},{"RollPaper start"},{"Print HeadClean"},{"version Info"}};//,{"Y print error"},{"X print error"},{"Y Sprink error"},{"X sprink error"}};
#elif(LANGUAGE==2)
	const char cMenu[7][17] = {{"Imprimir Prueba"},{"Borrar archivo"},{"Verifica sensor"},{"Verifica teclas"},{"Inicia enrollar"},{"Limpia cabezal"},{"Info de version"}};
#elif(LANGUAGE==3)
	const char cMenu[7][17] = {{"Test Cizimi"},{"Dosya Sil"},{"Reaktor Kontrol"},{"Klavye Kontrol"},{"On-Bar-Basla"},{"Kartus Temizle"},{"Version Bilgi"}};
#else
	const char cMenu[7][17] = {{"Plotagem teste"},{"Remover arquivo"},{"Verifica Sensor"},{"VerificaTeclado"},{"Rebobina Papel"},{"Limpar Cabeca"},{"Versao Info"}};
#endif
	if (cFirstMenuNo < cInverseMenuNo)
	{
		strcpy(gl_cLcdDispBuf[0],cMenu[cInverseMenuNo-1]);
		strcpy(gl_cLcdDispBuf[1],cMenu[cInverseMenuNo]);
		lcd_refresh();
		LineReverse(0x02);
	}
	else
	{
		strcpy(gl_cLcdDispBuf[0],cMenu[cInverseMenuNo]);
		strcpy(gl_cLcdDispBuf[1],cMenu[cInverseMenuNo+1]);
		lcd_refresh();
		LineReverse(0x01);
	}
}

//功能：按键处理
void KBProc(void)
{
	unsigned char cReadKey;
	unsigned char cKeyPE;
	int iPulseNum,iOriginalPos;
	
	while (1)
	{
		if (((gl_cReacterStatus & B_NOPAPER) != 0) && (gl_cPrintStatus != 0x10))
		{//没纸
			gl_cDelayFlag |= 0x40;
			gl_iWaitCount = 0;
			gl_iWaitCount1 = 0;
			gl_iDelayCount = 0;
			while ((gl_cPrintStatus != 0) && (gl_cPrintStatus != 0x20))//等待打印结束
				ReceiveData();				//接收数据
			WaitPrintEnd();
			while ((gl_cReacterStatus & B_SENDPAPER) != 0)//等待送纸结束
				ReceiveData();				//接收数据
			gl_cSelfCheckFlag = 1;
			LineReverse(0);
#if( LANGUAGE == 0)
			strcpy(gl_cLcdDispBuf[0],"  纸张已用完");
			strcpy(gl_cLcdDispBuf[1],"  请上纸!");
#elif(LANGUAGE==1)
			strcpy(gl_cLcdDispBuf[0],"   NO PAPER!");
			strcpy(gl_cLcdDispBuf[1]," ");
#elif(LANGUAGE==2)
			strcpy(gl_cLcdDispBuf[0],"SIN PAPEL...!");
			strcpy(gl_cLcdDispBuf[1]," ");
#elif(LANGUAGE==3)
			strcpy(gl_cLcdDispBuf[0],"Kagit Yok!");
			strcpy(gl_cLcdDispBuf[1]," ");
#else
			strcpy(gl_cLcdDispBuf[0],"Sem Papel!");
			strcpy(gl_cLcdDispBuf[1]," ");
#endif
			lcd_refresh();
			WaitKeyExit(Key_Pause);
			gl_cReacterStatus &= (~B_NOPAPER);
#if( LANGUAGE == 0 )
			strcpy(gl_cLcdDispBuf[0],"  欢迎使用");
			strcpy(gl_cLcdDispBuf[1],"富怡喷墨绘图仪");
#elif(LANGUAGE==1)
	        strcpy(gl_cLcdDispBuf[0],"Welcome to use");
			strcpy(gl_cLcdDispBuf[1],"PloterRichpeace");
#elif(LANGUAGE==2)
	        strcpy(gl_cLcdDispBuf[0],"Bienvenidos al");
	        strcpy(gl_cLcdDispBuf[1],"Ploter Richpeace");
#elif(LANGUAGE==3)
			strcpy(gl_cLcdDispBuf[0],"Hosgeldiniz");
			strcpy(gl_cLcdDispBuf[1],"Plotter RYOU");
#else// 葡萄牙
			strcpy(gl_cLcdDispBuf[0],"Bem vindo ao");
			strcpy(gl_cLcdDispBuf[1],"PlotterPlotag");
#endif
			lcd_refresh();
			InitPaper();
			gl_cSelfCheckFlag = 0;
			gl_cDelayFlag &= 0xb0;
		}
		else
			break;
	}
	if ((gl_cFileReceive & 0x10) == 0x10)
	{
		//Printf("Del File Start 0: gl_cPrintStatus: %x\n",gl_cPrintStatus);
		gl_cDelayFlag |= 0x40;
		gl_iWaitCount = 0;
		gl_iWaitCount1 = 0;
		gl_iDelayCount = 0;
		while ((gl_cPrintStatus != 0) && (gl_cPrintStatus != 0x10))
		{
			ReceiveData();				//接收数据
			//Printf("gl_cPrintStatus: %x\n" , gl_cPrintStatus);
		}
		//Printf("Del File Start 1\n");
		WaitPrintEnd();
		//Printf("Del File Start 2\n");
		ClrFIFO;
		gl_iEthnetRecBufPH = 0;
		gl_iEthnetRecBufPE = 0;
		gl_cFileReceive = 0;
		gl_cCommunicationMode = 0xff;
		gl_cInvertBufStatus = 0;
		gl_cCurPrintBuf = 0;
		gl_cRemainderBit = 16;
		gl_cCountFlag = 0;
		if ((gl_iPulseNumY < 0) && (gl_cPrintStatus != 0x10))
		{//喷头回原点
			DriveSprinkler(0,gl_iPulseNumY);//驱动Y轴
			gl_iAxisYPos += gl_iPulseNumY;
			gl_iPulseNumY = -gl_iPulseNumY;
			WaitPrintEnd();
		}
		gl_cDelayFlag &= 0xb0;
		//Printf("Del File End\n");
	}
	while (gl_cKeyPH != gl_cKeyPE)
	{
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		cKeyPE = gl_cKeyPE + 1;
		if (cKeyPE == KeyBufLength)
			cKeyPE = 0;
		if ((cReadKey != Key_Pause) && (cReadKey != '9'))
		{
			gl_cKeyPE = cKeyPE;
			continue;
		}
		else
		{
			if (gl_cKeyPH != cKeyPE)
			{
				gl_cKeyPE = cKeyPE;
				cKeyPE = gl_cKeyBuf[gl_cKeyPE];
				if (cKeyPE == Key_Pause)
					continue;
			}
			else
				continue;
		}
		gl_cDelayFlag |= 0x40;
		gl_iWaitCount = 0;
		gl_iWaitCount1 = 0;
		gl_iDelayCount = 0;
		while ((gl_cPrintStatus != 0) && (gl_cPrintStatus != 0x10) && (gl_cPrintStatus != 0x20))
			ReceiveData();				//接收数据
		WaitPrintEnd();
		ClrFIFO;
		while ((gl_cReacterStatus & B_SENDPAPER) != 0)
			ReceiveData();				//接收数据
		gl_cSelfCheckFlag = 1;
		gl_cKeyPE = gl_cKeyPH;
		if (cReadKey == '9')
		{
			if (gl_cPrintStatus != 0x10)
			{
				if (gl_iAxisYPos != 0)
				{
					DriveSprinkler(0,-gl_iAxisYPos);
					WaitPrintEnd();//等待打印结束
				}
#if( LANGUAGE == 0 )
				strcpy(gl_cLcdDispBuf[0],"更换墨盒中...");
				strcpy(gl_cLcdDispBuf[1],"7:退出");
#elif(LANGUAGE==1)
				strcpy(gl_cLcdDispBuf[0],"Change Ink Box!");
				strcpy(gl_cLcdDispBuf[1],"7:Exit");
#elif(LANGUAGE==2)
				strcpy(gl_cLcdDispBuf[0],"Cambie la tinta");
				strcpy(gl_cLcdDispBuf[1],"7:Salir");
#else			
				strcpy(gl_cLcdDispBuf[0],"Kartus Degistir");
				strcpy(gl_cLcdDispBuf[1],"7: Sair");
#endif
				lcd_refresh();
				WaitKeyExit(Key_Pause);
				if (gl_iAxisYPos != 0)
				{
					DriveSprinkler(0,gl_iAxisYPos);
					WaitPrintEnd();//等待打印结束
				}
			}
			else
			{
#if( LANGUAGE == 0 )
				strcpy(gl_cLcdDispBuf[0],"驱动报警...");
				strcpy(gl_cLcdDispBuf[1],"7:退出");
#elif(LANGUAGE==1)
				strcpy(gl_cLcdDispBuf[0],"Drive Error!");
				strcpy(gl_cLcdDispBuf[1],"7:EXIT");
#elif(LANGUAGE==2)
				strcpy(gl_cLcdDispBuf[0],"Error de Driver");
				strcpy(gl_cLcdDispBuf[1]," ");
#elif(LANGUAGE==3)
				strcpy(gl_cLcdDispBuf[0],"Surucu Hatasi!");
				strcpy(gl_cLcdDispBuf[1]," ");
#else
				strcpy(gl_cLcdDispBuf[0],"Drive Error!");
				strcpy(gl_cLcdDispBuf[1]," ");
#endif
				lcd_refresh();
				WaitKeyExit(Key_Pause);
			}
		}
		else
		{
			iOriginalPos = gl_iOriginalPos;
			if (gl_iPulseNumY > 0)
			{
				if (gl_cPrintStatus != 0x10)
				{
					//将喷头走到真正原点位置
					iPulseNum = (OriginalSideDot + 160) * DotPulseNum;
					if (iPulseNum != 0)
					{
						DriveSprinkler(0,iPulseNum);
						gl_iAxisYPos += iPulseNum;
						WaitPrintEnd();//等待打印结束
					}
				}
				SetOriginalPos();		//进入原点设置
				if (gl_cPrintStatus != 0x10)
				{
					if ((gl_cFileReceive & 0x08) == 0)
					{
						//将喷头回到打印起始点
						if (iPulseNum != 0)
						{
							iPulseNum = -iPulseNum;
							DriveSprinkler(0,iPulseNum);
							gl_iAxisYPos += iPulseNum;
							WaitPrintEnd();//等待打印结束
						}
					}
					else
					{
						ClearPrintHead(1);
						gl_cFileReceive &= 0xf7;
						WriteFilePrint(0xaa);
						ClrFIFO;
					}
				}
			}
			else
			{
				SetOriginalPos();		//进入原点设置
				if (((gl_cFileReceive & 0x08) != 0) && (gl_cPrintStatus != 0x10))
				{//文件刚开始打印，清洗喷头
					ClearPrintHead(0);
					gl_cFileReceive &= 0xf7;
					WriteFilePrint(0xaa);
					ClrFIFO;
				}
			}
			if ((gl_iOriginalPos != iOriginalPos) && (gl_cPrintStatus != 0x10))
			{
				WriteFilePrint(0xaa);
				ClrFIFO;
			}
		}
		gl_cSelfCheckFlag = 0;
		LineReverse(0);
#if( LANGUAGE == 0 )
		strcpy(gl_cLcdDispBuf[0],"  欢迎使用");
		strcpy(gl_cLcdDispBuf[1],"富怡喷墨绘图仪");
#elif(LANGUAGE==1)
	    strcpy(gl_cLcdDispBuf[0],"Welcome to use");
		strcpy(gl_cLcdDispBuf[1],"PloterRichpeace");
#elif(LANGUAGE==2)
	    strcpy(gl_cLcdDispBuf[0],"Bienvenidos al");
	    //strcpy(gl_cLcdDispBuf[1],"Ploter Stamp&go");
	    strcpy(gl_cLcdDispBuf[1],"Ploter Richpeace");
#elif(LANGUAGE==3)
		strcpy(gl_cLcdDispBuf[0],"Hosgeldiniz");
		strcpy(gl_cLcdDispBuf[1],"Plotter RYOU");
#else// 葡萄牙
		strcpy(gl_cLcdDispBuf[0],"Bem vindo ao");
		strcpy(gl_cLcdDispBuf[1],"PlotterPlotag");

#endif
		lcd_refresh();
		gl_cDelayFlag &= 0xb0;
	}
}
//一般菜单处理
void MenuProc(void)		
{
	unsigned char cReadKey;
	unsigned char cEnd;
	unsigned char cInverseMenuNo1;
	unsigned char cFirstMenuNo1;	
		
	cInverseMenuNo1 = 0;
	cFirstMenuNo1 = 0;
	disp_menu(cFirstMenuNo1,cInverseMenuNo1);
	cEnd = 1;
	while (cEnd)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cReadKey = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		switch (cReadKey)
		{
			case Key_Pause:
			WaitKeyBound(Key_Pause);
			WaitPrintEnd();
			cEnd = 0;
			break;

			case Key_Ok:
			LineReverse(0);
			switch (cInverseMenuNo1)
			{
				case 0:		//测试图形绘制
				if (gl_cPrintStatus == 0x10)
				{
#if( LANGUAGE == 0 )
					strcpy(gl_cLcdDispBuf[0],"驱动报警");
					strcpy(gl_cLcdDispBuf[1],"7:退出");
#elif(LANGUAGE==1)
					strcpy(gl_cLcdDispBuf[0],"Drive Error!");
					strcpy(gl_cLcdDispBuf[1],"7:EXIT");
#elif(LANGUAGE==2)
					strcpy(gl_cLcdDispBuf[0],"Error de Driver");
					strcpy(gl_cLcdDispBuf[1]," ");
#elif(LANGUAGE==3)
					strcpy(gl_cLcdDispBuf[0],"Surucu Hatasi!");
					strcpy(gl_cLcdDispBuf[1]," ");
#else
					strcpy(gl_cLcdDispBuf[0],"Drive Error!");
					strcpy(gl_cLcdDispBuf[1]," ");

#endif
					lcd_refresh();
					WaitKeyExit(Key_Pause);
				}
				else if (gl_cFileReceive == 0)
					DrawTestGraph();
				break;

				case 1:
				DeleteCurFile();
				break;

				case 2:		//感应器自检
				ReactorCheck();
				break;

				case 3:		//键盘自检
				KeyboardCheck();
				break;

				case 4:		//收纸启停20100413
				TestRecSendMotor(1);
				break;

				case 5:
				SetPrintHeadCleanCount(); //打印头清洗次数
				break;

				case 6:		//版本号显示
				VersionDisplay();
				break;

				default:
				break;
			}
			disp_menu(cFirstMenuNo1,cInverseMenuNo1);
			break;

			case Key_Down:
			if (cInverseMenuNo1 < 6)//收纸启停20100413
			{   
				if (cFirstMenuNo1 != cInverseMenuNo1)
					cFirstMenuNo1++;
				cInverseMenuNo1++;
				disp_menu(cFirstMenuNo1,cInverseMenuNo1);
			}
			break;

			case Key_Up:
			if (cInverseMenuNo1 > 0)
			{   
				if (cFirstMenuNo1 == cInverseMenuNo1)
					cFirstMenuNo1--;
				cInverseMenuNo1--;
				disp_menu(cFirstMenuNo1,cInverseMenuNo1);
			}
			break;

			case Key_Menu:
			if (VerifySecrity() == 1)
			{
				DealPopMenu();
				disp_menu(cFirstMenuNo1,cInverseMenuNo1);
			}
			break;

			default:
			break;
		}
	}
}
const unsigned char Secrity[6] = {'3','5','2','4','8','6'};
//密码验证 0验证错误,1正确
int VerifySecrity(void)
{
	unsigned int i;
	unsigned char ch;
	
	for (i=0;i<6;)
	{
		WaitKeyExit(0xff);		//等待有键按下
		ch = gl_cKeyBuf[gl_cKeyPE];
		if (ch == Secrity[i])
		{
			if ((gl_cKeyPE + 1) == KeyBufLength)
				gl_cKeyPE = 0;
			else
				gl_cKeyPE++;
			i++;
		}
		else
			return 0;
	}
	return 1;
}

// 喷头选择20101214
void SelectPritHeadCount(void)
{
	unsigned char ch, cEnd;
#if( LANGUAGE == 0)
	strcpy(gl_cLcdDispBuf[0],"2:双喷  4:四喷");
	if (SprinklerNum == 2)
		strcpy(gl_cLcdDispBuf[1],"双喷");
	else
		strcpy(gl_cLcdDispBuf[1],"四喷");
#elif(LANGUAGE==1)
	strcpy(gl_cLcdDispBuf[0],"2:Two  4:Four");
	if (SprinklerNum == 2)
		strcpy(gl_cLcdDispBuf[1],"Two");
	else
		strcpy(gl_cLcdDispBuf[1],"Four");
#elif(LANGUAGE==2)
	strcpy(gl_cLcdDispBuf[0],"2:Dos 4:Cuatro");
	if (SprinklerNum == 2)
		strcpy(gl_cLcdDispBuf[1],"Dos");
	else
		strcpy(gl_cLcdDispBuf[1],"Cuatro");
#elif(LANGUAGE==3)
	strcpy(gl_cLcdDispBuf[0],"2:iki 4:dort");
	if (SprinklerNum == 2)
		strcpy(gl_cLcdDispBuf[1],"iki");
	else
		strcpy(gl_cLcdDispBuf[1],"dort");	
#else
	strcpy(gl_cLcdDispBuf[0],"2:Dois 4:Quatro");
	if (SprinklerNum == 2)
		strcpy(gl_cLcdDispBuf[1],"Dois");
	else
		strcpy(gl_cLcdDispBuf[1],"Quatro");		
#endif
	lcd_refresh();
	
	cEnd = 1;
	while (cEnd)
	{
		WaitKeyExit(0xff);		//等待有键按下
		ch = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		switch (ch)
		{
			case Key_Up: 
			SprinklerNum = 2; // 双喷
			WriteParameter();
			cEnd = 0;
			break;
				
			case Key_Left:
			SprinklerNum = 4;// 四喷
			WriteParameter();
			cEnd = 0;
			break;
				
			case Key_Pause:
			case Key_Ok:
			cEnd = 0;
			break;
		}
	}
}
extern int _atoi_(char * str);
extern char *_itoa_(int num, char * str, int radix);

//走纸长度设置
void SetSendPaperLength(void)
{
	short int i, iTemp;
	char tempBuff[5], ch, cEnd;
	
	iTemp = (short int )(gl_sSendPaperLength / X_PPMM)+1;
	_itoa_(iTemp, tempBuff, 10);
	strcpy(gl_cLcdDispBuf[0],"1:300  2:660");
	strcpy(gl_cLcdDispBuf[1],tempBuff);
	lcd_refresh();
	
	cEnd = 1;
	while (cEnd)
	{
		WaitKeyExit(0xff);		//等待有键按下
		ch = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		switch (ch)
		{
			case Key_Menu: 
			gl_sSendPaperLength = 20299;
			cEnd = 0;
			WriteParameter();
			break;
				
			case Key_Up:
			gl_sSendPaperLength = 44659;
			cEnd = 0;
			WriteParameter();
			break;

			case Key_3:
#if(LANGUAGE==0)
			lcd_display(0, "自定义:"); // 100 ~700mm
#elif(LANGUAGE==1)
			lcd_display(0, "user-defined");
#elif(LANGUAGE==2)
			lcd_display(0, "Usuariodefinido");
#elif(LANGUAGE==3)
			lcd_display(0, "Kullanici Girisi");
#else
			lcd_display(0, "Personalizado");
#endif
			for (i=0;i<2;i++)
			{
				WaitKeyExit(0xff);		//等待有键按下
				tempBuff[i] = gl_cKeyBuf[gl_cKeyPE];
				lcd_display(1, tempBuff);
				if ((gl_cKeyPE+1) == KeyBufLength)
					gl_cKeyPE = 0;
				else
					gl_cKeyPE++;
			}
			tempBuff[i] = '\0';
			iTemp = _atoi_(tempBuff);
			iTemp = (iTemp % 10 == 9) ? (iTemp+1) : iTemp;
			iTemp = iTemp > 70 ? 70 : iTemp;
			//Printf("Paper length : %d\n", iTemp);
			gl_sSendPaperLength = (short int)(iTemp * 10 * X_PPMM);
			WriteParameter();
			cEnd = 0;
			break;
				
			case Key_Pause:
			case Key_Ok:
			cEnd = 0;
			break;
		}
	}
}

// 设置送纸延迟
void SetSendPaperDelay(void)
{
	short int  iTemp;
	char tempBuff[5];
	
	iTemp = gl_sSendPaperDelay;
	_itoa_(iTemp, tempBuff, 10);
#if( LANGUAGE == 0)
	strcpy(gl_cLcdDispBuf[0],"送纸感应器延时");  // 1~9
	strcpy(gl_cLcdDispBuf[1],tempBuff);
#elif(LANGUAGE==1)
	strcpy(gl_cLcdDispBuf[0],"Inductor delay");  // 1~9
	strcpy(gl_cLcdDispBuf[1],tempBuff);
#elif(LANGUAGE==2)
	strcpy(gl_cLcdDispBuf[0],"Tiempo inductor");  // 1~9
	strcpy(gl_cLcdDispBuf[1],tempBuff);
#elif(LANGUAGE==3)
	strcpy(gl_cLcdDispBuf[0],"Induktor Ayar");  // 1~9
	strcpy(gl_cLcdDispBuf[1],tempBuff);
#else
	strcpy(gl_cLcdDispBuf[0],"Atraso Sensor");  // 1~9
	strcpy(gl_cLcdDispBuf[1],tempBuff);
#endif
	lcd_refresh();

	WaitKeyExit(0xff);		//等待有键按下
	iTemp= gl_cKeyBuf[gl_cKeyPE]-'0';
	if ((gl_cKeyPE+1) == KeyBufLength)
		gl_cKeyPE = 0;
	else
		gl_cKeyPE++;
	//Printf("Paper Delay: %d\n", iTemp);
	iTemp = (iTemp>9)?9:iTemp==0?1:iTemp;
	
	gl_sSendPaperDelay = iTemp;
	WriteParameter();
}

// 20110221 设置打印头清洗次数
void SetPrintHeadCleanCount(void)
{
	short int  iTemp=1;
	char tempBuff[5], dispBuff[3], cEnd, ch;
	
	iTemp = gl_sPrintHeadCleanCount;
	_itoa_(iTemp, tempBuff, 10);
#if( LANGUAGE == 0)
	strcpy(gl_cLcdDispBuf[0],"打印头清洗次数"); // 1~5
	strcpy(gl_cLcdDispBuf[1],tempBuff);
#elif(LANGUAGE==1)
	strcpy(gl_cLcdDispBuf[0],"clean count"); // 1~5
	strcpy(gl_cLcdDispBuf[1],tempBuff);
#elif(LANGUAGE==2)
	strcpy(gl_cLcdDispBuf[0],"Limpia cabezal"); // 1~5
	strcpy(gl_cLcdDispBuf[1],tempBuff);
#elif(LANGUAGE==3)	
	strcpy(gl_cLcdDispBuf[0],"Limpia cabezal"); // 1~5
	strcpy(gl_cLcdDispBuf[1],tempBuff);
#else
	strcpy(gl_cLcdDispBuf[0],"Quantidade"); // 1~5
	strcpy(gl_cLcdDispBuf[1],tempBuff);
#endif
	lcd_refresh();

	cEnd = 1;
	while (cEnd)
	{
		WaitKeyExit(0xff);		//等待有键按下
		ch = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		switch (ch)
		{
			case Key_Up:
			iTemp++;
			if(iTemp > 5)
				iTemp = 1;
			_itoa_(iTemp, dispBuff, 10);
			lcd_display(1, dispBuff);
			break;
				
			case Key_Down:
			iTemp--;
			if(iTemp < 1)
				iTemp = 5;
			_itoa_(iTemp, dispBuff, 10);
			lcd_display(1, dispBuff);
			break;
				
			case Key_Ok:
			gl_sPrintHeadCleanCount = iTemp;
			WriteParameter();
			cEnd = 0;
			break;
				
			case Key_Pause:
			cEnd = 0;
			break;
				
		}
	}
}

// 设置电机类型 默认是无刷电机 20111101
void SetDeviceType()
{
	char cTemp, cEnd , ch;
#if( LANGUAGE == 0)	
	if(gl_DeviceType == 0)
		strcpy(gl_cLcdDispBuf[1],"有刷电机");//
	else
		strcpy(gl_cLcdDispBuf[1],"无刷电机");
	strcpy(gl_cLcdDispBuf[0],"电机类型设置");
#elif(LANGUAGE==1)
	if(gl_DeviceType == 0)
		strcpy(gl_cLcdDispBuf[1],"have brush");//
	else
		strcpy(gl_cLcdDispBuf[1],"none brush");
	strcpy(gl_cLcdDispBuf[0],"Motor type set");
#elif(LANGUAGE==2)
	if(gl_DeviceType == 0)
		strcpy(gl_cLcdDispBuf[1],"Con carbones");//
	else
		strcpy(gl_cLcdDispBuf[1],"Brushless");
	strcpy(gl_cLcdDispBuf[0],"Tipo de motor");
#elif(LANGUAGE==3)
	if(gl_DeviceType == 0)
		strcpy(gl_cLcdDispBuf[1],"Kuyrukla Acik");//
	else
		strcpy(gl_cLcdDispBuf[1],"Kuyrukla Kapa");
	strcpy(gl_cLcdDispBuf[0],"Motor Tipi");
#else
	if(gl_DeviceType == 0)
		strcpy(gl_cLcdDispBuf[1],"Com escova");//
	else
		strcpy(gl_cLcdDispBuf[1],"Sem escova");
	strcpy(gl_cLcdDispBuf[0],"Tipo de motor");
#endif
	
	lcd_refresh();
	
	cEnd = 1;
	cTemp = gl_DeviceType;
	while (cEnd)
	{
		WaitKeyExit(0xff);		//等待有键按下
		ch = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		switch (ch)
		{
			case Key_Menu: 
				cTemp = 0;
#if( LANGUAGE == 0)		
				lcd_display(1, "有刷电机");
#elif(LANGUAGE==1)
				lcd_display(1, "have brush");
#elif(LANGUAGE==2)
			    lcd_display(1, "Con carbones");
#elif(LANGUAGE==3)
				lcd_display(1, "Kuyrukla Acik");
#else
				lcd_display(1, "Com escova");

#endif
				break;
				
			case Key_Up:
				cTemp = 1;
#if( LANGUAGE == 0)	
				lcd_display(1, "无刷电机");
#elif(LANGUAGE==1)
				lcd_display(1, "none brush");
#elif(LANGUAGE==2)
			    lcd_display(1, "Brushless");
#elif(LANGUAGE==3)
				lcd_display(1, "Kuyrukla Kapa");
#else
				lcd_display(1, "Sem escova");
#endif
				break;

			case Key_Pause:
				cEnd = 0;
				break;
				
			case Key_Ok:
				
				gl_DeviceType = cTemp;
				WriteParameter();
				cEnd = 0;
				break;
		}
	}
}

// 20111101 flash芯片选择 0=3201B 1=3201
void FlashChipSelect()
{
	char cTemp, cEnd, ch;

	if(gl_FlashChipSelect == 0)
		strcpy(gl_cLcdDispBuf[1],"SST39VF3201B");
	else
		strcpy(gl_cLcdDispBuf[1],"SST39VF3201");
#if( LANGUAGE == 0)		
	strcpy(gl_cLcdDispBuf[0],"Ｆｌａｓｈ设置");
#elif(LANGUAGE==1)
	strcpy(gl_cLcdDispBuf[0],"Flash chip set");
#elif(LANGUAGE==2)
    strcpy(gl_cLcdDispBuf[0],"Chip flash");
#elif(LANGUAGE==3)
	strcpy(gl_cLcdDispBuf[0],"Flash Ayar");
#else
	strcpy(gl_cLcdDispBuf[0],"Versao da Placa");
#endif
	lcd_refresh();
	
	cEnd = 1;
	cTemp = gl_FlashChipSelect;
	while (cEnd)
	{
		WaitKeyExit(0xff);		//等待有键按下
		ch = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		switch (ch)
		{
			case Key_Menu: 
			cTemp = 0;
			lcd_display(1, "SST39VF3201B");
			break;

			case Key_Up:
			cTemp = 1;
			lcd_display(1, "SST39VF3201");
			break;

			case Key_Pause:
			cEnd = 0;
			break;
				
			case Key_Ok:
			gl_FlashChipSelect = cTemp;
			WriteParameter();
			cEnd = 0;
			break;
		}
	}
}

//高级菜单处理
void DealPopMenu(void)
{
	unsigned char cTemp;
	unsigned char cChooseMenu,cDispFirstMenu;
  
	cChooseMenu = 0;
	cDispFirstMenu = 0;
	DispPopMenu(cDispFirstMenu,cChooseMenu);
	while (1)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cTemp = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		switch (cTemp)
		{
			case Key_Pause:
			WaitKeyBound(Key_Pause);
			WaitPrintEnd();	//等待打印结束
			return;

			case Key_Down:
			if (cChooseMenu < 10)// 20101214 喷头选择
			{
				if (cDispFirstMenu != cChooseMenu)
					cDispFirstMenu++;
				cChooseMenu++;
				DispPopMenu(cDispFirstMenu,cChooseMenu);
			}
			break;

			case Key_Up:
			if (cChooseMenu > 0)
			{
				if (cDispFirstMenu == cChooseMenu)
					cDispFirstMenu--;
				cChooseMenu--;
				DispPopMenu(cDispFirstMenu,cChooseMenu);
			}
			break;	

			case Key_Ok:
			LineReverse(0);
			switch (cChooseMenu)
			{
				case 0:
				SelectPlotType();		//机型选择
				break;

				case 1:
				SetupIPAddr(MyIP);	//网络设置
				break;

				case 2:
				SetupIPAddr(SubnetMask);	//网络设置
				break;

				case 3:
				SetupIPAddr(GatewayIP);	//网络设置
				break;

				case 4:
				TestRecSendMotor(0);			//送纸电机测试
				break;

				case 5:
				TestRecSendMotor(1);			//收纸电机测试
				break;

				case 6:
				SelectPritHeadCount();	// 20101214 设置打印头数
				break;

				case 7:
				SetSendPaperLength();	// 20110221设置走纸长度
				break;

				case 8:
				SetSendPaperDelay();	// 20110221 设置感应器延迟
				break;

				case 9:
				SetDeviceType();  // 20111101 电机类型设置
				break;

				case 10:
				FlashChipSelect();	// 20111101 Flash芯片选择 0= 3201B
				break;
						
				default:
				break;
			}
			DispPopMenu(cDispFirstMenu,cChooseMenu);
			break;

			default:
			break;
		}
	}
}

//原点位置设置
void SetOriginalPos(void)
{
	int iMaxPosY,iPulseNum,iTotalPulseNum,iTotalPulseNumX;
	unsigned char cTemp,cEnd;
	int iRepeatPress;
	float dStep;
  
	while (1)
	{
	  	if (((gl_cReacterStatus & B_NOPAPER) != 0) && (gl_cPrintStatus != 0x10))
		{//没纸
			LineReverse(0);
#if( LANGUAGE == 0 )
			strcpy(gl_cLcdDispBuf[0],"  纸张已用完");
			strcpy(gl_cLcdDispBuf[1],"  请上纸!");
#elif(LANGUAGE==1)
			strcpy(gl_cLcdDispBuf[0],"   NO PAPER!");
			strcpy(gl_cLcdDispBuf[1]," ");
#elif(LANGUAGE==2)
			strcpy(gl_cLcdDispBuf[0],"SIN PAPEL...!");
			strcpy(gl_cLcdDispBuf[1]," ");
#elif(LANGUAGE==3)
			strcpy(gl_cLcdDispBuf[0],"Kagit Yok!");
			strcpy(gl_cLcdDispBuf[1]," ");
#else
			strcpy(gl_cLcdDispBuf[0],"Sem Papel!");
			strcpy(gl_cLcdDispBuf[1]," ");

#endif
			lcd_refresh();
			WaitKeyExit(Key_Pause);
			gl_cReacterStatus &= (~B_NOPAPER);
			InitPaper();
		}
		else
			break;
	}

  	LineReverse(0);
#if( LANGUAGE == 0 )
	strcpy(gl_cLcdDispBuf[0],"2:退纸  8:进纸");
	strcpy(gl_cLcdDispBuf[1],"4:左移  6:右移");
#elif(LANGUAGE==1)
	strcpy(gl_cLcdDispBuf[0],"2:back 8:front");
	strcpy(gl_cLcdDispBuf[1],"4:left 6:right");
#elif(LANGUAGE==2)
	strcpy(gl_cLcdDispBuf[0],"2:Retro 8:Avanz");
	strcpy(gl_cLcdDispBuf[1],"4:Izq.  6:Der.");
#elif(LANGUAGE==3)
	strcpy(gl_cLcdDispBuf[0],"2:geri 8:ileri");
	strcpy(gl_cLcdDispBuf[1],"4:sol 6:sag");
#else
	strcpy(gl_cLcdDispBuf[0],"2:Tras 8:Frente");
	strcpy(gl_cLcdDispBuf[1],"4:Esquer6:Direi");
#endif
	lcd_refresh();

	iTotalPulseNum = 0;
	iTotalPulseNumX = 0;
	iMaxPosY = (int)(gl_sPlotType * Y_PPMM) + (OriginalSideDot + 160) * DotPulseNum;
	while (1)
	{
		iRepeatPress = 0;
		while (gl_cKeyPE == gl_cKeyPH)
			ReceiveData();				//接收数据

		cTemp = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;

		switch (cTemp)
		{
			case Key_Menu:
			WaitKeyBound(Key_Menu);
			WaitPrintEnd();
			if (((iTotalPulseNum != 0) || (iTotalPulseNumX != 0)) && (gl_cPrintStatus != 0x10))
			{
				iTotalPulseNum = -iTotalPulseNum;
				iTotalPulseNumX = -iTotalPulseNumX;
				DriveSprinkler(iTotalPulseNumX,iTotalPulseNum);
				WaitPrintEnd();
				gl_iAxisYPos += iTotalPulseNum;
			}
			MenuProc();
			return;

			case Key_Pause:			//退出子菜单，进主菜单
			WaitKeyBound(Key_Pause);
			WaitPrintEnd();
			if (((iTotalPulseNum != 0) || (iTotalPulseNumX != 0)) && (gl_cPrintStatus != 0x10))
			{
				iTotalPulseNum = -iTotalPulseNum;
				iTotalPulseNumX = -iTotalPulseNumX;
				DriveSprinkler(iTotalPulseNumX,iTotalPulseNum);
				WaitPrintEnd();
				gl_iAxisYPos += iTotalPulseNum;
			}
			return;

			case Key_Ok:
			if (gl_cPrintStatus == 0x10)
				break;
			if (gl_iAxisYPos > iMaxPosY)
				iTotalPulseNum = (int)(gl_sPlotType * Y_PPMM);
			else if (gl_iAxisYPos < (OriginalSideDot + 160) * DotPulseNum)
				iTotalPulseNum = 0;
			else
				iTotalPulseNum = gl_iAxisYPos - (OriginalSideDot + 160) * DotPulseNum;
			if (iTotalPulseNum != gl_iOriginalPos)
			{
				gl_iOriginalPos = iTotalPulseNum;
				WaitPrintEnd();
				WriteParameter();  //写参数配置文件
				CalVariableValue();
			}
			if (iTotalPulseNumX < 0)
				InitPaper();
			if (((gl_cFileReceive & 0x01) == 0x01) && (gl_iGraphWidth > gl_iMaxPictureWidth))
			{
#if( LANGUAGE == 0 )
				strcpy(gl_cLcdDispBuf[0],"打印图形超宽");
				strcpy(gl_cLcdDispBuf[1]," ");
#elif(LANGUAGE==1)
				strcpy(gl_cLcdDispBuf[0],"Y:out-of-range");
				strcpy(gl_cLcdDispBuf[1]," ");
#elif(LANGUAGE==2)
				strcpy(gl_cLcdDispBuf[1]," ");
				strcpy(gl_cLcdDispBuf[0],"Y Fuera d rango");
#elif(LANGUAGE==3)
				strcpy(gl_cLcdDispBuf[1]," ");
				strcpy(gl_cLcdDispBuf[0],"Y:Kontur Disi!");
#else
				strcpy(gl_cLcdDispBuf[1]," ");
				strcpy(gl_cLcdDispBuf[0],"Y:ForaDoLimite");

#endif
				lcd_refresh();
				while (1);
			}
			return;

			case Key_Left:
			case Key_Right:
	      	if (gl_cPrintStatus == 0x10)
	      	{
#if( LANGUAGE == 0 )
				lcd_display(0,"驱动报警");
				lcd_display(1,"7:退出");
#elif(LANGUAGE==1)
				lcd_display(0,"Drive Error!");
				lcd_display(1,"7:EXIT");

#elif(LANGUAGE==2)
				lcd_display(0,"Error de Driver");
				lcd_display(1,"7:Salir");
#elif(LANGUAGE==3)
				strcpy(gl_cLcdDispBuf[0],"Surucu Hatasi!");
				strcpy(gl_cLcdDispBuf[1]," ");
			#else
				strcpy(gl_cLcdDispBuf[0],"Drive Error!");
				strcpy(gl_cLcdDispBuf[1]," ");
#endif
				WaitKeyExit(Key_Pause);
				lcd_refresh();
				break;
			}
			cEnd = 1;
			while (cEnd)
			{
				ReceiveData();				//接收数据
				if (iRepeatPress > 50)
				{
					iRepeatPress += 2;
					if (iRepeatPress > 300)
						iRepeatPress = 300;
					dStep = (double)iRepeatPress * (double)iRepeatPress / 50.0 * 0.2 * Y_PPMM;
				}
				else
				{
					iRepeatPress++;
					dStep = (double)iRepeatPress * (double)iRepeatPress / 50.0 * 0.1;
					if (dStep < 0.3)
						dStep = 0.3;
					dStep = dStep * Y_PPMM;
				}
				if (cTemp == Key_Left)
					iPulseNum = (int)dStep;
				else
					iPulseNum = -(int)dStep;
				if (gl_iAxisYPos >= iMaxPosY)
				{
					if (iPulseNum > 0)
						iPulseNum = 0;
					else if ((gl_iAxisYPos + iPulseNum) < (OriginalSideDot + 160) * DotPulseNum)
						iPulseNum = -(gl_iAxisYPos - (OriginalSideDot + 160) * DotPulseNum);
				}
				else if ((gl_iAxisYPos + iPulseNum) >= iMaxPosY)
					iPulseNum = iMaxPosY - gl_iAxisYPos;
				else if (gl_iAxisYPos <= (OriginalSideDot + 160) * DotPulseNum)
				{
					if (iPulseNum < 0)
						iPulseNum = 0;
					else if ((gl_iAxisYPos + iPulseNum) >= iMaxPosY)
						iPulseNum = iMaxPosY - gl_iAxisYPos;
				}
				else if ((gl_iAxisYPos + iPulseNum) < (OriginalSideDot + 160) * DotPulseNum)
					iPulseNum = -(gl_iAxisYPos - (OriginalSideDot + 160) * DotPulseNum);
				gl_iAxisYPos += iPulseNum;
				iTotalPulseNum += iPulseNum;
				if (iPulseNum != 0)
				{
	    			WaitPrintEnd();//等待打印结束
					DriveSprinkler(0,iPulseNum);
				}
				while (gl_cKeyPE != gl_cKeyPH)
				{
					if (gl_cKeyBuf[gl_cKeyPE] != cTemp)
					{
						cEnd = 0;
						break;
					}
					else if ((gl_cKeyPE +1) == KeyBufLength)
						gl_cKeyPE = 0;
					else
						gl_cKeyPE++;
					}
					if (gl_cPrintStatus == 0x10)
	      			cEnd = 0;
				}
				break;

				case Key_Up:
				case Key_Down:
				if (gl_cPrintStatus == 0x10)
				{
#if(LANGUAGE==0)
					lcd_display(0,"驱动报警");
					lcd_display(1,"7:退出");
#elif(LANGUAGE==1)
					lcd_display(0,"Drive Error!");
					lcd_display(1,"7:EXIT");
#elif(LANGUAGE==2)
				lcd_display(0,"Error de Driver");
				lcd_display(1,"7:Salir");
#elif(LANGUAGE==3)
					strcpy(gl_cLcdDispBuf[0],"Surucu Hatasi!");
					strcpy(gl_cLcdDispBuf[1]," ");
#else
					strcpy(gl_cLcdDispBuf[0],"Drive Error!");
					strcpy(gl_cLcdDispBuf[1]," ");

#endif
					WaitKeyExit(Key_Pause);
					lcd_refresh();
					break;
				}

				cEnd = 1;
				WaitPrintEnd();//等待打印结束
				while (cEnd)
				{
					ReceiveData();				//接收数据
					if (iRepeatPress > 50)
					{
						iRepeatPress += 2;
						if (iRepeatPress > 90)
							iRepeatPress = 90;
						dStep = (double)iRepeatPress * (double)iRepeatPress / 50.0 * 0.2 * X_PPMM;
					}
					else
					{
						iRepeatPress++;
						dStep = (double)iRepeatPress * (double)iRepeatPress / 50.0 * 0.1;
						if (dStep < 0.5)
							dStep = 0.5;
						dStep = dStep * X_PPMM;
					}
					if (cTemp == Key_Up)
						iPulseNum = (int)dStep;
					else
						iPulseNum = -(int)dStep;
					if (iPulseNum != 0)
					{
						if (cTemp == Key_Up)
						{
							if (GetIOBit(I_ROLLPEND) == StatusOk)
							{
								cEnd = 0;
								break;
							}
						}
					else
					{
						if (GetIOBit(I_GIVEPEND) == StatusOk)
						{
							cEnd = 0;
							break;
						}
					}
					iTotalPulseNumX += iPulseNum;
					DriveSprinkler(iPulseNum,0);
					WaitPrintEnd();//等待打印结束
				}

				while (gl_cKeyPE != gl_cKeyPH)
				{
					if (gl_cKeyBuf[gl_cKeyPE] != cTemp)
					{
						cEnd = 0;
						break;
					}
					else if ((gl_cKeyPE +1) == KeyBufLength)
						gl_cKeyPE = 0;
					else
						gl_cKeyPE++;
				}

				if (gl_cPrintStatus == 0x10)
					cEnd = 0;
			}
			break;

			default:
			iRepeatPress = 0;
			cTemp = 0xff;
			break;
		}
	}
}

//机型选择
void SelectPlotType(void)
{
	unsigned int i;
	unsigned char ch;
	LineReverse(0x02);
	switch(gl_sPlotType)
	{
		case 1350:i=1;break;
		case 1630:i=2;break;
		case 1830:i=3;break;
		case 2030:i=4;break;
		case 2230:i=5;break;
		case 2430:i=6;break;
		case 2630:i=7;break;
		case 2830:i=8;break;
		case 3230:i=9;break;//20150812
		default:i=0;break;
	}


#if( LANGUAGE == 0 )
	switch(i)
	{
		case 1: strcpy(gl_cLcdDispBuf[0],"当前:RP_MJ135"); break;
		case 2: strcpy(gl_cLcdDispBuf[0],"当前:RP_MJ160"); break;
		case 3: strcpy(gl_cLcdDispBuf[0],"当前:RP_MJ180"); break;
		case 4: strcpy(gl_cLcdDispBuf[0],"当前:RP_MJ200"); break;
		case 5: strcpy(gl_cLcdDispBuf[0],"当前:RP_MJ220"); break;
		case 6: strcpy(gl_cLcdDispBuf[0],"当前:RP_MJ240"); break;
		case 7: strcpy(gl_cLcdDispBuf[0],"当前:RP_MJ260"); break;// 20100813 新增2.83米机器
		case 8: strcpy(gl_cLcdDispBuf[0],"当前:RP_MJ280"); break;// 20100926 新增3.03米机器
		case 9: strcpy(gl_cLcdDispBuf[0],"当前:RP_MJ320");break;//20150812 新增3.23米机型
		default: strcpy(gl_cLcdDispBuf[0],"当前:RP_MJ300"); break;
	}
#elif(LANGUAGE==1)
	switch(i)
	{
		case 1: strcpy(gl_cLcdDispBuf[0],"Type:SP135"); break;
		case 2: strcpy(gl_cLcdDispBuf[0],"Type:SP160"); break;
		case 3: strcpy(gl_cLcdDispBuf[0],"Type:SP180"); break;
		case 4: strcpy(gl_cLcdDispBuf[0],"Type:SP200"); break;
		case 5: strcpy(gl_cLcdDispBuf[0],"Type:SP220"); break;
		case 6: strcpy(gl_cLcdDispBuf[0],"Type:SP240"); break;
		case 7: strcpy(gl_cLcdDispBuf[0],"Type:SP260"); break;
		case 8: strcpy(gl_cLcdDispBuf[0],"Type:SP280"); break;
		case 9: strcpy(gl_cLcdDispBuf[0],"Type:SP320");break;//20150812 新增3.23米机型
		default: strcpy(gl_cLcdDispBuf[0],"Type:SP300 "); break;
	}
#elif(LANGUAGE==2)
	switch(i)
	{
		case 1: strcpy(gl_cLcdDispBuf[0],"Tipo:SP135 "); break;
		case 2: strcpy(gl_cLcdDispBuf[0],"Tipo:SP160"); break;
		case 3: strcpy(gl_cLcdDispBuf[0],"Tipo:SP180"); break;
		case 4: strcpy(gl_cLcdDispBuf[0],"Tipo:SP200 "); break;
		case 5: strcpy(gl_cLcdDispBuf[0],"Tipo:SP220 "); break;
		case 6: strcpy(gl_cLcdDispBuf[0],"Tipo:SP240 "); break;
		case 7: strcpy(gl_cLcdDispBuf[0],"Tipo:SP260"); break;
		case 8: strcpy(gl_cLcdDispBuf[0],"Tipo:SP280"); break;
		case 9: strcpy(gl_cLcdDispBuf[0],"Tipo:SP320");break;//20150812 新增3.23米机型
		default: strcpy(gl_cLcdDispBuf[0],"Tipo:SP300 "); break;
	}
#elif(LANGUAGE==3)
	switch(i)
	{
		case 1: strcpy(gl_cLcdDispBuf[0],"Tip:SP135 "); break;
		case 2: strcpy(gl_cLcdDispBuf[0],"Tip:SP160"); break;
		case 3: strcpy(gl_cLcdDispBuf[0],"Tip:SP180"); break;
		case 4: strcpy(gl_cLcdDispBuf[0],"Tip:SP200 "); break;
		case 5: strcpy(gl_cLcdDispBuf[0],"Tip:SP220 "); break;
		case 6: strcpy(gl_cLcdDispBuf[0],"Tip:SP240 "); break;
		case 7: strcpy(gl_cLcdDispBuf[0],"Tip:SP260"); break;
		case 8: strcpy(gl_cLcdDispBuf[0],"Tip:SP280"); break;
		case 9: strcpy(gl_cLcdDispBuf[0],"Tip:SP320");break;//20150812 新增3.23米机型
		default: strcpy(gl_cLcdDispBuf[0],"Tip:SP300 "); break;
	}
#else
	switch(i)
	{
		case 1: strcpy(gl_cLcdDispBuf[0],"Tipo:SP135 "); break;
		case 2: strcpy(gl_cLcdDispBuf[0],"Tipo:SP160"); break;
		case 3: strcpy(gl_cLcdDispBuf[0],"Tipo:SP180"); break;
		case 4: strcpy(gl_cLcdDispBuf[0],"Tipo:SP200 "); break;
		case 5: strcpy(gl_cLcdDispBuf[0],"Tipo:SP220 "); break;
		case 6: strcpy(gl_cLcdDispBuf[0],"Tipo:SP240 "); break;
		case 7: strcpy(gl_cLcdDispBuf[0],"Tipo:SP260"); break;
		case 8: strcpy(gl_cLcdDispBuf[0],"Tipo:SP280"); break;
		case 9: strcpy(gl_cLcdDispBuf[0],"Tipo:SP320");break;//20150812 新增3.23米机型
		default: strcpy(gl_cLcdDispBuf[0],"Tipo:SP300 "); break;
	}
#endif

	while (1)
	{
#if( LANGUAGE == 0 )
		switch(i)
		{
			case 1: strcpy(gl_cLcdDispBuf[1],"选择:RP_MJ135 "); break;
			case 2: strcpy(gl_cLcdDispBuf[1],"选择:RP_MJ160 "); break;
			case 3: strcpy(gl_cLcdDispBuf[1],"选择:RP_MJ180 "); break;
			case 4: strcpy(gl_cLcdDispBuf[1],"选择:RP_MJ200 "); break;
			case 5: strcpy(gl_cLcdDispBuf[1],"选择:RP_MJ220 "); break;
			case 6: strcpy(gl_cLcdDispBuf[1],"选择:RP_MJ240 "); break;
			case 7: strcpy(gl_cLcdDispBuf[1],"选择:RP_MJ260 ");	break;
			case 8: strcpy(gl_cLcdDispBuf[1],"选择:RP_MJ280 "); break;
			case 9: strcpy(gl_cLcdDispBuf[0],"选择:RP_MJ320 ");break;//20150812 新增3.23米机型
			default: strcpy(gl_cLcdDispBuf[1],"选择:RP_MJ300 "); break;
		}
#elif(LANGUAGE==1)
		switch(i)
		{
			case 1: strcpy(gl_cLcdDispBuf[1],"Select:SP135 "); break;
			case 2: strcpy(gl_cLcdDispBuf[1],"Select:SP160 "); break;
			case 3: strcpy(gl_cLcdDispBuf[1],"Select:SP180 "); break;
			case 4: strcpy(gl_cLcdDispBuf[1],"Select:SP200 "); break;
			case 5: strcpy(gl_cLcdDispBuf[1],"Select:SP220 "); break;
			case 6: strcpy(gl_cLcdDispBuf[1],"Select:SP240 "); break;
			case 7: strcpy(gl_cLcdDispBuf[1],"Select:SP260 "); break;
			case 8: strcpy(gl_cLcdDispBuf[1],"Select:SP280 "); break;
			case 9: strcpy(gl_cLcdDispBuf[0],"Select:SP320 ");break;//20150812 新增3.23米机型
			default: strcpy(gl_cLcdDispBuf[1],"Select:SP300 "); break;
		}
#elif(LANGUAGE==2)
		switch(i)
		{
			case 1: strcpy(gl_cLcdDispBuf[1],"Seleccion:SP135 "); break;
			case 2: strcpy(gl_cLcdDispBuf[1],"Seleccion:SP160 "); break;
			case 3: strcpy(gl_cLcdDispBuf[1],"Seleccion:SP180 "); break;
			case 4: strcpy(gl_cLcdDispBuf[1],"Seleccion:SP200 "); break;
			case 5: strcpy(gl_cLcdDispBuf[1],"Seleccion:SP220 "); break;
			case 6: strcpy(gl_cLcdDispBuf[1],"Seleccion:SP240 "); break;
			case 7: strcpy(gl_cLcdDispBuf[1],"Seleccion:SP260 "); break;
			case 8: strcpy(gl_cLcdDispBuf[1],"Seleccion:SP280 "); break;
			case 9: strcpy(gl_cLcdDispBuf[0],"Seleccion:SP320 ");break;//20150812 新增3.23米机型
			default: strcpy(gl_cLcdDispBuf[1],"Seleccion:SP300 "); break;
		}
#elif(LANGUAGE==3)
		switch(i)
		{
			case 1: strcpy(gl_cLcdDispBuf[1],"Secim:SP135 "); break;
			case 2: strcpy(gl_cLcdDispBuf[1],"Secim:SP160 "); break;
			case 3: strcpy(gl_cLcdDispBuf[1],"Secim:SP180 "); break;
			case 4: strcpy(gl_cLcdDispBuf[1],"Secim:SP200 "); break;
			case 5: strcpy(gl_cLcdDispBuf[1],"Secim:SP220 "); break;
			case 6: strcpy(gl_cLcdDispBuf[1],"Secim:SP240 "); break;
			case 7: strcpy(gl_cLcdDispBuf[1],"Secim:SP260 "); break;
			case 8: strcpy(gl_cLcdDispBuf[1],"Secim:SP280 "); break;
			case 9: strcpy(gl_cLcdDispBuf[0],"Secim:SP320 ");break;//20150812 新增3.23米机型
			default: strcpy(gl_cLcdDispBuf[1],"Secim:SP300 "); break;
		}
#else
				switch(i)
				{
					case 1: strcpy(gl_cLcdDispBuf[1],"Selecione:SP135 "); break;
					case 2: strcpy(gl_cLcdDispBuf[1],"Selecione:SP160 "); break;
					case 3: strcpy(gl_cLcdDispBuf[1],"Selecione:SP180 "); break;
					case 4: strcpy(gl_cLcdDispBuf[1],"Selecione:SP200 "); break;
					case 5: strcpy(gl_cLcdDispBuf[1],"Selecione:SP220 "); break;
					case 6: strcpy(gl_cLcdDispBuf[1],"Selecione:SP240 "); break;
					case 7: strcpy(gl_cLcdDispBuf[1],"Selecione:SP260 "); break;
					case 8: strcpy(gl_cLcdDispBuf[1],"Selecione:SP280 "); break;
					case 9: strcpy(gl_cLcdDispBuf[0],"Selecione:SP320 ");break;//20150812 新增3.23米机型
					default: strcpy(gl_cLcdDispBuf[1],"Selecione:SP300 "); break;
				}

#endif
		lcd_refresh();
		WaitKeyExit(0xff);		//等待有键按下
		ch = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;

		if (ch == Key_Up)
		{
			if (i == 0)
				i = 8;
			else
				i--;
		}
		else if (ch == Key_Down)
		{
			if (i == 8)
				i = 0;
			else
				i++;
		}
		else if (ch == Key_Ok)
		{
			if (gl_cFileReceive == 0)
			{
				switch(i)
				{
					case 1:gl_sPlotType = 1350;break;
					case 2:gl_sPlotType = 1630;break;
					case 3:gl_sPlotType = 1830;break;
					case 4:gl_sPlotType = 2030;break;
					case 5:gl_sPlotType = 2230;break;
					case 6:gl_sPlotType = 2430;break;
					case 7:gl_sPlotType = 2630;break;
					case 8:gl_sPlotType = 2830;break;
					case 9:gl_sPlotType = 3230;break;
					default:gl_sPlotType = 3030;break;
				}
				WaitPrintEnd();//等待打印结束
				gl_iMaxYMM = (gl_sPlotType + 3) * Y_PPMM;
				//gl_iGraphWidth = (int)((double)(gl_iMaxYMM - gl_iOriginalPos) / (double)(8 * DotPulseNum));	
				gl_iMaxYMM += (3 * (MaxMalposedDot + SprinklerSpace) + 2 * AdditionDot + OriginalSideDot + MotorSideDot + 320) * DotPulseNum;
				WriteParameter();
				break;
			}
		}
		else if (ch == Key_Pause)
		{
			WaitKeyBound(Key_Pause);
			break;
		}
		else
			continue;
	}
}

//网络设置
void SetupIPAddr(unsigned short *sNetWorkValue)
{
	unsigned char i,cTemp;
	unsigned char IPAddr[4];
	unsigned char ch;
	unsigned char cPin;
	unsigned short sBakValue[2];
	
	IPAddr[0] = (unsigned char)(*sNetWorkValue >> 8);
	IPAddr[1] = (unsigned char)(*sNetWorkValue);
	IPAddr[2] = (unsigned char)(*(sNetWorkValue+1) >> 8);
	IPAddr[3] = (unsigned char)(*(sNetWorkValue+1));
	strcpy(gl_cLcdDispBuf[0]," *");
	sprintf(gl_cLcdDispBuf[1],"%3d.%3d.%3d.%3d",IPAddr[0],IPAddr[1],IPAddr[2],IPAddr[3]);
	lcd_refresh();
	cPin = 0;
	while (1)
	{
		WaitKeyExit(0xff);		//等待有键按下
		ch = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		switch (ch)
		{
			case Key_Left:
			if (cPin > 0)
				cPin--;
			cTemp = cPin * 4 + 1;
			for (i=0;i<cTemp;i++)
				gl_cLcdDispBuf[0][i] = ' ';
			gl_cLcdDispBuf[0][i++] = '*';
			gl_cLcdDispBuf[0][i] = '\0';
			break;

			case Key_Right:
			if (cPin < 3)
				cPin++;
			cTemp = cPin * 4 + 1;
			for (i=0;i<cTemp;i++)
				gl_cLcdDispBuf[0][i] = ' ';
			gl_cLcdDispBuf[0][i++] = '*';
			gl_cLcdDispBuf[0][i] = '\0';
			break;

			case Key_Up:
			while (1)
			{
				if (IPAddr[cPin] < 0xff)
					IPAddr[cPin]++;
				else
					IPAddr[cPin] = 0;
				sprintf(gl_cLcdDispBuf[1],"%3d.%3d.%3d.%3d",IPAddr[0],IPAddr[1],IPAddr[2],IPAddr[3]);
				lcd_refresh();
				for (sBakValue[0]=0;sBakValue[0]<20;sBakValue[0]++)
				{
					for (sBakValue[1]=0;sBakValue[1]<6400;sBakValue[1]++)
						ReceiveData();				//接收数据
				}
				if (gl_cKeyPH != gl_cKeyPE)
				{
					if (gl_cKeyBuf[gl_cKeyPE] != Key_Up)
						break;
					else if ((gl_cKeyPE +1) == KeyBufLength)
						gl_cKeyPE = 0;
					else
						gl_cKeyPE++;
				}
			}
			break;

			case Key_Down:
			while (1)
			{
				if (IPAddr[cPin] > 0)
					IPAddr[cPin]--;
				else
					IPAddr[cPin] = 0xff;
				sprintf(gl_cLcdDispBuf[1],"%3d.%3d.%3d.%3d",IPAddr[0],IPAddr[1],IPAddr[2],IPAddr[3]);
				
				lcd_refresh();
				for (sBakValue[0]=0;sBakValue[0]<20;sBakValue[0]++)
				{
					for (sBakValue[1]=0;sBakValue[1]<6400;sBakValue[1]++)
						ReceiveData();				//接收数据
				}
				if (gl_cKeyPH != gl_cKeyPE)
				{
					if (gl_cKeyBuf[gl_cKeyPE] != Key_Down)
						break;
					else if ((gl_cKeyPE +1) == KeyBufLength)
						gl_cKeyPE = 0;
					else
						gl_cKeyPE++;
				}
			}
			break;

			case Key_Ok:
			if (gl_cFileReceive == 0)
			{
				sBakValue[0] = *sNetWorkValue;
				sBakValue[1] = *(sNetWorkValue+1);
				*sNetWorkValue = ((unsigned short)IPAddr[0] << 8) + (unsigned short)IPAddr[1];
				*(sNetWorkValue+1) = ((unsigned short)IPAddr[2] << 8) + (unsigned short)IPAddr[3];
				if (WriteParameter() == 1)
					return;
				else
				{
#if(LANGUAGE==0)
					lcd_display(0,"参数设置失败!");
					lcd_display(1," ");
#elif(LANGUAGE==1)
					lcd_display(0,"Paremeter save");
					lcd_display(1,"err! ");
#else
					lcd_display(0,"Salvar Param");
					lcd_display(1,"Error! ");
#endif
					WaitKeyBound(Key_Ok);
					*sNetWorkValue = sBakValue[0];
					*(sNetWorkValue+1) = sBakValue[1];
				}
				return;
			}
			else
				break;

			case Key_Pause:
			WaitKeyBound(Key_Pause);
			return;

			default:
			break;
		}
		lcd_refresh();
	}
}

//版本号显示
void VersionDisplay(void)
{
#if( LANGUAGE == 0 )
	strcpy(gl_cLcdDispBuf[0],"版本号:V3.44");
	strcpy(gl_cLcdDispBuf[1],"日期:14/07/07");
#elif(LANGUAGE==1)
	strcpy(gl_cLcdDispBuf[0],"Ver:V3.44");
	strcpy(gl_cLcdDispBuf[1],"Date:14/07/07");
#elif(LANGUAGE==2)
	strcpy(gl_cLcdDispBuf[0],"Ver:V3.44");
	strcpy(gl_cLcdDispBuf[1],"Fecha:14/07/07");
#elif(LANGUAGE==3)
	strcpy(gl_cLcdDispBuf[0],"Version:V3.44");
	strcpy(gl_cLcdDispBuf[1],"Tarih:14/07/07");
#else
	strcpy(gl_cLcdDispBuf[0],"Ver:V3.44");
	strcpy(gl_cLcdDispBuf[1],"Data:14/07/07");
#endif
	lcd_refresh();
	WaitKeyExit(Key_Pause);
}

//等待按键弹起
void WaitKeyBound(unsigned char cKeyValue)
{
	while (1)
	{
		ReceiveData();				//接收数据
		if (gl_cKeyPH == gl_cKeyPE)
			continue;
		if (gl_cKeyBuf[gl_cKeyPE] != cKeyValue)
			break;
		else if ((gl_cKeyPE +1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
	}
}
//等待按键退出
void WaitKeyExit(unsigned char cKeyValue)
{
	unsigned char ch;
	while (1)
	{
		ReceiveData();				//接收数据
		if (gl_cKeyPH == gl_cKeyPE)
			continue;
		ch = gl_cKeyBuf[gl_cKeyPE];
		if (cKeyValue == 0xff)
		{
			if (ch == 0)
			{
				if ((gl_cKeyPE +1) == KeyBufLength)
					gl_cKeyPE = 0;
				else
					gl_cKeyPE++;
			}
			else
				break;
		}
		else
		{
			if ((gl_cKeyPE +1) == KeyBufLength)
				gl_cKeyPE = 0;
			else
				gl_cKeyPE++;
			if (ch == cKeyValue)
			{
				WaitKeyBound(cKeyValue);
				break;
			}
		}
	}
}
//等待打印结束
void WaitPrintEnd(void)
{
	while (ReadPrintEnd() == 0)
		ReceiveData();				//接收数据
}
//**************************************************
//功能：	通过灯的亮灭提示错误信息
//输入参数：iErrorCode错误代码
//返回值：无
//**************************************************
void ViewError(int iErrorCode)
{
	strcpy(gl_cLcdDispBuf[1]," ");
#if( LANGUAGE == 0 )
	switch (iErrorCode)
	{
		case ER_INITPORT:					//初始化串口出错
		strcpy(gl_cLcdDispBuf[0],"初始化串口出错");
		break;

		case ER_FRAME:						//帧出错
		case ER_PARITY:						//校验错
		strcpy(gl_cLcdDispBuf[0],"    通讯错误!");
		break;

		case ER_XOVER:						//X轴移动超界
		strcpy(gl_cLcdDispBuf[0]," X轴移动超界!");
		break;

		case ER_YOVER:						//Y轴移动超界
		strcpy(gl_cLcdDispBuf[0]," Y轴移动超界!");
		break;

		case ER_YOBAD:						//Y轴原点传感器坏
		strcpy(gl_cLcdDispBuf[0],"  原点感应器坏");
		break;

		case ER_SRAM:
		strcpy(gl_cLcdDispBuf[0],"  SRAM自检出错");
		break;

		case ER_POSITIONX:
		strcpy(gl_cLcdDispBuf[0],"X 轴位置偏差大");
		break;

		case ER_POSITIONY:
		strcpy(gl_cLcdDispBuf[0],"Y 轴位置偏差大");
		break;

		case ER_NOPAPER:
		strcpy(gl_cLcdDispBuf[0],"纸张用完!");
		strcpy(gl_cLcdDispBuf[1],"7:退出");
		lcd_refresh();
		WaitKeyExit(Key_Pause);
		return;

		default:
		break;
	}
#elif(LANGUAGE==1)
	switch (iErrorCode)
	{
		case ER_INITPORT:					//初始化串口出错
		strcpy(gl_cLcdDispBuf[0],"COM initial err");
		break;

		case ER_FRAME:						//帧出错
		case ER_PARITY:						//校验错
		strcpy(gl_cLcdDispBuf[0],"communicate err");
		break;

		case ER_XOVER:						//X轴移动超界
		strcpy(gl_cLcdDispBuf[0],"X:out-of-range");
		break;

		case ER_YOVER:						//Y轴移动超界
		strcpy(gl_cLcdDispBuf[0],"Y:out-of-range");
		break;

		case ER_YOBAD:						//Y轴原点传感器坏
		strcpy(gl_cLcdDispBuf[0],"Bad origin");
		break;

		case ER_SRAM:
		strcpy(gl_cLcdDispBuf[0],"SRAM error");
		break;

		case ER_POSITIONX:
		strcpy(gl_cLcdDispBuf[0],"Positional err");
		break;

		case ER_POSITIONY:
		strcpy(gl_cLcdDispBuf[0],"Positional err");
		break;

		case ER_NOPAPER:
		strcpy(gl_cLcdDispBuf[0],"No paper!");
		strcpy(gl_cLcdDispBuf[1],"7:exit");
		lcd_refresh();
		WaitKeyExit(Key_Pause);
		return;

		default:
		break;
	}
#elif(LANGUAGE==2)
	switch (iErrorCode)
	{
		case ER_INITPORT:					//初始化串口出错
		strcpy(gl_cLcdDispBuf[0],"Error inic.COM");
		break;

		case ER_FRAME:						//帧出错
		case ER_PARITY:						//校验错
		strcpy(gl_cLcdDispBuf[0],"Error comunicac");
		break;

		case ER_XOVER:						//X轴移动超界
		strcpy(gl_cLcdDispBuf[0],"X Fuera d rango");
		break;

		case ER_YOVER:						//Y轴移动超界
		strcpy(gl_cLcdDispBuf[0],"Y Fuera d rango");
		break;

		case ER_YOBAD:						//Y轴原点传感器坏
		strcpy(gl_cLcdDispBuf[0],"Origen erroneo");
		break;

		case ER_SRAM:
		strcpy(gl_cLcdDispBuf[0],"Error mem SRAM");
		break;

		case ER_POSITIONX:
		strcpy(gl_cLcdDispBuf[0],"Err de posicion");
		break;

		case ER_POSITIONY:
		strcpy(gl_cLcdDispBuf[0],"Err de posicion");
		break;

		case ER_NOPAPER:
		strcpy(gl_cLcdDispBuf[0],"SIN PAPEL...!");
		strcpy(gl_cLcdDispBuf[1],"7:Salir");
		lcd_refresh();
		WaitKeyExit(Key_Pause);
		return;

		default:
		break;
	}
	
#elif(LANGUAGE==3)
	switch (iErrorCode)
	{
		case ER_INITPORT:					//初始化串口出错
		strcpy(gl_cLcdDispBuf[0],"Seriport Hatasi");
		break;

		case ER_FRAME:						//帧出错
		case ER_PARITY:						//校验错
		strcpy(gl_cLcdDispBuf[0],"Baglanti Hatasi");
		break;

		case ER_XOVER:						//X轴移动超界
		strcpy(gl_cLcdDispBuf[0],"X:kontur disi!");
		break;

		case ER_YOVER:						//Y轴移动超界
		strcpy(gl_cLcdDispBuf[0],"Y:Kontur Disi!");
		break;

		case ER_YOBAD:						//Y轴原点传感器坏
		strcpy(gl_cLcdDispBuf[0],"Baslangic");
		break;

		case ER_SRAM:
		strcpy(gl_cLcdDispBuf[0],"Ram Hatasi");
		break;

		case ER_POSITIONX:
		strcpy(gl_cLcdDispBuf[0],"Baslama Hatasi");
		break;

		case ER_POSITIONY:
		strcpy(gl_cLcdDispBuf[0],"Baslama Hatasi");
		break;

		case ER_NOPAPER:
		strcpy(gl_cLcdDispBuf[0],"Kagit Yok!");
		strcpy(gl_cLcdDispBuf[1],"7:Cikis");
		lcd_refresh();
		WaitKeyExit(Key_Pause);
		return;

		default:
		break;
	}
#else
		switch (iErrorCode)
		{
			case ER_INITPORT:					//初始化串口出错
			strcpy(gl_cLcdDispBuf[0],"Erro porta COM");
			break;
	
			case ER_FRAME:						//帧出错
			case ER_PARITY: 					//校验错
			strcpy(gl_cLcdDispBuf[0],"Errocomunicacao");
			break;
	
			case ER_XOVER:						//X轴移动超界
			strcpy(gl_cLcdDispBuf[0],"X:ForaDoLimite!");
			break;
	
			case ER_YOVER:						//Y轴移动超界
			strcpy(gl_cLcdDispBuf[0],"Y:ForaDoLimite!");
			break;
	
			case ER_YOBAD:						//Y轴原点传感器坏
			strcpy(gl_cLcdDispBuf[0],"Problema Origem");
			break;
	
			case ER_SRAM:
			strcpy(gl_cLcdDispBuf[0],"Erro SRAM");
			break;
	
			case ER_POSITIONX:
			strcpy(gl_cLcdDispBuf[0],"Baslama Hatasi");
			break;
	
			case ER_POSITIONY:
			strcpy(gl_cLcdDispBuf[0],"Baslama Hatasi");
			break;
	
			case ER_NOPAPER:
			strcpy(gl_cLcdDispBuf[0],"Sem Papel!!");
			strcpy(gl_cLcdDispBuf[1],"7: Sair");
			lcd_refresh();
			WaitKeyExit(Key_Pause);
			return;
	
			default:
			break;
		}

#endif

	lcd_refresh();
	while (1);
}

//删除当前剩余文件
void DeleteCurFile(void)
{
	unsigned char ch;
#if( LANGUAGE == 0 )
	strcpy(gl_cLcdDispBuf[0],"  真的删除吗?");
	strcpy(gl_cLcdDispBuf[1],"5:确定  7:退出");
#elif(LANGUAGE==1)
	strcpy(gl_cLcdDispBuf[0]," Delete,realy?");
	strcpy(gl_cLcdDispBuf[1],"5:Yes    7:Exit");
#elif(LANGUAGE==2)
	strcpy(gl_cLcdDispBuf[0],"   Borrar?");
	strcpy(gl_cLcdDispBuf[1],"5:Si   7:Salir");
#elif(LANGUAGE==3)
	strcpy(gl_cLcdDispBuf[0],"Sil Gercekten?");
	strcpy(gl_cLcdDispBuf[1],"5:evet 7:cikis");
#else
	strcpy(gl_cLcdDispBuf[0],"CertezaRemover?");
	strcpy(gl_cLcdDispBuf[1],"5: Sim 7: Sair");
#endif
	lcd_refresh();
	while (1)
	{
		WaitKeyExit(0xff);	//等待有按键
		ch = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		if (ch == Key_Ok)
		{
			gl_iEthnetRecBufPH = 0;
			gl_iEthnetRecBufPE = 0;
			gl_cFileReceive = 0;
			gl_cCommunicationMode = 0xff;
			gl_cInvertBufStatus = 0;
			gl_cCurPrintBuf = 0;
			gl_cRemainderBit = 16;
			gl_cCountFlag = 0;
			if ((gl_iPulseNumY < 0) && (gl_cPrintStatus != 0x10))
			{//喷头回原点
#if( LANGUAGE == 0 ) 
				strcpy(gl_cLcdDispBuf[0],"喷头回原点");
				strcpy(gl_cLcdDispBuf[1],"请稍后...");
#elif(LANGUAGE==1)
				strcpy(gl_cLcdDispBuf[0],"waiting...");
				strcpy(gl_cLcdDispBuf[1]," ");
#elif(LANGUAGE==2)
				strcpy(gl_cLcdDispBuf[0],"Esperando...");
				strcpy(gl_cLcdDispBuf[1]," ");
#elif(LANGUAGE==3)
				strcpy(gl_cLcdDispBuf[0],"Lutfen Bekleyin!");
				strcpy(gl_cLcdDispBuf[1]," ");
#else
				strcpy(gl_cLcdDispBuf[0],"Esperando!");
				strcpy(gl_cLcdDispBuf[1]," ");

#endif
				lcd_refresh();
				ClrFIFO;
				DriveSprinkler(0,gl_iPulseNumY);//驱动Y轴
				gl_iAxisYPos += gl_iPulseNumY;
				gl_iPulseNumY = -gl_iPulseNumY;
				WaitPrintEnd();
			}
			gl_WatchDogState = 0;//20101118
			gl_WatchDogCount = 0;
			break;
		}
		else if (ch == Key_Pause)
			break;
		else
			continue;
	}	
}

//感应器自检
void ReactorCheck(void)
{
	unsigned char cTemp;
	unsigned char DispView=0, DispMen=1;//20100225 按键无响应
	gl_cSelfCheckFlag = 1;
#if( LANGUAGE == 0 )
	strcpy(gl_cLcdDispBuf[1],"7:退出");
#elif(LANGUAGE==1)
	strcpy(gl_cLcdDispBuf[1],"7:exit");
#elif(LANGUAGE==2)
	strcpy(gl_cLcdDispBuf[1],"7:Salir");
#elif(LANGUAGE==3)
	strcpy(gl_cLcdDispBuf[1],"7:Cikis");
#else
	strcpy(gl_cLcdDispBuf[1],"7:Sair");

#endif

#if( LANGUAGE == 0 )
	while(1)
	{
		if (GetIOBit(I_YORIGIN) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"原点位置到达");
			DispView=1;//20100225
		}
		else if (GetIOBit(I_ROLLPEND) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"收纸位置到达");
			DispView=1;
		}
		else if (GetIOBit(I_ROLLSTART) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"启动收纸");
			DispView=1;
		}
		else if (GetIOBit(I_GIVEBACK) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"送纸位置到达");
			DispView=1;
		}
		else if (GetIOBit(I_GIVEPEND) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"启动送纸");
			DispView=1;
		}
			
		if(DispView)
		{//20100225
			lcd_refresh();
			DispView=0;
			DispMen=1;
		}
		else if(DispMen)
			{//20100225
				strcpy(gl_cLcdDispBuf[0],"感应器检测");
				lcd_refresh();				
				DispMen=0;
			}
		
		if (gl_cKeyPE != gl_cKeyPH)
		{
			cTemp = gl_cKeyBuf[gl_cKeyPE];
			if ((gl_cKeyPE + 1) == KeyBufLength)
				gl_cKeyPE = 0;
			else
				gl_cKeyPE++;
			if (cTemp == Key_Pause)
				break;
		}
		ReceiveData();				//接收数据
	}
#elif(LANGUAGE==1)
	while(1)
	{
		if (GetIOBit(I_YORIGIN) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"ORIGIN ok!");
			DispView=1;//20100225
		}
		else if (GetIOBit(I_ROLLPEND) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"RollPaper stop!");
			DispView=1;
		}
		else if (GetIOBit(I_ROLLSTART) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"RollPaper start");
			DispView=1;
		}
		else if (GetIOBit(I_GIVEBACK) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"FeedPaper stop!");
			DispView=1;
		}
		else if (GetIOBit(I_GIVEPEND) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"FeedPaper start");
			DispView=1;
		}
			
		if(DispView)
		{//20100225
			lcd_refresh();
			DispView=0;
			DispMen=1;
		}
		else if(DispMen)
			{//20100225
				strcpy(gl_cLcdDispBuf[0],"reactor check");
				lcd_refresh();				
				DispMen=0;
			}
		
		if (gl_cKeyPE != gl_cKeyPH)
		{
			cTemp = gl_cKeyBuf[gl_cKeyPE];
			if ((gl_cKeyPE + 1) == KeyBufLength)
				gl_cKeyPE = 0;
			else
				gl_cKeyPE++;
			if (cTemp == Key_Pause)
				break;
		}
		ReceiveData();				//接收数据
	}
#elif(LANGUAGE==2)
	while(1)
	{
		if (GetIOBit(I_YORIGIN) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"Origen OK!");
			DispView=1;//20100225
		}
		else if (GetIOBit(I_ROLLPEND) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"Enro papel stop");
			DispView=1;
		}
		else if (GetIOBit(I_ROLLSTART) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"Inicia enrollar");
			DispView=1;
		}
		else if (GetIOBit(I_GIVEBACK) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"Avan Papel stop");
			DispView=1;
		}
		else if (GetIOBit(I_GIVEPEND) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"Avan Papel inic");
			DispView=1;
		}
			
		if(DispView)
		{//20100225
			lcd_refresh();
			DispView=0;
			DispMen=1;
		}
		else if(DispMen)
		{//20100225
			strcpy(gl_cLcdDispBuf[0],"Reaktor Kontrol");
			lcd_refresh();				
			DispMen=0;
		}
		
		if (gl_cKeyPE != gl_cKeyPH)
		{
			cTemp = gl_cKeyBuf[gl_cKeyPE];
			if ((gl_cKeyPE + 1) == KeyBufLength)
				gl_cKeyPE = 0;
			else
				gl_cKeyPE++;
			if (cTemp == Key_Pause)
				break;
		}
		ReceiveData();				//接收数据
	}
	
#elif(LANGUAGE==3)
	while(1)
	{
		if (GetIOBit(I_YORIGIN) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"Baslangic Onay!");
			DispView=1;//20100225
		}
		else if (GetIOBit(I_ROLLPEND) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"On-Bar-Dur");
			DispView=1;
		}
		else if (GetIOBit(I_ROLLSTART) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"On-Bar-Basla");
			DispView=1;
		}
		else if (GetIOBit(I_GIVEBACK) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"Kagit-Bes-Dur");
			DispView=1;
		}
		else if (GetIOBit(I_GIVEPEND) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"Reaktor Kontrol");
			DispView=1;
		}
			
		if(DispView)
		{//20100225
			lcd_refresh();
			DispView=0;
			DispMen=1;
		}
		else if(DispMen)
		{//20100225
			strcpy(gl_cLcdDispBuf[0],"Verifica sensor");
			lcd_refresh();				
			DispMen=0;
		}
		
		if (gl_cKeyPE != gl_cKeyPH)
		{
			cTemp = gl_cKeyBuf[gl_cKeyPE];
			if ((gl_cKeyPE + 1) == KeyBufLength)
				gl_cKeyPE = 0;
			else
				gl_cKeyPE++;
			if (cTemp == Key_Pause)
				break;
		}
		ReceiveData();				//接收数据
	}
#else

	while(1)
	{
		if (GetIOBit(I_YORIGIN) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"Origem Ok!");
			DispView=1;//20100225
		}
		else if (GetIOBit(I_ROLLPEND) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"Rebobina Para");
			DispView=1;
		}
		else if (GetIOBit(I_ROLLSTART) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"Rebobina Papel");
			DispView=1;
		}
		else if (GetIOBit(I_GIVEBACK) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"Soltar Para");
			DispView=1;
		}
		else if (GetIOBit(I_GIVEPEND) == StatusOk)
		{
			strcpy(gl_cLcdDispBuf[0],"Soltar Inicia");
			DispView=1;
		}
			
		if(DispView)
		{//20100225
			lcd_refresh();
			DispView=0;
			DispMen=1;
		}
		else if(DispMen)
		{//20100225
			strcpy(gl_cLcdDispBuf[0],"Verifica Sensor");
			lcd_refresh();				
			DispMen=0;
		}
		
		if (gl_cKeyPE != gl_cKeyPH)
		{
			cTemp = gl_cKeyBuf[gl_cKeyPE];
			if ((gl_cKeyPE + 1) == KeyBufLength)
				gl_cKeyPE = 0;
			else
				gl_cKeyPE++;
			if (cTemp == Key_Pause)
				break;
		}
		ReceiveData();				//接收数据
	}
#endif
	gl_cSelfCheckFlag = 0;
}

void KeyboardCheck(void)
{
	char cKey;
	char ch;
	
	cKey = '0';
	do
	{
		cKey++;
#if(LANGUAGE==0)
		strcpy(gl_cLcdDispBuf[0],"  按键测试");
		sprintf(gl_cLcdDispBuf[1],"  请按键'%c'!",cKey);
#elif(LANGUAGE==1)
		strcpy(gl_cLcdDispBuf[0],"keyboard check");
		sprintf(gl_cLcdDispBuf[1],"Press key '%c'!",cKey);
#elif(LANGUAGE==2)
		strcpy(gl_cLcdDispBuf[0],"Verifica teclas");
		sprintf(gl_cLcdDispBuf[1],"Presione '%c'!",cKey);
#elif(LANGUAGE==3)
		strcpy(gl_cLcdDispBuf[0],"Klavye Kontrol");
		sprintf(gl_cLcdDispBuf[1],"Tusa Basiniz '%c'!",cKey);
#else
		strcpy(gl_cLcdDispBuf[0],"VerificaTeclado");
		sprintf(gl_cLcdDispBuf[1],"Pressione tecla '%c'!",cKey);
#endif
		lcd_refresh();
		gl_cKeyPE = gl_cKeyPH;
		while (1)
		{
			WaitKeyExit(0xff);		//等待有键按下
			ch = gl_cKeyBuf[gl_cKeyPE];
			if (ch != cKey)
			{
#if(LANGUAGE==0)
				strcpy(gl_cLcdDispBuf[0],"    错误");
				sprintf(gl_cLcdDispBuf[1],"您按下的是键%c!",ch);
#elif(LANGUAGE==1)
				strcpy(gl_cLcdDispBuf[0],"    WARING!");
				sprintf(gl_cLcdDispBuf[1],"The key is '%c'!",ch);
#elif(LANGUAGE==2)
				strcpy(gl_cLcdDispBuf[0],"  ATENCION!");
				sprintf(gl_cLcdDispBuf[1],"Tecla es '%c!'",ch);
#elif(LANGUAGE==3)
				strcpy(gl_cLcdDispBuf[0],"  Uyari!!");
				sprintf(gl_cLcdDispBuf[1],"Anahtar '%c!'",ch);
#else
				strcpy(gl_cLcdDispBuf[0],"	AVISO!!");
				sprintf(gl_cLcdDispBuf[1],"A tecla foi '%c!'",ch);
#endif
				lcd_refresh();
				Delay(20000);
			}
			if ((gl_cKeyPE + 1) == KeyBufLength)
				gl_cKeyPE = 0;
			else
				gl_cKeyPE++;
			break;
		}
	}while (cKey != '9');
}

void TestRecSendMotor(unsigned char cFlag)
{
	unsigned char cTemp;
	
	gl_cSelfCheckFlag = 1;
#if(LANGUAGE==0)
	if (cFlag == 0)
	{  
		strcpy(gl_cLcdDispBuf[0],"2:送纸电机收纸");
		strcpy(gl_cLcdDispBuf[1],"8:送纸电机送纸");
	}
	else
	{
		strcpy(gl_cLcdDispBuf[0],"2:收纸电机收纸");
		strcpy(gl_cLcdDispBuf[1],"8:收纸电机退纸");
	}
#elif(LANGUAGE==1)
	if (cFlag == 0)
	{  
		strcpy(gl_cLcdDispBuf[0],"2:rolling paper");
		strcpy(gl_cLcdDispBuf[1],"8:feeding paper");
	}
	else
	{
		strcpy(gl_cLcdDispBuf[0],"2:rolling paper");
		strcpy(gl_cLcdDispBuf[1],"8:rewind paper");
	}
#elif(LANGUAGE==2)
	if (cFlag == 0)
	{  
		strcpy(gl_cLcdDispBuf[0],"2:Desenrollar");
		strcpy(gl_cLcdDispBuf[1],"8:Avanzar papel");
	}
	else
	{
		strcpy(gl_cLcdDispBuf[0],"2:Desenrollar");
		strcpy(gl_cLcdDispBuf[1],"8:Rebobinar");
	}
#elif(LANGUAGE==3)
	if (cFlag == 0)
	{  
		strcpy(gl_cLcdDispBuf[0],"2:Kagit Sarma");
		strcpy(gl_cLcdDispBuf[1],"8:Kagit Besleme");
	}
	else
	{
		strcpy(gl_cLcdDispBuf[0],"2:Kagit Sarma");
		strcpy(gl_cLcdDispBuf[1],"8:Geri Sarma");
	}
#else
		if (cFlag == 0)
		{  
			strcpy(gl_cLcdDispBuf[0],"2:Retorna Papel");
			strcpy(gl_cLcdDispBuf[1],"8:Entrega Papel");
		}
		else
		{
			strcpy(gl_cLcdDispBuf[0],"2:GuardaPlotage");
			strcpy(gl_cLcdDispBuf[1],"8:VoltaPlotagem");
		}

#endif
	lcd_refresh();
	while (1)
	{
		WaitKeyExit(0xff);		//等待有键按下
		cTemp = gl_cKeyBuf[gl_cKeyPE];
		if ((gl_cKeyPE+1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		if (cTemp == Key_Pause)
			break;
		else if (cTemp == Key_Down)
		{
			if (cFlag == 0)
				SetIOBit(O_GIVEMOTORDIR,G_SENDPAPER);			//送纸电机方向为送纸
			else
				SetIOBit(O_ROLLMOTORDIR,R_BACKPAPER);			//收纸电机方向为退纸
		}
		else if (cTemp == Key_Up)
		{
			if (cFlag == 0)
				SetIOBit(O_GIVEMOTORDIR,G_ROLLPAPER);			//送纸电机方向为收纸
			else
				SetIOBit(O_ROLLMOTORDIR,R_RECEIVEPAPER);	//收纸电机方向为收纸
		}
		else
			continue;
#if(LANGUAGE==0)
		strcpy(gl_cLcdDispBuf[0],"按任意键停止");
		strcpy(gl_cLcdDispBuf[1]," ");
#elif(LANGUAGE==1)
		strcpy(gl_cLcdDispBuf[0],"Press any Key");
		strcpy(gl_cLcdDispBuf[1],"to stop!");
#elif(LANGUAGE==2)
		strcpy(gl_cLcdDispBuf[0],"Pres. una tecla");
		strcpy(gl_cLcdDispBuf[1],"para detener");
#elif(LANGUAGE==3)
		strcpy(gl_cLcdDispBuf[0],"Bir-Tusa-Basin");
		strcpy(gl_cLcdDispBuf[1]," ");
#else
		strcpy(gl_cLcdDispBuf[0],"Pressione Parar");
		strcpy(gl_cLcdDispBuf[1]," ");
#endif
		lcd_refresh();
		Delay(50);
		if (cFlag == 0)
		{
			SetIOBit(O_STARTGIVEPMOTOR,0);			//启动送纸电机
			if (cTemp == Key_Down)
			{
				while (1)
				{
					if (GetIOBit(I_GIVEBACK) == StatusOk)
						break;
					if (gl_cKeyPH != gl_cKeyPE)
					{
						cTemp = gl_cKeyBuf[gl_cKeyPE];
						if (cTemp == 0)
						{
							if ((gl_cKeyPE +1) == KeyBufLength)
								gl_cKeyPE = 0;
							else
								gl_cKeyPE++;
						}
						else
							break;
					}
				}
			}
			else
			{
				while (1)
				{
					if (GetIOBit(I_GIVEPEND) == StatusOk)
						break;
					if (gl_cKeyPH != gl_cKeyPE)
					{
						cTemp = gl_cKeyBuf[gl_cKeyPE];
						if (cTemp == 0)
						{
							if ((gl_cKeyPE +1) == KeyBufLength)
								gl_cKeyPE = 0;
							else
								gl_cKeyPE++;
						}
						else
							break;
					}
				}
			}
			SetIOBit(O_STARTGIVEPMOTOR,1);			//关闭送纸电机
			Delay(50);
			if (gl_cKeyPE != gl_cKeyPH)
			{
				if ((gl_cKeyPE+1) == KeyBufLength)
					gl_cKeyPE = 0;
				else
					gl_cKeyPE++;
			}
#if(LANGUAGE==0)
			strcpy(gl_cLcdDispBuf[0],"2:送纸电机收纸");
			strcpy(gl_cLcdDispBuf[1],"8:送纸电机送纸");
#elif(LANGUAGE==1)
			strcpy(gl_cLcdDispBuf[0],"2:rolling paper");
			strcpy(gl_cLcdDispBuf[1],"8:feeding paper");
#elif(LANGUAGE==2)
			strcpy(gl_cLcdDispBuf[0],"2:Desenrollar");
			strcpy(gl_cLcdDispBuf[1],"8:Avanzar papel");
		
#elif(LANGUAGE==3)

			strcpy(gl_cLcdDispBuf[0],"2:Kagit Sarma");
			strcpy(gl_cLcdDispBuf[1],"8:Kagit Besleme");
#else 
			strcpy(gl_cLcdDispBuf[0],"2:Retorna Papel");
			strcpy(gl_cLcdDispBuf[1],"8:Entrega Papel");		
#endif
			lcd_refresh();
		}
		else
		{
			SetIOBit(O_STARTROLLPMOTOR,0);			//启动收纸电机
			if (cTemp == Key_Down)
			{
				while (1)
				{
					if (GetIOBit(I_ROLLSTART) == StatusOk)
						break;
					if (gl_cKeyPH != gl_cKeyPE)
					{
						cTemp = gl_cKeyBuf[gl_cKeyPE];
						if (cTemp == 0)
						{
							if ((gl_cKeyPE +1) == KeyBufLength)
								gl_cKeyPE = 0;
							else
								gl_cKeyPE++;
						}
						else
							break;
					}
				}
			}
			else
			{
				while (1)
				{
					if (GetIOBit(I_ROLLPEND) == StatusOk)
						break;
					if (gl_cKeyPH != gl_cKeyPE)
					{
						cTemp = gl_cKeyBuf[gl_cKeyPE];
						if (cTemp == 0)
						{
							if ((gl_cKeyPE +1) == KeyBufLength)
								gl_cKeyPE = 0;
							else
								gl_cKeyPE++;
						}
						else
							break;
					}
				}
			}
			SetIOBit(O_STARTROLLPMOTOR,1);			//关闭收纸电机
			Delay(50);
			if (gl_cKeyPE != gl_cKeyPH)
			{
				if ((gl_cKeyPE+1) == KeyBufLength)
					gl_cKeyPE = 0;
				else
					gl_cKeyPE++;
			}
#if(LANGUAGE==0)
			strcpy(gl_cLcdDispBuf[0],"2:收纸电机收纸");
			strcpy(gl_cLcdDispBuf[1],"8:收纸电机退纸");
#elif(LANGUAGE==1)
			strcpy(gl_cLcdDispBuf[0],"2:rolling paper");
			strcpy(gl_cLcdDispBuf[1],"8:feeding paper");
#elif(LANGUAGE==2)
			strcpy(gl_cLcdDispBuf[0],"2:Desenrollar");
			strcpy(gl_cLcdDispBuf[1],"8:Avanzar papel");
#elif(LANGUAGE==3)
			strcpy(gl_cLcdDispBuf[0],"2:Kagit Sarma");
			strcpy(gl_cLcdDispBuf[1],"8:Kagit Besleme");
#else
			strcpy(gl_cLcdDispBuf[0],"2:GuardaPlotage");
			strcpy(gl_cLcdDispBuf[1],"8:VoltaPlotagem");;
#endif
			lcd_refresh();
		}
	}
	if (cFlag == 0)
		SetIOBit(O_GIVEMOTORDIR,G_SENDPAPER);			//送纸电机方向为送纸
	else
		SetIOBit(O_ROLLMOTORDIR,R_RECEIVEPAPER);	//收纸电机方向为收纸
	Delay(50);
	gl_cSelfCheckFlag = 0;
}
