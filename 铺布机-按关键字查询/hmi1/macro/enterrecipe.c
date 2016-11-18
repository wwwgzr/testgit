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
*/
int MacroEntry()
{
   //char str[]={"0123456789ABCDE"};
	//short buf[2] = {0};
	//WriteLocal("D", 0, 32, (char*) str, 0);
	//组数
	PLCSET = SET;
	//层数
	PLCREPEAT = REPEAT;
	//拉布速度
	PLCSPEED = SPEED;
	//拉布长度
	PLCLENGTH = LENGTH;
	//加速时间
	PLCACC = ACC;
	//减速时间
	PLCDCC = DCC;
	//切割速度
	PLCCUTSPEED = CUTSPEED;
	//方向 正转 反转
	//PLCCUTDIR = CUTDIR;

	allSet = SET;
	allRepeat = REPEAT;
	return 0;
}
 