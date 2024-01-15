#include "types.h"

struct env;
typedef struct env env;

struct env
{
  const char *id;
  long value;
  env *tail;
};

stm_t *
build_stm (void)
{
  /* a := 5 + 3; */
  stm_t *stm1 = stm_new_assign (
      "a", exp_new_op (exp_new_num (5), PLUS, exp_new_num (3)));

  /* b := (print (a, a - 1), 10 * a); */
  stm_t *stm2 = stm_new_assign (
      "b",
      exp_new_eseq (stm_new_print (exp_list_new_pair (
                        exp_new_id ("a"),
                        exp_list_new_last (exp_new_op (exp_new_id ("a"), MINUS,
                                                       exp_new_num (1))))),
                    exp_new_op (exp_new_num (10), TIMES, exp_new_id ("a"))));

  /* print (b); */
  stm_t *stm3 = stm_new_print (exp_list_new_last (exp_new_id ("b")));

  return stm_new_compound (stm1, stm_new_compound (stm2, stm3));
}

int
main (void)
{
  build_stm ();
}
