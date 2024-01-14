struct stm_t;
struct exp_t;
struct exp_list_t;
enum binop_t;

typedef struct stm_t stm_t;
typedef struct exp_t exp_t;
typedef struct exp_list_t exp_list_t;
typedef enum binop_t binop_t;

enum binop_t
{
  PLUS,
  MINUS,
  TIMES,
  DIV,
};

struct stm_t
{
  enum stm_k
  {
    STM_COMPOUND,
    STM_ASSIGN,
    STM_PRINT,
  } kind;

  union stm_d
  {
    struct
    {
      stm_t *stm1;
      stm_t *stm2;
    } compound;
    struct
    {
      const char *id;
      exp_t *exp;
    } assign;
    struct
    {
      exp_list_t *exps;
    } print;
  } data;
};

stm_t *stm_new_compound (stm_t *stm1, stm_t *stm2);
stm_t *stm_new_assign (const char *id, exp_t *exp);
stm_t *stm_new_print (exp_list_t *exps);

struct exp_t
{
  enum exp_k
  {
    EXP_ID,
    EXP_NUM,
    EXP_OP,
    EXP_ESEQ,
  } kind;

  union exp_d
  {
    const char *id;
    long num;
    struct
    {
      exp_t *left;
      binop_t oper;
      exp_t *right;
    } op;
    struct
    {
      stm_t *stm;
      exp_t *exp;
    } eseq;
  } data;
};

exp_t *exp_new_id (const char *id);
exp_t *exp_new_num (long num);
exp_t *exp_new_op (exp_t *left, binop_t oper, exp_t *right);
exp_t *exp_new_eseq (stm_t *stm, exp_t *exp);

struct exp_list_t
{
  enum exp_list_k
  {
    EXP_LIST_PAIR,
    EXP_LIST_LAST,
  } kind;
  union exp_list_d
  {
    struct
    {
      exp_t *head;
      exp_list_t *tail;
    } pair;
    exp_t *last;
  } data;
};

exp_list_t *exp_list_new_pair (exp_t *head, exp_list_t *tail);
exp_list_t *exp_list_new_last (exp_t *last);

#include <stddef.h>

void *checked_malloc (size_t size);
