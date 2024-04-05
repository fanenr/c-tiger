#include "parser.h"
#include "array.h"
#include "array_ext.h"
#include "ast.h"
#include "common.h"
#include "mstr.h"

#include "tiger.y.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define ARRAY_INIT_CAP 8
#define ARRAY_EXPAN_RATIO 2

ast_env prog;

static ast_pos m_pos;
static ast_env *m_env;

ast_type base_type[] = {
  [AST_TYPE_VOID] = { .kind = AST_TYPE_VOID, .size = sizeof (void) },

  [AST_TYPE_INT8] = { .kind = AST_TYPE_INT8, .size = sizeof (int8_t) },
  [AST_TYPE_INT16] = { .kind = AST_TYPE_INT16, .size = sizeof (int16_t) },
  [AST_TYPE_INT32] = { .kind = AST_TYPE_INT32, .size = sizeof (int32_t) },
  [AST_TYPE_INT64] = { .kind = AST_TYPE_INT64, .size = sizeof (int64_t) },

  [AST_TYPE_UINT8] = { .kind = AST_TYPE_UINT8, .size = sizeof (uint8_t) },
  [AST_TYPE_UINT16] = { .kind = AST_TYPE_UINT16, .size = sizeof (uint16_t) },
  [AST_TYPE_UINT32] = { .kind = AST_TYPE_UINT32, .size = sizeof (uint32_t) },
  [AST_TYPE_UINT64] = { .kind = AST_TYPE_UINT64, .size = sizeof (uint64_t) },

  [AST_TYPE_FLOAT] = { .kind = AST_TYPE_FLOAT, .size = sizeof (float) },
  [AST_TYPE_DOUBLE] = { .kind = AST_TYPE_DOUBLE, .size = sizeof (double) },
};

ast_type base_type_string = {
  .kind = AST_TYPE_POINTER,
  .size = sizeof (uint8_t *),
  .ref = &base_type[AST_TYPE_UINT8],
};

const char *base_type_name[] = {
  [AST_TYPE_VOID] = "void",

  [AST_TYPE_INT8] = "int8",     [AST_TYPE_INT16] = "int16",
  [AST_TYPE_INT32] = "int32",   [AST_TYPE_INT64] = "int64",

  [AST_TYPE_UINT8] = "uint8",   [AST_TYPE_UINT16] = "uint16",
  [AST_TYPE_UINT32] = "uint32", [AST_TYPE_UINT64] = "uint64",

  [AST_TYPE_FLOAT] = "float",   [AST_TYPE_DOUBLE] = "double",
};

static void array_expand (array_t *arr);
static void menv_push_stm (ast_stm *base);
static void menv_push_def (ast_def *base);
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
  m_env = &prog;

  for (int i = AST_TYPE_BASE_ST + 1; i < AST_TYPE_BASE_ED; i++)
    {
      ast_def_type *def = mem_malloc (sizeof (ast_def_type));
      ast_def *base = &def->base;
      mstr_t *name = &base->name;

      base->kind = AST_DEF_TYPE;
      base->pos = m_pos;

      mstr_init (name);
      mstr_assign_cstr (name, base_type_name[i]);

      def->type = &base_type[i];

      menv_push_def (base);
    }
}

ast_env *
ast_env_new (void)
{
  ast_env *env = mem_malloc (sizeof (ast_env));

  env->defs = (array_t){ .elem_size = sizeof (ast_def *) };
  env->stms = (array_t){ .elem_size = sizeof (ast_stm *) };
  env->outer = m_env;

  return (m_env = env);
}

ast_env *
ast_env_end (void)
{
  ast_env *env = m_env;

  m_env = env->outer;

  return env;
}

static inline void
menv_push_stm (ast_stm *base)
{
  array_t *stms = &m_env->stms;
  array_expand (stms);

  ast_stm **inpos = array_push_back (stms);
  *inpos = base;
}

static inline void
menv_push_def (ast_def *base)
{
  array_t *defs = &m_env->defs;
  array_expand (defs);

  ast_def **defined;
  if ((defined = array_find (defs, &base, defs_name_comp)))
    {
      ast_pos pos = (*defined)->pos;
      ast_error (base->pos, "%s has been defined here %d:%d",
                 mstr_data (&base->name), pos.ln, pos.ch);
    }

  ast_def **inpos = array_push_back (defs);
  *inpos = base;
}

static inline ast_def *
env_find_def (ast_env *env, const mstr_t *name)
{
  for (; env; env = env->outer)
    {
      array_t *defs = &env->defs;
      for (size_t i = defs->size; i; i--)
        {
          ast_def *base = *(ast_def **)array_at (defs, i - 1);
          if (mstr_cmp_mstr (name, &base->name) == 0)
            return base;
        }
    }
  return NULL;
}

ast_type *
ast_type1_new (ast_tok name)
{
  ast_def *base = env_find_def (m_env, &name.string);
  if (!base || base->kind != AST_DEF_TYPE)
    ast_error (name.pos, "use undefined type %s", mstr_data (&name.string));
  ast_def_type *def = container_of (base, ast_def_type, base);
  return def->type;
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

void
ast_def_var_new (ast_tok name, ast_type *type)
{
  ast_def_var *def = mem_malloc (sizeof (ast_def_var));
  ast_def *base = &def->base;

  base->kind = AST_DEF_VAR;
  base->name = name.string;
  base->pos = name.pos;

  def->type = type;

  if (type->kind == AST_TYPE_VOID)
    ast_error (name.pos, "variable type can not be void");

  menv_push_def (base);
}

void
ast_def_type_new (ast_tok name, ast_type *type)
{
  ast_def_type *def = mem_malloc (sizeof (ast_def_type));
  ast_def *base = &def->base;

  base->kind = AST_DEF_TYPE;
  base->name = name.string;
  base->pos = name.pos;

  def->type = type;

  menv_push_def (base);
}

static unsigned union_size;

void
ast_def_union_new (ast_tok name, ast_env *env)
{
  ast_def_type *def = mem_malloc (sizeof (ast_def_type));
  ast_type *type = mem_malloc (sizeof (ast_type));
  ast_def *base = &def->base;

  base->kind = AST_DEF_TYPE;
  base->name = name.string;
  base->pos = name.pos;

  def->type = type;

  type->kind = AST_TYPE_UNION;
  type->pos = name.pos;
  type->mem = env;

  array_t *stms = &env->stms;
  if (stms->size != 0)
    ast_error (name.pos, "statement can not be here");

  array_t *defs = &env->defs;
  for (size_t i = defs->size; i; i--)
    {
      ast_def *base = *(ast_def **)array_at (defs, i - 1);
      switch (base->kind)
        {
        case AST_DEF_FUNC:
          ast_error (base->pos, "function can not be here");

        case AST_DEF_TYPE:
          break;

        case AST_DEF_VAR:
          {
            ast_def_var *def = container_of (base, ast_def_var, base);
            unsigned type_size = def->type->size;
            if (type_size > union_size)
              union_size = type_size;
          }
          break;
        }
    }

  type->size = union_size;

  union_size = 0;
  menv_push_def (base);
}

static unsigned struct_size;

void
ast_def_struct_new (ast_tok name, ast_env *env)
{
  ast_def_type *def = mem_malloc (sizeof (ast_def_type));
  ast_type *type = mem_malloc (sizeof (ast_type));
  ast_def *base = &def->base;

  base->kind = AST_DEF_TYPE;
  base->name = name.string;
  base->pos = name.pos;

  def->type = type;

  type->kind = AST_TYPE_STRUCT;
  type->pos = name.pos;
  type->mem = env;

  array_t *stms = &env->stms;
  if (stms->size != 0)
    ast_error (name.pos, "statement can not be here");

  array_t *defs = &env->defs;
  for (size_t i = defs->size; i; i--)
    {
      ast_def *base = *(ast_def **)array_at (defs, i - 1);
      switch (base->kind)
        {
        case AST_DEF_FUNC:
          ast_error (base->pos, "function can not be here");

        case AST_DEF_TYPE:
          break;

        case AST_DEF_VAR:
          {
            ast_def_var *def = container_of (base, ast_def_var, base);
            unsigned type_size = def->type->size;
            struct_size += type_size;
          }
          break;
        }
    }

  type->size = struct_size;

  struct_size = 0;
  menv_push_def (base);
}

static unsigned func_parms;

void
ast_func_parm_new (ast_tok name, ast_type *type)
{
  ast_def_var_new (name, type);
  func_parms++;
}

void
ast_def_func_new (ast_tok name, ast_type *type, ast_env *env)
{
  ast_def_func *def = mem_malloc (sizeof (ast_def_func));
  ast_def *base = &def->base;

  base->kind = AST_DEF_FUNC;
  base->name = name.string;
  base->pos = name.pos;

  def->parms = func_parms;
  def->type = type;
  def->env = env;

  func_parms = 0;
  menv_push_def (base);
}

void
ast_stm_return_new (ast_exp *val)
{
  ast_stm_return *stm = mem_malloc (sizeof (ast_stm_return));
  ast_stm *base = &stm->base;

  base->kind = AST_STM_RETURN;

  stm->val = val;

  menv_push_stm (base);
}

void
ast_stm_assign_new (ast_exp *obj, ast_exp *val)
{
  ast_stm_assign *stm = mem_malloc (sizeof (ast_stm_assign));
  ast_stm *base = &stm->base;

  base->kind = AST_STM_ASSIGN;

  stm->obj = obj;
  stm->val = val;

  menv_push_stm (base);
}

static inline bool
exp_is_integer (const ast_exp *exp)
{
  int kind = exp->kind;
  return AST_TYPE_INT8 <= kind && kind <= AST_TYPE_UINT64;
}

static inline bool
exp_is_number (const ast_exp *exp)
{
  int kind = exp->kind;
  return AST_TYPE_INT8 <= kind && kind <= AST_TYPE_DOUBLE;
}

void
ast_stm_while_new (ast_exp *cond, ast_env *env)
{
  ast_stm_while *stm = mem_malloc (sizeof (ast_stm_while));
  ast_stm *base = &stm->base;

  base->kind = AST_STM_WHILE;

  stm->cond = cond;
  stm->env = env;

  if (!exp_is_integer (cond))
    ast_error (cond->pos, "while statement need a integer condition");

  menv_push_stm (base);
}

void
ast_stm_if_new (ast_exp *cond, ast_env *then_env, ast_env *else_env)
{
  ast_stm_if *stm = mem_malloc (sizeof (ast_stm_if));
  ast_stm *base = &stm->base;

  base->kind = AST_STM_IF;

  stm->then_env = then_env;
  stm->else_env = else_env;
  stm->cond = cond;

  if (!exp_is_integer (cond))
    ast_error (cond->pos, "if statement need a integer condition");

  menv_push_stm (base);
}

ast_exp *
ast_exp_elem_new (ast_tok tok)
{
  ast_exp_elem *exp = mem_malloc (sizeof (ast_exp_elem));
  ast_exp *base = &exp->base;
  base->pos = tok.pos;

  switch (tok.kind)
    {
    case ID:
      {
        ast_def *find_base = env_find_def (m_env, &tok.string);
        if (!find_base || find_base->kind != AST_DEF_VAR)
          ast_error (tok.pos, "use undefined variable %s",
                     mstr_data (&tok.string));

        ast_type *type = container_of (find_base, ast_def_var, base)->type;
        base->kind = AST_EXP_ELEM_VAR;
        exp->reference = find_base;
        base->type = type;
      }
      break;

    case STR:
      base->type = &base_type_string;
      base->kind = AST_EXP_ELEM_STR;
      exp->string = tok.string;
      break;

    case INT:
      base->type = &base_type[AST_TYPE_INT32];
      base->kind = AST_EXP_ELEM_INT;
      exp->integer = tok.integer;
      break;

    case REAL:
      base->type = &base_type[AST_TYPE_DOUBLE];
      base->kind = AST_EXP_ELEM_REAL;
      exp->realnum = tok.realnum;
      break;
    }

  return base;
}

ast_exp *
ast_exp_unary_new (int kind, ast_exp *base)
{
  switch (kind)
    {
    case AST_EXP_UN_UPLUS:
      if (!exp_is_number (base))
        ast_error (m_pos, "unary plus can only be used for number");
      return base;

    case AST_EXP_UN_UMINUS:
      if (!exp_is_number (base))
        ast_error (m_pos, "unary minus can only be used for number");
      {
        ast_exp_elem *zero = mem_malloc (sizeof (ast_exp_elem));
        ast_exp *zero_base = &zero->base;

        zero_base->type = &base_type[AST_TYPE_INT32];
        zero_base->kind = AST_EXP_ELEM_INT;
        zero_base->pos = m_pos;
        zero->integer = 0;

        ast_exp_binary *ret = mem_malloc (sizeof (ast_exp_binary));
        ast_exp *ret_base = &ret->base;

        ret_base->kind = AST_EXP_BIN_MINUS;
        ret_base->type = base->type;
        ret_base->pos = m_pos;
        ret->exp1 = zero_base;
        ret->exp2 = base;

        return ret_base;
      }

    case AST_EXP_UN_ADDR:
      if (base->kind != AST_EXP_ELEM_VAR)
        ast_error (m_pos, "address operator can only be used for variable");
      {
        ast_exp_unary *ret = mem_malloc (sizeof (ast_exp_unary));
        ast_type *type = mem_malloc (sizeof (ast_type));
        ast_exp *ret_base = &ret->base;

        type->kind = AST_TYPE_POINTER;
        type->size = sizeof (void *);
        type->pos = m_pos;

        ret_base->kind = AST_EXP_UN_ADDR;
        ret_base->type = type;
        ret_base->pos = m_pos;
        ret->exp = base;

        return ret_base;
      }

    case AST_EXP_UN_DREF:
      if (base->type->kind != AST_TYPE_POINTER)
        ast_error (m_pos, "dereference operator can only be used for pointer");
      {
        ast_exp_unary *ret = mem_malloc (sizeof (ast_exp_unary));
        ast_exp *ret_base = &ret->base;

        ret_base->kind = AST_EXP_UN_ADDR;
        ret_base->type = base->type->ref;
        ret_base->pos = m_pos;
        ret->exp = base;

        return ret_base;
      }
    }

  ast_error (m_pos, "use unknow unary operator");
}

ast_exp *
ast_exp_call_new (ast_tok name, array_t *args)
{
}

array_t *
ast_call_args_new (array_t *args, ast_exp *arg)
{
}

ast_exp *
ast_exp_binary_new (int kind, ast_exp *, ast_exp *exp2)
{
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
                const void *b /* const ast_def ** */)
{
  const ast_def *da = *(const ast_def **)a;
  const ast_def *db = *(const ast_def **)b;
  return mstr_cmp_mstr (&da->name, &db->name);
}
