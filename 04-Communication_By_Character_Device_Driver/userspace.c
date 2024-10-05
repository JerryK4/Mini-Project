#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

char write_buf[1024];
char read_buf[1024];

int main()
{
    int fd;
    char option;
    printf("***********************************\n");
    printf("*******>>Jerry K4 FROM UET<<*******\n");
    fd=open("/dev/m_device",O_RDWR);
    if(fd<0){
        printf("Cannot open device file!\n");
        return 0;
    }else{
        while(1) {
                printf("****Please Enter the Option******\n");
                printf("        1. Write               \n");
                printf("        2. Read                 \n");
                printf("        3. Exit                 \n");
                printf("*********************************\n");
                scanf(" %c", &option);
                printf("Your Option = %c\n", option);
                
                switch(option) {
                        case '1':
                                memset(write_buf, 0, 1024);
                                printf("Enter the string to write into driver :");
                                scanf("  %[^\t\n]s", write_buf);//Đọc tất cả các ký tự từ đầu vào cho đến khi gặp tab hoặc xuống dòng
                                printf("Data Writing ...");
                                write(fd, write_buf, strlen(write_buf)+1);
                                printf("Done!\n");
                                break;
                        case '2':
                                printf("Data Reading ...");
                                memset(read_buf, 0, 1024);
                                read(fd, read_buf, 1024);
                                printf("Done!\n\n");
                                printf("Data = %s\n\n", read_buf);
                                break;
                        case '3':
                                close(fd);
                                exit(1);
                                break;
                        default:
                                printf("Enter Valid option = %c\n",option);
                                break;
                }
        }
    }
    close(fd);
}