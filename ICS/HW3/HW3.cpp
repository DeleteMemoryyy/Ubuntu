#include <stdio.h>

long loop(long x,long n)
 {
        long result = 0;
        long mask;
        for(mask = 1; mask != 0;mask = mask << n)
        {
            result |= x & mask;
        }
        return result;
    }

long cread(long *xp)
    {
        const static long z = 0;
        return *(xp?xp:&z);
    }

int main()
    {
        long *xp = 0;
        cread(xp);
        return 0;
    }