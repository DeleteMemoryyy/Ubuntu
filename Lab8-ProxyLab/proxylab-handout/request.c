#include <stdio.h>

#include "cache.h"
#include "csapp.h"
#include "request.h"

/* You won't lose style points for including this long line in your code */
static const char* user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux "
                                    "x86_64; rv:10.0.3) Gecko/20120305 "
                                    "Firefox/10.0.3\r\n";
static const char* connection_hdr = "Connection: close\r\n";
static const char* proxy_connection_hdr = "Proxy-Connection: close\r\n";

void init_proxy()
{
    readcnt = 0;
    sem_init(&mutex, 0, 1);
    sem_init(&w, 0, 1);
    cache.head = NULL;
    cache.tail = NULL;
    cache.max_size = 0;
    cache.rest_size = MAX_CACHE_SIZE;
}

/* Free all cache objects in memory */
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

    /* Block SIGPIPE by pthread_sigmask after every thread been created */
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGPIPE);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);

    free(vargp);
    /* Process one request every time creating a new thread */
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

/* First search in cache */
#ifdef CACHE_WITH_HEADER
    size_t hash = get_hash(request.header);

    P(&mutex);
    readcnt++;
    if (readcnt == 1)
        P(&w);
    V(&mutex);
    Object* obj_cache = search_object(request.header, hash);
#else
    size_t hash = get_hash(request.content);

    P(&mutex);
    readcnt++;
    if (readcnt == 1)
        P(&w);
    V(&mutex);
    Object* obj_cache = search_object(request.content, hash);
#endif

    /* Hit the cache, respond with cache object */
    if ((obj_cache))
    {
        dbg_printf("    Found response in cache, update the position\n");
        if ((rio_writen(clientfd, obj_cache->content, obj_cache->content_size))
            < 0)
        {
            clienterror(clientfd, "GET", "502", "Bad Gate",
                "Proxy couldn't respond content in cache\n");
            return;
        }

        P(&mutex);
        readcnt--;
        if (!readcnt)
            V(&w);
        V(&mutex);
    }
    /* Missed, connect with server and cache received object */
    else
    {
        char object_buf[MAX_OBJECT_SIZE + 1];
        memset(object_buf, 0, sizeof(object_buf));
        int object_len;

        P(&mutex);
        readcnt--;
        if (!readcnt)
            V(&w);
        V(&mutex);

        if ((object_len = connect_with_server(clientfd, &request, object_buf))
            > 0)
        {
            dbg_printf("    Save response in cache\n");

            P(&w);
            update_object_position();
#ifdef CACHE_WITH_HEADER
            insert_object(request.header, hash, object_buf, object_len);
#else
            insert_object(request.content, hash, object_buf, object_len);
#endif
            V(&w);
        }
    }
}

int parse_requsethdrs(rio_t* rp, int clientfd, Request* request)
{
    char ori_request[MAXLINE], method[MAXLINE], pathname[MAXLINE], uri[MAXLINE];

    memset(ori_request, 0, sizeof(ori_request));

    /* Read request line */
    if (rio_readlineb(rp, ori_request, MAXLINE) < 0)
    {
        clienterror(clientfd, "GET", "400", "Bad Request",
            "Proxy couldn't prase the request");
        return 1;
    }

    dbg_printf("Request header from client:\n%s", ori_request);

    /* Command to clear cache */
    if (!strncasecmp(ori_request, "clear cache", 11))
    {
        P(&w);
        clear_cache();
        V(&w);
        return -1;
    }

    memset(method, 0, sizeof(method));
    memset(uri, 0, sizeof(uri));
    memset(pathname, 0, sizeof(pathname));

    sscanf(ori_request, "%s %s", method, uri);

#ifdef ONLY_GET
    if (strcasecmp(method, "GET"))
    {
        dbg_printf("Method not implemented\n");
        clienterror(clientfd, method, "501", "Not Implemented",
            "Proxy does not implement this method");
        return 2;
    }
#endif

    /* Parse host/path/port in uri */
    if (parse_uri(uri, request, pathname))
    {
        dbg_printf("Parse uri error\n");
        clienterror(clientfd, "GET", "400", "Bad Request",
            "Proxy couldn't prase the uri");
        return 3;
    }

    memset(request->content, 0, sizeof(request->content));
    sprintf(request->content, "%s %s %s\r\n", method, pathname, "HTTP/1.0");

#ifdef CACHE_WITH_HEADER
    memset(request->header, 0, sizeof(request->header));
    sprintf(request->header, "%s %s %s %s", method, request->host,
        request->port, pathname);
#endif

    /* Parse rest request lines */
    if (read_requesthdrs(rp, request))
    {
        dbg_printf("Read request error\n");
        clienterror(clientfd, "GET", "400", "Bad Request",
            "Proxy couldn't prase the request");
        return 3;
    }

    return 0;
}

int parse_uri(char* uri, Request* request, char* pathname)
{
    char *hostst, *hosted, *pathst;
    int hostlen;

    if (!(hostst = strstr(uri, "//")))
    {
        dbg_printf("Parse uri error: illegal host\n");
        return 1;
    }

    hostst += 2;
    hosted = strpbrk(hostst, "/:\r\n\0");
    hostlen = (size_t)hosted - (size_t)hostst;

    strncpy(request->host, hostst, hostlen);

    memset(request->port, 0, PORT_SIZE);

    if (*hosted == ':')
    {
        char* portst = hosted + 1;
        char* ported = portst;
        int len = 0;
        while (*ported >= '0' && *ported <= '9')
        {
            ported++;
            len++;
        }
        strncpy(request->port, portst, len);
        request->port[len] = '\0';
    }
    else
        strcpy(request->port, "80");

    if ((pathst = strchr(hosted, '/')))
        strcpy(pathname, pathst);
    else
        strcpy(pathname, "/");

    return 0;
}

int read_requesthdrs(rio_t* rp, Request* request)
{
    char buf[MAXLINE];

    int flag_contain_host = 0, flag_user_agent = 0, flag_connection = 0,
        flag_proxy_connection = 0;

    while (1)
    {
        if (rio_readlineb(rp, buf, MAXLINE) < 0)
            return 1;

        if (!strcmp(buf, "\r\n"))
            break;

        dbg_printf("%s", buf);

        if (!strncasecmp(buf, "User-Agent:", 11))
        {
            strcat(request->content, user_agent_hdr);
            flag_user_agent = 1;
        }
        else if (!strncasecmp(buf, "Connection:", 11))
        {
            strcat(request->content, connection_hdr);
            flag_connection = 1;
        }
        else if (!strncasecmp(buf, "Proxy-Connection:", 17))
        {
            strcat(request->content, proxy_connection_hdr);
            flag_proxy_connection = 1;
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

    if (!flag_user_agent)
        strcat(request->content, user_agent_hdr);
    if (!flag_connection)
        strcat(request->content, connection_hdr);
    if (!flag_proxy_connection)
        strcat(request->content, proxy_connection_hdr);

    strcat(request->content, "\r\n");

    return 0;
}

int connect_with_server(int clientfd, Request* request, char* object_buf)
{
    int serverfd, object_len = 0, len;
    rio_t ser_rio;
    char buf[MAX_OBJECT_SIZE + 1];

    if ((serverfd = open_clientfd(request->host, request->port)) < 0)
    {

        clienterror(clientfd, "GET", "502", "Bad Gate",
            "Proxy couldn't' connect to server");
        close(serverfd);
        return -1;
    }

    Rio_readinitb(&ser_rio, serverfd);

    if ((rio_writen(serverfd, request->content, sizeof(request->content))) < 0)
    {
        clienterror(clientfd, "GET", "502", "Bad Gate",
            "Proxy couldn't send massage to server");
        close(serverfd);
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
            close(serverfd);
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
    sprintf(body, "<html><title>Proxy Error</title>");
    sprintf(body, "%s<body bgcolor="
                  "ffffff"
                  ">\r\n",
        body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The proxy server</em>\r\n", body);

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