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
	//组数 16
	SET = 0;
	//层数 17
	REPEAT = 0;
	//拉布速度 19
	SPEED = 0;
	//拉布长度 18
	LENGTH = 0;
	//加速时间 20
	ACC = 0;
	//减速时间 21
	DCC = 0;
	//切割速度 23
	CUTSPEED = 0;
	//方向 正转 反转 22
	CUTDIR = 0;
	CUTDIR_1 = 0;
	//SYSINDEX = (OLDINDEX - 1) * 25;
	selectIndex = 0;
	return 0;
}
 