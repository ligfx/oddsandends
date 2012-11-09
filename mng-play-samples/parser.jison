%lex

%%
\s+ /* skip whitespace */
"//"[^\n]* /* skip comments */
[A-Za-z][A-Za-z0-9]* return 'word';
"-"?[0-9]+("."[0-9]+)?\b return 'number';
"(" return '(';
")" return ')';
"{" return '{';
"}" return '}';
"," return ',';
"=" return "=";
<<EOF>> return 'EOF';

/lex

%start main

%%

main: node* EOF { console.log(util.inspect($1, false, null)); } ;
node
	: word args { $$ = [$word, $args, []]; }
	| word args block { $$ = [$word, $args, $block]; }
	| word block { $$ = [$word, [], $block]; }
	| word '=' expression { $$ = ['Assign', $word, $expression]; }
	;
args: '(' (expression (',' expression)*)? ')' { $$ = $2; };
block: '{' node* '}' { $$ = $2; };
expression: word | number;

%%

var util = require('util');