#include "ast.h"
#include "util.h"
#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

ast_env *m_env;
ast_env *m_parm;
ast_type *m_type;

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

void
ast_type_push (ast_type **type, int cond, void *ptr)
{
  switch (cond)
    {
    /* type: ID */
    case 1:
      {
        char *name = ptr;
        ast_type *get = checked_alloc (sizeof (ast_type));
        get->kind = AST_TYPE_USER;
        get->size = 0;
        for (int i = AST_TYPE_BASE_ST + 1; i <= AST_TYPE_BASE_ED - 1; i++)
          if (strcmp (name, base_type_name[i]) == 0)
            {
              get->kind = i;
              get->size = base_type_size[i];
            }
        *type = get;
        break;
      }
    /* type: TIMES ID */
    case 2:
      {
        ast_type *get = checked_alloc (sizeof (ast_type));
        get->kind = AST_TYPE_POINTER;
        get->size = sizeof (void *);
        get->ref = *type;
        *type = get;
        break;
      }
    /* type: LBRACK RBRACK type */
    case 3:
      {
        ast_type *get = checked_alloc (sizeof (ast_type));
        get->kind = AST_TYPE_ARRAY;
        get->size = sizeof (void *);
        get->ref = *type;
        *type = get;
        break;
      }
    }
}

void
ast_parm_push (ast_env **env, int cond, char *name, ast_type *type)
{
  switch (cond)
    {
    /* parm: ID COLON type */
    case 1:
      {
        ast_env *get = checked_alloc (sizeof (ast_env));
        get->outer = GENV;
        *env = get;
        __attribute__ ((fallthrough));
      }
    /* parm: parm COMMA ID COLON type */
    case 2:
      {
        ast_def *def = checked_alloc (AST_DEF_SIZE (var));
        def->kind = AST_DEF_VAR;
        def->pos = m_pos;
        def->id = name;
        ast_def_var *get = AST_DEF_GET (var, def);
        get->type = type;
        ast_env_push (env, 2, def);
        break;
      }
    }
}

void
ast_env_push (ast_env **env, int cond, void *ptr)
{
  int kind = *(int *)ptr;
  switch (cond)
    {
    /* bloc: bloc_elem */
    case 1:
      {
        ast_env *old = *env;
        if (kind > AST_DEF_ST && kind < AST_DEF_ED)
          old->defs.num--;
        if (kind > AST_STM_ST && kind < AST_STM_ED)
          old->stms.num--;
        ast_env *new = checked_alloc (sizeof (ast_env));
        new->outer = old;
        *env = new;
        __attribute__ ((fallthrough));
      }
    /* bloc: bloc bloc_elem */
    case 2:
      {
        ast_env *now = *env;
        if (kind > AST_DEF_ST && kind < AST_DEF_ED)
          {
            now->defs.list = checked_realloc (
                now->defs.list, (now->defs.num + 1) * sizeof (ast_def *));
            now->defs.list[now->defs.num++] = ptr;
          }
        if (kind > AST_STM_ST && kind < AST_STM_ED)
          {
            now->stms.list = checked_realloc (
                now->stms.list, (now->stms.num + 1) * sizeof (ast_stm *));
            now->stms.list[now->stms.num++] = ptr;
          }
        break;
      }
    }
}

ast_def *
ast_def_new (int type, ast_pos pos, ...)
{
  static unsigned sizes[] = { [AST_DEF_VAR] = AST_DEF_SIZE (var),
                              [AST_DEF_TYPE] = AST_DEF_SIZE (type),
                              [AST_DEF_FUNC] = AST_DEF_SIZE (func) };

  ast_def *ret = checked_alloc (sizes[type]);
  ret->kind = type;
  ret->pos = pos;

  va_list ap;
  va_start (ap, pos);

  switch (type)
    {
    /* def_var: VAR ID COLON type EQ exp SEMI */
    case AST_DEF_VAR:
      {
        ast_def_var *get = AST_DEF_GET (var, ret);
        char *id = va_arg (ap, char *);
        ast_type *type = va_arg (ap, ast_type *);
        ast_exp *init = va_arg (ap, ast_exp *);
        /* set id */
        ret->id = id;
        /* set type */
        get->type = type;
        /* set init exp */
        get->init = init;
        /* push to env */
        GENV_PUSH (2, ret);
        break;
      }
    /* def_type: TYPE ID EQ type SEMI */
    case AST_DEF_TYPE:
      {
        ast_def_type *get = AST_DEF_GET (type, ret);
        int kind = va_arg (ap, int);
        char *id = va_arg (ap, char *);
        void *ptr = va_arg (ap, void *);
        /* set id */
        ret->id = id;
        /* set type */
        ast_type *type = ptr;
        if (kind != 1)
          {
            type = checked_alloc (sizeof (ast_type));
            type->kind = kind == 2 ? AST_TYPE_UNION : AST_TYPE_STRUCT;
            type->mem = ptr;
          }
        get->type = type;
        /* push to env */
        GENV_PUSH (2, ret);
        break;
      }
    /* def_func: FUNC ID LPAREN parm RPAREN type LBRACE bloc RBRACE */
    case AST_DEF_FUNC:
      {
        ast_def_func *get = AST_DEF_GET (func, ret);
        char *id = va_arg (ap, char *);
        ast_env *parm = va_arg (ap, ast_env *);
        ast_type *type = va_arg (ap, ast_type *);
        ast_env *env = va_arg (ap, ast_env *);
        /* set id */
        ret->id = id;
        /* set type */
        get->type = type;
        /* set parm */
        get->parm_num = 0;
        if (env && parm)
          {
            get->parm_num = parm->defs.num;
            for (int i = 0; i < parm->defs.num; i++)
              ast_env_push (&env, 2, parm->defs.list + i);
            free (parm);
          }
        /* set env */
        get->env = env;
        /* push to env */
        GENV = env->outer;
        GENV_PUSH (2, ret);
        break;
      }
    }

  va_end (ap);
  return ret;
}

ast_stm *
ast_stm_new (int type, ast_pos pos, ...)
{
  static unsigned sizes[]
      = { [AST_STM_ASSIGN] = AST_STM_SIZE (assign),
          [AST_STM_WHILE] = AST_STM_SIZE (while),
          [(AST_STM_IF_ST + 1)...(AST_STM_IF_ED - 1)] = AST_STM_SIZE (if),
        };

  ast_stm *ret = checked_alloc (sizes[type]);
  ret->kind = type;
  ret->pos = pos;

  va_list ap;
  va_start (ap, pos);

  switch (type)
    {
    /* stm_assign: ID EQ exp SEMI */
    case AST_STM_ASSIGN:
      {
        ast_stm_assign *get = AST_STM_GET (assign, ret);
        char *id = va_arg (ap, char *);
        ast_exp *exp = va_arg (ap, ast_exp *);
        /* set id */
        get->var = (ast_def *)id;
        /* set exp */
        get->exp = exp;
        /* push to env */
        GENV_PUSH (2, ret);
        break;
      }
    /* stm_while: WHILE LPAREN exp RPAREN LBRACE bloc RBRACE */
    case AST_STM_WHILE:
      {
        ast_stm_while *get = AST_STM_GET (while, ret);
        ast_exp *exp = va_arg (ap, ast_exp *);
        ast_env *env = va_arg (ap, ast_env *);
        /* set exp */
        get->exp = exp;
        /* set env */
        get->env = env;
        /* push to env */
        GENV = env->outer;
        GENV_PUSH (2, ret);
        break;
      }
    /* stm_if: IF LPAREN exp RPAREN LBRACE bloc RBRACE ELSE LBRACE bloc RBRACE
     */
    case AST_STM_IF_ST + 1 ... AST_STM_IF_ED - 1:
      {
        ast_stm_if *get = AST_STM_GET (if, ret);
        ast_exp *exp = va_arg (ap, ast_exp *);
        ast_env *env1 = va_arg (ap, ast_env *);
        ast_env *env2 = va_arg (ap, ast_env *);
        /* set exp */
        get->exp = exp;
        /* set env */
        get->then_env = env1;
        get->else_env = env2;
        if (env2)
          {
            env1 = env2->outer;
            env2->outer = env1->outer;
            get->then_env = env1;
          }
        /* push to env */
        GENV = env1->outer;
        GENV_PUSH (2, ret);
        break;
      }
    }

  va_end (ap);
  return ret;
}

ast_exp *
ast_exp_new (int type, ast_pos pos, ...)
{
  static unsigned sizes[] = {
    [(AST_EXP_ELEM_ST + 1)...(AST_EXP_ELEM_ED - 1)] = AST_EXP_SIZE (elem),
    [AST_EXP_UN_UMINUS] = AST_EXP_SIZE (binary),
    [(AST_EXP_BIN_ST + 1)...(AST_EXP_BIN_ED - 1)] = AST_EXP_SIZE (binary),
  };

  ast_exp *ret = checked_alloc (sizes[type]);
  ret->kind = type;
  ret->pos = pos;

  va_list ap;
  va_start (ap, pos);

  switch (type)
    {
    /* exp_elem: ID */
    case AST_EXP_ELEM_ID:
      {
        ast_exp_elem *get = AST_EXP_GET (elem, ret);
        char *id = va_arg (ap, char *);
        /* set id */
        get->id = (ast_def *)id;
        break;
      }
    /* exp_elem: NUM */
    case AST_EXP_ELEM_NUM:
      {
        ast_exp_elem *get = AST_EXP_GET (elem, ret);
        long num = va_arg (ap, long);
        /* set num */
        get->num = num;
        break;
      }
    /* exp_elem: STR */
    case AST_EXP_ELEM_STR:
      {
        ast_exp_elem *get = AST_EXP_GET (elem, ret);
        char *string = va_arg (ap, char *);
        /* set string */
        get->string = string;
        break;
      }
    /* exp_elem: REAL */
    case AST_EXP_ELEM_REAL:
      {
        ast_exp_elem *get = AST_EXP_GET (elem, ret);
        double real = va_arg (ap, double);
        /* set real */
        get->real = real;
        break;
      }
    /* exp_unary: OP exp */
    case AST_EXP_UN_ST + 1 ... AST_EXP_UN_ED - 1:
      {
        ast_exp_unary *get = AST_EXP_GET (unary, ret);
        ast_exp *exp = va_arg (ap, ast_exp *);
        /* set exp */
        get->exp = exp;
        break;
      }
    /* exp_binary: exp OP exp */
    case AST_EXP_BIN_BIT_ST + 1 ... AST_EXP_BIN_BIT_ED - 1:
    case AST_EXP_BIN_MEM_ST + 1 ... AST_EXP_BIN_MEM_ED - 1:
    case AST_EXP_BIN_MATH_ST + 1 ... AST_EXP_BIN_MATH_ED - 1:
    case AST_EXP_BIN_LOGIC_ST + 1 ... AST_EXP_BIN_LOGIC_ED - 1:
      {
        ast_exp_binary *get = AST_EXP_GET (binary, ret);
        ast_exp *exp1 = va_arg (ap, ast_exp *);
        ast_exp *exp2 = va_arg (ap, ast_exp *);
        /* set exp */
        get->exp1 = exp1;
        get->exp2 = exp2;
        break;
      }
    }

  va_end (ap);
  return ret;
}

void
ast_env_init (void)
{
  m_env = checked_alloc (sizeof (ast_env));

  ast_def *def;
  ast_def_type *get;
  ast_pos origin = { 0 };

  for (int i = AST_TYPE_BASE_ST + 1; i <= AST_TYPE_BASE_ED - 1; i++)
    {
      def = checked_alloc (AST_DEF_SIZE (type));
      def->id = checked_strdup (base_type_name[i]);
      def->kind = AST_DEF_TYPE;
      def->pos = origin;

      ast_type *type = checked_alloc (sizeof (ast_type));
      type->size = base_type_size[i];
      type->kind = i;

      /* set type */
      get = AST_DEF_GET (type, def);
      get->type = type;

      GENV_PUSH (2, def);
    }
}

void
yyerror (const char *pos)
{
  error (m_pos, "%s", pos);
}
