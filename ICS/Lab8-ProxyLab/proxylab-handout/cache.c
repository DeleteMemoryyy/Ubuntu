#include <stdio.h>

#include "cache.h"
#include "csapp.h"
#include "request.h"

/* Global variables */
int readcnt;
sem_t mutex, w;
Cache cache;

/* BSD hash for string */
size_t get_hash(char* str)
{
    size_t c, hash = 0;
    while ((c = *str))
    {
        hash = hash * 131 + c;
        str++;
    }
    return hash;
}

/* Search a object in cache */
Object* search_object(char* header, size_t hash)
{
    Object* cur_obj = cache.tail;
    while ((cur_obj))
    {
        if (hash == cur_obj->hash && !strcmp(header, cur_obj->header))
            break;
        cur_obj = cur_obj->prev;
    }

    if ((cur_obj))
        cur_obj->dirty = 1;

    return cur_obj;
}

/* Move modified objects to the tail of cache list */
void update_object_position()
{
    Object* obj = cache.head;

    while ((obj))
    {
        if (obj->dirty)
        {
            delete_in_list(obj);
            push_back_object(obj);
        }
        obj = obj->succ;
    }
}

/* Put the object at the tail of cache list */
inline void push_back_object(Object* obj)
{
    obj->dirty = 0;
    obj->succ = NULL;
    obj->prev = cache.tail;

    if (cache.tail)
        cache.tail->succ = obj;
    else
        cache.head = obj;

    cache.tail = obj;
}

/* Delete an object larger than requesting size */
int delete_one_object(int req_size)
{
    Object* obj = cache.head;

    while (obj && obj->content_size < req_size)
        obj = obj->succ;

    if (!obj)
        return -1;

    delete_in_list(obj);

    req_size = obj->content_size;

    free(obj->content);
    free(obj);

    return req_size;
}

/* Peel the object in list */
void delete_in_list(Object* obj)
{
    Object *obj_prev = obj->prev, *obj_succ = obj->succ;
    if ((obj_prev))
    {
        obj_prev->succ = obj_succ;
        if ((obj_succ))
            obj_succ->prev = obj_prev;
        else
            cache.tail = obj_prev;
    }
    else
    {
        cache.head = obj_succ;
        if ((obj_succ))
            obj_succ->prev = NULL;
        else
            cache.tail = NULL;
    }
}

/* Delete objects up to requesting size */
int delete_objects_to_size(int req_size)
{
    Object *cur_obj = cache.head, *obj_ed = cache.head, *delete_obj;
    int delete_size = 0;

    while ((obj_ed) && delete_size < req_size)
    {
        delete_size += obj_ed->content_size;
        obj_ed = obj_ed->succ;
    }

    if (delete_size < req_size)
        return -1;

    cache.head = obj_ed;
    if ((obj_ed))
        obj_ed->prev = NULL;
    else
        cache.tail = NULL;
    while (cur_obj != obj_ed)
    {
        delete_obj = cur_obj;
        cur_obj = cur_obj->succ;
        free(delete_obj->content);
        free(delete_obj);
    }

    return delete_size;
}

void update_max_size()
{
    int tmp = 0;
    Object* obj = cache.head;
    while (obj)
    {
        if (obj->content_size > tmp)
            tmp = obj->content_size;
        obj = obj->succ;
    }
    cache.max_size = tmp;
}

/* Insert a new object in cache
 *
 * First check if there's enough rest space for the object.
 * If so, put it at the tail of object list.
 *
 * If not, delete object in cache to get more space.
 */
void insert_object(char* header, size_t hash, char* object_buf, int req_size)
{
    Object* obj = Malloc(sizeof(Object));
    strcpy(obj->header, header);
    obj->hash = hash;
    obj->content = Malloc(req_size + 1);
    memset(obj->content, 0, req_size + 1);
    memcpy(obj->content, object_buf, req_size);
    obj->content_size = req_size;

    if (req_size > cache.rest_size)
    {
        if (req_size <= cache.max_size)
            cache.rest_size += delete_one_object(req_size);
        else
            cache.rest_size += delete_objects_to_size(req_size);
        update_max_size();
    }

    cache.rest_size -= req_size;
    push_back_object(obj);
    if (req_size > cache.max_size)
        cache.max_size = req_size;
}
