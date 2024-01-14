#include "types.h"
#include <assert.h>
#include <stdlib.h>

stm_t *
stm_new_compound (stm_t *stm1, stm_t *stm2)
{
  stm_t *ret = checked_malloc (sizeof (stm_t));
  ret->kind = STM_COMPOUND;
  ret->data.compound.stm1 = stm1;
  ret->data.compound.stm2 = stm2;
  return ret;
}

stm_t *
stm_new_assign (const char *id, exp_t *exp)
{
  stm_t *ret = checked_malloc (sizeof (stm_t));
  ret->kind = STM_ASSIGN;
  ret->data.assign.id = id;
  ret->data.assign.exp = exp;
  return ret;
}

stm_t *
stm_new_print (exp_list_t *exps)
{
  stm_t *ret = checked_malloc (sizeof (stm_t));
  ret->kind = STM_PRINT;
  ret->data.print.exps = exps;
  return ret;
}

exp_t *
exp_new_id (const char *id)
{
  exp_t *ret = checked_malloc (sizeof (exp_t));
  ret->kind = EXP_ID;
  ret->data.id = id;
  return ret;
}

exp_t *
exp_new_num (long num)
{
  exp_t *ret = checked_malloc (sizeof (exp_t));
  ret->kind = EXP_NUM;
  ret->data.num = num;
  return ret;
}

exp_t *
exp_new_op (exp_t *left, binop_t oper, exp_t *right)
{
  exp_t *ret = checked_malloc (sizeof (exp_t));
  ret->kind = EXP_OP;
  ret->data.op.left = left;
  ret->data.op.oper = oper;
  ret->data.op.right = right;
  return ret;
}

exp_t *
exp_new_eseq (stm_t *stm, exp_t *exp)
{
  exp_t *ret = checked_malloc (sizeof (exp_t));
  ret->kind = EXP_ESEQ;
  ret->data.eseq.stm = stm;
  ret->data.eseq.exp = exp;
  return ret;
}

exp_list_t *
exp_list_new_pair (exp_t *head, exp_list_t *tail)
{
  exp_list_t *ret = checked_malloc (sizeof (exp_list_t));
  ret->kind = EXP_LIST_PAIR;
  ret->data.pair.head = head;
  ret->data.pair.tail = tail;
  return ret;
}

exp_list_t *
exp_list_new_last (exp_t *last)
{
  exp_list_t *ret = checked_malloc (sizeof (exp_list_t));
  ret->kind = EXP_LIST_LAST;
  ret->data.last = last;
  return ret;
}

void *
checked_malloc (size_t size)
{
  void *ret = malloc (size);
  assert (ret != NULL);
  return ret;
}
