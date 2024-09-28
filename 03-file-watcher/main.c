#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/inotify.h>
#include <errno.h>
#include "./inc/event_handler.h"

int main() {
    //Khởi tạo 1 inotify
    int fd = inotify_init();
    if (fd < 0) {
        perror("inotify_init() failed");
        exit(EXIT_FAILURE);
    }

    // Thêm thư mục để theo dõi
    int wd = inotify_add_watch(fd, "./watched_dir", IN_CREATE | IN_MODIFY | IN_DELETE);
    if (wd == -1) {
        perror("Failed to add watch");
        exit(EXIT_FAILURE);
    }

    printf("Watching for events in: ./watched_dir\n");

    // Tạo thread xử lý sự kiện
    pthread_t event_thread;
    pthread_create(&event_thread, NULL, event_handler, &fd);

    // Đợi cho đến khi kết thúc
    pthread_join(event_thread, NULL);

    close(fd);
    return 0;
}