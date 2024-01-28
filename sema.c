#include "ast.h"
#include "sema.h"
#include "util.h"
#include <string.h>

stack stac;

#define STACK_PUSH(STAC, PTR)                                                 \
  if (!stack_push (STAC, PTR))                                                \
    error ("error: stack_push\n");

#define STACK_POP(STAC)                                                       \
  if (!stack_pop (STAC))                                                      \
    error ("error: stack_pop\n");

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

void
sema_check_def_id (ast_tok id, ast_env *env)
{
  vector *defs = &env->defs;
  for (size_t i = 0; i < defs->size; i++)
    {
      ast_def *def = vector_get (defs, i);

      if ((def->id.pos.ln > id.pos.ln)
          || (def->id.pos.ln == id.pos.ln && def->id.pos.ch >= id.pos.ch))
        return;

      if (!strcmp (id.str, def->id.str))
        error ("the identifier %s has been defined at %u:%u\n", id.str,
               def->id.pos.ln, def->id.pos.ch);
    }
}

void
sema_check_def (ast_def *def, ast_env *env)
{
  ast_tok id = def->id;
  sema_check_def_id (id, env);
}
