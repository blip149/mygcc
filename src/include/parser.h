#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "parse_helper.h"
#include "cst.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    Lexer* lex;
    Token current;
    Token next;
    bool has_error;
}Parser;

void parser_init(Parser* parser, Lexer* lex);
void advance_parse(Parser* parser);
bool check_parser(Parser* parser, TokenType type);
CstNode* parse_translation_unit(Parser* p);
CstNode* parse_external_declaration(Parser* p);
CstNode* parse_declaration_specifiers(Parser* p);
CstNode* parse_declarator(Parser* p);
CstNode* parse_compound_statement(Parser* p);
CstNode* parse_function_definition(Parser* p);
CstNode* parse_direct_declarator(Parser* p);
CstNode* parse_statement(Parser* p);
CstNode* parse_expression_statement(Parser* p);
CstNode* parse_if_statement(Parser* p);
CstNode* parse_while_statement(Parser* p);
CstNode* parse_return_statement(Parser* p);
CstNode* parse_expression(Parser* p);
CstNode* parse_declaration(Parser* p);
CstNode* parse_for_statement(Parser* p);
CstNode* parse_do_while_statement(Parser* p);
CstNode* parse_jump_statement(Parser* p);
CstNode* parse_nud(Parser* p);
CstNode* parse_led(Parser* p, Token op, CstNode* left);
CstNode* parse_expr(Parser* p, int min_bp);

#endif