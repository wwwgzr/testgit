;**********OPTIONS*******************************
;_RAM_STARTADDRESS  EQU   0xc000000
_ISR_STARTADDRESS   EQU   0xdfff000 ;GCS6:32MB SDRAM for SanJiang DVK
_IRQ_BASEADDRESS   EQU   0xc000000 ;GCS6:32MB SDRAM for SanJiang DVK

;BUSWIDTH; 16,32
                GBLA    BUSWIDTH
BUSWIDTH	SETA    16


;"DRAM","SDRAM"
                GBLS    BDRAMTYPE
BDRAMTYPE	SETS    "SDRAM"


;This value has to be TRUE on ROM program.
;This value has to be FALSE in RAM program.
;Set PLLONSTART always TRUE for test by Eric Rong(2002-3-19-12-51)
                GBLL    PLLONSTART
PLLONSTART      SETL    {TRUE}
;PLLONSTART      SETL    {FALSE}
	GBLA	PLLCLK
PLLCLK		SETA	64000000
	[	PLLCLK = 64000000	
M_DIV	EQU	56	;Fin=8MHz Fout=((56+8)/8)*8=64MHz
P_DIV	EQU	4;2
S_DIV	EQU	1
	]
;************************************************
;NOTE:If you change this file, you should sync option.a (Eric Rong 2002-3-15)
	END
