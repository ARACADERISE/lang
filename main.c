#include <stdio.h>
#include "src/read_file.h"
#include "src/lexer.h"
#include "src/parser.h"

int main(int args, char **argv)
{
  dbg(args > 1, "Expect file as argument\n")

  char *d = read_file(argv[1]);
  Lexer *l = init_lexer(d);
  Parser *p = init_parser(l);
}