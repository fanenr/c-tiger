#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

extern char *string (const char *str);
extern void *checked_malloc (size_t size);
extern void *checked_realloc (void *ptr, size_t size);

static ast_env *m_env;
static ast_parm *m_parm;
static ast_type *m_type;

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
        m_type->name = name;
        break;
      }
    /* type: TIMES ID */
    case 2:
      {
        m_type->kind = AST_TYPE_POINTER;
        m_type->size = sizeof (void *);
        m_type->ref = m_type;
        char *name = checked_malloc (strlen (m_type->name) + 2);
        name[0] = '*';
        strcat (name + 1, m_type->name);
        free (m_type->name);
        break;
      }
    /* type: LBRACK RBRACK type */
    case 3:
      {
        m_type->kind = AST_TYPE_ARRAY;
        m_type->size = sizeof (void *);
        m_type->ref = m_type;
        char *name = checked_malloc (strlen (m_type->name) + 3);
        strcat (name, "[]");
        strcat (name + 2, m_type->name);
        free (m_type->name);
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
    case AST_DEF_VAR:
      {
        ast_def_var *get = AST_DEF_GET (var, ret);
        /* check and set type */
        char *type = va_arg (ap, char *);
        ast_def *defined = ast_def_seek (pos, type);
        if (defined == NULL)
          error ("use undefined type %s\n", type);
        get->type = defined;
        /* check and set id */
        char *id = va_arg (ap, char *);
        defined = ast_def_seek (pos, id);
        if (defined != NULL)
          error ("identifier %s has been defined here %u:%u\n",
                 defined->pos.ln, defined->pos.ch);
        ret->id = id;
        /* set init exp */
        get->init = va_arg (ap, ast_exp *);
        ast_env_add (ret);
        break;
      }
    case AST_DEF_TYPE:
      {
        ast_def_type *get = AST_DEF_GET (type, ret);
        /* check and set id */
        char *id = va_arg (ap, char *);
        ast_def *defined = ast_def_seek (pos, id);
        if (defined != NULL)
          error ("identifier %s has been defined here %u:%u\n",
                 defined->pos.ln, defined->pos.ch);
        ret->id = id;
        /* check and set origin */
        char *type = va_arg (ap, char *);
        defined = ast_def_seek (pos, type);
        if (defined == NULL)
          error ("the original type %s is undefined\n");
        get->origin = defined;
        ast_env_add (ret);
        break;
      }
    case AST_DEF_FUNC:
      {
        ast_def_func *get = AST_DEF_GET (func, ret);
        /* check and set return type */
        char *type = va_arg (ap, char *);
        ast_def *defined = ast_def_seek (pos, type);
        if (defined == NULL)
          error ("use undefined type %s\n", type);
        get->type = defined;
        /* check and set id */
        char *id = va_arg (ap, char *);
        defined = ast_def_seek (pos, id);
        if (defined != NULL)
          error ("identifier %s has been defined here %u:%u\n",
                 defined->pos.ln, defined->pos.ch);
        ret->id = id;
        /* set parms */
        get->parm = va_arg (ap, int) ? ast_parm_get () : NULL;
        /* set env */
        if (va_arg (ap, int))
          {
            get->env = glob_env;
            glob_env = glob_env->outer;
            ast_env_add (ret);
          }
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
    case AST_STM_ASSIGN:
      {
        ast_stm_assign *get = AST_STM_GET (assign, ret);
        ast_def *id = ast_def_seek (pos, va_arg (ap, char *));
        if (id == NULL)
          error ("use undefined identifier %s\n", id);
        ast_exp *exp = va_arg (ap, ast_exp *);
        get->id = id;
        ast_env_add (ret);
        break;
      }
    case AST_STM_WHILE:
      {
        ast_stm_while *get = AST_STM_GET (while, ret);
        get->exp = va_arg (ap, ast_exp *);
        /* set env */
        if (va_arg (ap, int))
          {
            get->env = glob_env;
            glob_env = glob_env->outer;
          }
        ast_env_add (ret);
        break;
      }
    case AST_STM_IF_ST + 1 ... AST_STM_IF_ED - 1:
      {
        ast_stm_if *get = AST_STM_GET (if, ret);
        get->exp = va_arg (ap, ast_exp *);
        /* set env */
        int cases = va_arg (ap, int) + va_arg (ap, int);
        if (cases == 1 + 1)
          {
            /* both then and else */
            get->else_env = glob_env;
            glob_env = glob_env->outer;
            get->else_env->outer = glob_env->outer;
            get->then_env = glob_env;
            glob_env->outer = glob_env->outer;
          }
        if (cases == 1 + 0)
          {
            /* only then */
            get->then_env = glob_env;
            glob_env->outer = glob_env->outer;
          }
        ast_env_add (ret);
        break;
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
    [AST_EXP_UNARY_UMINUS] = AST_EXP_SIZE (binary),
    [(AST_EXP_BINARY_ST + 1)...(AST_EXP_BINARY_ED - 1)]
    = AST_EXP_SIZE (binary),
  };

  ast_exp *ret = checked_malloc (sizes[type]);
  ret->kind = type;
  ret->pos = pos;

  va_list ap;
  va_start (ap, pos);

  switch (type)
    {
    case AST_EXP_ELEM_ID:
      {
        ast_exp_elem *get = AST_EXP_GET (elem, ret);
        char *id = va_arg (ap, char *);
        ast_def *defined = ast_def_seek (pos, id);
        if (defined == NULL)
          error ("use undefined identifier %s\n", id);
        get->id = defined;
        break;
      }
    case AST_EXP_ELEM_NUM:
      {
        ast_exp_elem *get = AST_EXP_GET (elem, ret);
        get->num = va_arg (ap, long);
        break;
      }
    case AST_EXP_ELEM_STR:
      {
        ast_exp_elem *get = AST_EXP_GET (elem, ret);
        get->string = va_arg (ap, char *);
        break;
      }
    case AST_EXP_ELEM_REAL:
      {
        ast_exp_elem *get = AST_EXP_GET (elem, ret);
        get->real = va_arg (ap, double);
        break;
      }
    case AST_EXP_UNARY_UMINUS:
      {
        ast_exp *exp2 = va_arg (ap, ast_exp *);
        ast_exp *exp1 = AST_EXP_NEW (ELEM_NUM, pos, 0);
        ret = AST_EXP_NEW (BINARY_MINUS, pos, exp1, exp2);
        break;
      }
    case AST_EXP_BINARY_MATH_ST + 1 ... AST_EXP_BINARY_MATH_ED - 1:
    case AST_EXP_BINARY_LOGIC_ST + 1 ... AST_EXP_BINARY_LOGIC_ED - 1:
      {
        ast_exp_binary *get = AST_EXP_GET (binary, ret);
        get->exp1 = va_arg (ap, ast_exp *);
        get->exp2 = va_arg (ap, ast_exp *);
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
  ast_env *now = glob_env;
  ast_defs *defs = &now->defs;
  ast_def **list = defs->list;

find:
  for (unsigned i = 0; i < defs->size; i++)
    {
      if (list[i]->pos.ln >= pos.ln)
        break;
      if (strcmp (name, list[i]->id) == 0)
        return list[i];
    }

  if (now->outer == NULL)
    return NULL;

  now = now->outer;
  defs = &now->defs;
  list = defs->list;

  goto find;
}

void
ast_env_init (void)
{
  ast_env_new ();

  static const char *names[] = {
    [AST_TYPE_INT8] = "int8",     [AST_TYPE_INT16] = "int16",
    [AST_TYPE_INT32] = "int32",   [AST_TYPE_INT64] = "int64",
    [AST_TYPE_UINT8] = "uint8",   [AST_TYPE_UINT16] = "uint16",
    [AST_TYPE_UINT32] = "uint32", [AST_TYPE_UINT64] = "uint64",
    [AST_TYPE_FLOAT] = "float",   [AST_TYPE_DOUBLE] = "double",
    [AST_TYPE_ARRAY] = "array",   [AST_TYPE_STRUCT] = "struct",
    [AST_TYPE_UNION] = "union",   [AST_TYPE_POINTER] = "pointer",
  };

  ast_def *def;
  ast_def_type *get;
  ast_pos origin = { 0 };

  for (int i = AST_TYPE_ELEM_ST + 1; i <= AST_TYPE_ELEM_ED - 1; i++)
    {
      def = checked_malloc (AST_DEF_SIZE (type));
      get = AST_DEF_GET (type, def);

      def->kind = AST_DEF_TYPE;
      def->id = string (names[i]);
      def->pos = origin;

      get->origin = NULL;
      ast_defs_add (&glob_env->defs, def);
    }
}

void
ast_env_new (void)
{
  ast_env *new = checked_malloc (sizeof (ast_env));
  memset (new, 0, sizeof (ast_env));
  new->outer = glob_env;
  glob_env = new;
}

void
ast_env_add (void *ptr)
{
  int type = *(int *)ptr;

  switch (type)
    {
    case AST_DEF_ST ... AST_DEF_ED:
      ast_defs_add (&glob_env->defs, ptr);
      break;
    case AST_STM_ST ... AST_STM_ED:
      ast_stms_add (&glob_env->stms, ptr);
      break;
    default:
      error ("bad type\n");
    }
}

void
ast_defs_add (ast_defs *defs, ast_def *def)
{
  if (defs->size + 1 > defs->cap)
    {
      size_t cap = 2 * defs->cap;
      cap == 0 ? cap = 8 : 0;
      defs->list = realloc (defs->list, cap * sizeof (ast_def *));
      if (defs->list == NULL)
        error ("realloc failed\n");
      defs->cap = cap;
    }
  defs->list[defs->size++] = def;
}

void
ast_stms_add (ast_stms *stms, ast_stm *stm)
{
  if (stms->size + 1 > stms->cap)
    {
      size_t cap = 2 * stms->cap;
      cap == 0 ? cap = 8 : 0;
      stms->list = realloc (stms->list, cap * sizeof (ast_stm *));
      if (stms->list == NULL)
        error ("realloc failed\n");
      stms->cap = cap;
    }
  stms->list[stms->size++] = stm;
}

void
ast_type_add (int cond, ast_pos pos, char *name)
{
  switch (cond)
    {
      /* type: ID */
    case 1:
      {
      }
    }
}

ast_def *
ast_type_get (void)
{
  ast_def *ret = glob_type;
  glob_type = NULL;
  return ret;
}

void
ast_parm_add (ast_pos pos, char *type)
{
  ast_def *defined = ast_def_seek (pos, type);
  if (defined == NULL)
    error ("use undefined type %s\n", type);

  if (glob_parm.size + 1 > glob_parm.cap)
    {
      size_t cap = 2 * glob_parm.cap;
      cap == 0 ? cap = 8 : 0;
      glob_parm.list = realloc (glob_parm.list, cap * sizeof (ast_def *));
      if (glob_parm.list == NULL)
        error ("realloc failed\n");
      glob_parm.cap = cap;
    }
  glob_parm.list[glob_parm.size++] = defined;
}

ast_defs *
ast_parm_get (void)
{
  ast_defs *ret = checked_malloc (sizeof (ast_defs));
  memcpy (ret, &glob_parm, sizeof (ast_defs));
  memset (&glob_parm, 0, sizeof (ast_defs));
  return ret;
}
