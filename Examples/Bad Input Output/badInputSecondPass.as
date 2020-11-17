.entry LABEL0
.extern LABEL0


LABEL1:		add	r3 , LABEL5
		jsr	LABEL0
LABEL2:		prn	                       #2048
		lea	LABEL4,r6
		inc	r6
		mov	*r6,LABEL999
		sub	r1,r4



;		compering betwin max legal number to illegal number
		cmp	#-2048,#-2049
		bne	LABEL3
		add	r7,*r6
		clr	LABEL6
		sub	LABEL7,LABEL7
.entry LABEL2
		jmp	LABEL2
LABEL3:		stop
LABEL4:		.string "abcd"
LABEL5:		.data	16383,-16384,-16385
		.data	34579384725983457342594328575
LABEL6:		.data	31
.extern LABEL600
