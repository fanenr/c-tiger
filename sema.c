#include "ast.h"
#include "sema.h"
#include "util.h"
#include "parser.h"
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
sema_check_def_id (ast_tok id, ast_env *env)
{
  vector *defs = &env->defs;
  for (size_t i = 0; i < defs->size; i++)
    {
      ast_def *def = vector_get (defs, i);

      if (sema_check_pos (def->id.pos, id.pos) >= 0)
        return;

      if (!strcmp (id.str, def->id.str))
        error ("the identifier %s has been defined at %u:%u\n", id.str,
               def->id.pos.ln, def->id.pos.ch);
    }
}

ast_def *
sema_seek_def (const char *name, ast_env *env)
{
  vector *defs = &env->defs;
  while (true)
    {
      for (size_t i = 0; i < defs->size; i++)
        {
          ast_def *def = vector_get (defs, i);
          if (!strcmp (name, def->id.str))
            return def;
        }
      if (!env->outer)
        return NULL;
      defs = &env->outer->defs;
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

        if (!def || def->kind != AST_DEF_TYPE
            || sema_check_pos (get->type->pos, type->pos) >= 0)
          error ("the type %s is undefined\n", name);
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
    }
}

void
sema_check_def (ast_def *def, ast_env *env)
{
  ast_tok id = def->id;
  sema_check_def_id (id, env);
  switch (def->kind)
    {
    case AST_DEF_VAR:
      {
        ast_def_var *get = AST_DEF_GET (var, def);
        sema_check_type (get->type, env);
        break;
      }
    }
}

void
sema_check_exp (ast_exp *exp, ast_env *env)
{
}
