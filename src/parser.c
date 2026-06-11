#include "include/parser.h"


static Token parser_consume(Parser* p) {
    Token t = p->current;
    advance_parse(p);
    return t;
}


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
    }
    else if (check_parser(p, TOKEN_LPAREN)) {
        parser_consume(p);  
        node = parse_declarator(p);  
        if (!check_parser(p, TOKEN_RPAREN)) {
            fprintf(stderr, "Line %d: Expected ')'\n", p->current.line);
            p->has_error = true;
        } else {
            parser_consume(p); 
        }

    }
    else {
        fprintf(stderr, "Line %d: Expected identifier or '('\n", p->current.line);
        p->has_error = true;
        return cst_make_rule(CST_DIRECT_DECLARATOR, p->current.line);
    }

    while (1) {
        if (check_parser(p, TOKEN_LBRACE)) {
            CstNode* array_node = cst_make_rule(CST_DIRECT_DECLARATOR, p->current.line);
            cst_add_child(array_node, node); 
            parser_consume(p);  
            while (!check_parser(p, TOKEN_RBRACE) && !check_parser(p, TOKEN_EOF))
                advance_parse(p);
            if (check_parser(p, TOKEN_RBRACE))
                parser_consume(p); 
            node = array_node;  
        }
        else if (check_parser(p, TOKEN_LPAREN)) {
            CstNode* func_node = cst_make_rule(CST_DIRECT_DECLARATOR, p->current.line);
            cst_add_child(func_node, node); 
            parser_consume(p); 
            while (!check_parser(p, TOKEN_RPAREN) && !check_parser(p, TOKEN_EOF))
                advance_parse(p);
            if (check_parser(p, TOKEN_RPAREN))
                parser_consume(p); 
            node = func_node;
        }
        else {
            break;
        }
    }

    return node;
}

CstNode* parse_compound_statement(Parser* p) {
    if (!check_parser(p, TOKEN_LBRACE)) {
        fprintf(stderr, "Line %d: Expected '{'\n", p->current.line);
        p->has_error = true;
        return;
    }

    CstNode* node = cst_make_rule(CST_COMPOUND_STMT, p->current.line);

    Token lbrace = parser_consume(p);
    cst_add_child(node, cst_make_terminal(CST_LBRACE, lbrace.start, lbrace.length, lbrace.line));


    while (!check_parser(p, TOKEN_RBRACKET) && !check_parser(p, TOKEN_EOF)){
        CstNode* stmt = parse_statement(p);
        if(stmt){
            cst_add_child(node, stmt);
        }
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
    switch (p->current.type) {
        case TOKEN_LBRACE:
            return parse_compound_statement(p);
        case TOKEN_IF:
            return parse_if_statement(p);
        case TOKEN_WHILE:
            return parse_while_statement(p);
        case TOKEN_RETURN:
            return parse_return_statement(p);
        case TOKEN_FOR:       // not yet, but we can skip safely
        case TOKEN_DO:
        case TOKEN_SWITCH:
        case TOKEN_GOTO:
        case TOKEN_CONTINUE:
        case TOKEN_BREAK:
        case TOKEN_SEMICOLON:  // empty statement
            // fallback: treat as expression statement
            return parse_expression_statement(p);
        default:
            // could be an expression statement
            return parse_expression_statement(p);
    }
}

CstNode* parse_expression(Parser* p) {
    /* Stub: consume tokens until a statement terminator */
    while (!check_parser(p, TOKEN_SEMICOLON) &&
           !check_parser(p, TOKEN_RBRACE) &&
           !check_parser(p, TOKEN_EOF))
        advance_parse(p);

    return cst_make_terminal(CST_ERROR, "expr_stub", 9, p->current.line);
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

CstNode* parse_if_statement(Parser* p) {
    CstNode* node = cst_make_rule(CST_IF_STMT, p->current.line);

    parser_consume(p);  

    if (!check_parser(p, TOKEN_LPAREN)) {
        fprintf(stderr, "Line %d: Expected '(' after if\n", p->current.line);
        p->has_error = true;
        return node;
    }else{
        parser_consume(p); 
    }

    CstNode* condition = parse_expression(p);
    cst_add_child(node, condition);

    if (!check_parser(p, TOKEN_RPAREN)) {
        fprintf(stderr, "Line %d: Expected ')' after condition\n", p->current.line);
        p->has_error = true;
    } else {
        parser_consume(p);  
    }

    CstNode* body = parse_statement(p);
    cst_add_child(node, body);

    if (check_parser(p, TOKEN_ELSEIF)) {
        if (!check_parser(p, TOKEN_LPAREN)) {

            fprintf(stderr, "Line %d: Expected '(' after if\n", p->current.line);
            p->has_error = true;
            return node;
        }else{
            parser_consume(p); 
        }

        CstNode* condition = parse_expression(p);
        cst_add_child(node, condition);

        if (!check_parser(p, TOKEN_RPAREN)) {
            fprintf(stderr, "Line %d: Expected ')' after condition\n", p->current.line);
            p->has_error = true;
        } else {
            parser_consume(p);  
        }
        CstNode* elseif_body = parse_statement(p);
        cst_add_child(node, elseif_body);

    }else {
       if (check_parser(p, TOKEN_ELSE))
           parser_consume(p);  // consume 'else'
           CstNode* else_body = parse_statement(p);
           cst_add_child(node, else_body);
    }
    return node;
}

CstNode* parse_while_statement(Parser* p) {
    CstNode* node = cst_make_rule(CST_WHILE_STMT, p->current.line);

    if (!check_parser(p, TOKEN_LPAREN)){
        fprintf(stderr, "Line %d: Expected '(' before condition\n", p->current.line);
        p->has_error = true;
    }else{
        parser_consume(p);
    }
    CstNode* condition = parse_statement(p);
    cst_add_child(node, condition);
    if(!check_parser(p, TOKEN_RPAREN)){
        fprintf(stderr, "Line %d: Expected ')' after condition\n", p->current.line);
        p->has_error = true;

    }else{
        parser_consume(p);
    }
    CstNode* body = parse_statement(p);
    cst_add_child(node, body);

    return node;
}

CstNode* parse_return_statement(Parser* p) {
    CstNode* node = cst_make_rule(CST_RETURN_STMT, p->current.line);
    parser_consume(p); 

    if (!check_parser(p, TOKEN_SEMICOLON)) {
        CstNode* expr = parse_expression(p);
        cst_add_child(node, expr);
        fprintf(stderr, "Line %d: Expected ';' after return\n", p->current.line);
        p->has_error = true;
    }
    if (check_parser(p, TOKEN_SEMICOLON))
        parser_consume(p);

    return node;
}


