#ifndef  PRATT_H
#define PRATT_H
#include "lexer.h"

typedef enum{
    BP_NONE = 0,
    BP_ASSIGN = 1,
    BP_TERNARY = 2,
    BP_OR = 3,
    BP_AND = 4,
    BP_BIT_OR = 5,
    BP_BIT_XOR = 6,
    BP_BIT_AND = 7,
    BP_EQ = 8,
    BP_REL = 9,
    BP_SHIFT = 10,
    BP_ADD = 11,
    BP_MUL = 12,
    BP_UNARY = 13,
    BP_POSTFIX = 14
}Bp;

int get_lbp(TokenType t);


#endif