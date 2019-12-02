#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/time.h>
#include<sys/select.h>
#include<sys/socket.h>
#include<arpa/inet.h>


#define BUF_SIZE 100

int main(int argc, char* argv[]){


    int serv_sock;  // listening socket
    int cli_sock;   // temporal connected socket
    struct sockaddr_in serv_adr, cli_adr;


    struct timeval timeout; // used for select timeout
    fd_set reads, cpy_reads; // a set of file descripter especially for sockets which this process is interested in.

    socklen_t adr_sz;
    int fd_max, str_len, fd_num, i;
    char buf[BUF_SIZE];

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));

    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        perror("Error : ");

    if(listen(serv_sock, 5) == -1)
        perror("Error : ");


    FD_ZERO(&reads);
    FD_SET(serv_sock, &reads); // register the server_socket fd.

    fd_max = serv_sock;
    

    for(;;){
        
        cpy_reads = reads;

        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        fd_num = select(fd_max+1, &cpy_reads, 0, 0, &timeout);


        if(fd_num == -1) // error occurs 
            break;
        else if(fd_num == 0) // timeout
            continue;
        else
        {  // one or more of fds event occurs
        
            
            for(i = 0; i <fd_max+1; i++){
                
                if(FD_ISSET(i, &cpy_reads))
                {
                    if(i == serv_sock) // connection request was reached...
                    {    
                        adr_sz = sizeof(cli_adr);
                        cli_sock = accept(serv_sock, (struct sockaddr*)&cli_adr, &adr_sz);

                        FD_SET(cli_sock, &reads); // register cli sock fd.

                        if(fd_max < cli_sock){
                            fd_max = cli_sock;
                            printf("connected client : %d\n", cli_sock);
                        }

                    }
                    else // handle client request | client socket kernel buffer is ready for data.
                    {
                        str_len = read(i, buf, BUF_SIZE);
                        
                        if(str_len == 0)
                        {
                            FD_CLR(i, &reads);
                            close(i);
                            printf("close client : %d\n", i);
                            
                        }
                        else
                        {
                            write(i, buf, str_len); // echo
                        }

                    }

                       
                }
                
                
            }
            
        }
        






    }
     
    close(serv_sock);










    return 0;
}
