#include "csapp.h"
#include <stdio.h>

#define _GNU_SOURCE

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char* user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux "
                                    "x86_64; rv:10.0.3) Gecko/20120305 "
                                    "Firefox/10.0.3\r\n";
static const char* connection_hdr = "Connection: close\r\n";
static const char* proxy_connection_hdr = "Proxy-Connection: close\r\n";

#define DEFAULT_HOST 80

#define PORT_SIZE 32

typedef struct
{
    char host[MAXLINE];
    char content[MAXLINE];
    char port[16];
} Request;

typedef struct
{
    char header[MAXLINE];
    size_t hash;
    char* content;
    int content_size;
    Object *prev, *succ;
} Object;

typedef struct
{
    Object *head, *tail;
    int max_size, rest_size;
} Cache;

Cache cache;

#define DEBUG

#ifdef DEBUG
#define dbg_printf(...) printf(__VA_ARGS__)
#else
#define dbg_printf(...)
#endif

void* thread(void* vargp);
void process_request(int connfd);
int parse_requsethdrs(rio_t* rp, int clientfd, Request* request);
int parse_uri(char* uri, char* hostname, char* pathname, char* port);
int read_requesthdrs(rio_t* rp, Request* request);
int connect_with_server(int clientfd, Request* request, char* object_buf);
void clienterror(
    int fd, char* cause, char* errnum, char* shortmsg, char* longmsg);

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

    init_cache();

    listenfd = Open_listenfd(argv[1]);

    while (1)
    {
        clientlen = sizeof(struct sockaddr_storage);
        connfdp = Malloc(sizeof(int));
        *connfdp = Accept(listenfd, (SA*)&clientaddr, &clientlen);
        Pthread_create(&tid, NULL, thread, connfdp);
    }

    Close(listenfd);

    return 0;
}

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

void init_cahce()
{
    cache.head = NULL;
    cache.tail = NULL;
    cache.max_size = 0;
    cache.rest_size = MAX_CACHE_SIZE;
}

Object* search_object(char* header, size_t hash)
{
    Object* cur = cache.tail;
    while ((cur))
    {
        if (hash == cur->hash && !strcmp(header, cur->header))
        {
            delete_in_list(cur);
            push_back_object(cur);
            break;
        }
        cur = cur->prev;
    }

    return cur;
}

inline void push_back_object(Object* object)
{
    object->succ = NULL;
    object->prev = cache.tail;

    if (cache.tail)
        cache.tail->succ = object;
    else
        cache.head = object;
}

int delete_one_object(int requesting_size)
{
    Object* object = cache.head;

    while ((object) && object->content_size < requesting_size)
        object = object->succ;

    if (!object)
        return -1;

    delete_in_list(object);

    requesting_size = object->content_size;

    Free(object->content);
    Free(object);

    return requesting_size;
}

void delete_in_list(Object* object)
{
    Object *object_prev = object->prev, *object_succ = object->succ;
    if ((object_prev))
    {
        object_prev->succ = object_succ;
        if ((object_succ))
            object_succ->prev = object_prev;
        else
            cache->tail = object_prev;
    }
    else
    {
        cache->head = object_succ;
        if ((object_succ))
            object_succ->prev = NULL;
        else
            cache->tail = NULL;
    }
}

int delete_objects_to_size(int requesting_size)
{
    Object *cur_object = cache.head, *object_ed = cache.head, *delete_object;
    int delete_size = 0;

    while ((object_ed) && delete_size < requesting_size)
    {
        delete_size += object_ed->content_size;
        object_ed = object_ed->succ;
    }

    if (delte_size < requesting_size)
        retunr - 1;

    cache.head = object_ed;
    if ((object_ed))
        object_ed->prev = NULL;
    else
        cache.tail = NULL;
    while (cur_object != object_ed)
    {
        delete_object = cur_object;
        cur_object = cur_object->succ;
        Free(delete_object->content);
        Free(delete_object);
    }

    return delete_size;
}

int update_max_size() 
{
	int tmp = 0;
	Object *object = cache.head;
	while((object))
	{
		if(Object->content_size > tmp)
			tmp = object->content_size;
		object = object->succ;
	}
	cache.max_size = tmp;
}

int insert_object(char* header, char* object_buf)
{
    int requesting_size = strlen(object_buf);

    Object* object = Malloc(sizeof(Object));
    memcpy(object->header, header);
    object->hash = get_hash(header);
    memcpy(object->content, object_buf);
    object->content_size = strlen(object_buf);

    if (requesting_size > cache.rest_size)
    {
        if (requesting_size <= cache.max_size)
        {
            cache.rest_size += delete_one_object(requesting_size);
            update_max_size();
        }
        else
        {
            cache.rest_size -= delete_objects_to_size(requesting_size);
            update_max_size();
        }
    }

    cache.rest_size -= requesting_size;
    push_back_object(object);
    if(requesting_size > cache.max_size)
    	cache.max_size = requesting_size;
}

void clear_cache()
{
    if (!(cache->head))
        return;

    Object* cur = cache->head;
    while (cur)
    {
        Free(cur->content);
        cur = cur->succ;
    }
}

void* thread(void* vargp)
{
    int connfd = *((int*)vargp);
    Pthread_detach(Pthread_self());
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGPIPE);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);
    Free(vargp);
    process_request(connfd);
    Close(connfd);
    return NULL;
}

void process_request(int clientfd)
{
    rio_t cli_rio;
    Request request;
    char object_buf[MAX_OBJECT_SIZE];
    int object_len;

    Rio_readinitb(&cli_rio, clientfd);

    /* Read request line and headers */
    if (parse_requsethdrs(&cli_rio, clientfd, &request))
        return;

    dbg_printf("Host name: %s\n", request.host);
    dbg_printf("Request to server:\n%s", request.content);

    if ((object_len = connect_with_server(clientfd, &request, object_buf)) > 0)
    {
        dbg_printf("Save cache\n");
    }
}

int parse_requsethdrs(rio_t* rp, int clientfd, Request* request)
{
    char ori_request[MAXLINE], method[MAXLINE], pathname[MAXLINE], uri[MAXLINE];

    memset(ori_request, 0, sizeof(ori_request));
    if (rio_readlineb(rp, ori_request, MAXLINE) < 0)
    {
        clienterror(clientfd, "GET", "400", "Bad Request",
            "Proxy couldn't prase the request");
        return 1;
    }

    dbg_printf("Request from client:\n%s", ori_request);

    memset(method, 0, sizeof(method));
    memset(uri, 0, sizeof(uri));
    memset(pathname, 0, sizeof(pathname));

    sscanf(ori_request, "%s %s", method, uri);

    if (strcasecmp(method, "GET"))
    {
        dbg_printf("Method not implemented\n");
        clienterror(clientfd, method, "501", "Not Implemented",
            "Proxy does not implement this method");
        return 2;
    }

    if (parse_uri(uri, request->host, pathname, request->port))
    {
        dbg_printf("Parse uri error\n");
        clienterror(clientfd, "GET", "400", "Bad Request",
            "Proxy couldn't prase the uri");
        return 3;
    }

    memset(request->content, 0, sizeof(request->content));

    sprintf(request->content, "%s %s %s\r\n", method, pathname, "HTTP/1.0");

    if (read_requesthdrs(rp, request))
    {
        dbg_printf("Read request error\n");
        clienterror(clientfd, "GET", "400", "Bad Request",
            "Proxy couldn't prase the request");
        return 3;
    }

    return 0;
}

int parse_uri(char* uri, char* hostname, char* pathname, char* port)
{
    if (strncasecmp(uri, "http://", 7))
    {
        hostname[0] = '\0';
        dbg_printf("Parse uri error: illegal host\n");
        return 1;
    }

    char *hostst, *hosted, *pathst;
    int hostlen;

    hostst = uri + 7;
    hosted = strpbrk(hostst, "/:\r\n\0");
    hostlen = hosted - hostst;

    memcpy(hostname, hostst, hostlen);

    memset(port, 0, PORT_SIZE);

    pathname[0] = '/';

    if (*hosted == '/')
    {
        strcpy(port, "80");
        pathst = hosted + 1;
    }
    else if (*hosted == ':')
    {
        char* portst = hosted + 1;
        char* ported = portst;
        int len = 0;
        while (*ported >= '0' && *ported <= '9')
        {
            ported++;
            len++;
        }
        strncpy(port, portst, len);

        if (!(pathst = strchr(hosted, '/')))
            pathst++;
    }
    if ((pathst))
        strcat(pathname, pathst);

    return 0;
}

int read_requesthdrs(rio_t* rp, Request* request)
{
    char buf[MAXLINE];

    int flag_contain_host = 0;

    while (1)
    {
        if (rio_readlineb(rp, buf, MAXLINE) < 0)
            return 1;

        if (!strcmp(buf, "\r\n"))
            break;

        if (!strncasecmp(buf, "User-Agent:", 11))
        {
            strcat(request->content, user_agent_hdr);
            continue;
        }
        else if (!strncasecmp(buf, "Connection:", 11))
        {
            strcat(request->content, connection_hdr);
            continue;
        }
        else if (!strncasecmp(buf, "Proxy-Connection:", 17))
        {
            strcat(request->content, proxy_connection_hdr);
            continue;
        }
        else
        {
            if (!strncasecmp(buf, "Host:", 5))
                flag_contain_host = 1;
            strcat(request->content, buf);
        }
    }

    if (!flag_contain_host)
    {
        strcat(request->content, "Host: ");
        strcat(request->content, request->host);
        strcat(request->content, "\r\n");
    }

    strcat(request->content, "\r\n");

    dbg_printf("Request to server:\n%s", request->content);

    return 0;
}

int connect_with_server(int clientfd, Request* request, char* object_buf)
{
    int serverfd, object_len = 0, len;
    rio_t ser_rio;
    char buf[MAXLINE];

    if ((serverfd = open_clientfd(request->host, request->port)) < 0)
    {

        clienterror(clientfd, "GET", "502", "Bad Gate",
            "Proxy couldn't' connect to server");
        return -1;
    }

    Rio_readinitb(&ser_rio, serverfd);

    memset(object_buf, 0, MAX_OBJECT_SIZE);

    if ((rio_writen(serverfd, request->content, sizeof(request->content))) < 0)
    {
        clienterror(clientfd, "GET", "502", "Bad Gate",
            "Proxy couldn't receive massage from server");
        return -1;
    }

    memset(buf, 0, sizeof(buf));
    while ((len = rio_readnb(&ser_rio, buf, sizeof(buf))) > 0)
    {
        object_len += len;
        if (object_len < MAX_OBJECT_SIZE)
            strcat(object_buf, buf);

        if ((rio_writen(clientfd, buf, len)) < 0)
        {
            clienterror(clientfd, "GET", "502", "Bad Gate",
                "Proxy couldn't send massage to client");
            return -1;
        }

        memset(buf, 0, sizeof(buf));
    }

    if (object_len >= MAX_OBJECT_SIZE)
        object_len = -1;

    Close(serverfd);

    return object_len;
}

/*
 * clienterror - returns an error message to the client
 */
void clienterror(
    int fd, char* cause, char* errnum, char* shortmsg, char* longmsg)
{
    char buf[MAXLINE], body[MAXBUF];

    /* Build the HTTP response body */
    sprintf(body, "<html><title>Tiny Error</title>");
    sprintf(body, "%s<body bgcolor="
                  "ffffff"
                  ">\r\n",
        body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

    /* Print the HTTP response */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    int buf_len = strlen(buf), body_len = strlen(buf);
    if (rio_writen(fd, buf, buf_len) != buf_len)
        return;
    sprintf(buf, "Content-type: text/html\r\n");
    buf_len = strlen(buf);
    if (rio_writen(fd, buf, buf_len) != buf_len)
        return;
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    buf_len = strlen(buf);
    if (rio_writen(fd, buf, buf_len) != buf_len)
        return;
    rio_writen(fd, body, body_len);
}