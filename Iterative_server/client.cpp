#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/wait.h>

#define BUF_SIZE 1<<10
//#define N 10
int main(int argc, char* argv[]){
    int N = atoi(argv[3]);

    pid_t pid;
    for(int i=0;i<N;i++){
        pid = fork();   
        
        if(pid == 0)
            break;
    }
    
    if(pid !=0){
        // parent

        for(int i =0; i < N; i++){
            wait(NULL);
        }

        printf("Parent done\n");
        return 0;
    }


    int sock;
    char message[BUF_SIZE] = "abcde";

    int str_len;
    struct sockaddr_in serv_adr;

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        perror("error : ");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port =htons(atoi(argv[2]));


    /*
        connect :

        connect 함수 호출 시 : 커널에서 자동으로 IP와 port를 할당해준다. 

    */
    if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        perror("error : ");

    //printf("pid[%d] : send data to server\n", getpid());    
    write(sock, message, strlen(message));

    
    str_len = read(sock, message, BUF_SIZE-1);

    printf("respone - pid[%d] msg : %s\n", getpid(), message);

   
    close(sock); 


    return 0;
}
