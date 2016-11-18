NoInt		EQU	0x80

	AREA |C$$code|, CODE, READONLY

	EXPORT	ChangeMemCon
;*************************************************************************
;*	The function for changing the Bank control and refresh registers) *
;**************************************************************************
;void ChangeMemCon();
ChangeMemCon
    stmfd   sp!,{r4-r9}	    ;Assembler uses the high registers(r4~). 
	   
    ldmia   r0,{r1-r9}
    ldr	    r0,=0x01c80004  ;BANKCON0 Address
    stmia   r0,{r1-r9}

    ldmfd   sp!,{r4-r9}

    mov	    pc,lr
    
;        EXPORT	_disable_
;_disable_
;	STMDB	sp!, {r0}
;	MRS		r0, CPSR
;	ORR		r0, r0, #NoInt
;	MSR		CPSR_cxsf, r0
;	LDMIA	sp!, {r0}
;	MOV	pc, lr


;	EXPORT	_enable_
;_enable_
;	STMDB	sp!, {r0}
;	MRS	r0, CPSR
;	BIC	r0, r0, #NoInt
;	MSR	CPSR_cxsf, r0
;	LDMIA	sp!, {r0}
;	MOV	pc, lr
	
	END
