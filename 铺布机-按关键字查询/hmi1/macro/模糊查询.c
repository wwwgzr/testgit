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
	//Param1输入的关键字
	//Param2需要做匹配的字符
	//Param3标识位 0匹配失败 1匹配成功
	//Param4字符结束标识

	Param3 = 0;
	short i = 0;
	short k = -1;
	short f = 0;
	while(i < 16)
	{
		if(Param4 == Param1[i])
			break;
		short j = 0;
		if(k > 0)
			j = k;
		while(j < 16)
		{
			if(Param4 == Param2[j])
			{
				Param3 = 0;
				break;
			}
			if(j > 0 && f == 1 && Param1[i] != Param2[j])
			{
				Param3 = 0;
				return 0;
			}
			if(Param1[i] == Param2[j])
			{
				Param3 = 1;
				f = 1;
				k = j + 1;
				break;
			}
			j++;
		}
		
		if(f == 1 && j >= 16)
		{
			Param3 = 0;
			break;
		}
		i++;
	}
	return 0;
}
 