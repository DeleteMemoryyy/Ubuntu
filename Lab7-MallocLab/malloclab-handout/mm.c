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

/* Define rb_tree struct */
static struct rb_node_t
{
#define RB_RED 0
#define RB_BLACK 4
    struct rb_node_t *rb_lChild, *rb_rChild;
    void* rb_parent_color;
} __attribute__(aligned(ALIGNMENT));
typedef struct rb_node_t node_t;

static struct rb_root_t
{
    struct rb_node_t* rb_node;
};
typedef struct rb_root_t root_t;

/* Define rb_tree macros */
#define rb_parent(tn) ((node_t*)((tn)->rb_parent_color & ~0x7))
#define rb_color(tn) ((tn)->rb_parent_color & 0x4)
#define rb_is_red(tn) (!rb_color(tn))
#define rb_is_black(tn) (rb_color(tn))
#define rb_set_red(tn)                                                         \
    do                                                                         \
    {                                                                          \
        (tn)->rb_parent_color &= ~0x4;                                         \
    }
}
while (0)
#define rb_set_black(tn)
    do
    {
        (tn)->rb_parent_color |= 0x4;
    } while (0)

        static inline void
        rb_set_parent(node_t* node, node_t* parent)
    {
        node->rb_parent_color = (node->rb_parent_color & 0x7) | (void*)parent;
    }

static inline void rb_set_color(node_t* node, int color)
{
    node->rb_parent_color = (node->rb_parent_color & ~0x1) | color;
}

static void rb_left_rotate(node_t* node, root_t* root)
{
    node_t* parent = rb_parent(node);
    node_t* right = node->rb_rChild;

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

static void rb_right_rotate(node_t* node, root_t* root)
{
    node_t* parent = rb_parent(node);
    node_t* left = node->rb_lChild;

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

static inline void rb_link_node(node_t* node, node_t* parent, node_t** rb_link)
{
    /* Set node's parent and color
     * Default color of new node is red (0) */
    node->rb_parent_color = (void*)parent;

    node->rb_lChild = node->rb_rChild = NULL;
    /* Set relationship between parent and child,
     * rb_link could be *rb_lChild or *rb_rChild */
    *rb_link = node;
}

static void rb_insert_adjust(node_t* node, root_t* root)
{
    node_t *parent, *gparent;

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
                node_t* uncle;
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
                node_t* tmp_node = parent;
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
                node_t* uncle;
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
                node_t* tmp_node = parent;
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

static void rb_delete_adjust(node_t* node, node_t* parent, root_t* root)
{
    node_t* sibling;

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
                    node_t* sibling_left;
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
                 *continueremoge555 and then left rotate to make it Case 6 */
                if (!sibling->rb_lChild
                    || rb_color(sibling->rb_lChild) == RB_BLACK)
                {
                    node_t* sibling_right;
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

static void rb_delete_node(node_t* node, root_t* root)
{
    node_t *parent, *child;
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
        node_t *old = node, *tmp;
        node_t* old_parent = rb_parent(old);
        node_t* old_left = old->rb_lChild;

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

        /* Exchange the position of node to be deleted and successor */
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

#define WSIZE 4 /* Word and header/footer size (bytes) */
#define DSIZE 8 /* Double word size (bytes) */
#define RBTNSIZE 0x18 /* Red-black tree node size (bytes) */
#define MAXSEGSIZE 0x80 /* Max size of segregated block (bytes)*/

/* Pack a size or a address offset, type and allocated bit into a word */
#define PACK(val, type, alloc) ((val) | ((type) << 1) | (alloc))

/* Read and write a word at address p */
#define GET(p) (*(unsigned int*)(p))
#define PUT(p, val) (*(unsigned int*)(p) = (val))

/* Read the size and address offset from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_OFFSET(P) (GET(p) & ~0x7)

/* Read the allocated fields and type from address p */
#define GET_ALLOC(p) (GET(p) & 0x1)
#define GET_TYPE(p) (GET(p) & 0x2)

/* Given ptr bp, compute address of its header */
#define HDRP(bp) ((char*)(bp)-WSIZE)

/* Given segregated block ptr bp, compute address of its footer */
#define SEG_FTRP(bp) ((char*)(bp) + GET_SIZE(HDRP(bp)))

/* Given block ptr bp, compute address of its red-black tree node */
#define GET_RBTNP(bp) ((node_t*)((char*)(bp)-RBTNSIZE))

/* Given red-black node ptr np, compute address of its block */
#define GET_BLK(np) ((void*)((char*)(np) + RBTNSIZE))

/* Global variables */
static char* heap_rbtnp = NULL; /* Pointer to pointer of red-black tree root */
static char* heap_segbp = NULL; /* Pointer to tail pointer of smallest segregated black list */
static const int heap_segbnum = MAXSEGSIZE/DSIZE; /* Number of segregated block pointer link */

/*
 * Initialize: return -1 on error, 0 on success.
 */
int mm_init(void) 
{
    /* Create the initial empty heap */
    if ((heap_rbtnp = mem_sbrk((1+heap_segbnum) * DSIZE + WSIZE)) == (void*)-1)
        return -1;
    heap_segbp = heap_rbtnp + 1;
    memset(heap_rbtnp, 0, (1+heap_segbnum) * DSIZE + WSIZE));
    return 0;
}

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
