#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define handle_error(msg) \
    do{perror(msg);exit(EXIT_FAILURE);} while(0)

#define LISTEN_BACKLOG 10
#define MSG_SIZE  1024
#define MAX_CONNECT 10

void chat_func(int client_fd)
{
    int numb_read,numb_write;
    char send_buff[MSG_SIZE];
    char recieve_buff[MSG_SIZE];
    while(1){
        memset(send_buff,'0',MSG_SIZE);
        memset(recieve_buff,'0',MSG_SIZE);

        numb_read=read(client_fd,recieve_buff,MSG_SIZE);
        if(numb_read<0){
            handle_error("read()");
        }else if(strncmp(recieve_buff,"exit",4)==0){
            system("clear)");
            break;
        }

        printf("\nMessage from client : %s\n",recieve_buff);

        printf("Reponse to client: ");
        fflush(stdin);
        fgets(send_buff,MSG_SIZE,stdin);
        numb_write=write(client_fd,send_buff,MSG_SIZE);
        if(numb_write<0){
            handle_error("write()");
        }else if(strncmp(send_buff,"exit",4)==0){
            system("clear)");
            break;
        }
    }
    close(client_fd);
}


int main(int argc,const char *argv[])
{
    int port_no,len,opt;
    int server_fd,client_fd;
    struct sockaddr_in server_addr,client_addr;

    memset(&server_addr,0,sizeof(server_addr));
    memset(&client_addr,0,sizeof(client_addr));

    if(argc<2){
        printf("Please run with command : ./server <port_number>\n");
        exit(EXIT_FAILURE);
    }else{
        port_no=atoi(argv[1]);
    }

    //Tạo socket
    server_fd=socket(AF_INET,SOCK_STREAM,0);
    if(server_fd<0){
        handle_error("socket()");
    }

    if(setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT,&opt,sizeof(opt))==-1){
        handle_error("setsockopt()");
    }

    //Define server address
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(port_no);
    server_addr.sin_addr.s_addr=INADDR_ANY;

    //Bind()
    if(bind(server_fd,(struct sockaddr *)&server_addr,sizeof(server_addr))==-1){
        handle_error("bind()");
    }

    //Listen()
    if(listen(server_fd,LISTEN_BACKLOG)==-1){
        handle_error("listen()");
    }
    len=sizeof(client_addr);
    while(1){
        if((client_fd=accept(server_fd,(struct sockaddr*)&client_addr,&len))==-1){
            handle_error("accept()");
        }
        system("clear");
        printf("Server got connect!\n");
        chat_func(client_fd);
    }
    return 0;
}