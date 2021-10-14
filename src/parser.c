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
  p->mi = calloc(1, sizeof(*p->mi));
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
  dbg(p->lex->curr_token->TT == number, "%sError Report:\n\t%sLine: %d\n\tReport:Expected number", RED, LRED, p->lex->line);

  int num = try_conv_from_hex(p->lex->curr_token->tv, data_max);
  mo->using_ = num;

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
  dbg(mo->using_ > 0, "%sError Report:\n\t%sLine: %d\n\tReport:No memory in use.", RED, LRED, p->lex->line);

  next_token(p);
  if(atoi(p->lex->curr_token->tv) >= mo->using_-10)
  {
    err("%sError Report:\n\t%sLine: %d\n\tReport:Cannot originate program at, or above, bytes being used.", RED, LRED, p->lex->line);
  }

  dbg(p->lex->curr_token->TT == number, "%sError Report:\n\t%sLine: %d\n\tReport:Expected number", RED, LRED, p->lex->line);
  int number = try_conv_from_hex(p->lex->curr_token->tv, data_max);

  mo->origin = number;
  mo->program_pointer = mo->origin;
  mo->base_p = mo->origin;

  return p;
}

Parser *parse_set(Parser *p)
{

  gen_token(p->lex);
  unsigned short number;

  switch(p->lex->curr_token->TT)
  {
    case ax_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "%sError Report:\n\t%sLine: %d\n\tReport:Expected comma", RED, LRED, p->lex->line);

      gen_token(p->lex);
      
      number = try_conv_from_hex(p->lex->curr_token->tv, general_max);

      dbg(number <= general_max, "%sError Report:\n\t%sLine: %d\n\tReport:Overflow Error", RED, LRED, p->lex->line);

      assign_bits(&p_regs[ax], number,(CRegisterOutline[]){crego[ah], crego[al]});

      break;
    }
    case ah_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "%sError Report:\n\t%sLine: %d\n\tReport:Expected comma", RED, LRED, p->lex->line);

      gen_token(p->lex);
      number = try_conv_from_hex(p->lex->curr_token->tv, child_max);

      dbg(number <= child_max, "%sError Report:\n\t%sLine: %d\n\tReport:Overflow Error", RED, LRED, p->lex->line);

      p_regs[ah].high_bit = (unsigned char)number;
      crego[ah].r_val = p_regs[ah].high_bit;
      break;
    }
    case al_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "%sError Report:\n\t%sLine: %d\n\tReport:Expected comma", RED, LRED, p->lex->line);

      gen_token(p->lex);
      number = try_conv_from_hex(p->lex->curr_token->tv, general_max);

      dbg(number <= child_max, "%sError Report:\n\t%sLine: %d\n\tReport:Overflow Error", RED, LRED, p->lex->line);

      p_regs[al].low_bit = (unsigned char)number;
      crego[al].r_val = p_regs[al].low_bit;
      break;
    }
    case bx_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "%sError Report:\n\t%sLine: %d\n\tReport:Expected comma", RED, LRED, p->lex->line);

      gen_token(p->lex);
      number = try_conv_from_hex(p->lex->curr_token->tv, general_max);

      dbg(number <= general_max, "%sError Report:\n\t%sLine: %d\n\tReport:Overflow Error", RED, LRED, p->lex->line);

      assign_bits(&p_regs[bx], number, (CRegisterOutline[]){crego[ah], crego[al]});
      
      break;
    }
    case bh_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "%sError Report:\n\t%sLine: %d\n\tReport:Expected comma", RED, LRED, p->lex->line);

      gen_token(p->lex);
      number = try_conv_from_hex(p->lex->curr_token->tv, child_max);

      dbg(number <= child_max, "%sError Report:\n\t%sLine: %d\n\tReport:Overflow Error", RED, LRED, p->lex->line);

      p_regs[bh].high_bit = (unsigned char)number;
      break;
    }
    case bl_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "%sError Report:\n\t%sLine: %d\n\tReport:Expected comma", RED, LRED, p->lex->line);

      gen_token(p->lex);
      number = try_conv_from_hex(p->lex->curr_token->tv, child_max);

      dbg(number <= child_max, "%sError Report:\n\t%sLine: %d\n\tReport:Overflow Error", RED, LRED, p->lex->line);

      p_regs[bl].low_bit = (unsigned char)number;
      break;
    }
    case cx_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "%sError Report:\n\t%sLine: %d\n\tReport:Expected comma", RED, LRED, p->lex->line);

      gen_token(p->lex);
      number = try_conv_from_hex(p->lex->curr_token->tv, general_max);

      dbg(number <= general_max, "%sError Report:\n\t%sLine: %d\n\tReport:Overflow Error", RED, LRED, p->lex->line);

      assign_bits(&p_regs[cx], number,(CRegisterOutline[]){crego[ch], crego[cl]});

      break;
    }
    case ch_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "%sError Report:\n\t%sLine: %d\n\tReport:Expected comma", RED, LRED, p->lex->line);

      gen_token(p->lex);
      number = try_conv_from_hex(p->lex->curr_token->tv, child_max);

      dbg(number <= child_max, "%sError Report:\n\t%sLine: %d\n\tReport:Overflow Error", RED, LRED, p->lex->line);

      p_regs[ch].high_bit = (unsigned char)number;
      break;
    }
    case cl_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "%sError Report:\n\t%sLine: %d\n\tReport:Expected comma", RED, LRED, p->lex->line);

      gen_token(p->lex);
      number = try_conv_from_hex(p->lex->curr_token->tv, child_max);

      dbg(number <= child_max, "%sError Report:\n\t%sLine: %d\n\tReport:Overflow Error", RED, LRED, p->lex->line);

      p_regs[cl].low_bit = (unsigned char)number;
      break;
    }
    case dx_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "%sError Report:\n\t%sLine: %d\n\tReport:Expected comma", RED, LRED, p->lex->line);

      gen_token(p->lex);
      number = try_conv_from_hex(p->lex->curr_token->tv, general_max);

      dbg(number <= general_max, "%sError Report:\n\t%sLine: %d\n\tReport:Overflow Error", RED, LRED, p->lex->line);

      assign_bits(&p_regs[dx], number, (CRegisterOutline[]){crego[dh], crego[dl]});

      break;
    }
    case dh_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "%sError Report:\n\t%sLine: %d\n\tReport:Expected comma", RED, LRED, p->lex->line);

      gen_token(p->lex);
      number = try_conv_from_hex(p->lex->curr_token->tv, child_max);

      dbg(number <= child_max, "%sError Report:\n\t%sLine: %d\n\tReport:Overflow Error", RED, LRED, p->lex->line);

      p_regs[dh].high_bit = (unsigned char)number;
      break;
    }
    case dl_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "%sError Report:\n\t%sLine: %d\n\tReport:Expected comma", RED, LRED, p->lex->line);

      gen_token(p->lex);
      number = try_conv_from_hex(p->lex->curr_token->tv, child_max);

      dbg(number <= child_max, "%sError Report:\n\t%sLine: %d\n\tReport:Overflow Error", RED, LRED, p->lex->line);

      p_regs[dl].low_bit = number;
      break;
    }
    case es_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "%sError Report:\n\t%sLine: %d\n\tReport:Expected comma", RED, LRED, p->lex->line);

      gen_token(p->lex);
      number = try_conv_from_hex(p->lex->curr_token->tv, general_max);

      dbg(number <= general_max, "%sError Report:\n\t%sLine: %d\n\tReport:Overflow Error", RED, LRED, p->lex->line);

      assign_bits(&p_regs[es], number,NULL);

      break;
    }
    case si_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "%sError Report:\n\t%sLine: %d\n\tReport:Expected comma", RED, LRED, p->lex->line);

      gen_token(p->lex);
      number = try_conv_from_hex(p->lex->curr_token->tv, general_max);

      dbg(number <= general_max, "%sError Report:\n\t%sLine: %d\n\tReport:Overflow Error", RED, LRED, p->lex->line);

      assign_bits(&p_regs[si], number,NULL);
      break;
    }
    case di_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "%sError Report:\n\t%sLine: %d\n\tReport:Expected comma", RED, LRED, p->lex->line);

      gen_token(p->lex);
      number = try_conv_from_hex(p->lex->curr_token->tv, general_max);

      dbg(number <= general_max, "%sError Report:\n\t%sLine: %d\n\tReport:Overflow Error", RED, LRED, p->lex->line);

      assign_bits(&p_regs[di], number,NULL);
      break;
    }
    case ds_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "%sError Report:\n\t%sLine: %d\n\tReport:Expected comma", RED, LRED, p->lex->line);

      gen_token(p->lex);
      number = try_conv_from_hex(p->lex->curr_token->tv, general_max);

      dbg(number <= general_max, "%sError Report:\n\t%sLine: %d\n\tReport:Overflow Error", RED, LRED, p->lex->line);

      assign_bits(&p_regs[ds], number,NULL);
      break;
    }
    case cs_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "Expected comma");

      gen_token(p->lex);
      number = try_conv_from_hex(p->lex->curr_token->tv, general_max);

      dbg(number <= general_max, "%sError Report:\n\t%sLine: %d\n\tReport:Overflow Error", RED, LRED, p->lex->line);

      assign_bits(&p_regs[cs], number,NULL);
      break;
    }
    case bp_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "%sError Report:\n\t%sLine: %d\n\tReport:Expected comma", RED, LRED, p->lex->line);

      gen_token(p->lex);
      number = try_conv_from_hex(p->lex->curr_token->tv, general_max);

      if(number >= mo->stack_p)
      {
        int last_val = mo->stack_p;
        mo->stack_p = number + 100;
        mo->mem.stack = realloc(
          mo->mem.stack,
          mo->stack_p * sizeof(*mo->mem.stack)
        );
        reallocStack(mo->mem.stack, last_val, mo->stack_p);
      }

      mo->base_p = number;

      mo->base_p_val = mo->mem.stack[mo->base_p];
      break;
    }
    case sp_reg:
    {
      gen_token(p->lex);

      dbg(p->lex->curr_token->TT == comma, "%sError Report:\n\t%sLine: %d\n\tReport:Expected comma",RED,LRED,p->lex->line);

      gen_token(p->lex);

      int last_val = mo->stack_p;
      number = try_conv_from_hex(p->lex->curr_token->tv, general_max);

      mo->stack_p = number;

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
      err("%sError Report:\n\t%sLine: %d\n\tReport: Unknown register %s", RED,LRED,p->lex->line,p->lex->curr_token->tv);
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

            dbg(p->lex->curr_token->TT == LC, "%sError Report:\n\t%sLine: %d\n\tReport:Expected `{`", RED, LRED, p->lex->line);
            gen_token(p->lex);

            dbg(p->lex->curr_token->TT != RC, "%sError Report:\n\t%sLine: %d\n\tReport: Null data sections requires `null` specifier.", RED,LRED,p->lex->line);

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
                    if(mo_index==0)
                        mo->mem.data_sect = calloc(1, sizeof(*mo->mem.data_sect));
                    else
                        mo->mem.data_sect = realloc(
                            mo->mem.data_sect,
                            (mo_index+1)*sizeof(*mo->mem.data_sect)
                        );
                    mo->mem.data_sect[mo_index] = calloc(1, sizeof(*mo->mem.data_sect[mo_index]));

                    mo->mem.data_sect[mo_index]->name = p->lex->curr_token->tv;
          
                    gen_token(p->lex);

                    /*
                     *   ToDo:
                     *     There has to be a way to automate this process
                    */
                    switch(p->lex->curr_token->TT)
                    {
                        case db_k:
                        {
                            mo->mem.data_sect[mo_index]->var_type = db;
                            mo->mem.data_sect[mo_index]->db_data = calloc(1, sizeof(char));
                
                            set_void(mo->mem.data_sect[mo_index]->var_type, mo);

                            gen_token(p->lex);
                            assign(p->lex->curr_token->tv, mo->mem.data_sect[mo_index]->db_data, sizeof(char));

                            break;
                        }
                        case dw_k:
                        {
                            mo->mem.data_sect[mo_index]->var_type = dw;
                            mo->mem.data_sect[mo_index]->dw_data = calloc(1, sizeof(*mo->mem.data_sect[mo_index]->dw_data));

                            set_void(mo->mem.data_sect[mo_index]->var_type,mo);

                            gen_token(p->lex);
                            
                            dbg(p->lex->curr_token->TT == number, "%sError Report:%s\n\tLine: %d\n\tReport: dw requires numbere assignments.",RED,LRED, p->lex->line);
                            
                            

                            break;
                        }
                        default:
                        {
                            err("%sError Report:\n\t%sLine: %d\n\tReport:Unknown type.",RED,LRED, p->lex->line);
                        }
                    }
          
                break;
            }
            default:
            {
                err("%sError Report:\n\t%sLine: %d\n\tReport:Unknown",RED,LRED, p->lex->line);
            }
        }
      
        data_sect_end:
        mo_index++;
        gen_token(p->lex);

        if(!(p->lex->curr_token->TT == RC))
            goto recheck;
            break;
        }
        default: {
            err("%sError Report:\n\t%sLine: %d\n\tReport:Unknown section: %s",RED,LRED, p->lex->line, p->lex->curr_token->tv);
        }
    }

  return p;
}

void has_comma(Parser *p)
{
    dbg(p->lex->curr_token->TT == comma, "%sError Report:\n\t%sLine: %d\n\tReport: Execting comma",RED,LRED, p->lex->line);
    gen_token(p->lex);
}

void set_rreg(Parser *p)
{
    switch(p->lex->curr_token->TT)
    {
        case bx_reg: p->mi->rreg=bx;break;
        case ax_reg: p->mi->rreg=ax;break;
        case cx_reg: p->mi->rreg=cx;break;
        case dx_reg: p->mi->rreg=dx;break;
        case ah_reg: p->mi->rreg=ah;break;
        case al_reg: p->mi->rreg=al;break;
        case ch_reg: p->mi->rreg=ch;break;
        case cl_reg: p->mi->rreg=cl;break;
        case dh_reg: p->mi->rreg=dh;break;
        case dl_reg: p->mi->rreg=dl;break;
        case bh_reg: p->mi->rreg=bh;break;
        case bl_reg: p->mi->rreg=bl;break;
        case si_reg: p->mi->rreg=si;break;
        case di_reg: p->mi->rreg=di;break;
        case bp_reg: p->mi->rreg=bp;break;
        case sp_reg: p->mi->rreg=sp;break;
        case es_reg: p->mi->rreg=es;break;
        case ds_reg: p->mi->rreg=ds;break;
        case cs_reg: p->mi->rreg=cs;break;
        case ss_reg: p->mi->rreg=ss;break;
        default:
        {
            err("%sError Report:\n\t%sLine: %d\n\tReport: Expecting a valid rval register.",RED,LRED, p->lex->line);
        }
    }
}

Parser *parse_mov(Parser *p)
{
    gen_token(p->lex);

    switch(p->lex->curr_token->TT)
    {
        case ax_reg:
        case bx_reg:
        case cx_reg:
        case dx_reg:
        {
            p->mi->lType = mem16;
            p->mi->lreg = p->lex->curr_token->TT;

            gen_token(p->lex);
            
            has_comma(p);
	    printf("%s", p->lex->curr_token->tv);

            dbg(p->lex->curr_token->parent_reg == 0, "%sError Report:\n\t%sLine: %d\n\tReport: Invalid combination", RED, LRED, p->lex->line);

            //set_rreg(p);
	    p->mi->rreg = p->lex->curr_token->TT;
            p->mi->rType = reg16;
            get_binary_data(p->mi);

	    break;
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
      case mov_k: parse_mov(p);break;
      case eof: goto end;
      default: break;
    }
  } while(p->lex->curr_token->TT != eof);

end:

  free(p->lex->curr_token);
  free(p->lex);
  free(p);
}
