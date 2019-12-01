#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<signal.h>

#define BUF_SIZE 1<<10

int serv_sock;

void killBy(int sig){

    printf("Close the server socket\n");
    if(sig == SIGINT){
        close(serv_sock);

        exit(0);
    }

}

int main(int argc, char* argv[]){

        

    signal(SIGINT, killBy);

    int cli_sock;
    char message[BUF_SIZE];
    int str_len, i;
    struct sockaddr_in serv_adr, cli_adr;

    socklen_t cli_adr_sz;

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1)
        perror("error : ");

    /**
    *    serv_sock : 서버 소켓( 리스닝 소켓, 문지기 소캣)
    */
    printf("Server start\n [server socket] fd : %d\n", serv_sock);

    
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));


    /**
    *  
    *    bind : 서버소켓에 주소를 할당. 
    */    
    if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
        perror("error : ");

    /**
        listen : 연결요청 대기 상태 진입 
        
        int listen(int sock, int backlog)
        
            sock : 이렇게 전달된 소캣 파일 디스크립터가 문지기 소캣이 된다.
            backlog : 연결요청 대기 큐의 크기 - 5라면 클라이언트의 요청을 5개까지 대기시킬 수 있다.  

        한번에 요청 큐 보다 더 많은 요청이 들어온다면
        클라이언트의 connect 함수가 blocking 되어 결국 전부 처리된다.

    */
    if(listen(serv_sock, 5) == -1)
        perror("error : ");
    else
        printf("Waiting...\n");


    /**
        accept : 

        int accept(int sock, struct sockaddr* addr, socklen_t* addrlen);
        
        클라이언트와 서버 간 데이터를 주고 받기 위해서는 application 관점의 소캣이 필요하다.
        클라이언트와 서버 쌍방에게만 dedicate한...

        serv_sock은 (리스닝 소캣) 클라이언트의 요청을 받아드리기위한 전용 소캣이며,

        요청 수락 이후 클라이언트와 통신하기 위한 새로운 소캣이 필요하다. 
        
        accept 함수는 연결을 요청한 클라이언트를 위한 새로운 소캣을 생성하여 그 fd를 반환한다.

        
        
    */
    
    cli_adr_sz = sizeof(cli_adr);

    while(1){
        
        /*
            listen으로부터 생성된 연결요청 대기큐에 연결요청이 들어오면 이를 가져옴.
        */
        cli_sock = accept(serv_sock, (struct sockaddr*)&cli_adr, &cli_adr_sz);
        
        if(cli_sock == -1)
            perror("error : ");
        else
            printf("accept the client request and socket fd : %d is assigned for this cli\n", cli_sock);

        
        while((str_len = read(cli_sock, message, BUF_SIZE)) != 0){
            printf("msg size is %d\n", str_len);

         
            write(cli_sock, message, str_len);
        }
      
        printf("sending was done \n");
        
        /* if not closed ?    
            fd가 쌓여간다... 4부터 
        */    
        close(cli_sock);
        
        sleep(3);
   }




    return 0;
}
