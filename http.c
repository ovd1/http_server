#include <stdio.h>




void statServer(char *port)
{
    int sock, comp; // дескрипторы сокетов
    char buf[1024]; //буфур для приема
    char *p;
    struct sockaddr_in addr;
    
    sock = socket(AF_INET, SOCK_STREAM, 0); // создаем сокет для входных подключений
    if (sock < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }
    
    memset(&addr, 0, sizeof(addr));
    
    // Указываем параметры сервера
    int port = 8080;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    
    if (bind(sock, (struct sockaddr*) &addr, sizeof(addr)) == -1){
        perror("bind error");
        
        close(sock);
        exit(1);
    }
    // Setting all elements to -1: signifies there is no client connected
    if (listen(sock, 100) == -1){
        perror("listen error");
        
        close(sock);
        exit(1);
    }
}

void respond(int n)


static char* find_page(char *p, size_t size){
    while (size-- != 0 && *(p++) != ' ');
    if (*(p - 1) == ' '){
        return (p - 1);
    }
    else
    {
        return NULL;
    }
    
}
