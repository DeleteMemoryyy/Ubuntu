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

static struct t_rb_node
{
    ptr_t rb_parent_color;
#define RB_RED 0
#define RB_BLACK 1
    struct t_rb_node *rb_lChild, *rb_rChild;
    int size;
    ptr_t p;
} __attribute__(aligned(ALIGNMENT));
typedef struct t_rb_node t_node;

static struct t_rb_root
{
    struct t_rb_node* rb_node;
};
typedef struct t_rb t_root;

#define rb_parent(tn) ((t_node*)((tn)->rb_parent_color & ~7))
#define rb_color(tn) ((tn)->rb_parent_color & 1)
#define rb_is_red(tn) (!rb_color(tn))
#define rb_is_black(tn) (rb_color(tn))
#define rb_root_is_empty(root) ((root) == NULL)
#define rb_is_empty(tb) (rb_parent(tn) == (tn))
#define rb_set_red(tn)                                                         \
    do                                                                         \
        {                                                                      \
            (tn)->rb_parent_color &= ~1;                                       \
        }                                                                      \
    while (0)
#define rb_set_black(tn)                                                       \
    do                                                                         \
        {                                                                      \
            (tn)->rb_parent_color |= 1;                                        \
        }                                                                      \
    while (0)

static inline void rb_set_parent(t_node* node, t_node* parent)
{
    node->rb_parent_color = (node->rb_parent_color & 7) | (ptr_t)parent;
}

static inline void rb_set_color(t_node* node, int color)
{
    node->rb_parent_color = (node->rb_parent_color & ~1) | color;
}

static void rb_left_rotate(t_node* node, t_root* root)
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
    if (parent)
        {
            if (node == parent->rb_lChild)
                parent->rb_lChild = right;
            else
                parent->rb_rChild = right;
        }
    else
        root->rb_node = right;
}

static void rb_right_rotate(t_node* node, t_root* root)
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
    if (parent)
        {
            if (node == parent->rb_lChild)
                parent->rb_lChild = left;
            else
                parent->rb_rChild = left;
        }
    else
        root->rb_node = left;
}

static inline void rb_link_node(t_node* node, t_node* parent, t_node** rb_link)
{
    /* Set node's parent and color
     * Default color of new node is red (0) */
    node->rb_parent_color = (ptr_t)parent;

    node->rb_lChild = node->rb_rChild = NULL;
    /* Set relationship between parent and child,
     * rb_link could be *rb_lChild or *rb_rChild */
    *rb_link = node;
}

static void rb_insert_adjust(t_node* node, t_root* root)
{
    t_node *parent, *gparent;

    /* Adjust position and color while color of the node's parent is red */
    while ((parent = rb_parent(node)) && rb_is_red(parent))
        {
            /* gparent won't be NULL, because color of parent is red
             * which means parent isn't root node */
            gparent = rb_parent(parent);

            if (parent == gparent->rb_lChild)
                {
                    /* Case 3: If uncle exists and its color is red,
                     * change color of gparent/parent/uncle */
                    {
                        t_node* uncle;
                        if ((uncle = gparent->rb_rChild) && rb_is_red(uncle))
                            {
                                rb_set_red(gparent);
                                rb_set_black(parent);
                                rb_set_black(uncle);
                                node = gparent;
                                continue;
                            }
                    }

                    /* Case 4: Color of uncle  is black
                     * and node is parent's right child (LR) */
                    if (node == parent->rb_rChild)
                        {
                            /* Left rotate from parent and change the situation
                             * in to case 5 */
                            rb_left_rotate(parent, root);

                            /* Exchange node and parent to make they get
                             * correct meanings */
                            t_node* tmp_node = parent;
                            parent = node;
                            node = tmp_node;
                        }

                    /* Case 5: LL with red parent,
                     * exchange colors of parent and gparent
                     * and right rotate gparent */
                    rb_set_red(gparent);
                    rb_set_black(parent);
                    rb_right_rotate(gparent, root);
                }
            /* Mirrored situation */
            else
                {
                    /* Case 3: If uncle exists and its color is red,
                     * change color of gparent/parent/uncle */
                    {
                        t_node* uncle;
                        if ((uncle = gparent->rb_lChild) && rb_is_red(uncle))
                            {
                                rb_set_red(gparent);
                                rb_set_black(parent);
                                rb_set_black(uncle);
                                node = gparent;
                                continue;
                            }
                    }

                    /* Case 4: Color of uncle is black
                     * and node is parent's left child (RL) */
                    if (node == parent->rb_lChild)
                        {
                            /* Right rotate from parent and change the 
                             * situation in to case 5 */
                            rb_right_rotate(parent, root);

                            /* Exchange node and parent to make they get 
                             * correct meanings */
                            t_node* tmp_node = parent;
                            parent = node;
                            node = tmp_node;
                        }

                    /* Case 5: RR with red parent,
                     * exchange colors of parent and gparent
                     * and left rotate gparent */
                    rb_set_red(gparent);
                    rb_set_black(parent);
                    rb_left_rotate(gparent, root);
                }
        }
    rb_set_black(root->rb_node);
}

static void rb_delete_adjust(t_node* node, t_node* parent, t_root* root)
{
    t_node* sibling;

    while ((!node || rb_color(node) == RB_BLACK) && node != root->rb_node)
        {
            if (node == parent->rb_lChild)
                {

                    /* It will start adjusting only if the node to be deleted's
                     * black-height is at least 2,
                     * so sibling won't be NULL */
                    sibling = parent->rb_rChild;

                    /* Case 2: Parent is black while sibling is red
                     * Exchange colors of parent and sibling,
                     * left rotate parent node */
                    if (rb_color(sibling) == RB_RED)
                        {
                            rb_set_black(sibling);
                            rb_set_red(parent);
                            rb_left_rotate(parent, root);
                            sibling = parent->rb_rChild;
                        }

                    /* Case 3 & Case 4: Sibling's children are all black,
                     * Set sibling red and adjust from parent */
                    if ((!sibling->rb_lChild
                            || rb_color(sibling->rb_lChild) == RB_BLACK)
                        && (!sibling->rb_rChild
                               || rb_color(sibling->rb_rChild) == RB_BLACK))
                        {
                            rb_set_red(sibling);
                            node = parent;
                            parent = rb_parent(node);
                        }
                    else
                        {
                            /* Case 5: Sibling's left child is red,
                             * exchange colors of sibling and its left child,
                             * and then right rotate to make it Case 6 */
                            if (!sibling->rb_rChild
                                || rb_color(sibling->rb_rChild) == RB_BLACK)
                                {
                                    t_node* sibling_left;
                                    if (sibling_left = sibling->rb_lChild)
                                        rb_set_black(sibling_left);
                                    rb_set_red(sibling);
                                    rb_right_rotate(sibling, root);
                                    sibling = parent->rb_rChild;
                                }

                            /* Case 6: Sibling's right child is red,
                             * exchange colors of sibling and parent,
                             * set parent and sibling's right child black,
                             * left rotate and then stop sdjusting */
                            rb_set_color(sibling, rb_color(parent));
                            rb_set_black(parent);

                            if (sibling->rb_rChild)
                                rb_set_black(sibling->rb_rChild);

                            rb_left_rotate(parent, root);

                            node = root->rb_node;
                        }
                }
            /* Mirrored situation */
            else
                {

                    /* It will start adjusting only if the node to be deleted's
                     * black-height is at least 2,
                     * so sibling won't be NULL */
                    sibling = parent->rb_lChild;

                    /* Case 2: Parent is black while sibling is red
                     * Exchange colors of parent and sibling,
                     * right rotate parent node */
                    if (rb_color(sibling) == RB_RED)
                        {
                            rb_set_black(sibling);
                            rb_set_red(parent);
                            rb_right_rotate(parent, root);
                            sibling = parent->rb_lChild;
                        }

                    /* Case 3 & Case 4: Sibling's children are all black,
                     * Set sibling red and adjust from parent */
                    if ((!sibling->rb_lChild
                            || rb_color(sibling->rb_lChild) == RB_BLACK)
                        && (!sibling->rb_rChild
                               || rb_color(sibling->rb_rChild) == RB_BLACK))
                        {
                            rb_set_red(sibling);
                            node = parent;
                            parent = rb_parent(node);
                        }
                    else
                        {
                            /* Case 5: Sibling's right child is red,
                             * exchange colors of sibling and its right child,
                             * and then left rotate to make it Case 6 */
                            if (!sibling->rb_lChild
                                || rb_color(sibling->rb_lChild) == RB_BLACK)
                                {
                                    t_node* sibling_right;
                                    if (sibling_right = sibling->rb_rChild)
                                        rb_set_black(sibling_right);
                                    rb_set_red(sibling);
                                    rb_left_rotate(sibling, root);
                                    sibling = parent->rb_lChild;
                                }

                            /* Case 6: Sibling's left child is red,
                             * exchange colors of sibling and parent,
                             * set parent and sibling's left child black,
                             * right rotate and then stop sdjusting */
                            rb_set_color(sibling, rb_color(parent));
                            rb_set_black(parent);

                            if (sibling->rb_lChild)
                                rb_set_black(sibling->rb_lChild);

                            rb_right_rotate(parent, root);

                            node = root->rb_node;
                        }
                }
        }

    if (node)
        rb_set_black(node);
}

static void rb_delete_node(t_node* node, t_root* root)
{
    t_node *parent, *child;
    int color;

    /* The node to be deleted has only one child
     * or doesn't have a child */
    if (!node->rb_rChild)
        child = node->rb_lChild;
    else if (!node->rb_lChild)
        child = node->rb_rChild;

    /* The node to be deleted has two children 
     * Choose its successor to replace it */
    else
        {
            t_node *old = node, *tmp;
            t_node* old_parent = rb_parent(old);
            t_node* old_left = old->rb_lChild;

            /* Look for successor of node */
            node = node->right;
            while ((tmp = node->rb_lChild))
                node = tmp;

            /* Save successor's information */
            child = node->rb_rChild;
            parent = rb_parent(node);
            color = rb_color(node);

            /* Set the relationship between successor's right child
             * and its parent */
            if (child)
                rb_set_parent(child, parent);
            if (node == parent->rb_lChild)
                parent->rb_lChild = child;
            else
                parent->rb_rChild = child;

            /* If successor is right child of the node to be deleted,
             * make sure the relationship between successor and its child
             * being set correctly */
            if (rb_parent(node) == old)
                parent = node;

            /* Exchange position of node to be deleted and successor */
            rb_set_parent(node, old_parent);
            rb_set_color(node, rb_color(old));
            node->rb_lChild = old->rb_lChild;
            node->rb_rChild = old->rb_rChild;

            if (old_parent)
                {
                    if (old == old_parent->rb_lChild)
                        old_parent->rb_lChild = node;
                    else
                        old_parent->rb_rChild = node;
                }
            /* If the node to be deleted is root,
             * make successor new root of the rb_tree */
            else
                root->rb_node = node;

            rb_set_parent(old_left, node);
            if (old->rb_rChild)
                rb_set_parent(old->rb_rChild, node);

            /* If the color of moved node is black,
             * the rb_tree need further adjustment */
            if (color == RB_BLACK)
                rb_delete_adjust(child, parent, root);
            return;
        }

    parent = node->parent;
    color = rb_color(node);

    /* Set the relationship between successor's child
     * and its parent if necessary */
    if (child)
        rb_set_parent(child, parent);

    if (parent)
        {
            if (node == parent->rb_lChild)
                parent->rb_lChild = child;
            else
                parent->rb_rChild = child;
        }
    else
        root->rb_node = child;

    /* If the color of moved node is black,
     * the rb_tree need further adjustment */
    if (color == RB_BLACK)
        rb_delete_adjust(child, parent, root);
}

/*
 * Initialize: return -1 on error, 0 on success.
 */
int mm_init(void) { return 0; }

/*
 * malloc
 */
void* malloc(size_t size) { return NULL; }

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
void* realloc(void* oldptr, size_t size) { return NULL; }

/*
 * calloc - you may want to look at mm-naive.c
 * This function is not tested by mdriver, but it is
 * needed to run the traces.
 */
void* calloc(size_t nmemb, size_t size) { return NULL; }

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
static int aligned(const void* p) { return (size_t)ALIGN(p) == (size_t)p; }

/*
 * mm_checkheap
 */
void mm_checkheap(int lineno) {}
