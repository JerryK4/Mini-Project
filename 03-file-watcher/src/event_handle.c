#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/inotify.h>
#include"../inc/logger.h"

#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024*(EVENT_SIZE+16))

void *event_handler(void *arg){
    int fd=*((int *)arg);
    char buffer[EVENT_BUF_LEN];

    while(1) {
        //Đọc sự kiện diễn ra và ghi vào buffer đồng thời lấy kích thước dữ liệu mà inotify trả về 
        int length = read(fd,buffer,EVENT_BUF_LEN);
        if(length<0){
            perror("Failed to read events");
            exit(EXIT_FAILURE);
        }

        int i=0;
        //Vòng lặp này sẽ tiếp tục cho đến khi i nhỏ hơn length ,nó sẽ lặp qua tất cả các sự kiện trong buffer mà inotify đọc được
        while(i<length) {
            //Chuyển đổi địa chỉ của buffer tại chỉ số i thành một con trỏ đến kiểu struct inotify_event
            struct inotify_event *event=(struct inotify_event *)&buffer[i];

            if(event->len) {
                //Tạo một mảng ký tự để lưu trữ mô tả của sự kiện
                char event_desc[1024];
                //Tiến hành kiểm tra các cờ(flags) tương ứng có được thiết lập sự event->mask sau đó lưu mô tả sự kiện vào event_desc
                if (event->mask & IN_CREATE) {
                    snprintf(event_desc, sizeof(event_desc), "Event: created on %s", event->name);
                } else if (event->mask & IN_MODIFY) {
                    snprintf(event_desc, sizeof(event_desc), "Event: modified on %s", event->name);
                } else if (event->mask & IN_DELETE) {
                    snprintf(event_desc, sizeof(event_desc), "Event: deleted on %s", event->name);
                }

                //Tạo luồng mới để xử lý sự kiện 
                pthread_t event_thread;
                char *event_copy=strdup(event_desc);//Tạo một bản sao của mô tả sự kiện để tránh bị ghi đè bởi các sự kiện tiếp theo
                pthread_create(&event_thread,NULL,handle_single_event,event_copy);
                pthread_detach(event_thread);
            }
            i+=EVENT_SIZE+event->len;
        }
    }
    return NULL;
}