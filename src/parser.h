#pragma once
#include "util.h"
#include "lexer.h"

typedef struct parser
{
  Lexer *lex;
  MovInstruction *mi;
} Parser;

Parser *init_parser(Lexer *lex);