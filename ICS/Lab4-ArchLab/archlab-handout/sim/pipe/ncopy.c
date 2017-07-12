#include <stdio.h>

int src[8], dst[8];

/* $begin ncopy */
/*
 * ncopy - copy src to dst, returning number of positive ints
 * contained in src array.
 */
int ncopy(int *src, int *dst, int len)
{
    int count = 0;
    int val;
    while(len >= 16)
    {
        val = *src++;   // 1
        *dst++ = val;
	    if (val > 0)
	        count++;
        val = *src++;   // 2
        *dst++ = val;
	    if (val > 0)
	        count++;
        val = *src++;   // 3
        *dst++ = val;
	    if (val > 0)
	        count++;
        val = *src++;   // 4
        *dst++ = val;
	    if (val > 0)
	        count++;
        val = *src++;   // 5
        *dst++ = val;
	    if (val > 0)
	        count++;
        val = *src++;   // 6
        *dst++ = val;
	    if (val > 0)
	        count++;
        val = *src++;   // 7
        *dst++ = val;
	    if (val > 0)
	        count++;
        val = *src++;   // 8
        *dst++ = val;
	    if (val > 0)
	        count++;
        val = *src++;
        *dst++ = val;   // 9
	    if (val > 0)
	        count++;
        val = *src++;   // 10
        *dst++ = val;
	    if (val > 0)
	        count++;
        val = *src++;   // 11
        *dst++ = val;
	    if (val > 0)
	        count++;
        val = *src++;   // 12
        *dst++ = val;
	    if (val > 0)
	        count++;
        val = *src++;   // 13
        *dst++ = val;
	    if (val > 0)
	        count++;
        val = *src++;   // 14
        *dst++ = val;
	    if (val > 0)
	        count++;
        val = *src++;   // 15
        *dst++ = val;
	    if (val > 0)
	        count++;
        val = *src++;   // 16
        *dst++ = val;
	    if (val > 0)
	        count++;

        len -= 16;
    }


    while (len > 0) {
	val = *src++;
	*dst++ = val;
	if (val > 0)
	    count++;
	len--;
    }
    return count;
}
/* $end ncopy */

int main()
{
    int i, count;

    for (i=0; i<8; i++)
	src[i]= i+1;
    count = ncopy(src, dst, 8);
    printf ("count=%d\n", count);
    exit(0);
}


