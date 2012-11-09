CC=clang

all: main libmng.dylib

main: main.c
	$(CC) -framework OpenAL $< -o $@

libmng.dylib: main.c lexer.c
	clang $^ -framework OpenAL -shared -o $@

lexer.c: lexer.c.rl
	ragel $< -o $@
