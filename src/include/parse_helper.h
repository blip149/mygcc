#ifndef PARSE_HELPER_H
#define PARSE_HELPER_H
#include "cst.h"
#include "lexer.h"
#include <stdbool.h>

CstKind cst_kind_for_keywords(TokenType type);
CstKind cst_kind_token_match(TokenType type);
bool is_specifier_keyword(TokenType type);

#endif