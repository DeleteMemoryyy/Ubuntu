#ifndef __REQUEST_H__
#define __REQUEST_H__

/* Recommended max cache and obj sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

#define DEFAULT_HOST 80
#define PORT_SIZE 16

/* Decide to check method or not */
#define ONLY_GET
/* Decide to compare object with header line or whole request */
#define CACHE_WITH_HEADER

#define DEBUG
#ifdef DEBUG
#define dbg_printf(...) printf(__VA_ARGS__)
#else
#define dbg_printf(...)
#endif

typedef struct
{
    char host[MAXLINE];
    char header[MAXLINE];
    char content[MAXLINE];
    char port[16];
} Request;

void* thread(void* vargp);
void process_request(int connfd);
int parse_requsethdrs(rio_t* rp, int clientfd, Request* request);
int parse_uri(char* uri, Request* request, char* pathname);
int read_requesthdrs(rio_t* rp, Request* request);
int connect_with_server(int clientfd, Request* request, char* object_buf);
void clienterror(
    int fd, char* cause, char* errnum, char* shortmsg, char* longmsg);
void init_proxy();
void clear_cache();

#endif