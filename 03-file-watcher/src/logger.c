#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<pthread.h>

//Ghi lại sự kiện vào file log
void log_event(const char *event_desc){
    FILE *log_file = fopen("event_log.txt","a");
    if(!log_file){
        perror("Failed to open log file!");
        exit(EXIT_FAILURE);
    }

    time_t now = time(NULL);
    char *time_str = ctime(&now);
    time_str[strlen(time_str)-1]='\0'; //Loại bỏ ký tự '\n'
    fprintf(log_file,"[%s] %s\n",time_str,event_desc);
    fclose(log_file);
}
//Hàm xử lý các sự kiện trong luồng 
void *handle_single_event(void *arg){
    char *event_desc = (char *)arg;
    printf("Handling event: %s\n",event_desc);
    log_event(event_desc);
    free(arg);
    return NULL;
}
