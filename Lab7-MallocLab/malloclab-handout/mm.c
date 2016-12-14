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
/* createe aliases for driver tests */
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
struct rb_node_t
{
#define RB_RED 0
#define RB_BLACK 1
    struct rb_node_t *rb_lChild, *rb_rChild;
    long unsigned int rb_parent_color;
};
typedef struct rb_node_t node_t;

struct rb_root_t
{
    struct rb_node_t* rb_node;
};
typedef struct rb_root_t root_t;

#define TYPE_SEGB 0
#define TYPE_RBTN 2
#define UNALLOCED 0
#define ALLOCED 1

#define WSIZE 4 /* Word and header/footer size (bytes) */
#define DSIZE 8 /* Double word size (bytes) */
#define RBTNSIZE 0x18 /* Red-black tree node size (bytes) */

#define MINBLKSIZE (DSIZE * 2)

/* Max size of segregated block (bytes)
 * while at the same time it's the size of tail pointer of segregated block link
 * array */
#define MAXSEGBLKSIZE 0x80

typedef unsigned int offset_t;

/* Pack a size or a address offset, type and allocated bit into a word */
#define PACK(val, type, alloc) ((val) | (type) | (alloc))

/* Get and write a word at address p */
#define GET(p) (*(unsigned int*)(p))
#define PUT(p, val) (*(unsigned int*)(p) = (val))

/* Get size from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)

/* Read size from value */
#define READ_SIZE(val) ((unsigned int)(val) & ~0x7)

/* Compute address for header of given block ptr bp */
#define HDRP(bp) ((char*)(bp)-WSIZE)

/* Compute address for footer of given block ptr bp */
#define FTRP(bp) ((char*)(bp) + GET_SIZE(HDRP(bp)))

#define IS_UNALLOCED(info) (((unsigned int)(info)&0x1) == UNALLOCED)

#define IS_SEGB(info) (((unsigned int)(info)&0x2) == TYPE_SEGB)

#define seg_successor_offset(bp) ((offset_t)(GET((bp))))

#define seg_prev_offset(bp) ((offset_t)(GET((char*)(bp) + WSIZE)))

#define seg_set_successor(bp, offset) (PUT((bp), (offset)))

#define seg_set_prev(bp, offset) (PUT((char*)(bp) + WSIZE, (offset)))

/* Global variables */
static root_t* heap_rbrp = 0; /* Pointer to pointer of red-black tree root */
static char** heap_segbp
    = 0; /* Pointer to tail pointer of smallest segregated block list array */
// static char** heap_segbp_end_search = 0;
static char** heap_edsegbp = 0; /* Pointer to after tail pointer of smallest
                                   segregated block list array */
static char* heap_st = 0;

#define TO_ADD(offset)                                                         \
    ((char*)((long unsigned int)(offset_t) + (long unsigned int)heap_st))
#define TO_OFFSET(add)                                                         \
    ((offset_t)((long unsigned int)add - (long unsigned int)heap_st))

static inline void rb_link_node(node_t* node, node_t* parent, node_t** rb_link);
static void rb_insert_node(node_t* cur_node, node_t* node, size_t asize);
static void rb_delete_node(node_t* node);
static void seg_delete_blk(char* bp);
static void* find_fit_rbtn(node_t* cur_node, size_t asize);
static char* create_blk(size_t asize);
static void place_blk(char* bp, size_t asize);
static void devide_blk(char* bp, size_t asize);
static void* coalesce_blk(char* bp, size_t asize);

/*
 * Initialize: return -1 on error, 0 on success.
 */
int mm_init(void)
{
    /* createe the initial empty heap */
    if ((void*)(heap_rbrp = mem_sbrk(MAXSEGBLKSIZE + 2 * DSIZE + WSIZE))
        == (void*)-1)
        return -1;
    heap_st = (char*)heap_rbrp;
    heap_segbp = (char**)((char*)heap_rbrp + DSIZE);
    // heap_segbp_end_search = heap_segbp - 1;
    heap_edsegbp = heap_segbp + MAXSEGBLKSIZE / DSIZE;
    memset(heap_rbrp, 0, (MAXSEGBLKSIZE + DSIZE + WSIZE));
    PUT(heap_edsegbp, PACK(0, 0, ALLOCED));
    PUT((char*)heap_edsegbp + WSIZE, PACK(0, 0, ALLOCED));

#ifdef DEBUG
    printf("\n\nInitialization finished\n");
    printf("    heap_st:%lx\n", (size_t)heap_st);
    printf("    heap_rbrp:%lx\n", (size_t)heap_rbrp);
    printf("    heap_segbp:%lx\n", (size_t)heap_segbp);
    // printf("    heap_segbp_end_search:%lx\n", (size_t)heap_segbp_end_search);
    printf("    heap_edsegbp:%lx\n", (size_t)heap_edsegbp);
#endif
    return 0;
}

/*
 * malloc
 */
void* malloc(size_t size)
{
    size_t asize; /* Adjusted block size */

    char** p_ptr;
    char* bp = NULL;

    if (heap_rbrp == 0)
    {
        mm_init();
    }

    if (size == 0)
        return NULL;

    asize = ALIGN(size);

#ifdef DEBUG
    printf("\n\nMalloc  requiring size:%lu, adjusted size:%lu\n", size, asize);
#endif

    if (asize <= MAXSEGBLKSIZE)
    {
#ifdef DEBUG
        printf("    Asking for segregated block\n");
#endif
        p_ptr = heap_segbp + asize / DSIZE - 1;
        while (p_ptr < heap_edsegbp && !(*p_ptr))
            ++p_ptr;
        if (p_ptr < heap_edsegbp)
        {
            bp = *p_ptr;
#ifdef DEBUG
            printf("       Found available segregated block: address = %lx, "
                   "size = %u\n",
                (size_t)bp, GET_SIZE(HDRP(bp)));
#endif
            offset_t suc_os = seg_successor_offset(bp);
            if (suc_os)
            {
                char* suc_add = TO_ADD(suc_os);
                seg_set_prev(suc_add, 0);
                *p_ptr = suc_add;
            }
            else
                *p_ptr = NULL;

            devide_blk(bp, asize);
            return (void*)bp;
        }
    }
#ifdef DEBUG
    printf("    Asking for red-black tree block\n");
#endif

    node_t* fit_node = find_fit_rbtn(heap_rbrp->rb_node, asize);
    if (!fit_node)
    {
        if (!(bp = create_blk(asize)))
        {
#ifdef DEBUG
            printf("    Fail to find available block. Go to create_blk\n");
#endif
            return NULL;
        }

        return (void*)bp;
    }

    rb_delete_node(fit_node);
    bp = (char*)fit_node;
    devide_blk(bp, asize);
    return (void*)bp;
}

/*
 * free
 */
void free(void* ptr)
{
    if (!ptr)
        return;
    size_t asize = GET_SIZE(HDRP(ptr));

    ptr = coalesce_blk((char*)ptr, asize);
}

/*
 * realloc - you may want to look at mm-naive.c
 */
void* realloc(void* old_ptr, size_t asize)
{
    size_t old_size;

    void* new_ptr;

    if (asize == 0)
    {
        free(old_ptr);
        return NULL;
    }

    if (old_ptr == NULL)
        return malloc(asize);

    old_size = GET_SIZE(HDRP(old_ptr));

    if (asize <= old_size)
    {
        devide_blk(old_ptr, asize);
        new_ptr = old_ptr;
    }
    else
    {
        if (!(new_ptr = malloc(asize)))
            return NULL;

        memcpy(new_ptr, old_ptr, old_size);

        free(old_ptr);
    }

    return new_ptr;
}

/*
 * calloc - you may want to look at mm-naive.c
 * This function is not tested by mdriver, but it is
 * needed to run the traces.
 */
void* calloc(size_t nmemb, size_t size)
{
    size_t bytes = nmemb * size;
    void* newptr;

    newptr = malloc(bytes);
    memset(newptr, 0, bytes);

    return newptr;
}

/* Functions of rb_tree */
/* Define rb_tree macros */
#define rb_parent(tn) ((node_t*)((tn)->rb_parent_color & ~0x7))
#define rb_color(tn) ((tn)->rb_parent_color & 0x1)
#define rb_is_red(tn) (!rb_color(tn))
#define rb_is_black(tn) (rb_color(tn))
// #define rb_set_red(tn) ((tn)->rb_parent_color &= ~0x1)
// #define rb_set_black(tn) ((tn)->rb_parent_color |= 0x1)

static inline void rb_set_red(node_t* node)
{
    node->rb_parent_color = (long unsigned int)((
        (long unsigned int)(node->rb_parent_color) & ~0x1));
}

static inline void rb_set_black(node_t* node)
{
    node->rb_parent_color = (long unsigned int)((
        (long unsigned int)(node->rb_parent_color) | 0x1));
}

static inline void rb_set_parent(node_t* node, node_t* parent)
{
    node->rb_parent_color
        = (long unsigned int)(((long unsigned int)(node->rb_parent_color) & 0x7)
            | (long unsigned int)parent);
}

static inline void rb_set_color(node_t* node, int color)
{
    node->rb_parent_color = (node->rb_parent_color & ~0x1) | color;
}

static void rb_left_rotate(node_t* node)
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
        heap_rbrp->rb_node = right;
}

static void rb_right_rotate(node_t* node)
{
    node_t* parent = rb_parent(node);
    node_t* left = node->rb_lChild;

    /* Set the relationship between node's left child's right child and node */
    if ((node->rb_lChild = left->rb_rChild))
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
        heap_rbrp->rb_node = left;
}

static inline void rb_link_node(node_t* node, node_t* parent, node_t** rb_link)
{
    /* Set node's parent and color
     * Default color of new node is red (0) */
    node->rb_parent_color = (long unsigned int)parent;

    node->rb_lChild = node->rb_rChild = NULL;
    /* Set relationship between parent and child,
     * rb_link could be *rb_lChild or *rb_rChild */
    *rb_link = node;
}

static void rb_insert_adjust(node_t* node)
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
                rb_left_rotate(parent);

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
            rb_right_rotate(gparent);
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
                rb_right_rotate(parent);

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
            rb_left_rotate(gparent);
        }
    }
    rb_set_black(heap_rbrp->rb_node);
}

static void rb_insert_node(node_t* cur_node, node_t* node, size_t asize)
{
    assert(cur_node);
    size_t cur_size = GET_SIZE(HDRP(cur_node));
    if (asize < cur_size)
        if (cur_node->rb_lChild)
            rb_insert_node(cur_node->rb_lChild, node, asize);
        else
        {
            rb_link_node(node, cur_node, &(cur_node->rb_lChild));
            rb_set_black(node);
            rb_insert_adjust(node);
        }
    else if (cur_node->rb_rChild)
        rb_insert_node(cur_node->rb_rChild, node, asize);
    else
    {
        rb_link_node(node, cur_node, &(cur_node->rb_rChild));
        rb_set_black(node);
        rb_insert_adjust(node);
    }
}

static void rb_delete_adjust(node_t* node, node_t* parent)
{
    node_t* sibling;

    while ((!node || rb_color(node) == RB_BLACK) && node != heap_rbrp->rb_node)
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
                rb_left_rotate(parent);
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
                    if ((sibling_left = sibling->rb_lChild))
                        rb_set_black(sibling_left);
                    rb_set_red(sibling);
                    rb_right_rotate(sibling);
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

                rb_left_rotate(parent);

                node = heap_rbrp->rb_node;
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
                rb_right_rotate(parent);
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
                    if ((sibling_right = sibling->rb_rChild))
                        rb_set_black(sibling_right);
                    rb_set_red(sibling);
                    rb_left_rotate(sibling);
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

                rb_right_rotate(parent);

                node = heap_rbrp->rb_node;
            }
        }
    }

    if (node)
        rb_set_black(node);
}

static void rb_delete_node(node_t* node)
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
        node = node->rb_rChild;
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
            heap_rbrp->rb_node = node;

        rb_set_parent(old_left, node);
        if (old->rb_rChild)
            rb_set_parent(old->rb_rChild, node);

        /* If the color of moved node is black,
         * the rb_tree need further adjustment */
        if (color == RB_BLACK)
            rb_delete_adjust(child, parent);
        return;
    }

    parent = rb_parent(node);
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
        heap_rbrp->rb_node = child;

    /* If the color of moved node is black,
     * the rb_tree need further adjustment */
    if (color == RB_BLACK)
        rb_delete_adjust(child, parent);
}

static void seg_delete_blk(char* bp)
{
    size_t asize = GET_SIZE(HDRP(bp));
    offset_t cur_suc_os = seg_successor_offset(bp),
             cur_pre_os = seg_prev_offset(bp);
    if (cur_suc_os)
    {
        if (cur_pre_os)
        {
            seg_set_prev(TO_ADD(cur_suc_os), cur_pre_os);
            seg_set_successor(TO_ADD(cur_pre_os), cur_suc_os);
        }
        else
            seg_set_prev(TO_ADD(cur_suc_os), 0);
    }
    else
    {
        char** p_ptr = heap_segbp + asize / DSIZE - 1;
        // while(p_ptr != heap_segbp_end_search && *p_ptr != bp)
        //     --p_ptr;
        assert(*p_ptr == bp);

        if (cur_pre_os)
        {
            char* pre_node = TO_ADD(cur_pre_os);
            seg_set_successor(pre_node, 0);
            *p_ptr = pre_node;
        }
        else
            *p_ptr = NULL;
    }
}

static void* find_fit_rbtn(node_t* cur_node, size_t asize)
{
    if (!cur_node)
        return NULL;

    size_t tsize = GET_SIZE(HDRP(cur_node));
    if (asize == tsize)
        return cur_node;
    if (asize > tsize)
        return find_fit_rbtn(cur_node->rb_rChild, asize);

    node_t* tmp_node;
    if ((tmp_node = find_fit_rbtn(cur_node->rb_lChild, asize)))
        return tmp_node;
    return cur_node;
}

static char* create_blk(size_t asize)
{
#ifdef DEBUG
    printf("        Creating block  requring size = %lu\n", asize);
#endif
    char* bp;
    if ((long)(bp = mem_sbrk(asize + DSIZE)) == -1)
    {
#ifdef DEBUG
        printf("!!!mem_sbrk failed\n");
#endif
        return NULL;
    }

#ifdef DEBUG
    printf(
        "        Creating successfully  address = %lx, adjusted size = %lu\n",
        (size_t)(bp + WSIZE), asize);
#endif

    PUT(bp, PACK(asize, 0, ALLOCED));
    PUT((bp + asize + WSIZE), PACK(asize, 0, ALLOCED));

    return bp + WSIZE;
}

static void place_blk(char* bp, size_t asize)
{
    assert(asize >= DSIZE);
    if (asize > MAXSEGBLKSIZE)
    {
        PUT(HDRP(bp), PACK(asize, TYPE_RBTN, UNALLOCED));
        PUT(bp + asize, PACK(asize, TYPE_RBTN, UNALLOCED));
        rb_insert_node(heap_rbrp->rb_node, (node_t*)(bp), asize);
    }
    else
    {
        PUT(HDRP(bp), PACK(asize, TYPE_SEGB, UNALLOCED));
        PUT(bp + asize, PACK(asize, TYPE_SEGB, UNALLOCED));

        char** p_ptr = heap_segbp + asize / DSIZE - 1;
        char* tmp_p = *p_ptr;
        seg_set_prev(bp, 0);
        if (tmp_p)
        {
            seg_set_prev(tmp_p, TO_OFFSET(bp));
            seg_set_successor(bp, TO_OFFSET(tmp_p));
        }
        else
        {
            seg_set_successor(bp, 0);
            *p_ptr = bp;
        }
    }
}

static void devide_blk(char* bp, size_t asize)
{
    int rest_size = GET_SIZE(HDRP(bp)) - asize;
    if (rest_size < MINBLKSIZE)
        return;
    PUT(bp, PACK(asize, 0, ALLOCED));
    PUT(bp + asize, PACK(asize, 0, ALLOCED));
    place_blk(bp + asize + DSIZE, rest_size - DSIZE);
}

static void* coalesce_blk(char* bp, size_t asize)
{
    unsigned int prev_info = GET(bp - DSIZE),
                 next_info = ((long unsigned int)(bp + asize + WSIZE)
                                 <= (long unsigned int)mem_heap_hi())
        ? (GET(bp + asize + WSIZE))
        : ALLOCED;

    if (IS_UNALLOCED(next_info))
    {
        asize += READ_SIZE(next_info) + DSIZE;
        char* next_ptr = bp + asize + DSIZE;
        if (IS_SEGB(next_info))
            seg_delete_blk(next_ptr);
        else
            rb_delete_node((node_t*)next_ptr);
    }

    if (IS_UNALLOCED(prev_info))
    {
        size_t prev_size = READ_SIZE(prev_info);
        asize += prev_size + DSIZE;
        bp -= (prev_size + DSIZE);
        if (IS_SEGB(prev_info))
            seg_delete_blk(bp);
        else
            rb_delete_node((node_t*)bp);
    }

    place_blk(bp, asize);

    return (void*)bp;
}

/*
 * Return /whether the pointer is in the heap.
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
