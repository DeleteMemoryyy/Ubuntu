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
// #define DEBUG
// #define DEBUG_PRINT

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
    unsigned int rb_color, rb_size;
    struct rb_node_t* rb_parent;
    struct rb_node_t *rb_lChild, *rb_rChild;
    struct rb_node_t *rb_succ, *rb_prev;
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

#define MINBLK_WITHINFO_SIZE (DSIZE * 2)

#define MINBLKSIZE DSIZE

/* Max size of segregated block (bytes)
     * while at the same time it's the size of tail pointer of segregated block
 * link
     * array */
#define MAXSEGBLKSIZE 0x30

typedef unsigned int offset_t;

/* Pack a size or a address offset, type and allocated bit into a word */
#define PACK(val, type, alloc) ((val) | (type) | (alloc))

/* Get and write a word at address p */
#define GET(p) (*(unsigned int*)(p))
#define PUT(p, val) (*(unsigned int*)(p) = (val))

/* Get size from address p */
#define GET_SIZE(p) ((size_t)(GET((p)) & ~0x7))

/* Read size from value */
#define READ_SIZE(val) ((size_t)((unsigned int)(val) & ~0x7))

/* Compute address for header of given block ptr bp */
#define HDRP(bp) ((char*)(bp)-WSIZE)

/* Compute address for footer of given block ptr bp */
#define FTRP(bp) ((char*)(bp) + GET_SIZE(HDRP(bp)))

#define IS_ALLOCED(info) ((((unsigned int)(info)) & 0x1) == ALLOCED)

#define IS_UNALLOCED(info) ((((unsigned int)(info)) & 0x1) == UNALLOCED)

#define IS_SEGB(info) (((unsigned int)(info)&0x2) == TYPE_SEGB)

#define IS_RBTN(info) (((unsigned int)(info)&0x2) == TYPE_RBTN)

#define seg_successor_offset(bp) (GET((bp)))

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

static char* heap_blk_st = 0;

#define TO_ADD(offset)                                                         \
    ((char*)((long unsigned int)(offset) + (long unsigned int)(heap_st)))
#define TO_OFFSET(add)                                                         \
    ((offset_t)((long unsigned int)(add) - (long unsigned int)(heap_st)))

static void rb_insert_node(node_t* cur_node, node_t* node, size_t asize);
static void rb_delete_node(node_t* node);
static void seg_delete_blk(char* bp);
static node_t* find_fit_rbtn(node_t* cur_node, size_t asize);
static char* create_blk(size_t asize);
static void place_blk(char* bp, size_t asize);
static void devide_blk(char* bp, size_t asize);
static void coalesce_blk(char* bp, size_t asize);

/*
     * Initialize: return -1 on error, 0 on success.
     */
int mm_init(void)
{
    /* createe the initial empty heap */
    if ((void*)(heap_rbrp = (root_t*)mem_sbrk(MAXSEGBLKSIZE + 2 * DSIZE))
        == (void*)-1)
        return -1;
    heap_st = (char*)heap_rbrp;
    heap_segbp = (char**)((char*)heap_rbrp + DSIZE);
    // heap_segbp_end_search = heap_segbp - 1;
    heap_edsegbp = heap_segbp + MAXSEGBLKSIZE / DSIZE;
    heap_blk_st = (char*)heap_edsegbp + DSIZE;
    memset(heap_rbrp, 0, (MAXSEGBLKSIZE + DSIZE + WSIZE));
    PUT(heap_edsegbp, PACK(0, 0, ALLOCED));

#ifdef DEBUG_PRINT
    printf("\n\nInitialization finished\n");
    printf("Current heap bottom = %lx\n", (size_t)mem_heap_lo());
    printf("Current heap top = %lx\n", (size_t)mem_heap_hi());
    printf("    heap_st:%lx\n", (size_t)heap_st);
    printf("    heap_blk_st:%lx\n", (size_t)heap_blk_st);
    printf("    heap_rbrp:%lx\n", (size_t)heap_rbrp);
    printf("    heap_segbp:%lx\n", (size_t)heap_segbp);
    printf("    heap_segbp_end_search:%lx\n", (size_t)heap_segbp_end_search);
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

#ifdef DEBUG_PRINT
    printf(
        "\n\nMalloc  requisite size = %lu, adjusted size = %lu\n", size, asize);
    printf("Current heap bottom = %lx\n", (size_t)mem_heap_lo());
    printf("Current heap top = %lx\n", (size_t)mem_heap_hi());
#endif

    if (asize <= MAXSEGBLKSIZE)
    {

#ifdef DEBUG_PRINT
        printf("    Asking for segregated block\n");
#endif
        p_ptr = heap_segbp + asize / DSIZE - 1;
        while (p_ptr < heap_edsegbp && !(*p_ptr))
            ++p_ptr;
        if (p_ptr < heap_edsegbp)
        {
            bp = *p_ptr;

#ifdef DEBUG_PRINT
            printf("    Found available segregated block  address = %lx, "
                   "size = %lu\n",
                (size_t)bp, GET_SIZE(HDRP(bp)));
#endif

            offset_t pre_os = seg_prev_offset(bp);

#ifdef DEBUG_PRINT
            printf("            Deleting seg_blk  address = %lx, size = %lu\n",
                (size_t)bp, GET_SIZE(HDRP(bp)));
            printf("            successor_offset = %x, prev_offset = %x\n", 0,
                pre_os);
#endif

            if (pre_os)
            {
                char* pre_add = TO_ADD(pre_os);
                seg_set_successor(pre_add, 0);
                *p_ptr = pre_add;
            }
            else
                *p_ptr = NULL;

            devide_blk(bp, asize);
            return (void*)bp;
        }
    }

#ifdef DEBUG_PRINT
    printf("    Asking for red-black tree block\n");
#endif

    node_t* fit_node = find_fit_rbtn(heap_rbrp->rb_node, asize);

#ifdef DEBUG_PRINT
    printf("    Search result: address = %lx\n", (size_t)fit_node);
#endif

    if (!fit_node)
    {

#ifdef DEBUG_PRINT
        printf("    Fail to find available block. Go to create_blk\n");
#endif

        if (!(bp = create_blk(asize)))
            return NULL;

#ifdef DEBUG_PRINT
        printf("    Return block  address = %lx, "
               "size = %lu\n",
            (size_t)bp, GET_SIZE(HDRP(bp)));
#endif

        return (void*)bp;
    }

#ifdef DEBUG_PRINT
    printf("    Found available red-black tree block  address = %lx, "
           "size = %lu\n",
        (size_t)fit_node, fit_node->rb_size);
#endif

    rb_delete_node(fit_node);
    bp = (char*)fit_node;
    devide_blk(bp, asize);

#ifdef DEBUG_PRINT
    printf("    Return block  address = %lx, "
           "size = %lu\n",
        (size_t)bp, GET_SIZE(HDRP(bp)));
#endif

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

#ifdef DEBUG_PRINT
    printf("\n\nFree  given address = %lx, adjusted size = %lu\n", (size_t)ptr,
        asize);
    printf("Current heap bottom = %lx\n", (size_t)mem_heap_lo());
    printf("Current heap top = %lx\n", (size_t)mem_heap_hi());
#endif

    coalesce_blk((char*)ptr, asize);
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

    asize = ALIGN(asize);

    if (old_ptr == NULL)
        return malloc(asize);

    old_size = GET_SIZE(HDRP(old_ptr));

#ifdef DEBUG_PRINT
    printf("\n\nRealloc  given address = %lx, old_size = %lu, requisite size = "
           "%lu\n",
        (size_t)old_ptr, old_size, asize);
    printf("Current heap bottom = %lx\n", (size_t)mem_heap_lo());
    printf("Current heap top = %lx\n", (size_t)mem_heap_hi());
#endif

    if (asize <= old_size)
    {

#ifdef DEBUG_PRINT
        printf("    Not to change block\n");
#endif
        devide_blk(old_ptr, asize);
        new_ptr = old_ptr;
    }
    else
    {

#ifdef DEBUG_PRINT
        printf("    Looking for new block\n");
#endif

        if (!(new_ptr = malloc(asize)))
            return NULL;

        memcpy(new_ptr, old_ptr, old_size);

        free(old_ptr);
    }

#ifdef DEBUG_PRINT
    printf("    Return block  address = %lx, "
           "size = %lu\n",
        (size_t)new_ptr, GET_SIZE(HDRP(new_ptr)));
#endif

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
    void* new_ptr;

#ifdef DEBUG_PRINT
    printf("\n\nCalloc  requisite size = %lu\n", bytes);
    printf("Current heap bottom = %lx\n", (size_t)mem_heap_lo());
    printf("Current heap top = %lx\n", (size_t)mem_heap_hi());
#endif

    new_ptr = malloc(bytes);
    if (new_ptr)
        memset(new_ptr, 0, bytes);

#ifdef DEBUG_PRINT
    printf("    Return block  address = %lx, "
           "size = %lu\n",
        (size_t)new_ptr, GET_SIZE(HDRP(new_ptr)));
#endif

    return new_ptr;
}

static char* create_blk(size_t asize)
{

#ifdef DEBUG_PRINT
    printf("        Creating block  requisite size = %lu\n", asize);
#endif

    char* bp;
    if ((void*)(bp = mem_sbrk(asize + DSIZE)) == (void*)-1)
    {

#ifdef DEBUG_PRINT
        printf("!!!mem_sbrk failed\n");
#endif

        return NULL;
    }

#ifdef DEBUG_PRINT
    printf("        Creating successfully  address = %lx, adjusted size = "
           "%lu\n",
        (size_t)bp, asize);
#endif

    PUT(HDRP(bp), PACK(asize, 0, ALLOCED));
    PUT(bp + asize, PACK(asize, 0, ALLOCED));
    PUT(bp + asize + WSIZE, PACK(0, 0, ALLOCED));

#ifdef DEBUG_PRINT
    printf("        Set new block  header = %u, footer = %u, default = %u\n",
        GET(HDRP(bp)), GET(bp + asize), GET(bp + asize + WSIZE));
#endif

    return bp;
}

static void place_blk(char* bp, size_t asize)
{
    assert(asize >= DSIZE);

#ifdef DEBUG_PRINT
    printf("        Placing block  address = %lx, size = %lu\n", (size_t)bp,
        asize);
#endif

    if (asize > MAXSEGBLKSIZE)
    {

#ifdef DEBUG_PRINT
        printf("        Block type = rbtn_blk\n");
#endif

        PUT(HDRP(bp), PACK(asize, TYPE_RBTN, UNALLOCED));
        PUT(bp + asize, PACK(asize, TYPE_RBTN, UNALLOCED));
        if (!heap_rbrp->rb_node)
        {
            heap_rbrp->rb_node = (node_t*)bp;
            node_t* root = (node_t*)bp;
            root->rb_color = RB_BLACK;
            root->rb_size = asize;
            root->rb_parent = root->rb_lChild = root->rb_rChild = root->rb_succ
                = root->rb_prev = NULL;

#ifdef DEBUG_PRINT
            printf("        Initialize red-black tree root  address = %lx, "
                   "size = %lu\n",
                (size_t)heap_rbrp->rb_node, heap_rbrp->rb_node->rb_size);
#endif
        }
        else
        {

#ifdef DEBUG_PRINT
            printf("            Insert rbtn_blk  insert_address = %lx, "
                   "insert_size "
                   "= %lu\n",
                (size_t)bp, asize);
#endif

            rb_insert_node(heap_rbrp->rb_node, (node_t*)(bp), asize);
        }
    }
    else
    {

#ifdef DEBUG_PRINT
        printf("        Block type = seg_blk\n");
#endif

        PUT(HDRP(bp), PACK(asize, TYPE_SEGB, UNALLOCED));
        PUT(bp + asize, PACK(asize, TYPE_SEGB, UNALLOCED));
        char** p_ptr = heap_segbp + asize / DSIZE - 1;
        char* tmp_p = *p_ptr;
        seg_set_successor(bp, 0);
        if (tmp_p)
        {
            seg_set_successor(tmp_p, TO_OFFSET(bp));
            seg_set_prev(bp, TO_OFFSET(tmp_p));
        }
        else
            seg_set_prev(bp, 0);

        *p_ptr = bp;
    }
}

static void devide_blk(char* bp, size_t asize)
{
    size_t old_size = GET_SIZE(HDRP(bp));
    size_t rest_size = old_size - asize;

    if (rest_size < MINBLK_WITHINFO_SIZE)
    {

#ifdef DEBUG_PRINT
        printf("        Not to devide block\n");
#endif

        PUT(HDRP(bp), PACK(old_size, 0, ALLOCED));
        PUT(bp + old_size, PACK(old_size, 0, ALLOCED));
    }
    else
    {

#ifdef DEBUG_PRINT
        printf("        Deviding block  asize = %lu, bsize = %lu\n", asize,
            rest_size - DSIZE);
#endif

        PUT(HDRP(bp), PACK(asize, 0, ALLOCED));
        PUT(bp + asize, PACK(asize, 0, ALLOCED));

        place_blk(bp + asize + DSIZE, rest_size - DSIZE);
    }
}

static void coalesce_blk(char* bp, size_t asize)
{
    unsigned int prev_info = GET(bp - DSIZE),
                 next_info = GET(bp + asize + WSIZE);

#ifdef DEBUG_PRINT
    printf("        Decide to coalesce or not  cur_address = %lx, "
           "cur_size = %lu\n",
        (size_t)bp, asize);
    printf("        prev_info = %u, next_info = %u\n", prev_info, next_info);
    if (!IS_UNALLOCED(next_info) && !IS_UNALLOCED(prev_info))
        printf("        Not to coalesce block\n");
#endif

    if (IS_UNALLOCED(next_info))
    {
        char* next_ptr = bp + asize + DSIZE;

        asize += READ_SIZE(next_info) + DSIZE;

#ifdef DEBUG_PRINT
        printf("        Coalescing block with next  next_address = %lx, "
               "next_size = %lu\n",
            (size_t)next_ptr, READ_SIZE(next_info));
        printf("        cur_size changed to %lu\n", asize);
#endif

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

#ifdef DEBUG_PRINT
        printf("        Coalescing block with prev  prev_address = %lx, "
               "prev_size = %lu\n",
            (size_t)bp, prev_size);
        printf("        cur_address changed to %lx, cur_size changed to %lu\n",
            (size_t)bp, asize);
#endif

        if (IS_SEGB(prev_info))
            seg_delete_blk(bp);
        else
            rb_delete_node((node_t*)bp);
    }

    place_blk(bp, asize);
}

static void seg_delete_blk(char* bp)
{
    size_t asize = GET_SIZE(HDRP(bp));

    offset_t cur_suc_os = seg_successor_offset(bp),
             cur_pre_os = seg_prev_offset(bp);

#ifdef DEBUG_PRINT
    printf("            Deleting seg_blk  address = %lx, size = %lu\n",
        (size_t)bp, asize);
    printf("            successor_offset = %x, prev_offset = %x\n", cur_suc_os,
        cur_pre_os);
#endif

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
        assert(*p_ptr == bp);

        if (cur_pre_os)
        {
            char* pre_add = TO_ADD(cur_pre_os);
            seg_set_successor(pre_add, 0);
            *p_ptr = pre_add;
        }
        else
            *p_ptr = NULL;
    }
}

static node_t* find_fit_rbtn(node_t* cur_node, size_t asize)
{

#ifdef DEBUG_PRINT
    printf("            Looking for suitable block in red-black tree\n");
    printf("            requisite size = %lu, cur_address = %lx\n", asize,
        (size_t)cur_node);
#endif

    if (!cur_node)
        return NULL;

    size_t cur_size = cur_node->rb_size;

#ifdef DEBUG_PRINT
    printf("            cur_size = %lu,cur_color = %s\n", cur_size,
        (cur_node->rb_color == RB_RED) ? "RED" : "BLACK");
#endif

    if (asize == cur_size)
    {

#ifdef DEBUG_PRINT
        printf("            Return current node  address = %lx\n",
            (size_t)cur_node);
#endif

        return cur_node;
    }

    if (asize > cur_size)
    {

#ifdef DEBUG_PRINT
        printf("            Go to right child\n");
#endif

        return find_fit_rbtn(cur_node->rb_rChild, asize);
    }

    node_t* tmp_node;

#ifdef DEBUG_PRINT
    printf("            Go to left child\n");
#endif

    if ((tmp_node = find_fit_rbtn(cur_node->rb_lChild, asize)))
        return tmp_node;

#ifdef DEBUG_PRINT
    printf("            Fail to find smaller node, return current node\n");
    printf("            Return node  address = %lx\n", (size_t)cur_node);

#endif

    return cur_node;
}

static void rb_left_rotate(node_t* node)
{
    node_t* parent = node->rb_parent;
    node_t* right = node->rb_rChild;

    /* Set the relationship between node's right child's left child and node */
    if ((node->rb_rChild = right->rb_lChild))
        right->rb_lChild->rb_parent = node;

    /* Set the relationship between node and the new node */
    right->rb_lChild = node;
    node->rb_parent = right;

    /* Set the relationship between the new old and the old parent */
    right->rb_parent = parent;
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
    node_t* parent = node->rb_parent;
    node_t* left = node->rb_lChild;

    /* Set the relationship between node's left child's right child and node */
    if ((node->rb_lChild = left->rb_rChild))
        left->rb_rChild->rb_parent = node;

    /* Set the relationship between node and the new node */
    left->rb_rChild = node;
    node->rb_parent = left;

    /* Set the relationship between the new old and the old parent */
    left->rb_parent = parent;
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

static void rb_insert_adjust(node_t* node)
{
    node_t *parent, *gparent;

    /* Adjust position and color while color of the node's parent is red */
    while ((parent = node->rb_parent) && parent->rb_color == RB_RED)
    {
        /* gparent won't be NULL, because color of parent is red
             * which means parent isn't root node */
        gparent = parent->rb_parent;
        assert(gparent);

#ifdef DEBUG_PRINT
        printf("                Adjust after inserting\n");
#endif

        if (parent == gparent->rb_lChild)
        {

            /* Case 3: If uncle exists and its color is red,
                 * change color of gparent/parent/uncle */
            {
                node_t* uncle;
                if ((uncle = gparent->rb_rChild) && uncle->rb_color == RB_RED)
                {

#ifdef DEBUG_PRINT
                    printf("                Case 3: gparent is black while "
                           "parent and uncle are red\n");
                    printf("                Change color of "
                           "gparent/parent/uncle\n");
                    printf("                gparent_address = %lx, "
                           "parent_address = %lx, "
                           "uncle_address = %lx\n",
                        (size_t)gparent, (size_t)parent, (size_t)uncle);
#endif

                    gparent->rb_color = RB_RED;
                    parent->rb_color = RB_BLACK;
                    uncle->rb_color = RB_BLACK;
                    node = gparent;
                    continue;
                }
            }

            /* Case 4: Color of uncle  is black
                 * and node is parent's right child (LR) */
            if (node == parent->rb_rChild)
            {

#ifdef DEBUG_PRINT
                printf(
                    "                Case 4: gparent and uncle are black while "
                    "parent is red (LR)\n");
                printf("                left_rotate parent\n");
                printf("                gparent_address = %lx, parent_address "
                       "= %lx\n",
                    (size_t)gparent, (size_t)parent);
#endif

                /* Left rotate from parent and change the situation
                     * in to case 5 */
                rb_left_rotate(parent);

                /* Exchange node and parent to make they get
                     * correct meanings */
                node_t* tmp_node = parent;
                parent = node;
                node = tmp_node;
            }

#ifdef DEBUG_PRINT
            printf("                Case 5: gparent and uncle are black while "
                   "parent is red (LL)\n");
            printf("                Change color of gparent and parent,then "
                   "right_rotate gparent\n");
            printf(
                "                gparent_address = %lx, parent_address = %lx\n",
                (size_t)gparent, (size_t)parent);
#endif

            /* Case 5: LL with red parent,
                 * exchange colors of parent and gparent
                 * and right rotate gparent */

            gparent->rb_color = RB_RED;
            parent->rb_color = RB_BLACK;
            rb_right_rotate(gparent);
        }
        /* Mirrored situation */
        else
        {
            /* Case 3: If uncle exists and its color is red,
                 * change color of gparent/parent/uncle */
            {
                node_t* uncle;
                if ((uncle = gparent->rb_lChild) && uncle->rb_color == RB_RED)
                {

#ifdef DEBUG_PRINT
                    printf("                Case 3: gparent is black while "
                           "parent and uncle are red\n");
                    printf("                Change color of "
                           "gparent/parent/uncle\n");
                    printf("                gparent_address = %lx, "
                           "parent_address = %lx, "
                           "uncle_address = %lx\n",
                        (size_t)gparent, (size_t)parent, (size_t)uncle);
#endif

                    gparent->rb_color = RB_RED;
                    parent->rb_color = RB_BLACK;
                    uncle->rb_color = RB_BLACK;
                    node = gparent;
                    continue;
                }
            }

            /* Case 4: Color of uncle is black
                 * and node is parent's left child (RL) */
            if (node == parent->rb_lChild)
            {

#ifdef DEBUG_PRINT
                printf(
                    "                Case 4: gparent and uncle are black while "
                    "parent is red (RL)\n");
                printf("                right_rotate parent\n");
                printf("                gparent_address = %lx, parent_address "
                       "= %lx\n",
                    (size_t)gparent, (size_t)parent);
#endif
                /* Right rotate from parent and change the
                     * situation in to case 5 */
                rb_right_rotate(parent);

                /* Exchange node and parent to make they get
                     * correct meanings */
                node_t* tmp_node = parent;
                parent = node;
                node = tmp_node;
            }

#ifdef DEBUG_PRINT
            printf("                Case 5: gparent and uncle are black while "
                   "parent is red (RR)\n");
            printf("                Change color of gparent and parent,then "
                   "left_rotate gparent\n");
            printf(
                "                gparent_address = %lx, parent_address = %lx\n",
                (size_t)gparent, (size_t)parent);
#endif

            /* Case 5: RR with red parent,
                 * exchange colors of parent and gparent
                 * and left rotate gparent */
            gparent->rb_color = RB_RED;
            parent->rb_color = RB_BLACK;
            rb_left_rotate(gparent);
        }
    }
    heap_rbrp->rb_node->rb_color = RB_BLACK;
}

static void rb_insert_node(node_t* cur_node, node_t* node, size_t asize)
{
    size_t cur_size = cur_node->rb_size;

#ifdef DEBUG_PRINT
    printf("            cur_address = %lx, cur_size = %lu\n", (size_t)cur_node,
        cur_size);
#endif

    if (asize == cur_size)
    {

#ifdef DEBUG_PRINT
        printf("            Set insert_node as successor of cur_node\n");
#endif
        node->rb_color = cur_node->rb_color;
        node->rb_size = cur_size;

        node_t *parent, *left, *right;
        if ((parent = cur_node->rb_parent))
        {
            if (cur_node == parent->rb_lChild)
                parent->rb_lChild = node;
            else
                parent->rb_rChild = node;
        }
        else
        {
            heap_rbrp->rb_node = node;

#ifdef DEBUG_PRINT
            printf("            Changing root  root address = %lx\n",
                (size_t)heap_rbrp->rb_node);
#endif
        }
        node->rb_parent = parent;

        if ((left = cur_node->rb_lChild))
            left->rb_parent = node;
        node->rb_lChild = left;

        if ((right = cur_node->rb_rChild))
            right->rb_parent = node;
        node->rb_rChild = right;

        node->rb_prev = cur_node;
        cur_node->rb_succ = node;
        node->rb_succ = NULL;
    }
    else if (asize < cur_size)
        if (cur_node->rb_lChild)
        {

#ifdef DEBUG_PRINT
            printf("            Go to left child\n");
#endif

            rb_insert_node(cur_node->rb_lChild, node, asize);
        }
        else
        {

#ifdef DEBUG_PRINT
            printf("            Make insert_node be left child of the node\n");
#endif

            node->rb_color = RB_RED;
            node->rb_size = asize;
            node->rb_parent = cur_node;
            node->rb_lChild = node->rb_rChild = node->rb_succ = node->rb_prev
                = NULL;
            cur_node->rb_lChild = node;

            rb_insert_adjust(node);
        }
    else if (cur_node->rb_rChild)
    {

#ifdef DEBUG_PRINT
        printf("            Go to right child\n");
#endif

        rb_insert_node(cur_node->rb_rChild, node, asize);
    }
    else
    {

#ifdef DEBUG_PRINT
        printf("            Make insert_node be right child of the node\n");
#endif

        node->rb_color = RB_RED;
        node->rb_size = asize;
        node->rb_parent = cur_node;
        node->rb_lChild = node->rb_rChild = node->rb_succ = node->rb_prev
            = NULL;
        cur_node->rb_rChild = node;

        rb_insert_adjust(node);
    }
}

static void rb_delete_adjust(node_t* node, node_t* parent)
{
    node_t* sibling;

    while ((!node || node->rb_color == RB_BLACK) && node != heap_rbrp->rb_node)
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
            if (sibling->rb_color == RB_RED)
            {
                sibling->rb_color = RB_BLACK;
                parent->rb_color = RB_RED;
                rb_left_rotate(parent);
                sibling = parent->rb_rChild;
            }

            /* Case 3 & Case 4: Sibling's children are all black,
                 * Set sibling red and adjust from parent */
            if ((!sibling->rb_lChild
                    || sibling->rb_lChild->rb_color == RB_BLACK)
                && (!sibling->rb_rChild
                       || sibling->rb_rChild->rb_color == RB_BLACK))
            {
                sibling->rb_color = RB_RED;
                node = parent;
                parent = node->rb_parent;
            }
            else
            {
                /* Case 5: Sibling's left child is red,
                     * exchange colors of sibling and its left child,
                     * and then right rotate to make it Case 6 */
                if (!sibling->rb_rChild
                    || sibling->rb_rChild->rb_color == RB_BLACK)
                {
                    node_t* sibling_left;
                    if ((sibling_left = sibling->rb_lChild))
                        sibling_left->rb_color = RB_BLACK;
                    sibling->rb_color = RB_RED;
                    rb_right_rotate(sibling);
                    sibling = parent->rb_rChild;
                }

                /* Case 6: Sibling's right child is red,
                     * exchange colors of sibling and parent,
                     * set parent and sibling's right child black,
                     * left rotate and then stop sdjusting */
                sibling->rb_color = parent->rb_color;
                parent->rb_color = RB_BLACK;

                if (sibling->rb_rChild)
                    sibling->rb_rChild->rb_color = RB_BLACK;

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
            if (sibling->rb_color == RB_RED)
            {
                sibling->rb_color = RB_BLACK;
                parent->rb_color = RB_RED;
                rb_right_rotate(parent);
                sibling = parent->rb_lChild;
            }

            /* Case 3 & Case 4: Sibling's children are all black,
                 * Set sibling red and adjust from parent */
            if ((!sibling->rb_lChild
                    || sibling->rb_lChild->rb_color == RB_BLACK)
                && (!sibling->rb_rChild
                       || sibling->rb_rChild->rb_color == RB_BLACK))
            {
                sibling->rb_color = RB_RED;
                node = parent;
                parent = node->rb_parent;
            }
            else
            {
                /* Case 5: Sibling's right child is red,
                     * exchange colors of sibling and its right child,
                     *continue and then left rotate to make it Case 6
                     */
                if (!sibling->rb_lChild
                    || sibling->rb_lChild->rb_color == RB_BLACK)
                {
                    node_t* sibling_right;
                    if ((sibling_right = sibling->rb_rChild))
                        sibling_right->rb_color = RB_BLACK;
                    sibling->rb_color = RB_RED;
                    rb_left_rotate(sibling);
                    sibling = parent->rb_lChild;
                }

                /* Case 6: Sibling's left child is red,
                     * exchange colors of sibling and parent,
                     * set parent and sibling's left child black,
                     * right rotate and then stop sdjusting */
                sibling->rb_color = parent->rb_color;
                parent->rb_color = RB_BLACK;

                if (sibling->rb_lChild)
                    sibling->rb_lChild->rb_color = RB_BLACK;

                rb_right_rotate(parent);

                node = heap_rbrp->rb_node;
            }
        }
    }

    if (node)
        node->rb_color = RB_BLACK;
}

static void rb_delete_node(node_t* node)
{

#ifdef DEBUG_PRINT
    printf("            Deleting rbtn_blk  address = %lx, size = %lu, color = "
           "%s\n",
        (size_t)node, node->rb_size,
        (node->rb_color == RB_RED) ? "RED" : "BLACK");
#endif

    node_t *pre_node = node->rb_prev, *suc_node = node->rb_succ;
    if (pre_node || suc_node)
    {

#ifdef DEBUG_PRINT
        printf("            The block isn't the only block in this node\n");
#endif

        if (suc_node)
            suc_node->rb_prev = pre_node;
        else
        {

#ifdef DEBUG_PRINT
            printf("            The block is the last block in the node\n");
            printf("            Change the tail of this node, prev_address = "
                   "%lx\n",
                (size_t)pre_node);
#endif

            node_t *parent = node->rb_parent, *left = node->rb_lChild,
                   *right = node->rb_rChild;

            pre_node->rb_color = node->rb_color;
            pre_node->rb_size = node->rb_size;
            pre_node->rb_parent = parent;
            if (parent)
            {
                if (node == parent->rb_lChild)
                    parent->rb_lChild = pre_node;
                else
                    parent->rb_rChild = pre_node;
            }
            else
            {
                heap_rbrp->rb_node = pre_node;

#ifdef DEBUG_PRINT
                printf("            Changing root  root address = %lx\n",
                    (size_t)heap_rbrp->rb_node);
#endif
            }
            pre_node->rb_lChild = left;
            if (left)
                left->rb_parent = pre_node;
            pre_node->rb_rChild = right;
            if (right)
                right->rb_parent = pre_node;
            if (heap_rbrp->rb_node == node)
                heap_rbrp->rb_node = pre_node;
        }

        if (pre_node)
            pre_node->rb_succ = suc_node;
        return;
    }

#ifdef DEBUG_PRINT
    printf("            The block is the only block in this node, delete the "
           "node\n");
#endif

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
        node_t* old_parent = old->rb_parent;
        node_t* old_left = old->rb_lChild;

        /* Look for successor of node */
        node = node->rb_rChild;
        while ((tmp = node->rb_lChild))
            node = tmp;

        /* Save successor's information */
        child = node->rb_rChild;
        parent = node->rb_parent;
        color = node->rb_color;

#ifdef DEBUG_PRINT
        printf("            delete_node has less than two children\n");
        printf("            Successor of the node  address = %lx, color = %s\n",
            (size_t)node, (color == RB_RED) ? "RED" : "BLACK");
#endif

        /* Set the relationship between successor's right child
             * and its parent */
        if (child)
            child->rb_parent = parent;
        if (node == parent->rb_lChild)
            parent->rb_lChild = child;
        else
            parent->rb_rChild = child;

        /* If successor is right child of the node to be deleted,
             * make sure the relationship between successor and its child
             * being set correctly */
        if (node->rb_parent == old)
            parent = node;

        /* Exchange the position of node to be deleted and successor */
        node->rb_parent = old_parent;
        node->rb_color = old->rb_color;
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
        {
            heap_rbrp->rb_node = node;

#ifdef DEBUG_PRINT
            printf("            Changing root  root address = %lx\n",
                (size_t)heap_rbrp->rb_node);
#endif
        }

        old_left->rb_parent = node;
        if (old->rb_rChild)
            old->rb_rChild->rb_parent = node;

        /* If the color of moved node is black,
             * the rb_tree need further adjustment */
        if (color == RB_BLACK)
            rb_delete_adjust(child, parent);
        return;
    }
    parent = node->rb_parent;
    color = node->rb_color;

#ifdef DEBUG_PRINT
    printf("            delete_node has less than two children\n");

#endif

    /* Set the relationship between successor's child
         * and its parent if necessary */
    if (child)
    {
        child->rb_parent = parent;

#ifdef DEBUG_PRINT
        printf("            Successor of the node  address = %lx, color = "
               "%s\n",
            (size_t)child, (child->rb_color == RB_RED) ? "RED" : "BLACK");
#endif
    }

#ifdef DEBUG_PRINT
    else
        printf("            delete_node has no child\n");
#endif

    if (parent)
    {
        if (node == parent->rb_lChild)
            parent->rb_lChild = child;
        else
            parent->rb_rChild = child;
    }
    else
    {

        heap_rbrp->rb_node = child;

#ifdef DEBUG_PRINT
        printf("            Changing root  root address = %lx\n",
            (size_t)heap_rbrp->rb_node);
#endif
    }

    /* If the color of moved node is black,
         * the rb_tree need further adjustment */
    if (color == RB_BLACK)
        rb_delete_adjust(child, parent);
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

#define CHK_IS_ALLOCED(bp) ((GET((bp)) & 0x1) == ALLOCED)
#define CHK_IS_UNALLOCED(bp) ((GET((bp)) & 0x1) == UNALLOCED)
#define CHK_IS_MARKED(bp) (GET((bp)) & 0x4)
#define CHK_IS_UNMARKED(bp) ((~GET((bp))) & 0x4)
inline void CHK_REVERSE_MARK(bp)
{
    unsigned int info = GET(BP);
    PUT(bp, (info ^ 0x4));
}

void seg_check_blocklist(char* bp, int asize)
{

    assert(asize >= MINBLKSIZE && asize <= MAXSEGBLKSIZE);

    char* pre_ptr = bp;
    offset_t cur_pre_os;

    while (bp = pre_ptr)
    {
        assert(in_heap(HDRP(bp)));
        assert(in_heap(bp + asize + WSIZE));

        assert(asize == GET_SIZE(HDRP(bp));
        assert(asize == GET_SIZE(bp + asize));

        assert(CHK_IS_UNALLOCED(HDRP(bp)));
        assert(CHK_IS_UNALLOCED(bp + asize));

        assert(IS_SEGB(GET(HDRP(bp))));
        assert(IS_SEGB(GET(bp + asize)));

        CHK_REVERSE_MARK(HDRP(bp));
        CHK_REVERSE_MARK(bp + asize);
        
        if((cur_pre_os = seg_set_prev(bp)))
        {
            pre_ptr = TO_ADD(cur_pre_os);
            assert(bp == TO_ADD(seg_prev_offset(pre_ptr)));    
        }
        else
            pre_ptr = NULL;
    }
}

void rb_check_blocklist(node_t* node, int asize)
{
    asize(asize > MAXSEGBLKSIZE);

    node_t* pre_node = node;

    while (node = pre_node)
    {
        assert(in_heap(HDRP(bp)));
        assert(in_heap(bp + asize + WSIZE));

        assert(asize == GET_SIZE(HDRP(bp));
        assert(asize == GET_SIZE(bp + asize));

        assert(CHK_IS_UNALLOCED(HDRP(bp)));
        assert(CHK_IS_UNALLOCED(bp + asize));

        assert(IS_RBTN(GET(HDRP(bp))));
        assert(IS_RBTN(GET(bp + asize)));

        CHK_REVERSE_MARK(HDRP(bp));
        CHK_REVERSE_MARK(bp + asize);
        
        if(pre_node = node->rb_prev)
            assert(node == pre_node->rb_succ);
    }
}

size_t rb_check_node(node_t* node)
{
    rb_check_blocklist(node, node->rb_size);

    node_t *left, *right;
    size_t left_blackheight = 1, right_blackheight = 1;
    if (left = node->rb_lChild)
    {
        left_blackheight = rb_check_node(left);
        assert(left->rb_size < node->rb_size);
        assert(left->parent == node);
        assert(!(node->rb_color == RB_RED && left->rb_color == RB_RED));
    }
    if (right = node->rb_rChild)
    {
        right_blackheight = rb_check_node(right);
        assert(left->rb_size > node->rb_size);
        assert(right->parent == node);
        assert(!(node->rb_color == RB_RED && right->rb_color == RB_RED));
    }

    assert(left_blackheight == right_blackheight);
    return left_blackheight;
}

/*
     * mm_checkheap
     */
void mm_checkheap(int lineno)
{
    size_t asize = MINBLKSIZE;
    char** p_ptr = heap_segbp;
    char* tmp_p;
    while (p_ptr != heap_edsegbp)
    {
        if (tmp_p = *p_ptr)
            seg_check_blocklist(tmp_p, asize);
        p_ptr++;
        asize += 8;
    }

    rb_check_node(heap_rbrp->rb_node);

    char* bp = heap_blk_st,next_ptr;
    while(in_heap(bp))
    {
        asize = GET_SIZE(HDRP(bp));
        next_ptr = bp + asize + DSIZE;
        if(CHK_IS_UNALLOCED(HDRP(bp)))
        {
            assert(CHK_IS_UNALLOCED(bp + asize));
            assert(CHK_IS_MARKED(HDRP(bp)));
            assert(CHK_IS_MARKED(bp + asize));
            if(in_heap(next_ptr))
                assert(CHK_IS_ALLOCED(HDRP(next_ptr)));
        }
        else
        {
            assert(CHK_IS_ALLOCED(bp + asize));
            assert(GET_SIZE(bp + asize) == asize);
            assert(CHK_IS_UNMARKED(HDRP(bp)));
            assert(CHK_IS_UNMARKED(bp + asize));
        }
        bp = next_ptr;
    }
}
