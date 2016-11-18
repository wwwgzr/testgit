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
	//夹层旋转低速
	//if(speedflag[0])
	{
		plc_speed[0] = speed[0] * 4;
	}
	//夹层旋转中低速
	//if(speedflag[1])
	{
		plc_speed[2] = speed[2] * 4;
	}

	//夹层旋转中高速
	//if(speedflag[2])
	{
		plc_speed[4] = speed[4] * 4;
	}

	//夹层旋转高速
	//if(speedflag[3])
	{
		plc_speed[6] = speed[6] * 4;
	}

	//夹层旋转最高速
	//if(speedflag[4])
	{
		plc_speed[8] = speed[8] * 4;
	}

	//布料旋转低速
	//if(speedflag[5])
	{
		plc_speed[10] = speed[10] * 4;
	}

	//布料旋转中低速
	//if(speedflag[6])
	{
		plc_speed[12] = speed[12] * 4;
	}

	//布料旋转中高速
	//if(speedflag[7])
	{
		plc_speed[14] = speed[14] * 4;
	}

	//布料旋转高速
	//if(speedflag[8])
	{
		plc_speed[16] = speed[16] * 4;
	}

	//布料旋转最高速
	//if(speedflag[9])
	{
		plc_speed[18] = speed[18] * 4;
	}

	return 0;
}
 