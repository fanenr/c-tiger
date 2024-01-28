#include "sema.h"
#include "util.h"

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
    }

  vector *stms = &env->stms;
  for (size_t i = 0; i < stms->size; i++)
    {
    }

  STACK_POP (&stac);
}
