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
	currentIndex = selectedItem + rwIndex / 25 +1;
	rwIndex = selectedItem * 25 +  rwIndex;
	selectedItem = 0;

	//组数 
	SET =  R_SET;
	//层数
	REPEAT = R_REPEAT;
	//拉布速度
	SPEED = R_SPEED;
	//拉布长度
	LENGTH = R_LENGTH;
	//加速时间
	ACC = R_ACC;
	//减速时间
	DCC = R_DCC;
	//切割速度
	CUTSPEED = R_CUTSPEED;
	//方向 正转 反转
	if(R_CUTDIR == 1)
		CUTDIR = 1;
	else
		CUTDIR = 0;

	return 0;
}
 