mod mnglex {
	enum lexstate {
		START,
		WORD,
		NUMBER,
		COMMENT,
		COMMENTSTART
	}
	type lexmachine = { mut data: [char], mut state: lexstate };
	impl extensions for lexmachine {
		fn take(c: char, s: lexstate) {
			self.data = [c];
			self.state = s;
		}
		fn take_while(c: char, push: fn(token), condition: fn(char)->bool, final: fn(str)) {
			if condition(c) {
				self.data += [c];
			} else {
				final(str::from_chars(self.data));
				self.data = [];
				self.state = START;
				self.start(c, push);
			}
		}
		fn start(c: char, push: fn(token)) {
			alt c {
				c if char::is_whitespace(c) { }
				'='                         { push(equals) }
				','                         { push(comma) }
				'{'                         { push(lbrace) }
				'}'                         { push(rbrace) }
				'('                         { push(lparen) }
				')'                         { push(rparen) }
				c if char::is_alphabetic(c) { self.take(c, WORD) }
				'-'                         { self.take(c, NUMBER) }
				c if char::is_digit(c)      { self.take(c, NUMBER) }
				'/'                         { self.state = COMMENTSTART }
				c                           { push(err(#fmt("%c", c))) }
			}
		}
		fn accept(c: char, push: fn(token)) {
			alt self.state {
				START { self.start(c, push) }
				WORD  {
					self.take_while(c, push,
					char::is_alphanumeric,
					{ |s| push(word(s)) })
				}
				NUMBER {
					self.take_while(c, push,
					{ |c| char::is_digit(c) || c == '.' },
					{ |s| push(number(option::get(float::from_str(s)))) })
				}
				COMMENT {
					self.take_while(c, push,
					{ |c| c != '\n' },
					{ |s| push(comment(s)) })
				}
				COMMENTSTART {
					if c != '/' { fail }
					self.state = COMMENT;
				}
				_ { push(err(#fmt("Unknown state %i", self.state as int))) }
			}
		}
	}

	fn lexer() -> lexmachine {
		ret { mut data: [], mut state: START };
	}

	enum token {
		equals,
		comma,
		lbrace,
		rbrace,
		lparen,
		rparen,
		comment(str),
		number(float),
		word(str),
		err(str),
		eoi
	}

	impl extensions for token {
		fn inspect() -> str {
			ret alt self {
				equals     { "equals" }
				comma      { "comma" }
				lbrace     { "lbrace" }
				rbrace     { "rbrace" }
				lparen     { "lparen" }
				rparen     { "rparen" }
				comment(s) { #fmt("Comment: %s", s) }
				number(n)  { #fmt("Number: %f", n) }
				word(s)    { #fmt("Word: %s", s) }
				err(c)     { #fmt("Error: %s", c) }
				eoi        { "eoi" }

				_          { fail }
			}
		}
	}

	fn lex(data: [char]) -> [token] {
		let l = lexer();
		let mut tokens = [];
		vec::iter(data, { |c|
			l.accept(c, { |t|
				tokens += [t]
			})
		});
		ret tokens;
	}
}

mod file {
	fn read_chars(filename: str) -> [char] {
		ret str::chars(result::get(io::read_whole_file_str(filename)));
	}
}

mod mngparse {
	import mnglex::token;

	type node = {
		c: char
	};

	type parsestream = {
		tokens: [token]
	};

	fn parser(tokens: [token]) -> parsestream {
		ret {tokens: tokens};
	}
	impl extensions for parsestream {
		fn is_done() -> bool {
			ret true;
		}
		fn parse_function(tokens: [token]) -> node {
			ret { c: 'h'}
			alt tokens[0] {
				word(name) {
					tokens.shift();
					ret parse_function_core(tokens, name);
				}
				_ { fail }
			}
		}
	}

	fn parse(tokens: [token]) -> [node] {
		let p = parser(tokens);
		let mut nodes = [];
		while (!p.is_done()) {
			nodes += [p.parse_function()];
		}
		ret nodes;
	}
}

fn main(args: [str]) unsafe {
	import al::extensions;
	import mnglex::extensions;

	let device = al::open_device();
	let context = device.create_context();

	let data = file::read_chars(args[1]);
	let tokens = mnglex::lex(data);

	vec::iter(tokens, { |t|
		io::println(t.inspect());
	});

	fail;

	let ast = mngparse::parse(tokens);

	context.destroy();
	device.close();
}
