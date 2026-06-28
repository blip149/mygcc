#include "include/pratt.h"


int get_lbp(TokenType t) {
    switch(t) {
        case TOKEN_EQUALS:
        case TOKEN_PLUS_EQUAL:
        case TOKEN_MINUS_EQUAL:
        case TOKEN_STAR_EQUAL:
        case TOKEN_SLASH_EQUAL:   return BP_ASSIGN;

        case TOKEN_OR_OP:         return BP_OR;
        case TOKEN_AND_OP:        return BP_AND;
        case TOKEN_PIPE:          return BP_BIT_OR;
        case TOKEN_HAT:           return BP_BIT_XOR;
        case TOKEN_AMPERSAND:     return BP_BIT_AND;

        case TOKEN_EQ_OP:   
        case TOKEN_NE_OP:         return BP_EQ;

        case TOKEN_GE_OP:
        case TOKEN_LE_OP:
        case TOKEN_LESS:       
        case TOKEN_GREATER:        return BP_REL;

        case TOKEN_LEFT_SHIFT:
        case TOKEN_RIGHT_SHIFT:     return BP_SHIFT;

        case TOKEN_PLUS:
        case TOKEN_MINUS:            return BP_ADD;

        case TOKEN_ASTERISK:
        case TOKEN_SLASH:
        case TOKEN_PERCENT:           return BP_MUL;

        case TOKEN_INCREMENT:
        case TOKEN_LPAREN:
        case TOKEN_LBRACKET:
        case TOKEN_DOT:
        case TOKEN_ARROW:
        case TOKEN_DECREMENT:          return BP_POSTFIX;

        default:  return BP_NONE;

    }
}