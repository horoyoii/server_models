# Server models   

 * This repository is for introducing the variable server models and to show how these can be implemented based on Linux.  
 1. Iteratvie server  [code](/Iterative_server/Iteratvie_server.cpp)
 2. Multi-process based server [code](/Multi-process_server/Multi_process_server.cpp)  
 3. IO-Multiplexing server with select() [code](/IO-multiplexing_server_select/select_server.cpp)
 4. Multi-Thread based server without thread-pool [code](/Multi-thread_server/Multi-thread_server.cpp)





# 1. Iterative server  

* Handle one request at a time.  
* Connection request stored in a queue created when **listen()** was called.  



# 2. Multi-process server  

 * Handle multiple connection at a time.  
 * this type of a server is called a **concurrent server**.  
 
 ### How to implement  
 * with Unix **fork()** function...
 * When the server receives and accepts the client' connection, it forks a copy of itself and let the child handle the client. 
 
 ![스크린샷, 2019-12-01 17-33-23](https://user-images.githubusercontent.com/34915108/69911577-bab40f80-1460-11ea-9b63-8b469d53db1a.png)
 


 1. Main process accepts the client's connection request with **accept()**.  
 2. Then  main process will fork the child process to handle the request and give the file descripter of socket to the child.  
 3. The child process created by parent will handle(or process) the request with the socket passed by parent.  
 
 
 ### Features  
 
 * Listening socket and Connected socket must be distinguished.  
    - **Listening socket** is for the connection handling of client.
      created by **socket()**  
    - **Connected socket** is for the dedicated socket for the single client. 
      created by **accept()**  
      
 * Prevent the Zombie process.  
    - Main process needs to handle the completion of the child routine with **wait()** or **waitpid()**  
 
 * when fork() called, it copies a socket's file descripter, not a socket itself.  
    - socket itself is possessed by OS, not by application.  
    - process possesses only file descripter of socket.  
    - fork를 통한 복사 시 동일한 소캣에 대한 두 개의 fd가 생성되고 모든 fd가 닫혀야 socket이 소멸한다.  
    
    
 

# 3. IO Multiplexing Model with select() system call.  

### Abstract  
 * The server uses a single thread to mamange all connections.
    it uses select() or epoll() system call to wait for the events on all connectinos   
    When select() delivers one or more events, the server's main loop invokes handlers for each ready connection.  
    
 * I/O Multiplexing means that single process or thread can handle all of the read and write.  
 * The reason why a single process(or thread) can handle only one client request is that I/O functions are blocking.  
   So 입출력 데이터가 준비될 때까지 무한정 blocking되어 다른 여러 클라이언트의 입출력을 처리할 수 없었기 때문이다.  
   
  * I/O Multiplexing 기법 적용 시 입출력 함수는 여전히 blocking 되지만, 이러한 입출력 함수 자체를 호출하기 전에 어떤 File Descripter(즉, 소캣)에서 입출력의 준비가 되었는지 확인할 수 있다.  
 
 ### 3.1. Blocking I/O model  
  ![스크린샷, 2019-12-02 12-01-55](https://user-images.githubusercontent.com/34915108/69927510-93f6e700-14fb-11ea-877c-6d56e88b647a.png)
  
  
  1. 어플리케이션에서 I/O 작업을 하는 경우 ( 소캣을 통해 read 를 수행하는 경우) 데이터가 네트워크를 통해 도착하는 것을 기다린다.  
  2. 패킷이 네트워크를 통해 도착하게 되면 커널 내의 버퍼( ex) TCP 수신 버퍼)에 복사된다.  
  3. 커널 내의 복사된 데이터를 어플리케이션이 사용하기 위해서 커널 버퍼에서 유저 버퍼로 복사되어야 한다.  
  
  **즉, blocking read 함수의 호출은 네트워크를 통하여 도착한 패킷이 커널 버퍼에 복사된 후 다시 유저 버퍼에 복사되어야 반환된다.**   
  **read함수는 커널 버퍼에 패킷이 도착하기를, 커널 버퍼에서 유저 버퍼로 데이터가 복사되기를 기다리는 blocking 함수이다.**  
  
  
### 3.2. I/O Multiplexing model  
 ![스크린샷, 2019-12-02 12-12-40](https://user-images.githubusercontent.com/34915108/69927882-10d69080-14fd-11ea-839a-7964d98528bd.png)
 
 
 * 이 모델에서는 **하나 이상의 소캣**이 준비될 때까지 기다린다.  
 * 하나의 프로세스에서 여러 소캣을 관찰하다가 그 중 하나 이상의 소캣이 준비되면( 패킷이 도착하여 커널 버퍼에 복사되어 읽을 수 있는 상태가 되었다면)
이 프로세스는 어떤 소캣이 준비되었는지를 알 수 있고 이를 대상으로 read or wrtie을 수행한다.  

* So read() function used with select() can read data immediately.  


### How to implement  


    while(1){

       fd_num = select(fd_max+1, &cpy_reads, 0, 0, &timeout);

       for(i =0; i <fd_max+1 ; i++)
       {  
         if(FD_ISSET(i, &cpy_reads)
         {
            if( i == serv_sock)
            {
               // connection request
            }
            else{
              // client connected socket is ready to read
            }
         } 
       }

    }


* fd_set : 데이터 수신여부를 관찰하는 관찰 대상 집합.  
  - fd_set에 서버 소캣과 connected socket 이 등록되면 이후 select를 통해 변화를 감지할 수 있다.  
  
* select : **네트워크를 통해 들어온 패킷이 커널 버퍼에 복사되어 어플리케이션이 읽을 수 있도록 준비된다면 blocking이 해제되어 반환된다.**  

* for loop : 이후 어떤 소캣이 읽힐 준비가 되었는지 for 문을 돌면서 확인하고 이를 대상으로 I/O 를 수행한다.  


# 4. Multi-thread model without thread-pool.  

## Abstract  

  * This model works very much like a multi-process based model.  
  * Whenever a new connection arrives via Listening socket, a main process craetes a new thread.  
  * Then a newly craeted thread gets a connected socket fd from its parent and processes its job.  
  ![스크린샷, 2019-12-06 13-48-45](https://user-images.githubusercontent.com/34915108/70296865-26b2c100-182f-11ea-9c71-13d361759de7.png)

## How to implement  
    whlie(1){
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

## Strength over multi-process based server  

1) Multi-process servers can suffer from context-switching overhead and IPC
  




# 5. Multi-thread model with thread-pool.  

## Abstract  

 * whenever a new request arrives, a server creates a new thread for every single request,  
   which is a overhead that spend more time and consume more resource in creating and destroying threads than procesing actual requests.

  * But actually each created thread does the short job.  
  * So a thread pool makes it possible to reuse previously created threads to execute tasks.  
    Since threads are already existing when new request arrives, the delay introduced by thread creation is eliminated.  
    
  * the number of threads in thread pool is limited. So handling a lot of requests needs coordination with help of some kind of QUEUE.  
  
  
  
 












 
 
 













## Reference  

http://www.cs.ucr.edu/~makho001/masoud/cs164/fork.pdf

http://byteliu.com/2019/05/08/LINUX-%E2%80%93-IO-MULTIPLEXING-%E2%80%93-SELECT-VS-POLL-VS-EPOLL/

https://jongmin92.github.io/2019/02/28/Java/java-with-non-blocking-io/#%EB%A9%80%ED%8B%B0%ED%94%8C%EB%A0%89%EC%8B%B1-%EA%B8%B0%EB%B0%98%EC%9D%98-%EB%8B%A4%EC%A4%91-%EC%A0%91%EC%86%8D-%EC%84%9C%EB%B2%84

https://www.usenix.org/legacy/publications/library/proceedings/osdi99/full_papers/banga/banga_html/node3.html




