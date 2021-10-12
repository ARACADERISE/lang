#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;

#define true 1
#define false 0

#define general_max   USHRT_MAX
#define child_max     UCHAR_MAX
#define data_max      UINT32_MAX

enum registers
{
  ax,
  ah,
  al,
  bx,
  bh,
  bl,
  cx,
  ch,
  cl,
  dx,
  dh,
  dl,
  sp,
  ss,
  bp,
  si,
  di,
  es,
  cs,
  ds
};

enum reg_type
{
  // ax
  accumulator,
  // bx
  base_reg,
  // cx
  count_reg,
  // dx
  data_reg,
  // si
  source_index,
  // di
  dest_index,
  // cs
  code_seg,
  // ds
  data_seg,
  // ss
  stack_seg,
  // es
  extra_seg
};

enum variable_type
{
  db,
  dw,
  dd,
  dq
};

typedef enum instruction_request {
    mem8,
    mem16,
    reg8,
    reg16
} ir;

// mem8, reg8
#define H88     (int[]){mem8, reg8}
// mem16, reg16
#define H89     (int[]){mem16, reg16}
/*
 * reg8, reg8
 * reg8, mem8
*/
#define H8A     (int[][2]){{reg8, reg8}, {reg8, mem8}}

// Opcodes
typedef struct AsmOpcodes
{
    int opcode;
    u8 mod[2];
    u8 reg[3];
    u8 RM[3];

    enum registers lreg[2];
    enum registers rreg[2];

    union {
        struct {};
    };
} ASM_OPCODES;

static ASM_OPCODES ops[] = {
    [0xC0] = {
        .opcode = 0xC0,
        .mod={'1','1'},
        .reg={'0','0','0'},
        .RM={'0','0','0'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xC1] = {
        .opcode = 0xC1,
        .mod={'1','1'},
        .reg={'0','0','1'},
        .RM={'0','0','0'},
        .lreg={cl,cx},
        .rreg={al,ax}
    },
    [0xC2] = {
        .opcode = 0xc2,
        .mod={'1','1'},
        .reg={'0','1','0'},
        .RM={'0','0','0'},
        .lreg={dl,dx},
        .rreg={al,ax}
    },
    [0xC3] = {
        .opcode = 0xc3,
        .mod={'1','1'},
        .reg={'0','1','1'},
        .RM={'0','0','0'},
        .lreg={bl,bx},
        .rreg={al,ax}
    },
    [0xC4] = {
        .opcode = 0xc4,
        .mod={'1','1'},
        .reg={'1','0','0'},
        .RM={'0','0','0'},
        .lreg={sp,ah},
        .rreg={al,ax}
    },
    [0xC5] = {
        .opcode = 0xc5,
        .mod={'1','1'},
        .reg={'1','0','1'},
        .RM={'0','0','0'},
        .lreg={bp,ch},
        .rreg={al,ax}
    },
    [0xC6] = {
        .opcode = 0xc6,
        .mod={'1','1'},
        .reg={'1','1','0'},
        .RM={'0','0','0'},
        .lreg={si,dh},
        .rreg={al,ax}
    },
    [0xC7] = {
        .opcode = 0xc7,
        .mod={'1','1'},
        .reg={'1','1','1'},
        .RM={'0','0','0'},
        .lreg={di,bh},
        .rreg={al,ax}
    },
    [0xC8] = {
        .opcode = 0xc8,
        .mod={'1','1'},
        .reg={'0','0','0'},
        .RM={'0','0','1'},
        .lreg={al,ax},
        .rreg={cl,cx}
    },
    [0xC9] = {
        .opcode = 0xc9,
        .mod={'1','1'},
        .reg={'0','0','1'},
        .RM={'0','0','1'},
        .lreg={cl,cx},
        .rreg={cl,cx}
    },
    [0xCA] = {
        .opcode = 0xCA,
        .mod={'1','1'},
        .reg={'0','1','0'},
        .RM={'0','0','1'},
        .lreg={dl,dx},
        .rreg={cl,cx}
    },
    [0xCB] = {
        .opcode = 0xCB,
        .mod={'1','1'},
        .reg={'0','1','1'},
        .RM={'0','0','1'},
        .lreg={bl,bx},
        .rreg={cl,cx}
    },
    [0xCC] = {
        .opcode = 0xCC,
        .mod={'1','1'},
        .reg={'1','0','0'},
        .RM={'0','0','1'},
        .lreg={dp,ah},
        .rreg={cl,cx}
    },
    [0xCD] = {
        .opcode = 0xCD,
        .mod={'1','1'},
        .reg={'1','0','1'},
        .RM={'0','0','1'},
        .lreg={bp,ch},
        .rreg={cl,cx}
    },
    [0xCE] = {
        .opcode = 0xCE,
        .mod={'1','1'},
        .reg={'1','1','0'},
        .RM={'0','0','1'},
        .lreg={si,dh},
        .rreg={cl,cx}
    },
    [0xCF] = {
        .opcode = 0xCF,
        .mod={'1','1'},
        .reg={'1','1','1'},
        .RM={'0','0','1'},
        .lreg={di,bh},
        .rreg={cl,cx}
    },
    [0xD0] = {
        .opcode = 0xDO,
        .mod={'1','1'},
        .reg={'0','0','0'},
        .RM={'0','1','0'},
        .lreg={al,ax},
        .rreg={dl,dx}
    },
    [0xD1] = {
        .opcode = 0xD1,
        .mod={'1','1'},
        .reg={'0','0','1'},
        .RM={'0','1','0'},
        .lreg={cl,cx},
        .rreg={dl,dx}
    },
    [0xD2] = {
        .opcode = 0xD2,
        .mod={'1','1'},
        .reg={'0','1','0'},
        .RM={'0','1','0'},
        .lreg={dl,dx},
        .rreg={dl,dx}
    },
    [0xD3] = {
        .opcode = 0xD3,
        .mod={'1','1'},
        .reg={'0','1','1'},
        .RM={'0','1','0'},
        .lreg={bl,bx},
        .rreg={dl,dx}
    },
    [0xD4] = {
        .opcode = 0xD4,
        .mod={'1','1'},
        .reg={'1','0','0'},
        .RM={'0','1','0'},
        .lreg={sp,ah},
        .rreg={dl,dx}
    },
    [0xD5] = {
        .opcode = 0xD5,
        .mod={'1','1'},
        .reg={'1','0','1'},
        .RM={'0','1','0'},
        .lreg={bp,ch},
        .rreg={dl,dx}
    },
    [0xD6] = {
        .opcode = 0xD6,
        .mod={'1','1'},
        .reg={'1','1','0'},
        .RM={'0','1','0'},
        .lreg={si,dh},
        .rreg={dl,dx}
    },
    [0xD7] = {
        .opcode = 0xD7,
        .mod={'1','1'},
        .reg={'1','1','1'},
        .RM={'0','1','0'},
        .lreg={di,bh},
        .rreg={dl,dx}
    },
    [0xD8] = {
        .opcode = 0xD8,
        .mod={'1','1'},
        .reg={'0','0','0'},
        .RM={'0','1','1'},
        .lreg={al,ax},
        .rreg={bl,bx}
    },
    [0xD9] = {
        .opcode = 0xD9,
        .mod={'1','1'},
        .reg={'0','0','1'},
        .RM={'0','1','1'},
        .lreg={cl,cx},
        .rreg={bl,bx}
    },
    [0xDA] = {
        .opcode = 0xDA,
        .mod={'1','1'},
        .reg={'0','1','0'},
        .RM={'0','1','1'},
        .lreg={dl,dx},
        .rreg={bl,bx}
    },
    [0xDB] = {.opcode = 0xDB,
        .mod={'1','1'},
        .reg={'0','1','1'},
        .RM={'0','1','1'},
        .lreg={bl,bx},
        .rreg={bl,bx}
    },
    [0xDC] = {
        .opcode = 0xDC,
        .mod={'1','1'},
        .reg={'1','0','0'},
        .RM={'0','1','1'},
        .lreg={sp,ah},
        .rreg={bl,bx}
    },
    [0xDD] = {
        .opcode = 0xDD,
        .mod={'1','1'},
        .reg={'1','0','1'},
        .RM={'0','1','1'},
        .lreg={bp,ch},
        .rreg={bl,bx}
    },
    [0xDE] = {
        .opcode = 0xDE,
        .mod={'1','1'},
        .reg={'1','1','0'},
        .RM={'0','1','1'},
        .lreg={si,dh},
        .rreg={bl,bx}
    },
    [0xDF] = {
        .opcode = 0xDF,
        .mod={'1','1'},
        .reg={'1','1','1'},
        .RM={'0','1','1'},
        .lreg={bi,dh},
        .rreg={bl,bx}
    },
    [0xE0] = {
        .opcode = 0xE0,
        .mod={'1','1'},
        .reg={'0','0','0'},
        .RM={'1','0','0'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xE1] = {
        .opcode = 0xE1,
        .mod={'1','1'},
        .reg={'0','0','1'},
        .RM={'1','0','0'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xE2] = {
        .opcode = 0xE2,
        .mod={'1','1'},
        .reg={'0','1','0'},
        .RM={'1','0','0'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xE3] = {
        .opcode = 0xE3,
        .mod={'1','1'},
        .reg={'0','1','1'},
        .RM={'1','0','0'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xE4] = {
        .opcode = 0xE4,
        .mod={'1','1'},
        .reg={'1','0','0'},
        .RM={'1','0','0'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xE5] = {
        .opcode = 0xE5,
        .mod={'1','1'},
        .reg={'1','0','1'},
        .RM={'1','0','0'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xE6] = {
        .opcode = 0xE6,
        .mod={'1','1'},
        .reg={'1','1','0'},
        .RM={'1','0','0'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xE7] = {
        .opcode = 0xE7,
        .mod={'1','1'},
        .reg={'1','1','1'},
        .RM={'1','0','0'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xE8] = {
        .opcode = 0xE8,
        .mod={'1','1'},
        .reg={'0','0','0'},
        .RM={'1','0','0'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xE9] = {
        .opcode = 0xE9,
        .mod={'1','1'},
        .reg={'0','0','1'},
        .RM={'1','0','1'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xEA] = {
        .opcode = 0xEA,
        .mod={'1','1'},
        .reg={'0','1','0'},
        .RM={'1','0','1'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xEB] = {
        .opcode = 0xEB,
        .mod={'1','1'},
        .reg={'0','1','1'},
        .RM={'1','0','1'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xEC] = {
        .opcode = 0xEC,
        .mod={'1','1'},
        .reg={'1','0','0'},
        .RM={'1','0','1'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xED] = {
        .opcode = 0xED,
        .mod={'1','1'},
        .reg={'1','0','1'},
        .RM={'1','0','1'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xEE] = {
        .opcode = 0xEE,
        .mod={'1','1'},
        .reg={'1','1','0'},
        .RM={'1','0','1'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xEF] = {
        .opcode = 0xEF,
        .mod={'1','1'},
        .reg={'1','1','1'},
        .RM={'1','0','1'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xF0] = {
        .opcode = 0xF0,
        .mod={'1','1'},
        .reg={'0','0','0'},
        .RM={'1','1','0'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xF1] = {
        .opcode = 0xF1,
        .mod={'1','1'},
        .reg={'0','0','1'},
        .RM={'1','1','0'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xF2] = {
        .opcode = 0xF2,
        .mod={'1','1'},
        .reg={'0','1','0'},
        .RM={'1','1','0'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xF3] = {
        .opcode = 0xF3,
        .mod={'1','1'},
        .reg={'0','1','1'},
        .RM={'1','1','0'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xF4] = {
        .opcode = 0xF4,
        .mod={'1','1'},
        .reg={'1','0','0'},
        .RM={'1','1','0'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xF5] = {
        .opcode = 0xF5,
        .mod={'1','1'},
        .reg={'1','0','1'},
        .RM={'1','1','0'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xF6] = {
        .opcode = 0xF6,
        .mod={'1','1'},
        .reg={'1','1','0'},
        .RM={'1','1','0'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xF7] = {
        .opcode = 0xF7,
        .mod={'1','1'},
        .reg={'1','1','1'},
        .RM={'1','1','0'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xF8] = {
        .opcode = 0xF8,
        .mod={'1','1'},
        .reg={'0','0','0'},
        .RM={'1','1','1'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xF9] = {
        .opcode = 0xF9,
        .mod={'1','1'},
        .reg={'0','0','1'},
        .RM={'1','1','1'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xFA] = {
        .opcode = 0xFA,
        .mod={'1','1'},
        .reg={'0','1','0'},
        .RM={'1','1','1'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xFB] = {
        .opcode = 0xFB,
        .mod={'1','1'},
        .reg={'0','1','1'},
        .RM={'1','1','1'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xFC] = {
        .opcode = 0xFC,
        .mod={'1','1'},
        .reg={'1','0','0'},
        .RM={'1','1','1'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xFD] = {
        .opcode = 0xFD,
        .mod={'1','1'},
        .reg={'1','0','1'},
        .RM={'1','1','1'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xFE] = {
        .opcode = 0xFE,
        .mod={'1','1'},
        .reg={'1','1','0'},
        .RM={'1','1','1'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
    [0xFF] = {
        .opcode = 0xFF,
        .mod={'1','1'},
        .reg={'1','1','1'},
        .RM={'1','1','1'},
        .lreg={al,ax},
        .rreg={al,ax}
    },
};

// Parent registers.
// ax,bx,cx,dx,es,si,ss,di
typedef struct p_register_outline
{
  char *name;
  unsigned short parent_val;
  unsigned char high_bit;
  unsigned char low_bit;
  bool is_common;
  enum reg_type type;

  union {
    struct {
      enum registers regs;
    };
  };
} PRegisterOutline;

typedef struct mov_instruction
{
    enum instruction_request lType;
    enum instruction_request rType;

    int *instruction_set;
    int opcode;
} MovInstruction;

static inline void get_binary_data(MovInstruction *mi)
{
    if(mi->lType == mem8 && mi->rType == reg8)
    {
        mi->instruction_set = H88;
        mi->opcode = 0x88;
        return;
    }
    if(mi->lType == mem16 && mi->rType == reg16)
    {
        mi->instruction_set = H89;
        mi->opcode = 0x89;
        return;
    }
    if(mi->lType == reg8 && mi->rType == reg8)
    {
        mi->instruction_set = H8A[0];
        mi->opcode = 0x8A;
        return;
    }
    if(mi->lType == reg8 && mi->rType == mem8)
    {
        mi->instruction_set = H8A[1];
        mi->opcode = 0x8A;
        return;
    }

    fprintf(stderr, "Cannot configure binary data for current instruction set.");
    exit(EXIT_FAILURE);
}

// Child registers.
/*
  ax:
    ah, al
  bx:
    bh, bl
  dx:
    dh, dl
  cx:
    ch, cl
*/
typedef struct c_register_outline
{
  char *name;
  u8 r_val;

  union {
    struct  {
      enum registers regs;
    };
  };
} CRegisterOutline;

// Initializing it here.
static CRegisterOutline crego[] = {
    [ah] = {
        .name = "ah",
        .r_val = 0
    },
    [al] = {
        .name = "al",
        .r_val = 0
    },
    [bh] = {
        .name = "bh",
        .r_val = 0
    },
    [bl] = {
        .name = "bl",
        .r_val = 0
    },
    [ch] = {
        .name = "ch",
        .r_val = 0
    },
    [cl] = {
        .name = "cl",
        .r_val = 0
    },
    [dh] = {
        .name = "dh",
        .r_val = 0
    },
    [dl] = {
        .name = "dl",
        .r_val = 0
    }
};

// What register can correspond with what register.
typedef struct r_correspond
{
  enum registers access[6];

  union {
    struct {
      enum registers regs;
    };
  };
} RCorrespond;

typedef struct MemoryOutline
{
  // Where to place the program in mem.data
  u16 origin;
  u32 using_;
  // Pointer to current index of the data.
  u16 program_pointer;

  // stack/base pointer value
  u16 base_p;
  u8 base_p_val;

  u16 stack_p;
  u8 stack_p_val;

  /*
    The binary output data is purely controlled
    via the program, where, when the user wants to be able to
    have access to a wide range of "memory", they have to "ask"
    for it, which will lead to the action of allocating the needed
    memory. 
      If you attempt to set the origin of the program and have not
      previously requested for "memory", it will error due to there being no "memory" available to set the origin to.
      As well as, if you attempt to write code and compile, it, too, will error
      due to there being no set-aside "memory" to be assigned.

      To get "memory", add this to the top of the file "use xbytes".
        "use 15000" means we want 15000 bytes of "memory" allocated.
      All memory will be freed at runtime, if not used.

    When pushing to the stack, however, there is no need to
    allocate needed "memory". When pushing, memory is auto-allocated
    for you, but it is released at runtime.
  */
  struct
  {
    // The stack is for data being stored for later use, freed at runtime
    u16 *stack;

    // The programs binary output. Manually allocated via the program
    u8 *bin;

    // The programs .data section
    struct {
      char *name;
      enum variable_type var_type;

      u8    *db_data;
      u16   *dw_data;
      u32   *dd_data;
      u64   *dq_data;
    } **data_sect;
  } mem;
} MemOutline;

static int mo_index = 0;
static MemOutline *mo;

#define defOrg 0x0

/* COLORS */
static const char *RED =    "\e[1;31m";
static const char *LRED =   "\e[1;91m";

static inline int isAscii(char c)
{
   return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ? 0 : 1;
}

static inline int isNumber(char c)
{
  return (c >= '0' && c <= '9') ? 0 : 1;
}

static inline void assign_bits(PRegisterOutline *p, unsigned short val, CRegisterOutline c[2])
{
    p->high_bit = val >> 8;
    p->low_bit = val & ((1 << 8)-1);

    p->parent_val = (p->high_bit << 8) | p->low_bit;

    // Nothing to do if the register does not have a high/low bit register
    if(c == NULL)
        return;
    
    c[0].r_val = p->high_bit;
    c[1].r_val = p->low_bit;
}

static inline bool is_hex(char *v)
{
    for(int i = 0; i < strlen(v); i++)
    {
        if(v[i] == 'x' || v[i] == 'h') return 0;
    }

    return 1;
}

static inline unsigned short try_conv_from_hex(char *v, int max)
{
    if(is_hex(v)==1)
        return atoi(v);
        
    long long dec = 0;
    long long base = 1;

    for(int i = strlen(v); i >= 0; i--)
    {
        if(v[i] >= '0' && v[i] <= '9')
        {
            dec += (v[i] - 48) * base;
            base *= 16;
        }
        else if(v[i] >= 'A' && v[i] <= 'F')
        {
            dec += (v[i] - 55) * base;
            base *= 16;
        }
        else if(v[i] >= 'a' && v[i] <= 'f')
        {
            dec += (v[i] - 87) * base;
            base *= 16;
        }
    }

    if(dec > max)
        return 0;
    
    return (unsigned short)dec;
}

static inline void set_void(enum variable_type vtype, MemOutline *m)
{
  switch(vtype)
  {
    case db:
    {
      m->mem.data_sect[0]->dw_data = (void*)0;
      m->mem.data_sect[0]->dd_data = (void*)0;
      m->mem.data_sect[0]->dq_data = (void*)0;
      break;
    }
    case dw:
    {
      m->mem.data_sect[0]->db_data = (void*)0;
      m->mem.data_sect[0]->dd_data = (void*)0;
      m->mem.data_sect[0]->dq_data = (void*)0;
      break;
    }
    case dd:
    {
      m->mem.data_sect[0]->dw_data = (void*)0;
      m->mem.data_sect[0]->db_data = (void*)0;
      m->mem.data_sect[0]->dq_data = (void*)0;
      break;
    }
    case dq:
    {
      m->mem.data_sect[0]->dw_data = (void*)0;
      m->mem.data_sect[0]->dd_data = (void*)0;
      m->mem.data_sect[0]->db_data = (void*)0;
      break;
    }
  }
}

#define reallocStack(s, start, end) \
  for(int i = start; i < end; i++) \
  { \
    s[i] = 0; \
  }

#define assign(val, data, type) \
  for(int i = 0; i < sizeof(val) / type; i++) { \
    data[i] = val[i]; \
    data = realloc( \
      data, \
      ((sizeof(data)/type)+1)*sizeof(data) \
    ); \
  }

#define dbg(e, f, ...) if(!(e)) { \
    fprintf(stderr, f, ##__VA_ARGS__); \
    exit(EXIT_FAILURE); \
  }

#define err(msg, ...) \
    fprintf(stderr, msg, ##__VA_ARGS__); \
    exit(EXIT_FAILURE);
    