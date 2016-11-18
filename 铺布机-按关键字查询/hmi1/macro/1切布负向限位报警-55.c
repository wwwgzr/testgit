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
	index++;
	if(index <= 0)
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
	time1[index * 7 - 1] = 55;

	return 0;
}
 