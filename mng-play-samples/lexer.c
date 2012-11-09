
#line 1 "lexer.c.rl"
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


#line 39 "lexer.c.rl"



#line 22 "lexer.c"
static const char _mng_lexer_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 5, 1, 6, 1, 
	7, 1, 8, 1, 9, 1, 10, 1, 
	11, 1, 12, 1, 13, 1, 14, 1, 
	15, 2, 0, 14
};

static const char _mng_lexer_key_offsets[] = {
	0, 0, 2, 4, 5, 23, 26, 28, 
	29, 30
};

static const char _mng_lexer_trans_keys[] = {
	48, 57, 48, 57, 47, 10, 32, 40, 
	41, 44, 45, 47, 61, 123, 125, 9, 
	13, 48, 57, 65, 90, 97, 122, 46, 
	48, 57, 48, 57, 10, 10, 48, 57, 
	65, 90, 97, 122, 0
};

static const char _mng_lexer_single_lengths[] = {
	0, 0, 0, 1, 10, 1, 0, 1, 
	1, 0
};

static const char _mng_lexer_range_lengths[] = {
	0, 1, 1, 0, 4, 1, 1, 0, 
	0, 3
};

static const char _mng_lexer_index_offsets[] = {
	0, 0, 2, 4, 6, 21, 24, 26, 
	28, 30
};

static const char _mng_lexer_trans_targs[] = {
	5, 0, 6, 4, 7, 0, 4, 4, 
	4, 4, 4, 1, 3, 4, 4, 4, 
	4, 5, 9, 9, 0, 2, 5, 4, 
	6, 4, 4, 8, 4, 8, 9, 9, 
	9, 4, 4, 4, 4, 4, 4, 4, 
	0
};

static const char _mng_lexer_trans_actions[] = {
	7, 0, 0, 31, 0, 0, 21, 23, 
	9, 11, 17, 0, 0, 19, 13, 15, 
	23, 7, 0, 0, 0, 0, 7, 27, 
	0, 27, 33, 1, 29, 0, 0, 0, 
	0, 25, 31, 27, 27, 33, 29, 25, 
	0
};

static const char _mng_lexer_to_state_actions[] = {
	0, 0, 0, 0, 3, 0, 0, 0, 
	0, 0
};

static const char _mng_lexer_from_state_actions[] = {
	0, 0, 0, 0, 5, 0, 0, 0, 
	0, 0
};

static const char _mng_lexer_eof_trans[] = {
	0, 0, 35, 0, 0, 37, 37, 38, 
	39, 40
};

static const int mng_lexer_start = 4;
static const int mng_lexer_first_final = 4;
static const int mng_lexer_error = 0;

static const int mng_lexer_en_main = 4;


#line 42 "lexer.c.rl"

bool mng_lex(char *script, void(*cb)(char*,float,void*)) {	
	int cs, act, curline = 1;
	char *p = script,
		*pe = p + strlen(script),
		*eof = pe,
		*ts = 0,
		*te = 0;
		
	
#line 110 "lexer.c"
	{
	cs = mng_lexer_start;
	ts = 0;
	te = 0;
	act = 0;
	}

#line 52 "lexer.c.rl"
	
#line 120 "lexer.c"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_acts = _mng_lexer_actions + _mng_lexer_from_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 2:
#line 1 "NONE"
	{ts = p;}
	break;
#line 141 "lexer.c"
		}
	}

	_keys = _mng_lexer_trans_keys + _mng_lexer_key_offsets[cs];
	_trans = _mng_lexer_index_offsets[cs];

	_klen = _mng_lexer_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _mng_lexer_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
_eof_trans:
	cs = _mng_lexer_trans_targs[_trans];

	if ( _mng_lexer_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _mng_lexer_actions + _mng_lexer_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 17 "lexer.c.rl"
	{ ts = p; }
	break;
	case 3:
#line 1 "NONE"
	{te = p+1;}
	break;
	case 4:
#line 30 "lexer.c.rl"
	{te = p+1;{ cb("lparen", curline, NULL); }}
	break;
	case 5:
#line 31 "lexer.c.rl"
	{te = p+1;{ cb("rparen", curline, NULL); }}
	break;
	case 6:
#line 32 "lexer.c.rl"
	{te = p+1;{ cb("lbrace", curline, NULL); }}
	break;
	case 7:
#line 33 "lexer.c.rl"
	{te = p+1;{ cb("rbrace", curline, NULL); }}
	break;
	case 8:
#line 34 "lexer.c.rl"
	{te = p+1;{ cb("comma", curline, NULL); }}
	break;
	case 9:
#line 35 "lexer.c.rl"
	{te = p+1;{ cb("assignment", curline, NULL); }}
	break;
	case 10:
#line 36 "lexer.c.rl"
	{te = p+1;{ curline += 1; }}
	break;
	case 11:
#line 37 "lexer.c.rl"
	{te = p+1;}
	break;
	case 12:
#line 20 "lexer.c.rl"
	{te = p;p--;{
		cb("word", curline, astrncpy(ts, te-ts));
	}}
	break;
	case 13:
#line 23 "lexer.c.rl"
	{te = p;p--;{
		float n = atof(ts);
		cb("number", curline, &n);
	}}
	break;
	case 14:
#line 27 "lexer.c.rl"
	{te = p;p--;{
		cb("comment", curline, astrncpy(ts, te-ts));
	}}
	break;
	case 15:
#line 23 "lexer.c.rl"
	{{p = ((te))-1;}{
		float n = atof(ts);
		cb("number", curline, &n);
	}}
	break;
#line 272 "lexer.c"
		}
	}

_again:
	_acts = _mng_lexer_actions + _mng_lexer_to_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 1:
#line 1 "NONE"
	{ts = 0;}
	break;
#line 285 "lexer.c"
		}
	}

	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	if ( p == eof )
	{
	if ( _mng_lexer_eof_trans[cs] > 0 ) {
		_trans = _mng_lexer_eof_trans[cs] - 1;
		goto _eof_trans;
	}
	}

	_out: {}
	}

#line 53 "lexer.c.rl"
	
	return p == pe;
}