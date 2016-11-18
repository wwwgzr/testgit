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
	Param = 0;
	Param_1=0;
	Param_2=0;
	Param_3=0;
	Param_4=0;
	Param_5=0;
	Param_6=0;
	Param_7=0;
	Param_8=0;
	Param_9=0;
	Param_10=0;
	Param_11=0;
	Param_12=0;
	Param_13=0;
	Param_14=0;
	Param_15=0;
	Param_16=0;
	Param_17=0;
	Param_18=0;
	Param_19=0;
	Param_20=0;
	Param_21=0;
	Param_22=0;
	Param_23=0;
	Param_24=0;
	Param1=0;
	Param1_1=0;
	Param1_2=0;
	Param1_3=0;
	Param1_4=0;
	Param1_5=0;
	Param1_6=0;
	Param1_7=0;
	Param1_8=0;
	Param1_9=0;
	Param1_10=0;
	Param1_11=0;
	Param1_12=0;
	Param1_13=0;
	Param1_14=0;
	Param1_15=0;
	Param1_16=0;
	Param1_17=0;
	Param1_18=0;
	Param1_19=0;
	Param1_20=0;
	Param1_21=0;
	Param1_22=0;
	Param1_23=0;
	Param1_24=0;
	Param2=0;
	Param2_1=0;
	Param2_2=0;
	Param2_3=0;
	Param2_4=0;
	Param2_5=0;
	Param2_6=0;
	Param2_7=0;
	Param2_8=0;
	Param2_9=0;
	Param2_10=0;
	Param2_11=0;
	Param2_12=0;
	Param2_13=0;
	Param2_14=0;
	Param2_15=0;
	Param2_16=0;
	Param2_17=0;
	Param2_18=0;
	Param2_19=0;
	Param2_20=0;
	Param2_21=0;
	Param2_22=0;
	Param2_23=0;
	Param2_24=0;
	Param3=0;
	Param3_1=0;
	Param3_2=0;
	Param3_3=0;
	Param3_4=0;
	Param3_5=0;
	Param3_6=0;
	Param3_7=0;
	Param3_8=0;
	Param3_9=0;
	Param3_10=0;
	Param3_11=0;
	Param3_12=0;
	Param3_13=0;
	Param3_14=0;
	Param3_15=0;
	Param3_16=0;
	Param3_17=0;
	Param3_18=0;
	Param3_19=0;
	Param3_20=0;
	Param3_21=0;
	Param3_22=0;
	Param3_23=0;
	Param3_24=0;
	return 0;
}
 