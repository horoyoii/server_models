# 1. Server models   


## 1. Iterative server  

* Handle one request at a time.  
* Connection request stored in a queue created when **listen()** was called.  



## 2. Multi-process server  

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
    
    
 
















## Reference  

http://www.cs.ucr.edu/~makho001/masoud/cs164/fork.pdf
