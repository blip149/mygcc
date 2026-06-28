#include "include/parser.h"
#include "include/pratt.h"


void parser_init(Parser* parser, Lexer* lex) {
    parser->lex = lex;
    parser->has_error = false;

    next_token(lex);
    parser->current = lex->current_token;

    next_token(lex);
    parser->next = lex->current_token;
}

void advance_parse(Parser* parser) {
    if (!parser) return;
    parser->current = parser->next;
    next_token(parser->lex);
    parser->next = parser->lex->current_token;
}

bool check_parser(Parser* p, TokenType type) {
    if (!p) return false;
    return p->current.type == type;
}

bool match_parser(Parser* p, TokenType type) {
    if (check_parser(p, type)) {
        advance_parse(p);
        return true;
    }
    return false;
}

void expect_parser(Parser* p, TokenType type) {
    if (!match_parser(p, type)) {
        fprintf(stderr, "Line %d: Expected token type %d, but got %d\n",
                p->current.line, type, p->current.type);
        p->has_error = true;
    }
}

static Token parser_consume(Parser* p) {
    Token t = p->current;
    advance_parse(p);
    return t;
}


CstNode* parse_translation_unit(Parser* p) {
    CstNode* node = cst_make_rule(CST_TRANSLATION_UNIT, p->current.line);
    while (!check_parser(p, TOKEN_EOF)) {
        CstNode* ext_decl = parse_external_declaration(p);
        if (ext_decl)
            cst_add_child(node, ext_decl);
    }
    return node;
}


CstNode* parse_external_declaration(Parser* p) {
    if (check_parser(p, TOKEN_EOF))
        return NULL;

    if (!is_specifier_keyword(p->current.type)) {
        fprintf(stderr, "Line %d: Expected declaration or function definition\n",
                p->current.line);
        p->has_error = true;
        while (!check_parser(p, TOKEN_SEMICOLON) &&
               !check_parser(p, TOKEN_RBRACE) &&
               !check_parser(p, TOKEN_EOF))
            advance_parse(p);
        if (!check_parser(p, TOKEN_EOF))
            advance_parse(p);
        return NULL;
    }

    CstNode* specifier = parse_declaration_specifiers(p);
    CstNode* decl = parse_declarator(p);

    if (check_parser(p, TOKEN_LBRACE)) {
        CstNode* body = parse_compound_statement(p);
        CstNode* func = cst_make_rule(CST_FUNCTION_DEFINITION, p->current.line);
        cst_add_child(func, specifier);
        cst_add_child(func, decl);
        cst_add_child(func, body);
        return func;
    } else {
        while (!check_parser(p, TOKEN_SEMICOLON) && !check_parser(p, TOKEN_EOF))
            advance_parse(p);
        if (check_parser(p, TOKEN_SEMICOLON))
            advance_parse(p);
        CstNode* decl_node = cst_make_rule(CST_DECLARATION, p->current.line);
        cst_add_child(decl_node, specifier);
        cst_add_child(decl_node, decl);
        return decl_node;
    }
}

CstNode* parse_declaration_specifiers(Parser* p) {
    CstNode* node = cst_make_rule(CST_DECL_SPECIFIERS, p->current.line);
    while (is_specifier_keyword(p->current.type)) {
        CstKind kind = cst_kind_for_keywords(p->current.type);
        Token t = parser_consume(p);
        CstNode* leaf = cst_make_terminal(kind, t.start, t.length, t.line);
        cst_add_child(node, leaf);
    }
    return node;
}

CstNode* parse_declarator(Parser* p) {
    CstNode* node = cst_make_rule(CST_DECLARATOR, p->current.line);
    CstNode* direct = parse_direct_declarator(p);
    cst_add_child(node, direct);
    return node;
}

CstNode* parse_direct_declarator(Parser* p) {
    CstNode* node = NULL;

    if (check_parser(p, TOKEN_IDENTIFIER)) {
        Token id = parser_consume(p);
        node = cst_make_terminal(CST_IDENTIFIER, id.start, id.length, id.line);
    } else if (check_parser(p, TOKEN_LPAREN)) {
        parser_consume(p);
        node = parse_declarator(p);
        if (!check_parser(p, TOKEN_RPAREN)) {
            fprintf(stderr, "Line %d: Expected ')'\n", p->current.line);
            p->has_error = true;
        } else {
            parser_consume(p);
        }
    } else {
        fprintf(stderr, "Line %d: Expected identifier or '('\n", p->current.line);
        p->has_error = true;
        return cst_make_rule(CST_DIRECT_DECLARATOR, p->current.line);
    }

    while (1) {
        if (check_parser(p, TOKEN_LBRACKET)) {
            CstNode* array_node = cst_make_rule(CST_DIRECT_DECLARATOR, p->current.line);
            cst_add_child(array_node, node);
            parser_consume(p);
            while (!check_parser(p, TOKEN_RBRACKET) && !check_parser(p, TOKEN_EOF))
                advance_parse(p);
            if (check_parser(p, TOKEN_RBRACKET))
                parser_consume(p);
            node = array_node;
        } else if (check_parser(p, TOKEN_LPAREN)) {
            CstNode* func_node = cst_make_rule(CST_DIRECT_DECLARATOR, p->current.line);
            cst_add_child(func_node, node);
            parser_consume(p);
            while (!check_parser(p, TOKEN_RPAREN) && !check_parser(p, TOKEN_EOF))
                advance_parse(p);
            if (check_parser(p, TOKEN_RPAREN))
                parser_consume(p);
            node = func_node;
        } else {
            break;
        }
    }

    return node;
}


CstNode* parse_compound_statement(Parser* p) {
    if (!check_parser(p, TOKEN_LBRACE)) {
        fprintf(stderr, "Line %d: Expected '{'\n", p->current.line);
        p->has_error = true;
        return NULL;
    }

    CstNode* node = cst_make_rule(CST_COMPOUND_STMT, p->current.line);

    Token lbrace = parser_consume(p);
    cst_add_child(node, cst_make_terminal(CST_LBRACE, lbrace.start, lbrace.length, lbrace.line));

    while (!check_parser(p, TOKEN_RBRACE) && !check_parser(p, TOKEN_EOF)) {
        CstNode* stmt = parse_statement(p);
        if (stmt)
            cst_add_child(node, stmt);
    }

    if (check_parser(p, TOKEN_RBRACE)) {
        Token rbrace = parser_consume(p);
        cst_add_child(node, cst_make_terminal(CST_RBRACE, rbrace.start, rbrace.length, rbrace.line));
    } else {
        fprintf(stderr, "Line %d: Expected '}'\n", p->current.line);
        p->has_error = true;
    }
    return node;
}

CstNode* parse_statement(Parser* p) {
    if (is_specifier_keyword(p->current.type))
        return parse_declaration(p);

    switch (p->current.type) {
        case TOKEN_LBRACE:   return parse_compound_statement(p);
        case TOKEN_IF:       return parse_if_statement(p);
        case TOKEN_WHILE:    return parse_while_statement(p);
        case TOKEN_FOR:      return parse_for_statement(p);
        case TOKEN_DO:       return parse_do_while_statement(p);
        case TOKEN_RETURN:   return parse_return_statement(p);
        case TOKEN_BREAK:
        case TOKEN_CONTINUE: return parse_jump_statement(p);
        case TOKEN_SEMICOLON:
            parser_consume(p);
            return cst_make_rule(CST_EXPRESSION_STMT, p->current.line);
        default:
            return parse_expression_statement(p);
    }
}

CstNode* parse_declaration(Parser* p) {
    CstNode* node = cst_make_rule(CST_DECLARATION, p->current.line);

    CstNode* specifiers = parse_declaration_specifiers(p);
    cst_add_child(node, specifiers);

    while (check_parser(p, TOKEN_ASTERISK)) {
        Token star = parser_consume(p);
        cst_add_child(node, cst_make_terminal(CST_ASTERISK, star.start, star.length, star.line));
    }

    if (check_parser(p, TOKEN_IDENTIFIER)) {
        CstNode* decl = parse_declarator(p);
        cst_add_child(node, decl);
    }

    if (check_parser(p, TOKEN_EQUALS)) {
        parser_consume(p);
        CstNode* init = parse_expression(p);
        cst_add_child(node, init);
    }

    if (check_parser(p, TOKEN_SEMICOLON))
        parser_consume(p);
    else {
        fprintf(stderr, "Line %d: Expected ';' after declaration\n", p->current.line);
        p->has_error = true;
    }

    return node;
}

CstNode* parse_if_statement(Parser* p) {
    CstNode* node = cst_make_rule(CST_IF_STMT, p->current.line);
    parser_consume(p);  // consume 'if'

    if (!check_parser(p, TOKEN_LPAREN)) {
        fprintf(stderr, "Line %d: Expected '(' after if\n", p->current.line);
        p->has_error = true;
        return node;
    }
    parser_consume(p);  // consume '('

    CstNode* condition = parse_expression(p);
    cst_add_child(node, condition);

    if (!check_parser(p, TOKEN_RPAREN)) {
        fprintf(stderr, "Line %d: Expected ')' after condition\n", p->current.line);
        p->has_error = true;
    } else {
        parser_consume(p);  // consume ')'
    }

    CstNode* body = parse_statement(p);
    cst_add_child(node, body);

    if (check_parser(p, TOKEN_ELSE)) {
        parser_consume(p);  // consume 'else'
        if (check_parser(p, TOKEN_IF)) {
            CstNode* elseif = parse_if_statement(p);
            cst_add_child(node, elseif);
        } else {
            CstNode* else_body = parse_statement(p);
            cst_add_child(node, else_body);
        }
    }

    return node;
}

CstNode* parse_while_statement(Parser* p) {
    CstNode* node = cst_make_rule(CST_WHILE_STMT, p->current.line);
    parser_consume(p);  // consume 'while'

    if (!check_parser(p, TOKEN_LPAREN)) {
        fprintf(stderr, "Line %d: Expected '(' after while\n", p->current.line);
        p->has_error = true;
        return node;
    }
    parser_consume(p);  // consume '('

    CstNode* condition = parse_expression(p);
    cst_add_child(node, condition);

    if (!check_parser(p, TOKEN_RPAREN)) {
        fprintf(stderr, "Line %d: Expected ')' after condition\n", p->current.line);
        p->has_error = true;
    } else {
        parser_consume(p);  // consume ')'
    }

    CstNode* body = parse_statement(p);
    cst_add_child(node, body);

    return node;
}

CstNode* parse_for_statement(Parser* p) {
    // stub 
    parser_consume(p);  // consume 'for'
    while (!check_parser(p, TOKEN_LBRACE) && !check_parser(p, TOKEN_EOF))
        advance_parse(p);
    CstNode* body = parse_compound_statement(p);
    CstNode* node = cst_make_rule(CST_FOR_STMT, p->current.line);
    cst_add_child(node, body);
    return node;
}

CstNode* parse_do_while_statement(Parser* p) {
    // stub 
    parser_consume(p);  // consume 'do'
    while (!check_parser(p, TOKEN_SEMICOLON) && !check_parser(p, TOKEN_EOF))
        advance_parse(p);
    return cst_make_rule(CST_DO_WHILE_STMT, p->current.line);
}

CstNode* parse_return_statement(Parser* p) {
    CstNode* node = cst_make_rule(CST_RETURN_STMT, p->current.line);
    parser_consume(p);  // consume 'return'

    if (!check_parser(p, TOKEN_SEMICOLON)) {
        CstNode* expr = parse_expression(p);
        cst_add_child(node, expr);
    }

    if (check_parser(p, TOKEN_SEMICOLON))
        parser_consume(p);
    else {
        fprintf(stderr, "Line %d: Expected ';' after return\n", p->current.line);
        p->has_error = true;
    }

    return node;
}

CstNode* parse_jump_statement(Parser* p) {
    Token t = parser_consume(p);  // consume 'break' or 'continue'
    CstKind kind = (t.type == TOKEN_BREAK) ? CST_BREAK_STMT : CST_CONTINUE_STMT;
    CstNode* node = cst_make_rule(kind, t.line);
    if (check_parser(p, TOKEN_SEMICOLON))
        parser_consume(p);
    return node;
}


CstNode* parse_expression(Parser* p) {
    return parse_expr(p, BP_NONE);
}

CstNode* parse_expression_statement(Parser* p) {
    CstNode* node = cst_make_rule(CST_EXPRESSION_STMT, p->current.line);
    CstNode* expr = parse_expression(p);
    cst_add_child(node, expr);
    if (check_parser(p, TOKEN_SEMICOLON))
        parser_consume(p);
    else {
        fprintf(stderr, "Line %d: Expected ';' after expression\n", p->current.line);
        p->has_error = true;
    }
    return node;
}

CstNode* parse_nud(Parser* p) {
    if (!p) return NULL;

    Token t = parser_consume(p);
    CstKind kind = cst_kind_token_match(t.type);

    switch (t.type) {
        case TOKEN_INT_CONST:  return cst_make_terminal(kind, t.start, t.length, t.line);
        case TOKEN_FLOAT_CONST: return cst_make_terminal(kind, t.start, t.length, t.line);
        case TOKEN_CHAR_CONST:  return  cst_make_terminal(kind, t.start, t.length, t.line);
        case TOKEN_STRING_LITERAL: return cst_make_terminal(kind, t.start, t.length, t.line);

        case TOKEN_IDENTIFIER:     return cst_make_terminal(kind, t.start, t.length, t.line);

        case TOKEN_LPAREN: {
            CstNode* inner = parse_expr(p, BP_NONE);
            if (!check_parser(p, TOKEN_RPAREN)){
                fprintf(stderr, "Line %d: Expected ')'\n", p->current.line);
                p->has_error = true;
            }else{
                parser_consume(p);
            }
            return inner;
        }

        case TOKEN_MINUS:
        case TOKEN_EXCLAM:
        case TOKEN_TILDE:
        case TOKEN_AMPERSAND:
        case TOKEN_ASTERISK:
        case TOKEN_INCREMENT:
        case TOKEN_DECREMENT:  {
            CstNode* operand = parse_expr(p, BP_UNARY);
            CstNode* node = cst_make_rule(CST_UNARY_EXPR,t.line);
            cst_add_child(node, cst_make_terminal(kind, t.start, t.length, t.line));
            cst_add_child(node, operand);
            return node;
        }

        default: {
            fprintf(stderr, "Line %d: Unexpected token in expression\n", t.line);
            return cst_make_rule(CST_ERROR, t.line);
        }
    }
}

CstNode* parse_led(Parser* p, Token op, CstNode* left) {
    switch (op.type) {

        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_ASTERISK:
        case TOKEN_SLASH:
        case TOKEN_PERCENT:
        case TOKEN_EQ_OP:
        case TOKEN_NE_OP:
        case TOKEN_LESS:
        case TOKEN_GREATER:
        case TOKEN_LE_OP:
        case TOKEN_GE_OP:
        case TOKEN_AND_OP:
        case TOKEN_OR_OP:
        case TOKEN_PIPE:
        case TOKEN_HAT:
        case TOKEN_AMPERSAND:
        case TOKEN_LEFT_SHIFT:
        case TOKEN_RIGHT_SHIFT: {
            CstNode* right = parse_expr(p, get_lbp(op.type));

            CstNode* node = cst_make_rule(CST_BINARY_EXPR, op.line);
            cst_add_child(node, left);
            cst_add_child(node, cst_make_terminal(cst_kind_token_match(op.type),
                                                   op.start, op.length, op.line));
            cst_add_child(node, right);
            return node;
        }

        case TOKEN_EQUALS:
        case TOKEN_PLUS_EQUAL:
        case TOKEN_MINUS_EQUAL:
        case TOKEN_STAR_EQUAL:
        case TOKEN_SLASH_EQUAL: {

            CstNode* right = parse_expr(p, BP_ASSIGN - 1);

            CstNode* node = cst_make_rule(CST_ASSIGNMENT_EXPR, op.line);
            cst_add_child(node, left);
            cst_add_child(node, cst_make_terminal(cst_kind_token_match(op.type),
                                                   op.start, op.length, op.line));
            cst_add_child(node, right);
            return node;
        }

        case TOKEN_LPAREN: {
            CstNode* node = cst_make_rule(CST_POSTFIX_EXPR, op.line);
            cst_add_child(node, left);

            CstNode* args = cst_make_rule(CST_ARGUMENT_LIST, op.line);
            if (!check_parser(p, TOKEN_RPAREN)) {
                cst_add_child(args, parse_expr(p, BP_ASSIGN));
                while (check_parser(p, TOKEN_COMMA)) {
                    parser_consume(p);
                    cst_add_child(args, parse_expr(p, BP_ASSIGN));
                }
            }
            cst_add_child(node, args);

            if (!check_parser(p, TOKEN_RPAREN)) {
                fprintf(stderr, "Line %d: Expected ')' after arguments\n", p->current.line);
                p->has_error = true;
            } else {
                parser_consume(p);
            }
            return node;
        }


        case TOKEN_LBRACKET: {
            CstNode* index = parse_expr(p, BP_NONE);

            if (!check_parser(p, TOKEN_RBRACKET)) {
                fprintf(stderr, "Line %d: Expected ']' after index\n", p->current.line);
                p->has_error = true;
            } else {
                parser_consume(p);
            }

            CstNode* node = cst_make_rule(CST_POSTFIX_EXPR, op.line);
            cst_add_child(node, left);
            cst_add_child(node, index);
            return node;
        }

        case TOKEN_DOT:
        case TOKEN_ARROW: {
            CstNode* node = cst_make_rule(CST_POSTFIX_EXPR, op.line);
            cst_add_child(node, left);
            cst_add_child(node, cst_make_terminal(cst_kind_token_match(op.type),
                                                   op.start, op.length, op.line));

            if (check_parser(p, TOKEN_IDENTIFIER)) {
                Token field = parser_consume(p);
                cst_add_child(node, cst_make_terminal(CST_IDENTIFIER,
                                                       field.start, field.length, field.line));
            } else {
                fprintf(stderr, "Line %d: Expected field name\n", p->current.line);
                p->has_error = true;
            }
            return node;
        }

        case TOKEN_INCREMENT:
        case TOKEN_DECREMENT: {
            CstNode* node = cst_make_rule(CST_POSTFIX_EXPR, op.line);
            cst_add_child(node, left);
            cst_add_child(node, cst_make_terminal(cst_kind_token_match(op.type),
                                                   op.start, op.length, op.line));
            return node;
        }

        default:
            fprintf(stderr, "Line %d: Unexpected operator in expression\n", op.line);
            return left;
    }
}

CstNode* parse_expr(Parser* p, int min_bp) {
    CstNode* left = parse_nud(p);

    while(get_lbp(p->current.type ) > min_bp){
        Token op = parser_consume(p);
        left = parse_led(p, op, left);
    }
    return left;
}
