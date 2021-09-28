#include "parser.h"

void start(Parser *p);
Parser *parse_use(Parser *p);

static PRegisterOutline p_regs[] = {
  [ax] = {
    .name = "ax",
    .r_val = 0,
    .is_common = true,
    .type = accumulator
  },
  [bx] = {
    .name = "bx",
    .r_val = 0,
    .is_common = true,
    .type = base_reg
  },
  [cx] = {
    .name = "cx",
    .r_val = 0,
    .is_common = true,
    .type = count_reg
  },
  [dx] = {
    .name = "dx",
    .r_val = 0,
    .is_common = true,
    .type = data_reg
  },
  [es] = {
    .name = "dx",
    .r_val = 0,
    .is_common = false,
    .type = extra_seg
  },
  [si] = {
    .name = "si",
    .r_val = 0,
    .is_common = false,
    .type = source_index
  },
  [di] = {
    .name = "di",
    .r_val = 0,
    .is_common = false,
    .type = dest_index
  },
  [ds] = {
    .name = "ds",
    .r_val = 0,
    .is_common = false,
    .type = data_seg
  },
  [cs] = {
    .name = "cs",
    .r_val = 0,
    .is_common = false,
    .type = code_seg
  },
  [ss] = {
    .name = "ss",
    .r_val = 0,
    .is_common = false,
    .type = stack_seg
  }
};

static CRegisterOutline c_regs[] = {
  [ah] = {
    .name = "ah",
    .r_val = 0,
  },
  [al] = {
    .name = "al",
    .r_val = 0
  },
  [bh] = {
    .name = "bh",
    .r_val = 0,
  },
  [bl] = {
    .name = "bl",
    .r_val = 0
  },
  [ch] = {
    .name = "ch",
    .r_val = 0,
  },
  [cl] = {
    .name = "cl",
    .r_val = 0
  },
  [dh] = {
    .name = "dh",
    .r_val = 0,
  },
  [dl] = {
    .name = "dl",
    .r_val = 0
  },
};

static const RCorrespond reg_relations[] = {
  [ax] = {
    .access = {bx, cx, dx, es, si, di}
  },
  [ah] = {
    .access = {bh, ch, dh}
  },
  [al] = {
    .access = {bl, cl, dl}
  },
  [bx] = {
    .access = {ax, cx, dx, es, si, di}
  },
  [bh] = {
    .access = {ah, ch, dh}
  },
  [bl] = {
    .access = {al, cl, dl}
  },
  [cx] = {
    .access = {ax, bx, dx, es, si, di}
  },
  [ch] = {
    .access = {ah, bh, dh}
  },
  [cl] = {
    .access = {al, bl, dl}
  },
  [dx] = {
    .access = {ax, bx, cx, es, si, di}
  },
  [dh] = {
    .access = {ah, bh, ch}
  },
  [dl] = {
    .access = {al, bl, cl}
  },
  [si] = {
    .access = {ax, bx, cx, dx, es, di}
  },
  [di] = {
    .access = {ax, bx, cx, dx, es, si}
  },
  [es] = {
    .access = {ax, bx, cx, dx, si, di}
  },
  [ds] = {
    .access = {ax, bx, cx, dx, si, di}
  },
  [cs] = {
    .access = {ax, bx, cx, dx, si, di}
  }
};

Parser *init_parser(Lexer *lex)
{
  Parser *p = calloc(1, sizeof(*p));
  mo = calloc(1, sizeof(*mo));

  p->lex = lex;
  start(p);

  return (void*)0;
}

static inline void next_token(Parser *p)
{
  free(p->lex->curr_token);
  gen_token(p->lex);
}

Parser *parse_use(Parser *p)
{
  next_token(p);
  dbg(p->lex->curr_token->TT == number, "Expected number");
  mo->using_ = atoi(p->lex->curr_token->tv);
  mo->mem.bin = calloc(mo->using_, sizeof(*mo->mem.bin));
  mo->origin = defOrg;
  mo->program_pointer = mo->origin;
  mo->stack_p = 1;
  mo->base_p = 0;

  // Make sure we have one opening for the stack.
  mo->mem.stack = calloc(1, sizeof(*mo->mem.stack));

  memset(mo->mem.bin, 0, mo->using_);

  return p;
}

Parser *parse_org(Parser* p)
{
  dbg(mo->using_ > 0, "No memory in use.");

  next_token(p);
  if(atoi(p->lex->curr_token->tv) >= mo->using_-10)
  {
    err("Cannot originate program at, or above, bytes being used.");
  }
  dbg(p->lex->curr_token->TT == number, "Expected number");
  
  mo->origin = atoi(p->lex->curr_token->tv);
  mo->program_pointer = mo->origin;
  mo->base_p = mo->origin;

  return p;

}

Parser *parse_set(Parser *p)
{

  gen_token(p->lex);

  switch(p->lex->curr_token->TT)
  {
    case ax_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);

      dbg(atoi(p->lex->curr_token->tv) >= general_max, "Overflow Error");

      p_regs[ax].r_val = atoi(p->lex->curr_token->tv);
      break;
    }
    case ah_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= child_max, "Overflow Error");

      p_regs[ah].r_val = atoi(p->lex->curr_token->tv);
      break;
    }
    case al_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= child_max, "Overflow Error");

      p_regs[al].r_val = atoi(p->lex->curr_token->tv);
      break;
    }
    case bx_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= general_max, "Overflow Error");

      p_regs[bx].r_val = atoi(p->lex->curr_token->tv);
      break;
    }
    case bh_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= child_max, "Overflow Error");

      p_regs[bh].r_val = atoi(p->lex->curr_token->tv);
      break;
    }
    case bl_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= child_max, "Overflow Error");

      p_regs[bl].r_val = atoi(p->lex->curr_token->tv);
      break;
    }
    case cx_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= general_max, "Overflow Error");

      p_regs[cx].r_val = atoi(p->lex->curr_token->tv);
      break;
    }
    case ch_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= child_max, "Overflow Error");

      p_regs[ch].r_val = atoi(p->lex->curr_token->tv);
      break;
    }
    case cl_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= child_max, "Overflow Error");

      p_regs[cl].r_val = atoi(p->lex->curr_token->tv);
      break;
    }
    case dx_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= general_max, "Overflow Error");

      p_regs[dx].r_val = atoi(p->lex->curr_token->tv);
      break;
    }
    case dh_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= child_max, "Overflow Error");

      p_regs[dh].r_val = atoi(p->lex->curr_token->tv);
      break;
    }
    case dl_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= child_max, "Overflow Error");

      p_regs[dl].r_val = atoi(p->lex->curr_token->tv);
      break;
    }
    case es_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= general_max, "Overflow Error");

      p_regs[es].r_val = atoi(p->lex->curr_token->tv);
      break;
    }
    case si_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= general_max, "Overflow Error");

      p_regs[si].r_val = atoi(p->lex->curr_token->tv);
      break;
    }
    case di_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= general_max, "Overflow Error");

      p_regs[di].r_val = atoi(p->lex->curr_token->tv);
      break;
    }
    case ds_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= general_max, "Overflow Error");

      p_regs[ds].r_val = atoi(p->lex->curr_token->tv);
      break;
    }
    case cs_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= general_max, "Overflow Error");

      p_regs[cs].r_val = atoi(p->lex->curr_token->tv);
      break;
    }
    case bp_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);

      if(atoi(p->lex->curr_token->tv) >= mo->stack_p)
      {
        int last_val = mo->stack_p;
        mo->stack_p = atoi(p->lex->curr_token->tv) + 100;
        mo->mem.stack = realloc(
          mo->mem.stack,
          mo->stack_p * sizeof(*mo->mem.stack)
        );
        reallocStack(mo->mem.stack, last_val, mo->stack_p);
      }

      p_regs[bp].r_val = atoi(p->lex->curr_token->tv);
      mo->base_p = p_regs[bp].r_val;

      mo->base_p_val = mo->mem.stack[mo->base_p];
      break;
    }
    case sp_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);

      int last_val = mo->stack_p;
      p_regs[sp].r_val = atoi(p->lex->curr_token->tv);
      mo->stack_p = p_regs[sp].r_val;
      mo->mem.stack = realloc(
        mo->mem.stack,
        mo->stack_p * sizeof(*mo->mem.stack)
      );
      reallocStack(mo->mem.stack, last_val, mo->stack_p);

      // Make sure the bp register is zero
      mo->base_p = 0;
      mo->stack_p_val = mo->mem.stack[mo->stack_p];
      break;
    }
    default: {
      err("Unknown register %s", p->lex->curr_token->tv);
    }
  }

  return p;
}

Parser *parse_sect(Parser *p)
{
  gen_token(p->lex);

  switch(p->lex->curr_token->TT)
  {
    case sect_data:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == LC, "Expected `{`");
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT != RC, "Null data sections requires `null` specifier.");

      if(p->lex->curr_token->TT == null_k)
      {
        gen_token(p->lex);
        goto data_sect_end;
      }
      
      switch(p->lex->curr_token->TT)
      {
        case string:
        {
          mo->mem.data_sect = calloc(1, sizeof(*mo->mem.data_sect));
          mo->mem.data_sect[0] = calloc(1, sizeof(*mo->mem.data_sect[0]));

          mo->mem.data_sect[0]->name = p->lex->curr_token->tv;
          
          gen_token(p->lex);
          
          switch(p->lex->curr_token->TT)
          {
            case db_k:
            {
              mo->mem.data_sect[0]->var_type = db;
              mo->mem.data_sect[0]->db_data = calloc(1, sizeof(char));
              mo->mem.data_sect[0]->db_data[0] = 'a';

              break;
            }
            default:
            {
              err("Unknown type.");
            }
          }
          
          break;
        }
        default:
        {
          err("Unknown");
        }
      }
      
      data_sect_end:
      gen_token(p->lex);
      break;
    }
    default: {
      err("Unknown section: %s", p->lex->curr_token->tv);
    }
  }

  return p;
}

void start(Parser *p)
{
  do {
    gen_token(p->lex);
    
    switch(p->lex->curr_token->TT)
    {
      case use_k: parse_use(p);break;
      case org_k: parse_org(p);break;
      case set_k: parse_set(p);break;
      case sect_k: parse_sect(p);break;
      case eof: goto end;
      default: break;
    }
  } while(p->lex->curr_token->TT != eof);

end:

  free(p->lex->curr_token);
  free(p->lex);
  free(p);
}