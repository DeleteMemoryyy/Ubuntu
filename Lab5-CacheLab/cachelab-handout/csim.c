/*    Liu DeXin    1500017704@pku.edu.cn 
*
*    Cache with LRU replace strategy simulator
*    
*    Use struct to achieve the funcional of set in cache.
*
*/

#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include "cachelab.h"

/* Name the type of pointer in 64-bits machine. */
typedef unsigned long long Pointer_64;

Pointer_64 bMask = 0, sMask = 0, tMask = 0;
int hit_count = 0, miss_count = 0, eviction_count = 0;
int opt = 0, s = 0, sNumber = 0, E = 0, b = 0, bNumber = 0;
char *fileName;

typedef struct SET
{
    int *valid, *callTime;
    Pointer_64 *tag;
} TSet;

/* Initialize set, allot memory space for every element. */
void initialize(TSet *set)
{
    int lI = sizeof(int), lP = sizeof(Pointer_64);
    set->valid = NULL;
    if (set->valid == NULL)
    {
	set->valid = (int *)malloc(lI * E);
    }
    set->callTime = NULL;
    if (set->callTime == NULL)
    {
	set->callTime = (int *)malloc(lI * E);
    }
    set->tag = NULL;
    if (set->tag == NULL)
    {
	set->tag = (Pointer_64 *)malloc(lP * E);
    }
    if (set->valid != NULL && set->callTime != NULL && set->tag != NULL)
    {
	memset(set->valid, 0, lI * E);
	memset(set->callTime, 0, lI * E);
	memset(set->tag, 0, lP * E);
    }
}

/* Destruct set, free the space. */
void destruct(TSet *set)
{
    if (set->valid != NULL)
	free(set->valid);
    if (set->callTime != NULL)
	free(set->callTime);
    if (set->tag != NULL)
	free(set->tag);
}

/* Detect if the target is in cache. */
int exist(TSet *set, Pointer_64 iTag, int time)
{
    for (int i = 0; i < E; ++i)
	if (set->valid[i] == 1 && set->tag[i] == iTag)
	{
	    set->callTime[i] = time;
	    return 1;
	}
    return 0;
}

/* If misses, choose which line to replace. */
int replace(TSet *set, Pointer_64 iTag, int time)
{
    int tMin = (1 << 15), ti = 0, i = 0;
    while (i < E)
    {
	if (set->valid[i] == 0)
	{
	    set->valid[i] = 1;
	    set->tag[i] = iTag;
	    set->callTime[i] = time;
	    return 0;
	}
	if (set->callTime[i] < tMin)
	{
	    ti = i;
	    tMin = set->callTime[i];
	}
	++i;
    }
    set->tag[ti] = iTag;
    set->callTime[ti] = time;
    return 1;
}

int main(int argc, char *argv[])
{
    opterr = 0;

    /* Use the getopt function in library to set the parameters s/E/b/t. */
    while ((opt = getopt(argc, argv, "s:E:b:t:")) != -1)
    {
	switch (opt)
	{
	case 's':
	    s = atoi(optarg);
	    sNumber = (1 << s);
	    break;
	case 'E':
	    E = atoi(optarg);
	    break;
	case 'b':
	    b = atoi(optarg);
	    bNumber = (1 << b);
	case 't':
	    fileName = optarg;
	    break;
	default:
	    break;
	}
    }

    /* Check input's legitimacy. */
    if (s < 0 || E < 0 || b < 0 || fileName == NULL)
    {
	return 0;
    }

    /* Calculate the mask numbers. */
    bMask = ((1u << b) - 1u);
    sMask = (((1u << s) - 1u) << b);
    tMask = (Pointer_64)(-1) - bMask - sMask;

    /* Redirect the stdin stream to get the contents in the trace files. */
    freopen(fileName, "r", stdin);

    /* Allot space for sets. */
    TSet *set = NULL;
    if (set == NULL)
    {
	set = (TSet *)malloc(sNumber * sizeof(TSet));
    }
    if (set == NULL)
    {
	return 0;
    }
    for (int i = 0; i < sNumber; ++i)
	initialize(set + i);

    Pointer_64 pInput, iTag;
    int iSet, existence, time = 0;
    char cInput, mode;

    /* Read the file in cycling. */
    while ((cInput = getchar()) != EOF)
    {
	time++;
	if (cInput == ' ')
	{
	    scanf("%c %llx,%*d", &mode, &pInput);

	    /* Get the number of set and the tag of every address. */
	    iTag = pInput & tMask;
	    iSet = ((pInput & sMask) >> b);

	    /* Check if this data is in cache. */
	    existence = exist(set + iSet, iTag, time);
	    /* If exists, hits + 1. */
	    if (existence == 1)
	    {
		++hit_count;
	    }
	    /* Else, decide to replace a line in cache. */
	    else
	    {
		++miss_count;
		if (replace(set + iSet, iTag, time) == 1)
		{
		    ++eviction_count;
		}
	    }
		
	    /* If the operator is Modify, 
             the second memory access must hit the cache. */
	    if (mode == 'M')
	    {
		++hit_count;
	    }
	    if (getchar() == EOF)
	    {
		break;
	    }
	}
	else
	{
	    /* If the operator is Istruction Load, discard this line. */
	    while ((cInput = getchar()) != EOF)
	    {
		if (cInput == '\n')
		    break;
	    }
	    if (cInput == EOF)
	    {
		break;
	    }
	}
    }

    /* Recycle memory space. */
    if (set != NULL)
    {
	for (int i = 0; i < sNumber; ++i)
	    destruct(set + i);
	free(set);
    }

    /* Print the answer. */
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}