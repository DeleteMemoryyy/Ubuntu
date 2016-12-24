#include "csapp.h"
#include <stdio.h>

#define _GNU_SOURCE

/* Recommended max cache and obj sizes */
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

#define DEBUG
#ifdef DEBUG
#define dbg_printf(...) printf(__VA_ARGS__)
#else
#define dbg_printf(...)
#endif

typedef struct
{
    char host[MAXLINE];
    char content[MAXLINE];
    char port[16];
} Request;

typedef struct Object_t
{
    char header[MAXLINE];
    size_t hash;
    char* content;
    int content_size;
    struct Object_t *prev, *succ;
} Object;

typedef struct
{
    Object *head, *tail;
    int max_size, rest_size;
} Cache;

/* Global variables */
sem_t mutex;
Cache cache;

void* thread(void* vargp);
void process_request(int connfd);
int parse_requsethdrs(rio_t* rp, int clientfd, Request* request);
int parse_uri(char* uri, char* hostname, char* pathname, char* port);
int read_requesthdrs(rio_t* rp, Request* request);
int connect_with_server(int clientfd, Request* request, char* object_buf);
void clienterror(
    int fd, char* cause, char* errnum, char* shortmsg, char* longmsg);

size_t get_hash(char* str);
void init_proxy();
Object* search_object(char* header, size_t hash);
void update_object_position(Object* obj);
void push_back_object(Object* obj);
int delete_one_object(int req_size);
void delete_in_list(Object* obj);
int delete_objects_to_size(int req_size);
void update_max_size();
void insert_object(char* header, size_t hash, char* object_buf, int req_size);
void clear_cache();

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

    clear_cache();

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

void init_proxy()
{
    sem_init(&mutex, 0, 1);
    cache.head = NULL;
    cache.tail = NULL;
    cache.max_size = 0;
    cache.rest_size = MAX_CACHE_SIZE;
}

Object* search_object(char* header, size_t hash)
{
    Object* cur_obj = cache.tail;
    while ((cur_obj))
    {
        if (hash == cur_obj->hash && !strcmp(header, cur_obj->header))
            break;
        cur_obj = cur_obj->prev;
    }

    return cur_obj;
}

inline void update_object_position(Object* obj)
{
    delete_in_list(obj);
    push_back_object(obj);
}

inline void push_back_object(Object* obj)
{
    obj->succ = NULL;
    obj->prev = cache.tail;

    if (cache.tail)
        cache.tail->succ = obj;
    else
        cache.head = obj;

    cache.tail = obj;
}

int delete_one_object(int req_size)
{
    Object* obj = cache.head;

    while ((obj) && obj->content_size < req_size)
        obj = obj->succ;

    if (!obj)
        return -1;

    delete_in_list(obj);

    req_size = obj->content_size;

    free(obj->content);
    free(obj);

    return req_size;
}

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
    while ((obj))
    {
        if (obj->content_size > tmp)
            tmp = obj->content_size;
        obj = obj->succ;
    }
    cache.max_size = tmp;
}

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

void clear_cache()
{
    if (!(cache.head))
        return;

    Object *cur_obj = cache.head, *delete_obj;
    while (cur_obj)
    {
        delete_obj = cur_obj;
        cur_obj = cur_obj->succ;
        free(delete_obj->content);
        free(delete_obj);
    }

    cache.head = NULL;
    cache.tail = NULL;
    cache.max_size = 0;
    cache.rest_size = MAX_CACHE_SIZE;
}

void* thread(void* vargp)
{
    int connfd = *((int*)vargp);
    Pthread_detach(Pthread_self());
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGPIPE);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);
    free(vargp);
    process_request(connfd);
    Close(connfd);
    return NULL;
}

void process_request(int clientfd)
{
    rio_t cli_rio;
    Request request;

    Rio_readinitb(&cli_rio, clientfd);

    /* Read request line and headers */
    if (parse_requsethdrs(&cli_rio, clientfd, &request))
        return;

    dbg_printf("Host name: %s\n", request.host);
    dbg_printf("Request to server:\n%s", request.content);

    size_t hash = get_hash(request.content);
    Object* obj_cache = search_object(request.content, hash);
    if ((obj_cache))
    {
        dbg_printf("	Found response in cache, update the position\n");
        P(&mutex);
        update_object_position(obj_cache);
        V(&mutex);
        if ((rio_writen(clientfd, obj_cache->content, obj_cache->content_size))
            < 0)
        {
            clienterror(clientfd, "GET", "502", "Bad Gate",
                "Proxy couldn't respond content in cache\n");
            return;
        }
    }
    else
    {
        char object_buf[MAX_OBJECT_SIZE + 1];
        int object_len;

        if ((object_len = connect_with_server(clientfd, &request, object_buf))
            > 0)
        {
            dbg_printf("	Save response in cache\n");
            P(&mutex);
            insert_object(request.content, hash, object_buf, object_len);
            V(&mutex);
        }
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

    dbg_printf("Request header from client:\n%s", ori_request);

    if (!strncmp(ori_request, "Clear cache", 11))
    {
        P(&mutex);
        clear_cache();
        V(&mutex);
        return -1;
    }

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

    memset(object_buf, 0, MAX_OBJECT_SIZE + 1);

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
        {
            memcpy(object_buf, buf, len);
            object_buf += len;
        }
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