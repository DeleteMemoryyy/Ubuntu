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
    int length;
    char* contentp;
} Response;

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
    Signal(SIGALRM,SIG_IGN);

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

void* thread(void* vargp)
{
    int connfd = *((int*)vargp);
    Pthread_detach(Pthread_self());
    Free(vargp);
    process_request(connfd);
    Close(connfd);
    return NULL;
}

void process_request(int clientfd)
{
    rio_t cli_rio;
    Request request;
    Response* response;
    char object_buf[MAX_OBJECT_SIZE];
    int object_len;

    Rio_readinitb(&cli_rio, clientfd);

    /* Read request line and headers */
    if (parse_requsethdrs(&cli_rio, clientfd, &request))
        return;

    dbg_printf("\nHost name: %s\n", request.host);
    dbg_printf("Request to server:\n%s", request.content);

    response = Malloc(sizeof(Response));
    memset(response, 0, sizeof(Response));

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
            "Proxy can't prase the request");
        return 1;
    }

    dbg_printf("Request from client: %s\n\n\n", ori_request);

    memset(method, 0, sizeof(method));
    memset(uri, 0, sizeof(uri));

    sscanf(ori_request, "%s %s", method, uri);

    if (strcasecmp(method, "GET"))
    {

        clienterror(clientfd, method, "501", "Not Implemented",
            "Proxy does not implement this method");
        return 2;
    }

    if (parse_uri(uri, request->host, pathname, request->port))
    {
        dbg_printf("Parse uri error\n");
        return 3;
    }

    memset(request->content, 0, sizeof(request->content));

    sprintf(request->content, "%s %s %s\r\n", method, pathname, "HTTP/1.0");

    if (read_requesthdrs(rp, request))
    {
        dbg_printf("Read request error\n");
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
        pathname = strcpy(pathname, pathst);
    else
        pathname[0] = '\0';

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
        dbg_printf("Request header from client:\n%s", buf);

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
        strcat(request->content, "Host:");
        strcat(request->content, request->host);
        strcat(request->content, "\r\n");
    }

    strcat(request->content, "\r\n");

    return 0;
}

int connect_with_server(int clientfd, Request* request, char* object_buf)
{
    int serverfd, object_len = 0, len;
    rio_t ser_rio;
    char buf[MAXLINE];

    serverfd = Open_clientfd(request->host, request->port);

    Rio_readinitb(&ser_rio, serverfd);

    memset(object_buf, 0, MAX_OBJECT_SIZE);

    Rio_writen(serverfd, request->content, sizeof(request->content));

    memset(buf, 0, sizeof(buf));
    while ((len = rio_readnb(&ser_rio, buf, sizeof(buf))) > 0)
    {
        object_len += len;
        strcat(object_buf, buf);

        Rio_writen(clientfd, buf, len);

        memset(buf, 0, sizeof(buf));
    }

    if (object_len >= MAX_OBJECT_SIZE)
        object_len = -1;

    Close(serverfd);

    // return response;
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
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
}