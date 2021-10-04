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
static MemOutline *mo;

#define defOrg 0x0

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
    
    printf("%s", v);
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
    