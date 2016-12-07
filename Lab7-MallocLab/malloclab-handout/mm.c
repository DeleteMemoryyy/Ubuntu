/*
 * mm.c
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "memlib.h"
#include "mm.h"

/* If you want debugging output, use the following macro.  When you hand
 * in, remove the #define DEBUG line. */
#define DEBUG
#ifdef DEBUG
#define dbg_printf(...) printf(__VA_ARGS__)
#else
#define dbg_printf(...)
#endif

/* do not change the following! */
#ifdef DRIVER
/* create aliases for driver tests */
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#endif /* def DRIVER */

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(p) (((size_t)(p) + (ALIGNMENT - 1)) & ~0x7)

typedef unsigned int ptr_t;

static struct t_rb_node {
    ptr_t rb_parent_color;
#define RB_RED 0
#define RB_BLACK 1
    struct t_rb_node *rb_lChild, *rb_rChild;
    int size;
    ptr_t p;
} __attribute__(aligned(ALIGNMENT));
typedef struct t_rb_node t_node;

static struct t_rb_root {
    struct t_rb_node* rb_node;
    // struct t_rb_node* rb_nil;
};
typedef struct t_rb t_root;

#define rb_parent(tn) ((t_node*)((tn)->rb_parent_color & ~7))
#define rb_color(tn) ((tn)->rb_parent_color & 1)
#define rb_is_red(tn) (!rb_color(tn))
#define rb_is_black(tn) (rb_color(tn))
#define rb_root_is_empty(root) ((root) == NULL)
#define rb_is_empty(tb) (rb_parent(tn) == (tn))
#define rb_set_red(tn)               \
    do {                             \
        (tn)->rb_parent_color &= ~1; \
    } while (0)
#define rb_set_black(tn)            \
    do {                            \
        (tn)->rb_parent_color |= 1; \
    } while (0)

static inline void rb_set_parent(t_node* node, t_node* parent)
{
    node->rb_parent_color = (node->rb_parent_color & 7) | (ptr_t)parent;
}

static inline void rb_set_color(t_node* node, int color)
{
    node->rb_parent_color = (node->rb_parent_color & ~1) | color;
}

static t_node* rb_left_rotate(t_node* node, t_root* root)
{
    t_node* parent = rb_parent(node);
    t_node* right = node->rb_rChild;

    /* Set the relationship between node's right child's left child and node */
    if ((node->rb_rChild = right->rb_lChild))
        rb_set_parent(right->rb_lChild, node);

    /* Set the relationship between node and the new node */
    right->rb_lChild = node;
    rb_set_parent(node, right);

    /* Set the relationship between the new old and the old parent */
    rb_set_parent(right, parent);
    if (parent) {
        if (node == parent->rb_lChild)
            parent->rb_lChild = right;
        else
            parent->rb_rChild = right;
    } else
        root->rb_node = right;
}

static t_node* rb_right_rotate(t_node* node, t_root* root)
{
    t_node* parent = rb_parent(node);
    t_node* left = node->rb_lChild;

    /* Set the relationship between node's left child's right child and node */
    if ((node->lb_rChild = left->rb_rChild))
        rb_set_parent(left->rb_rChild, node);

    /* Set the relationship between node and the new node */
    left->rb_rChild = node;
    rb_set_parent(node, left);

    /* Set the relationship between the new old and the old parent */
    rb_set_parent(left, parent);
    if (parent) {
        if (node == parent->rb_lChild)
            parent->rb_lChild = left;
        else
            parent->rb_rChild = left;
    } else
        root->rb_node = left;
}

static inline void rb_link_node(t_node* node, t_node* parent, t_node** rb_link)
{
    /* Set node's parent and color
     * The new node's default color is red (0) */
    node->rb_parent_color = (ptr_t)parent;

    node->rb_lChild = node->rb_rChild = NULL;

    /* Set relationship between parent and child,
     * rb_link could be *rb_lChild or *rb_rChild */
    *rb_link = node;
}

static void rb_insert_adjust(t_node* node, t_root* root)
{
    t_node *parent, *gparent;

    /* Adjust position and color while the node's parent's color is red */
    while ((parent = rb_parent(node)) && rb_is_red(parent)) {
        /* gparent won't be NULL, because parent's color is red
         * which means parent isn't root node */
        gparent = rb_parent(parent);
    }

    rb_set_black(root->rb_node);
}

/*
 * Initialize: return -1 on error, 0 on success.
 */
int mm_init(void)
{
    return 0;
}

/*
 * malloc
 */
void* malloc(size_t size)
{
    return NULL;
}

/*
 * free
 */
void free(void* ptr)
{
    if (!ptr)
        return;
}

/*
 * realloc - you may want to look at mm-naive.c
 */
void* realloc(void* oldptr, size_t size)
{
    return NULL;
}

/*
 * calloc - you may want to look at mm-naive.c
 * This function is not tested by mdriver, but it is
 * needed to run the traces.
 */
void* calloc(size_t nmemb, size_t size)
{
    return NULL;
}

/*
 * Return whether the pointer is in the heap.
 * May be useful for debugging.
 */
static int in_heap(const void* p)
{
    return p <= mem_heap_hi() && p >= mem_heap_lo();
}

/*
 * Return whether the pointer is aligned.
 * May be useful for debugging.
 */
static int aligned(const void* p)
{
    return (size_t)ALIGN(p) == (size_t)p;
}

/*
 * mm_checkheap
 */
void mm_checkheap(int lineno) {}
