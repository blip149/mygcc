#include "include/lexer.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static char peek(const Lexer* lex, int offset) {
    if (lex->cursor + offset < lex->length)
        return lex->source[lex->cursor + offset];
    return '\0';
}

void lexer_init(Lexer* lex, const char* source, int len) {
    lex->source = source;
    lex->length = len;
    lex->cursor = 0;
    lex->line = 1;
}

static void skip_whitespace_and_comments(Lexer* lex) {
    while (lex->cursor < lex->length) {
        char c = lex->source[lex->cursor];
        
        if (c == ' ' || c == '\t') {
            lex->cursor++;
        }
        else if (c == '\n') {
            lex->line++;
            lex->cursor++;
        }
        else if (c == '\r') {
            lex->cursor++;
            if (lex->cursor < lex->length && lex->source[lex->cursor] == '\n') {
                lex->cursor++;
            }
            lex->line++;
        }
        else if (c == '/' && peek(lex, 1) == '/') {
            lex->cursor += 2;
            while (lex->cursor < lex->length && lex->source[lex->cursor] != '\n')
                lex->cursor++;
        }
        else if (c == '/' && peek(lex, 1) == '*') {
            lex->cursor += 2;
            while (lex->cursor + 1 < lex->length) {
                if (lex->source[lex->cursor] == '*' && lex->source[lex->cursor + 1] == '/') {
                    lex->cursor += 2;
                    break;
                }
                if (lex->source[lex->cursor] == '\n')
                    lex->line++;
                lex->cursor++;
            }
        }
        else if (c == '#') {
            while (lex->cursor < lex->length && lex->source[lex->cursor] != '\n')
                lex->cursor++;
        }
        else {
            break;
        }
    }
}

/* --- Number scanning --- */

static void scan_integer_suffix(Lexer* lex) {
    while (lex->cursor < lex->length) {
        char c = lex->source[lex->cursor];
        if (c == 'u' || c == 'U' || c == 'l' || c == 'L')
            lex->cursor++;
        else
            break;
    }
}

static Token* scan_integer(Lexer* lex, const char* start) {
    if (start[0] == '0') {
        char next = lex->source[lex->cursor];
        if ((next == 'x' || next == 'X') && isxdigit(peek(lex, 1))) {
            lex->cursor++; /* skip 'x' */
            while (lex->cursor < lex->length && isxdigit(lex->source[lex->cursor]))
                lex->cursor++;
        } else {
            while (lex->cursor < lex->length && lex->source[lex->cursor] >= '0' 
                   && lex->source[lex->cursor] <= '7')
                lex->cursor++;
        }
    } else {
        while (lex->cursor < lex->length && isdigit(lex->source[lex->cursor]))
            lex->cursor++;
    }

    scan_integer_suffix(lex);


    int len = (lex->source + lex->cursor) - start;
    lex->current_token = (Token){TOKEN_INTEGER_CONST, start, len, lex->line};
    return &lex->current_token;
}

static Token* scan_float(Lexer* lex, const char* start) {
    char c = lex->source[lex->cursor];
    if (c == '.') {
        lex->cursor++;
        while (lex->cursor < lex->length && isdigit(lex->source[lex->cursor]))
            lex->cursor++;
    }
    if (lex->cursor < lex->length && 
        (lex->source[lex->cursor] == 'e' || lex->source[lex->cursor] == 'E')) {
        lex->cursor++;
        if (lex->cursor < lex->length && 
            (lex->source[lex->cursor] == '+' || lex->source[lex->cursor] == '-'))
            lex->cursor++;
        while (lex->cursor < lex->length && isdigit(lex->source[lex->cursor]))
            lex->cursor++;
    }
    if (lex->cursor < lex->length) {
        char suff = lex->source[lex->cursor];
        if (suff == 'f' || suff == 'F' || suff == 'l' || suff == 'L')
            lex->cursor++;
    }


    int len = (lex->source + lex->cursor) - start;
    lex->current_token = (Token){TOKEN_FLOAT_CONST, start, len, lex->line};
    return &lex->current_token;
}



static Token* scan_identifier(Lexer* lex, const char* start) {
    while (lex->cursor < lex->length) {
        char ch = lex->source[lex->cursor];
        if (isalnum(ch) || ch == '_')
            lex->cursor++;
        else
            break;
    }
    int len = (lex->source + lex->cursor) - start;
    TokenType type = TOKEN_IDENTIFIER;

    switch (len) {
        case 2:
            if (strncmp(start, "do", 2) == 0) type = TOKEN_DO;
            else if (strncmp(start, "if", 2) == 0) type = TOKEN_IF;
            break;
        case 3:
            if (strncmp(start, "for", 3) == 0) type = TOKEN_FOR;
            else if (strncmp(start, "int", 3) == 0) type = TOKEN_INT;
            break;
        case 4:
            if (strncmp(start, "auto", 4) == 0) type = TOKEN_AUTO;
            else if (strncmp(start, "case", 4) == 0) type = TOKEN_CASE;
            else if (strncmp(start, "char", 4) == 0) type = TOKEN_CHAR;
            else if (strncmp(start, "else", 4) == 0) type = TOKEN_ELSE;
            else if (strncmp(start, "enum", 4) == 0) type = TOKEN_ENUM;
            else if (strncmp(start, "goto", 4) == 0) type = TOKEN_GOTO;
            else if (strncmp(start, "long", 4) == 0) type = TOKEN_LONG;
            else if (strncmp(start, "void", 4) == 0) type = TOKEN_VOID;
            break;
        case 5:
            if (strncmp(start, "break", 5) == 0) type = TOKEN_BREAK;
            else if (strncmp(start, "const", 5) == 0) type = TOKEN_CONST;
            else if (strncmp(start, "float", 5) == 0) type = TOKEN_FLOAT;
            else if (strncmp(start, "short", 5) == 0) type = TOKEN_SHORT;
            else if (strncmp(start, "union", 5) == 0) type = TOKEN_UNION;
            else if (strncmp(start, "while", 5) == 0) type = TOKEN_WHILE;
            break;
        case 6:
            if (strncmp(start, "double", 6) == 0) type = TOKEN_DOUBLE;
            else if (strncmp(start, "extern", 6) == 0) type = TOKEN_EXTERN;
            else if (strncmp(start, "return", 6) == 0) type = TOKEN_RETURN;
            else if (strncmp(start, "signed", 6) == 0) type = TOKEN_SIGNED;
            else if (strncmp(start, "sizeof", 6) == 0) type = TOKEN_SIZEOF;
            else if (strncmp(start, "static", 6) == 0) type = TOKEN_STATIC;
            else if (strncmp(start, "struct", 6) == 0) type = TOKEN_STRUCT;
            else if (strncmp(start, "switch", 6) == 0) type = TOKEN_SWITCH;
            break;
        case 7:
            if (strncmp(start, "default", 7) == 0) type = TOKEN_DEFAULT;
            else if (strncmp(start, "typedef", 7) == 0) type = TOKEN_TYPEDEF;
            break;
        case 8:
            if (strncmp(start, "continue", 8) == 0) type = TOKEN_CONTINUE;
            else if (strncmp(start, "register", 8) == 0) type = TOKEN_REGISTER;
            else if (strncmp(start, "unsigned", 8) == 0) type = TOKEN_UNSIGNED;
            else if (strncmp(start, "volatile", 8) == 0) type = TOKEN_VOLATILE;
            break;
    }
    lex->current_token = (Token){type, start, len, lex->line};
    return &lex->current_token;
}

/* --- Character and string literals --- */

static Token* scan_char_literal(Lexer* lex, const char* start) {
    lex->cursor++; /* skip opening quote */
    while (lex->cursor < lex->length && lex->source[lex->cursor] != '\'') {
        if (lex->source[lex->cursor] == '\\') {
            lex->cursor++;
            if (lex->cursor < lex->length)
                lex->cursor++;
        } else {
            lex->cursor++;
        }
    }
    if (lex->cursor < lex->length)
        lex->cursor++; /* skip closing quote */

    int len = (lex->source + lex->cursor) - start;
    lex->current_token = (Token){TOKEN_CHAR_CONST, start, len, lex->line};
    return &lex->current_token;
}

static Token* scan_string_literal(Lexer* lex, const char* start) {
    lex->cursor++; /* skip opening quote */
    while (lex->cursor < lex->length && lex->source[lex->cursor] != '"') {
        if (lex->source[lex->cursor] == '\\') {
            lex->cursor++;
            if (lex->cursor < lex->length)
                lex->cursor++;
        } else {
            lex->cursor++;
        }
    }
    if (lex->cursor < lex->length)
        lex->cursor++; /* skip closing quote */

    int len = (lex->source + lex->cursor) - start;
    lex->current_token = (Token){TOKEN_STRING_LITERAL, start, len, lex->line};
    return &lex->current_token;
}

/* --- Main token dispatcher --- */

Token* next_token(Lexer* lex) {
    skip_whitespace_and_comments(lex);

    if (lex->cursor >= lex->length) {
        lex->current_token = (Token){TOKEN_EOF, lex->source + lex->cursor, 0, lex->line};
        return &lex->current_token;
    }

    char c = lex->source[lex->cursor];
    lex->current_token.line = lex->line;

    /* Multi‑character operators first */
    if (lex->cursor + 1 < lex->length) {
        char next = lex->source[lex->cursor + 1];
        switch (c) {
            case '+':
                if (next == '+') {
                    lex->current_token = (Token){TOKEN_INCREMENT, &lex->source[lex->cursor], 2, lex->line};
                    lex->cursor += 2;
                    return &lex->current_token;
                }
                break;
            case '-':
                if (next == '-') {
                    lex->current_token = (Token){TOKEN_DECREMENT, &lex->source[lex->cursor], 2, lex->line};
                    lex->cursor += 2;
                    return &lex->current_token;
                } else if (next == '>') {
                    lex->current_token = (Token){TOKEN_ARROW, &lex->source[lex->cursor], 2, lex->line};
                    lex->cursor += 2;
                    return &lex->current_token;
                }
                break;
            case '=':
                if (next == '=') {
                    lex->current_token = (Token){TOKEN_EQ_OP, &lex->source[lex->cursor], 2, lex->line};
                    lex->cursor += 2;
                    return &lex->current_token;
                }
                break;
            case '!':
                if (next == '=') {
                    lex->current_token = (Token){TOKEN_NE_OP, &lex->source[lex->cursor], 2, lex->line};
                    lex->cursor += 2;
                    return &lex->current_token;
                }
                break;
            case '<':
                if (next == '=') {
                    lex->current_token = (Token){TOKEN_LE_OP, &lex->source[lex->cursor], 2, lex->line};
                    lex->cursor += 2;
                    return &lex->current_token;
                } else if (next == '<') {
                    lex->current_token = (Token){TOKEN_LEFT_SHIFT, &lex->source[lex->cursor], 2, lex->line};
                    lex->cursor += 2;
                    return &lex->current_token;
                }
                break;
            case '>':
                if (next == '=') {
                    lex->current_token = (Token){TOKEN_GE_OP, &lex->source[lex->cursor], 2, lex->line};
                    lex->cursor += 2;
                    return &lex->current_token;
                } else if (next == '>') {
                    lex->current_token = (Token){TOKEN_RIGHT_SHIFT, &lex->source[lex->cursor], 2, lex->line};
                    lex->cursor += 2;
                    return &lex->current_token;
                }
                break;
            case '&':
                if (next == '&') {
                    lex->current_token = (Token){TOKEN_AND_OP, &lex->source[lex->cursor], 2, lex->line};
                    lex->cursor += 2;
                    return &lex->current_token;
                }
                break;
            case '|':
                if (next == '|') {
                    lex->current_token = (Token){TOKEN_OR_OP, &lex->source[lex->cursor], 2, lex->line};
                    lex->cursor += 2;
                    return &lex->current_token;
                }
                break;
            case '.':
                if (next == '.' && lex->cursor + 2 < lex->length && lex->source[lex->cursor + 2] == '.') {
                    lex->current_token = (Token){TOKEN_ELLIPSIS, &lex->source[lex->cursor], 3, lex->line};
                    lex->cursor += 3;
                    return &lex->current_token;
                }
                break;
        }
    }

    /* Single‑character tokens */
    switch (c) {
        case '(': lex->cursor++; lex->current_token = (Token){TOKEN_LPAREN, &lex->source[lex->cursor-1], 1, lex->line}; return &lex->current_token;
        case ')': lex->cursor++; lex->current_token = (Token){TOKEN_RPAREN, &lex->source[lex->cursor-1], 1, lex->line}; return &lex->current_token;
        case '[': lex->cursor++; lex->current_token = (Token){TOKEN_LBRACKET, &lex->source[lex->cursor-1], 1, lex->line}; return &lex->current_token;
        case ']': lex->cursor++; lex->current_token = (Token){TOKEN_RBRACKET, &lex->source[lex->cursor-1], 1, lex->line}; return &lex->current_token;
        case '{': lex->cursor++; lex->current_token = (Token){TOKEN_LBRACE, &lex->source[lex->cursor-1], 1, lex->line}; return &lex->current_token;
        case '}': lex->cursor++; lex->current_token = (Token){TOKEN_RBRACE, &lex->source[lex->cursor-1], 1, lex->line}; return &lex->current_token;
        case ';': lex->cursor++; lex->current_token = (Token){TOKEN_SEMICOLON, &lex->source[lex->cursor-1], 1, lex->line}; return &lex->current_token;
        case ',': lex->cursor++; lex->current_token = (Token){TOKEN_COMMA, &lex->source[lex->cursor-1], 1, lex->line}; return &lex->current_token;
        case '.': lex->cursor++; lex->current_token = (Token){TOKEN_DOT, &lex->source[lex->cursor-1], 1, lex->line}; return &lex->current_token;
        case '&': lex->cursor++; lex->current_token = (Token){TOKEN_AMPERSAND, &lex->source[lex->cursor-1], 1, lex->line}; return &lex->current_token;
        case '*': lex->cursor++; lex->current_token = (Token){TOKEN_ASTERISK, &lex->source[lex->cursor-1], 1, lex->line}; return &lex->current_token;
        case '+': lex->cursor++; lex->current_token = (Token){TOKEN_PLUS, &lex->source[lex->cursor-1], 1, lex->line}; return &lex->current_token;
        case '-': lex->cursor++; lex->current_token = (Token){TOKEN_MINUS, &lex->source[lex->cursor-1], 1, lex->line}; return &lex->current_token;
        case '~': lex->cursor++; lex->current_token = (Token){TOKEN_TILDE, &lex->source[lex->cursor-1], 1, lex->line}; return &lex->current_token;
        case '!': lex->cursor++; lex->current_token = (Token){TOKEN_EXCLAM, &lex->source[lex->cursor-1], 1, lex->line}; return &lex->current_token;
        case '/': lex->cursor++; lex->current_token = (Token){TOKEN_SLASH, &lex->source[lex->cursor-1], 1, lex->line}; return &lex->current_token;
        case '%': lex->cursor++; lex->current_token = (Token){TOKEN_PERCENT, &lex->source[lex->cursor-1], 1, lex->line}; return &lex->current_token;
        case '<': lex->cursor++; lex->current_token = (Token){TOKEN_LESS, &lex->source[lex->cursor-1], 1, lex->line}; return &lex->current_token;
        case '>': lex->cursor++; lex->current_token = (Token){TOKEN_GREATER, &lex->source[lex->cursor-1], 1, lex->line}; return &lex->current_token;
        case '^': lex->cursor++; lex->current_token = (Token){TOKEN_HAT, &lex->source[lex->cursor-1], 1, lex->line}; return &lex->current_token;
        case '|': lex->cursor++; lex->current_token = (Token){TOKEN_PIPE, &lex->source[lex->cursor-1], 1, lex->line}; return &lex->current_token;
        case '?': lex->cursor++; lex->current_token = (Token){TOKEN_QUESTION, &lex->source[lex->cursor-1], 1, lex->line}; return &lex->current_token;
        case ':': lex->cursor++; lex->current_token = (Token){TOKEN_COLON, &lex->source[lex->cursor-1], 1, lex->line}; return &lex->current_token;
        case '=': lex->cursor++; lex->current_token = (Token){TOKEN_EQUALS, &lex->source[lex->cursor-1], 1, lex->line}; return &lex->current_token;
    }

    /* Identifier */
    if (isalpha(c) || c == '_') {
        const char* start = &lex->source[lex->cursor];
        lex->cursor++;  /* advance past first char */
        return scan_identifier(lex, start);
    }

    /* Number: integer or float */
    if (isdigit(c)) {
        const char* start = &lex->source[lex->cursor];
        if (c == '0' && (peek(lex, 1) == 'x' || peek(lex, 1) == 'X')) {
            /* hex integer (cannot be float) */
            lex->cursor++; /* skip '0' */
            lex->cursor++; /* skip 'x' */
            while (lex->cursor < lex->length && isxdigit(lex->source[lex->cursor]))
                lex->cursor++;
            scan_integer(lex, start);
            int len = (lex->source + lex->cursor) - start;  
            lex->current_token = (Token){TOKEN_INTEGER_CONST, start, len, lex->line};
            return &lex->current_token;
        }
        /* decimal or octal */
        lex->cursor++; /* consume first digit */
        if (c == '0') {
            while (lex->cursor < lex->length && lex->source[lex->cursor] >= '0' && lex->source[lex->cursor] <= '7')
                lex->cursor++;
        } else {
            while (lex->cursor < lex->length && isdigit(lex->source[lex->cursor]))
                lex->cursor++;
        }
        /* decide if float */
        char next = peek(lex, 0);
        if (next == '.' && isdigit(peek(lex, 1))) {
            return scan_float(lex, start);
        } else if (next == 'e' || next == 'E') {
            return scan_float(lex, start);
        } else {
            scan_integer(lex, start);
            int len = (lex->source + lex->cursor) - start;   /* FIX */
            lex->current_token = (Token){TOKEN_INTEGER_CONST, start, len, lex->line};
            return &lex->current_token;
        }
    }

    /* Character constant */
    if (c == '\'') {
        return scan_char_literal(lex, &lex->source[lex->cursor]);
    }

    /* String literal */
    if (c == '"') {
        return scan_string_literal(lex, &lex->source[lex->cursor]);
    }

    /* Anything else is an error */
    lex->current_token = (Token){TOKEN_ERROR, &lex->source[lex->cursor], 1, lex->line};
    lex->cursor++;
    return &lex->current_token;
}
