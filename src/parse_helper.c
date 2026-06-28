#include "include/parse_helper.h"

CstKind cst_kind_for_keywords(TokenType type) {
    switch (type) {
        case TOKEN_AUTO:     return CST_AUTO;
        case TOKEN_BREAK:    return CST_BREAK;
        case TOKEN_CASE:     return CST_CASE;
        case TOKEN_CHAR:     return CST_CHAR;
        case TOKEN_CONST:    return CST_CONST;
        case TOKEN_CONTINUE: return CST_CONTINUE;
        case TOKEN_DEFAULT:  return CST_DEFAULT;
        case TOKEN_DO:       return CST_DO;
        case TOKEN_DOUBLE:   return CST_DOUBLE;
        case TOKEN_ELSE:     return CST_ELSE;
        case TOKEN_ENUM:     return CST_ENUM;
        case TOKEN_EXTERN:   return CST_EXTERN;
        case TOKEN_FLOAT:    return CST_FLOAT;
        case TOKEN_FOR:      return CST_FOR;
        case TOKEN_GOTO:     return CST_GOTO;
        case TOKEN_IF:       return CST_IF;
        case TOKEN_INT:      return CST_INT;
        case TOKEN_LONG:     return CST_LONG;
        case TOKEN_REGISTER: return CST_REGISTER;
        case TOKEN_RETURN:   return CST_RETURN;
        case TOKEN_SHORT:    return CST_SHORT;
        case TOKEN_SIGNED:   return CST_SIGNED;
        case TOKEN_SIZEOF:   return CST_SIZEOF;
        case TOKEN_STATIC:   return CST_STATIC;
        case TOKEN_STRUCT:   return CST_STRUCT;
        case TOKEN_SWITCH:   return CST_SWITCH;
        case TOKEN_TYPEDEF:  return CST_TYPEDEF;
        case TOKEN_UNION:    return CST_UNION;
        case TOKEN_UNSIGNED: return CST_UNSIGNED;
        case TOKEN_VOID:     return CST_VOID;
        case TOKEN_VOLATILE: return CST_VOLATILE;
        case TOKEN_WHILE:    return CST_WHILE;
        case TOKEN_INLINE:   return  CST_INLINE;
        default:             return CST_ERROR; // should not happen
    }
}
CstKind cst_kind_token_match(TokenType type) {
    switch (type) {
        case TOKEN_IDENTIFIER:     return CST_IDENTIFIER;
        case TOKEN_INT_CONST:  return CST_INT_CONST;
        case TOKEN_FLOAT_CONST:    return CST_FLOAT_CONST;
        case TOKEN_CHAR_CONST:     return CST_CHAR_CONST;
        case TOKEN_STRING_LITERAL: return CST_STRING_LITERAL;

        case TOKEN_LPAREN:         return CST_LPAREN;
        case TOKEN_RPAREN:         return CST_RPAREN;
        case TOKEN_LBRACKET:       return CST_LBRACKET;
        case TOKEN_RBRACKET:       return CST_RBRACKET;
        case TOKEN_LBRACE:         return CST_LBRACE;
        case TOKEN_RBRACE:         return CST_RBRACE;
        case TOKEN_SEMICOLON:      return CST_SEMICOLON;
        case TOKEN_COMMA:          return CST_COMMA;
        case TOKEN_DOT:            return CST_DOT;
        case TOKEN_AMPERSAND:      return CST_AMPERSAND;
        case TOKEN_ASTERISK:       return CST_ASTERISK;
        case TOKEN_PLUS:           return CST_PLUS;
        case TOKEN_MINUS:          return CST_MINUS;
        case TOKEN_TILDE:          return CST_TILDE;
        case TOKEN_EXCLAM:         return CST_EXCLAM;
        case TOKEN_SLASH:          return CST_SLASH;
        case TOKEN_PERCENT:        return CST_PERCENT;
        case TOKEN_LESS:           return CST_LESS;
        case TOKEN_GREATER:        return CST_GREATER;
        case TOKEN_HAT:            return CST_HAT;
        case TOKEN_PIPE:           return CST_PIPE;
        case TOKEN_QUESTION:       return CST_QUESTION;
        case TOKEN_COLON:          return CST_COLON;
        case TOKEN_EQUALS:         return CST_EQUALS;

        case TOKEN_ELLIPSIS:       return CST_ELLIPSIS;
        case TOKEN_ARROW:          return CST_ARROW;
        case TOKEN_INCREMENT:      return CST_INCREMENT;
        case TOKEN_DECREMENT:      return CST_DECREMENT;
        case TOKEN_LEFT_SHIFT:     return CST_LEFT_SHIFT;
        case TOKEN_RIGHT_SHIFT:    return CST_RIGHT_SHIFT;
        case TOKEN_LE_OP:          return CST_LE_OP;
        case TOKEN_GE_OP:          return CST_GE_OP;
        case TOKEN_EQ_OP:          return CST_EQ_OP;
        case TOKEN_NE_OP:          return CST_NE_OP;
        case TOKEN_AND_OP:         return CST_AND_OP;
        case TOKEN_OR_OP:          return CST_OR_OP;
        case TOKEN_PLUS_EQUAL:     return CST_PLUS_EQUAL;
        case TOKEN_STAR_EQUAL:     return CST_STAR_EQUAL;
        case TOKEN_SLASH_EQUAL:    return CST_SLASH_EQUAL;
        case TOKEN_MINUS_EQUAL:    return CST_MINUS_EQUAL;

        case TOKEN_EOF:            return CST_EOF;
        case TOKEN_ERROR:          return CST_ERROR;

        default:                   return CST_ERROR;
    }
}

bool is_specifier_keyword(TokenType type) {
    switch (type) {
        /* Storage class */
        case TOKEN_AUTO:
        case TOKEN_REGISTER:
        case TOKEN_STATIC:
        case TOKEN_EXTERN:
        case TOKEN_TYPEDEF:
        /* Type specifiers */
        case TOKEN_VOID:
        case TOKEN_CHAR:
        case TOKEN_SHORT:
        case TOKEN_INT:
        case TOKEN_LONG:
        case TOKEN_FLOAT:
        case TOKEN_DOUBLE:
        case TOKEN_SIGNED:
        case TOKEN_UNSIGNED:
        case TOKEN_STRUCT:
        case TOKEN_UNION:
        case TOKEN_ENUM:
        /* Type qualifiers */
        case TOKEN_CONST:
        case TOKEN_VOLATILE:
        // function specifier
        case TOKEN_INLINE:
            return true;
        default:
            return false;
    }
}