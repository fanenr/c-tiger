#include "parser.h"
#include "array.h"
#include "array_ext.h"
#include "ast.h"
#include "common.h"
#include "mstr.h"

#include <stdint.h>
#include <stdlib.h>

#define ARRAY_INIT_CAP 8
#define ARRAY_EXPAN_RATIO 2

ast_env prog;
static ast_pos m_pos = { .ln = 0, .ch = 0 };

void
set_parse_pos (ast_tok tok)
{
  m_pos = tok.pos;
}

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
ast_prog_init (void)
{
  prog.defs.elem_size = sizeof (ast_def *);
  prog.stms.elem_size = sizeof (ast_stm *);

  for (int i = AST_TYPE_BASE_ST + 1; i < AST_TYPE_BASE_ED; i++)
    {
      ast_type *type = mem_malloc (sizeof (ast_type));
      type->size = base_type_size[i];
      type->pos = m_pos;
      type->kind = i;

      ast_def_type *def = mem_malloc (sizeof (ast_def_type));
      ast_def *base = &def->base;
      mstr_t *name = &base->name;
      base->kind = AST_DEF_TYPE;
      base->pos = m_pos;
      def->type = type;

      *name = MSTR_INIT;
      mstr_assign_cstr (name, base_type_name[i]);

      ast_env_push_def (&prog, base);
    }
}

void
array_expand (array_t *arr)
{
  if (arr->size < arr->cap)
    return;

  size_t newcap = arr->cap * ARRAY_EXPAN_RATIO ?: ARRAY_INIT_CAP;
  void *newdata = mem_realloc (arr->data, newcap * arr->elem_size);

  arr->data = newdata;
  arr->cap = newcap;
}

ast_env *
ast_env_new (void)
{
  ast_env *ret = mem_malloc (sizeof (ast_env));

  ret->defs = (array_t){ .elem_size = sizeof (ast_def *) };
  ret->stms = (array_t){ .elem_size = sizeof (ast_stm *) };
  ret->outer = NULL;

  return ret;
}

ast_env *
ast_env_push_stm (ast_env *env, ast_stm *stm)
{
  if (!env)
    env = ast_env_new ();

  array_t *stms = &env->stms;
  array_t *defs = &env->defs;
  array_expand (stms);

  ast_stm **inpos = array_push_back (stms);
  stm->index = stms->size + defs->size;
  *inpos = stm;

  return env;
}

int
defs_name_comp (const void *a /* const ast_def ** */,
                const void *b /* const ast_def** */)
{
  const ast_def *da = *(const ast_def **)a;
  const ast_def *db = *(const ast_def **)b;
  return mstr_cmp_mstr (&da->name, &db->name);
}

ast_env *
ast_env_push_def (ast_env *env, ast_def *def)
{
  if (!env)
    env = ast_env_new ();

  array_t *defs = &env->defs;
  array_t *stms = &env->stms;
  array_expand (defs);

  ast_def **defined;
  if ((defined = array_find (defs, &def, defs_name_comp)))
    {
      ast_pos pos = (*defined)->pos;
      ast_error (def->pos, "%s has been defined here %d:%d",
                 mstr_data (&def->name), pos.ln, pos.ch);
    }

  ast_def **inpos = array_push_back (defs);
  def->index = defs->size + stms->size;
  *inpos = def;

  return env;
}

ast_type *
ast_type_new (ast_type *origin, ast_tok tok)
{
  ast_type *type = mem_malloc (sizeof (ast_type));
  type->pos = tok.pos;

  if (origin)
    { /* pointer */
      type->ref = origin;
      type->size = sizeof (void *);
      type->kind = AST_TYPE_POINTER;
    }
  else
    { /* undetermined */
      type->kind = -1;
      type->name = tok.str;
    }

  return type;
}

ast_def *
ast_def_var_new (ast_tok name, ast_type *type)
{
  ast_def_var *def = mem_malloc (sizeof (ast_def_var));
  ast_def *base = &def->base;
  base->kind = AST_DEF_VAR;
  base->name = name.str;
  base->pos = m_pos;
  def->type = type;
  return base;
}

ast_def *
ast_def_type_new (ast_tok name, ast_type *origin)
{
  ast_def_type *def = mem_malloc (sizeof (ast_def_type));
  ast_def *base = &def->base;
  base->kind = AST_DEF_TYPE;
  base->name = name.str;
  base->pos = m_pos;
  def->type = origin;
  return base;
}

ast_def *
ast_def_type_union_new (ast_tok name, ast_env *env)
{
  ast_def_type *def = mem_malloc (sizeof (ast_def_type));
  ast_def *base = &def->base;
  base->kind = AST_DEF_TYPE;
  base->name = name.str;
  base->pos = m_pos;

  ast_type *type = mem_malloc (sizeof (ast_type));
  type->kind = AST_TYPE_UNION;
  type->pos = m_pos;
  type->mem = env;

  def->type = type;

  return base;
}

ast_def *
ast_def_type_struct_new (ast_tok name, ast_env *env)
{
  ast_def_type *def = mem_malloc (sizeof (ast_def_type));
  ast_def *base = &def->base;
  base->kind = AST_DEF_TYPE;
  base->name = name.str;
  base->pos = m_pos;

  ast_type *type = mem_malloc (sizeof (ast_type));
  type->kind = AST_TYPE_STRUCT;
  type->pos = m_pos;
  type->mem = env;

  def->type = type;

  return base;
}

array_t *
ast_def_func_parm_new (array_t *parm, ast_tok name, ast_type *type)
{
  if (!parm)
    {
      parm = mem_malloc (sizeof (array_t));
      *parm = (array_t){ .elem_size = sizeof (ast_def *) };
    }

  ast_def_var *def = mem_malloc (sizeof (ast_def_var));
  ast_def *base = &def->base;
  base->kind = AST_DEF_VAR;
  base->name = name.str;
  base->pos = name.pos;
  array_expand (parm);

  ast_def **defined;
  if ((defined = array_find (parm, &base, defs_name_comp)))
    {
      ast_pos pos = (*defined)->pos;
      ast_error (name.pos, "%s has been defined here %d:%d",
                 mstr_data (&name.str), pos.ln, pos.ch);
    }

  ast_def **inpos = array_push_back (parm);
  base->index = parm->size;
  def->type = type;
  *inpos = base;

  return parm;
}

ast_def *
ast_def_func_new (ast_tok name, array_t *parm, ast_type *type, ast_env *env)
{
  ast_def_func *def = mem_malloc (sizeof (ast_def_func));
  ast_def *base = &def->base;
  base->kind = AST_DEF_FUNC;
  base->name = name.str;
  base->pos = m_pos;

  def->env = env;
  def->type = type;
  def->parms = parm ? parm->size : 0;

  if (!parm)
    return base;

  array_t *defs = &env->defs;
  size_t size_parm = parm->size;

  for (size_t i = size_parm; i; i--)
    {
      ast_def *def_parm = *(ast_def **)array_at (parm, i - 1);
      def_parm->index -= size_parm;
      array_expand (defs);

      ast_def **defined;
      if ((defined = array_find (defs, &def_parm, defs_name_comp)))
        {
          ast_pos pos_parm = def_parm->pos;
          ast_pos pos_defined = (*defined)->pos;
          ast_error (pos_defined, "%s has been defined here %d:%d",
                     mstr_data (&name.str), pos_parm.ln, pos_parm.ch);
        }

      ast_def **inpos = array_insert (defs, 0);
      *inpos = def_parm;
    }

  free (parm->data);
  free (parm);

  return base;
}

ast_stm *
ast_stm_return_new (ast_exp *val)
{
  ast_stm_return *stm = mem_malloc (sizeof (ast_stm_return));
  ast_stm *base = &stm->base;
  base->kind = AST_STM_RETURN;
  base->pos = m_pos;
  stm->val = val;
  return base;
}

ast_stm *
ast_stm_assign_new (ast_exp *obj, ast_exp *val)
{
  ast_stm_assign *stm = mem_malloc (sizeof (ast_stm_assign));
  ast_stm *base = &stm->base;
  base->kind = AST_STM_ASSIGN;
  base->pos = obj->pos;
  stm->obj = obj;
  stm->val = val;
  return base;
}

ast_stm *
ast_stm_while_new (ast_exp *cond, ast_env *env)
{
  ast_stm_while *stm = mem_malloc (sizeof (ast_stm_while));
  ast_stm *base = &stm->base;
  base->kind = AST_STM_WHILE;
  base->pos = m_pos;
  stm->cond = cond;
  stm->env = env;
  return base;
}

ast_stm *
ast_stm_if_new (ast_exp *cond, ast_env *then_env, ast_env *else_env)
{
  ast_stm_if *stm = mem_malloc (sizeof (ast_stm_if));
  ast_stm *base = &stm->base;
  stm->then_env = then_env;
  stm->else_env = else_env;
  base->kind = AST_STM_IF;
  base->pos = m_pos;
  stm->cond = cond;
  return base;
}
