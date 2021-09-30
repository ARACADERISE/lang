#include "parser.h"

void start(Parser *p);
Parser *parse_use(Parser *p);

static PRegisterOutline p_regs[] = {
  [ax] = {
    .name = "ax",
    .high_bit = 0,
    .low_bit = 0,
    .is_common = true,
    .type = accumulator
  },
  [bx] = {
    .name = "bx",
    .high_bit = 0,
    .low_bit = 0,
    .is_common = true,
    .type = base_reg
  },
  [cx] = {
    .name = "cx",
    .high_bit = 0,     
    .low_bit = 0,
    .is_common = true,
    .type = count_reg
  },
  [dx] = {
    .name = "dx",
    .high_bit = 0,
    .low_bit = 0,
    .is_common = true,
    .type = data_reg
  },
  [es] = {
    .name = "dx",
    .high_bit = 0,
    .low_bit = 0,
    .is_common = false,
    .type = extra_seg
  },
  [si] = {
    .name = "si",
    .high_bit = 0, 
    .low_bit = 0,
    .is_common = false,
    .type = source_index
  },
  [di] = {
    .name = "di",
    .high_bit = 0,
    .low_bit = 0,
    .is_common = false,
    .type = dest_index
  },
  [ds] = {
    .name = "ds",
    .high_bit = 0,
    .low_bit = 0,
    .is_common = false,
    .type = data_seg
  },
  [cs] = {
    .name = "cs",
    .high_bit = 0,
    .low_bit = 0,
    .is_common = false,
    .type = code_seg
  },
  [ss] = {
    .name = "ss",
    .high_bit = 0,
    .low_bit = 0,
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

      p_regs[ax].parent_val = atoi(p->lex->curr_token->tv);
      assign_bits(&p_regs[ax], p_regs[ax].parent_val);

      break;
    }
    case ah_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= child_max, "Overflow Error");

      p_regs[ah].high_bit = atoi(p->lex->curr_token->tv);
      break;
    }
    case al_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= child_max, "Overflow Error");

      p_regs[al].low_bit = atoi(p->lex->curr_token->tv);
      break;
    }
    case bx_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= general_max, "Overflow Error");

      p_regs[bx].parent_val = atoi(p->lex->curr_token->tv);
      assign_bits(&p_regs[bx], p_regs[bx].parent_val);
      
      break;
    }
    case bh_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= child_max, "Overflow Error");

      p_regs[bh].high_bit = atoi(p->lex->curr_token->tv);
      break;
    }
    case bl_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= child_max, "Overflow Error");

      p_regs[bl].low_bit = atoi(p->lex->curr_token->tv);
      break;
    }
    case cx_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= general_max, "Overflow Error");

      p_regs[cx].parent_val = atoi(p->lex->curr_token->tv);
      assign_bits(&p_regs[cx], p_regs[cx].parent_val);

      break;
    }
    case ch_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= child_max, "Overflow Error");

      p_regs[ch].high_bit = atoi(p->lex->curr_token->tv);
      break;
    }
    case cl_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= child_max, "Overflow Error");

      p_regs[cl].low_bit = atoi(p->lex->curr_token->tv);
      break;
    }
    case dx_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= general_max, "Overflow Error");

      p_regs[dx].parent_val = atoi(p->lex->curr_token->tv);
      assign_bits(&p_regs[dx], p_regs[dx].parent_val);

      break;
    }
    case dh_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= child_max, "Overflow Error");

      p_regs[dh].high_bit = atoi(p->lex->curr_token->tv);
      break;
    }
    case dl_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= child_max, "Overflow Error");

      p_regs[dl].low_bit = atoi(p->lex->curr_token->tv);
      break;
    }
    case es_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= general_max, "Overflow Error");

      p_regs[es].parent_val = atoi(p->lex->curr_token->tv);
      assign_bits(&p_regs[es], p_regs[es].parent_val);

      break;
    }
    case si_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= general_max, "Overflow Error");

      p_regs[si].parent_val = atoi(p->lex->curr_token->tv);
      assign_bits(&p_regs[si], p_regs[si].parent_val);
      break;
    }
    case di_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= general_max, "Overflow Error");

      p_regs[di].parent_val = atoi(p->lex->curr_token->tv);
      assign_bits(&p_regs[di], p_regs[di].parent_val);
      break;
    }
    case ds_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= general_max, "Overflow Error");

      p_regs[ds].parent_val = atoi(p->lex->curr_token->tv);
      assign_bits(&p_regs[ds], p_regs[ds].parent_val);
      break;
    }
    case cs_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      dbg(atoi(p->lex->curr_token->tv) >= general_max, "Overflow Error");

      p_regs[cs].parent_val = atoi(p->lex->curr_token->tv);
      assign_bits(&p_regs[cs], p_regs[cs].parent_val);
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

      mo->base_p = atoi(p->lex->curr_token->tv);

      mo->base_p_val = mo->mem.stack[mo->base_p];
      break;
    }
    case sp_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);

      int last_val = mo->stack_p;
      mo->stack_p = atoi(p->lex->curr_token->tv);

      mo->mem.stack = realloc(
        mo->mem.stack,
        mo->stack_p * sizeof(*mo->mem.stack)
      );
      reallocStack(mo->mem.stack, last_val, mo->stack_p);

      // Make sure the bp register is zero
      if(last_val == 0)
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
      
      recheck:
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
              
              set_void(mo->mem.data_sect[0]->var_type, mo);

              gen_token(p->lex);
              assign(p->lex->curr_token->tv, mo->mem.data_sect[0]->db_data, sizeof(char));

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

      if(!(p->lex->curr_token->TT == RC))
        goto recheck;
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