#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_AUTO,          /* auto */
    TOKEN_BREAK,         /* break */
    TOKEN_CASE,          /* case */
    TOKEN_CHAR,          /* char */
    TOKEN_CONST,         /* const */
    TOKEN_CONTINUE,      /* continue */
    TOKEN_DEFAULT,       /* default */
    TOKEN_DO,            /* do */
    TOKEN_DOUBLE,        /* double */
    TOKEN_ELSE,         /* else */
    TOKEN_ENUM,          /* enum */
    TOKEN_EXTERN,        /* extern */
    TOKEN_FLOAT,         /* float */
    TOKEN_FOR,           /* for */
    TOKEN_GOTO,          /* goto */
    TOKEN_IF,            /* if */
    TOKEN_INT,           /* int */
    TOKEN_LONG,          /* long */
    TOKEN_REGISTER,      /* register */
    TOKEN_RETURN,        /* return */
    TOKEN_SHORT,         /* short */
    TOKEN_SIGNED,        /* signed */
    TOKEN_SIZEOF,        /* sizeof */
    TOKEN_STATIC,        /* static */
    TOKEN_STRUCT,        /* struct */
    TOKEN_SWITCH,        /* switch */
    TOKEN_TYPEDEF,       /* typedef */
    TOKEN_UNION,         /* union */
    TOKEN_UNSIGNED,      /* unsigned */
    TOKEN_VOID,          /* void */
    TOKEN_VOLATILE,      /* volatile */
    TOKEN_WHILE,         /* while */
    TOKEN_INLINE,

    /* Identifiers and literals */
    TOKEN_IDENTIFIER,    /* variable / function / type names */
    TOKEN_INT_CONST, /* e.g., 123, 0x1A, 077 */
    TOKEN_FLOAT_CONST,   /* e.g., 3.14, 0.5e-2, .5F */
    TOKEN_CHAR_CONST,    /* e.g., 'a', '\n' */
    TOKEN_STRING_LITERAL,/* e.g., "hello" */

    /* Single‑character operators and punctuation */
    TOKEN_LPAREN,        /* ( */
    TOKEN_RPAREN,        /* ) */
    TOKEN_LBRACKET,      /* [ */
    TOKEN_RBRACKET,      /* ] */
    TOKEN_LBRACE,        /* { */
    TOKEN_RBRACE,        /* } */
    TOKEN_SEMICOLON,     /* ; */
    TOKEN_COMMA,         /* , */
    TOKEN_DOT,           /* . */
    TOKEN_AMPERSAND,     /* & */
    TOKEN_ASTERISK,      /* * */
    TOKEN_PLUS,          /* + */
    TOKEN_MINUS,         /* - */
    TOKEN_TILDE,         /* ~ */
    TOKEN_EXCLAM,        /* ! */
    TOKEN_SLASH,         /* / */
    TOKEN_PERCENT,       /* % */
    TOKEN_LESS,          /* < */
    TOKEN_GREATER,       /* > */
    TOKEN_HAT,           /* ^ */
    TOKEN_PIPE,          /* | */
    TOKEN_QUESTION,      /* ? */
    TOKEN_COLON,         /* : */
    TOKEN_EQUALS,        /* = */

    /* Multi‑character operators */
    TOKEN_ELLIPSIS,      /* ... */
    TOKEN_ARROW,         /* -> */
    TOKEN_INCREMENT,     /* ++ */
    TOKEN_DECREMENT,     /* -- */
    TOKEN_LEFT_SHIFT,    /* << */
    TOKEN_RIGHT_SHIFT,   /* >> */
    TOKEN_LE_OP,         /* <= */
    TOKEN_GE_OP,         /* >= */
    TOKEN_EQ_OP,         /* == */
    TOKEN_NE_OP,         /* != */
    TOKEN_AND_OP,        /* && */
    TOKEN_OR_OP,         /* || */
    TOKEN_PLUS_EQUAL,    // += 
    TOKEN_MINUS_EQUAL,   // -=
    TOKEN_STAR_EQUAL,    //  *=
    TOKEN_SLASH_EQUAL,   // /=

    TOKEN_EOF,           /* End of File / Input */
    TOKEN_ERROR          /* For handling lexical errors safely */
} TokenType;


typedef struct {
    TokenType type;
    const char* start;    
    int length; 
    int line;
} Token;


typedef struct {
    const char* source;
    int length;
    int cursor;
    int line;
    Token current_token;
} Lexer;


void lexer_init(Lexer* lex, const char* source, int len);


Token* next_token(Lexer* lex);

#endif


