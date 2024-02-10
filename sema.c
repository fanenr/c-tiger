#include "sema.h"
#include "ast.h"
#include "parser.h"
#include "util.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

stack stac;

#define STACK_PUSH(STAC, PTR)                                                 \
  if (!stack_push (STAC, PTR))                                                \
  error ("error: stack_push\n")

#define STACK_POP(STAC)                                                       \
  if (!stack_pop (STAC))                                                      \
  error ("error: stack_pop\n")

#define STACK_TOP(STAC)                                                       \
  ({                                                                          \
    void *ptr = stack_top (STAC);                                             \
    if (!ptr)                                                                 \
      error ("error: stack_top\n");                                           \
    ptr;                                                                      \
  })

void
sema_check (ast_env *env)
{
  if (!env)
    return;

  env->outer = stac.vec.size ? STACK_TOP (&stac) : NULL;
  STACK_PUSH (&stac, env);

  vector *defs = &env->defs;
  for (size_t i = 0; i < defs->size; i++)
    {
      ast_def *def = vector_get (defs, i);
      sema_check_def (def, env);
    }

  vector *stms = &env->stms;
  for (size_t i = 0; i < stms->size; i++)
    {
      ast_stm *stm = vector_get (stms, i);
      sema_check_stm (stm, env);
    }

  STACK_POP (&stac);
}

void
sema_check_def (ast_def *def, ast_env *env)
{
  ast_tok id = def->id;
  sema_check_id (id, env);

  switch (def->kind)
    {
    case AST_DEF_VAR:
      {
        ast_def_var *get = AST_DEF_GET (var, def);
        sema_check_type (get->type, env);
        if (get->type->kind == AST_TYPE_VOID)
          error ("%s (%u:%u) can not be variable type\n", "void",
                 get->type->pos.ln, get->type->pos.ch);
        break;
      }
    case AST_DEF_TYPE:
      {
        ast_def_type *get = AST_DEF_GET (type, def);
        sema_check_type (get->type, env);
        break;
      }
    case AST_DEF_FUNC:
      {
        ast_def_func *get = AST_DEF_GET (func, def);
        sema_check_type (get->type, env);
        sema_check (get->env);
        break;
      }
    }
}

void
sema_check_stm (ast_stm *stm, ast_env *env)
{
  switch (stm->kind)
    {
    case AST_STM_ASSIGN:
      {
        ast_stm_assign *get = AST_STM_GET (assign, stm);
        ast_exp *obj = get->obj;
        ast_exp *exp = get->exp;
        sema_check_exp (obj, env);
        sema_check_exp (exp, env);

        if (!sema_exp_is_lv (obj))
          error ("%s (%u:%u) need a lvalue as its first operand\n",
                 "assign stm", obj->pos.ln, obj->pos.ch);
        break;
      }
    }
}

void
sema_check_exp (ast_exp *exp, ast_env *env)
{
  switch (exp->kind)
    {
    case AST_EXP_ELEM_ID:
      {
        ast_exp_elem *get = AST_EXP_GET (elem, exp);
        ast_tok tok = get->elem;
        ast_pos pos = tok.pos;
        ast_def *def = sema_seek_def2 (tok.str, env);

        if (!def || sema_pos_comp (def->pos, exp->pos) >= 0)
          error ("%s (%u:%u) is undefined\n", tok.str, pos.ln, pos.ch);
        if (def->kind != AST_DEF_VAR)
          error ("%s (%u:%u) is not a varibale\n", tok.str, pos.ln, pos.ch);

        exp->type = AST_DEF_GET (var, def)->type;
        break;
      }
    case AST_EXP_ELEM_STR:
      {
        break;
      }
    case AST_EXP_ELEM_NUM:
      {
        ast_def *def = sema_seek_def2 ("int32", &prog);
        exp->type = AST_DEF_GET (type, def)->type;
        break;
      }
    case AST_EXP_ELEM_REAL:
      {
        ast_def *def = sema_seek_def2 ("double", &prog);
        exp->type = AST_DEF_GET (type, def)->type;
        break;
      }
    case AST_EXP_UN_UPLUS:
    case AST_EXP_UN_UMINUS:
      {
        ast_exp *operand = AST_EXP_GET (unary, exp)->exp;
        sema_check_exp (operand, env);

        if (!sema_exp_is_num (operand))
          error ("%s (%u:%u) can only be used for numbers\n", "+-",
                 exp->pos.ln, exp->pos.ch);

        exp->type = operand->type;
        break;
      }
    case AST_EXP_UN_DREF:
      {
        ast_exp *operand = AST_EXP_GET (unary, exp)->exp;
        sema_check_exp (operand, env);

        if (operand->type->kind != AST_TYPE_POINTER)
          error ("%s (%u:%u) can only be used for pointer\n", "*", exp->pos.ln,
                 exp->pos.ch);

        exp->type = operand->type->ref;
        break;
      }
    case AST_EXP_UN_ADDR:
      {
        ast_exp *operand = AST_EXP_GET (unary, exp)->exp;
        sema_check_exp (operand, env);

        if (!sema_exp_is_lv (operand))
          error ("%s (%u:%u) can only be used for lvalue\n", "&", exp->pos.ln,
                 exp->pos.ch);

        exp->type->kind = AST_TYPE_POINTER;
        exp->type->size = sizeof (void *);
        exp->type->ref = operand->type;
        break;
      }
    case AST_EXP_BIN_OR:
    case AST_EXP_BIN_AND:
    case AST_EXP_BIN_XOR:
      {
        ast_exp *operand1 = AST_EXP_GET (binary, exp)->exp1;
        ast_exp *operand2 = AST_EXP_GET (binary, exp)->exp2;
        sema_check_exp (operand1, env);
        sema_check_exp (operand2, env);

        const char *fmt = "%s (%u:%u) can only be used for integer\n";
        if (!sema_exp_is_int (operand1))
          error (fmt, "| & ^", operand1->pos.ln, exp->pos.ch);
        if (!sema_exp_is_int (operand2))
          error (fmt, "| & ^", operand2->pos.ln, exp->pos.ch);

        exp->type = operand1->type->kind > operand2->type->kind
                        ? operand1->type
                        : operand2->type;
        break;
      }
    case AST_EXP_BIN_DMEM:
      {
        ast_exp *operand1 = AST_EXP_GET (binary, exp)->exp1;
        ast_exp *operand2 = AST_EXP_GET (binary, exp)->exp2;
        sema_check_exp (operand1, env);

        ast_pos pos1 = operand1->pos;
        ast_pos pos2 = operand2->pos;
        if (!sema_exp_is_obj (operand1))
          error ("%s (%u:%u) can only be used for struct and union object\n",
                 ".", pos1.ln, pos1.ch);
        if (operand2->kind != AST_EXP_ELEM_ID)
          error ("%s (%u:%u) need a member name\n", ".", pos2.ln, pos2.ch);

        ast_tok mname = AST_EXP_GET (elem, operand2)->elem;
        ast_def *mem = sema_seek_def1 (mname.str, operand1->type->mem);
        if (!mem)
          error ("%s (%u:%u) is a undefined member\n", mname.str, pos2.ln,
                 pos2.ch);
        if (mem->kind != AST_DEF_VAR)
          error ("%s (%u:%u) is not a variable member\n", mname.str, pos2.ln,
                 pos2.ch);

        exp->type = AST_DEF_GET (var, mem)->type;
        break;
      }
    case AST_EXP_BIN_PMEM:
      {
        ast_exp *operand1 = AST_EXP_GET (binary, exp)->exp1;
        ast_exp *operand2 = AST_EXP_GET (binary, exp)->exp2;
        sema_check_exp (operand1, env);

        ast_pos pos1 = operand1->pos;
        ast_pos pos2 = operand2->pos;
        if (!sema_exp_is_pobj (operand1))
          error ("%s (%u:%u) can only be used for struct and union pointer\n",
                 "->", pos1.ln, pos1.ch);
        if (operand2->kind != AST_EXP_ELEM_ID)
          error ("%s (%u:%u) need a member name\n", "->", pos2.ln, pos2.ch);

        ast_tok mname = AST_EXP_GET (elem, operand2)->elem;
        ast_def *mem = sema_seek_def1 (mname.str, operand1->type->ref->mem);
        if (!mem)
          error ("%s (%u:%u) is a undefined member\n", mname.str, pos2.ln,
                 pos2.ch);
        if (mem->kind != AST_DEF_VAR)
          error ("%s (%u:%u) is not a variable member\n", mname.str, pos2.ln,
                 pos2.ch);

        exp->type = AST_DEF_GET (var, mem)->type;
        break;
      }
    case AST_EXP_BIN_INDEX:
      {
        ast_exp *operand1 = AST_EXP_GET (binary, exp)->exp1;
        ast_exp *operand2 = AST_EXP_GET (binary, exp)->exp2;
        sema_check_exp (operand1, env);

        ast_pos pos1 = operand1->pos;
        ast_pos pos2 = operand2->pos;
        if (operand1->type->kind != AST_TYPE_POINTER)
          error ("%s (%u:%u) can only be used for pointer\n", "[]", pos1.ln,
                 pos1.ch);
        if (operand1->type->ref->kind == AST_TYPE_VOID)
          error ("%s (%u:%u) can not be used for void pointer\n", "[]",
                 pos1.ln, pos1.ch);
        if (!sema_exp_is_int (operand2))
          error ("%s (%u:%u) need an integer value as its offset\n", "[]",
                 pos2.ln, pos2.ch);

        exp->type = operand1->type->ref;
        break;
      }
    case AST_EXP_BIN_PLUS:
    case AST_EXP_BIN_MINUS:
    case AST_EXP_BIN_TIMES:
    case AST_EXP_BIN_DIV:
      {
        ast_exp *operand1 = AST_EXP_GET (binary, exp)->exp1;
        ast_exp *operand2 = AST_EXP_GET (binary, exp)->exp2;
        sema_check_exp (operand1, env);
        sema_check_exp (operand2, env);

        const char *fmt = "%s (%u:%u) need a number as its operand\n";
        if (!sema_exp_is_num (operand1))
          error (fmt, "+ - * /", operand1->pos.ln, operand1->pos.ch);
        if (!sema_exp_is_num (operand2))
          error (fmt, "+ - * /", operand2->pos.ln, operand2->pos.ch);

        exp->type = operand1->type->kind > operand2->type->kind
                        ? operand1->type
                        : operand2->type;
        break;
      }
    }
}

void
sema_check_id (ast_tok id, ast_env *env)
{
  vector *defs = &env->defs;
  for (size_t i = 0; i < defs->size; i++)
    {
      ast_def *def = vector_get (defs, i);
      if (sema_pos_comp (def->id.pos, id.pos) >= 0)
        return;
      if (!strcmp (id.str, def->id.str))
        error ("%s (%u:%u) has been defined\n", id.str, def->id.pos.ln,
               def->id.pos.ch);
    }
}

void
sema_check_type (ast_type *type, ast_env *env)
{
  switch (type->kind)
    {
    case AST_TYPE_UNDEF:
      {
        char *name = (char *)type->ref;
        ast_def *def = sema_seek_def2 (name, env);
        ast_def_type *get = AST_DEF_GET (type, def);

        if (!def || sema_pos_comp (get->type->pos, type->pos) >= 0)
          error ("%s (%u:%u) is undefined\n", name, type->pos.ln,
                 type->pos.ch);
        if (def->kind != AST_DEF_TYPE)
          error ("%s (%u:%u) is not a type\n", name, type->pos.ln,
                 type->pos.ch);
        free (name);

        type->kind = get->type->kind;
        type->size = get->type->size;
        type->ref = get->type->ref;
        break;
      }
    case AST_TYPE_POINTER:
      {
        sema_check_type (type->ref, env);
        break;
      }
    case AST_TYPE_UNION:
    case AST_TYPE_STRUCT:
      {
        ast_env *tenv = type->mem;

        vector *stms = &tenv->stms;
        if (stms->size != 0)
          {
            ast_stm *stm = vector_get (stms, 0);
            error ("statement (%u:%u) can not be here\n", stm->pos.ln,
                   stm->pos.ch);
          }
        vector *defs = &tenv->defs;
        for (size_t i = 0; i < defs->size; i++)
          {
            ast_def *def = vector_get (defs, i);
            if (def->kind == AST_DEF_FUNC)
              error ("function (%u:%u) can not be here\n", def->pos.ln,
                     def->pos.ch);
          }

        sema_check (tenv);

        /* set size */
        unsigned size = 1;
        if (type->kind == AST_TYPE_UNION)
          for (size_t i = 0; i < tenv->defs.size; i++)
            {
              ast_def *def = vector_get (&tenv->defs, i);
              unsigned msize = AST_DEF_GET (type, def)->type->size;
              if (def->kind == AST_DEF_VAR && msize > size)
                size = msize;
            }
        else
          for (size_t i = 0; i < tenv->defs.size; i++)
            {
              ast_def *def = vector_get (&tenv->defs, i);
              if (def->kind == AST_DEF_VAR)
                size += AST_DEF_GET (type, def)->type->size;
            }
        type->size = size;
        break;
      }
    }
}

bool
sema_exp_is_lv (ast_exp *exp)
{
  switch (exp->kind)
    {
    case AST_EXP_ELEM_ID:
    case AST_EXP_BIN_DMEM:
    case AST_EXP_BIN_PMEM:
    case AST_EXP_BIN_INDEX:
      return true;
    default:
      return false;
    }
}

bool
sema_exp_is_obj (ast_exp *exp)
{
  switch (exp->type->kind)
    {
    case AST_TYPE_UNION:
    case AST_TYPE_STRUCT:
      return true;
    default:
      return false;
    }
}

bool
sema_exp_is_num (ast_exp *exp)
{
  if (sema_exp_is_int (exp) || sema_exp_is_real (exp))
    return true;
  return false;
}

bool
sema_exp_is_int (ast_exp *exp)
{
  switch (exp->type->kind)
    {
    case AST_TYPE_INT8 ... AST_TYPE_INT64:
    case AST_TYPE_UINT8 ... AST_TYPE_UINT64:
      return true;
    default:
      return false;
    }
}

bool
sema_exp_is_real (ast_exp *exp)
{
  switch (exp->type->kind)
    {
    case AST_TYPE_FLOAT:
    case AST_TYPE_DOUBLE:
      return true;
    default:
      return false;
    }
}

bool
sema_exp_is_pobj (ast_exp *exp)
{
  if (exp->type->kind != AST_TYPE_POINTER)
    return false;

  switch (exp->type->ref->kind)
    {
    case AST_TYPE_UNION:
    case AST_TYPE_STRUCT:
      return true;
    default:
      return false;
    }
}

int
sema_pos_comp (ast_pos p1, ast_pos p2)
{
  unsigned ln1 = p1.ln;
  unsigned ch1 = p1.ch;
  unsigned ln2 = p2.ln;
  unsigned ch2 = p2.ch;
  if (ln1 > ln2)
    return 1;
  if (ln1 < ln2)
    return -1;
  if (ch1 > ch2)
    return 1;
  if (ch1 < ch2)
    return -1;
  return 0;
}

ast_def *
sema_seek_def1 (const char *name, ast_env *env)
{
  if (!env)
    return NULL;

  vector *defs = &env->defs;
  for (size_t i = 0; i < defs->size; i++)
    {
      ast_def *def = vector_get (defs, i);
      if (!strcmp (name, def->id.str))
        return def;
    }

  return NULL;
}

ast_def *
sema_seek_def2 (const char *name, ast_env *env)
{
  while (env)
    {
      ast_def *def = sema_seek_def1 (name, env);
      if (def)
        return def;
      env = env->outer;
    }
  return NULL;
}
