grammar ATalk;

@members{
  void print(String str){
    System.out.println(str);
  }
}

fragment LOWERCASE: [a-z];
fragment UPPERCASE: [A-Z];
fragment NATURALDIGIT: [1-9];
fragment ARITHDIGIT: [0-9];
fragment ARITHOPERANDS: [+-/*];

program: ((NEWLINE)|(actor))+;

actor: ACTOR actor_name=ID {print("ACTOR: " + $actor_name.text);}
  LT NATURALNUM GT NEWLINE (actor_block) NEWLINE? END (NEWLINE | EOF)
;

actor_block : ((reciever | (initialization_global NEWLINE) | (assignment NEWLINE) | NEWLINE)* );

reciever: RECEIVER reciever_name=ID {print("RECIEVER: " + $reciever_name.text);}
  LPAR (defenition_arguments)? RPAR NEWLINE reciever_block END NEWLINE
;

reciever_block: (statement | NEWLINE);

statement: ((function_call | assignment | assignment_multi | initialization_local | {print("operation");} operation |  if_statement | foreach_statement |
              send | begin_end_block | QUIT | BREAK) NEWLINE)*;

begin_end_block: BEGIN {print("begin/end block");} NEWLINE (statement | NEWLIÇà=NE?) END;

defenition_arguments: (type ID (',' type ID)* );

call_arguments: ((ID | STRING | CHARACTER | ARITHNUM | operation) (',' (ID | STRING | CHARACTER | ARITHNUM | operation))*);

initialization_global: type var_name=ID {print("GLOBAL VAR DEC: "+ $var_name.text);} (ASSIGN rvalue)?
    (',' var_name=ID {print("GLOBAL VAR DEC: "+ $var_name.text);} (ASSIGN rvalue)?)*
;

initialization_local: type var_name=ID {print("LOCAL VAR DEC: "+ $var_name.text);} (ASSIGN rvalue)?
    (',' var_name=ID {print("LOCAL VAR DEC: "+ $var_name.text);} (ASSIGN rvalue)?)*
;

rvalue: (ID | STRING | CHARACTER | ARITHNUM | NATURALNUM | assignment |  operation | array_init);

assignment: (ID ASSIGN {print("assignment");} (read_function | operation | array_init | ID | STRING | CHARACTER | ARITHNUM | assignment));

assignment_multi: assignment (',' assignment)*;

function_call: (read_function NEWLINE) | (write_function) |
  (func_name=ID {print("function call: " + $func_name.text);} LPAR (call_arguments)? RPAR);Çà=

read_function: READ {print("function call: read");} LPAR (ARITHNUM | NATURALNUM) RPAR;

write_function: WRITE  {print("function call: write");} LPAR ( (ID(LBRAC ( operation | ID | ARITHNUM | NATURALNUM) RBRAC)*)
    | STRING | CHARACTER |  operation ) RPAR;

send: (SENDER | ID | SELF) SENDOP {print("sender");} ID LPAR (call_arguments)? RPAR;

type : (INT | CHAR) (LBRAC NATURALNUM RBRAC)*;

operation: or_op ;
or_op: and_op | (and_op OR {print("'or' operator");} or_op);
and_op: (equality_op AND {print("'and' operator");} and_op) | equality_op;
equality_op: (comparison_op (EQUAL {print("'==' operator");} | NOTEQUAL {print("'<>' operator");}) equality_op) | comparison_op;
comparison_op: ((add_op (GT {print("'>' operator");} | LT {print("'<' operator");}) comparison_op)) | add_op;
add_op: (mult_op (ADD {print("'+' operator");} | SUB {print("'-' operator");}) add_op) | mult_op;
mult_op: (single_operator_op (MULT {print("'*' operator");} | DIV {print("'/' operator");}) mult_op) | single_operator_opÇà=;
single_operator_op: (operands (NOT {print("'not' operator");}| SUB {print("'-' operator");}) single_operator_op) | operands;
operands: (((LPAR (assignment | operation) RPAR) | (ID (LBRAC (operation | ID | STRING | CHARACTER | ARITHNUM | NATURALNUM) RBRAC)*)
  | ARITHNUM | STRING | CHARACTER | NATURALNUM | read_function) operands?);

if_statement: (IF {print("if statement");} ( operation | ARITHNUM) NEWLINE condition_block)
  (ELSEIF ( operation | ARITHNUM) NEWLINE condition_block)*
  (ELSE NEWLINE condition_block)? END
;

condition_block: (statement);

foreach_statement: FOREACH {print("foreach statement");} ID IN (ID(LBRAC  operation RBRAC)* | array_init) NEWLINE (foreach_block) END;

foreach_block: (statement);

array_init: '{' (value (',' value)*) '}';

value: (array_init |  operation | read_function | ID | STRING | CHARACTER | NATURALNUM | ARITHNUM);

/*
Lexer
*/

CM: ('#' (~[\n])*) -> skip;

NATURALNUM: NATURALDIGIT ARITHDIGIT*;
ARITHNUM: ('0' | (NATURALDIGIT ARITHDIGIT*));
Çà=
ACTOR: 'actor';

RECEIVER: 'receiver';
SENDER: 'sender';

SELF: 'self';

READ: 'read';
WRITE: 'write';

IF: 'if';
ELSEIF: 'elseif';
ELSE: 'else';

FOREACH: 'foreach';
IN: 'in';

BEGIN: 'begin';
END: 'end';

BREAK: 'break';
QUIT: 'quit';

INT: 'int';
CHAR: 'char';

EQUAL: '==';
NOTEQUAL: '<>';
LT: '<';
GT: '>';
OR: 'or';
AND: 'and';
NOT: 'not';
LPAR: '(';
RPAR: ')';
ADD: '+';
SUB: '-';
MULT: '*';
DIV: '/';
ASSIGN:'=';
LBRAC: '[';
RBRAC: ']';
NEWLINE: ('\n')+;
SENDOP: '<<';

WS: [ \t\r] -> skip;

ID: ('_' | LOWERCASE | UPPERCASE)('_' | LOWERCASE | UPPERCASE | ARITHDIGIT)*;

STRING: '"' .*? '"';
CHARACTER: ['] .? ['];
end of file.