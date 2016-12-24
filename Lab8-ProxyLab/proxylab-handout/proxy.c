/*
 * Proxy Lab
 *
 * Liu DeXin 1500017704@pku.edu.cn
 *
 * Index of files:
 *
 * 	proxy.c 	Main program of proxy
 *
 *  request.c & request.h 	Programs to process every request, including
 * receive request from client and send it to server, then passing response from
 * server to client.
 *
 * 	cache.c & cache.h 	Cache with LRU strategy, realized with hash
 * number and linklist.
 *
 *
 *
 * 	Program are running in multiply-thread, with methods to aviod race in cache. 
 *
 *
 */

#include <stdio.h>

#include "cache.h"
#include "csapp.h"
#include "request.h"

int main(int argc, char** argv)
{

    /* Ignore SIGPIPE signal */
    Signal(SIGPIPE, SIG_IGN);
    Signal(SIGALRM, SIG_IGN);

    int listenfd, *connfdp;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;

    /* Check command line args */
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    /* Initialize cache and mutex */
    init_proxy();

    listenfd = Open_listenfd(argv[1]);

    while (1)
    {
        clientlen = sizeof(struct sockaddr_storage);
        connfdp = Malloc(sizeof(int));
        *connfdp = Accept(listenfd, (SA*)&clientaddr, &clientlen);
        Pthread_create(&tid, NULL, thread, connfdp);
    }

    Close(listenfd);

    /* Free cache */
    clear_cache();

    return 0;
}
