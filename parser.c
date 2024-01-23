#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

extern char *string (const char *str);
extern void *checked_malloc (size_t size);
extern void *checked_realloc (void *ptr, size_t size);

ast_env *m_env;
ast_parm *m_parm;
ast_type *m_type;

void
ast_type_push (int cond, void *ptr)
{
  switch (cond)
    {
    /* type: ID */
    case 1:
      {
        char *name = ptr;
        ast_def *defined = ast_def_seek (pos, name);
        if (!defined || defined->kind != AST_DEF_TYPE)
          error ("use undefined type %s\n", name);
        ast_def_type *get = AST_DEF_GET (type, defined);
        m_type = checked_malloc (sizeof (ast_type));
        memcpy (m_type, get->type, sizeof (ast_type));
        break;
      }
    /* type: TIMES ID */
    case 2:
      {
        m_type->kind = AST_TYPE_POINTER;
        m_type->size = sizeof (void *);
        m_type->ref = m_type;
        break;
      }
    /* type: LBRACK RBRACK type */
    case 3:
      {
        m_type->kind = AST_TYPE_ARRAY;
        m_type->size = sizeof (void *);
        m_type->ref = m_type;
        break;
      }
    default:
      break;
    }
}

ast_type *
ast_type_clear (void)
{
  ast_type *ret = m_type;
  m_type = NULL;
  return ret;
}

void
ast_parm_push (int cond, void *ptr)
{
  switch (cond)
    {
    /* parm: ID COLON type */
    case 1:
      {
        m_parm = checked_malloc (sizeof (ast_parm));
        m_parm->list = checked_malloc (48 * sizeof (ast_parm *));
        m_parm->list[0] = ptr;
        m_parm->num = 1;
        break;
      }
    /* parm: parm COMMA ID COLON type */
    case 2:
      {
        m_parm->list[m_parm->num++] = ptr;
        break;
      }
    default:
      break;
    }
}

ast_parm *
ast_parm_clear (void)
{
  ast_parm *ret = m_parm;
  m_parm = NULL;
  return ret;
}

void
ast_env_push (int cond, void *ptr)
{
  int kind = *(int *)ptr;
  switch (cond)
    {
    /* bloc: bloc_elem */
    case 1:
      {
        ast_env *n_env = checked_malloc (sizeof (ast_env));
        n_env->outer = m_env;
        if (kind > AST_DEF_ST && kind < AST_DEF_ED)
          m_env->defs.num--;
        if (kind > AST_STM_ST && kind < AST_STM_ED)
          m_env->stms.num--;
        m_env = n_env;
        __attribute__ ((fallthrough));
      }
    /* bloc: bloc bloc_elem */
    case 2:
      {
        if (kind > AST_DEF_ST && kind < AST_DEF_ED)
          {
            m_env->defs.list = checked_realloc (
                m_env->defs.list, (m_env->defs.num + 1) * sizeof (ast_def *));
            m_env->defs.list[m_env->defs.num++] = ptr;
          }
        if (kind > AST_STM_ST && kind < AST_STM_ED)
          {
            m_env->stms.list = checked_realloc (
                m_env->stms.list, (m_env->stms.num + 1) * sizeof (ast_stm *));
            m_env->stms.list[m_env->stms.num++] = ptr;
          }
        break;
      }
    default:
      break;
    }
}

ast_env *
ast_env_clear (void)
{
  ast_env *ret = m_env;
  m_env = ret->outer;
  return ret;
}

ast_def *
ast_def_new (int type, ast_pos pos, ...)
{
  static unsigned sizes[] = { [AST_DEF_VAR] = AST_DEF_SIZE (var),
                              [AST_DEF_TYPE] = AST_DEF_SIZE (type),
                              [AST_DEF_FUNC] = AST_DEF_SIZE (func) };

  ast_def *ret = checked_malloc (sizes[type]);
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
        /* check and set id */
        char *id = va_arg (ap, char *);
        ast_def *defined = ast_def_seek (pos, id);
        if (defined != NULL)
          error ("identifier %s has been defined here %u:%u\n", id,
                 defined->pos.ln, defined->pos.ch);
        ret->id = id;
        /* set type and init exp */
        ast_type *type = va_arg (ap, ast_type *);
        ast_exp *init = va_arg (ap, ast_exp *);
        get->type = type;
        get->init = init;
        /* push to env */
        ast_env_push (2, ret);
        break;
      }
    /* def_type: TYPE ID EQ type SEMI */
    case AST_DEF_TYPE:
      {
        ast_def_type *get = AST_DEF_GET (type, ret);
        /* check and set id */
        char *id = va_arg (ap, char *);
        ast_def *defined = ast_def_seek (pos, id);
        if (defined != NULL)
          error ("identifier %s has been defined here %u:%u\n", id,
                 defined->pos.ln, defined->pos.ch);
        ret->id = id;
        /* set type */
        ast_type *type = va_arg (ap, ast_type *);
        get->type = type;
        /* push to env */
        ast_env_push (2, ret);
        break;
      }
    /* def_func: FUNC ID LPAREN parm RPAREN type LBRACE bloc RBRACE */
    case AST_DEF_FUNC:
      {
        ast_def_func *get = AST_DEF_GET (func, ret);
        /* check and set id */
        char *id = va_arg (ap, char *);
        ast_def *defined = ast_def_seek (pos, id);
        if (defined != NULL)
          error ("identifier %s has been defined here %u:%u\n", id,
                 defined->pos.ln, defined->pos.ch);
        ret->id = id;
        /* set parm and type */
        ast_parm *parm = va_arg (ap, ast_parm *);
        ast_type *type = va_arg (ap, ast_type *);
        get->parm = parm;
        get->type = type;
        /* set env */
        ast_env *env = va_arg (ap, ast_env *);
        get->env = env;
        /* push to env */
        ast_env_push (2, ret);
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

  ast_stm *ret = checked_malloc (sizes[type]);
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
        /* check and set id */
        char *id = va_arg (ap, char *);
        ast_def *defined = ast_def_seek (pos, id);
        if (id == NULL)
          error ("use undefined identifier %s\n", id);
        get->var = defined;
        /* set exp */
        ast_exp *exp = va_arg (ap, ast_exp *);
        get->exp = exp;
        /* push to env */
        ast_env_push (2, ret);
        break;
      }
    /* stm_while: WHILE LPAREN exp RPAREN LBRACE bloc RBRACE */
    case AST_STM_WHILE:
      {
        ast_stm_while *get = AST_STM_GET (while, ret);
        /* set exp */
        ast_exp *exp = va_arg (ap, ast_exp *);
        get->exp = exp;
        /* set env */
        ast_env *env = va_arg (ap, ast_env *);
        get->env = env;
        /* push to env */
        ast_env_push (2, ret);
        break;
      }
    /* stm_if: IF LPAREN exp RPAREN LBRACE bloc RBRACE ELSE LBRACE bloc RBRACE
     */
    case AST_STM_IF_ST + 1 ... AST_STM_IF_ED - 1:
      {
        ast_stm_if *get = AST_STM_GET (if, ret);
        /* set exp */
        ast_exp *exp = va_arg (ap, ast_exp *);
        get->exp = exp;
        /* set envs */
      }
    default:
      error ("unknown stm type %d\n", type);
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

  ast_exp *ret = checked_malloc (sizes[type]);
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
        /* check and set id */
        char *id = va_arg (ap, char *);
        ast_def *defined = ast_def_seek (pos, id);
        if (defined == NULL)
          error ("use undefined identifier %s\n", id);
        get->id = defined;
        break;
      }
    /* exp_elem: NUM */
    case AST_EXP_ELEM_NUM:
      {
        ast_exp_elem *get = AST_EXP_GET (elem, ret);
        get->num = va_arg (ap, long);
        break;
      }
    /* exp_elem: STR */
    case AST_EXP_ELEM_STR:
      {
        ast_exp_elem *get = AST_EXP_GET (elem, ret);
        get->string = va_arg (ap, char *);
        break;
      }
    /* exp_elem: REAL */
    case AST_EXP_ELEM_REAL:
      {
        ast_exp_elem *get = AST_EXP_GET (elem, ret);
        get->real = va_arg (ap, double);
        break;
      }
    /* exp_un_uminus: MINUS exp */
    case AST_EXP_UN_UMINUS:
      {
        ast_exp *exp1 = AST_EXP_NEW (ELEM_NUM, pos, 0);
        ast_exp *exp2 = va_arg (ap, ast_exp *);
        ret = AST_EXP_NEW (BIN_MINUS, pos, exp1, exp2);
        break;
      }
    /* exp_binary: exp OPER exp */
    case AST_EXP_BIN_MATH_ST + 1 ... AST_EXP_BIN_MATH_ED - 1:
    case AST_EXP_BIN_LOGIC_ST + 1 ... AST_EXP_BIN_LOGIC_ED - 1:
      {
        ast_exp_binary *get = AST_EXP_GET (binary, ret);
        /* set exp */
        ast_exp *exp1 = va_arg (ap, ast_exp *);
        ast_exp *exp2 = va_arg (ap, ast_exp *);
        get->exp1 = exp1;
        get->exp2 = exp2;
        break;
      }
    default:
      error ("unknown exp type %d\n", type);
    }

  va_end (ap);
  return ret;
}

ast_def *
ast_def_seek (ast_pos pos, char *name)
{
  ast_env *now = m_env;
  unsigned num = now->defs.num;
  ast_def **list = now->defs.list;

find:
  for (unsigned i = 0; i < num; i++)
    {
      if (list[i]->pos.ln >= pos.ln)
        break;
      if (strcmp (name, list[i]->id) == 0)
        return list[i];
    }

  if (now->outer == NULL)
    return NULL;

  now = now->outer;
  num = now->defs.num;
  list = now->defs.list;

  goto find;
}

void
ast_env_init (void)
{
  m_env = checked_malloc (sizeof (ast_env));
  memset (m_env, 0, sizeof (ast_env));

  static const char *names[] = {
    /* base */
    [AST_TYPE_VOID] = "void",     [AST_TYPE_INT8] = "int8",
    [AST_TYPE_INT16] = "int16",   [AST_TYPE_INT32] = "int32",
    [AST_TYPE_INT64] = "int64",   [AST_TYPE_UINT8] = "uint8",
    [AST_TYPE_UINT16] = "uint16", [AST_TYPE_UINT32] = "uint32",
    [AST_TYPE_UINT64] = "uint64", [AST_TYPE_FLOAT] = "float",
    [AST_TYPE_DOUBLE] = "double",
  };

  static unsigned sizes[] = {
    [AST_TYPE_VOID] = sizeof (void),
    [AST_TYPE_INT8] = sizeof (int8_t),
    [AST_TYPE_INT16] = sizeof (int16_t),
    [AST_TYPE_INT32] = sizeof (int32_t),
    [AST_TYPE_INT64] = sizeof (int64_t),
    [AST_TYPE_UINT8] = sizeof (uint8_t),
    [AST_TYPE_UINT16] = sizeof (uint16_t),
    [AST_TYPE_UINT32] = sizeof (uint32_t),
    [AST_TYPE_UINT64] = sizeof (uint64_t),
    [AST_TYPE_FLOAT] = sizeof (float),
    [AST_TYPE_DOUBLE] = sizeof (double),
  };

  ast_def *def;
  ast_def_type *get;
  ast_pos origin = { 0 };

  for (int i = AST_TYPE_BASE_ST + 1; i <= AST_TYPE_BASE_ED - 1; i++)
    {
      def = checked_malloc (AST_DEF_SIZE (type));
      def->id = string (names[i]);
      def->kind = AST_DEF_TYPE;
      def->pos = origin;

      ast_type *type = checked_malloc (sizeof (ast_type));
      type->size = sizes[i];
      type->kind = i;

      /* set type */
      get = AST_DEF_GET (type, def);
      get->type = type;

      ast_env_push (2, def);
    }
}

void
yyerror (const char *pos)
{
  error ("%s", pos);
}
