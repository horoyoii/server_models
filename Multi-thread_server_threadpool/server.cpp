#include<pthread.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include"ThreadPool.cpp"

#define BUF_SIZE 256

void requestHandler(void* arg);

int main(int argc, char* argv[]){
    int serv_sock;  // Listening socket fd
    int cli_sock;   // client connected socket fd
    
    struct sockaddr_in serv_adr, cli_adr;
    int cli_adr_sz;
    
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        perror("bind : ");

    if(listen(serv_sock, 5) == -1)
        perror("listen : ");

    
    // create a ThreadPool with a pool size 50
    ThreadPool tp(10);

    tp.createWorkers();

    while(1)
    {
        cli_adr_sz = sizeof(cli_adr);
        cli_sock = accept(serv_sock, (struct sockaddr*)&cli_adr, (socklen_t*)&cli_adr_sz);
        printf("client connected - assigned cli_sock : %d\n", cli_sock);
        
        int* passSock = new int(cli_sock);
        
        /** ### usinge a Call-by-Reference in a multithread program is dangerous. #####
            
            if pass the cli_sock directly 
            like this -> tp.dispatch(requestHandler, (void*)&cli_sock); ,
            
            before the cli_sock's original value is copied in a thread routine, 
            
            the value may be overwritten by accept() call.
            

        */
            
        tp.dispatch(requestHandler, (void*)passSock);

    }

    close(serv_sock);
    return 0;
}

/**
    This will be executed in a available thread routine.
*/
void requestHandler(void *arg)
{
    int cli_sock = *((int*)arg);
    delete (int*)arg;

    printf("handler req : sock %d\n", cli_sock);   
    sleep(3); // assume this thread handles havy job.

    int str_len=0, i;
    char msg[BUF_SIZE];
    
    while((str_len = read(cli_sock, msg, sizeof(msg))) != 0)
    {
        write(cli_sock, msg, str_len);
        printf("write : sock %d \n", cli_sock);
    }
    

    close(cli_sock);
}
