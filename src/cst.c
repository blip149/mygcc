#include "include/cst.h"
#include <stdlib.h>
#include <stdio.h>

CstNode* cst_make_terminal(CstKind kind, const char* start, int length, int line){
    CstNode* node = malloc(sizeof(CstNode));
    if(!node) return NULL;

    node->kind = kind;
    node->line = line;
    node->u.token.length = length;
    node->u.token.start = start;

    return node;

}

CstNode* cst_make_rule(CstKind kind, int line){
    CstNode* node = malloc(sizeof(CstNode));
    if(!node) return NULL;

    node->kind = kind;
    node->line = line;
    node->u.rule.capacity = 0;
    node->u.rule.children = NULL;
    node->u.rule.count = 0;

    return node;
}


void cst_add_child(CstNode* parent, CstNode* child) {
    if (parent->u.rule.count >= parent->u.rule.capacity) {
        int new_cap = (parent->u.rule.capacity == 0) ? 4 : parent->u.rule.capacity * 2;
        CstNode** new_children = realloc(parent->u.rule.children,
                                          new_cap * sizeof(CstNode*));
        if (!new_children) return;
        parent->u.rule.children = new_children;
        parent->u.rule.capacity = new_cap;
    }
    parent->u.rule.children[parent->u.rule.count++] = child;
}


void cst_free(CstNode* node){
    if (!node) return;

    if (node->kind < CST_IDENTIFIER) {
        for( int i = 0; i<node->u.rule.count; i++) 
            cst_free(node->u.rule.children[i]);
        free(node->u.rule.children);
    }
    free(node);
}

void cst_print(CstNode* node, int depth) {
    if(!node) return;

    for (int i = 0; i< depth * 2; i++) {
        printf(" ");
    }

    if(node->kind >= CST_IDENTIFIER){
        printf("[%s] \"%.*s\"\n", cst_kind_name(node->kind), node->u.token.length, node->u.token.start);
    }else{
        printf("(%s)\n", cst_kind_name(node->kind));
        for (int i = 0; i<node->u.rule.count; i++) {
            cst_print(node->u.rule.children[i], depth + 1);
        }
    }

}

const char* cst_kind_name(CstKind kind) {
    switch(kind) {
        case CST_TRANSLATION_UNIT: return "translation_unit";
        case CST_EXTERNAL_DECLARATION: return "external_declaration";
        case CST_FUNCTION_DEFINITION: return "function_definition";
        case CST_DECLARATION: return "declaration";
        case CST_DECL_SPECIFIERS: return "decl_specifiers";
        case CST_STORAGE_CLASS: return "storage_class";
        case CST_TYPE_QUALIFIER: return "type_qualifier";
        case CST_TYPE_SPECIFIER: return "type_specifier";
        case CST_STRUCT_OR_UNION: return "struct_or_union";
        case CST_ENUM_SPECIFIER: return "enum_specifier";
        case CST_INIT_DECLARATOR: return "init_declarator";
        case CST_DECLARATOR: return "declarator";
        case CST_POINTER: return "pointer";
        case CST_DIRECT_DECLARATOR: return "direct_declarator";
        case CST_PARAMETER_LIST: return "parameter_list";
        case CST_PARAMETER_DECLARATION: return "parameter_declaration";
        case CST_INITIALIZER: return "initializer";
        case CST_INITIALIZER_LIST: return "initializer_list";
        case CST_DESIGNATED_INITIALIZER: return "designated_initializer";
        case CST_STATEMENT: return "statement";
        case CST_COMPOUND_STMT: return "compound_stmt";
        case CST_IF_STMT: return "if_stmt";
        case CST_SWITCH_STMT: return "switch_stmt";
        case CST_WHILE_STMT: return "while_stmt";
        case CST_DO_WHILE_STMT: return "do_while_stmt";
        case CST_FOR_STMT: return "for_stmt";
        case CST_GOTO_STMT: return "goto_stmt";
        case CST_CONTINUE_STMT: return "continue_stmt";
        case CST_BREAK_STMT: return "break_stmt";
        case CST_RETURN_STMT: return "return_stmt";
        case CST_LABELED_STMT: return "labeled_stmt";
        case CST_EXPRESSION_STMT: return "expression_stmt";
        case CST_PRIMARY_EXPR: return "primary_expr";
        case CST_POSTFIX_EXPR: return "postfix_expr";
        case CST_ARGUMENT_LIST: return "argument_list";
        case CST_BINARY_EXPR: return "binary_expr";
        case CST_UNARY_EXPR: return "unary_expr";
        case CST_CAST_EXPR: return "cast_expr";
        case CST_MULTIPLICATIVE_EXPR: return "multiplicative_expr";
        case CST_ADDITIVE_EXPR: return "additive_expr";
        case CST_SHIFT_EXPR: return "shift_expr";
        case CST_RELATIONAL_EXPR: return "relational_expr";
        case CST_EQUALITY_EXPR: return "equality_expr";
        case CST_AND_EXPR: return "and_expr";
        case CST_EXCLUSIVE_OR_EXPR: return "exclusive_or_expr";
        case CST_INCLUSIVE_OR_EXPR: return "inclusive_or_expr";
        case CST_LOGICAL_AND_EXPR: return "logical_and_expr";
        case CST_LOGICAL_OR_EXPR: return "logical_or_expr";
        case CST_CONDITIONAL_EXPR: return "conditional_expr";
        case CST_ASSIGNMENT_EXPR: return "assignment_expr";
        case CST_EXPRESSION: return "expression";
        case CST_TYPE_NAME: return "type_name";
        case CST_SPECIFIER_QUALIFIER_LIST: return "specifier_qualifier_list";
        case CST_ABSTRACT_DECLARATOR: return "abstract_declarator";
        case CST_DIRECT_ABSTRACT_DECLARATOR: return "direct_abstract_declarator";

        case CST_IDENTIFIER: return "identifier";
        case CST_INT_CONST: return "int_const";
        case CST_FLOAT_CONST: return "float_const";
        case CST_CHAR_CONST: return "char_const";
        case CST_STRING_LITERAL: return "string_literal";

        case CST_AUTO: return "auto";
        case CST_BREAK: return "break";
        case CST_CASE: return "case";
        case CST_CHAR: return "char";
        case CST_CONST: return "const";
        case CST_CONTINUE: return "continue";
        case CST_DEFAULT: return "default";
        case CST_DO: return "do";
        case CST_DOUBLE: return "double";
        case CST_ELSE: return "else";
        case CST_ENUM: return "enum";
        case CST_EXTERN: return "extern";
        case CST_FLOAT: return "float";
        case CST_FOR: return "for";
        case CST_GOTO: return "goto";
        case CST_IF: return "if";
        case CST_INT: return "int";
        case CST_LONG: return "long";
        case CST_REGISTER: return "register";
        case CST_RETURN: return "return";
        case CST_SHORT: return "short";
        case CST_SIGNED: return "signed";
        case CST_SIZEOF: return "sizeof";
        case CST_STATIC: return "static";
        case CST_STRUCT: return "struct";
        case CST_SWITCH: return "switch";
        case CST_TYPEDEF: return "typedef";
        case CST_UNION: return "union";
        case CST_UNSIGNED: return "unsigned";
        case CST_VOID: return "void";
        case CST_VOLATILE: return "volatile";
        case CST_WHILE: return "while";
        case CST_INLINE: return "inline";

        case CST_LPAREN: return "lparen";
        case CST_RPAREN: return "rparen";
        case CST_LBRACKET: return "lbracket";
        case CST_RBRACKET: return "rbracket";
        case CST_LBRACE: return "lbrace";
        case CST_RBRACE: return "rbrace";
        case CST_SEMICOLON: return "semicolon";
        case CST_COMMA: return "comma";
        case CST_DOT: return "dot";
        case CST_AMPERSAND: return "ampersand";
        case CST_ASTERISK: return "asterisk";
        case CST_PLUS: return "plus";
        case CST_MINUS: return "minus";
        case CST_TILDE: return "tilde";
        case CST_EXCLAM: return "exclam";
        case CST_SLASH: return "slash";
        case CST_PERCENT: return "percent";
        case CST_LESS: return "less";
        case CST_GREATER: return "greater";
        case CST_HAT: return "hat";
        case CST_PIPE: return "pipe";
        case CST_QUESTION: return "question";
        case CST_COLON: return "colon";
        case CST_EQUALS: return "equals";
        case CST_ELLIPSIS: return "ellipsis";
        case CST_ARROW: return "arrow";
        case CST_INCREMENT: return "increment";
        case CST_DECREMENT: return "decrement";
        case CST_LEFT_SHIFT: return "left_shift";
        case CST_RIGHT_SHIFT: return "right_shift";
        case CST_LE_OP: return "le_op";
        case CST_GE_OP: return "ge_op";
        case CST_EQ_OP: return "eq_op";
        case CST_NE_OP: return "ne_op";
        case CST_AND_OP: return "and_op";
        case CST_OR_OP: return "or_op";
        case CST_MINUS_EQUAL: return "minus_equal";
        case CST_PLUS_EQUAL: return "plus_equal";
        case CST_STAR_EQUAL: return "star_equal";
        case CST_SLASH_EQUAL: return "slash_equal";
        case CST_EOF: return "eof";
        case CST_ERROR: return "error";
        
        default: return "unknown";
    }
}