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
	currentIndexW = selectedItem + rwIndex / 25 + 1;
	rwIndexW = selectedItem * 25 +  rwIndex;
   oldIndex = currentIndex;
	selectedItemW = 0;
	//组数
	SET = 0;
	//层数
	REPEAT = 0;
	//拉布速度
	SPEED = 0;
	//拉布长度
	LENGTH = 0;
	//加速时间
	ACC = 0;
	//减速时间
	DCC = 0;
	//切割速度
	CUTSPEED = 0;
	//方向 正转 反转
	CUTDIR = 0;
	changeFlag = 1;
	return 0;
}
 