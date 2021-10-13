#include "lexer.h"

Lexer *init_lexer(char *data)
{
  Lexer *l = calloc(1,sizeof(*l));

  l->curr_token = (void*)0;
  l->data = data;
  l->line = 1;
  l->index = 0;
  l->c = l->data[l->index];
  
  return l;
}

static inline void advance_lexer(Lexer *l)
{
  if(!(l->index >= strlen(l->data)))
  {
    l->index++;
    l->c = l->data[l->index];
  }
}

static inline Token* init_token(int tt, char *val, Lexer *l, u8 is_preg)
{
  Token *t = calloc(1, sizeof(*t));

  t->TT = tt;
  t->tv = val;
  l->curr_token = t;
  t->parent_reg = is_preg;

  advance_lexer(l);
  return t;
}

char *get_s(Lexer *l)
{
  char *s = calloc(1, sizeof(*s));
  int index = 0;

  while(isAscii(l->c) == 0 && !(l->c == '\0'))
  {
    s[index] = l->c;
    s = realloc(
      s,
      (strlen(s) + 1) * sizeof(*s)
    );
    index++;
    advance_lexer(l);
  }
  l->index--;
  return s;
}

static inline void skip_whitespace(Lexer *l)
{
  do {
    advance_lexer(l);
  } while(l->c == ' ' || l->c == '\n');
}

Token *get_value(Lexer *l)
{
  char *val = calloc(1, sizeof(*val));
  int index = 0;

  do {
    val[index] = l->c;
    index++;
    val = realloc(
      val,
      (strlen(val) + 1) * sizeof(*val)
    );
    advance_lexer(l);
  } while(l->c != ' ' && l->c != '\n' && l->c != '\0');

  return init_token(number, val, l, 1);
}

void skip_comment(Lexer *l)
{
  advance_lexer(l);

  while(l->c != '\n' && !(l->c == '\0'))
  {
    advance_lexer(l);
  }
  //advance_lexer(l);
}

char *get_number(Lexer *l)
{
  char *n = calloc(1, sizeof(*n));
  int index = 0;

  while(!(l->c == '\n') && !(l->c == '\0'))
  {
    n[index] = l->c;
    index++;

    n = realloc(
      n,
      (strlen(n)+1)*sizeof(*n)
    );
    advance_lexer(l);
  }

  return n;
}

char *to_str(char c)
{
  char *val = calloc(2, sizeof(char));

  val[0] = c;
  val[1] = '\0';
  
  return val;
}

char *gather_db_val(Lexer *lex)
{
  char *val = calloc(1, sizeof(*val));
  int index = 0;

  while(!(lex->c == '\''))
  {
    val[index] = lex->c;
    index++;

    val = realloc(
      val,
      (strlen(val) + 1) * sizeof(*val)
    );
    advance_lexer(lex);
  }

  return val;
}

Token *gen_token(Lexer *lex)
{

  while(lex->c != '\0' && !(lex->index >= strlen(lex->data)))
  {
redo:
    if(lex->c == ' ')
    {
      skip_whitespace(lex);
      goto redo;
    }
    if(lex->c == '\n')
    {
        lex->line++;
        while(lex->c == '\n')
        {
            advance_lexer(lex);
            lex->line++;
        }
    }

    switch(lex->c)
    {
      case ';': skip_comment(lex);goto redo;
      case '{': return init_token(LC, to_str(lex->c), lex,1);break;
      case '}': return init_token(RC, to_str(lex->c), lex,1);break;
      case ',': return init_token(comma,to_str(lex->c),lex,1);break;
      default:break;
    }

    if(lex->c == '\'')
    {
      advance_lexer(lex);
      char *db_v;

      if(isAscii(lex->c) == 0)
      {
        db_v = gather_db_val(lex);
      
        return init_token(db_val, db_v, lex,1);
      }
      
    }

    if(isAscii(lex->c)==0)
    {
      char *s = get_s(lex);
      if(strcmp(s, "use")==0) return init_token(use_k, s, lex,1);
      if(strcmp(s, "NULL")==0) return init_token(null_k, s, lex,1);
      if(strcmp(s, "org")==0) return init_token(org_k, s, lex,1);
      if(strcmp(s, "set")==0) return init_token(set_k, s, lex,1);
      if(strcmp(s, "sect")==0) return init_token(sect_k, s, lex,1);
      if(strcmp(s, "data")==0) return init_token(sect_data, s, lex,1);
      if(strcmp(s, "mov")==0) return init_token(mov_k, s, lex,1);
      if(strcmp(s, "ax")==0) return init_token(ax_reg, s, lex,0);
      if(strcmp(s, "ah")==0) return init_token(ah_reg, s, lex,1);
      if(strcmp(s, "al")==0) return init_token(al_reg, s, lex,1);
      if(strcmp(s, "bx")==0) return init_token(bx_reg, s, lex,0);
      if(strcmp(s, "bh")==0) return init_token(bh_reg, s, lex,1);
      if(strcmp(s, "bl")==0) return init_token(bl_reg, s, lex,1);
      if(strcmp(s, "cx")==0) return init_token(cx_reg, s, lex,0);
      if(strcmp(s, "ch")==0) return init_token(ch_reg, s, lex,1);
      if(strcmp(s, "cl")==0) return init_token(cl_reg, s, lex,1);
      if(strcmp(s, "dx")==0) return init_token(dx_reg, s, lex,0);
      if(strcmp(s, "dh")==0) return init_token(dh_reg, s, lex,1);
      if(strcmp(s, "dl")==0) return init_token(dh_reg, s, lex,1);
      if(strcmp(s, "si")==0) return init_token(si_reg, s, lex,0);
      if(strcmp(s, "di")==0) return init_token(di_reg, s, lex,0);
      if(strcmp(s, "es")==0) return init_token(es_reg, s, lex,0);
      if(strcmp(s, "ss")==0) return init_token(ss_reg, s, lex,0);
      if(strcmp(s, "sp")==0) return init_token(sp_reg, s, lex,0);
      if(strcmp(s, "bp")==0) return init_token(bp_reg, s, lex,0);
      if(strcmp(s, "ds")==0) return init_token(ds_reg, s, lex,0);
      if(strcmp(s, "cs")==0) return init_token(cs_reg, s, lex,0);
      if(strcmp(s, "db")==0) return init_token(db_k, s, lex,1);
      if(strcmp(s, "dw")==0) return init_token(dw_k, s, lex,1);
      if(strcmp(s, "dd")==0) return init_token(dd_k, s, lex,1);
      if(strcmp(s, "dq")==0) return init_token(dq_k, s, lex,1);

      return init_token(string, s, lex,1);
    }

    if(isNumber(lex->c)==0)
    {
      char *s = get_number(lex);
      return init_token(number, s, lex,1);
    }

    
    advance_lexer(lex);
  }

  return init_token(eof, "\0", lex,1);
}