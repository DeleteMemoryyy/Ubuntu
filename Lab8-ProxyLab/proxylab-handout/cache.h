#ifndef __CACHE_H__
#define __CACHE_H__

#include "csapp.h"

typedef struct Object_t
{
    char header[MAXLINE];
    size_t hash;
    char* content;
    int content_size;
    int dirty;
    struct Object_t *prev, *succ;
} Object;

typedef struct
{
    Object *head, *tail;
    int max_size, rest_size;
} Cache;

/* Global variables */
extern int readcnt;
extern sem_t mutex, w;
extern Cache cache;

size_t get_hash(char* str);
Object* search_object(char* header, size_t hash);
void update_object_position();
void push_back_object(Object* obj);
int delete_one_object(int req_size);
void delete_in_list(Object* obj);
int delete_objects_to_size(int req_size);
void update_max_size();
void insert_object(char* header, size_t hash, char* object_buf, int req_size);

#endif