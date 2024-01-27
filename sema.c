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
