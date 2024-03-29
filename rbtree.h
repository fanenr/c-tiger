#ifndef RBTREE_H
#define RBTREE_H

#include "util.h"
#include <stddef.h>

typedef int rbtree_color_t;
typedef struct rbtree_t rbtree_t;
typedef struct rbtree_node_t rbtree_node_t;

#define RBTREE_RED 0
#define RBTREE_BLACK 1

struct rbtree_node_t
{
  rbtree_node_t *left;
  rbtree_node_t *right;
  rbtree_node_t *parent;
  rbtree_color_t color;
};

struct rbtree_t
{
  size_t size;
  rbtree_node_t *root;
};

#define RBTREE_INIT                                                           \
  (rbtree_t) {}

extern void rbtree_link (rbtree_t *tree, rbtree_node_t **inpos,
                         rbtree_node_t *parent, rbtree_node_t *node)
    attr_nonnull (1, 2, 4);

extern void rbtree_erase (rbtree_t *tree, rbtree_node_t *node)
    attr_nonnull (1, 2);

#endif
