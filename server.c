#include <stdio.h> 
#include <sys/types.h>  
#include <sys/socket.h>  
#include <sys/un.h>   
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define CAN_SERVICE "CAN_SERVICE" 
#define NUM_OF_CPU 5

/* master */
struct var
{
	char time[50];
	char device[50];
	int priority;
};
struct var var1;


void *downloadfile(void *filename)
{
	printf("I am downloading the file %s!\n",(char *)filename);
	sleep(10);
	long downloadtime = rand()%100;
	printf("I finsh downing files %s within %ld minutes!\n",(char *)filename,downloadtime);

	pthread_exit((void *)downloadtime);
}

int main(void)  
{      
    int ret;     
    int accept_fd;
    int socket_fd; 
	int len;
	char str[50];
	int rc;
	int t;
	int downloadtime;

    static char recv_buf[1024];
    socklen_t clt_addr_len; 
    struct sockaddr_un clt_addr;  
    struct sockaddr_un srv_addr;  
    pthread_t threads[NUM_OF_CPU];
	char files[NUM_OF_CPU][20] = {"1.txt","2.txt","3.txt","4.txt","5.txt"};
    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_JOINABLE);
    
	//创建socket
    socket_fd=socket(PF_UNIX,SOCK_STREAM,0);  
    if(socket_fd<0)  
    {  
        perror("cannot create communication socket");  
        return 1;  
    }    
          
    // 设置服务器参数  
    srv_addr.sun_family=AF_UNIX;  
    strncpy(srv_addr.sun_path,CAN_SERVICE,sizeof(srv_addr.sun_path)-1);  
    unlink(CAN_SERVICE);  
 
    // 绑定socket地址 
    ret=bind(socket_fd,(struct sockaddr*)&srv_addr,sizeof(srv_addr));  
    if(ret==-1)  
    {  
        perror("cannot bind server socket");  
        close(socket_fd);  
        unlink(CAN_SERVICE);  
        return 1;  
    }  
 
    // 监听   
    ret=listen(socket_fd,1);  
    if(ret==-1)  
    {  
        perror("cannot listen the client connect request");  
        close(socket_fd);  
        unlink(CAN_SERVICE);  
        return 1;  
    }  

 

 while(1)
 {
   
       // 接受connect请求 
       len=sizeof(clt_addr);  
  	   accept_fd=accept(socket_fd,(struct sockaddr*)&clt_addr,&len);  
 	   if(accept_fd<0)  
       {  
      	  perror("cannot accept client connect request");  
          close(socket_fd);  
          unlink(CAN_SERVICE);  
          return 1;  
       }  
        //printf("You got a connection from %s\n",inet_ntoa(client.sin_addr) ); 

       	printf("Creating thread\n");
		rc = pthread_create(&threads[t],&thread_attr,downloadfile,(void *)files[t]);
		if(rc)
		{
			perror("Fail to pthread_create");
			exit(-1);
		}	
 	  
	   	// 读取和写入  
   	    memset(recv_buf,0,1024);  
    	int num=read(accept_fd,recv_buf,sizeof(recv_buf));  
		strcpy(var1.time,recv_buf);
    	printf("Message from client (%d))time :%s\n",num,var1.time);    
         
    }

	//receive input data & create new thread

		pthread_attr_destroy(&thread_attr);

		pthread_join(threads[t],(void **)&downloadfile);	

		pthread_exit(NULL);

	

    // 关闭socket
    close(accept_fd);  
    close(socket_fd);  
    unlink(CAN_SERVICE);  
    return 0;  
}
