#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* astrncpy(const char *src, size_t n) {
	char *s = malloc(n + 1);
	s[n] = '\0';
	memcpy(s, src, n);
	return s;
}

%%{
	machine mng_lexer;
	
	start_token = null % { ts = p; };
	
	main := |*
	alpha alnum* {
		cb("word", curline, astrncpy(ts, te-ts));
	};
	'-'? digit+ ('.' digit+)? {
		float n = atof(ts);
		cb("number", curline, &n);
	};
	'//' start_token ([^\n]*) {
		cb("comment", curline, astrncpy(ts, te-ts));
	};
	'(' { cb("lparen", curline, NULL); };
	')' { cb("rparen", curline, NULL); };
	'{' { cb("lbrace", curline, NULL); };
	'}' { cb("rbrace", curline, NULL); };
	',' { cb("comma", curline, NULL); };
	'=' { cb("assignment", curline, NULL); };
	'\n' { curline += 1; };
	space;
	*|;
}%%

%% write data;

bool mng_lex(char *script, void(*cb)(char*,float,void*)) {	
	int cs, act, curline = 1;
	char *p = script,
		*pe = p + strlen(script),
		*eof = pe,
		*ts = 0,
		*te = 0;
		
	%% write init;
	%% write exec;	
	
	return p == pe;
}