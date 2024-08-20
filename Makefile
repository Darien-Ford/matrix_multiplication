all: pipe 

pipe: matrix-multiplication

matrix-multiplication: matrix-multiplication.c
	cc matrix-multiplication.c -o matrix-multiplication
