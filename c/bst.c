#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

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

  // walk up the tree setting depth and size
  while(true) {
    node->size = node->count + SIZE(node->right) + SIZE(node->left);
    node->depth = 1 + MAX(DEPTH(node->right),DEPTH(node->left));
    if (node->parent == NULL) {
      break;
    }
    node = node->parent;
  }
  
  return node;
} 

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

int 
main()
{
  srand48(time(NULL));
  bst_node_t *root = NULL;

  for (int i = 0; i < 50; i++) {
    uint64_t key = lrand48() % 100;

    root = bst_insert(root, key);
    printf("<%ld,%ld,%ld>", i,root->depth,root->size);
  }
  printf("\n\n");

  bst_display(root, 0);
}
