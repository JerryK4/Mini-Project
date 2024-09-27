// #include<stdio.h>
// #include<stdlib.h>
// #include<string.h>
// #include<fcntl.h>
// #include<unistd.h>
// #include<errno.h>

// #define handle_error(msg) \
//     do{perror(msg);exit(EXIT_FAILURE);} while(0)

// #define FIFO_NAME "./order_fifo"
// #define BUFF_SIZE 1024

// int main(int argc,const char *argv[])
// {
//     char buff_msg[BUFF_SIZE];
//     int fd,fifo_fd;

//     mkfifo(FIFO_NAME,0666);

//     fd = open("./orders.txt",O_RDONLY);
//     if(fd==-1){
//         handle_error("open() orders.txt");
//     }

//     fifo_fd = open(FIFO_NAME,O_WRONLY);
//     if(fifo_fd<0){
//         handle_error("open() fifo");
//     }

//     while(fgets(buff_msg, BUFF_SIZE, fd) != NULL){
//         printf("\nOrder Manager sending: %s",buff_msg);
//         size_t len = strnlen(buff_msg, BUFF_SIZE);
//         if (write(fifo_fd, buff_msg, len + 1) == -1) {  // Gửi cả ký tự '\0'
//             handle_error("write() fifo");
//         }
//         sleep(2);
//     }

//     close(fd);
//     close(fifo_fd);

//     return 0;
// }

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define FIFO_NAME "./order_fifo"
#define BUFF_SIZE 1024

int main(int argc, const char *argv[]) {
    char buff_msg[BUFF_SIZE];
    int fifo_fd;
    FILE *fd;

    // Tạo FIFO nếu chưa tồn tại
    if (mkfifo(FIFO_NAME, 0666) == -1) {
        if (errno != EEXIST) {
            handle_error("mkfifo() failed");
        }
    }

    // Mở file orders.txt chỉ để đọc
    fd = fopen("./orders.txt", "r");
    if (fd == NULL) {
        handle_error("open() orders.txt");
    }

    // Mở FIFO để ghi
    fifo_fd = open(FIFO_NAME, O_WRONLY);
    if (fifo_fd < 0) {
        handle_error("open() fifo");
    }

    //Đọc từng dòng từ file orders.txt và gửi vào FIFO
    while (fgets(buff_msg, sizeof(buff_msg), fd) != NULL) {
        printf("\nOrder Manager sending: %s", buff_msg);

        // Đảm bảo rằng buffer không bị tràn
        size_t len = strnlen(buff_msg, sizeof(buff_msg));
        if (write(fifo_fd, buff_msg, len) == -1) {  // Gửi đủ độ dài của chuỗi
            handle_error("write() fifo");
        }
        sleep(2);
        memset(buff_msg, 0, sizeof(buff_msg)); // Xóa buffer
    }

    fclose(fd);
    close(fifo_fd);

    return 0;
}
