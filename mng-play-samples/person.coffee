FFI = require 'node-ffi'
argv = process.argv.splice 2
_ = require 'underscore'

mng = (->
	lib = new FFI.Library "libmng"
		mng_file_new_from_filename: ["pointer", ["string"]]
		mng_file_destroy: ["void", ["pointer"]]
		mng_file_get_script: ["string", ["pointer"]]
		mng_lex: ["int", ["string", "pointer"]]
	stdlib = new FFI.Library null
		free: ["void", ["pointer"]]

	return {
		File: (filename) ->
			pointer = lib.mng_file_new_from_filename filename
			@destroy = () ->  lib.mng_file_destroy pointer
			@__defineGetter__ 'script', () -> return lib.mng_file_get_script pointer
			
		lex: (script, cb) ->
			_cb = new FFI.Callback ["void", ["string", "float", "pointer"]], (type, lineno, ptr) ->
				data = null
				if type == "word" or type == "comment"
					data = ptr.getCString()
					stdlib.free ptr
				else if type == "number"
					data = ptr.getFloat()
				
				cb type, lineno, data
			success = lib.mng_lex script, _cb.getPointer()
			return success != 0
			
		lex_reduce: (script) ->
			tokens = []
			success = @lex script, (type, lineno, data) ->
				tokens.push [type, lineno, data]
			if success
				return tokens
			else
				return null
	}
)()

if argv.length == 0
	console.log "Usage: #{process.argv[0]} #{process.argv[1]} FILE"
	process.exit -1
m = new mng.File argv[0]
m.destroy()

grammar =
	lex:
		rules: [
			["\\s+", "/* skip whitespace */"]
			["\\/\\/[^\\n]*", "/* skip comments */"]
			["[A-Za-z][A-Za-z0-9]*", "return 'word';"]
			["-?[0-9]+(\\.[0-9]+)?\\b", "return 'number';"]
			["\\(", "return '(';"]
			["\\)", "return ')';"]
			["\\{", "return '{';"]
			["\\}", "return '}';"]
			[",", "return ',';"]
			["=", "return \"=\";"]
			["$", "return 'EOF';"]
		]
	start: "main"
	ebnf:
		main: [["node* EOF", "return $1"]]
		node: [
			["word args", "$$ = [$word, $args, [], @word.first_line];"]
			["word args block", "$$ = [$word, $args, $block, @word.first_line];"]
			["word block", "$$ = [$word, [], $block, @word.first_line];"]
			["word '=' expression", "$$ = ['Assign', [$word, $expression], [], @word.first_line]"]
		]
		args: [["'(' (expression ',')* expression ')'", "$2.push($3); $$ = $2"]]
		block: [["'{' node* '}'", "$$ = $2"]]
		expression: ["word", "number", "node"]

EBNF = require "./ebenezer"
console.log(EBNF)
grammar.bnf = EBNF.transform grammar.ebnf
console.log grammar.bnf

Parser = require('jison').Parser
parser = new Parser grammar
fs = require('fs')
util = require('util')
ast = parser.parse fs.readFileSync "test.mngscript", "ascii"
console.log util.inspect ast, false, null

variables = {}
tracks = {}
effects = {}
ast.forEach (node) ->
	switch node[0]
		when "Variable"
			variables[node[1][0]] = number(node[1][1])
		when "Track"
			tracks[node[1][0]] = node[2]
		when "Effect"
			effects[node[1][0]] = node[2]
		else return
console.log variables
console.log tracks
console.log effects