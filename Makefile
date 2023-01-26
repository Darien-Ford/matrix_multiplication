all:	pipe 

pipe: matrix-multiplication

partB: matrix-multiplication.c
	cc matrix-multiplication.c -o matrix-multiplication
