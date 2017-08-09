#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>

typedef struct bst_node_t{
  struct bst_node_t *parent;
  struct bst_node_t *left;
  struct bst_node_t *right;
  int64_t key;
  int64_t count;
  int64_t size;
  int64_t depth;
} bst_node_t;

#define SIZE(x)  (((x) == NULL) ? 0 : x->size)
#define DEPTH(x) (((x) == NULL) ? 0 : x->depth)
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define SET_PARENT(a,b) do { if ((a) != NULL) {a->parent = b;}} while(0)

void
bst_display(const bst_node_t *node, int indent)
{
  if (node != NULL) {
    bst_display(node->left, indent + 1);
    for (int64_t i = 0; i < indent; i++) {
      printf("    ");
    }
    if (node->count == 1) {
      printf("%ld\n", node->key);
    } else {
      printf("%ld <%ld>\n", node->key, node->count);
    }
    bst_display(node->right, indent + 1);
  }
}

static void
bst_fill_stats(bst_node_t *node) 
{
  node->size = node->count + SIZE(node->right) + SIZE(node->left);
  node->depth = 1 + MAX(DEPTH(node->right),DEPTH(node->left));
}

bst_node_t*
bst_rotate(bst_node_t *node, bst_node_t *child) 
{
  if (child == NULL) {
    return node;
  }

  if (child == node->left) {
    /*
                   D
                   |
                 node
                /    \
             child    C
             /   \
            A     B
                 / \
                E   F
    */
    bst_node_t *a = child->left;
    bst_node_t *b = child->right;
    bst_node_t *c = node->right;
    bst_node_t *d = node->parent;

    int64_t depth1 = 1 + MAX(DEPTH(b), DEPTH(c));
    int64_t depth2 = 1 + MAX(depth1, DEPTH(a));
    if (depth2 < node->depth) {
      node->parent = child;
      node->left = b;     
      node->right = c;    
      bst_fill_stats(node);

      child->parent = d;    
      child->left = a;    
      child->right = node;
      bst_fill_stats(child);

      SET_PARENT(a, child);
      SET_PARENT(b, node);
      SET_PARENT(c, node);
      return child;
    }

    if (b != NULL) {
      bst_node_t *e = b->left;
      bst_node_t *f = b->right;
      int64_t depth3 = 1 + MAX(DEPTH(a), DEPTH(e));
      int64_t depth4 = 1 + MAX(DEPTH(f), DEPTH(c));
      int64_t depth5 = 1 + MAX(depth3, depth4);

      if (depth5 < node->depth) {
	child->parent = b;
	child->left = a;
	child->right = e;
	bst_fill_stats(child);
	
	node->parent = b;
	node->left = f;
	node->right = c;
	bst_fill_stats(node);

	b->parent = d;
	b->left = child;
	b->right = node;
	bst_fill_stats(b);

	SET_PARENT(a, child);
	SET_PARENT(e, child);
	SET_PARENT(f, node);
	SET_PARENT(c, node);

	return b;
      }
    }
  } else if (child == node->right) {
    /*
                   D
                   |
                 node
                /    \
               A    child    
                    /   \
                   B     C
                  / \
                 E   F
    */
    bst_node_t *a = node->left;
    bst_node_t *b = child->left;
    bst_node_t *c = child->right;
    bst_node_t *d = node->parent;

    int64_t depth1 = 1 + MAX(DEPTH(a), DEPTH(b));
    int64_t depth2 = 1 + MAX(depth1, DEPTH(c));
    if (depth2 < node->depth) {
      node->parent = child;
      node->left = a;      
      node->right = b;     
      bst_fill_stats(node);

      child->parent = d;
      child->left = node;
      child->right = c;    
      bst_fill_stats(child);

      SET_PARENT(a, node);
      SET_PARENT(b, node);
      SET_PARENT(c, child);
      return child;
    }

    if (b != NULL) {
      bst_node_t *e = b->left;
      bst_node_t *f = b->right;
      int64_t depth3 = 1 + MAX(DEPTH(a), DEPTH(e));
      int64_t depth4 = 1 + MAX(DEPTH(f), DEPTH(c));
      int64_t depth5 = 1 + MAX(depth3, depth4);

      if (depth5 < node->depth) {
	node->parent = b;
	node->left = a;
	node->right = e;
	bst_fill_stats(node);

	child->parent = b;
	child->left = f;
	child->right = c;
	bst_fill_stats(child);

	b->parent = d;
	b->left = node;
	b->right = child;
	bst_fill_stats(b);

	SET_PARENT(a,node);
	SET_PARENT(e,node);
	SET_PARENT(f,child);
	SET_PARENT(c,child);

	return b;
      }
    }
  }
  return node;
}

bst_node_t*
bst_insert(bst_node_t *root, int64_t value)
{
  bst_node_t *parent = NULL;
  bst_node_t *node = root;
  
  // walk down the tree 
  while(true) {
    if (node == NULL) {
      bst_node_t *leaf = malloc(sizeof(bst_node_t));
      leaf->parent = parent;
      leaf->left = NULL;
      leaf->right= NULL;
      leaf->key = value;
      leaf->count = 1;
      leaf->size = 1;
      node = leaf;

      if (parent != NULL) {
	if (value > parent->key) {
	  parent->right = node;
	} else {
	  parent->left = node;
	}
      }
      break;
    }

    if (value == node->key) {
      node->count += 1;
      break;
    }

    parent = node;
    node = value > node->key ? node->right : node->left;
  }

  bst_node_t *child = node;
  while(true) {
    // set depth and size
    bst_fill_stats(node);

    // save the down link from parent
    // recursion would make this cleaner
    bst_node_t *parent = node->parent;
    int dir = -1;
    if (parent != NULL) {
      if (parent->left == node) {
	dir = 0;
      } 
      if (parent->right == node) {
	dir = 1;
      }
      assert(dir != -1);
    }

    // should we rotate?
    bst_node_t *save = node;
    node = bst_rotate(node, child);

    // are we done?
    if (node->parent == NULL) {
      break;
    }

    if (dir == 0) {
      parent->left = node;
    } else {
      parent->right = node;
    }

    // walk up the tree
    child = node;
    node = parent;
  }
  
  return node;
} 


int 
main()
{
  srand48(time(NULL));
  bst_node_t *root = NULL;

  for (int i = 0; i < 10000; i++) {
    uint64_t key = lrand48() % 1000000;

    root = bst_insert(root, key);
    // printf("<%ld,%ld,%ld,%ld>", i,key,root->depth,root->size);
    if (root->size != i + 1) {
      printf("BAD!!!");
      break;
    }
  }
  printf("\n\n");
  printf("%ld\n", root->depth);
}
