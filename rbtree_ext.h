#ifndef RBTREE_EXT_H
#define RBTREE_EXT_H

#include "rbtree.h"

typedef void rbtree_visit_t (rbtree_node_t *n);
typedef int rbtree_comp_t (const rbtree_node_t *a, const rbtree_node_t *b);

extern rbtree_node_t *rbtree_find (const rbtree_t *tree,
                                   const rbtree_node_t *target,
                                   rbtree_comp_t *comp) attr_nonnull (1, 2, 3);

extern rbtree_node_t *rbtree_insert (rbtree_t *tree, rbtree_node_t *node,
                                     rbtree_comp_t *comp)
    attr_nonnull (1, 2, 3);

extern void rbtree_for_each (rbtree_t *tree, rbtree_visit_t *visit)
    attr_nonnull (1, 2);

#endif
