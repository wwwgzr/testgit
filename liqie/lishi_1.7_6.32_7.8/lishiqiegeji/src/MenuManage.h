/*
����ʱ��:2010-06-23
��		  ��:����ϵͳ�˵�
�޸�ʱ��:

*/

#ifndef __MENUMANAGE_H__
#define __MENUMANAGE_H__

extern void WaitKeyExit(unsigned char cKeyValue);
extern	int		WriteParameter(void);


void  KBProc(void);
//��ͨ�˵�
void KeyReOriginalPos(void); //20120905 ������ԭ��
void PessKeyRepetPrint(void); // 20120905 ��������
void PenUpDownDelay(void);  // 20120906 ���䵶��ʱ����
void OverCutOffPaperSet(void); // ��ӡ&�и�������ж�ֽ�� 20120906
//void RunOverCutOffPaper(void);	// ��ӡ�и�����ж�ֽ 20120906
void  DispPumpAdsorbMenu(char state);//��������
void  PumpAdsorbManage(void);//��������
//void  PrintTestGraphManage(void);//��ӡ����ͼ
void  OffsetDistanceManage(void);//ƫ������
void  CutSpeedManage(void);//�и��ٶ�
void  CutPressManage(void);//��ѹ����
void  CutDistanceManage(void);//��������
//void  SetOriginalPosManage(void);//����ԭ��
void  DeleteCurFile(void);//ɾ����ǰ�ļ�
//�߼��˵�
void  MacTypeSetManage(void);//������������
void  MacIpSetManage(unsigned short *sNetWorkValue);//��������
void  XYLengthReviseManage(void);//XY����У��
void  LiftCutExtensionValueManage(void);// 1:�ᵶ����ֵ0:ת���ᵶ����ֵ
void  RP_EndReturnLowerRight(void);

#endif


