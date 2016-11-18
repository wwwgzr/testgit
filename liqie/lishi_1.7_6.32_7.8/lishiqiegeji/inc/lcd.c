//*****************LCD.Cģ��˵��******************
//����: 
// 1,Һ����ʼ������ʾ�ַ�����ˢ��LCD
// 2,������
// 3,������Ӧ����
//*****************************************************

#include "..\src\defconst.h"
#include "..\Target\44blib.h"
#include "..\src\extern.h"
#include <stdio.h>
#include <string.h>

extern void ReceiveData(void);
// LCD
void lcd_init(void);
void lcd_clr(void);
void char_reverse(unsigned char line);
void lcd_refresh(void);
void lcd_display(unsigned char row,const char *string);
void char_reverse(unsigned char line);
void LineReverse(unsigned char line);
void LcdDisp(char *s1, char *s2);
//������Ӧ
void WaitKeyBound(unsigned char cKeyValue);
void WaitKeyExit(unsigned char cKeyValue);
//������
void ViewError(int iErrorCode);
/*
void test_lcd(void)
{
	gl_scankeyflag = 0;
	Delay(50);
	//�����趨DDRAM��ַrow * 16����
	WriteLcdCommand(0x80);
	Delay(8);
	WriteLcdData(0xa1);
	WriteLcdData(0xd2);
	WriteLcdData(0xa8);
	WriteLcdData(0xae);
	WriteLcdData(0x4);
	WriteLcdData(0x0);
	WriteLcdData(0x0);
	WriteLcdData(0x4);
	Delay(8);
	WriteLcdCommand(0xc); //20100222
	gl_scankeyflag =1 ;
}
*/
//LCD��ʼ������
void lcd_init(void)
{
	gl_scankeyflag = 0;
	Delay(50);
	WriteLcdCommand(0x30); 		//����8λ��������00110000
	WriteLcdCommand(0x30); 		//����8λ��������00110000
	WriteLcdCommand(0xc); 			//���Ϳ���ʾ�ع������00001100
	WriteLcdCommand(0x1);			//���������ʾ����00000001
	Delay(200);
	WriteLcdCommand(0x06);
	gl_cLCDStatus = 0;

	/*20141202 rwt test */
	Printf("\n gl_clanguageselect: %d\n",gl_cLanguageSelect);
	
	if(gl_cLanguageSelect == 0)
	{
		LcdDisp("��ʽ��ī�и��", "���ڳ�ʼ��������");
	}
	else
	{
		LcdDisp("Cutter Plotter ", "initializing...");		//20141201 rwt
	}
	gl_scankeyflag = 1;
	
}

//�����������Ļ���������ݣ�
void lcd_clr(void)
{
	gl_scankeyflag = 0;
	Delay(50);
	WriteLcdCommand(0x1); 			//���������ʾ����00000001
	Delay(200);
	gl_scankeyflag = 1;
}

//�����ַ�������
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

//�����ַ�����ʾ״̬����1λ�����0��,��2λ�����1��;ֵΪ0������,Ϊ0����)
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
//���ܣ�	��Ļˢ��
//��ڲ�������
//����ֵ����
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
//���ܣ�	��ָ��λ����ʾ����
//��ڲ�����row��ʾ�У�col��ʾ�У�string��ʾ�ַ���
//����ֵ����
//**************************************************************
void lcd_display(unsigned char row,const char *string)
{
	unsigned short i;
	gl_scankeyflag = 0;//20100222
	Delay(50);//20100222
	if (row < 2)
	{
		//�����ַ�Ϊ2��7.5��(���ֱ���ż������)
		//�����趨DDRAM��ַrow * 16����
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

void LcdDisp(char *s1, char *s2)
{
	char InputState=2;

	if(strcmp(s1, "$$$")==0) InputState = 1;
	if(strcmp(s2, "$$$")==0) InputState = 0;
	
	if(InputState==2)
	{
		strcpy(gl_cLcdDispBuf[0],s1);
		strcpy(gl_cLcdDispBuf[1],s2);
		lcd_refresh();
		
	}
	else if(InputState == 1)
	{
		lcd_display(InputState, s2);	
	}
	else
	{
		lcd_display(InputState, s1);	
	}
		
		
}

//�ȴ���������
void WaitKeyBound(unsigned char cKeyValue)
{
	while (1)
	{
		//ReceiveData();				//��������
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
//�ȴ������˳�
void WaitKeyExit(unsigned char cKeyValue)
{
	unsigned char ch;
	
	while (1)
	{
		//ReceiveData();				//��������
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

//**************************************************
//���ܣ�	ͨ���Ƶ�������ʾ������Ϣ
//���������iErrorCode�������
//����ֵ����
//**************************************************
void ViewError(int iErrorCode)
{
	strcpy(gl_cLcdDispBuf[1]," ");
	switch (iErrorCode)
	{
		case ER_INITPORT:					//��ʼ�����ڳ���
			strcpy(gl_cLcdDispBuf[0],gl_cViewErrMenu[ER_INITPORT]);		//20141202 rwt
			break;
		case ER_FRAME:						//֡����
		case ER_PARITY:						//У���
			strcpy(gl_cLcdDispBuf[0],gl_cViewErrMenu[ER_FRAME]);		//20141202 rwt
			break;
		case ER_XOVER:						//X���ƶ�����
			strcpy(gl_cLcdDispBuf[0],gl_cViewErrMenu[ER_XOVER]);		//20141202 rwt
			break;
		case ER_YOVER:						//Y���ƶ�����
			strcpy(gl_cLcdDispBuf[0],gl_cViewErrMenu[ER_YOVER]);		//20141202 rwt
			break;
		case ER_YOBAD:						//Y��ԭ�㴫������
			strcpy(gl_cLcdDispBuf[0],gl_cViewErrMenu[ER_YOBAD]);		//20141202 rwt
			break;
		case ER_SRAM:
			strcpy(gl_cLcdDispBuf[0],gl_cViewErrMenu[ER_SRAM]);		//20141202 rwt
			break;
		case ER_POSITIONX:
			strcpy(gl_cLcdDispBuf[0],gl_cViewErrMenu[ER_POSITIONX]);		//20141202 rwt
			break;
		case ER_POSITIONY:
			strcpy(gl_cLcdDispBuf[0],gl_cViewErrMenu[ER_POSITIONY]);		//20141202 rwt
			break;
		case ER_NOPAPER:		
			strcpy(gl_cLcdDispBuf[0],gl_cViewErrMenu[ER_NOPAPER]); 	//20141202 rwt
			if (gl_cLanguageSelect == 0) {
				strcpy(gl_cLcdDispBuf[1],"7:�˳�");
			}
			else {
				strcpy(gl_cLcdDispBuf[1],"7:Exit");
			}
			//strcpy(gl_cLcdDispBuf[0],"ֽ������!");
			//strcpy(gl_cLcdDispBuf[1],"7:�˳�");
			lcd_refresh();
			WaitKeyExit(Key_Pause);
			return;
		case ER_COMMAND:
			strcpy(gl_cLcdDispBuf[0],gl_cViewErrMenu[ER_COMMAND]);		//20141202 rwt
			break;
		case ER_UINDERR:
			strcpy(gl_cLcdDispBuf[0],gl_cViewErrMenu[ER_UINDERR]);		//20141202 rwt
			break;
		case ER_DINDERR:
			strcpy(gl_cLcdDispBuf[0],gl_cViewErrMenu[ER_DINDERR]);		//20141202 rwt
			break;
		case ER_LINDERR:
			strcpy(gl_cLcdDispBuf[0],gl_cViewErrMenu[ER_LINDERR]);		//20141202 rwt
			break;
		case ER_RINDERR:
			strcpy(gl_cLcdDispBuf[0],gl_cViewErrMenu[ER_RINDERR]);		//20141202 rwt
			break;
		case ER_ZINDERR:
			strcpy(gl_cLcdDispBuf[0],gl_cViewErrMenu[ER_ZINDERR]);		//20141202 rwt
			break;
		case ER_CUTNUMSETERR:
			strcpy(gl_cLcdDispBuf[0],gl_cViewErrMenu[ER_CUTNUMSETERR]);		//20141202 rwt
			break;
		default:
			break;
	}
	lcd_refresh();
	while (1);
}


//�жϰ����Ƿ�һֱ��Ч  1��Ч 0��Ч
int KeyEffectState(unsigned char cKeyValue)
{
	Delay(160);

	if(gl_cKeyPE == gl_cKeyPH)
		return 1;
	
	if (gl_cKeyBuf[gl_cKeyPE] != cKeyValue)
	{//����µļ�ֵ��ԭ���Ĳ����
		//Printf("A PE[%d],PH[%d], [0x%x] [0x%x]\n", gl_cKeyPE, gl_cKeyPH, gl_cKeyBuf[gl_cKeyPE], cKeyValue);
		if ((gl_cKeyPE +1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		return 0;
	}

	//�¼�ֵ��ԭ�������
	//Printf("B PE[%d],PH[%d], [%c]\n", gl_cKeyPE, gl_cKeyPH, gl_cKeyBuf[gl_cKeyPE]);
	if ((gl_cKeyPE +1) == KeyBufLength)
		gl_cKeyPE = 0;
	else
		gl_cKeyPE++;
	return 1;
	
}

//*********20130807 �Ľ���ʾ����
void DelayNs(int count)
{
	while(count--);
}

//�жϰ������º��Ƿ��ɿ����� 0���� 1û��
int IsKeyBound(unsigned char cKeyValue)
{
	while(gl_cKeyPE == gl_cKeyPH);
	
	if (gl_cKeyBuf[gl_cKeyPE] != cKeyValue)
	{//����µļ�ֵ��ԭ���Ĳ����
		//Printf("A PE[%d],PH[%d], [%d]\n", gl_cKeyPE, gl_cKeyPH, gl_cKeyBuf[gl_cKeyPE]);
		if ((gl_cKeyPE +1) == KeyBufLength)
			gl_cKeyPE = 0;
		else
			gl_cKeyPE++;
		
		return 0;
	}

	//Delay(3000);
	
	
	//if(gl_cKeyPE == gl_cKeyPH)
	//	return 1;
	
	//�¼�ֵ��ԭ�������
	//Printf("B PE[%d],PH[%d], [%c]\n", gl_cKeyPE, gl_cKeyPH, gl_cKeyBuf[gl_cKeyPE]);
	if ((gl_cKeyPE +1) == KeyBufLength)
		gl_cKeyPE = 0;
	else
		gl_cKeyPE++;
	
	return 1;
}


//******************************

