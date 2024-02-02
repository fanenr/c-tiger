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
        if (get->init)
          sema_check_exp (get->init, env);
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
        sema_check (get->parm);
        sema_check_type (get->type, env);
        sema_check (get->env);
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
        ast_def *def = sema_seek_def (tok.str, env);
        if (!def)
          error ("%s (%u:%u) is undefined\n", tok.str, tok.pos.ln, tok.pos.ch);
        if (def->kind != AST_DEF_VAR)
          error ("%s (%u:%u) is not a varibale\n", tok.str, tok.pos.ln,
                 tok.pos.ch);
        exp->type = AST_DEF_GET (var, def)->type;
        break;
      }
    case AST_EXP_ELEM_STR:
      {
        break;
      }
    case AST_EXP_ELEM_NUM:
      {
        ast_def *def = sema_seek_def ("int32", &prog);
        exp->type = AST_DEF_GET (type, def)->type;
        break;
      }
    case AST_EXP_ELEM_REAL:
      {
        ast_def *def = sema_seek_def ("double", &prog);
        exp->type = AST_DEF_GET (type, def)->type;
        break;
      }
    case AST_EXP_UN_UPLUS:
    case AST_EXP_UN_UMINUS:
      {
        ast_exp *operand = AST_EXP_GET (unary, exp)->exp;
        sema_check_exp (operand, env);
        int kind = operand->type->kind;
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
        if (!sema_exp_is_int (operand1))
          error ("%s (%u:%u) can only be used for integer\n", "|&^",
                 operand1->pos.ln, exp->pos.ch);
        if (!sema_exp_is_int (operand2))
          error ("%s (%u:%u) can only be used for integer\n", "|&^",
                 operand2->pos.ln, exp->pos.ch);
        exp->type = operand1->type->kind > operand2->type->kind
                        ? operand1->type
                        : operand2->type;
        break;
      }
    }
}

int
sema_check_pos (ast_pos p1, ast_pos p2)
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

void
sema_check_id (ast_tok id, ast_env *env)
{
  vector *defs = &env->defs;
  for (size_t i = 0; i < defs->size; i++)
    {
      ast_def *def = vector_get (defs, i);
      if (sema_check_pos (def->id.pos, id.pos) >= 0)
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
    case AST_TYPE_USER:
      {
        char *name = (char *)type->ref;
        ast_def *def = sema_seek_def (name, env);
        ast_def_type *get = AST_DEF_GET (type, def);

        if (!def || sema_check_pos (get->type->pos, type->pos) >= 0)
          error ("%s (%u:%u) is undefined\n", name, type->pos.ln,
                 type->pos.ch);
        if (def->kind != AST_DEF_TYPE)
          error ("%s (%u:%u) is not a type\n", name, type->pos.ln,
                 type->pos.ch);

        free (name);
        ast_pos pos = type->pos;
        *type = *get->type;
        type->pos = pos;
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
            if (def->kind == AST_DEF_VAR)
              {
                ast_exp *exp = AST_DEF_GET (var, def)->init;
                exp ? error ("expression (%u:%u) can not be here\n",
                             exp->pos.ln, exp->pos.ch)
                    : 0;
              }
            if (def->kind == AST_DEF_FUNC)
              error ("function (%u:%u) can not be here\n", def->pos.ln,
                     def->pos.ch);
          }

        sema_check (tenv);
        break;
      }
    }
}

ast_def *
sema_seek_def (const char *name, ast_env *env)
{
  vector *defs = &env->defs;
  for (;;)
    {
      for (size_t i = 0; i < defs->size; i++)
        {
          ast_def *def = vector_get (defs, i);
          if (!strcmp (name, def->id.str))
            return def;
        }
      if (!env->outer)
        return NULL;
      env = env->outer;
      defs = &env->defs;
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
