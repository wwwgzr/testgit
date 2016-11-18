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
	//Param4字符结束标识(空字符)
	//searchCount符合条件的结果总数
	//indexList存储符合条件的配方数据索引
	//Index 显示的最小索引
	short newi = 0;
	if(index >=6)
	{
		newi = index - 6;
		index = newi;
	}
	if(newi > 0)
		page[0] = 1;
	else
		page[0] = 0;

	if(newi + 6 < searchCount)
		page[1] = 1;
	else
		page[1] = 0;

	short i = 0;
	while(i < 6)
	{
		if(i + newi < searchCount)
		{
			short j = 0;
			short cellI = indexList[i + newi];
			if(i + newi  >= 0 && i + newi  < searchCount)
			{
				while(j < 16)
				{
					if(cellI < 40)
						displayR[i * 17 + j] = Param2[cellI * 25 + j];
					else if(cellI >=40 && cellI < 80)
						displayR[i * 17 + j] = Param21[cellI * 25 + j];
					else if(cellI >=80 && cellI < 100)
						displayR[i * 17 + j] = Param22[cellI * 25 + j];
					j++;
				}
				if(cellI < 40)
					displayR[i * 17 + 16] = Param2[cellI * 25 + 24];
				else if(cellI >=40 && cellI < 80)
					displayR[i * 17 + 16] = Param21[cellI * 25 + 24];
				else if(cellI >=80 && cellI < 100)
					displayR[i * 17 + 16] = Param22[cellI * 25 + 24];
			}
		}
		else
			break;
		i++;
	}
	return 0;
}
 