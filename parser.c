#include "parser.h"
#include "array.h"
#include "array_ext.h"
#include "ast.h"
#include "common.h"
#include "mstr.h"

#include "tiger.y.h"
#include <stdint.h>
#include <stdlib.h>

#define ARRAY_INIT_CAP 8
#define ARRAY_EXPAN_RATIO 2

ast_env prog;
static ast_env *m_env = &prog;
static ast_pos m_pos = { .ln = 0, .ch = 0 };

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

static void array_expand (array_t *arr);
static void ast_env_push_stm (ast_stm *stm);
static void ast_env_push_def (ast_def *def);
static int defs_name_comp (const void *a, const void *b);

void
set_parse_pos (ast_tok tok)
{
  m_pos = tok.pos;
}

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

      ast_env_push_def (base);
    }
}

void
ast_env_new (void)
{
  ast_env *new = mem_malloc (sizeof (ast_env));

  new->defs = (array_t){ .elem_size = sizeof (ast_def *) };
  new->stms = (array_t){ .elem_size = sizeof (ast_stm *) };
  new->outer = m_env;

  m_env = new;
}

static inline void
ast_env_push_stm (ast_stm *stm)
{
  array_t *stms = &m_env->stms;
  array_t *defs = &m_env->defs;
  array_expand (stms);

  ast_stm **inpos = array_push_back (stms);
  stm->index = stms->size + defs->size;
  *inpos = stm;
}

static inline void
ast_env_push_def (ast_def *def)
{
  array_t *defs = &m_env->defs;
  array_t *stms = &m_env->stms;
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
}

ast_type *
ast_type1_new (ast_tok name)
{
  ast_type *type = mem_malloc (sizeof (ast_type));
  type->pos = name.pos;

  /* TODO */

  return type;
}

ast_type *
ast_type2_new (ast_type *ref)
{
  ast_type *type = mem_malloc (sizeof (ast_type));
  type->kind = AST_TYPE_POINTER;
  type->size = sizeof (void *);
  type->pos = m_pos;
  type->ref = ref;
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
ast_def_type_new (ast_tok name, ast_type *type)
{
  ast_def_type *def = mem_malloc (sizeof (ast_def_type));
  ast_def *base = &def->base;
  base->kind = AST_DEF_TYPE;
  base->name = name.str;
  base->pos = m_pos;
  def->type = type;
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

  m_env = env->outer;
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

  m_env = env->outer;
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

  m_env = env->outer;
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

  m_env = env->outer;
  stm->cond = cond;
  stm->env = env;

  return base;
}

ast_stm *
ast_stm_if1_new (ast_exp *cond, ast_env *then_env)
{
  ast_stm_if *stm = mem_malloc (sizeof (ast_stm_if));
  ast_stm *base = &stm->base;
  base->kind = AST_STM_IF;
  base->pos = m_pos;

  stm->then_env = then_env;
  m_env = then_env->outer;
  stm->else_env = NULL;
  stm->cond = cond;

  return base;
}

ast_stm *
ast_stm_if2_new (ast_stm *base, ast_env *else_env)
{
  ast_stm_if *stm = container_of (base, ast_stm_if, base);
  stm->else_env = else_env;
  m_env = else_env->outer;
  return base;
}

ast_exp *
ast_exp_elem_new (ast_tok tok)
{
  ast_exp_elem *exp = mem_malloc (sizeof (ast_exp_elem));
  ast_exp *base = &exp->base;
  base->pos = tok.pos;
  base->type = NULL;

  switch (tok.kind)
    {
    case ID:
      base->kind = AST_EXP_ELEM_ID;
      exp->str = tok.str;
      break;
    case NUM:
      base->kind = AST_EXP_ELEM_NUM;
      exp->num = tok.num;
    case STR:
      base->kind = AST_EXP_ELEM_STR;
      exp->str = tok.str;
    case REAL:
      base->kind = AST_EXP_ELEM_REAL;
      exp->real = tok.real;
      break;
    }

  return base;
}

static inline void
array_expand (array_t *arr)
{
  if (arr->size < arr->cap)
    return;

  size_t newcap = arr->cap * ARRAY_EXPAN_RATIO ?: ARRAY_INIT_CAP;
  void *newdata = mem_realloc (arr->data, newcap * arr->elem_size);

  arr->data = newdata;
  arr->cap = newcap;
}

int
defs_name_comp (const void *a /* const ast_def ** */,
                const void *b /* const ast_def** */)
{
  const ast_def *da = *(const ast_def **)a;
  const ast_def *db = *(const ast_def **)b;
  return mstr_cmp_mstr (&da->name, &db->name);
}
