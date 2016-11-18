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
	short d = 0;
	short searchWordLen = 0;
	while(searchWordLen < 16)
	{
		if(Param1[searchWordLen] == Param4)
			break;
		searchWordLen++;
	}

	if(searchWordLen <= 0 || searchWordLen > 16)
		return 0;
	searchCount = 0;
	short i = 0;
	while(i < 40)
	{
		short j = 0;
		short f = 0;
		d = 0;
		while(j < 16)
		{
			if(Param2[i * 25 + j] == Param4)
				break;
			if(Param2[i * 25 + j] == Param1[0])
			{
				short k = 0;
				while(k < searchWordLen && j < 16)
				{
					if(Param2[i * 25 + j] == Param1[k])
					{
						d = 1;
						j++;
						k++;
					}
					else
					{
						d = 0;
						break;
					}
				}
				if(k == searchWordLen  && j <= 16)
				{
					indexList[searchCount] = Param2[i * 25 + 24];
					if(searchCount < 6)
					{
						short m = 0;
						while(m < 16)
						{
							displayR[searchCount * 17 + m] = Param2[i * 25 + m];
							m++;
						}
						displayR[searchCount * 17 + 16] = Param2[i * 25 + 24];
					}
					searchCount++;
					d = 5;
				}
				break;
			}
			j++;
		}
		i++;
	}
//第二段数据
	i = 0;
	while(i < 40)
	{
		short j = 0;
		short f = 0;
		d = 0;
		while(j < 16)
		{
			if(Param21[i * 25 + j] == Param4)
				break;
			if(Param21[i * 25 + j] == Param1[0])
			{
				short k = 0;
				while(k < searchWordLen && j < 16)
				{
					if(Param21[i * 25 + j] == Param1[k])
					{
						d = 1;
						j++;
						k++;
					}
					else
					{
						d = 0;
						break;
					}
				}
				if(k == searchWordLen  && j <= 16)
				{
					indexList[searchCount] = Param21[i * 25 + 24];
					if(searchCount < 6)
					{
						short m = 0;
						while(m < 16)
						{
							displayR[searchCount * 17 + m] = Param21[i * 25 + m];
							m++;
						}
						displayR[searchCount * 17 + 16] = Param21[i * 25 + 24];
					}
					searchCount++;
					d = 5;
				}
				break;
			}
			j++;
		}
		i++;
	}
//第三段数据
	i = 0;
	while(i < 40)
	{
		short j = 0;
		short f = 0;
		d = 0;
		while(j < 16)
		{
			if(Param22[i * 25 + j] == Param4)
				break;
			if(Param22[i * 25 + j] == Param1[0])
			{
				short k = 0;
				while(k < searchWordLen && j < 16)
				{
					if(Param22[i * 25 + j] == Param1[k])
					{
						d = 1;
						j++;
						k++;
					}
					else
					{
						d = 0;
						break;
					}
				}
				if(k == searchWordLen  && j <= 16)
				{
					indexList[searchCount] = Param22[i * 25 + 24];
					if(searchCount < 6)
					{
						short m = 0;
						while(m < 16)
						{
							displayR[searchCount * 17 + m] = Param22[i * 25 + m];
							m++;
						}
						displayR[searchCount * 17 + 16] = Param22[i * 25 + 24];
					}
					searchCount++;
					d = 5;
				}
				break;
			}
			j++;
		}
		i++;
	}
	index = 0;
	page[0] = 0;
	if(searchCount > 0)
		page[1] = 1;
	else
		page[1] = 0;
	return 0;
}
 