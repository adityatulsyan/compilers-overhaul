#include <stdlib.h>
#include "../utils/gen_utils.h"

#ifndef TREE_H
#define TREE_H

typedef struct ____TREE_NODE____ tree_node;

struct ____TREE_NODE____
{
    void *data;
    int num_children;
    int max_children;
    tree_node **child;
    tree_node *parent;
};

typedef struct ____TREE____ tree;

struct ____TREE____
{
    tree_node *root;
};

tree *create_tree();

void set_root(tree *t, void *data);

tree_node *get_root(tree *t);

void add_child_at(tree_node *n, void *data, int idx);

void add_child(tree_node *n, void *data);

int get_num_children(tree_node *n);

void *get_data(tree_node *n);

tree_node *get_child(tree_node *n, int idx);

tree_node *get_parent(tree_node *n);

void *remove_child(tree_node *n, int idx);

#endif

/*

TODO : DESTROY TREE

*/