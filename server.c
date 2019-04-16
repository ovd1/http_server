#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>



#define ERROR_404 "html_doc/error_404.html"
#define ERROR_400 "html_doc/error_401.html"

#define MY_PRINTF(a, b) printf("a %s\n", b) 

static const char response_header_200Ok[] = "HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n";

static const char resp_404[] = "HTTP/1.1 404 Not Found\r\n"
"Server: Apache/2.2.14 (Win32)\r\n"
"Connection: close\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n";

static const char resp_400[] = "HTTP/1.1 400 Bad Request\r\n"
"Server: Apache/2.2.14 (Win32)\r\n"
"Connection: close\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n";

int startServer(const char * host, int port) {
    int sock;
    struct sockaddr_in addr;
    struct hostent * host_ent;
    char * hostAddr;
    
    int hport = 8080;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(hport);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {  
        printf("Error creating socket!\n");  
        exit(1);  
    }  
    printf("Socket created...\n");

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf("Error binding socket to port!\n");  
        exit(1);
    }
    printf("Binding done...\n");
    
    if (listen(sock, 100) == -1){
        perror("listen error");
        
        close(sock);
        exit(1);
    }
    
    return sock;
}

  
static char* find_page(char *p, size_t size){
    while (size-- != 0 && *(p++) != ' ');
    if (*(p - 1) == ' '){
        return (p - 1);
    } else {
        return NULL;
    }
    
}


const char *parse_http_request(char *buf, int sockfd){
    int  comp, read_size; 
    char *p, *result;

    if (sockfd != -1){
        read_size = read(sockfd, buf, 1024);

        if (read_size >= 0) {
            printf("%s\n", buf);
            p = buf;
            comp = memcmp(p, "GET", 3);
            if (comp == 0) {
                p += 3;
                if (*p == ' '){
                    p++;
                    result = p;
                    p = find_page(p, read_size - 4);
                    if (p != NULL) { 
                        comp = memcmp(p, " HTTP/1.1\r\n", sizeof(" HTTP/1.1\r\n") - 1);
                        *p = '\0';
                        if (!comp)
                            return result;
                    }
                }
            }
        }
    }
    
    return NULL;
}


int main()
{
    int sockfd, comp, port; 
    char buf[1024]; 
    const char *host;
    
    sockfd = startServer(host, port);
 
    while (1){
        const char *page_name_begin;
        int open_file, bytes_read, err_file;  
        int sock2 = accept(sockfd, NULL, NULL);
        
        if (sock2 < 0) {
            continue;
        }
        
        page_name_begin = parse_http_request(buf, sock2);
        //printf("result %s\n", page_name_begin);
        MY_PRINTF("result", page_name_begin); 
        
        if (page_name_begin) {
            open_file = open(page_name_begin+1, 0, O_RDONLY);
            //comp = memcmp(page_name_begin, "/index.htm", sizeof("/index.htm") - 1);
            if (open_file != -1){
                write(sock2, response_header_200Ok, sizeof(response_header_200Ok)-1);
                bytes_read = read(open_file, buf, sizeof(buf));
                
                write(sock2, buf, bytes_read);
                write(sock2, "\r\n", sizeof("\r\n")-1);
            } else {
                err_file = open(ERROR_404, 0, O_RDONLY);
                write(sock2, resp_404, sizeof(resp_404)-1);
                bytes_read = read(err_file, buf, sizeof(buf));
                
                write(sock2, buf, bytes_read);
                write(sock2, "\r\n", sizeof("\r\n")-1);
            }
        } else {
            //TODO: 401 response
            err_file = open(ERROR_400, 0, O_RDONLY);
            write(sock2, resp_400, sizeof(resp_400)-1);
            bytes_read = read(err_file, buf, sizeof(buf));
                
            write(sock2, buf, bytes_read);
            write(sock2, "\r\n", sizeof("\r\n")-1);
        }
        
        close(sock2);
    }


    close(sockfd);
    return 0;
}
