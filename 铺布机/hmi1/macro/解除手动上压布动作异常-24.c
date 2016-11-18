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
	short i = 1;
	if(index == 0)
		index = 1;
	if(index >= 100)
	{
		index = 1;
	}

	while(i < index)
	{
		time1[i * 7] = time1[(i - 1) * 7];
		i++;
	}
	i = 0;
	while(i < 6)
	{
		time1[index * 7 - i - 2] = systime[i];
		i++;
	}
	//������ 3
	time1[index * 7 - 1] = 24;

	i = 0;

	while(i < 10)
	{
		if(index < i)
			break;
		uitime[i * 7] = time1[(index - i) * 7  - 7];
		uitime[i * 7 + 1] = time1[(index - i) * 7  - 6];
		uitime[i * 7 + 2] = time1[(index - i) * 7  - 5];
		uitime[i * 7 + 3] = time1[(index - i) * 7  - 4];
		uitime[i * 7 + 4] = time1[(index - i) * 7  - 3];
		uitime[i * 7 + 5] = time1[(index - i) * 7  - 2];
		uitime[i * 7 + 6] = time1[(index - i) * 7  - 1];
		short m = 0;
		while(m < 840)
		{
			if(time1[(index - i) * 7  - 1]== alertInfo[m])
			{
				short j = 0;
				while(j < 14)
				{
					uiInfo[i * 14 + j] = alertInfo[m + j + 1];
					j++;
				}
				break;
			}
			m = m + 15;
		}
		i++;
	}

	index++;

	return 0;
}
 