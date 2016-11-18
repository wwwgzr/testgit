#include "macrotypedef.h"
#include "math.h"

/*
	Read,Write Local address function:
  	int ReadLocal( const char *type, int addr, int nRegs, void *buf, int flag  );
	int WriteLocal( const char *type, int addr, int nRegs, void *buf , int flag );

	Parameter:     type     is the string of "LW","LB" etc;
								address is the Operation address ;
 								nRegs    is the length of read or write ;
								buf        is the buffer which store the reading or writing data;
 								flag       is 0,then codetype is BIN,is 1  then codetype is BCD;
	return value : 1  ,Operation success
 								0,  Operation fail.

 	eg: read the value of local lw200 and write it to the lw202,with the codetype BIN,
		The code is :

    	short buf[2] = {0};
		ReadLocal("LW", 200, 2, (void*)buf, 0);
		WriteLocal("LW", 202, 2, (void*)buf, 0);
param1
//W212.00	手动上压布电磁阀
//W212.01	手动下压布电磁阀
//W212.02	手动擀布辊电磁阀
//W212.03	已接（空）
//W212.04	手动压布杆电磁阀

param2
//W212.07	手动分层电磁阀
//W212.08	手动压布电磁阀
//W212.09	手动压夹层电磁阀
//W212.10	手动压脚电磁阀
//W212.11	手动喷气嘴电磁阀
//W212.12	已接（空）
//W212.13	手动夹层松布气缸电磁阀
//W212.14	手动布料松布气缸电磁阀
	
param3
//W21.00	拉布JOG正
//W21.01	拉布JOG负
//W21.02	切布行走正
//W21.03	切布行走负
//W21.04	切布旋转正
//W21.05	切布旋转负

param4
//W203.05	夹层旋转正
//W203.06	夹层旋转负
//W203.07	布料旋转正
//W203.08	布料旋转负

param5
//W108.00	网带旋转正
param6
//D3004.01 判断是否需要恢复手动各地址 1需要恢复 0不需要
//D3004.02 按钮事件触发地址，当按下
*/
int MacroEntry()
{
	short i = 0;
	while(i < 5)
	{
		if(i != 3 && param1[i])
		{
			param6 = 1;
			return 0;
		}
		i++;
	}
	i = 0;
	while(i < 8)
	{
		if(i != 5 && param2[i])
		{
			param6 = 1;
			return 0;
		}
		i++;
	}
	i = 0;
	while(i < 2)
	{
		if(param3[i])
		{
			param6 = 1;
			return 0;
		}
		i++;
	}
	return 0;
}
 