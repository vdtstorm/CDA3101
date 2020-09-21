	lw	0 1 five
bool	lw	5 2 neg1
	add	1 2 1
	beq	0 1 2
	beq	0 0 bool
	noop
done	halt
five	.fill	5
neg1	.fill	-1
stAddr	.fill	bool
kill	.fill	900
