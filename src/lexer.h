#pragma once
#include "util.h"

typedef struct token
{
  enum {
    LB = 0x1, // [
    RB, // ]
    LC, // {
    RC, // }
    dot, // .
    comma, // ,
    colon, // :
    ax_reg,
    bx_reg,
    cx_reg,
    dx_reg,
    es_reg,
    si_reg,
    di_reg,
    ss_reg,
    sp_reg,
    bp_reg,
    ah_reg,
    al_reg,
    bh_reg,
    bl_reg,
    ch_reg,
    cl_reg,
    dh_reg,
    dl_reg,
    ds_reg,
    cs_reg,
    db_k, // 1 byte
    dw_k, // 2 byte
    dd_k, // 4 byte
    dq_k, // 8 byte
    equ_k,
    db_val,
    integer_val,
    sect_k, // sect
    null_k, // null
    sect_data, // data
    set_k,  // set
    mov_k,  // mov
    use_k,    // use
    org_k,    // org
    block_k,  // block
    number,   // 1234
    string,   // "hey"
    eof       // \0
  } TT;
  char *tv;
} Token;

typedef struct lexer
{
  char *data;
  int index;
  int line;
  char c;
  struct token *curr_token;
} Lexer;

Lexer *init_lexer(char *data);
Token *gen_token(Lexer *lex);