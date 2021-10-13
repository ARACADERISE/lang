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
    ax_reg=0xAA,
    bx_reg=0xAB,
    cx_reg=0xAC,
    dx_reg=0xAD,
    es_reg=0xAE,
    si_reg=0xAF,
    di_reg=0xBA,
    ss_reg=0xBB,
    sp_reg=0xBC,
    bp_reg=0xBD,
    ah_reg=0xBE,
    al_reg=0xBF,
    bh_reg=0xCA,
    bl_reg=0xCB,
    ch_reg=0xCC,
    cl_reg=0xCD,
    dh_reg=0xCE,
    dl_reg=0xCF,
    ds_reg=0xDA,
    cs_reg=0xDB,
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
  u8 parent_reg;
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