#include "ast.h"
#include "util.h"
#include "lexer.h"
#include "parser.h"
#include "tiger.y.h"
#include <stdint.h>
#include <stdarg.h>

ast_env prog;

const char *base_type_name[] = {
  [AST_TYPE_VOID] = "void",     [AST_TYPE_BOOL] = "bool",
  [AST_TYPE_INT8] = "int8",     [AST_TYPE_INT16] = "int16",
  [AST_TYPE_INT32] = "int32",   [AST_TYPE_INT64] = "int64",
  [AST_TYPE_UINT8] = "uint8",   [AST_TYPE_UINT16] = "uint16",
  [AST_TYPE_UINT32] = "uint32", [AST_TYPE_UINT64] = "uint64",
  [AST_TYPE_FLOAT] = "float",   [AST_TYPE_DOUBLE] = "double",
};

const unsigned base_type_size[] = {
  [AST_TYPE_VOID] = sizeof (void),       [AST_TYPE_BOOL] = sizeof (bool),
  [AST_TYPE_INT8] = sizeof (int8_t),     [AST_TYPE_INT16] = sizeof (int16_t),
  [AST_TYPE_INT32] = sizeof (int32_t),   [AST_TYPE_INT64] = sizeof (int64_t),
  [AST_TYPE_UINT8] = sizeof (uint8_t),   [AST_TYPE_UINT16] = sizeof (uint16_t),
  [AST_TYPE_UINT32] = sizeof (uint32_t), [AST_TYPE_UINT64] = sizeof (uint64_t),
  [AST_TYPE_FLOAT] = sizeof (float),     [AST_TYPE_DOUBLE] = sizeof (double),
};

#define VEC_PUSH_BACK(VEC, PTR)                                               \
  if (!vector_push_back (VEC, PTR))                                           \
    error ("error: vector_push_back\n");

ast_env *
ast_env_push (ast_env *env, void *ptr)
{
  if (!env)
    env = checked_alloc (sizeof (ast_env));

  int type = *(int *)ptr;
  switch (type)
    {
    case AST_DEF_ST + 1 ... AST_DEF_ED - 1:
      {
        ast_def *def = ptr;
        VEC_PUSH_BACK (&env->defs, def);
        break;
      }
    case AST_STM_ST + 1 ... AST_STM_ED - 1:
      {
        ast_stm *stm = ptr;
        VEC_PUSH_BACK (&env->stms, stm);
        break;
      }
    }

  return env;
}

ast_exp *
ast_comma_push (ast_exp *exp, ast_exp *next)
{
  if (!exp)
    {
      exp = checked_alloc (AST_EXP_SIZE (comma));
      /* set kind */
      exp->kind = AST_EXP_COMMA;
      /* set pos */
      exp->pos = next->pos;
    }

  ast_exp_comma *get = AST_EXP_GET (comma, exp);
  /* push next exp */
  VEC_PUSH_BACK (&get->exps, next);

  return exp;
}

ast_type *
ast_type_push (ast_tok tok, ast_type *type)
{
  ast_type *new = checked_alloc (sizeof (ast_type));
  new->pos = tok.pos;

  switch (tok.kind)
    {
    case ID:
      {
        /* new->kind */
        /* new->size */
        new->ref = (ast_type *)tok.str;
        break;
      }
    case TIMES:
      {
        new->kind = AST_TYPE_POINTER;
        new->size = sizeof (void *);
        new->ref = type;
        break;
      }
    case LBRACK:
      {
        new->kind = AST_TYPE_ARRAY;
        /* new->size */
        new->ref = type;
        break;
      }
    }

  return new;
}

ast_env *
ast_parm_push (ast_env *env, ast_tok id, ast_type *type)
{
  if (!env)
    env = checked_alloc (sizeof (ast_env));

  ast_def *def = checked_alloc (AST_DEF_SIZE (var));
  def->kind = AST_DEF_VAR;
  def->pos = id.pos;
  def->id = id;

  ast_def_var *get = AST_DEF_GET (var, def);
  get->type = type;

  return ast_env_push (env, def);
}

ast_def *
ast_def_new (int type, ...)
{
  static unsigned sizes[] = { [AST_DEF_VAR] = AST_DEF_SIZE (var),
                              [AST_DEF_TYPE] = AST_DEF_SIZE (type),
                              [AST_DEF_FUNC] = AST_DEF_SIZE (func) };

  va_list ap;
  va_start (ap, type);
  ast_tok tok = va_arg (ap, ast_tok);

  ast_def *def = checked_alloc (sizes[type]);
  def->kind = type;
  def->pos = tok.pos;

  switch (type)
    {
    case AST_DEF_VAR:
      {
        ast_tok id = va_arg (ap, ast_tok);
        ast_type *type = va_arg (ap, ast_type *);
        ast_exp *exp = va_arg (ap, ast_exp *);
        /* set id */
        def->id = id;
        ast_def_var *get = AST_DEF_GET (var, def);
        /* set type */
        get->type = type;
        /* set init */
        get->init = exp;
        break;
      }
    case AST_DEF_TYPE:
      {
        ast_tok id = va_arg (ap, ast_tok);
        void *ptr = va_arg (ap, void *);
        switch (tok.kind)
          {
          case TYPE:
            {
              /* set id */
              def->id = id;
              ast_def_type *get = AST_DEF_GET (type, def);
              /* set type */
              ast_type *type = ptr;
              get->type = type;
              break;
            }
          case UNION:
          case STRUCT:
            {
              /* set id */
              def->id = id;
              ast_type *type = checked_alloc (sizeof (ast_type));
              type->kind
                  = tok.kind == UNION ? AST_TYPE_UNION : AST_TYPE_STRUCT;
              type->pos = tok.pos;
              /* type->size */
              /* set mem */
              ast_env *mem = ptr;
              type->mem = mem;
              ast_def_type *get = AST_DEF_GET (type, def);
              /* set type */
              get->type = type;
              break;
            }
          }
        break;
      }
    case AST_DEF_FUNC:
      {
        ast_tok id = va_arg (ap, ast_tok);
        ast_env *parm = va_arg (ap, ast_env *);
        ast_type *type = va_arg (ap, ast_type *);
        ast_env *env = va_arg (ap, ast_env *);
        /* set id */
        def->id = id;
        ast_def_func *get = AST_DEF_GET (func, def);
        /* set parm */
        get->parm = parm;
        /* set type */
        get->type = type;
        /* set env */
        get->env = env;
        break;
      }
    }

  va_end (ap);
  return def;
}

ast_stm *
ast_stm_new (int type, ...)
{
  static unsigned sizes[]
      = { [AST_STM_RETURN] = AST_STM_SIZE (return),
          [AST_STM_ASSIGN] = AST_STM_SIZE (assign),
          [AST_STM_WHILE] = AST_STM_SIZE (while),
          [(AST_STM_IF_ST + 1)...(AST_STM_IF_ED - 1)] = AST_STM_SIZE (if),
        };

  va_list ap;
  va_start (ap, type);

  ast_stm *stm = checked_alloc (sizes[type]);
  stm->kind = type;

  switch (type)
    {
    case AST_STM_RETURN:
      {
        ast_tok tok = va_arg (ap, ast_tok);
        ast_exp *exp = va_arg (ap, ast_exp *);
        ast_stm_return *get = AST_STM_GET (return, stm);
        /* set pos */
        stm->pos = tok.pos;
        /* set exp */
        get->exp = exp;
        break;
      }
    case AST_STM_ASSIGN:
      {
        ast_exp *obj = va_arg (ap, ast_exp *);
        ast_exp *val = va_arg (ap, ast_exp *);
        ast_stm_assign *get = AST_STM_GET (assign, stm);
        /* set pos */
        stm->pos = obj->pos;
        /* set obj */
        get->obj = obj;
        /* set exp */
        get->exp = val;
        break;
      }
    case AST_STM_WHILE:
      {
        ast_tok tok = va_arg (ap, ast_tok);
        ast_exp *exp = va_arg (ap, ast_exp *);
        ast_env *env = va_arg (ap, ast_env *);
        ast_stm_while *get = AST_STM_GET (while, stm);
        /* set pos */
        stm->pos = tok.pos;
        /* set exp */
        get->exp = exp;
        /* set env */
        get->env = env;
        break;
      }
    case AST_STM_IF_ST + 1 ... AST_STM_IF_ED - 1:
      {
        ast_tok tok = va_arg (ap, ast_tok);
        ast_exp *exp = va_arg (ap, ast_exp *);
        ast_env *env1 = va_arg (ap, ast_env *);
        ast_env *env2 = va_arg (ap, ast_env *);
        ast_stm_if *get = AST_STM_GET (if, stm);
        /* set pos */
        stm->pos = tok.pos;
        /* set exp */
        get->exp = exp;
        /* set env */
        get->then_env = env1;
        get->else_env = env2;
        break;
      }
    }

  va_end (ap);
  return stm;
}

ast_exp *
ast_exp_new (int type, ...)
{
  static unsigned sizes[] = {
    [(AST_EXP_ELEM_ST + 1)...(AST_EXP_ELEM_ED - 1)] = AST_EXP_SIZE (elem),
    [(AST_EXP_UN_ST + 1)...(AST_EXP_UN_ED - 1)] = AST_EXP_SIZE (unary),
    [(AST_EXP_BIN_ST + 1)...(AST_EXP_BIN_ED - 1)] = AST_EXP_SIZE (binary),
  };

  ast_exp *exp = checked_alloc (sizes[type]);
  exp->kind = type;

  va_list ap;
  va_start (ap, type);

  switch (type)
    {
    case AST_EXP_ELEM_ST + 1 ... AST_EXP_ELEM_ED - 1:
      {
        ast_tok tok = va_arg (ap, ast_tok);
        /* set pos */
        exp->pos = tok.pos;
        /* exp->type */
        /* set elem */
        ast_exp_elem *get = AST_EXP_GET (elem, exp);
        get->elem = tok;
        break;
      }
    case AST_EXP_UN_ST + 1 ... AST_EXP_UN_ED - 1:
      {
        ast_tok tok = va_arg (ap, ast_tok);
        ast_exp *val = va_arg (ap, ast_exp *);
        /* set pos */
        exp->pos = tok.pos;
        /* exp->type */
        /* set exp */
        ast_exp_unary *get = AST_EXP_GET (unary, exp);
        get->exp = val;
        break;
      }
    case AST_EXP_BIN_ST + 1 ... AST_EXP_BIN_ED - 1:
      {
        ast_exp *exp1 = va_arg (ap, ast_exp *);
        ast_exp *exp2 = va_arg (ap, ast_exp *);
        /* set pos */
        exp->pos = exp1->pos;
        /* exp->type */
        /* set exps */
        ast_exp_binary *get = AST_EXP_GET (binary, exp);
        get->exp1 = exp1;
        get->exp2 = exp2;
        break;
      }
    }

  va_end (ap);
  return exp;
}

void
ast_env_init (void)
{
  ast_def *def;
  ast_def_type *get;
  ast_tok tok = { 0 };

  for (int i = AST_TYPE_BASE_ST + 1; i <= AST_TYPE_BASE_ED - 1; i++)
    {
      /* set kind */
      def = checked_alloc (AST_DEF_SIZE (type));
      def->kind = AST_DEF_TYPE;
      /* set pos */
      def->pos = tok.pos;
      /* set id */
      tok.str = checked_strdup (base_type_name[i]);
      def->id = tok;

      ast_type *type = checked_alloc (sizeof (ast_type));
      type->size = base_type_size[i];
      type->kind = i;

      /* set type */
      get = AST_DEF_GET (type, def);
      get->type = type;

      ast_env_push (&prog, def);
    }
}

void
yyerror (const char *msg)
{
  error ("error occured at %u:%u: %s\n", m_pos.ln, m_pos.ch, msg);
}
