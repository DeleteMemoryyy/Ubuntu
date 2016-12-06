#include "csapp.h"

static sigjmp_buf env;

void handler_alrm(int sig)
{
    siglongjmp(env,1);
}

char *tfgets(char *buf, int bufsize, FILE *stream)
{
    signal(SIGALRM,handler_alrm);

    alarm(5);

    if(sigsetjmp(env,1) == 0)
    {
        return fgets(buf,bufsize,stream);
    }
    else
    {
        return NULL;
    }
}

int main()
{
    char buf[101];
    while(1)
    {
        if(tfgets(buf,sizeof(buf),stdin) != NULL)
        {
            printf("%s\n",buf);
        }
        else
        {
            printf("Time out.\n");
        }
    }
    return 0;
}