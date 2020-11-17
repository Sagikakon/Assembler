LABEL4:		.string "abc  asdhkjashdaskj         asdsdaad"
.extern LABEL0
		lea	LABEL4,r6
		jsr	LABEL0
		cmp	r3,#-259
.entry LABEL5
		cmp	LABEL2,LABEL7
		sub	r1,r4
		bne	LABEL3
		sub	LABEL7,LABEL7
LABEL1:		add	r3,LABEL5
		mov	*r6,LABEL7
LABEL5:		.data	780,-0,10000,-1234,                901, 283,14,901   ,  601
		add	r7,*r6
		.data	-887
		inc	r6
LABEL2:		prn	#29
		clr	LABEL6
.entry LABEL2
LABEL6:		.data	3131
		jmp	LABEL2
LABEL3:		stop
.extern LABEL7