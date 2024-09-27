#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>     //  Chứa cấu trúc cần thiết cho socket. 
#include <netinet/in.h>     //  Thư viện chứa các hằng số, cấu trúc khi sử dụng địa chỉ trên internet
#include <arpa/inet.h>
#include <unistd.h>

#define BUFF_SIZE 1024
#define handle_error(msg)\
    do{perror(msg);exit(EXIT_FAILURE);}while(0)

void chat_func(int server_fd)
{
    int numb_write,numb_read;
    char sendbuff[BUFF_SIZE];
    char recvbuff[BUFF_SIZE];

    while(1){
        memset(sendbuff,'0',BUFF_SIZE);
        memset(recvbuff,'0',BUFF_SIZE);
        
        printf("Please enter the message: ");
        fgets(sendbuff,BUFF_SIZE,stdin);
        if(write(server_fd,sendbuff,BUFF_SIZE)==-1){
            handle_error("write()");
        }
        if (strncmp("exit", sendbuff, 4) == 0) {
            printf("Client exit ...\n");
            break;
        }

        if(read(server_fd,recvbuff,BUFF_SIZE)==-1){
            handle_error("read()");
        }
        if (strncmp("exit", recvbuff, 4) == 0) {
            printf("Client exit ...\n");
            break;
        }
        printf("\nMessage from server: %s\n",recvbuff);
    }
    close(server_fd);
}

int main(int argc,const char *argv[])
{
    int port_no;
    int server_fd;
    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));

    if(argc<3){
        printf("Command: ./client <server address> <port number>\n");
        exit(EXIT_FAILURE);
    }else{
        port_no=atoi(argv[2]);
    }

    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(atoi(argv[2]));
    if(inet_pton(AF_INET,argv[1],&server_addr.sin_addr)==-1)
        handle_error("inet_pton()");

    server_fd=socket(AF_INET,SOCK_STREAM,0);
    if(server_fd<0)
        handle_error("socket()");

    if(connect(server_fd,(struct sockaddr *)&server_addr,sizeof(server_addr))==-1)
        handle_error("connect()");
    system("clear");
    chat_func(server_fd);
    return 0;
}