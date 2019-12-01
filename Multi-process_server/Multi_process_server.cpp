#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<signal.h>
#include<sys/wait.h>


#define BUF_SIZE 30

void handleTerminatedChild(int sig);

int main(int argc, char* argv[]){


    int serv_sock, cli_sock;
    struct sockaddr_in serv_adr, cli_adr;


    pid_t pid;
    struct sigaction act;
    socklen_t adr_sz;

    int str_len, state;

    char buf[BUF_SIZE];


    act.sa_handler = handleTerminatedChild;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    state = sigaction(SIGCHLD, &act, 0);

    
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        perror("error : ");


    if(listen(serv_sock, 5) == -1)
        perror("error : ");



    while(1){

        
        adr_sz = sizeof(cli_adr);

        cli_sock = accept(serv_sock, (struct sockaddr*)&cli_adr, &adr_sz);
        
        if(cli_sock == -1)
            continue;
        else
            printf("new client connected...\n");

        /**
        * create a new process to handle the request.
        */
        pid = fork();

        if(pid == -1){ // whne failed to craete new process 
            close(cli_sock);
            continue;
        }

        /**
        *  child process's routine
        *
        */
        if(pid == 0 ){
            close(serv_sock);

            //sleep(3);

            while((str_len = read(cli_sock, buf, BUF_SIZE)) != 0)
                write(cli_sock, buf, str_len);


            close(cli_sock);
            printf(" child process is done with its job\n");
            return 0;
        }else{ // Parent process removes the client socket fd immediately
            
            close(cli_sock);
        }
    }

    close(serv_sock);
    
    
    
    return 0;
}


void handleTerminatedChild(int sig){
    pid_t pid;
    int status;

    pid = waitpid(-1, &status, WNOHANG);

    printf("removed proc id : %d\n", pid);
}
