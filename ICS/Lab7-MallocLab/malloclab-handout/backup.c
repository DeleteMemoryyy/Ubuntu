static void rb_insert_node(node_t* cur_node, node_t* node, size_t asize)
{
    assert(cur_node);

    size_t cur_size = GET_SIZE(HDRP(cur_node));

#ifdef DEBUG
    printf("            cur_address = %lx, cur_size = %lu\n", (size_t)cur_node,
        cur_size);
#endif

    if (asize < cur_size)
        if (cur_node->rb_lChild)
        {

#ifdef DEBUG
            printf("            Go to left child\n");
#endif

            rb_insert_node(cur_node->rb_lChild, node, asize);
        }
        else
        {

#ifdef DEBUG
            printf("            Make insert_node be left child of the node\n");
#endif

            rb_link_node(node, cur_node, &(cur_node->rb_lChild));
            rb_insert_adjust(node);
        }
    else if (cur_node->rb_rChild)
    {

#ifdef DEBUG
        printf("            Go to right child\n");
#endif

        rb_insert_node(cur_node->rb_rChild, node, asize);
    }
    else
    {

#ifdef DEBUG
        printf("            Make insert_node be right child of the node\n");
#endif

        rb_link_node(node, cur_node, &(cur_node->rb_rChild));
        rb_insert_adjust(node);
    }
}


/* Functions of rb_tree */

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
        = ((node->rb_parent_color) & 0x7) | (long unsigned int)parent;
}

static inline void rb_set_color(node_t* node, int color)
{
    node->rb_parent_color = (node->rb_parent_color & ~0x7) | color;
}