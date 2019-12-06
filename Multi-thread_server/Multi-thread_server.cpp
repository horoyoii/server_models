#include<pthread.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#define BUF_SIZE 100

void* handleRequest(void* arg);

/**
* Server will create new thread to handle the request dedicately whenever a request comes.
* 
*/
int main(int argc, char* argv[]){
    
    int serv_sock;  // Listening socket fd
    int cli_sock;   // client connected socket fd
    
    struct sockaddr_in serv_adr, cli_adr;
    int cli_adr_sz;

    pthread_t tid;
    

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        perror("bind : ");

    if(listen(serv_sock, 5) == -1)
        perror("listen : ");



    while(1)
    {
        cli_adr_sz = sizeof(cli_adr);
        cli_sock = accept(serv_sock, (struct sockaddr*)&cli_adr, (socklen_t*)&cli_adr_sz);
        printf("client connected\n");

        /** 
        * Main thread needs to wait for new client connection so uses the detach mode, not join.
        *   
        * pass the connected socket fd to a craeted thread.
        */  
        pthread_create(&tid, NULL, handleRequest, (void*)&cli_sock);
        pthread_detach(tid);   
        

    }


    close(serv_sock);

    return 0;
}


void* handleRequest(void* arg)
{
    printf("thread [%d] handle the request\n", (int)pthread_self());
    int cli_sock = *((int*)arg);
    
    sleep(3); // assume this thread handles havy job.

    int str_len=0, i;
    char msg[BUF_SIZE];
    
    while((str_len = read(cli_sock, msg, sizeof(msg))) != 0)
    {
        write(cli_sock, msg, str_len);
    }
    

    close(cli_sock);
}
