/*
创建时间:2010-06-23
功		  能:管理系统菜单
修改时间:

*/

#ifndef __MENUMANAGE_H__
#define __MENUMANAGE_H__

extern void WaitKeyExit(unsigned char cKeyValue);
extern	int		WriteParameter(void);


void  KBProc(void);
//普通菜单
void KeyReOriginalPos(void); //20120905 按键回原点
void PessKeyRepetPrint(void); // 20120905 按键喷切
void PenUpDownDelay(void);  // 20120906 提落刀延时设置
void OverCutOffPaperSet(void); // 打印&切割结束，切断纸张 20120906
//void RunOverCutOffPaper(void);	// 打印切割结束切断纸 20120906
void  DispPumpAdsorbMenu(char state);//气泵吸附
void  PumpAdsorbManage(void);//气泵吸附
//void  PrintTestGraphManage(void);//打印测试图
void  OffsetDistanceManage(void);//偏距设置
void  CutSpeedManage(void);//切割速度
void  CutPressManage(void);//刀压设置
void  CutDistanceManage(void);//刀距设置
//void  SetOriginalPosManage(void);//设置原点
void  DeleteCurFile(void);//删除当前文件
//高级菜单
void  MacTypeSetManage(void);//机器类型设置
void  MacIpSetManage(unsigned short *sNetWorkValue);//网络设置
void  XYLengthReviseManage(void);//XY长度校正
void  LiftCutExtensionValueManage(void);// 1:提刀延伸值0:转角提刀延伸值
void  RP_EndReturnLowerRight(void);

#endif


